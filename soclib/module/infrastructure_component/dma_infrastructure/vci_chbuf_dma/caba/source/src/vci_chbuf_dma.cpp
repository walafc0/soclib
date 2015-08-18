/*
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6
 *         Alain Greiner <alain.greiner@lip6.fr> November 2010
 */

//////////////////////////////////////////////////////////////////////////////////
//  This component is a multi-channels DMA controller supporting chained buffers.
//  It can be used to move a stream from one set of chained buffers (SRC chbuf) 
//  to another set of chained buffers (DST chbuf), without involving software.
//
//  A "chbuf descriptor" is a circular array of "buffer descriptors". A buffer
//  descriptor contains the address of the buffer and the address of the
//  status (full or not). The buffer and the status have the same address
//  extension (bits[43:32]). Both addresses must be a multiple of 64 bytes.
//  Each buffer descriptor occupies 8 bytes (64 bits unsigned long long):
//  - The 12 MSB bits contain the common extension of the buffer address and
//    the buffer status address
//  - The 26 following bits contain the bits [31:6] of the buffer address
//  - The 26 LSB bits contain the bits [31:6] of the buffer status address
//
//  The status occupies 64 bytes but only the LSB bit of first byte contains
//  useful information: 0 if buffer empty / 1 if buffer full.
//
//  The buffer length must be the same for src_chbuf and dst_chbuf.
//  The "chbuf descriptor" base address must be a multiple of 4 bytes.
//
//  This DMA controller implements two "modes" to scan the SRC and DST chbufs:
//  - IN ORDER FIFO: The chained buffers are read and write in strict order,
//    with a polling policy to access both the expected SRC chbuf and the 
//    expected DST chbuf. The delay between two accesses is defined, for each
//    channel, by the CHBUF_PERIOD addressable register, and must be non zero.
//  - OUT OF ORDER: The first full SRC buffer found is read, the first empty
//    DST buffer found is written, with a round robin priority for the search.
//    This mode is activated when the CHBUF_PERIOD value is zero (default value).
//  
//  This component supports both 32 bits and 64 bits VCI RDATA & WDATA fields.
//
//  
//  To improve the throuput, this components supports pipelined bursts.
//  The number of channels, the max burst size and the max number of pipelined
//  bursts that can be parallelised are constructor parameters. 
//
//  The chbuf descriptor address (CHBUF_DESC), and the number of chained 
//  buffers (CHBUF_NBUFS), as well as the elementary buffer size (BUF_SIZE)
//  are software parameters that must be  written in addressable registers 
//  when launching a transfer between two chbufs.
//
//  - The number of channels (m_channels) cannot be larger than 4.
//  - The number of pipelined bursts (m_bursts) cannot be larger than 4.
//  - The burst length (m_burst_length) must be a power of 2 no larger than 64.
//  - The buffer size must be multiple of (m_burst_length * m_bursts).
//  - The buffer base address mut be multiple of  m_burst_length.
//  - The status base address must be multiple of m_burst_length.
//
//  In order to support various protection mechanisms, for each channel,
//  the channel addressable registers takes 4K bytes in the address space. 
//  Only 8 address bits are decoded .
//  - The 5 bits ADDRESS[4:Ø] define the target register (see chbuf_dma.h)
//  - The 3 bits ADDRESS[14:12] define the selected channel.
//
//  For each channel, the relevant values for the channel status are:
//  - CHANNEL_IDLE             : 0   / channel not running
//  - CHANNEL_SRC_DESC_ERROR   : 1   / bus error accessing SRC CHBUF descriptor
//  - CHANNEL_DST_DESC_ERROR   : 2   / bus error accessing DST CHBUF descriptor
//  - CHANNEL_SRC_STATUS_ERROR : 3   / bus error accessing SRC BUF status
//  - CHANNEL_DST_STATUS_ERROR : 4   / bus error accessing DST BUF status
//  - CHANNEL_DATA_ERROR       : 5   / bus error accessing SRC or DST CHBUF data
//  - CHANNEL_BUSY             : > 5 / channel running
// 
//  There is one private IRQ line for each channel, that is only used
//  for bus error signaling, and is activated when channel[k] enters
//  an error state. The channel can be reset by writing a nul value
//  in register CHBUF_RUN[k], focing channel[k] to IDLE state.
//
//  In order to support multiple simultaneous transactions, the channel
//  and sub-channel indexes are transmited in the VCI TRDID field.    
//////////////////////////////////////////////////////////////////////////////////
//  Implementation note:
// 
//  The tgt_fsm controls the configuration commands and responses.
//  The cmd_fsm controls the read and write commands on the VCI initiator port. 
//  The rsp_fsm controls the read and write responses on the VCI initiator port.
//  Each channel [k] is controled by an independant channel_fsm.
///////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <cassert>

#include "alloc_elems.h"
#include "vci_chbuf_dma.h"
#include "chbuf_dma.h"

namespace soclib { namespace caba {

#define tmpl(t) template<typename vci_param> t VciChbufDma<vci_param>

/////////////////////////
tmpl(void)::transition()
{
    if (!p_resetn) 
    {
        m_cycles     = 0;
        r_tgt_fsm    = TGT_IDLE;
        r_cmd_fsm    = CMD_IDLE;
        r_rsp_fsm    = RSP_IDLE;
        r_cmd_channel  = 0;
        r_rsp_channel  = 0;
        for ( uint32_t k = 0 ; k < m_channels ; k++ )
        {
            r_channel_fsm[k] 	   = CHANNEL_IDLE;
            r_channel_period[k]    = 1000;
            r_channel_run[k]	   = false;
            for ( uint32_t b = 0 ; b < m_bursts ; b++ )
            { 
                r_channel_vci_req[k][b]  = false;   
                r_channel_vci_rsp[k][b]  = false;
            }
        }
        return;
    }

    // update cycle counter
    m_cycles++;

    ///////////////////////////////////////////////////////////////////////////////
    // This TGT_FSM controls the VCI TARGET port
    // It access the following registers:
    //  - r_channel_run[k]	         channel active => transfer requested    (W)
    //  - r_channel_buf_size[k]      buffer size (bytes) for both SRC & DST  (R/W)
    //  - r_channel_src_desc[k]      source chbuf descriptor paddr           (R/W)
    //  - r_channel_src_nbufs[k]     number of buffers in source chbuf       (R/W)
    //  - r_channel_dst_desc[k]      dest chbuf descriptor paddr             (R/W)
    //  - r_channel_dst_nbufs[k]     number of buffers in dest chbuf         (R/W)
    //  - r_channel_period[k]        status polling period                   (R/W)
    ///////////////////////////////////////////////////////////////////////////////

    switch(r_tgt_fsm.read()) 
    {
        case TGT_IDLE:
        {
            if (p_vci_target.cmdval.read() )
            {
                typename vci_param::fast_addr_t	address = p_vci_target.address.read();
                typename vci_param::cmd_t	    cmd     = p_vci_target.cmd.read();

                bool found = false;
                std::list<soclib::common::Segment>::iterator seg;
                for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ ) 
                {
                    if ( seg->contains(address) ) found = true;
                }

                assert ( found  and
                "ERROR in VCI_CHBUF_DMA : VCI address is out of segment");
               
                r_tgt_srcid	= p_vci_target.srcid.read();
                r_tgt_trdid	= p_vci_target.trdid.read();
                r_tgt_pktid	= p_vci_target.pktid.read();
                
                int 	  cell = (int)((address & 0x3C) >> 2);
                uint32_t  k    = (uint32_t)((address & 0x7000) >> 12);

                assert( (k < m_channels) and 
                "VCI_CHBUF_DMA error : The channel index (ADDR[14:12] is too large");

                assert( p_vci_target.eop.read() and
                "VCI_CHBUF_DMA error : A configuration request must be one single flit");

                // get write data value for both 32 bits and 64 bits data width
                unsigned int wdata;
                if( (vci_param::B == 8) and (p_vci_target.be.read() == 0xF0) ) 
                    wdata = (uint32_t)(p_vci_target.wdata.read()>>32);
                else
                    wdata = p_vci_target.wdata.read();

                //////////////////////////////////////////////////////////
	            if ( (cell == CHBUF_RUN) and (cmd == vci_param::CMD_WRITE) )
                {
                    r_channel_run[k] = (wdata != 0);
                    r_tgt_fsm        = TGT_WRITE;
                }
                /////////////////////////////////////////////////////////////////
	            else if ( (cell == CHBUF_STATUS) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = r_channel_fsm[k].read();
                    r_tgt_fsm   = TGT_READ;
                }
                ////////////////////////////////////////////////////////////////////
	            else if ( (cell == CHBUF_SRC_DESC) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( (not r_channel_run[k].read()) and
                    "VCI_CHBUF_DMA error : Configuration request for an active channel");

                    assert( (wdata%4 == 0) and
                    "VCI_CHBUF_DMA error : SRC descriptor address not multiple of 4");

                    r_channel_src_desc[k]  = wdata;
                    r_channel_src_index[k] = 0;
                    r_tgt_fsm              = TGT_WRITE;
                }
                ///////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_SRC_EXT) and (cmd == vci_param::CMD_WRITE) )
                {               
                    assert( (not r_channel_run[k].read()) and 
                    "VCI_CHBUF_DMA error : Configuration request for an active channel");
                   
                    r_channel_src_desc[k] = (r_channel_src_desc[k].read() & 0XFFFFFFFF) + 
                                      ((uint64_t)wdata << 32) ;
                    r_tgt_fsm             = TGT_WRITE;
                }
                ///////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_SRC_DESC) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = (uint32_t)r_channel_src_desc[k].read();
                    r_tgt_fsm   = TGT_READ;
                }
                ///////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_SRC_EXT) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = (uint32_t)(r_channel_src_desc[k].read()>>32);
                    r_tgt_fsm   = TGT_READ;
                }
                ////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_DST_DESC) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( (not r_channel_run[k].read()) and
                    "VCI_CHBUF_DMA error : Configuration request for an active channel");

                    assert( (wdata%4 == 0) and
                    "VCI_CHBUF_DMA error : DST descriptor address not multiple of 4");

                    r_channel_dst_desc[k]  = wdata;
                    r_channel_dst_index[k] = 0;
                    r_tgt_fsm              = TGT_WRITE;
                }
                ///////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_DST_EXT) and (cmd == vci_param::CMD_WRITE) )
                {               
                    assert( (not r_channel_run[k].read()) and 
                    "VCI_CHBUF_DMA error : Configuration request for an active channel");
                   
                    r_channel_dst_desc[k] = (r_channel_dst_desc[k].read() & 0XFFFFFFFF) + 
                                      ((typename vci_param::fast_addr_t)wdata << 32);
                    r_tgt_fsm             = TGT_WRITE;
                }
                ///////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_DST_DESC) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = r_channel_dst_desc[k].read();
                    r_tgt_fsm   = TGT_READ;
                }
                ///////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_DST_EXT) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = (uint32_t)(r_channel_dst_desc[k].read()>>32);
                    r_tgt_fsm   = TGT_READ;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_BUF_SIZE) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( not r_channel_run[k].read() and
                    "VCI_CHBUF_DMA error : Configuration request for an active channel");

                    assert( (wdata%(m_burst_length*m_bursts) == 0) and
                    "VCI_CHBUF_DMA error : buffer size not multiple of burst_length*bursts");

                    r_channel_buf_size[k] = wdata;
                    r_tgt_fsm             = TGT_WRITE;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_BUF_SIZE) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = r_channel_buf_size[k].read();
                    r_tgt_fsm   = TGT_READ;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_SRC_NBUFS) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( not r_channel_run[k].read() and
                    "VCI_CHBUF_DMA error : Configuration request for an active channel");

                    r_channel_src_nbufs[k] = wdata;
                    r_tgt_fsm              = TGT_WRITE;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_SRC_NBUFS) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = r_channel_src_nbufs[k].read();
                    r_tgt_fsm   = TGT_READ;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_DST_NBUFS) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( not r_channel_run[k].read() and
                    "VCI_CHBUF_DMA error : Configuration request for an active channel");

                    r_channel_dst_nbufs[k] = wdata;
                    r_tgt_fsm              = TGT_WRITE;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_DST_NBUFS) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = r_channel_dst_nbufs[k].read();
                    r_tgt_fsm   = TGT_READ;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_PERIOD) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( not r_channel_run[k].read() and
                    "VCI_CHBUF_DMA error : Configuration request for an active channel");

                    r_channel_period[k] = wdata;
                    r_tgt_fsm           = TGT_WRITE;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_PERIOD) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = r_channel_period[k].read();
                    r_tgt_fsm   = TGT_READ;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_SRC_INDEX) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = r_channel_src_index[k].read();
                    r_tgt_fsm   = TGT_READ;
                }
                /////////////////////////////////////////////////////////////////////
                else if ( (cell == CHBUF_DST_INDEX) and (cmd == vci_param::CMD_READ) )
                {
                    r_tgt_rdata = r_channel_dst_index[k].read();
                    r_tgt_fsm   = TGT_READ;
                }
                /////
                else
                {
                    r_tgt_fsm = TGT_ERROR;
                }
            }
            break;
        }
        case TGT_WRITE:
        case TGT_READ:
        case TGT_ERROR:
        {
            if ( p_vci_target.rspack.read() )
            {
                r_tgt_fsm = TGT_IDLE;
            }
            break;
        }
    } // end switch tgt_fsm

    ///////////////////////////////////////////////////////////////////////
    // These CHANNEL_FSM define the transfer state for each channel.
    // Each channel FSM implements two nested loops:
    //
    // - In external loop, we get the SRC buffer address, and the
    //   DST buffer address from the chbufs descriptors, we transfer
    //   a full buffer (internal loop), and release SRC & DST buffers.
    // - In the internal loop, m_bursts bursts of size m_burst_length
    //   are pipelineded per iteration. The storage capacity for each
    //   channel is m_bursts * m_burst_length. The m_bursts read
    //   requests are sent successively on the m_bursts sub-channels,
    //   before the m_bursts write requests.
    //
    // Each channel FSM set the r_channel_vci_req[k][b] registers, and
    // the r_channel_vci_req_type[k][b] to request a VCI transaction 
    // to the CMD FSM. The CMD FSM uses the request type to build the 
    // VCI command. The channel index [k] and sub-channel index [b]
    // are transported in the VCI TRDID. The R/W type is transported
    // in the VCI PKTID field.
    //
    // The RSP FSM analyses the TRDID & PKTID fields, and the request type
    // to write the data in the relevant register or buffer. It set the 
    // r_channel_vci_rsp[k][b] and the R/W type and error bit to signal
    // the VCI transaction completion.
    ////////////////////////////////////////////////////////////////////////

    for ( uint32_t k=0 ; k<m_channels ; k++ )
    {
        switch( r_channel_fsm[k].read() )
        {
            //////////////////
            case CHANNEL_IDLE:
            {
                if ( r_channel_run[k] ) r_channel_fsm[k] = CHANNEL_READ_SRC_DESC;
                break;
            }

            // read the buffer descriptor and get SRC buffer and status base addresses

           /////////////////////////////
            case CHANNEL_READ_SRC_DESC:   // request VCI READ for SRC buffer descriptor 
                                          // on sub-channel 0
            {
                r_channel_vci_req[k][0]      = true;
                r_channel_vci_req_type[k][0] = REQ_READ_SRC_DESC;
                r_channel_fsm[k]             = CHANNEL_READ_SRC_DESC_WAIT;
                break;
            }
            ///////////////////////////////////
            case CHANNEL_READ_SRC_DESC_WAIT:  // wait response for SRC buffer descriptor
                                              // on sub-channel 0
            {
                if ( r_channel_vci_rsp[k][0].read() )
                {
                    if ( r_channel_vci_rsp_err[k][0].read() )   
                    {
                        std::cout << "SRC_DESC_ERROR in VCI_CHBUF_DMA for channel "
                        << std::dec << k << " at cycle " << m_cycles << std::endl;
                        r_channel_fsm[k] = CHANNEL_SRC_DESC_ERROR;
                        break;
                    }
                    r_channel_vci_rsp[k][0] = false;
                    r_channel_fsm[k]        = CHANNEL_READ_SRC_STATUS;
                }
                break;
            }
            //////////////////////////////
            case CHANNEL_READ_SRC_STATUS:   // request VCI READ for SRC buffer status
            {
                r_channel_vci_req[k][0]      = true;
                r_channel_vci_req_type[k][0] = REQ_READ_SRC_STATUS;
                r_channel_fsm[k]             = CHANNEL_READ_SRC_STATUS_WAIT;
                break;
            }
            //////////////////////////////////
            case CHANNEL_READ_SRC_STATUS_WAIT:  // wait response for SRC buffer status
            {
                if ( r_channel_vci_rsp[k][0].read() ) 
                {
                    if ( r_channel_vci_rsp_err[k][0].read() )   
                    {
                        std::cout << "SRC_STATUS_ERROR in VCI_CHBUF_DMA for channel "
                        << std::dec << k << " at cycle " << m_cycles << std::endl;
                        r_channel_fsm[k] = CHANNEL_SRC_STATUS_ERROR;
                        break;
                    }
                    r_channel_vci_rsp[k][0] = false;
                    if ( not r_channel_src_full[k].read() ) // buffer not full: failure
                    {
                        if ( r_channel_period[k] )          // in order mode
                        { 
                            r_channel_fsm[k]   = CHANNEL_READ_SRC_STATUS_DELAY;
                            r_channel_timer[k] = r_channel_period[k];
                        }
                        else                                // no order mode
                        {
                            // increment SRC buffer index
                            if ( r_channel_src_index[k].read() == 
                                 (r_channel_src_nbufs[k].read()-1) )
                            {
                                r_channel_src_index[k] = 0;
                            }
                            else
                            {
                                r_channel_src_index[k] = r_channel_src_index[k].read()+1;
                            }
                            r_channel_fsm[k]   = CHANNEL_READ_SRC_DESC;
                        }
                    }
                    else                                    // buffer full: success
                    {
                        r_channel_fsm[k] = CHANNEL_READ_DST_DESC;
                    }
                }
                break;
            }
            ///////////////////////////////////
            case CHANNEL_READ_SRC_STATUS_DELAY:  // delay to access SRC buffer status
            {
                if ( r_channel_timer[k].read() == 0 ) 
                {
                    r_channel_fsm[k]   = CHANNEL_READ_SRC_STATUS;
                }
                else
                {
                    r_channel_timer[k] = r_channel_timer[k].read() - 1;
                }
                break;
            }

            // get DST buffer base address from DST chbuf descriptor

           /////////////////////////////
            case CHANNEL_READ_DST_DESC:   // request VCI READ for DST buffer descriptor 
            {
                r_channel_vci_req[k][0]      = true;
                r_channel_vci_req_type[k][0] = REQ_READ_DST_DESC;
                r_channel_fsm[k]             = CHANNEL_READ_DST_DESC_WAIT;
                break;
            }
            ///////////////////////////////////
            case CHANNEL_READ_DST_DESC_WAIT:  // wait response for DST buffer descriptor
            {
                if ( r_channel_vci_rsp[k][0].read() )
                {
                    if ( r_channel_vci_rsp_err[k][0].read() )   
                    {
                        std::cout << "DST_DESC_ERROR in VCI_CHBUF_DMA for channel "
                        << std::dec << k << " at cycle " << m_cycles << std::endl;
                        r_channel_fsm[k] = CHANNEL_DST_DESC_ERROR;
                        break;
                    }
                    r_channel_vci_rsp[k][0] = false;
                    r_channel_fsm[k]        = CHANNEL_READ_DST_STATUS;
                }
                break;
            }
            //////////////////////////////
            case CHANNEL_READ_DST_STATUS:   // request VCI READ for DST buffer status
            {
                r_channel_vci_req[k][0]      = true;
                r_channel_vci_req_type[k][0] = REQ_READ_DST_STATUS;
                r_channel_fsm[k]             = CHANNEL_READ_DST_STATUS_WAIT;
                break;
            }
            //////////////////////////////////
            case CHANNEL_READ_DST_STATUS_WAIT:  // wait response for DST buffer status
            {
                if ( r_channel_vci_rsp[k][0].read() ) 
                {
                    if ( r_channel_vci_rsp_err[k][0].read() )   
                    {
                        std::cout << "DST_STATUS_ERROR in VCI_CHBUF_DMA for channel "
                        << std::dec << k << " at cycle " << m_cycles << std::endl;
                        r_channel_fsm[k] = CHANNEL_DST_STATUS_ERROR;
                        break;
                    }
                    r_channel_vci_rsp[k][0] = false;
                    if ( r_channel_dst_full[k].read() ) // buffer full: failure
                    {
                        if ( r_channel_period[k] )          // in order mode
                        { 
                            r_channel_fsm[k]   = CHANNEL_READ_DST_STATUS_DELAY;
                            r_channel_timer[k] = r_channel_period[k];
                        }
                        else                                // no order mode
                        {
                            // increment DST buffer index
                            if ( r_channel_dst_index[k].read() == 
                                 (r_channel_dst_nbufs[k].read()-1) )
                            {
                                r_channel_dst_index[k] = 0;
                            }
                            else
                            {
                                r_channel_dst_index[k] = r_channel_dst_index[k].read()+1;
                            }
                            r_channel_fsm[k]   = CHANNEL_READ_DST_DESC;
                        }
                    }
                    else                                    // buffer not full: enter data loop
                    {
                        r_channel_todo_bytes[k]  = r_channel_buf_size[k].read();
                        r_channel_data_error[k]  = false;
                        r_channel_read_count[k]  = 0; 
                        r_channel_write_count[k] = 0; 
                        r_channel_fsm[k]         = CHANNEL_DATA_READ_REQ;
                    }
                }
                break;
            }
            ///////////////////////////////////
            case CHANNEL_READ_DST_STATUS_DELAY:  // delay to access DST buffer status
            {
                if ( r_channel_timer[k].read() == 0 ) 
                {
                    r_channel_fsm[k]   = CHANNEL_READ_DST_STATUS;
                }
                else
                {
                    r_channel_timer[k] = r_channel_timer[k].read() - 1;
                }
                break;
            }

            // move data from SRC buffer to DST buffer (internal loo)
            // in each iteration (2 * m_bursts) pipelined transactions
            // are running in parallel (one READ + one WRITE per sub-channel)

            ///////////////////////////
            case CHANNEL_DATA_READ_REQ:	 // request m_bursts pipelined READ transactions 
                                         // send at most one request per cycle
                                         // use r_channel_read_count for READ commands
            {
                uint32_t b   = r_channel_read_count[k].read();

                // set CMD READ request for (k,b)
                r_channel_vci_req[k][b]      = true;
                r_channel_vci_req_type[k][b] = REQ_READ_DATA;

                // test if last READ command sent
                if ( b >= m_bursts - 1 )
                {
                    r_channel_fsm[k]        = CHANNEL_DATA_WRITE_REQ;
                    r_channel_read_count[k] = 0;
                }
                else
                {
                    r_channel_read_count[k] = b + 1;
                }
                break;
            }
            ////////////////////////////
            case CHANNEL_DATA_WRITE_REQ:  // wait the responses to READ transaction
                                          // request WRITE transaction when response received
                                          // send at most one request per cycle
                                          // use r_channel_read_count for READ responses
                                          // use r_channel_write_count for WRITE responses
            {
                // scan all sub-channels for READ or WRITE responses
                for ( uint32_t b = 0 ; b < m_bursts ; b++ )
                {
                    if ( r_channel_vci_rsp[k][b].read() ) // response received for (k,b)
                    {
                        // reset RSP for (k,b)
                        r_channel_vci_rsp[k][b] = false;
   
                        // register error reported for (k,b)
                        if ( r_channel_vci_rsp_err[k][b].read() ) 
                        {
                            std::cout << "DATA_READ_ERROR in VCI_CHBUF_DMA for channel "
                            << std::dec << k << " at cycle " << m_cycles << std::endl;
                            r_channel_data_error[k] = true;
                        }

                        // handle response for (k,b)
                        if ( r_channel_vci_rsp_read[k][b] )  // read response
                        {
                            // test if last READ response received
                            if ( r_channel_read_count[k].read() >= m_bursts - 1 )
                            {
                                r_channel_fsm[k]        = CHANNEL_DATA_WRITE_WAIT;
                                r_channel_read_count[k] = 0; 
                            }
                            else
                            {
                                r_channel_read_count[k]  = r_channel_read_count[k].read() + 1;
                            }

                            // send CMD write for (k,b)
                            r_channel_vci_req[k][b]      = true;
                            r_channel_vci_req_type[k][b] = REQ_WRITE_DATA;
                        }
                        else                                 // write response
                        {
                            // register response 
                            r_channel_write_count[k]  = r_channel_write_count[k].read() + 1;
                        }
                    }
                }
                break;
            }
            /////////////////////////////
            case CHANNEL_DATA_WRITE_WAIT: // wait completion of all WRITE transactions 
                                          // handle at most one response per cycle
                                          // use r_channel_write_count for WRITE responses
            {
                // scan all sub-channels for WRITE responses (READ response not expected)
                for ( uint32_t b = 0 ; b < m_bursts ; b++ )
                {
                    if ( r_channel_vci_rsp[k][b].read() ) // response received
                    {
                        assert( (r_channel_vci_rsp_read[k][b] == false) and
                        "VCI_CHBUF_DMA error : unexpected READ response");

                        // reset RSP for (k,b)
                        r_channel_vci_rsp[k][b] = false;
   
                        // register error reported for (k,b)
                        if ( r_channel_vci_rsp_err[k][b].read() )      
                        {
                            std::cout << "DATA_WRITE_ERROR in VCI_CHBUF_DMA for channel "
                            << std::dec << k << " at cycle " << m_cycles << std::endl;
                            r_channel_data_error[k] = true;
                        }

                        // test if last WRITE response received
                        if ( r_channel_write_count[k].read() >= m_bursts - 1 )
                        {
                            r_channel_todo_bytes[k]  = r_channel_todo_bytes[k].read() 
                                                       - (m_burst_length*m_bursts);
                            r_channel_write_count[k] = 0;
                            r_channel_fsm[k]         = CHANNEL_DATA_END;
                        }
                        else
                        {
                            r_channel_write_count[k] = r_channel_write_count[k].read() + 1;
                        }
                    }
                }
                break;
            }
            //////////////////////
            case CHANNEL_DATA_END:  // check error for the m_bursts transfers
                                    // check if the buffer is completed
            {
                if      ( r_channel_data_error[k].read() )        // error reported
                {
                    r_channel_fsm[k] = CHANNEL_DATA_ERROR;
                }
                else if ( r_channel_todo_bytes[k].read() == 0 )   // buffer completed
                {
                    r_channel_fsm[k] = CHANNEL_SRC_STATUS_WRITE;
                }
                else                                              // next multi-burst
                {
                    r_channel_fsm[k] = CHANNEL_DATA_READ_REQ;
                }
                break;
            }

            // Release SRC & DST buffers and increment buffer indexes 

            /////////////////////////////
            case CHANNEL_SRC_STATUS_WRITE:  // request VCI transaction on sub-channel 0
            {
                r_channel_vci_req[k][0]      = true;
                r_channel_vci_req_type[k][0] = REQ_WRITE_SRC_STATUS;
                r_channel_fsm[k]             = CHANNEL_SRC_STATUS_WRITE_WAIT;
                break;
            }
            ///////////////////////////////////
            case CHANNEL_SRC_STATUS_WRITE_WAIT:
            {
                if ( r_channel_vci_rsp[k][0].read() )
                {
                    r_channel_vci_rsp[k][0] = false;

                    if ( r_channel_vci_rsp_err[k][0].read() ) 
                        r_channel_fsm[k] = CHANNEL_SRC_STATUS_ERROR;
                    else                      
                        r_channel_fsm[k] = CHANNEL_DST_STATUS_WRITE;
                }
                break;
            }
            //////////////////////////////
            case CHANNEL_DST_STATUS_WRITE:
            {
                r_channel_vci_req[k][0]      = true;
                r_channel_vci_req_type[k][0] = REQ_WRITE_DST_STATUS;
                r_channel_fsm[k]             = CHANNEL_DST_STATUS_WRITE_WAIT;
                break;
            }
            ///////////////////////////////////
            case CHANNEL_DST_STATUS_WRITE_WAIT:
            {
                if ( r_channel_vci_rsp[k][0].read() )
                {
                    r_channel_vci_rsp[k][0] = false;
                    if ( r_channel_vci_rsp_err[k][0].read() ) 
                        r_channel_fsm[k] = CHANNEL_DST_STATUS_ERROR;
                    else
                        r_channel_fsm[k] = CHANNEL_NEXT_BUFFERS;
                }
                break;
            }
            //////////////////////////
            case CHANNEL_NEXT_BUFFERS:  // update SRC & DST buffers index
            {
                if ( r_channel_src_index[k].read() == (r_channel_src_nbufs[k].read()-1) )
                {
                    r_channel_src_index[k] = 0;
                }
                else
                {
                    r_channel_src_index[k] = r_channel_src_index[k].read()+1;
                }

                if ( r_channel_dst_index[k].read() == (r_channel_dst_nbufs[k].read()-1) )
                {
                    r_channel_dst_index[k] = 0;
                }
                else
                {
                    r_channel_dst_index[k] = r_channel_dst_index[k].read()+1;
                }

                r_channel_fsm[k] = CHANNEL_IDLE;
                break;
            }

            ////////////////////////
            case CHANNEL_DATA_ERROR:
            case CHANNEL_SRC_DESC_ERROR:
            case CHANNEL_DST_DESC_ERROR:
            case CHANNEL_SRC_STATUS_ERROR:
            case CHANNEL_DST_STATUS_ERROR:
            {
                if ( not r_channel_run[k] )	r_channel_fsm[k] = CHANNEL_IDLE;
                break; 
            }
        }
    } // end switch r_channel_fsm[k]
        
                
    ////////////////////////////////////////////////////////////////////////////
    // This CMD_FSM controls the VCI INIT command port
    ////////////////////////////////////////////////////////////////////////////
    switch(r_cmd_fsm.read()) 
    {
        //////////////
        case CMD_IDLE:
        {
            // round-robin arbitration between channels to send a command
            bool not_found = true;

            // loop on channels
            for( uint32_t n = 0 ; (n < m_channels) and not_found ; n++ )
            {
                uint32_t k = (r_cmd_channel.read() + n) % m_channels;

                // loop on sub-channels
                for ( uint32_t b = 0 ; (b < m_bursts) and not_found ; b++ )
                {
                    if ( r_channel_vci_req[k][b].read() )  // pending request
                    {
                        not_found               = false;
                        r_cmd_channel           = k;
                        r_cmd_burst             = b;
                        r_cmd_count             = 0;
                        r_channel_vci_req[k][b] = false;

                        switch ( r_channel_vci_req_type[k][b].read() )
                        {
                            case REQ_READ_SRC_DESC:
                            {
                                r_cmd_address = r_channel_src_desc[k].read() + 
                                                (r_channel_src_index[k].read() << 3);
                                r_cmd_bytes   = 8;
                                r_cmd_fsm     = CMD_READ;
                                break;
                            }
                            case REQ_READ_DST_DESC:
                            {
                                r_cmd_address = r_channel_dst_desc[k].read() + 
                                                (r_channel_dst_index[k].read() << 3);
                                r_cmd_bytes   = 8;
                                r_cmd_fsm     = CMD_READ;
                                break;
                            }
                            case REQ_READ_SRC_STATUS:
                            {
                                r_cmd_address = ((uint64_t)r_channel_src_ext[k].read() << 32) +
                                                r_channel_src_sts_addr[k].read();
                                r_cmd_bytes   = 4;
                                r_cmd_fsm     = CMD_READ;
                                break;
                            }
                            case REQ_READ_DST_STATUS:
                            {
                                r_cmd_address = ((uint64_t)r_channel_dst_ext[k].read() << 32) +
                                                r_channel_dst_sts_addr[k].read();
                                r_cmd_bytes   = 4;
                                r_cmd_fsm     = CMD_READ;
                                break;
                            }
                            case REQ_READ_DATA:
                            {
                                r_cmd_address = ((uint64_t)r_channel_src_ext[k].read() << 32) +
                                                r_channel_src_buf_addr[k].read() +
                                                r_channel_buf_size[k].read() - 
                                                r_channel_todo_bytes[k].read() +
                                                b*m_burst_length;
                                r_cmd_bytes   = m_burst_length;
                                r_cmd_fsm     = CMD_READ;
                                break;
                            }
                            case REQ_WRITE_DATA:
                            {
                                r_cmd_address = ((uint64_t)r_channel_dst_ext[k].read() << 32) +
                                                r_channel_dst_buf_addr[k].read() +
                                                r_channel_buf_size[k].read() - 
                                                r_channel_todo_bytes[k].read() +
                                                b*m_burst_length;
                                r_cmd_bytes   = m_burst_length;
                                r_cmd_fsm     = CMD_WRITE;
                                break;
                            }
                            case REQ_WRITE_SRC_STATUS:
                            {
                                r_cmd_address = ((uint64_t)r_channel_src_ext[k].read() << 32) +
                                                r_channel_src_sts_addr[k].read();
                                r_cmd_bytes   = 4;
                                r_cmd_fsm     = CMD_WRITE;
                                break;
                            }
                            case REQ_WRITE_DST_STATUS:
                            {
                                r_cmd_address = ((uint64_t)r_channel_dst_ext[k].read() << 32) +
                                                r_channel_dst_sts_addr[k].read();
                                r_cmd_bytes   = 4;
                                r_cmd_fsm     = CMD_WRITE;
                                break;
                            }
                        } // end switch on req_type
                    } // end if pending request
                } // end for sub-channels
            } // end for channels
            break;
        }
        //////////////
        case CMD_READ:
        {
            if ( p_vci_initiator.cmdack.read() )
            {
                uint32_t k = r_cmd_channel.read();
                uint32_t b = r_cmd_burst.read();

                r_channel_vci_req[k][b] = false;
                r_cmd_fsm               = CMD_IDLE;
            }
            break;
        }
        ///////////////
        case CMD_WRITE:
        {
            if ( p_vci_initiator.cmdack.read() )
            {
                uint32_t k = r_cmd_channel.read();
                uint32_t b = r_cmd_burst.read();

                if (vci_param::B==4)  // Data 32 bits
                {
                    if ( r_cmd_count.read() == (r_cmd_bytes.read() - 4) )
                    {
                        r_channel_vci_req[k][b] = false;
                        r_cmd_fsm               = CMD_IDLE;
                    }
                    else        
                    {
                        r_cmd_count = r_cmd_count.read() + 4;
                    }
                }
                else                   // Data 64 bits
                {
                    if ( r_cmd_count.read() == (r_cmd_bytes.read() - 4) )
                    {
                        r_channel_vci_req[k][b] = false;
                        r_cmd_fsm = CMD_IDLE;
                    }
                    else
                    {
                        if ( r_cmd_count.read() == (r_cmd_bytes.read() - 8))
                        {
                            r_channel_vci_req[k][b] = false;
                            r_cmd_fsm               = CMD_IDLE; 
                        }
                        else
                        {
                            r_cmd_count = r_cmd_count.read() + 8;
                        }
                    }
                }
             }
            break;
        }
    } // end switch cmd_fsm

    ///////////////////////////////////////////////////////////////////////////
    // This RSP_FSM controls the VCI INIT response port.
    // It get the channel and sub-channel indexes [k][b] in the VCI TRDID,
    // and writes into the relevant register or buffer if required.
    // It set r_channel_vci_rsp[k][b], r_channel_vci_rsp_err[k][b], and
    // r_channel_vci_rsp_read[k][b] to signal completion of the transaction.
    ///////////////////////////////////////////////////////////////////////////
    switch(r_rsp_fsm.read()) 
    {
        //////////////
        case RSP_IDLE:
        {
            if ( p_vci_initiator.rspval.read() )
            {
                uint32_t k = (uint32_t)((p_vci_initiator.rtrdid.read()>>2) & 0x3);
                uint32_t b = (uint32_t)(p_vci_initiator.rtrdid.read() & 0x3);

                r_rsp_channel = k;
                r_rsp_burst   = b;
                r_rsp_count   = 0;

                if      ( r_channel_vci_req_type[k][b].read() == REQ_READ_SRC_DESC )
                {
                    r_rsp_fsm = RSP_READ_SRC_DESC;
                }
                else if ( r_channel_vci_req_type[k][b].read() == REQ_READ_SRC_STATUS )
                {
                    r_rsp_fsm = RSP_READ_SRC_STATUS;
                }
                else if ( r_channel_vci_req_type[k][b].read() == REQ_READ_DST_DESC )
                {
                    r_rsp_fsm = RSP_READ_DST_DESC;
                }
                else if ( r_channel_vci_req_type[k][b].read() == REQ_READ_DST_STATUS )
                {
                    r_rsp_fsm = RSP_READ_DST_STATUS;
                }
                else if ( r_channel_vci_req_type[k][b].read() == REQ_READ_DATA )
                {
                    r_rsp_fsm = RSP_READ_DATA; 
                }
                else 
                {
                    r_rsp_fsm = RSP_WRITE;
                }
            }
            break;
        } 
        ///////////////////////
        case RSP_READ_SRC_DESC:  // set SRC status and buffer addresses
        {
            if ( p_vci_initiator.rspval.read() )
            {
                uint32_t k    = r_rsp_channel.read();

                if (vci_param::B==4)   // VCI DATA on 32 bits
                {
                    uint32_t rdata = (uint32_t)p_vci_initiator.rdata.read();

                    if ( r_rsp_count.read() == 0 ) // read bits[31:0] of buffer descriptor
                    {
                        // bits[31:26] of buffer descriptor are bits[11:6] of buffer address
                        r_channel_src_buf_addr[k] = (rdata & 0xFC000000) << 6;

                        // bits[25:0] of buffer descriptor are bits[31:6] of buffer status address
                        r_channel_src_sts_addr[k] = (rdata & 0x3FFFFFF) << 6;
                    }

                    else // read bits[63:32] of buffer descriptor
                    {
                        // bits[63:52] of buffer descriptor (<=> bits [31:24] of rdata) are
                        // address extension of buffer address and buffer status address
                        r_channel_src_ext[k] = (rdata & 0xFFF00000) >> 20;

                        // bits[51:32] of buffer descriptor (<=> bits [19:0] of rdata) are
                        // bits[31:12] of buffer address
                        r_channel_src_buf_addr[k] = (r_channel_src_buf_addr[k].read() & 0xFFF) +
                                                    ((rdata & 0xFFFFF) << 12);
                    }

                    r_rsp_count = r_rsp_count.read() + 4;
                }
                else                  // VCI DATA on 64 bits
                {
                    uint64_t rdata = (uint64_t)p_vci_initiator.rdata.read();

                    // bits[63:52] of buffer descriptor are address extension of buffer address
                    // and buffer status address
                    r_channel_src_ext[k] = (rdata & 0xFFF0000000000000ULL) >> 52;

                    // bits[51:26] of buffer descriptor are bits[31:6] of buffer address
                    r_channel_src_buf_addr[k] = (rdata & 0xFFFFFFC000000ULL) >> 20;

                    // bits[25:0] of buffer descriptor are bits[31:6] of buffer status address
                    r_channel_src_sts_addr[k] = (rdata & 0x3FFFFFFULL) << 6;
                }

                if ( p_vci_initiator.reop.read() )
                {
                    r_channel_vci_rsp[k][0]      = true;
                    r_channel_vci_rsp_err[k][0]  = ((p_vci_initiator.rerror.read()&0x1) != 0);
                    r_channel_vci_rsp_read[k][0] = true;
                    r_rsp_fsm = RSP_IDLE;
                } 
            }
            break;
        } 
        /////////////////////////
        case RSP_READ_SRC_STATUS:   // set SRC status
        {
            uint32_t k                   = r_rsp_channel.read();
            uint32_t rdata               = (uint32_t)p_vci_initiator.rdata.read();

            r_channel_src_full[k]        = ((rdata & 0x1) != 0);
            r_channel_vci_rsp[k][0]      = true;
            r_channel_vci_rsp_err[k][0]  = ((p_vci_initiator.rerror.read()&0x1) != 0);
            r_channel_vci_rsp_read[k][0] = true;
            r_rsp_fsm                    = RSP_IDLE;
            break;
        }
        ///////////////////////
        case RSP_READ_DST_DESC:  // set DST status and buffer address
        {
            if ( p_vci_initiator.rspval.read() )
            {
                uint32_t k    = r_rsp_channel.read();

                if(vci_param::B==4)   // VCI DATA on 32 bits
                {
                    uint32_t rdata = (uint32_t)p_vci_initiator.rdata.read();

                    if ( r_rsp_count.read() == 0 ) // read bits[31:0] of buffer descriptor
                    {
                        // bits[31:26] of buffer descriptor are bits[11:6] of buffer address
                        r_channel_dst_buf_addr[k] = (rdata & 0xFC000000) << 6;

                        // bits[25:0] of buffer descriptor are bits[31:6] of buffer status address
                        r_channel_dst_sts_addr[k] = (rdata & 0x3FFFFFF) << 6;
                    }

                    else // read bits[63:32] of buffer descriptor
                    {
                        // bits[63:52] of buffer descriptor (<=> bits [31:24] of rdata) are
                        // address extension of buffer address and buffer status address
                        r_channel_dst_ext[k] = (rdata & 0xFFF00000) >> 20;

                        // bits[51:32] of buffer descriptor (<=> bits [19:0] of rdata) are
                        // bits[31:12] of buffer address
                        r_channel_dst_buf_addr[k] = (r_channel_dst_buf_addr[k].read() & 0xFFF) +
                                                    ((rdata & 0xFFFFF) << 12);
                    }

                    r_rsp_count = r_rsp_count.read() + 4;
                }
                else                 // VCI DATA on 64 bits
                {
                    uint64_t rdata = (uint64_t)p_vci_initiator.rdata.read();

                    // bits[63:52] of buffer descriptor are address extension of buffer address
                    // and buffer status address
                    r_channel_dst_ext[k] = (rdata & 0xFFF0000000000000ULL) >> 52;

                    // bits[51:26] of buffer descriptor are bits[31:6] of buffer address
                    r_channel_dst_buf_addr[k] = (rdata & 0xFFFFFFC000000ULL) >> 20;

                    // bits[25:0] of buffer descriptor are bits[31:6] of buffer status address
                    r_channel_dst_sts_addr[k] = (rdata & 0x3FFFFFFULL) << 6;
                }

                if ( p_vci_initiator.reop.read() )
                {
                    r_channel_vci_rsp[k][0]      = true;
                    r_channel_vci_rsp_err[k][0]  = ((p_vci_initiator.rerror.read()&0x1) != 0);
                    r_channel_vci_rsp_read[k][0] = true;
                    r_rsp_fsm = RSP_IDLE;
                } 
            }
            break;
        } 
        /////////////////////////
        case RSP_READ_DST_STATUS:  // set DST status
        {
            uint32_t k                   = r_rsp_channel.read();
            uint32_t rdata               = (uint32_t)p_vci_initiator.rdata.read();

            r_channel_dst_full[k]        = ((rdata & 0x1) != 0);
            r_channel_vci_rsp[k][0]      = true;
            r_channel_vci_rsp_err[k][0]  = ((p_vci_initiator.rerror.read()&0x1) != 0);
            r_channel_vci_rsp_read[k][0] = true;
            r_rsp_fsm                    = RSP_IDLE;
            break;
        }
        ///////////////////
        case RSP_READ_DATA:
        {
            if ( p_vci_initiator.rspval.read() )
            {
                uint32_t k    = r_rsp_channel.read();    // channel index
                uint32_t b    = r_rsp_burst.read();      // sub-channel index
                uint32_t w    = r_rsp_count.read()>>2;   // word index
                
                if (vci_param::B==4)   // VCI DATA on 32 bits
                {
                     r_channel_vci_buf[k][b][w] = p_vci_initiator.rdata.read();
                     r_rsp_count                = r_rsp_count.read() + 4;
                }
                else                   // VCI DATA on 64 bits
                {
                     r_channel_vci_buf[k][b][w]   = (uint32_t)p_vci_initiator.rdata.read();
                     r_channel_vci_buf[k][b][w+1] = (uint32_t)(p_vci_initiator.rdata.read()>>32);
                     r_rsp_count                  = r_rsp_count.read() + 8;
                }

                if ( p_vci_initiator.reop.read() )
                {
                    r_channel_vci_rsp[k][b]      = true;
                    r_channel_vci_rsp_err[k][b]  = ((p_vci_initiator.rerror.read()&0x1) != 0);
                    r_channel_vci_rsp_read[k][b] = true;
                    r_rsp_fsm                    = RSP_IDLE;
                }
            }
            break; 
        }
        ///////////////
        case RSP_WRITE:
        {
            assert( (p_vci_initiator.reop.read() == true) and
             "VCI_CHBUF_DMA error : write response packed contains more than one flit");  

            uint32_t k = r_rsp_channel.read();
            uint32_t b = r_rsp_burst.read();

            r_channel_vci_rsp[k][b]      = true;
            r_channel_vci_rsp_err[k][b]  = ((p_vci_initiator.rerror.read()&0x1) != 0);
            r_channel_vci_rsp_read[k][b] = false;
            r_rsp_fsm                    = RSP_IDLE;
            break;
        }
    } // end switch rsp_fsm

} // end transition

//////////////////////
tmpl(void)::genMoore()
{
    /////// VCI INIT CMD ports //////
    switch( r_cmd_fsm.read() ) 
    {
        case CMD_IDLE:
        {   
            p_vci_initiator.cmdval  = false;
            p_vci_initiator.address = 0;
            p_vci_initiator.wdata   = 0;
            p_vci_initiator.be      = 0;
            p_vci_initiator.plen    = 0;
            p_vci_initiator.cmd     = vci_param::CMD_WRITE;
            p_vci_initiator.trdid   = 0;
            p_vci_initiator.pktid   = 0;
            p_vci_initiator.srcid   = 0;
            p_vci_initiator.cons    = false;
            p_vci_initiator.wrap    = false;
            p_vci_initiator.contig  = false;
            p_vci_initiator.clen    = 0;
            p_vci_initiator.cfixed  = false;
            p_vci_initiator.eop     = false;
            break;
        }
        case CMD_READ:
        {   
            uint32_t k    = r_cmd_channel.read();
            uint32_t b    = r_cmd_burst.read();
            uint32_t be;
            if ( (vci_param::B == 4) or                                     
                 (r_channel_vci_req_type[k][b] == REQ_READ_SRC_STATUS) or 
                 (r_channel_vci_req_type[k][b] == REQ_READ_DST_STATUS) or
                 (r_cmd_bytes.read() - r_cmd_count.read() == 4) )          be = 0x0F;
            else                                                           be = 0xFF;
            p_vci_initiator.cmdval  = true;
            p_vci_initiator.address = (typename vci_param::fast_addr_t)r_cmd_address.read();
            p_vci_initiator.wdata   = 0;
            p_vci_initiator.be      = (typename vci_param::be_t)be;      
            p_vci_initiator.plen    = (typename vci_param::plen_t)r_cmd_bytes.read();
            p_vci_initiator.cmd     = vci_param::CMD_READ;
            p_vci_initiator.trdid   = (typename vci_param::trdid_t)(((k & 0x3)<<2) | (b & 0x3));	
            p_vci_initiator.pktid   = 0;
            p_vci_initiator.srcid   = m_srcid;
            p_vci_initiator.cons    = false;
            p_vci_initiator.wrap    = false;
            p_vci_initiator.contig  = true;
            p_vci_initiator.clen    = 0;
            p_vci_initiator.cfixed  = false;
            p_vci_initiator.eop     = true;
            break;
        }
        case CMD_WRITE:
        {
            uint32_t k = r_cmd_channel.read();    // channel index
            uint32_t b = r_cmd_burst.read();      // sub-channel index
            uint32_t w = r_cmd_count.read()>>2;   // 32 bits word index

            if (vci_param::B ==4)     // Data width 32 bits
            {  
                if      ( r_channel_vci_req_type[k][b].read() == REQ_WRITE_SRC_STATUS ) 
                {
                    p_vci_initiator.wdata   = 0x0;
                    p_vci_initiator.be      = 0xF;
                    p_vci_initiator.eop     = true;
                }
                else if ( r_channel_vci_req_type[k][b].read() == REQ_WRITE_DST_STATUS ) 
                {
                    p_vci_initiator.wdata   = 0x1;
                    p_vci_initiator.be      = 0xF;
                    p_vci_initiator.eop     = true;
                }
                else   // REQ_WRITE_DATA
                {
                    p_vci_initiator.wdata   = r_channel_vci_buf[k][b][w];
                    p_vci_initiator.be      = 0xF;
                    p_vci_initiator.eop     = ( r_cmd_count.read() == r_cmd_bytes.read() - 4 );
                }
            }
            else   // Data width 64 bits
            {
                if      ( r_channel_vci_req_type[k][b].read() == REQ_WRITE_SRC_STATUS ) 
                {
                    p_vci_initiator.wdata   = 0x0;
                    p_vci_initiator.be      = 0x0F;
                    p_vci_initiator.eop     = true;
                }
                else if ( r_channel_vci_req_type[k][b].read() == REQ_WRITE_DST_STATUS ) 
                {
                    p_vci_initiator.wdata   = 0x1;
                    p_vci_initiator.be      = 0x0F;
                    p_vci_initiator.eop     = true;
                }
                else   // REQ_WRITE_DATA
                {
                    p_vci_initiator.wdata   = (uint64_t)r_channel_vci_buf[k][b][w]  | 
                                              (((uint64_t)r_channel_vci_buf[k][b][w+1])<<32);
                    p_vci_initiator.be      = 0xFF;
                    p_vci_initiator.eop = ( r_cmd_count.read() == r_cmd_bytes.read() - 8 );
                }
            }

            p_vci_initiator.cmdval  = true;
            p_vci_initiator.address = (typename vci_param::fast_addr_t)r_cmd_address.read() + 
                                          r_cmd_count.read();
            p_vci_initiator.plen    = r_cmd_bytes.read();
            p_vci_initiator.cmd     = vci_param::CMD_WRITE;
            p_vci_initiator.trdid   = (typename vci_param::trdid_t)(((k & 0x3)<<2) | (b & 0x3));
            p_vci_initiator.pktid   = 0x4;
            p_vci_initiator.srcid   = m_srcid;
            p_vci_initiator.cons    = false;
            p_vci_initiator.wrap    = false;
            p_vci_initiator.contig  = true;
            p_vci_initiator.clen    = 0;
            p_vci_initiator.cfixed  = false;
            break;
        }
    } // end switch cmd_fsm

    /////// VCI INIT RSP port ////// 
    if ( r_rsp_fsm.read() == RSP_IDLE ) p_vci_initiator.rspack = false;
    else                                p_vci_initiator.rspack = true;

    ////// VCI TARGET port /////// 
    switch( r_tgt_fsm.read() ) {
        case TGT_IDLE:
        {
            p_vci_target.cmdack = true;
            p_vci_target.rspval = false;
            p_vci_target.reop   = false;
            p_vci_target.rdata  = 0;
            break;
        }
        case TGT_WRITE:
        {
            p_vci_target.cmdack = false;
            p_vci_target.rspval = true;
            p_vci_target.rdata  = 0;
            p_vci_target.rerror = vci_param::ERR_NORMAL;
            p_vci_target.rsrcid = r_tgt_srcid.read();
            p_vci_target.rtrdid = r_tgt_trdid.read();
            p_vci_target.rpktid = r_tgt_pktid.read();
            p_vci_target.reop   = true;
            break;
        }
        case TGT_READ:
        {
            p_vci_target.cmdack = false;
            p_vci_target.rspval = true;
            p_vci_target.rdata  = r_tgt_rdata.read();
            p_vci_target.rerror = vci_param::ERR_NORMAL;
            p_vci_target.rsrcid = r_tgt_srcid.read();
            p_vci_target.rtrdid = r_tgt_trdid.read();
            p_vci_target.rpktid = r_tgt_pktid.read();
            p_vci_target.reop   = true;
            break;
        }
        case TGT_ERROR:
        {
            p_vci_target.cmdack = false;
            p_vci_target.rspval = true;
            p_vci_target.rdata  = 0;
            p_vci_target.rerror = vci_param::ERR_GENERAL_DATA_ERROR;
            p_vci_target.rsrcid = r_tgt_srcid.read();
            p_vci_target.rtrdid = r_tgt_trdid.read();
            p_vci_target.rpktid = r_tgt_pktid.read();
            p_vci_target.reop   = true;
            break;
        }
        default:
        {
            assert(false);
        }
    } // end switch rsp_fsm

    /////// IRQ ports //////////
    for ( uint32_t k = 0 ; k < m_channels ; k++ )
    {
	    p_irq[k] = (r_channel_fsm[k] == CHANNEL_SRC_DESC_ERROR)   || 
                   (r_channel_fsm[k] == CHANNEL_DST_DESC_ERROR)   ||
                   (r_channel_fsm[k] == CHANNEL_SRC_STATUS_ERROR) ||
                   (r_channel_fsm[k] == CHANNEL_DST_STATUS_ERROR) ||
                   (r_channel_fsm[k] == CHANNEL_DATA_ERROR);
    }
} // end genMoore

/////////////////////////
tmpl(void)::print_trace()
{
    const char* tgt_state_str[] = 
    {
        "  TGT_IDLE",
        "  TGT_READ",
        "  TGT_WRITE",
        "  TGT_ERROR"
    };
    const char* cmd_state_str[] = 
    {
        "  CMD_IDLE",
        "  CMD_READ",
        "  CMD_WRITE"
    };
    const char* rsp_state_str[] = 
    {
        "  RSP_IDLE",
        "  RSP_READ_SRC_DESC",
        "  RSP_READ_SRC_STATUS",
        "  RSP_READ_DST_DESC",
        "  RSP_READ_DST_STATUS",
        "  RSP_READ_DATA",
        "  RSP_WRITE"
    };
    const char* channel_state_str[] = 
    {
        " CHANNEL_IDLE",

        " CHANNEL_DATA_ERROR",
        " CHANNEL_SRC_DESC_ERROR",
        " CHANNEL_DST_DESC_ERROR",
        " CHANNEL_SRC_STATUS_ERROR",
        " CHANNEL_DST_STATUS_ERROR",

        " CHANNEL_READ_SRC_DESC",
        " CHANNEL_READ_SRC_DESC_WAIT",
        " CHANNEL_READ_SRC_STATUS",
        " CHANNEL_READ_SRC_STATUS_WAIT",
        " CHANNEL_READ_SRC_STATUS_DELAY",

        " CHANNEL_READ_DST_DESC",
        " CHANNEL_READ_DST_DESC_WAIT",
        " CHANNEL_READ_DST_STATUS",
        " CHANNEL_READ_DST_STATUS_WAIT",
        " CHANNEL_READ_DST_STATUS_DELAY",

        " CHANNEL_DATA_READ_REQ",
        " CHANNEL_DATA_WRITE_REQ",
        " CHANNEL_DATA_WRITE_WAIT",
        " CHANNEL_DATA_END",

        " CHANNEL_SRC_STATUS_WRITE",
        " CHANNEL_SRC_STATUS_WRITE_WAIT",
        " CHANNEL_DST_STATUS_WRITE",
        " CHANNEL_DST_STATUS_WRITE_WAIT",
        " CHANNEL_NEXT_BUFFERS",
    };

    std::cout << "CHBUF_DMA " << name() << " : " 
              << tgt_state_str[r_tgt_fsm.read()] << std::endl;
    for ( uint32_t k = 0 ; k < m_channels ; k++ )
    {
        
        if ( r_channel_run[k].read() )
        {
            std::cout << "  CHANNEL[" << std::dec << k << "] :"
                      << channel_state_str[r_channel_fsm[k].read()]
                      << " / src_buf_addr = " << std::hex 
                      << ((uint64_t)r_channel_src_ext[k].read() << 32) + r_channel_src_buf_addr[k].read()
                      << " / src_sts_addr = " << std::hex 
                      << ((uint64_t)r_channel_src_ext[k].read() << 32) + r_channel_src_sts_addr[k].read()
                      << " / src_index = " << std::dec << r_channel_src_index[k].read()
                      << std::endl
                      << "                                        / dst_buf_addr = " << std::hex 
                      << ((uint64_t)r_channel_dst_ext[k].read() << 32) + r_channel_dst_buf_addr[k].read()
                      << " / dst_sts_addr = " << std::hex 
                      << ((uint64_t)r_channel_dst_ext[k].read() << 32) + r_channel_dst_sts_addr[k].read()
                      << " / dst_index = " << std::dec << r_channel_dst_index[k].read()
                      << std::endl
                      << "            todo_bytes = " << std::hex << r_channel_todo_bytes[k].read()
                      << " / read_count = " << std::dec << r_channel_read_count[k].read()
                      << " / write_count = " << std::dec << r_channel_write_count[k].read()
                      << " / period = " << std::dec << r_channel_period[k].read()
                      << " / timer = " << r_channel_timer[k].read()
                      << std::endl;
        }
        
    }
    std::cout << cmd_state_str[r_cmd_fsm.read()] << std::dec 
              << " / channel = " << r_cmd_channel.read()
              << " / burst = " << r_cmd_burst.read()
              << " / length = " << r_cmd_bytes.read()
              << " / count = " << r_cmd_count.read() << std::endl;
    std::cout << rsp_state_str[r_rsp_fsm.read()] << std::dec 
              << " / channel = " << r_rsp_channel.read()
              << " / burst = " << r_rsp_burst.read()
              << " / count = " << r_rsp_count.read() << std::endl;
}

////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciChbufDma( sc_core::sc_module_name 		        name,
                         const soclib::common::MappingTable 	&mt,
                         const soclib::common::IntTab 		    &srcid,
                         const soclib::common::IntTab 		    &tgtid,
	                     const uint32_t 				        burst_length,
                         const uint32_t 				        channels,
                         const uint32_t                         bursts )
	: caba::BaseModule(name),
     
          m_seglist(mt.getSegmentList(tgtid)),
          m_burst_length(burst_length),
          m_bursts(bursts),
          m_channels(channels),
          m_srcid(mt.indexForId(srcid)),

          p_clk("p_clk"),
          p_resetn("p_resetn"),
          p_vci_target("p_vci_target"),
          p_vci_initiator("p_vci_initiator"),
          p_irq(soclib::common::alloc_elems<sc_core::sc_out<bool> >("p_irq", channels))
{
    std::cout << "  - Building VciChbufDma : " << name << std::endl;

    assert( (m_seglist.empty() == false) and 
    "VCI_CHBUF_DMA error : No segment allocated");

    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ ) 
    {
	    assert( ( (seg->baseAddress() & 0xFFF) == 0 ) and 
		"VCI_CHBUF_DMA Error : The segment base address must be multiple of 4 Kbytes"); 

	    assert( ( seg->size() >= (m_channels<<12) ) and 
		"VCI_CHBUF_DMA Error : The segment size cannot be smaller than 4K * channels"); 

        std::cout << "    => segment " << seg->name()
                  << " / base = " << std::hex << seg->baseAddress()
                  << " / size = " << seg->size() << std::endl; 
    }

    std::cout << "    => burst_length = " << std::dec << burst_length
              << " / channels = " << channels
              << " / bursts = " << bursts << std::endl;

    assert( (vci_param::T >= 4) and 
    "VCI_CHBUF_DMA error : The VCI TRDID field must be at least 4 bits");

    assert( ((vci_param::B == 4) or ( vci_param::B == 8 ) ) and 
    "VCI_CHBUF_DMA error : The VCI DATA field must be 32 or 64 bits");

    assert( (burst_length < (1<<vci_param::K)) and 
    "VCI_CHBUF_DMA error : The VCI PLEN size is too small for requested burst length");

    assert( (((burst_length==4)or(burst_length==8)or(burst_length==16)or 
             (burst_length==32)or(burst_length==64))) and
    "VCI_CHBUF_DMA error : The burst length must be 4, 8, 16, 32, 64 bytes");

    assert( (bursts <= 4) and
            "VCI_CHBUF_DMA error : The number of pipelined bursts cannot be larger than 4");
    
    assert( (channels <= 4)  and
    "VCI_CHBUF_DMA error : The number of channels cannot be larger than 4");

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
}

tmpl(/**/)::~VciChbufDma() { }


}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


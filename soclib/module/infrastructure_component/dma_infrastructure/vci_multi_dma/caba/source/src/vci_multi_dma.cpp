/* -*- c++ -*-
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
//  This component is a multi-channels DMA controller.
//  This component makes the assumption that the VCI RDATA & WDATA fields
//  have 32 bits. It supports VCI address up to 64 bits.
//  If the address is actually larger than 32 bits, the physical source and
//  destination addresses must be loaded in two successive accesses, using
//  DMA_SRC_EXT and DMA_DST_EXT addressable registers. 
//  The number of channels and the max burst length (in bytes) are
//  constructor parameters, wiyh the following constraints: 
//  - The number of channels (simultaneous transfers) cannot be larger than 16.
//  - The burst length (in bytes) must be a power of 2 no larger than 128,
//    and is typically equal to the sytem cache line width.
//  - The source buffer base address and the destination destination buffer
//    must be multiple of 4 bytes. If the source and destination buffers 
//    are not aligned on a burst boundary, the burst is split in two VCI
//    transactions.
//  - The memory buffers length is not constrained to be a multiple of the
//    burst length.
//
//  Each channel takes 32 bytes, and only 7 address bits are decoded:
//  - The 3 bits ADDRESS[4:2] define the target register (see dma.h)
//  - The 4 bits ADDRESS[8:5] define the selected channel.
//
//  For each channel, the DMA_LEN register is used as status register.
//  A read command returns the channel_fsm state value. Relevant values are
//  - DMA_SUCCESS     : 0
//  - DMA_IDLE        : 2
//  - DMA_ERROR_READ  : 1
//  - DMA_ERROR_WRITE : 3
//  - DMA_BUSY        : >3
// 
//  There is one private IRQ line for each channel.
//
//  In order to support multiple simultaneous transactions, the channel
//  index is transmited in the VCI TRDID field.
//  As the LSB bit of the TRDID is used to indicate a non-cachable access,
//  the channel index is encoded in the next 3 bits, and the TRDID width
//  must be at least 4 bits.
//  
//////////////////////////////////////////////////////////////////////////////////
//  Implementation note:
//  This component contains four FSM types:
//  - the tgt_fsm controls the configuration commands and responses 
//    on the VCI target ports.
//  - the cmd_fsm controls the read and write data transfer commands 
//    on the VCI initiator port.
//    It uses four registers : r_cmd_fsm (state), r_cmd_count
//    (counter of bytes in a write burst), r_cmd_index (selected channel),
//    r_cmd_nbytes (VCI PLEN) and r_cmd_curr (current byte in the internal
//    buffer during write burst).
//  - the rsp_fsm controls the read and write data transfer responses 
//    on the VCI initiator port.
//    It uses four registers : r_rsp_fsm (state), r_rsp_count
//    (counter of bytes in a burst), r_rsp_index (selected channel)
//    and r_rsp_nbytes (VCI PLEN).
//  - Each channel_fsm [k] has a set of "state" registers: 
//    r_channel_fsm[k]	         channel state
//    r_channel_activate[k]	     channel actived (a transfer has been requested)
//    r_channel_src_addr[k]	     address of the source memory buffer
//    r_channel_dst_addr[k]	     address of the destination memory buffer
//    r_channel_length[k]	     total length of the memory buffers (bytes)
//    r_channel_src_offset[k]    number of non aligned bytes for source buffer
//    r_channel_dst_offset[k]    number of non aligned bytes for dest buffer
//    r_channel_nbytes_first[k]  number of bytes for requested VCI transaction
//    r_channel_nbytes_second[k] number of bytes for requested VCI transaction
//    r_channel_last[k]          last read/write DMA transaction
//    r_channel_done[k]          transfer completion signaled by RSP FSM
//    r_channel_error[k]         error signaled by RSP FSM
//    r_channel_no_irq[k]        IRQ disabled when true
//    r_channel_buf[k][word]	 local burst buffer 
///////////////////////////////////i///////////////////////////////////////////////

#include <stdint.h>
#include <cassert>

#include "alloc_elems.h"
#include "../include/vci_multi_dma.h"

namespace soclib { namespace caba {

#define tmpl(t) template<typename vci_param> t VciMultiDma<vci_param>

/////////////////////////
tmpl(void)::transition()
{
    if (!p_resetn) 
    {
        r_tgt_fsm    = TGT_IDLE;
        r_cmd_fsm    = CMD_IDLE;
        r_rsp_fsm    = RSP_IDLE;
        r_cmd_index  = 0;
        r_rsp_index  = 0;
        for ( size_t k = 0 ; k < m_channels ; k++ )
        {
            r_channel_fsm[k]      = CHANNEL_IDLE;
            r_channel_activate[k] = false;
            r_channel_done[k] 	  = false;
            r_channel_error[k] 	  = false;
            r_channel_no_irq[k]   = false;
        }
        return;
    }

    ///////////////////////////////////////////////////////////////////////
    // This TGT_FSM controls the VCI TARGET port
    // and the following registers:
    // - r_tgt_fsm
    // - r_channel_src_addr[k] (when the channel is not active)
    // - r_channel_dst_addr[k] (when the channel is not active)
    // - r_channel_length[k]   (when the channel is not active)
    // - r_channel_activate[k]  
    //////////////////////////////////////////////////////////////////////
    switch(r_tgt_fsm.read()) 
    {
        case TGT_IDLE:
        {
            if (p_vci_target.cmdval.read() )
            {
                typename vci_param::fast_addr_t	address = p_vci_target.address.read();
                typename vci_param::cmd_t	    cmd     = p_vci_target.cmd.read();
                uint32_t	                    wdata   = p_vci_target.wdata.read();
               
                bool found = false;
                std::list<soclib::common::Segment>::iterator seg;
                for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ ) 
                {
                    if ( seg->contains(address) ) found = true;
                }
 
                r_srcid					= p_vci_target.srcid.read();
                r_trdid					= p_vci_target.trdid.read();
                r_pktid					= p_vci_target.pktid.read();
                
                size_t 	cell    = (((size_t)address % (DMA_SPAN*4)) >> 2);
                size_t	channel = (((size_t)address / (DMA_SPAN*4)) & 0xF);

                assert( found and
                "VCI_MULTI_DMA error : Out of segment address in configuration");

                assert( (channel < m_channels) and 
                "VCI_MULTI_DMA error : The channel index is too large");

                assert( p_vci_target.eop.read() and
                "VCI_MULTI_DMA error : A configuration or status request mut be one flit");

                //////////////////////////////////////////////////////////
	            if ( (cell == DMA_SRC) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( !r_channel_activate[channel] and
                    "VCI_MULTI_DMA error : Configuration request for an active channel");
                    assert( (wdata%4 == 0) and
                    "VCI_MULTI_DMA error : Source buffer address mut be multiple of 4");

                    r_channel_src_offset[channel] = wdata%m_burst_max_length; 
                    r_channel_src_addr[channel] = (uint64_t)wdata;
                    r_tgt_fsm = TGT_WRITE;
                }
                else if ( (cell == DMA_SRC) and (cmd == vci_param::CMD_READ) )
                {
                    r_rdata   = (uint32_t)r_channel_src_addr[channel].read();
                    r_tgt_fsm = TGT_READ;
                }

                ///////////////////////////////////////////////////////////////////
	            else if ( (cell == DMA_SRC_EXT) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( !r_channel_activate[channel] and
                    "VCI_MULTI_DMA error : Configuration request for an active channel");

                    r_channel_src_addr[channel] = r_channel_src_addr[channel].read() +
                                                  ((uint64_t)wdata << 32);
                    r_tgt_fsm = TGT_WRITE;
                }
                else if ( (cell == DMA_SRC_EXT) and (cmd == vci_param::CMD_READ) )
                {
                    r_rdata   = (uint32_t)(r_channel_src_addr[channel].read() >> 32);
                    r_tgt_fsm = TGT_READ;
                }

                ///////////////////////////////////////////////////////////////
                else if ( (cell == DMA_DST) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( !r_channel_activate[channel] and
                    "VCI_MULTI_DMA error : Configuration request for an active channel");
                    assert( (wdata%4 == 0) and
                    "VCI_MULTI_DMA error : Destination buffer address must be multiple of 4");

                    r_channel_dst_offset[channel] = wdata%m_burst_max_length; 
                    r_channel_dst_addr[channel] = (uint64_t)wdata;
                    r_tgt_fsm = TGT_WRITE;
                }
                else if ( (cell == DMA_DST) and (cmd == vci_param::CMD_READ) )
                {
                    r_rdata   = (uint32_t)r_channel_dst_addr[channel].read();
                    r_tgt_fsm = TGT_READ;
                }

                ///////////////////////////////////////////////////////////////////
	            else if ( (cell == DMA_DST_EXT) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( !r_channel_activate[channel] and
                    "VCI_MULTI_DMA error : Configuration request for an active channel");

                    r_channel_dst_addr[channel] = r_channel_dst_addr[channel].read() +
                                                  ((uint64_t)wdata << 32);
                    r_tgt_fsm = TGT_WRITE;
                }
                else if ( (cell == DMA_DST_EXT) and (cmd == vci_param::CMD_READ) )
                {
                    r_rdata   = (uint32_t)(r_channel_dst_addr[channel].read() >> 32);
                    r_tgt_fsm = TGT_READ;
                }

                ///////////////////////////////////////////////////////////////
                else if ( (cell == DMA_LEN) and (cmd == vci_param::CMD_WRITE) )
                {
                    assert( !r_channel_activate[channel] and
                    "VCI_MULTI_DMA error : Configuration request for an active channel");
                    assert( (wdata%4 == 0) and
                    "VCI_MULTI_DMA error : The buffers length must be a multiple of 4");

                    r_channel_length[channel] = wdata;
                    r_channel_activate[channel] = true;
                    r_tgt_fsm = TGT_WRITE;
                }
                else if ( (cell == DMA_LEN) and (cmd == vci_param::CMD_READ) )
                {
                    r_rdata   = r_channel_fsm[channel].read();
                    r_tgt_fsm = TGT_READ;
                }

                /////////////////////////////////////////////////////////////////
                else if ( (cell == DMA_RESET) and (cmd == vci_param::CMD_WRITE) )
                {
                    r_channel_activate[channel] = false;
                    r_tgt_fsm = TGT_WRITE;
                }

                ////////////////////////////////////////////////////////////////////////
                else if ( (cell == DMA_IRQ_DISABLED) and (cmd == vci_param::CMD_WRITE) )
                {
                    r_channel_no_irq[channel] = (wdata != 0);
                    r_tgt_fsm = TGT_WRITE;
                }
                else if ( (cell == DMA_IRQ_DISABLED) and (cmd == vci_param::CMD_READ) )
                {
                    r_rdata = r_channel_no_irq[channel].read();
                    r_tgt_fsm = TGT_READ;
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

    //////////////////////////////////////////////////////////////////
    // These CHANNEL_FSM define the transfer state for each channel.
    // It implement a loop: a burst of size m_burst_max_length
    // (corresponding to the storage capacity of the local buffer)
    // is transfered at each iteration.
    // The read transaction is split in two VCI transactions
    // if the source buffer is not aligned on a burst boundary.
    // The write transaction is split in two VCI transactions
    // if the destination buffer is not aligned on a burst boundary.
    //////////////////////////////////////////////////////////////////
    for ( size_t k=0 ; k<m_channels ; k++ )
    {
        switch( r_channel_fsm[k].read() )
        {
            //////////////////
            case CHANNEL_IDLE:
            {
                if ( r_channel_activate[k] )
                {
                    r_channel_fsm[k] = CHANNEL_READ_START;
                }
                break;
            }
            ////////////////////////
            case CHANNEL_READ_START:    // prepare the VCI READ transaction(s)
            {
                size_t first  = m_burst_max_length - r_channel_src_offset[k].read();
                size_t second = r_channel_src_offset[k].read();
                size_t length = r_channel_length[k].read();

                if ( length > m_burst_max_length ) 
                {
                    r_channel_nbytes_first[k]  = first;
                    r_channel_nbytes_second[k] = second;
                }
                else if ( length > first  )
                {
                    r_channel_nbytes_first[k]  = first;
                    r_channel_nbytes_second[k] = length - first;
                }
                else   // length <= first
                {
                    r_channel_nbytes_first[k]  = length;
                    r_channel_nbytes_second[k] = 0;
                }
                r_channel_fsm[k] = CHANNEL_READ_REQ_FIRST;
                break;
            }
            ////////////////////////////
            case CHANNEL_READ_REQ_FIRST:	// request first VCI READ transaction
            {
                if ( (r_cmd_fsm == CMD_READ) and (r_cmd_index.read() == k) ) 
                {
                    r_channel_fsm[k] = CHANNEL_READ_WAIT_FIRST;
                }
                break;
            }
            /////////////////////////////
            case CHANNEL_READ_WAIT_FIRST: 	// wait response for first VCI READ
            {
                if ( r_channel_done[k] ) 

                {
                    if ( r_channel_error[k] )   
                    {
                        r_channel_fsm[k] = CHANNEL_READ_ERROR;
                    }
                    else
                    {
                        if ( r_channel_nbytes_second[k].read() == 0 ) 
                        {
                            r_channel_fsm[k] = CHANNEL_WRITE_START;
                        }
                        else            
                        {
                            r_channel_fsm[k] = CHANNEL_READ_REQ_SECOND;
                        }
                    }
                    r_channel_done[k] = false;
                }
                break;
            }
            /////////////////////////////
            case CHANNEL_READ_REQ_SECOND:   // request second VCI READ transaction
            {
                if ( (r_cmd_fsm == CMD_READ) and (r_cmd_index.read() == k) ) 
                {
                    r_channel_fsm[k] = CHANNEL_READ_WAIT_SECOND;
                }
                break;
            }
            //////////////////////////////
            case CHANNEL_READ_WAIT_SECOND:  // wait response for second VCI READ
            {
                if ( r_channel_done[k] ) 
                {
                    if ( r_channel_error[k] )   
                    {
                        r_channel_fsm[k] = CHANNEL_READ_ERROR;
                    }
                    else
                    {
                        r_channel_fsm[k] = CHANNEL_WRITE_START;
                    }
                    r_channel_done[k] = false;
                }
                break;
            }
            /////////////////////////
            case CHANNEL_WRITE_START:	// prepare the VCI WRITE transaction(s)
            {
                size_t first  = m_burst_max_length - r_channel_dst_offset[k].read();
                size_t second = r_channel_dst_offset[k].read();
                size_t length = r_channel_length[k].read();

                if ( length > m_burst_max_length ) 
                {
                    r_channel_nbytes_first[k]  = first;
                    r_channel_nbytes_second[k] = second;
                    r_channel_last[k]          = false;
                }
                else if ( length > first  )
                {
                    r_channel_nbytes_first[k]  = first;
                    r_channel_nbytes_second[k] = length - first;
                    r_channel_last[k]          = true;
                }
                else   // length <= first
                {
                    r_channel_nbytes_first[k]  = length;
                    r_channel_nbytes_second[k] = 0;
                    r_channel_last[k]          = true;
                }
                r_channel_fsm[k] = CHANNEL_WRITE_REQ_FIRST;
                break;
            }
            /////////////////////////////
            case CHANNEL_WRITE_REQ_FIRST:	// request first VCI WRITE transaction
            {
                if ( (r_cmd_fsm == CMD_WRITE) and (r_cmd_index.read() == k) ) 
                {
                    r_channel_fsm[k] = CHANNEL_WRITE_WAIT_FIRST;
                }
                break;
            }
            //////////////////////////////
            case CHANNEL_WRITE_WAIT_FIRST:	// wait response for first VCI WRITE
            {
                if ( r_channel_done[k] ) 
                {
                    if ( r_channel_error[k] )
                    {
                        r_channel_fsm[k] = CHANNEL_WRITE_ERROR;
                    }
                    else 
                    {
                        if ( r_channel_nbytes_second[k].read() != 0 ) 
                        {
                            r_channel_fsm[k] = CHANNEL_WRITE_REQ_SECOND;
                        }
                        else          
                        {
                            if ( r_channel_last[k].read() ) 
                            {
                                r_channel_fsm[k] = CHANNEL_DONE;
                            }
                            else
                            {
                                r_channel_fsm[k] = CHANNEL_READ_START;
                            }
                        }
                    }
                    r_channel_done[k] = false;
                }
                break;
            }
            //////////////////////////////
            case CHANNEL_WRITE_REQ_SECOND:	// request second VCI WRITE transaction
            {
                if ( (r_cmd_fsm == CMD_WRITE) and (r_cmd_index.read() == k) ) 
                {
                    r_channel_fsm[k] = CHANNEL_WRITE_WAIT_SECOND;
                }
                break;
            }
            ///////////////////////////////
            case CHANNEL_WRITE_WAIT_SECOND:	// wait response for second VCI WRITE
            {
                if ( r_channel_done[k] ) 
                {
                    if ( r_channel_error[k] )
                    {
                        r_channel_fsm[k] = CHANNEL_WRITE_ERROR;
                    }
                    else 
                    {
                        if ( r_channel_last[k].read() ) 
                        {
                            r_channel_fsm[k] = CHANNEL_DONE;
                        }
                        else
                        {
                            r_channel_fsm[k] = CHANNEL_READ_START;
                        }
                    }
                    r_channel_done[k] = false;
                }
                break;
            }
            //////////////////
            case CHANNEL_DONE:
            case CHANNEL_READ_ERROR:
            case CHANNEL_WRITE_ERROR:
            {
                if ( !r_channel_activate[k] )	r_channel_fsm[k] = CHANNEL_IDLE;
                break; 
            }
        } // end switch r_channel_fsm[k]
    }
                
    ////////////////////////////////////////////////////////////////////////////
    // This CMD_FSM controls the VCI INIT command port
    // It updates the r_channel_src_addr[k] & r_channel_dst_addr[k] registers.
    ////////////////////////////////////////////////////////////////////////////
    switch(r_cmd_fsm.read()) 
    {
        //////////////
        case CMD_IDLE:
        {
            // round-robin arbitration between channels to send a command
            bool not_found = true;

            for( size_t n = 0 ; (n < m_channels) and not_found ; n++ )
            {
                size_t k = (r_cmd_index.read() + n + 1) % m_channels;
                if ( r_channel_fsm[k] == CHANNEL_READ_REQ_FIRST )
                { 
                    not_found    = false;

                    r_cmd_index  = k;
                    r_cmd_nbytes = r_channel_nbytes_first[k].read();
                    r_cmd_fsm    = CMD_READ;
                }
                else if ( r_channel_fsm[k] == CHANNEL_READ_REQ_SECOND )
                {
                    not_found    = false;

                    r_cmd_index  = k;
                    r_cmd_nbytes = r_channel_nbytes_second[k].read();
                    r_cmd_fsm    = CMD_READ;
                }
                else if ( r_channel_fsm[k] == CHANNEL_WRITE_REQ_FIRST )
                {
                    not_found    = false;

                    r_cmd_index  = k;
                    r_cmd_nbytes = r_channel_nbytes_first[k].read();
                    r_cmd_count  = 0;
                    r_cmd_curr   = 0;
                    r_cmd_fsm    = CMD_WRITE;
                }
                else if (r_channel_fsm[k] == CHANNEL_WRITE_REQ_SECOND )
                {
                    not_found    = false;
                    
                    r_cmd_index  = k;
                    r_cmd_nbytes = r_channel_nbytes_second[k].read();
                    r_cmd_count  = 0;
                    r_cmd_fsm    = CMD_WRITE;
                }
            }
            break;
        }
        //////////////
        case CMD_READ:
        {
            if ( p_vci_initiator.cmdack.read() )
            {
                size_t k = r_cmd_index.read();
                r_channel_src_addr[k] = r_channel_src_addr[k].read() + r_cmd_nbytes.read();
                r_cmd_fsm = CMD_IDLE;
            }
            break;
        }
        /////////////// 
        case CMD_WRITE:
        {
            if ( p_vci_initiator.cmdack.read() )
            {
                if ( r_cmd_count.read() == (r_cmd_nbytes.read() - 4) )
                {
                    size_t k = r_cmd_index.read();
                    r_channel_dst_addr[k] = r_channel_dst_addr[k].read() + r_cmd_nbytes.read();
                    r_cmd_fsm = CMD_IDLE;
                }
                r_cmd_count = r_cmd_count.read() + 4;
                r_cmd_curr  = r_cmd_curr.read() + 1;
            }
            break;
        }
    } // end switch cmd_fsm

    /////////////////////////////////////////////////////////////////////////
    // This RSP_FSM controls the VCI INIT response port
    // It updates the r_channel_length[k] register,
    // and sets the r_channel_done[k] & r_channel_error[k] flip-flops
    // to signal completion of the read / write VCI transaction.
    /////////////////////////////////////////////////////////////////////////
    switch(r_rsp_fsm.read()) 
    {
        //////////////
        case RSP_IDLE:
        {
            if ( p_vci_initiator.rspval.read() )
            {
                size_t k = (size_t)p_vci_initiator.rtrdid.read();

                if ( r_channel_fsm[k].read() == CHANNEL_READ_WAIT_FIRST ) 
                {
                    r_rsp_count  = 0;
                    r_rsp_index  = k;
                    r_rsp_fsm    = RSP_READ;
                }
                else if ( r_channel_fsm[k].read() == CHANNEL_READ_WAIT_SECOND ) 
                {
                    r_rsp_index  = k;
                    r_rsp_fsm    = RSP_READ;
                }
                else if ( r_channel_fsm[k].read() == CHANNEL_WRITE_WAIT_FIRST ) 
                {
                    r_rsp_index  = k;
                    r_rsp_nbytes = r_channel_nbytes_first[k].read();
                    r_rsp_fsm    = RSP_WRITE;
                }
                else if ( r_channel_fsm[k].read() == CHANNEL_WRITE_WAIT_SECOND )
                {
                    r_rsp_index  = k;
                    r_rsp_nbytes = r_channel_nbytes_second[k].read();
                    r_rsp_fsm    = RSP_WRITE;
                }
                else
                {
                    std::cout << "VCI_MULTI_DMA error : unexpected VCI response" << std::endl;
                    exit(0);
                }  
            }
            break;
        }
        //////////////
        case RSP_READ:
        {
            if ( p_vci_initiator.rspval.read() )
            {
                size_t k    = r_rsp_index.read();
                size_t word = r_rsp_count.read()>>2;

                r_channel_buf[k][word] = p_vci_initiator.rdata.read(); 

                if ( p_vci_initiator.reop.read() )
                {
                    assert( (r_rsp_count.read() < m_burst_max_length) and
                    "VCI_MULTI_DMA error : wrong number of flits for a read response packet");

                    r_channel_done[k] = true;
                    r_channel_error[k] = ((p_vci_initiator.rerror.read() & 0x1) != 0);
                    r_rsp_fsm = RSP_IDLE;
                } 
                r_rsp_count = r_rsp_count.read() + 4;
            }
            break;
        } 
        ///////////////
        case RSP_WRITE:
        {
            if ( p_vci_initiator.rspval.read() )
            {
                 assert( (p_vci_initiator.reop.read() == true) and
                 "VCI_MULTI_DMA error : write response packed must contain one flit");  

                size_t k  = r_rsp_index.read();
                r_channel_length[k] = r_channel_length[k].read() - r_rsp_nbytes.read();
                r_channel_done[k]   = true;
                r_channel_error[k]  = ((p_vci_initiator.rerror.read() & 0x1) != 0);
                r_rsp_fsm           = RSP_IDLE;
            }
            break;
        } 
    } // end switch rsp_fsm
} // end transition

//////////////////////
tmpl(void)::genMoore()
{
    /////// VCI INIT CMD ports ////// 
    switch( r_cmd_fsm.read() ) {
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
            size_t k = r_cmd_index.read();

            p_vci_initiator.cmdval  = true;
            p_vci_initiator.address = r_channel_src_addr[k].read(); 
            p_vci_initiator.wdata   = 0;
            p_vci_initiator.be      = 0xF;
            p_vci_initiator.plen    = r_cmd_nbytes.read();
            p_vci_initiator.cmd     = vci_param::CMD_READ;
            p_vci_initiator.trdid   = k;
            p_vci_initiator.pktid   = TYPE_READ_DATA_UNC; // compatible with TSAR pktid encoding
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
            size_t k = r_cmd_index.read();
            size_t n = r_cmd_curr.read();

            p_vci_initiator.cmdval  = true;
            p_vci_initiator.address = r_channel_dst_addr[k].read() + r_cmd_count.read();
            p_vci_initiator.wdata   = r_channel_buf[k][n].read();
            p_vci_initiator.be      = 0xF;
            p_vci_initiator.plen    = r_cmd_nbytes.read();
            p_vci_initiator.cmd     = vci_param::CMD_WRITE;
            p_vci_initiator.trdid   = k;
            p_vci_initiator.pktid   = TYPE_WRITE; // compatible with TSAR pktid encoding
            p_vci_initiator.srcid   = m_srcid;
            p_vci_initiator.cons    = false;
            p_vci_initiator.wrap    = false;
            p_vci_initiator.contig  = true;
            p_vci_initiator.clen    = 0;
            p_vci_initiator.cfixed  = false;
            p_vci_initiator.eop     = ( r_cmd_count.read() == r_cmd_nbytes.read() - 4 );
            break;
        }
    } // end switch cmd_fsm

    /////// VCI INIT RSP port ////// 
    if ( r_rsp_fsm.read() == RSP_IDLE )  p_vci_initiator.rspack = false;
    else                                 p_vci_initiator.rspack = true;

    ////// VCI TARGET ports /////// 
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
            p_vci_target.rsrcid = r_srcid.read();
            p_vci_target.rtrdid = r_trdid.read();
            p_vci_target.rpktid = r_pktid.read();
            p_vci_target.reop   = true;
            break;
        }
        case TGT_READ:
        {
            p_vci_target.cmdack = false;
            p_vci_target.rspval = true;
            p_vci_target.rdata  = r_rdata.read();
            p_vci_target.rerror = vci_param::ERR_NORMAL;
            p_vci_target.rsrcid = r_srcid.read();
            p_vci_target.rtrdid = r_trdid.read();
            p_vci_target.rpktid = r_pktid.read();
            p_vci_target.reop   = true;
            break;
        }
        case TGT_ERROR:
        {
            p_vci_target.cmdack = false;
            p_vci_target.rspval = true;
            p_vci_target.rdata  = 0;
            p_vci_target.rerror = vci_param::ERR_GENERAL_DATA_ERROR;
            p_vci_target.rsrcid = r_srcid.read();
            p_vci_target.rtrdid = r_trdid.read();
            p_vci_target.rpktid = r_pktid.read();
            p_vci_target.reop   = true;
            break;
        }
    } // end switch rsp_fsm

    /////// IRQ ports //////////////////////////
    for ( size_t k = 0 ; k < m_channels ; k++ )
    {

        bool irq = ( r_channel_fsm[k].read() == CHANNEL_DONE )       or 
                   ( r_channel_fsm[k].read() == CHANNEL_READ_ERROR ) or
                   ( r_channel_fsm[k].read() == CHANNEL_WRITE_ERROR );

	    p_irq[k] = irq and not r_channel_no_irq[k];
    }
}  // end genMoore()

/////////////////////////
tmpl(void)::print_trace()
{
    const char* tgt_state_str[] = 
    {
        "TGT_IDLE ",
        "TGT_READ ",
        "TGT_WRITE",
        "TGT_ERROR"
    };
    const char* cmd_state_str[] = 
    {
        "  CMD_IDLE ",
        "  CMD_READ ",
        "  CMD_WRITE"
    };
    const char* rsp_state_str[] = 
    {
        "  RSP_IDLE ",
        "  RSP_READ ",
        "  RSP_WRITE"
    };
    const char* channel_state_str[] = 
    {
        "CHANNEL_DONE",
        "CHANNEL_READ_ERROR",
        "CHANNEL_IDLE",
        "CHANNEL_WRITE_ERROR",
        "CHANNEL_READ_START",
        "CHANNEL_READ_REQ_FIRST",
        "CHANNEL_READ_WAIT_FIRST",
        "CHANNEL_READ_REQ_SECOND",
        "CHANNEL_READ_WAIT_SECOND",
        "CHANNEL_WRITE_START",
        "CHANNEL_WRITE_REQ_FIRST",
        "CHANNEL_WRITE_WAIT_FIRST",
        "CHANNEL_WRITE_REQ_SECOND",
        "CHANNEL_WRITE_WAIT_SECOND"
    };

    std::cout << "MULTI_DMA " << name() << " : " << tgt_state_str[r_tgt_fsm.read()] << std::endl;
    for ( size_t k = 0 ; k < m_channels ; k++ )
    {
        std::cout << "CHANNEL " << k << std::hex
                  << " : active = " << r_channel_activate[k].read() 
                  << " : state = " << channel_state_str[r_channel_fsm[k].read()]
                  << " / src = " << r_channel_src_addr[k].read()
                  << " / dst = " << r_channel_dst_addr[k].read() << std::dec
                  << " / length = " << r_channel_length[k].read()
                  << std::endl;
    }
    std::cout << cmd_state_str[r_cmd_fsm.read()] << std::dec 
              << " / channel = " << r_cmd_index.read()
              << " / length = " << r_cmd_nbytes.read()
              << " / count = " << r_cmd_count.read()/4 << std::endl;
    std::cout << rsp_state_str[r_rsp_fsm.read()] << std::dec 
              << " / channel = " << r_rsp_index.read()
              << " / length = " << r_rsp_nbytes.read()
              << " / count = " << r_rsp_count.read()/4 << std::endl;
}

////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciMultiDma( sc_core::sc_module_name 		        name,
                         const soclib::common::MappingTable 	&mt,
                         const soclib::common::IntTab 		    &srcid,
                         const soclib::common::IntTab 		    &tgtid,
	                     const size_t 				            burst_max_length,
                         const size_t 				            channels )
	: caba::BaseModule(name),
          r_tgt_fsm("r_tgt_fsm"),
          r_srcid("r_srcid"),
          r_trdid("r_trdid"),
          r_pktid("r_pktid"),
          r_rdata("r_rdata"),
          r_channel_fsm(soclib::common::alloc_elems<sc_signal<int> >
                    ("r_channel_fsm", channels)),
          r_channel_activate(soclib::common::alloc_elems<sc_signal<bool> >
                    ("r_channel_activate", channels)),
          r_channel_src_offset(soclib::common::alloc_elems<sc_signal<size_t> >
                    ("r_channel_src_offset", channels)),
          r_channel_dst_offset(soclib::common::alloc_elems<sc_signal<size_t> >
                    ("r_channel_dst_offset", channels)),
          r_channel_nbytes_first(soclib::common::alloc_elems<sc_signal<size_t> >
                    ("r_channel_nbytes_first", channels)),
          r_channel_nbytes_second(soclib::common::alloc_elems<sc_signal<size_t> >
                    ("r_channel_nbytes_second", channels)),
          r_channel_src_addr(soclib::common::alloc_elems<sc_signal<uint64_t> >
                    ("r_channel_src_addr", channels)),
          r_channel_dst_addr(soclib::common::alloc_elems<sc_signal<uint64_t> >
                    ("r_channel_dst_addr", channels)),
          r_channel_length(soclib::common::alloc_elems<sc_signal<size_t> >
                    ("r_channel_length", channels)),
          r_channel_buf(soclib::common::alloc_elems<sc_signal<uint32_t> >
                    ("r_channel_buf", channels, burst_max_length/4)),
          r_channel_last(soclib::common::alloc_elems<sc_signal<bool> >
                    ("r_channel_done", channels)),
          r_channel_done(soclib::common::alloc_elems<sc_signal<bool> >
                    ("r_channel_last", channels)),
          r_channel_error(soclib::common::alloc_elems<sc_signal<bool> >
                    ("r_channel_error", channels)),
          r_channel_no_irq(soclib::common::alloc_elems<sc_signal<bool> >
                    ("r_channel_no_irq", channels)),
          r_cmd_fsm("r_cmd_fsm"),
          r_cmd_count("r_cmd_count"),
          r_cmd_index("r_cmd_index"),
          r_cmd_nbytes("r_cmd_nbytes"),
          r_rsp_fsm("r_rsp_fsm"),
          r_rsp_count("r_rsp_count"),
          r_rsp_index("r_rsp_index"),
          r_rsp_nbytes("r_rsp_nbytes"),

          m_seglist(mt.getSegmentList(tgtid)),
          m_burst_max_length(burst_max_length),
          m_channels(channels),
          m_srcid(mt.indexForId(srcid)),

          p_clk("p_clk"),
          p_resetn("p_resetn"),
          p_vci_target("p_vci_target"),
          p_vci_initiator("p_vci_initiator"),
          p_irq(soclib::common::alloc_elems<sc_core::sc_out<bool> >("p_irq", channels))
{
    std::cout << "  - Building VciMultiDma " << name << std::endl;

    assert( (m_seglist.empty() == false) and
    "VCI_MULTI_DMA error : no segment allocated");

    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ )
    {
        std::cout << "    => segment " << seg->name()
                  << " / base = " << std::hex << seg->baseAddress()
                  << " / size = " << seg->size() << std::endl; 
    }

    assert( (vci_param::T >= 4) and 
    "VCI_MULTI_DMA error : The VCI TRDID field must be at least 4 bits");

    assert( (vci_param::B == 4) and 
    "VCI_MULTI_DMA error : The VCI DATA field must be 32 bits");

    assert( (burst_max_length < (1<<vci_param::K)) and 
    "VCI_MULTI_DMA error : Burst length is not possible with the VCI PLEN size");

    assert( ((burst_max_length==4) or (burst_max_length==8) or (burst_max_length==16) or 
             (burst_max_length==32) or (burst_max_length==64)) and
    "VCI_MULTI_DMA error : The burst length must be 4, 8, 16, 32, 64 bytes");
    
    assert( (channels <= 16)  and
    "VCI_MULTI_DMA error : The number of channels cannot be larger than 16");

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
}


tmpl(/**/)::~VciMultiDma() {
    soclib::common::dealloc_elems<sc_signal<int> >(r_channel_fsm, m_channels);
    soclib::common::dealloc_elems<sc_signal<bool> >(r_channel_activate, m_channels);
    soclib::common::dealloc_elems<sc_signal<size_t> >(r_channel_src_offset, m_channels);
    soclib::common::dealloc_elems<sc_signal<size_t> >(r_channel_dst_offset, m_channels);
    soclib::common::dealloc_elems<sc_signal<size_t> >(r_channel_nbytes_first, m_channels);
    soclib::common::dealloc_elems<sc_signal<size_t> >(r_channel_nbytes_second, m_channels);
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_src_addr, m_channels);
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_dst_addr, m_channels);
    soclib::common::dealloc_elems<sc_signal<size_t> >(r_channel_length, m_channels);
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_buf, m_channels, m_burst_max_length / 4);
    soclib::common::dealloc_elems<sc_signal<bool> >(r_channel_last, m_channels);
    soclib::common::dealloc_elems<sc_signal<bool> >(r_channel_done, m_channels);
    soclib::common::dealloc_elems<sc_signal<bool> >(r_channel_error, m_channels);
    soclib::common::dealloc_elems<sc_core::sc_out<bool> >(p_irq, m_channels);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


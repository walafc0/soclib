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
 *         Alain Greiner <alain.greiner@lip6.fr> May 2012
 *
 * Maintainers: alain
 */

/////////////////////////////////////////////////////////////////////////////////////////
// This component is a multi-channels DMA controller, that can be used to connect
// hardware coprocessors to VCI interconnects.
//
// On the coprocessor side, it provides TO_COPROC and FROM_COPROC ports, that implement
// FIFO-like interfaces. Coprocessor can request to read (resp. write) N 32 bits words
// on a TO_COPROC (resp. FROM_COPROC) port, without addresses.  
// Each TO_COPROC or FROM_COPROC port define a communication channel to a memory buffer.
// Each channel contains a private hardware fifo (2 32 bits slots).
// The number of TO_COPROC and FROM_COPROC channels, are constructor parameters. 
// The total number of channels cannot be larger than 16.
// 
// On the VCI side, it makes the assumption that the VCI RDATA & WDATA fields 
// have 32 bits, but the VCI address field can have 32 or 64 bits.
// 
// Each channel implements two transfer modes that can be defined by software:
//
// - In MWMR_MODE the channel FSM transfer an "infinite" data stream, between 
//   the coprocessor port and a MWMR channel (software FIFO in memory).
//   The channel FSM implements the 7 steps MWMR protocol:
//   1 - Read the ticket for queuing lock (1 flit VCI READ)
//   2 - Increment atomically the ticket (2 flits VCI CAS)
//   3 - Read the lock current value (1 flit VCI READ)
//   4 - Read the channel status (3 flits VCI READ)
//   5 - Transfer the data (N flits VCI READ or WRITE)
//   6 - Upate the status (3 flits VCI WRITE)
//   7 - Release the lock (1 flit VCI WRITE) 
//   In this mode the software must write in the channel configuration registers
//   the data buffer address, the channel descriptor address, the lock address, 
//   and the channel size. The IRQ is only asserted if a VCI error is reported.
//
// - In MODE_DMA_IRQ or MODE_DMA_NO_IRQ modes, the channel FSM transfer a single 
//   buffer between the memory and the coprocessor port. 
//   The number of VCI burst depends on both the memory buffer size, and the size 
//   of burst defined by the coprocessor type.
//   In this mode the software must define the channel configuration by writing 
//   the data buffer address & the buffer size in the channel configuration registers.
//   When the transfer is completed the channel FSM waits in a SUCCESS or ERROR state
//   until the channel is reset by writing a zero value in the MWR_CHANNEL_RUN register.
//   An optional interrupt can be activated when the requested transfer is completed
//   (only in MODE_DMA_IRQ). In MODE_DMA_NO_IRQ, the software must poll the 
//   MWR_CHANNEL_STATUS register.
//
// WARNING : In both modes the channel FSM uses read or write bursts to transfer
// the data, and a constructor parameter define the max burst size (bytes).
// 1) The memory buffer address and size  must be multiple of this burst size.
// 2) The number of bytes requested by the coprocessor on the TO_COPROC or
//    FROM_COPROC ports must be a multiple of this burst size.
//
// Several channels can simultaneously run in different modes, and the various 
// VCI transactions corresponding to different channels are interleaved and
// parallelized on the VCI network. The maximum number of simultaneous VCI
// transactions is equal to the number of channels. 
//
// For each channel, the software addressable registers are:
// - MWR_CHANNEL_BUFFER_LSB[k]     data buffer physical address 32 LSB bits    (MWMR or DMA)
// - MWR_CHANNEL_BUFFER_MSB[k]     data buffer physical address extend bits    (MWMR or DMA)
// - MWR_CHANNEL_DESC_LSB[k]       channel status physical address 32 LSB bits (MWMR   only)
// - MWR_CHANNEL_DESC_MSB[k]       channel status physical address extend bits (MWMR   only)
// - MWR_CHANNEL_LOCK_LSB[k]       channel lock physical address 32 LSB bits   (MWMR   only)
// - MWR_CHANNEL_LOCK_MSB[k]       channel lock physical address extend bits   (MWMR   only)
// - MWR_CHANNEL_WAY[k]            channel direction (TO_COPROC / FROM_COPROC) (MWMR or DMA)
// - MWR_CHANNEL_MODE[k]           MWMR / DMA_IRQ / DMA_NO_IRQ                 (MWMR or DMA)
// - MWR_CHANNEL_SIZE[k]           data buffer size (bytes)                    (MWMR or DMA) 
// - MWR_CHANNEL_RUN[k]            channel activation/deativation              (MWMR or DMA)
// - MWR_CHANNEL_STATUS[k]                                                     (MWMR or DMA)
//
// Besides these channel registers, this component supports
// up to 16 (R/W) coprocessor registers COPROC_REG[i].
//
// All addressable registers are 32 bits, and the register map is the following:
// - The 16 first registers are the coprocessor registers.
// - Each MWMR channel [k] occupies 16 slots. 
////////////////////////////////////////////////////////////////////////////////////////
// Implementation Note
//
// This module contains (m_all_channels + 3) FSMs:
// - TGT_FSM controls the configuration requests on the VCI target port
// - CMD_FSM : controls the VCI commands on the VCI init port
// - RSP_FSM : controls the VCI responses on the VCI init port
// - CHANNEL_FSM[k] : control the MWMR or DMA transactions for channel[k].
//
// To store the global state of each communication channel, we define an array 
// of (channel_state_t) structures, that contain all registers defining the 
// channel state. In this array, the TO_COPROC channels are stored first,
// and the FROM_COPROC channels are stored last.
//
// The communication between CHANNEL_FSM[k] and CMD_FSM uses the set/reset
// flip-flop r_channel[k].request.
// The communication between RSP_FSM and CHANNEL_FSM[k] uses the set/reset
// flip-flop r_channel[k].response.
//
// In order to avoid Mealy dependancies, each channel contains a two slots
// 32 bits wide, hardware FIFO directly connected to the coprocessor ports.
//
// In order to support VCI transactions parallelism, we use the VCI TRDID
// to transport the channel index.
//
// To access the lock protecting a MWMR channel, this component uses a 
// Compare&Swap (CAS) VCI transaction. Therefore, it uses the VCI command
// encoding extension defined by the TSAR architecture (PKTID field).
//
// The "coprocessor configuration registers" are actually located in the
// multi-channel DMA component. The "coprocessor status registers"
// are actually located in the coprocessor (no copy in the DMA component).
//
// For hardware simplicity, software configuration errors are detected,
// but the simulation stops with an error message. 
///////////////////////////////////////////////////////////////////////////////////////

#include "vci_mwmr_dma.h"
#include "generic_fifo.h"
#include "alloc_elems.h"
#include <cstring>
#include <cassert>


namespace soclib { namespace caba {

using soclib::common::alloc_elems;
using soclib::common::dealloc_elems;

#define tmpl(t) template<typename vci_param> t VciMwmrDma<vci_param>

////////////////////////
tmpl(void)::transition()
{
	if (!p_resetn) 
    {
        r_cmd_fsm  = CMD_IDLE;
        r_rsp_fsm  = RSP_IDLE;
        r_tgt_fsm  = TGT_IDLE;
        r_cmd_k    = 0;
        r_rsp_k    = 0;

        for ( size_t k = 0 ; k < m_all_channels ; k++ ) 
        {
            r_channel[k].fsm      = CHANNEL_IDLE;
            r_channel[k].running  = false;
            r_channel[k].request  = false;
            r_channel[k].response = false;
            r_channel[k].ptr      = 0;
            r_channel[k].ptw      = 0;
            r_channel[k].fifo->init();
        }

        for ( size_t i = 0 ; i < 16 ; i++ )
        {
            r_coproc_config[i] = 0;
        }
		return;
	}

    // set default values for the two selected hardware FIFOs,
    bool        vci_fifo_put   = false;   // the RSP_FSM put data to only one hardware FIFO
    bool        vci_fifo_get   = false;   // the CMD_FSM get data from only one hardware FIFO
    uint32_t    vci_fifo_wdata = 0;       // data written into hardware FIFO by RSP_FSM

    /////////////////////////////////////////////////////////////////////////////////
    // The TGT_FSM controls the VCI target port, handling configuration commands.
    // The command is consumed, decoded and executed in IDLE state.
    // The response is sent in READ or WRITE states.
    // In order to simplify error handling, the simulation stops in case 
    // of configuration errors (illegal commands).  
    /////////////////////////////////////////////////////////////////////////////////
    switch(r_tgt_fsm.read()) 
    {
        //////////////
        case TGT_IDLE:
        {
            if (p_vci_target.cmdval.read() )
            {
                typename vci_param::fast_addr_t	address = p_vci_target.address.read();
                typename vci_param::cmd_t	    cmd     = p_vci_target.cmd.read();
                uint32_t                        wdata   = p_vci_target.wdata.read();

                r_tgt_srcid	= p_vci_target.srcid.read();
                r_tgt_trdid	= p_vci_target.trdid.read();
                r_tgt_pktid	= p_vci_target.pktid.read();
                
                // get channel_id & register_id 
                // channel_id == 0 for coprocessor specific registers
                uint32_t cell = (uint32_t)((address & 0x03C) >> 2);  
                uint32_t k    = (uint32_t)((address & 0x3C0) >> 6); 

                assert( m_segment.contains( address ) and 
                "VCI_MWMR_DMA config error: out of segment address");

                assert( (k <= m_all_channels) and 
                "VCI_MWMR_DMA config error:  channel index too large");

                assert( p_vci_target.eop.read() and
                "VCI_MWMR_DMA config error: configuration request mut be one flit");

                // decoding read access
                if ( cmd == vci_param::CMD_READ )       
                {
                    r_tgt_fsm = TGT_READ;

                    if ( k == 0 )          // coprocessor status registers
                    {
                        assert ( (cell < m_coproc_status_regs) and
                        "VCI_MWMR_DMA error : coprocessor status register index too large");
                        r_tgt_data = p_status[cell].read();
                    }
                    else if ( cell == MWR_CHANNEL_WAY )
                    {
		                r_tgt_data = (uint32_t)r_channel[k-1].way;
                    }
                    else if ( cell == MWR_CHANNEL_MODE )
                    {
		                r_tgt_data = (uint32_t)r_channel[k-1].mode;
                    }
                    else if ( cell == MWR_CHANNEL_BUFFER_LSB )
                    {
		                r_tgt_data = (uint32_t)r_channel[k-1].buffer_paddr; 
                    }
                    else if ( cell == MWR_CHANNEL_BUFFER_MSB )
                    {
		                r_tgt_data = (uint32_t)(r_channel[k-1].buffer_paddr>>32); 
                    }
                    else if ( cell == MWR_CHANNEL_DESC_LSB )
                    {
		                r_tgt_data = (uint32_t)r_channel[k-1].desc_paddr; 
                    }
                    else if ( cell == MWR_CHANNEL_DESC_MSB )
                    {
		                r_tgt_data = (uint32_t)(r_channel[k-1].desc_paddr>>32); 
                    }
                    else if ( cell == MWR_CHANNEL_LOCK_LSB )
                    {
		                r_tgt_data = (uint32_t)r_channel[k-1].lock_paddr; 
                    }
                    else if ( cell == MWR_CHANNEL_LOCK_MSB )
                    {
		                r_tgt_data = (uint32_t)(r_channel[k-1].lock_paddr>>32); 
                    }
                    else if ( cell == MWR_CHANNEL_SIZE )
                    {
		                r_tgt_data = r_channel[k-1].buffer_size;
                    }
                    else if ( cell == MWR_CHANNEL_RUNNING )
                    {
		                r_tgt_data = r_channel[k-1].running;
                    }
                    else if ( cell == MWR_CHANNEL_STATUS )
                    {
                        if      ( r_channel[k-1].fsm.read() == CHANNEL_SUCCESS ) 
                            r_tgt_data = MWR_CHANNEL_SUCCESS;
                        else if ( r_channel[k-1].fsm.read() == CHANNEL_ERROR_DATA ) 
                            r_tgt_data = MWR_CHANNEL_ERROR_DATA;
                        else if ( r_channel[k-1].fsm.read() == CHANNEL_ERROR_DESC ) 
                            r_tgt_data = MWR_CHANNEL_ERROR_DESC;
                        else if ( r_channel[k-1].fsm.read() == CHANNEL_ERROR_LOCK ) 
                            r_tgt_data = MWR_CHANNEL_ERROR_LOCK;
                        else
                            r_tgt_data = MWR_CHANNEL_BUSY;
                    }
                    else
                    {
                        assert(false and 
                        "VCI_MWMR_DMA config error: undefined channel register");
                    }
	            }

                // decoding write access  
                else if ( cmd == vci_param::CMD_WRITE )    
                {
                    r_tgt_fsm = TGT_WRITE;

                    if ( k == 0 )         // coprocessor config registers
                    {
                        assert ( (cell < m_coproc_config_regs) and
                        "VCI_MWMR_DMA error: coprocessor config register index too large");
        		        r_coproc_config[cell] = wdata;
                    }
                    else if ( cell == MWR_CHANNEL_WAY )
                    {
                        assert(false and 
                        "VCI_MWMR_DMA error in config : WAY not configurable by soft");
                    }
                    else if ( cell == MWR_CHANNEL_MODE )
                    {
                        if      ( wdata == MODE_MWMR )       r_channel[k-1].mode = MODE_MWMR;
                        else if ( wdata == MODE_DMA_IRQ )    r_channel[k-1].mode = MODE_DMA_IRQ;
                        else if ( wdata == MODE_DMA_NO_IRQ ) r_channel[k-1].mode = MODE_DMA_NO_IRQ;
                        else assert( false and 
                             "VCI_MWMR_DMA error in config : undefined mode");
                    }

                    else if ( cell == MWR_CHANNEL_BUFFER_LSB )
                    {
                        assert( ((wdata % m_burst_size) == 0) and
                        "VCI_MWMR_DMA error in config : buffer base not multiple of burst size");
		                r_channel[k-1].buffer_paddr = (uint64_t)wdata;
                    }
                    else if ( cell == MWR_CHANNEL_BUFFER_MSB )
                    {
		                r_channel[k-1].buffer_paddr = r_channel[k-1].buffer_paddr +
                                                      (((uint64_t)wdata)<<32);
                    }
                    else if ( cell == MWR_CHANNEL_DESC_LSB )
                    {
		                r_channel[k-1].desc_paddr = (uint64_t)wdata;
                    }
                    else if ( cell == MWR_CHANNEL_DESC_MSB )
                    {
		                r_channel[k-1].desc_paddr = r_channel[k-1].desc_paddr +
                                                      (((uint64_t)wdata)<<32);
                    }
                    else if ( cell == MWR_CHANNEL_LOCK_LSB )
                    {
		                r_channel[k-1].lock_paddr = (uint64_t)wdata;
                    }
                    else if ( cell == MWR_CHANNEL_LOCK_MSB )
                    {
		                r_channel[k-1].lock_paddr = r_channel[k-1].lock_paddr +
                                                      (((uint64_t)wdata)<<32);
                    }
                    else if ( cell == MWR_CHANNEL_SIZE )
                    {
                        assert( ((wdata % m_burst_size) == 0) and
                        "VCI_MWMR_DMA error in config : buffer size not multiple of burst size");
		                r_channel[k-1].buffer_size = wdata;
                    }
                    else if ( cell == MWR_CHANNEL_RUNNING )
                    {
		                r_channel[k-1].running = (wdata != 0);
                    }
                    else
                    {
                        assert(false and 
                        "VCI_MWMR_DMA error in config: undefined channel register");
                    }
                }
                else    
                {
                    assert( false and 
                    "VCI_MWMR_DMA error : illegal VCI CMD");
                }
            }
            break;
        }
        case TGT_READ:
        case TGT_WRITE:
        {
            if ( p_vci_target.rspack.read() ) r_tgt_fsm = TGT_IDLE;
            break;
        }
    } // end switch TGT_FSM

    ////////////////////////////////////////////////////////////////////////////////
    // This CMD_FSM controls the commands on the VCI initiator port.
    // It is acting as a server for all CHANNEL_FSM[k], with a round_robin policy.
    // The 8 possible VCI transaction types are :
    // - TICKET_READ    1 flit
    // - TICKET_CAS     1/2 flits 
    // - LOCK_READ      1 flit
    // - STATUS_READ    3 flits
    // - STATUS_UPDT    3 flits
    // - LOCK_RELEASE   1 flit
    // - DATA_READ      m_burst_size/4 flits
    // - DATA_WRITE     m_burst_size/4 flits
    /////////////////////////////////////////////////////////////////////////////////
	switch ( r_cmd_fsm.read() ) 
    {
        ///////////////
	    case CMD_IDLE:     // scan pending requests for all channel
        {
            // round-robin arbitration between all channels
            bool   found = false;
            size_t k; 

		    for ( size_t x = 0 ; x < m_all_channels ; x++ )
            {
			    k = (r_cmd_k.read() + x + 1) % m_all_channels;
                if ( r_channel[k].request )
                {
                    found = true;
                    break;
                }
            }

            if ( found )
            {
                r_cmd_word = 0;
                r_cmd_k    = k;
                if      ( r_channel[k].reqtype == TICKET_READ  ) r_cmd_fsm = CMD_TICKET_READ;
                else if ( r_channel[k].reqtype == TICKET_CAS   ) r_cmd_fsm = CMD_TICKET_CAS_OLD;
                else if ( r_channel[k].reqtype == LOCK_READ    ) r_cmd_fsm = CMD_LOCK_READ;
                else if ( r_channel[k].reqtype == STATUS_READ  ) r_cmd_fsm = CMD_STATUS_READ;
                else if ( r_channel[k].reqtype == STATUS_UPDT  ) r_cmd_fsm = CMD_STATUS_UPDT_STS;
                else if ( r_channel[k].reqtype == LOCK_RELEASE ) r_cmd_fsm = CMD_LOCK_RELEASE;
                else if ( r_channel[k].reqtype == DATA_READ    ) r_cmd_fsm = CMD_DATA_READ;
                else if ( r_channel[k].reqtype == DATA_WRITE   ) r_cmd_fsm = CMD_DATA_WRITE;

                // reset synchro
                r_channel[k].request = false;
            } 
            break;
        }
        /////////////////////
	    case CMD_TICKET_READ:    // send read ticket command (one flit)
        {
		    if ( p_vci_initiator.cmdack.read() ) r_cmd_fsm = CMD_IDLE;
            break;
	    }
        ////////////////////////
	    case CMD_TICKET_CAS_OLD:     // send first flit for CAS command (old)
        {
		    if ( p_vci_initiator.cmdack.read() ) r_cmd_fsm = CMD_TICKET_CAS_NEW;
            break;
        }
        ////////////////////////
	    case CMD_TICKET_CAS_NEW:    // send second flit for CAS command (new)
        {
		    if ( p_vci_initiator.cmdack.read() ) r_cmd_fsm = CMD_IDLE;
            break;
        }
        ///////////////////
	    case CMD_LOCK_READ:    // send read lock command (one flit)
        {
		    if ( p_vci_initiator.cmdack.read() ) r_cmd_fsm = CMD_IDLE;
            break;
	    }
        /////////////////////
        case CMD_STATUS_READ:  // send read command for status (one flit) 
        {
		    if ( p_vci_initiator.cmdack.read() ) r_cmd_fsm = CMD_IDLE;
            break;
        }
        /////////////////////////
	    case CMD_STATUS_UPDT_STS: // send first flit: update STS
        {
		    if ( p_vci_initiator.cmdack.read() ) r_cmd_fsm = CMD_STATUS_UPDT_PTR;
            break;
        }
        /////////////////////////
	    case CMD_STATUS_UPDT_PTR: // send second flit: update PTR
        {
		    if ( p_vci_initiator.cmdack.read() ) r_cmd_fsm = CMD_STATUS_UPDT_PTW;
            break;
        }
        /////////////////////////
	    case CMD_STATUS_UPDT_PTW: // send third flit: update PTW
        {
		    if ( p_vci_initiator.cmdack.read() ) r_cmd_fsm = CMD_IDLE;
            break;
        }
        ///////////////////
        case CMD_DATA_READ:    // send read data command (one flit)
        {
		    if ( p_vci_initiator.cmdack.read() ) r_cmd_fsm = CMD_IDLE;
            break;
        }
        ////////////////////
        case CMD_DATA_WRITE:    // send write data command (m_burst_size/4 flits)
        {
		    if ( p_vci_initiator.cmdack.read() )
            {
                size_t k = r_cmd_k.read();

                assert( r_channel[k].fifo->rok() and
                        "Hardware fifo should not be empty in a Data Write transaction");

                vci_fifo_get = true;
                if ( r_cmd_word.read() == (m_burst_size>>2) - 1 ) r_cmd_fsm = CMD_IDLE;
                else    r_cmd_word = r_cmd_word.read() + 1;
            }
            break;
        }
    } // end switch r_cmd_fsm   

    ////////////////////////////////////////////////////////////////////////////////////////
    // This RSP_FSM control the responses on the VCI initiator port.
    // It analyses the RTRDID VCI field to get the channel index k, and computes
    // the VCI transaction type from the channel[k] FSM state.
    ////////////////////////////////////////////////////////////////////////////////////////
	switch ( r_rsp_fsm.read() ) 
    {
        //////////////
	    case RSP_IDLE:     // Decode valid response for all channels,
                           // but does not consume the VCI flit in this state. 
        {
            if ( p_vci_initiator.rspval.read() )
            {
                // get channel index from VCI TRDID
                uint32_t k = (uint32_t)p_vci_initiator.rtrdid.read();
			    r_rsp_k    = k;

                // get transaction type from channel FSM state
                uint32_t state        = r_channel[k].fsm;
                bool     is_to_coproc = (r_channel[k].way == TO_COPROC);

                if      ( state == CHANNEL_MWMR_TICKET_READ ) 
                    r_rsp_fsm = RSP_TICKET_READ;
                else if ( state == CHANNEL_MWMR_TICKET_CAS ) 
                    r_rsp_fsm = RSP_TICKET_CAS;
                else if ( state == CHANNEL_MWMR_LOCK_READ )
                    r_rsp_fsm = RSP_LOCK_READ;
                else if ( state == CHANNEL_MWMR_STATUS_READ )
                    r_rsp_fsm = RSP_STATUS_READ_STS;
                else if ( state == CHANNEL_MWMR_STATUS_UPDT )
                    r_rsp_fsm = RSP_STATUS_UPDT;
                else if ( state == CHANNEL_MWMR_LOCK_RELEASE ) 
                    r_rsp_fsm = RSP_LOCK_RELEASE;
                else if ( (state == CHANNEL_MWMR_DATA_MOVE) and is_to_coproc )
                    r_rsp_fsm = RSP_DATA_READ;
                else if ( (state == CHANNEL_MWMR_DATA_MOVE) and not is_to_coproc ) 
                    r_rsp_fsm = RSP_DATA_WRITE;
                else if ( (state == CHANNEL_DMA_DATA_MOVE) and is_to_coproc )
                    r_rsp_fsm = RSP_DATA_READ;
                else if ( (state == CHANNEL_DMA_DATA_MOVE) and not is_to_coproc ) 
                    r_rsp_fsm = RSP_DATA_WRITE;
                else
                    assert( false and
                    "VCI_MWMR_DMA error: unexpected VCI response");
   
                // initializes flit counter
                r_rsp_word = 0;
            } 
            break;
        }
        /////////////////////
	    case RSP_TICKET_READ:    // set the ticket value and signal completion
        {
		    if ( p_vci_initiator.rspval.read() )
            {
                assert( p_vci_initiator.reop.read() and
                       "VCI_MWMR_DMA error: TICKET_READ VCI response should be one flit");
 
                size_t k = r_rsp_k.read();

                r_channel[k].response = true;      
                r_channel[k].rerror   = (bool)p_vci_initiator.rerror.read();
                r_channel[k].ticket   = p_vci_initiator.rdata.read();
                r_rsp_fsm             = RSP_IDLE;
            }
		    break;
        }
        ////////////////////
	    case RSP_TICKET_CAS:   // set the data value and signal completion 
        {
		    if ( p_vci_initiator.rspval.read() )
            {
                assert( p_vci_initiator.reop.read() and
                       "VCI_MWMR_DMA error: TICKET_CAS VCI response should be one flit");
 
                size_t k = r_rsp_k.read();

                r_channel[k].response = true;      
                r_channel[k].rerror   = (bool)p_vci_initiator.rerror.read();
                r_channel[k].data     = (p_vci_initiator.rdata.read() == 0);
                r_rsp_fsm             = RSP_IDLE;
            }
		    break;
        }
        ///////////////////
	    case RSP_LOCK_READ:    // set the data value, and signal completion 
        {
		    if ( p_vci_initiator.rspval.read() )
            {
                assert( p_vci_initiator.reop.read() and
                       "VCI_MWMR_DMA error: LOCK_READ VCI response should be one flit");
 
                size_t k = r_rsp_k.read();

                r_channel[k].response = true;      
                r_channel[k].rerror   = (bool)p_vci_initiator.rerror.read();
                r_channel[k].data     = (p_vci_initiator.rdata.read() == 0);
                r_rsp_fsm             = RSP_IDLE;
            }
            break;
        }
        /////////////////////////
        case RSP_STATUS_READ_STS:  // set the sts value 
        {
		    if ( p_vci_initiator.rspval.read() )
            {
                assert( not p_vci_initiator.reop.read() and
                        "VCI_MWMR_DMA error: STATUS_READ VCI response should have 3 flits");

                size_t k = r_rsp_k.read();

                if ( p_vci_initiator.rerror.read() )  // error reported
                {
                    if ( p_vci_initiator.reop.read() )  // last flit
                    {
                        r_channel[k].rerror   = true;
                        r_channel[k].response = true;
                        r_rsp_fsm             = RSP_IDLE;
                    }
                    else
                    {
                        assert( false and
                        "VCI_MWMR_DMA error: illegal VCI error response format");
                    }
                }
                else                                 // no error reported
                {    
                    r_channel[k].sts = p_vci_initiator.rdata.read();
                    r_rsp_fsm        = RSP_STATUS_READ_PTR;
                }
            }
            break;
        }
        /////////////////////////
        case RSP_STATUS_READ_PTR:  // set the ptr value 
        {
		    if ( p_vci_initiator.rspval.read() )
            {
                assert( not p_vci_initiator.reop.read() and
                        "VCI_MWMR_DMA error: STATUS_READ VCI response should have 3 flits");

                size_t k = r_rsp_k.read();

                if ( p_vci_initiator.rerror.read() )  // error reported
                {
                    if ( p_vci_initiator.reop.read() )  // last flit
                    {
                        r_channel[k].rerror   = true;
                        r_channel[k].response = true;
                        r_rsp_fsm             = RSP_IDLE;
                    }
                    else
                    {
                        assert( false and
                        "VCI_MWMR_DMA error: illegal VCI error response format");
                    }
                }
                else                                 // no error reported
                {    
                    r_channel[k].ptr = p_vci_initiator.rdata.read();
                    r_rsp_fsm        = RSP_STATUS_READ_PTW;
                }
            }
            break;
        }
        /////////////////////////
        case RSP_STATUS_READ_PTW:  // set ptw value and signal completion
        {
		    if ( p_vci_initiator.rspval.read() )
            {
                assert(  p_vci_initiator.reop.read() and
                        "VCI_MWMR_DMA error: STATUS_READ VCI response should have 3 flits");

                size_t k = r_rsp_k.read();

                if ( p_vci_initiator.rerror.read() )  // error reported
                {
                    if ( p_vci_initiator.reop.read() )  // last flit
                    {
                        r_channel[k].rerror   = true;
                        r_channel[k].response = true;
                        r_rsp_fsm             = RSP_IDLE;
                    }
                    else
                    {
                        assert( false and
                        "VCI_MWMR_DMA error: illegal error respons format");
                    }
                }
                else                                 // no error reported
                { 
                    r_channel[k].ptw      = p_vci_initiator.rdata.read();
                    r_channel[k].response = true;      
                    r_channel[k].rerror   = false;
                    r_rsp_fsm             = RSP_IDLE;
                }
            }
            break;
        }
        /////////////////////
	    case RSP_STATUS_UPDT:    // signal completion
        {
		    if ( p_vci_initiator.rspval.read() )
            {
                assert( p_vci_initiator.reop.read() and
                       "VCI_MWMR_DMA error: Write Status VCI response should be one flit");
 
                size_t k = r_rsp_k.read();

                r_channel[k].response = true;
                r_channel[k].rerror   = (bool)p_vci_initiator.rerror.read();
                r_rsp_fsm             = RSP_IDLE;
            }
            break;
        }
        ///////////////////
        case RSP_DATA_READ:    // get data and write into FIFO 
        {
		    if ( p_vci_initiator.rspval.read() )
            {
                size_t k = r_rsp_k.read();

                assert( r_channel[k].fifo->wok() and
                        "VCI_MWMR_DMA error: Hardware fifo should not be full");


                if ( p_vci_initiator.rerror.read() )  // error reported
                {
                    r_channel[k].rerror = true;
                    if ( p_vci_initiator.reop.read() )  // last flit
                    {
                        r_channel[k].response = true;
                        r_rsp_fsm             = RSP_IDLE;
                    }
                        
                }
                else                                  // no error reported
                {
                    vci_fifo_put   = true;
                    vci_fifo_wdata = p_vci_initiator.rdata.read();
                    if ( r_rsp_word.read() + 1 == (m_burst_size>>2) )  // last flit
                    {
                        assert( p_vci_initiator.reop.read() and
                                "VCI_MWMR_DMA error: wrong READ_DATA VCI response");

                        r_channel[k].rerror   = false;
                        r_channel[k].response = true;
                        r_rsp_fsm             = RSP_IDLE;
                    }
                    else
                    {
                        assert( not p_vci_initiator.reop.read() and
                                "VCI_MWMR_DMA error: wrong READ_DATA VCI  response");

                        r_rsp_word = r_rsp_word.read() + 1;
                    }
                }
            }
            break;
        }
        ////////////////////
        case RSP_DATA_WRITE:    // signal completion
        {
		    if ( p_vci_initiator.rspval.read() )
            {
                assert( p_vci_initiator.reop.read() and
                       "VCI_MWMR_DMA error: wrong WRITE_DATA VCI response");
 
                size_t k = r_rsp_k.read();

                r_channel[k].response = true;
                r_channel[k].rerror   = (bool)p_vci_initiator.rerror.read();
                r_rsp_fsm             = RSP_IDLE;
            }
            break;
        }
    } // end switch r_rsp_fsm   

    ////////////////////////////////////////////////////////////////////////////////////
    // Each CHANNEL_FSM[k] controls a different channel.
    // The behaviour depends on the mode: MWMR / DMA_IRQ / DMA_NO_IRQ
    // As the MWMR software channel is protected by a ticket-based spin-lock,
    // it uses a CAS VCI transaction to take a ticket with atomic increment.
    // A MWMR transaction is split in (at least) 7 VCI requests to the CMD_FSM.
    // corresponding to 7 VCI transactions.
    // As the VCI commands and responses are handled by two separated FSMs,
    // the VCI transactions corresponding to different MWMR channels are interleaved,
    // and there is up to (m_all_channels) simultaneous VCI transactions. 
    // If there is no data (or no space) in the MWMR channel a retry_delay is set.
    // The synchronisation between channel_fsm[k] and cmd_fsm / rsp_fsm is done by
    // the set/reset flip-flops r_channel[k].request. It is set by the channel_fsm, 
    // and it is reset by the rsp_fsm.
    ////////////////////////////////////////////////////////////////////////////////////

    for ( size_t k=0 ; k<m_all_channels ; k++ )
    {            
        bool  is_to_coproc = (r_channel[k].way == TO_COPROC);

        switch( r_channel[k].fsm )
        { 
            //////////////////
            case CHANNEL_IDLE:      // wait a coprocessor request
		    {
                bool req = false;
                if ( is_to_coproc )  req = p_to_coproc[k].req;
                else  req = p_from_coproc[k - m_to_coproc_channels].req;
 
                if ( r_channel[k].running and req )
                {
                    if ( is_to_coproc ) 
                    {
                        r_channel[k].bursts = p_to_coproc[k].bursts;
                    }
                    else 
                    {
                        r_channel[k].bursts = p_from_coproc[k - m_to_coproc_channels].bursts;
                    }
                    r_channel[k].fsm = CHANNEL_ACK;
                }
                break;
            }
            /////////////////
            case CHANNEL_ACK:   // acknowledge coprocessor request
                                // makes first request to CMD_FSM
            {
                if ( r_channel[k].mode == MODE_MWMR ) // MWMR_TICKET_READ VCI transaction
                {
                    r_channel[k].reqtype = TICKET_READ;
                    r_channel[k].request = true;
                    r_channel[k].fsm     = CHANNEL_MWMR_TICKET_READ;
                }
                else if ( is_to_coproc )              // DMA_DATA_READ VCI transaction 
                {
                    r_channel[k].reqtype = DATA_READ;
                    r_channel[k].request = true;
                    r_channel[k].fsm     = CHANNEL_DMA_DATA_MOVE;
                }
                else                                  // DMA_DATA_WRITE VCI transaction 
                {
                    r_channel[k].reqtype = DATA_WRITE;
                    r_channel[k].request = true;
                    r_channel[k].fsm     = CHANNEL_DMA_DATA_MOVE;
                }
                    
                break;
            }
            ///////////////////////////
            case CHANNEL_DMA_DATA_MOVE:    // wait response for DMA_DATA VCI transaction
            {
                if ( r_channel[k].response == true )
                {
                    if ( r_channel[k].rerror == false )  // No error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;

                        // update number of bytes to be transfered and number of bursts
                        r_channel[k].buffer_size = r_channel[k].buffer_size - m_burst_size;
                        r_channel[k].bursts      = r_channel[k].bursts - 1;

                        // define next step
                        if ( r_channel[k].bursts )           // coproc request not completed
                        {
                            if ( is_to_coproc )
                            {
                               r_channel[k].reqtype = DATA_READ;
                               r_channel[k].request = true;
                               r_channel[k].ptr = r_channel[k].ptr + (m_burst_size>>2);
                            }
                            else
                            {
                               r_channel[k].reqtype = DATA_WRITE;
                               r_channel[k].request = true;
                               r_channel[k].ptw = r_channel[k].ptw + (m_burst_size>>2);
                            }
                        }
                        else if (r_channel[k].buffer_size )  // DMA transfert not completed
                        {
                            if ( is_to_coproc )
                            {
                                r_channel[k].ptr = r_channel[k].ptr + (m_burst_size>>2);
                                r_channel[k].fsm = CHANNEL_IDLE;
                            }
                            else
                            {
                                r_channel[k].ptw = r_channel[k].ptw + (m_burst_size>>2);
                                r_channel[k].fsm = CHANNEL_IDLE;
                            }
                        }
                        else                                 // DMA transfer completed
                        {
                            r_channel[k].ptr = 0;
                            r_channel[k].ptw = 0;
                            r_channel[k].fsm = CHANNEL_SUCCESS;
                        }
                    }
                    else                              // error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
                        r_channel[k].rerror   = false;
                        r_channel[k].ptr      = 0;
                        r_channel[k].ptw      = 0;
                        r_channel[k].fsm    = CHANNEL_ERROR_DATA;
                    }
                }
                break;
            }
            /////////////////////////
            case CHANNEL_SUCCESS:      // wait soft reset
            case CHANNEL_ERROR_LOCK:   // wait soft reset
            case CHANNEL_ERROR_DESC:   // wait soft reset
            case CHANNEL_ERROR_DATA:   // wait soft reset
            {
                if ( r_channel[k].running == false ) r_channel[k].fsm = CHANNEL_IDLE;
                break;
            }
            //////////////////////////////
            case CHANNEL_MWMR_TICKET_READ:   // wait a response to TICKET_READ VCI transaction
                                             // makes a TICKET_CAS request
            {
                if ( r_channel[k].response == true )  // response available in ticket
                {
                    if ( r_channel[k].rerror == false )   // No error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;

                        r_channel[k].reqtype = TICKET_CAS;
                        r_channel[k].request = true;
                        r_channel[k].fsm     = CHANNEL_MWMR_TICKET_CAS;
                    }
                    else                                  // error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
                        r_channel[k].rerror   = false;
                        r_channel[k].fsm     = CHANNEL_ERROR_LOCK;
                    }
                }
                break;
            }
            /////////////////////////////
            case CHANNEL_MWMR_TICKET_CAS:    // wait a response to a TICKET_CAS VCI transaction
                                             // - makes a LOCK_READ if atomic CAS
                                             // - retry a TICKET_READ if not atomic
            {
                if ( r_channel[k].response == true )   // response available in data
                {
                    if ( r_channel[k].rerror == false )   // No error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;

                        if ( r_channel[k].data == 0 )      // success
                        {
                            r_channel[k].reqtype = LOCK_READ;
                            r_channel[k].request = true;
                            r_channel[k].fsm     = CHANNEL_MWMR_LOCK_READ;
                        }
                        else                                   // failure
                        {
                            r_channel[k].reqtype = TICKET_READ;
                            r_channel[k].request = true;
                            r_channel[k].fsm     = CHANNEL_MWMR_TICKET_READ;
                        }
                    }
                    else                                  // error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
                        r_channel[k].rerror   = false;
                        r_channel[k].fsm     = CHANNEL_ERROR_LOCK;
                    }
                }
                break;
            }
            ////////////////////////////
            case CHANNEL_MWMR_LOCK_READ:    // wait a response to a LOCK_READ request
                                            // - makes a STATUS_READ if ticket == rdata
                                            // - retry a LOCK_READ else
            {
                if ( r_channel[k].response == true )
                {
                    if ( r_channel[k].rerror == false )   // No error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;

                        if ( r_channel[k].data == r_channel[k].ticket ) // lock available 
                        {
                            r_channel[k].reqtype = STATUS_READ;           
                            r_channel[k].request = true;
                            r_channel[k].fsm     = CHANNEL_MWMR_STATUS_READ;
                        }
                        else                                           // lock not available yet
                        {                            
                            r_channel[k].reqtype = LOCK_READ;
                            r_channel[k].request = true;
                            r_channel[k].fsm     = CHANNEL_MWMR_LOCK_READ;
                        }
                    }
                    else                                  // error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
                        r_channel[k].rerror   = false;
                        r_channel[k].fsm     = CHANNEL_ERROR_LOCK;
                    }
                }
                break;
            }
            //////////////////////////////
            case CHANNEL_MWMR_STATUS_READ:   // wait a response to a STATUS_READ request
                                             // (stored by rsp_fsm in ptr, pts, sts).
                                             // - makes a MWMR_DATA request if transfer possible
                                             // - makes a LOCK_RELEASE request if not possible
            {
                if ( r_channel[k].response == true )
                {
                    if ( r_channel[k].rerror == false )   // No error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;

                        if ( r_channel[k].way == FROM_COPROC ) // FROM_COPROC 
                        {
                            // space in memory buffer must be enough for coprocessor request
                            if ( r_channel[k].buffer_size - (r_channel[k].sts<<2) >=
                                 r_channel[k].bursts * m_burst_size )
                            {
                                r_channel[k].reqtype = DATA_WRITE;
                                r_channel[k].request = true;
                                r_channel[k].fsm     = CHANNEL_MWMR_DATA_MOVE;
                            }
                            else
                            {
                                r_channel[k].reqtype = LOCK_RELEASE;
                                r_channel[k].request = true;
                                r_channel[k].fsm     = CHANNEL_MWMR_LOCK_RELEASE;
                            }
                        }
                        else                                   // TO_COPROC 
                        {
                            // data in memory buffer must be enough for coprocessor request
                            if ( (r_channel[k].sts<<2) >=
                                 r_channel[k].bursts * m_burst_size )
                            {
                                r_channel[k].reqtype = DATA_READ;
                                r_channel[k].request = true;
                                r_channel[k].fsm     = CHANNEL_MWMR_DATA_MOVE;
                            }
                            else
                            {
                                r_channel[k].reqtype = LOCK_RELEASE;
                                r_channel[k].request = true;
                                r_channel[k].fsm     = CHANNEL_MWMR_LOCK_RELEASE;
                            }
                        }
                    }
                    else                                // error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
                        r_channel[k].rerror   = false;
                        r_channel[k].fsm     = CHANNEL_ERROR_DESC;
                    }
                }
                break;
            }
            ////////////////////////////
            case CHANNEL_MWMR_DATA_MOVE:    // wait response to a DATA_MOVE VCI transaction
                                            // and makes a STATUS_UPDT request
            {
                if ( r_channel[k].response == true )  // response available
                {
                    if ( r_channel[k].rerror == false )   // No error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
 
                        // update number of bursts for the current coprocessor request
                        r_channel[k].bursts      = r_channel[k].bursts - 1;

                        // update MWMR fifo local status (ptr, ptw, sts)
                        if ( is_to_coproc )
                        {
                            r_channel[k].ptr = (r_channel[k].ptr + (m_burst_size>>2)) %
                                               (r_channel[k].buffer_size>>2);
                            r_channel[k].sts = r_channel[k].sts - (m_burst_size>>2); 
                        }
                        else
                        {
                            r_channel[k].ptw = (r_channel[k].ptw + (m_burst_size>>2)) %
                                               (r_channel[k].buffer_size>>2);
                            r_channel[k].sts = r_channel[k].sts + (m_burst_size>>2); 
                        }

                        // compute next state
                        if ( r_channel[k].bursts )                   // coproc request not completed
                        {
                            if ( is_to_coproc )  r_channel[k].reqtype = DATA_READ;
                            else                 r_channel[k].reqtype = DATA_WRITE;
                            r_channel[k].request = true;
                            r_channel[k].fsm     = CHANNEL_MWMR_DATA_MOVE;
                        }
                        else                                        // request STATUS_UPDT
                        {
                            r_channel[k].reqtype = STATUS_UPDT;
                            r_channel[k].request = true;
                            r_channel[k].fsm     = CHANNEL_MWMR_STATUS_UPDT;
                        }
                    }
                    else                                // error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
                        r_channel[k].rerror   = false;
                        r_channel[k].fsm     = CHANNEL_ERROR_DATA;
                    }
                }
                break;
            }
            //////////////////////////////
            case CHANNEL_MWMR_STATUS_UPDT:   // wait response to STATUS_UPDT VCI transaction
                                             // and makes a LOCK_RELEASE request
            {
                if ( r_channel[k].response == true )  
                {
                    if ( r_channel[k].rerror == false )   // No error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
                        r_channel[k].reqtype = LOCK_RELEASE;
                        r_channel[k].request = true;
                        r_channel[k].fsm     = CHANNEL_MWMR_LOCK_RELEASE;
                    }
                    else                                  // error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
                        r_channel[k].rerror   = false;
                        r_channel[k].fsm     = CHANNEL_ERROR_DESC;
                    }
                }
                break;
            }
            ///////////////////////////////
            case CHANNEL_MWMR_LOCK_RELEASE:   // wait response to LOCK_RELEASE VCI transaction
                                              // and return to IDLE state
            {
                if ( r_channel[k].response == true )
                {
                    if ( r_channel[k].rerror == false )   // No error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;

                        r_channel[k].fsm = CHANNEL_IDLE;
                    }
                    else                                  // error reported
                    {
                        // reset synchro
                        r_channel[k].response = false;
                        r_channel[k].rerror   = false;
                        r_channel[k].fsm     = CHANNEL_ERROR_LOCK;
                    }
                }
                break;
            }
        } // end switch r_cmd_fsm
    } // end for channels

    ////////////////////////////////////////////////////////////////////////////////
    // update TO_COPROC FIFOs 
    // - On the coprocessor side, the get command can be true for all channels. 
    // - On the VCI side, the put command can only be true for the r_rsp_k channel.   
    ////////////////////////////////////////////////////////////////////////////////
    for ( size_t i = 0; i < m_to_coproc_channels; ++i ) 
    {
        size_t k = i;  // ports and Fifos have same index 

        bool coproc_fifo_get = p_to_coproc[i].wok.read();

        if ( k == r_rsp_k.read() )
        { 
            r_channel[k].fifo->update( coproc_fifo_get, 
                                       vci_fifo_put , 
                                       vci_fifo_wdata );
        }
        else if ( coproc_fifo_get )
        {
            r_channel[k].fifo->simple_get();
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // update FROM_COPROC FIFOs  
    // - On the coprocessor side, the put command can be true for all channels. 
    // - On the VCI side, the get command can only be true for the r_cm_k channel.
    ////////////////////////////////////////////////////////////////////////////////
    for ( size_t i = 0; i < m_from_coproc_channels; ++i ) 
    {
        bool     coproc_fifo_put   = p_from_coproc[i].rok.read();
        uint32_t coproc_fifo_wdata = p_from_coproc[i].data.read();

        uint32_t k = i + m_to_coproc_channels;   // Ports and Fifos have different index

        if ( k == r_cmd_k.read() ) 
        {
            r_channel[k].fifo->update( vci_fifo_get , 
                                       coproc_fifo_put , 
                                       coproc_fifo_wdata );
        }
        else if ( coproc_fifo_put ) 
        {
            r_channel[k].fifo->simple_put( coproc_fifo_wdata );
        }
    }

}  // end transition

//////////////////////
tmpl(void)::genMoore()
{
    // assert IRQ if (all channels completed) AND ((one error detected) OR (MODE_DMA_IRQ))
    bool completed = true;
    bool error     = false;
    for ( size_t k = 0 ; k < m_all_channels ; k++ )
    {
        completed = completed and ( (r_channel[k].fsm == CHANNEL_SUCCESS)    or
                                    (r_channel[k].fsm == CHANNEL_ERROR_LOCK) or
                                    (r_channel[k].fsm == CHANNEL_ERROR_DESC) or
                                    (r_channel[k].fsm == CHANNEL_ERROR_DATA) );

        error = error or ( (r_channel[k].fsm == CHANNEL_ERROR_LOCK) or
                           (r_channel[k].fsm == CHANNEL_ERROR_DESC) or
                           (r_channel[k].fsm == CHANNEL_ERROR_DATA) );
    }
    p_irq = completed and ( error or (r_channel[0].mode == MODE_DMA_IRQ) );

    // VCI target port
    if ( r_tgt_fsm.read() == TGT_IDLE )
    {
        p_vci_target.cmdack = true;
        p_vci_target.rspval = false;
    }
    else
    {
        p_vci_target.cmdack = false;
        p_vci_target.rspval = true;
        p_vci_target.rerror = 0;
        p_vci_target.reop   = true;
        p_vci_target.rsrcid = r_tgt_srcid;
        p_vci_target.rtrdid = r_tgt_trdid;
        p_vci_target.rpktid = r_tgt_pktid;
        if ( r_tgt_fsm.read() == TGT_READ ) p_vci_target.rdata = r_tgt_data;
        else                                p_vci_target.rdata = 0;
    }

    // VCI initiator port response
    // We consume the VCI flit in all other states than the IDLE state
	if ( r_rsp_fsm.read() == RSP_IDLE ) p_vci_initiator.rspack = false;
    else                                p_vci_initiator.rspack = true;
    
    // VCI initiator port command
	p_vci_initiator.contig  = true;
	p_vci_initiator.cons    = 0;
	p_vci_initiator.wrap    = 0;
	p_vci_initiator.cfixed  = true;
	p_vci_initiator.clen    = 0;
	p_vci_initiator.srcid   = m_srcid;
	p_vci_initiator.be      = 0xf;

	if ( r_cmd_fsm.read() == CMD_IDLE ) 
    {
        p_vci_initiator.cmdval = false;
    }
    else if ( r_cmd_fsm.read() == CMD_TICKET_READ )
    {
        p_vci_initiator.cmdval  = true;
        p_vci_initiator.address = r_channel[r_cmd_k.read()].lock_paddr + 4;  // &lock.free
        p_vci_initiator.cmd     = vci_param::CMD_READ;
        p_vci_initiator.wdata   = 0;
		p_vci_initiator.eop     = true;
        p_vci_initiator.plen    = 4;
        p_vci_initiator.trdid   = r_cmd_k.read();
	    p_vci_initiator.pktid   = PKTID_READ_DATA_UNC;
    }
    else if ( (r_cmd_fsm.read() == CMD_TICKET_CAS_OLD) or 
              (r_cmd_fsm.read() == CMD_TICKET_CAS_NEW) )
    {
        p_vci_initiator.cmdval  = true;
        p_vci_initiator.address = r_channel[r_cmd_k.read()].lock_paddr + 4;  // &lock.free
        p_vci_initiator.cmd     = vci_param::CMD_STORE_COND;
        if ( r_cmd_fsm.read() == CMD_TICKET_CAS_OLD ) 
        {
            p_vci_initiator.wdata = r_channel[r_cmd_k.read()].ticket;
            p_vci_initiator.eop   = false;
        }
        else
        {
            p_vci_initiator.wdata = r_channel[r_cmd_k.read()].ticket + 1;
            p_vci_initiator.eop   = true;
        }
        p_vci_initiator.plen    = 8;
        p_vci_initiator.trdid   = r_cmd_k.read();
	    p_vci_initiator.pktid   = PKTID_CAS;
    }
    else if ( r_cmd_fsm.read() == CMD_LOCK_READ )
    {
        p_vci_initiator.cmdval  = true;
        p_vci_initiator.address = r_channel[r_cmd_k.read()].lock_paddr + 4;  // &lock.current
        p_vci_initiator.cmd     = vci_param::CMD_READ;
        p_vci_initiator.wdata   = 0;
		p_vci_initiator.eop     = true;
        p_vci_initiator.plen    = 4;
        p_vci_initiator.trdid   = r_cmd_k.read();
	    p_vci_initiator.pktid   = PKTID_READ_DATA_UNC;
    }
    else if ( r_cmd_fsm.read() == CMD_STATUS_READ )
    {
        p_vci_initiator.cmdval  = true;
        p_vci_initiator.address = r_channel[r_cmd_k.read()].desc_paddr;  // &mwmr.sts
        p_vci_initiator.cmd     = vci_param::CMD_READ;
        p_vci_initiator.wdata   = 0;
		p_vci_initiator.eop     = true;
        p_vci_initiator.plen    = 12;                               // 3 flits
        p_vci_initiator.trdid   = r_cmd_k.read();
	    p_vci_initiator.pktid   = PKTID_READ_DATA_UNC;
    }
    else if ( r_cmd_fsm.read() == CMD_STATUS_UPDT_STS ) 
    {
        p_vci_initiator.cmdval  = true;
        p_vci_initiator.address = r_channel[r_cmd_k.read()].desc_paddr;  // &mwmr.sts
        p_vci_initiator.cmd     = vci_param::CMD_WRITE;
        p_vci_initiator.wdata   = r_channel[r_cmd_k.read()].sts;
		p_vci_initiator.eop     = false;
        p_vci_initiator.plen    = 12;
        p_vci_initiator.trdid   = r_cmd_k.read();
	    p_vci_initiator.pktid   = PKTID_WRITE;
    }
    else if ( r_cmd_fsm.read() == CMD_STATUS_UPDT_PTR ) 
    {
        p_vci_initiator.cmdval  = true;
        p_vci_initiator.address = r_channel[r_cmd_k.read()].desc_paddr + 4;  // &mwmr.ptr
        p_vci_initiator.cmd     = vci_param::CMD_WRITE;
        p_vci_initiator.wdata   = r_channel[r_cmd_k.read()].ptr;
		p_vci_initiator.eop     = false;
        p_vci_initiator.plen    = 12;
        p_vci_initiator.trdid   = r_cmd_k.read();
	    p_vci_initiator.pktid   = PKTID_WRITE;
    }
    else if ( r_cmd_fsm.read() == CMD_STATUS_UPDT_PTW ) 
    {
        p_vci_initiator.cmdval  = true;
        p_vci_initiator.address = r_channel[r_cmd_k.read()].desc_paddr + 8;  // &mwmr.ptw
        p_vci_initiator.cmd     = vci_param::CMD_WRITE;
        p_vci_initiator.wdata   = r_channel[r_cmd_k.read()].ptw;
		p_vci_initiator.eop     = true;
        p_vci_initiator.plen    = 12;
        p_vci_initiator.trdid   = r_cmd_k.read();
	    p_vci_initiator.pktid   = PKTID_WRITE;
    }
    else if ( r_cmd_fsm.read() == CMD_DATA_READ )
    {
        p_vci_initiator.cmdval  = true;
        p_vci_initiator.address = r_channel[r_cmd_k.read()].buffer_paddr + 
                                  (r_channel[r_cmd_k.read()].ptr<<2); 
        p_vci_initiator.cmd     = vci_param::CMD_READ;
        p_vci_initiator.wdata   = 0;
		p_vci_initiator.eop     = true;
        p_vci_initiator.plen    = m_burst_size;
        p_vci_initiator.trdid   = r_cmd_k.read();
	    p_vci_initiator.pktid   = PKTID_READ_DATA_UNC;
    }
    else if ( r_cmd_fsm.read() == CMD_DATA_WRITE )
    {
        p_vci_initiator.cmdval  = true;
        p_vci_initiator.address = r_channel[r_cmd_k.read()].buffer_paddr + 
                                  (r_channel[r_cmd_k.read()].ptw<<2) + 
                                  (r_cmd_word.read()<<2); 
        p_vci_initiator.cmd     = vci_param::CMD_WRITE;
        p_vci_initiator.wdata   = r_channel[r_cmd_k.read()].fifo->read();
        if ( r_cmd_word.read() == ((m_burst_size>>2)-1) ) p_vci_initiator.eop   = true;
        else                                              p_vci_initiator.eop   = false;
        p_vci_initiator.plen    = m_burst_size;
        p_vci_initiator.trdid   = r_cmd_k.read();
	    p_vci_initiator.pktid   = PKTID_WRITE;
    }

    //  TO_COPROC ports
    for ( size_t i = 0 ; i < m_to_coproc_channels ; ++i )
    {
        size_t k = i;
        p_to_coproc[i].w    = r_channel[k].fifo->rok();
        p_to_coproc[i].data = r_channel[k].fifo->read();
        p_to_coproc[i].ack  = (r_channel[k].fsm == CHANNEL_ACK);
    }

    // FROM_COPROC ports
    for ( size_t i = 0 ; i < m_from_coproc_channels; ++i ) 
    {
        size_t k = i + m_to_coproc_channels;
        p_from_coproc[i].r   = r_channel[k].fifo->wok();
        p_from_coproc[i].ack = (r_channel[k].fsm == CHANNEL_ACK);
    }

    //  Coprocessor configuration ports
    for ( size_t i = 0; i<m_coproc_config_regs; i++)
    {
        p_config[i] = r_coproc_config[i];
    }

} // end genMoore()

/////////////////////////
tmpl(void)::print_trace()
{
    const char *channel_states[] = 
    {
    "CHANNEL_IDLE",
    "CHANNEL_ACK",
    "CHANNEL_DMA_DATA_MOVE",
    "CHANNEL_MWMR_TICKET_READ",
    "CHANNEL_MWMR_TICKET_CAS",
    "CHANNEL_MWMR_LOCK_READ",
    "CHANNEL_MWMR_STATUS_READ",
    "CHANNEL_MWMR_STATUS_UPDT",
    "CHANNEL_MWMR_LOCK_RELEASE",
    "CHANNEL_MWMR_DATA_MOVE",
    "CHANNEL_SUCCESS",
    "CHANNEL_ERROR_LOCK",
    "CHANNEL_ERROR_DESC",
    "CHANNEL_ERROR_DATA",
    };

    const char *cmd_states[] = 
    {
	"CMD_IDLE",
	"CMD_TICKET_READ",
	"CMD_TICKET_CAS_OLD",
	"CMD_TICKET_CAS_NEW",
	"CMD_LOCK_READ",
	"CMD_STATUS_READ",
	"CMD_STATUS_WRITE_STS",
	"CMD_STATUS_WRITE_PTR",
	"CMD_STATUS_WRITE_PTW",
	"CMD_LOCK_RELEASE",
	"CMD_DATA_WRITE",
	"CMD_DATA_READ",
    };

    const char *rsp_states[] = 
    {
	"RSP_IDLE",
	"RSP_TICKET_READ",
	"RSP_TICKET_CAS",
	"RSP_LOCK_READ",
	"RSP_STATUS_READ_STS",
	"RSP_STATUS_READ_PTR",
	"RSP_STATUS_READ_PTW",
	"RSP_STATUS_UPDT",
	"RSP_LOCK_RELEASE",
	"RSP_DATA_WRITE",
	"RSP_DATA_READ",
    };

    const char *tgt_states[] =
    {
    "TGT_IDLE",
    "TGT_READ",
    "TGT_WRITE",
    };

    std::cout << "MWMR " << name() << " : "
              << tgt_states[r_tgt_fsm.read()] << " / "
              << cmd_states[r_cmd_fsm.read()] << " / "
              << rsp_states[r_rsp_fsm.read()] << std::endl;
    for ( size_t k = 0 ; k < m_all_channels ; k++ )
    {
        std::cout <<"  CHANNEL[" << std::dec << k << "] : "
                  << channel_states[r_channel[k].fsm.read()] 
                  << " / way = " << r_channel[k].way 
                  << " / mode = " << r_channel[k].mode 
                  << " / run = " << r_channel[k].running << std::hex
                  << " / buffer = " << r_channel[k].buffer_paddr
                  << " / desc = " << r_channel[k].desc_paddr
                  << " / lock = " << r_channel[k].lock_paddr
                  << std::endl;
    }
}  // end print_trace()

/////////////////////////////////////////////////
tmpl(/**/)::VciMwmrDma( sc_module_name      name,
                      const MappingTable  &mt,
                      const IntTab        &srcid,
                      const IntTab        &tgtid,
	                  const size_t        n_to_coproc,
	                  const size_t        n_from_coproc,
	                  const size_t        n_config,
	                  const size_t        n_status,
                      const size_t        burst_size )
		   : caba::BaseModule(name),

           m_srcid( mt.indexForId(srcid) ),
           m_segment( mt.getSegment(tgtid) ),
           m_to_coproc_channels( n_to_coproc ),
           m_from_coproc_channels( n_from_coproc ),
           m_all_channels( n_to_coproc + n_from_coproc ),
           m_coproc_config_regs( n_config ),
           m_coproc_status_regs( n_status ),
           m_burst_size( burst_size ),

           r_channel(new channel_state_t[m_all_channels]),

           r_cmd_fsm("r_init_fsm"),
           r_cmd_word("r_cmd_word"),

           r_rsp_fsm("rsp_fsm"),
           r_rsp_word("r_rsp_word"),

           r_tgt_fsm("r_tgt_fsm"),

		   p_clk("p_clk"),
		   p_resetn("p_resetn"),
		   p_vci_target("p_vci_target"),
		   p_vci_initiator("p_vci_initiator"),
  		   p_to_coproc(alloc_elems<ToCoprocOutput<uint32_t, uint8_t> > 
               ("p_to_coproc", n_to_coproc)),
  		   p_from_coproc(alloc_elems<FromCoprocInput<uint32_t, uint8_t> > 
               ("p_from_coproc", n_from_coproc)),
           p_config(alloc_elems<sc_out<uint32_t> >  
               ("p_config", n_config)),
           p_status(alloc_elems<sc_in<uint32_t> > 
               ("p_status", n_status)),
           p_irq( "p_irq" )
{
    std::cout << "  - Building VciMwmrDma : " << name << std::endl;
    std::cout << "    => segment " << m_segment.name()
              << " / base = " << std::hex << m_segment.baseAddress()
              << " / size = " << m_segment.size() << std::endl; 

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();


    assert( (m_coproc_config_regs <= 16) and
    "VCI_MWMR_DMA error : The number of config registers cannot be larger than 16");

    assert( (m_coproc_status_regs <= 16) and
    "VCI_MWMR_DMA error : The number of status registers cannot be larger than 16");

    assert( (m_all_channels <= 16) and
    "VCI_MWMR_DMA error : The number of channels cannot be larger than 16");

    assert( (vci_param::T >= 4) and 
    "VCI_MWMR_DMA error : The VCI TRDID field must be at least 4 bits");

    assert( (vci_param::B == 4) and 
    "VCI_MWMR_DMA error : The VCI DATA field must be 32 bits");

    assert( ((burst_size==4) or (burst_size==8) or (burst_size==16) or 
             (burst_size==32) or (burst_size==64)) and
    "VCI_MWMR_DMA error : The burst size must be 4, 8, 16, 32, 64 bytes");
    
    // construct hardware FIFOs and initialize channel ways for all channels
    for ( size_t k = 0; k<m_all_channels; ++k ) 
    {
        if ( k < m_to_coproc_channels )  // first channels are TO_COPROC
        {
            std::ostringstream o;
            o << "fifo_to_coproc[" << k << "]";
            r_channel[k].way  = TO_COPROC;
            r_channel[k].fifo = new GenericFifo<uint32_t>(o.str(), 2);
        }
        else                    // last channels are TO_COPROC
        {
            std::ostringstream o;
            o << "fifo_from_coproc[" << k << "]";
            r_channel[k].way = FROM_COPROC;
            r_channel[k].fifo = new GenericFifo<uint32_t>(o.str(), 2);
        }
    }
}  // end constructor


/////////////////////////
tmpl(/**/)::~VciMwmrDma()
{
    for ( size_t i = 0; i<m_all_channels; i++ ) delete r_channel[i].fifo;
    dealloc_elems(p_from_coproc,   m_from_coproc_channels); 
    dealloc_elems(p_to_coproc,     m_to_coproc_channels); 
    dealloc_elems(p_config,        m_coproc_config_regs);
    dealloc_elems(p_status,        m_coproc_status_regs);
    delete [] r_channel;
}

}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


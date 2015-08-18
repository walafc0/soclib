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
 * Copyright (c) UPMC, Lip6, SoC
 *	 alain.greiner@lip6.fr april 2015  
 *
 * Maintainers: alain 
 */

#include <stdint.h>
#include <iostream>
#include <arithmetics.h>
#include <fcntl.h>
#include "vci_ahci_sdc.h"
#include "ahci_sdc.h"
#include "alloc_elems.h"

namespace soclib { namespace caba {

#define tmpl(t) template<typename vci_param> t VciAhciSdc<vci_param>

using namespace soclib::caba;
using namespace soclib::common;


///////////////////////////////////////////////////
tmpl(uint32_t)::crc7( uint32_t current , bool val )
{
    bool crc_0 = (current & 0x01);
    bool crc_1 = (current & 0x02) >> 1;
    bool crc_2 = (current & 0x04) >> 2;
    bool crc_3 = (current & 0x08) >> 3;
    bool crc_4 = (current & 0x10) >> 4;
    bool crc_5 = (current & 0x20) >> 5;
    bool crc_6 = (current & 0x40) >> 6;

    bool nxt_0 = crc_6 ^ val;
    bool nxt_1 = crc_0;
    bool nxt_2 = crc_1;
    bool nxt_3 = crc_2 ^ nxt_0;
    bool nxt_4 = crc_3;
    bool nxt_5 = crc_4;
    bool nxt_6 = crc_5;

    return ( ((uint32_t)nxt_6 << 6) |
             ((uint32_t)nxt_5 << 5) |
             ((uint32_t)nxt_4 << 4) |
             ((uint32_t)nxt_3 << 3) |
             ((uint32_t)nxt_2 << 2) |
             ((uint32_t)nxt_1 << 1) |
             ((uint32_t)nxt_0     ) );

}  // end crc 7

////////////////////////////////////////////////////
tmpl(uint32_t)::crc16( uint32_t current , bool val )
{
    bool crc_0  = (current & 0x0001);
    bool crc_1  = (current & 0x0002) >> 1;
    bool crc_2  = (current & 0x0004) >> 2;
    bool crc_3  = (current & 0x0008) >> 3;
    bool crc_4  = (current & 0x0010) >> 4;
    bool crc_5  = (current & 0x0020) >> 5;
    bool crc_6  = (current & 0x0040) >> 6;
    bool crc_7  = (current & 0x0080) >> 7;
    bool crc_8  = (current & 0x0100) >> 8;
    bool crc_9  = (current & 0x0200) >> 9;
    bool crc_10 = (current & 0x0400) >> 10;
    bool crc_11 = (current & 0x0800) >> 11;
    bool crc_12 = (current & 0x1000) >> 12;
    bool crc_13 = (current & 0x2000) >> 13;
    bool crc_14 = (current & 0x4000) >> 14;
    bool crc_15 = (current & 0x8000) >> 15;

    bool nxt_0  = crc_15 ^ val;
    bool nxt_1  = crc_0;
    bool nxt_2  = crc_1;
    bool nxt_3  = crc_2;
    bool nxt_4  = crc_3;
    bool nxt_5  = crc_4 ^ nxt_0;
    bool nxt_6  = crc_5;
    bool nxt_7  = crc_6;
    bool nxt_8  = crc_7;
    bool nxt_9  = crc_8;
    bool nxt_10 = crc_9;
    bool nxt_11 = crc_10;
    bool nxt_12 = crc_11 ^ nxt_0;
    bool nxt_13 = crc_12;
    bool nxt_14 = crc_13;
    bool nxt_15 = crc_14;
 
    return ( ((uint32_t)nxt_15 << 15) |
             ((uint32_t)nxt_14 << 14) |
             ((uint32_t)nxt_13 << 13) |
             ((uint32_t)nxt_12 << 12) |
             ((uint32_t)nxt_11 << 11) |
             ((uint32_t)nxt_10 << 10) |
             ((uint32_t)nxt_9  <<  9) |
             ((uint32_t)nxt_8  <<  8) |
             ((uint32_t)nxt_7  <<  7) |
             ((uint32_t)nxt_6  <<  6) |
             ((uint32_t)nxt_5  <<  5) |
             ((uint32_t)nxt_4  <<  4) |
             ((uint32_t)nxt_3  <<  3) |
             ((uint32_t)nxt_2  <<  2) |
             ((uint32_t)nxt_1  <<  1) |
             ((uint32_t)nxt_0       ) );

} //  end crc16()

////////////////////////
tmpl(void)::transition()
{
    if(p_resetn.read() == false) 
    {
	    r_dma_fsm         = DMA_IDLE;
        r_dma_run         = false;
        r_dma_pxci        = 0;
        r_dma_pxie        = 0;
        r_dma_pxis        = 0;

	    r_tgt_fsm         = TGT_IDLE;

	    r_sdc_fsm         = SDC_IDLE;
        r_sdc_clk_period  = 1000;
        r_sdc_clk_low     = 500;
        r_sdc_clk_out     = false;
        r_sdc_cmd_en_out  = false;
        r_sdc_dat_en_out  = false;

        r_tgt2sdc_req     = false;
        r_dma2sdc_req     = false;

	    r_dma_fifo.init();

	    return;
    } 

    // DMA FIFO default input values
    bool        fifo_put   = false;
    bool        fifo_get   = false;
    vci_data_t  fifo_wdata = 0;

    //////////////////////////////////////////////////////////////////////////////
    // The TGT FSM handles the software access to addressable registers
    // controling both the SDC FSM behaviour, and the DMA FSM behaviour.
    //////////////////////////////////////////////////////////////////////////////

    switch(r_tgt_fsm)
    {
    ////////////
    case TGT_IDLE:
    {
        if ( p_vci_target.cmdval.read() ) 
        { 
	        r_tgt_srcid = p_vci_target.srcid.read();
	        r_tgt_trdid = p_vci_target.trdid.read();
	        r_tgt_pktid = p_vci_target.pktid.read();

	        uint64_t address = p_vci_target.address.read();
	        uint32_t wdata   = p_vci_target.wdata.read();
		    bool     write   = ( p_vci_target.cmd.read() == vci_param::CMD_WRITE );
            uint32_t cell    = (uint32_t)((address & 0x3F)>>2);

	        bool found = false;
	        std::list<soclib::common::Segment>::iterator seg;
	        for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ ) 
	        {
		        if ( seg->contains(address) ) found = true;
	        }

            // error detection
	        if (not found) 
            {
                r_tgt_fsm = TGT_ERROR;
	        } 
            else if (p_vci_target.cmd.read() != vci_param::CMD_READ &&
		             p_vci_target.cmd.read() != vci_param::CMD_WRITE) 
            {
	    	    r_tgt_fsm = TGT_ERROR;
	        } 

            ///////// commands to SDC FSM ///////////////
            else if ( (cell == SDC_CLK_PERIOD) and write ) // set SDC CLK format
            {
			    uint32_t period = wdata & 0x0000FFFF;
			    uint32_t low    = wdata >> 16;
                if ( (period < 2) or (low >= period) )
                {
                    r_tgt_fsm = TGT_ERROR;
                }
                else
                {
		            r_tgt_fsm        = TGT_RSP_WRITE;
			        r_sdc_clk_period = period;
			        r_sdc_clk_low    = low;
                }
		    }
            else if ( (cell == SDC_CMD_ARG) and write )   // set SDC CMD argument
            {
			    r_tgt_cmd_arg = wdata;
		        r_tgt_fsm     = TGT_RSP_WRITE;
		    }
            else if ( (cell == SDC_CMD_ID) and write )    // set SDC command index, and
                                                          // request access to SD card
            {
                if ( (wdata !=  SDC_CMD0 ) and (wdata != SDC_CMD2 ) and (wdata != SDC_CMD3 ) and 
                     (wdata !=  SDC_CMD7 ) and (wdata != SDC_CMD8 ) and (wdata != SDC_CMD9 ) and 
                     (wdata !=  SDC_CMD11) and (wdata != SDC_CMD13) and (wdata != SDC_CMD6 ) )   
                {
		            r_tgt_fsm = TGT_ERROR;
                }
                else
                {
			        r_tgt_cmd_id  = wdata;
                    r_tgt_cmd_req = true;  
		            r_tgt_fsm     = TGT_RSP_WRITE;
                }
		    }
            else if ( (cell == SDC_RSP) and not write )   // get SDC card RSP
            {
                if ( r_tgt2sdc_req.read() == true )  // blocked until RSP available
                {
                    r_tgt_rdata = r_tgt2sdc_rsp.read();
		            r_tgt_fsm   = TGT_RSP_READ;
                }
            }

            ///////// commands to DMA FSM ///////////////
            else if( (cell == HBA_PXCLB) and write )      // set  Command List Base paddr
            {
                r_dma_pxclb = wdata; 
                r_tgt_fsm   = TGT_RSP_WRITE;
            }
            else if( (cell == HBA_PXCLB) and not write )  // get Command List Base paddr
            {
                r_tgt_rdata = r_dma_pxclb.read(); 
                r_tgt_fsm   = TGT_RSP_READ;
            }
            else if( (cell == HBA_PXCLBU) and write )     // set  Command List Base extension
            {
                r_dma_pxclbu = wdata; 
                r_tgt_fsm    = TGT_RSP_WRITE;
            }
            else if( (cell == HBA_PXCLBU) and not write ) // get  Command List Base extension
            {
                r_tgt_rdata = r_dma_pxclbu; 
                r_tgt_fsm   = TGT_RSP_READ;
            }
            else if( (cell == HBA_PXIS) and write )       // reset PXIS register
            {
                r_dma_pxis = 0;
                r_tgt_fsm  = TGT_RSP_WRITE;
            }
            else if( (cell == HBA_PXIS) and not write )   // get PXIS register
            {
                r_rdata   = r_dma_pxis;  
                r_tgt_fsm = TGT_RSP_READ;
            }
            else if( (cell == HBA_PXIE) and write )       // set PXIE register
            {
                r_dma_pxie = wdata; 
                r_tgt_fsm  = TGT_RSP_WRITE;
            }
            else if( (cell == HBA_PXIE) and not write )   // get PXIE register
            {
                r_rdata   = r_dma_pxie;  
                r_tgt_fsm = TGT_RSP_READ;
            }
            else if( (cell == HBA_PXCI) and write )       // set only one bit in PXCI
            {
                r_dma_pxci = r_dma_pxci.read() | wdata;
                r_tgt_fsm  = TGT_RSP_WRITE;
            }
            else if( (cell == HBA_PXCI) and not write )   // get PXCI register
            {
                r_rdata   = r_dma_pxci;  
                r_tgt_fsm = TGT_RSP_READ;
            }
            else if( (cell == HBA_PXCMD) and write )   
            {
                if( wdata == 0 )                          // dma soft reset
                {
                    r_dma_pxci = 0;
                    r_dma_pxis[dma]  = 0;
                    r_dma_slot[dma]  = 0;
                    r_dma_run[dma]   = false;
                }
                else                                      // dma activation
                {
                    r_dma_run[dma]   = true;
                }
                r_tgt_fsm = T_WRITE_RSP;
            }
            else if( (cell == HBA_PXCMD) and not write )  // get RUN register
            {
                r_rdata   = r_dma_run;  
                r_tgt_fsm = TGT_RSP_READ;
            }
            ///////////// illegal command //////////////
            else           
            {
                r_vci_target = TGT_ERROR;
            }
	    }
	    break;
    }
    ////////////////
    case TGT_RSP_READ:
    case TGT_RSP_WRITE:
    case TGT_ERROR:
    {
	    if (p_vci_target.rspack.read() ) r_tgt_fsm  = TGT_IDLE;
	    break;
    }
    } // end switch target fsm


	

    //////////////////////////////////////////////////////////////////////////////
    // the SDC FSM controls the SDC bus signals.
    // It can execute 3 types of transactions:
    // - In case of direct CMD request from TGT FSM, it transfers the command 
    //   to the shift register, send the command, get the response
    //   in the shift register, and returns the response to software.
    // - In case of DMA_TX request from DMA FSM, it transfer the CMD24 command
    //   to the shifter, send it, get and check the response, and send the data 
    //   on the 4 bits SDC bus. It read the data in the DMA_FIFO, and pause 
    //   the SDC clock if FIFO is empty.
    // - In case of DMA_RX request from DMA FSM, it transfer the CMD17 command
    //   to the shifter, send it, get and check the response, and receive the data 
    //   on the 4 bits SDC bus. It writes the data in the DMA_FIFO, and pause 
    //   the SDC clock if FIFO is full.
    //////////////////////////////////////////////////////////////////////////////
    switch (r_sdc_fsm) 
    {
        //////////////
        case SDC_IDLE:   // poll both the DMA_FSM requests and the TGT_FSM requests
        {
            // disable SDC clock and tri-state for CMD and DAT
            r_sdc_clk_out    = false;
            r_sdc_cmd_en_out = false;
            r_sdc_dat_en_out = false;
        
            // initializes cycles, flits, and words counter
            r_sdc_cycles_count = 0;
            r_sdc_flits_count  = 0;
            r_sdc_words_count  = 0;

            // initializes CRC
            r_sdc_crc[0] = 0;
            r_sdc_crc[1] = 0;
            r_sdc_crc[2] = 0;
            r_sdc_crc[3] = 0;

            if ( r_tgt2sdc_req.read() )                                // TGT_FSM request
            {
                // set CMD type
                r_sdc_cmd_type = TGT_CMD_TYPE;

                // initialises data register
                r_sdc_data = (((uint64_t)0x1)                      << 46) | // start bit
                             (((uint64_t)r_tgt_cmd_id.read())      << 40) | // CMD index
                             (((uint64_t)r_tgt_cmd_arg.read())     <<  8) | // CMD argument
                             (((uint64_t)0x1)                           ) ; // stop bit

                r_sdc_fsm = SDC_CMD_PUT;
	        }
            else if ( r_dma2sdc_req.read() and r_dma_rx.read() )      // RX DMA request 
            {
                // set CMD type
                r_sdc_cmd_type = DMA_RX_TYPE;

                // initializes data register
                r_sdc_data = (((uint64_t)0x1)                      << 46) | // start bit
                             (((uint64_t)SDC_CMD17)                << 40) | // CMD index
                             (((uint64_t)r_dma_lba.read())         <<  8) | // CMD argument
                             (((uint64_t)0x1)                           ) ; // stop bit
  
                r_sdc_fsm = SDC_CMD_PUT;
            }
            else if ( r_dma2sdc_req.read() and not r_dma_rx.read() )    // TX DMA request 
            {
                // set CMD type
                r_sdc_cmd_type = DMA_TX_TYPE;

                // initializes data register
                r_sdc_data = (((uint64_t)0x1)                      << 46) | // start bit
                             (((uint64_t)SDC_CMD24)                << 40) | // CMD index
                             (((uint64_t)r_dma_lba.read())         <<  8) | // CMD argument
                             (((uint64_t)0x1)                           ) ; // stop bit
  
		        r_sdc_fsm = SDC_CMD_PUT;
	        } 
	        break;
        }
        /////////////////
        case SDC_CMD_PUT:   // send a 48 bits command => two nested loops 
                            // on cycles in SDC period / on flits in CMD frame
        {
            uint32_t cycle = r_sdc_cycles_count.read();   // current cycle in SDC period
            uint32_t flit  = r_sdc_flits_count.read();    // current flit in CMD frame

            // significant cycles in SDC period
            bool first_cycle  = (cycle == 0);
            bool sample_cycle = (cycle == r_sdc_clk_low.read());
            bool last_cycle   = (cycle == r_sdc_clk_period.read() - 1);

            bool last_flit    = (flit == 47);
  
            // Generate SDC clock
            if ( fisrt_cycle  ) r_sdc_clk_out = false;
            if ( sample_cycle ) r_sdc_clk_out = true;

            // update cycles and flits counter
            if ( last_cycle )  
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = flit + 1;
            }
            else
            {
                r_sdc_cycles_count = cycle + 1;
            }

            // send new value on CMD and compute CRC at first cycle in SDC period
            if ( first_cycle )
            {
                bool value    = (bool)((r_sdc_data.read() >> (47 - flit)) & 0x1);

                // send CMD bit
                r_sdc_cmd_out    = value;
                r_sdc_cmd_en_out = true;

                // compute CRC (only for the 40 first CMD flits)
                if ( flit < 40 ) r_sdc_crc[0] = crc7( r_sdc_crc[0].read() , value ); 
            }

            // insert crc at flit 39 and last cycle in SDC period
            if ( last_cycle and (flit == 39) 
            {
                r_sdc_data = r_sdc_data.read() | (r_src_crc[0].read() << 1) ;
            }

            // CMD completion
            if ( last_flit  and last cycle ) 
            {
                if ( r_tgt_cmd_id.read() == 0 ) // no response for CMD0
                {
                    r_tgt2sdc_req = false;
                    r_sdc_fsm     = SDC_IDLE;
                }
                else     // response expected => go to SDC_RSP_GET
                {
                    r_sdc_cycles_count = 0;
                    r_sdc_flits_count  = 0;
                    r_sdc_cmd_en_out   = false;
                    r_sdc_start_found  = false;
                    r_sdc_crc[0]       = 0;
                    r_sdc_fsm          = SDC_RSP_GET;
                }
            }  
            break;
        }
        /////////////////
        case SDC_RSP_GET:   // receive a 48 bits response on CMD wire => two nested loops 
                            // on cycles in SDC period / on flits in RSP frame
        {
            uint32_t cycle = r_sdc_cycles_count.read();  // current cycle in SDC period
            uint32_t flit  = r_sdc_flits_count.read();   // current flit in RSP frame

            // significant cycles in SDC period
            bool first_cycle  = (cycle == 0);
            bool sample_cycle = (cycle == r_sdc_clk_low.read());
            bool last_cycle   = (cycle == r_sdc_clk_period.read() - 1);

            bool last_flit    = (flit == 47);

            // Generate SDC clock
            if ( first_cycle  ) r_sdc_clk_out = false;
            if ( sample_cycle ) r_sdc_clk_out = true;

            // sample the CMD input on the SDC clock rising edge
            if ( sample_cycle ) r_sdc_cmd_value = p_sdc_cmd_in.read();

            ////////// waiting start bit /////////////////
            if ( r_sdc_start_found.read() == false )
            {
                // update cycles and flits counters
                if ( last_cycle ) 
                {
                    r_sdc_cycles_count = 0;
                    if ( r_sdc_cmd_value == false )
                    {
                        r_dsc_start_found = true;
                        r_sdc_flits_count = 1;
                    }
                }
                else   // not the last cycle 
                {
                    r_sdc_cycles_count = cycle + 1;
                }
            }
            ///////// get RSP frame //////////////////
            else       
            {
                // update cycles and flits counters
                if ( last_cycle )
                {
                    r_sdc_cycles_count = 0;
                    r_sdc_flits_count  = flit + 1;
                }
                else
                {
                    r_sdc_cycles_count = cycle + 1;
                }

                // update r_sdc_data and compute CRC at last cycle of each flit
                if ( last cycle )
                {
                    r_sdc_data = (r_sdc_data.read() << 1) | r_sdc_cmd_value;
                    if ( flit < 40 ) 
                        r_sdc_crc[0] = crc7( r_sdc_crc[0].read() , r_sdc_cmd_value );
                }
            
                // check RSP CRC at first cycle of last flit (corresponding to stop bit)
                if ( last_flit and first_cycle)
                {
                    r_sdc_crc_error = (r_sdc_crc[0].read() != ((r_sdc_data.read()>>1) & 0x7F));
                }

                // RSP completion
                if ( last_cycle and last_flit )
                {
                    uint32_t response = (uint32_t)(r_sdc_data.read() >> 8);

                    if ( r_sdc_cmd_type.read() == TGT_CMD_TYPE )   // return rsp to TGT_FSM
                    {
                        r_tgt2sdc_req   = false;
                        r_tgt2sdc_rsp   = (r_sdc_crc_error.read()) ? 0xFFFFFFFF : response;
                        r_sdc_crc_error = false;
                        r_sdc_fsm       = SDC_IDLE;
                    }
                    else if ( response or r_sdc_crc_error.read() )  // return error to DMA_FSM
                    {
                        r_dma2sdc_req   = false;
                        r_dma2sdc_rsp   = (r_sdc_crc_error.read()) ? 0xFFFFFFFF : response;
                        r_sdc_crc_error = false;
                        r_sdc_fsm       = SDC_IDLE;
                    }
                    else if ( r_sdc_type.read() == DMA_TX_TYPE )   // put data to SDC
                    {
                        r_sdc_cycles_count = 0;
                        r_sdc_flits_count  = 0;
                        r_sdc_words_count  = 0;
                        r_sdc_start_found  = false;
                        r_sdc_crc[0]       = 0;
                        r_sdc_fsm          = SDC_DMA_TX;
                    }
                    else if ( r_sdc_type.read() == DMA_RX_TYPE )   // get data from SDC
                    {
                        r_sdc_cycles_count = 0;
                        r_sdc_flits_count  = 0;
                        r_sdc_words_count  = 0;
                        r_sdc_start_found  = false;
                        r_sdc_crc[0]       = 0;
                        r_sdc_fsm          = SDC_DMA_RX;
                    }
                }
            }
            break;
        }
        ////////////////
        case SDC_DMA_TX:   // move one data block to SD card from DMA FIFO, and
                           // acknowledge the DMA_FSM request when block completed.
                           // It consume 32 bits or 64 bits words in the DMA FIFO.
                           // - 32 bits word =>  8 flits per word / 128 words per block
                           // - 64 bits word => 16 flits per word /  64 words per block
                           // => thee nested loops: on cycles, on flits, on words
                           //    requiring 1024 flits for data, plus 16 flits for CRC. 
        {
            uint32_t cycle    = r_sdc_cycles_count.read();   // current cycle in SDC period
            uint32_t flit     = r_sdc_flits_count.read();    // current SDC period 
            uint32_t word     = r_sdc_words_count.read();    // current word 

            // significant cycles in SDC period
            bool first_cycle  = (cycle == 0);
            bool sample_cycle = (cycle == r_sdc_clk_low.read());
            bool last_cycle   = (cycle == r_sdc_clk_period.read() - 1);

            uint32_t flit_max = (vci_param::B == 4) ?   8 : 16;
            uint32_t word_max = (vci_param::B == 4) ? 128 : 64;

            bool last_flit    = (flit = flit_max - 1);

            //////// send start bit (one flit) /////////////////
            if ( r_sdc_start_found.read() == false )
            {
                // get first word from FIFO at first cycle
                // break if FIFO empty (SDC clock will be stretched)
                if ( last_cycle )
                {
                    if ( r_dma_fifo.rok() == false )  break;
                    
                    r_sdc_data = r_dma_fifo.read();
                    fifo_get   = true;
                }

                // generate SDC clock
                if ( first_cycle  ) r_sdc_clk_out = false;
                if ( sample_cycle ) r_sdc_clk_out = true;

                // update cycles counter
                if ( last_cycle )
                {
                    r_cycles_count    = 0;   
                    r_sdc_start_found = true;
                }
                else
                {
                    r_cycles_count = cycle + 1;   
                }
                
                // send start bit at first cycle
                if ( first_cycle )
                {
                    r_sdc_dat_en_out = true;
                    r_sdc_dat_out[0] = false;
                    r_sdc_dat_out[1] = false;
                    r_sdc_dat_out[2] = false;
                    r_sdc_dat_out[3] = false;
                }
            }
            //////// send data stream (1024 flits) //////////////
            else if ( word*flit_max + flit < 1024 )
            {
                // get one new word from FIFO at first cycle and first flit  
                // break if FIFO empty (SDC clock will be stretched)
                if ( first_cycle and first_flit )
                {
                    if ( r_dma_fifo.rok() == false )  break;
                
                    r_sdc_data = r_dma_fifo.read();
                    fifo_get   = true;
                }

                // send data and compute CRC, on SDC clock falling edge 
                if ( first_cycle )
                {
                    uint32_t shift = (flit & 0x1) ? (flit - 1) : (flit + 1);

                    bool bit0 = (bool)((r_sdc_data.read() >> ((shift<<2)  )) & 0x1);
                    bool bit1 = (bool)((r_sdc_data.read() >> ((shift<<2)+1)) & 0x1);
                    bool bit2 = (bool)((r_sdc_data.read() >> ((shift<<2)+2)) & 0x1);
                    bool bit3 = (bool)((r_sdc_data.read() >> ((shift<<2)+3)) & 0x1);

                    r_sdc_dat_en_out = true;
                    r_sdc_dat_out[0] = bit0;
                    r_sdc_dat_out[1] = bit1;
                    r_sdc_dat_out[2] = bit2;
                    r_sdc_dat_out[3] = bit3;

                    r_sdc_crc[0] = crc16( r_sdc_crc[0].read() , bit0 );
                    r_sdc_crc[1] = crc16( r_sdc_crc[1].read() , bit1 );
                    r_sdc_crc[2] = crc16( r_sdc_crc[2].read() , bit2 );
                    r_sdc_crc[3] = crc16( r_sdc_crc[3].read() , bit3 );
                }

                // generate SDC clock
                if ( first_cycle  ) r_sdc_clk_out = false;
                if ( sample_cycle ) r_sdc_clk_out = true;

                // update cycles, flits, and words counters
                if ( last_cycle ) 
                {
                    if ( last_flit )                  // last cycle / last flit
                    {
                        r_sdc_cycles_count = 0;
                        r_sdc_flits_count  = 0;
                        r_sdc_words_count  = r_sdc_words_count.read() + 1;
                    }     
                    else                              // last cycle / not last flit
                    {
                        r_sdc_cycles_count = 0;
                        r_sdc_flits_count  = r_sdc_flits_count.read() + 1;
                    }
                }
                else                                 // not last cycle
                {
                    r_sdc_cycles_count = r_sdc_cycles_count + 1;
                }
            }
            //////// send CRC (16 flits)  ////////////////
            else if ( (word == word_max) and (flit < 16 )
            {
                // Generate SDC clock
                if ( first_cycle  ) r_sdc_clk_out = false;
                if ( sample_cycle ) r_sdc_clk_out = true;

                // update cycles and flits counters
                if ( last_cycle ) 
                {
                    r_sdc_cycles_count = 0;
                    r_sdc_flits_count  = flit + 1;
                }
                else
                {
                    r_sdc_cycles_count = cycle + 1;
                }

                // send data on SDC clock falling edge
                if ( first_cycle )
                {
                    r_sdc_dat_en_out = true;
                    r_sdc_dat_out[0] = (bool)((r_sdc_crc[0].read() >> (15-flit)) & 0x1);
                    r_sdc_dat_out[1] = (bool)((r_sdc_crc[1].read() >> (15-flit)) & 0x1);
                    r_sdc_dat_out[2] = (bool)((r_sdc_crc[2].read() >> (15-flit)) & 0x1);
                    r_sdc_dat_out[3] = (bool)((r_sdc_crc[3].read() >> (15-flit)) & 0x1);
                }
            }    
            //////// send stop bit (1 flit) //////////
            else
            {
                // Generate SDC clock
                if ( first_cycle  ) r_sdc_clk_out = false;
                if ( sample_cycle ) r_sdc_clk_out = true;

                // update cycles counter
                r_sdc_cycles_count = cycle + 1;
 
                // send stop bit on SDC clock falling edge
                if ( first_cycle ) 
                {
                    r_sdc_dat_en_out = true;
                    r_sdc_dat_out[0] = true;
                    r_sdc_dat_out[1] = true;
                    r_sdc_dat_out[2] = true;
                    r_sdc_dat_out[3] = true;
                }
                
                // goes to wait SDC acknowledge
                if ( last_cycle )
                {
                    r_sdc_cycles_count = 0;
                    r_sdc_flits_count  = 0;
                    r_sdc_start_found  = false;
                    r_sdc_fsm          = SDC_DMA_TX_ACK;
                }
            }
            break;
        }
        ////////////////////
        case SDC_DMA_TX_ACK:
        {
            break;
        }
        ////////////////
        case SDC_DMA_RX:   // move one data block from SD card to DMA FIFO, and
                           // acknowledge the DMA_FSM request when block completed.
                           // It produce 32 bits or 64 bits words to the DMA FIFO.
                           // - 32 bits word =>  8 flits per word / 128 words per block
                           // - 64 bits word => 16 flits per word /  64 words per block
                           // => three nested loops: on cycles, on flits, and on words,
                           //    requiring (1024) flits for data, plus (16) flits for CRC. 
        {
            uint32_t cycle    = r_sdc_cycles_count.read();   // current cycle in SDC period
            uint32_t flit     = r_sdc_flits_count.read();    // current SDC period 
            uint32_t word     = r_sdc_words_count.read();    // current word 

            uint32_t flit_max = (vci_param::B == 4) ?   8 : 16;
            uint32_t word_max = (vci_param::B == 4) ? 128 : 64;

            // Generate SDC clock
            if ( cycle == 0 )                    r_sdc_clk_out = false;
            if ( cycle == r_sdc_clk_low.read() ) r_sdc_clk_out = true;

            // sample DAT inputs, on the SDC clock rising edge 
            if ( cycle == r_sdc_clk_low.read() ) 
            {
                r_sdc_dat0_value = p_sdc_dat_in[0].read();
                r_sdc_dat1_value = p_sdc_dat_in[1].read();
                r_sdc_dat2_value = p_sdc_dat_in[2].read();
                r_sdc_dat3_value = p_sdc_dat_in[3].read();
            }

            //////////// waiting start bit (one flit )///////////////
            if ( r_sdc_start_found.read() == false )
            {
                // update cycles and flits counter / test start bit
                if ( cycle == r_sdc_clk_period.read() - 1 )          // last cycle
                {
                    r_sdc_cycles_count = 0;
                    if ( r_sdc_dat0 == false ) 
                    {
                        r_sdc_start_found = true;
                        assert( (r_sdc_dat1 == false) and 
                                (r_sdc_dat2 == false) and
                                (r_sdc_dat3 == false) );
                    }
                }
                else                                            // not the last cycle 
                {
                   r_sdc_cycles_count = cycle + 1;
                }
            }
            //////////// receive data stream (1024 flits) //////////////
            else if ( word*flit_max + flit < 1024 )
            {
                // update cycles, flits, and words counters
                if ( cycle == r_sdc_clk_period.read() - 1 ) 
                {
                    if ( flit  == flit_max - 1 )      // last cycle / last flit 
                    {
                        r_sdc_cycles_count = 0;
                        r_sdc_flits_count  = 0;
                        r_sdc_words_count  = r_sdc_words_count.read() + 1;
                    }     
                    else                              // last cycle / not last flit
                    {
                        r_sdc_cycles_count = 0;
                        r_sdc_flits_count  = r_sdc_flits_count.read() + 1;
                    }
                }
                else                                 // not last cycle
                {
                    r_sdc_cycles_count = r_sdc_cycles_count + 1;
                }

                // insert 4 DAT bits into shift register 
                // and compute CRC at last cycle of each flit
                if ( cycle == r_sdc_period.read() - 1 )
                {
                    uint32_t shift = (flit & 0x1) ? (flit - 1) : (flit + 1);
                    uint64_t mask  = ((uint64_t)0xF) << (shift<<2);
                    r_sdc_data = (r_sdc_data.read() & ~mask)             | 
                                  (((uint64_t)r_sdc_dat0) << (shift<<2)  ) |  
                                  (((uint64_t)r_sdc_dat1) << (shift<<2)+1) |  
                                  (((uint64_t)r_sdc_dat2) << (shift<<2)+2) |  
                                  (((uint64_t)r_sdc_dat3) << (shift<<2)+3) ;  

                    r_sdc_crc[0] = crc16( r_sdc_crc[0].read() , r_sdc_dat0 );
                    r_sdc_crc[1] = crc16( r_sdc_crc[1].read() , r_sdc_dat1 );
                    r_sdc_crc[2] = crc16( r_sdc_crc[2].read() , r_sdc_dat2 );
                    r_sdc_crc[3] = crc16( r_sdc_crc[3].read() , r_sdc_dat3 );
                }
            }
            //////////// receive CRC (16 flits) ///////////////
            else if ( word*flit_max + flit < 1040 )
            {
                // update cycles and flits counters
                if ( cycle == r_sdc_clk_period.read() - 1 ) 
                {
                    if ( flit  == flit_max - 1 )      // last cycle / last flit 
                    {
                        r_sdc_cycles_count = 0;
                        r_sdc_flits_count  = 0;
                    }     
                    else                              // last cycle / not last flit
                    {
                        r_sdc_cycles_count = 0;
                        r_sdc_flits_count  = r_sdc_flits_count.read() + 1;
                    }
                }
                else                                 // not last cycle
                {
                    r_sdc_cycles_count = r_sdc_cycles_count + 1;
                }
                
                // check each received CRC bit in the last cycle of current flit
                if ( cycle == r_sdc_clk_period.read() - 1 ) 
                {
                    if ( (r_sdc_dat0 != (bool)((r_sdc_crc[0] >> flit) & 0x1)) or
                         (r_sdc_dat1 != (bool)((r_sdc_crc[1] >> flit) & 0x1)) or
                         (r_sdc_dat2 != (bool)((r_sdc_crc[2] >> flit) & 0x1)) or
                         (r_sdc_dat3 != (bool)((r_sdc_crc[3] >> flit) & 0x1)) )
                    {
                        // TODO
                    }
                }
            }
            // signal block completion
            else
            {
                r_dma2sdc_req = false;
                r_dma2sdc_rsp = // TODO
                r_sdc_fsm     = SDC_IDLE;
            }

            // write into FIFO, at first cycle and first flit for each received word
            if ( (cycle == 0) and (flit == 0) and (word > 0) and (word <= word_max) )
            {
               fifo_put   = true;
               fifo_wdata = (vci_data_t)r_sdc_data.read();
            }

            break;
        }   // end SDC_DMA_RX
    }   // end SDC_FSM



    //////////////////////////////////////////////////////////////////////////////////
    // This DMA_FSM implements the AHCI protocol.
    // It read successively the next command descriptor, then the command header,
    // register the command aarguments, and loops on the buffers defined in the
    // command table. The buffer address and size must be multiple of the burst size.
    // For each buffer, it loops on the blocks covering the buffer, making VCI burst 
    // request that are stored in one single FIFO containing one burst, and used
    // both for RX and TX transfers. 
    //////////////////////////////////////////////////////////////////////////////////
    switch(r_dma_fsm.read())
    {
        //////////////
        case DMA_IDLE:    // waiting a valid command in Command List
        {
            uint32_t pxci = r_dma_pxci.read();
            if ( pxci != 0 and r_dma_run.read() )  // Command List not empty
            {
                // select a command with round-robin priority
                // last served command has the lowest priority
                unsigned int cmd_slot;

                for ( size_t n = 0 ; n < 32 ; n++ )
                {
                    cmd_slot = (r_dma_slot.read() + n + 1) % 32;
                    if ( pxci & (1 << cmd_slot) )  break;
                }

                r_dma_bytes_count = 0;
                r_dma_slot        = cmd_slot;
                r_dma_fsm         =   DMA_DESC_VCI_CMD;
            }
            break;
        }
        //////////////////////
        case DMA_DESC_VCI_CMD:   // send VCI read for one Command Descriptor
        {
            if ( p_vci_initiator.cmdack.read() ) r_dma_fsm = DMA_DESC_VCI_RSP;
            break;
        }
        //////////////////////
        case DMA_DESC_VCI_RSP:  // waiting VCI response for Command Descriptor 
        {
            if ( p_vci_initiator.rspval.read() )
            {
                if ( vci_param::B == 4 )        // VCI DATA = 32 bits
                {
                    uint32_t*  buf = (uint32_t*)(r_dma_store + r_dma_bytes_count);
                    *buf = p_vci_initiator.rdata.read();
                    r_dma_bytes_count  = r_dma_bytes_count.read() + 4;
                }
                else                           // VCI DATA = 64 bits
                {
                    uint64_t*  buf = (uint64_t*)(r_dma_store + r_dma_bytes_count);
                    *buf = p_vci_initiator.rdata.read();
                    r_dma_bytes_count  = r_dma_bytes_count.read() + 8;;
                }

                if ( p_vci_initiator.reop.read() )  // last flit
                {
                    if ( p_vci_initiator.rerror.read() & 0x1 ) r_dma_fsm = DMA_ERROR;
                    else                                       r_dma_fsm = DMA_DESC_REGISTER;
                }
            }
            break;
        }
        ///////////////////////
        case DMA_DESC_REGISTER:   // Register Command Descriptor arguments 
        {
            hba_cmd_desc_t* desc = (hba_cmd_desc_t*)r_dma_store;
            r_dma_prdtl       = (desc->prdtl[1]<<8) | desc->prdtl[0];
            r_dma_rx          = desc->flag[0] & 0x40;
            r_dma_ctba        = (((uint64_t)desc->ctbau)<<32) | desc->ctba;
            r_dma_bytes_count = 0;
            r_dma_fsm         = DMA_LBA_VCI_CMD;
            break;
        }
        /////////////////////
        case DMA_LBA_VCI_CMD:   // send a VCI read for Command Table Header (16 bytes)
        {
            if ( p_vci_initiator.cmdack.read() ) r_dma_fsm = DMA_LBA_VCI_RSP;
            break;
        }
        /////////////////////            
        case DMA_LBA_VCI_RSP:   // waiting VCI response for Command Table Header
        {
            if ( p_vci_initiator.rspval.read() )
            {
                if ( vci_param::B == 4 )        // VCI DATA = 32 bits
                {
                    uint32_t*  buf = (uint32_t*)(r_dma_store + r_dma_bytes_count);
                    *buf = p_vci_initiator.rdata.read();
                    r_dma_bytes_count  = r_dma_bytes_count.read() + 4;
                }
                else                           // VCI DATA = 64 bits
                {
                    uint64_t*  buf = (uint64_t*)(r_dma_store + r_dma_bytes_count);
                    *buf = p_vci_initiator.rdata.read();
                    r_dma_bytes_count  = r_dma_bytes_count.read() + 8;;
                }

                if ( p_vci_initiator.reop.read() )  // last flit
                {
                    if ( p_vci_initiator.rerror.read() & 0x1 ) r_dma_fsm = DMA_ERROR;
                    else                                       r_dma_fsm = DMA_LBA_REGISTER;
                }
            }
            break;
        }
        //////////////////////
        case DMA_LBA_REGISTER:   // register lba 
                                 // initialize buffers counter 
        {
            hba_cmd_header_t* head = (hba_cmd_header_t*)r_dma_buffer;
            r_dma_lba = (((uint32_t)head->lba3)<<24)|(((uint32_t)head->lba2)<<16)|
                        (((uint32_t)head->lba1)<<8 )|(((uint32_t)head->lba0));

            r_dma_buffers_count = 0; 
            r_dma_fsm           = DMA_BUF_VCI_CMD;
            break;
        }
        /////////////////////
        case DMA_BUF_VCI_CMD:   // send VCI read for Buffer Descriptor
        {
            if ( p_vci_initiator.cmdack.read() ) r_dma_fsm = DMA_BUF_VCI_RSP;
            break;
        }
        /////////////////////
        case DMA_BUF_VCI_RSP:  // waiting VCI response for Buffer Descriptor read
        {
            if ( p_vci_initiator.rspval.read() )
            {
                if ( vci_param::B == 4 )        // VCI DATA = 32 bits
                {
                    uint32_t*  buf = (uint32_t*)(r_dma_store + r_dma_bytes_count);
                    *buf = p_vci_initiator.rdata.read();
                    r_dma_bytes_count  = r_dma_bytes_count.read() + 4;
                }
                else                           // VCI DATA = 64 bits
                {
                    uint64_t*  buf = (uint64_t*)(r_dma_store + r_dma_bytes_count);
                    *buf = p_vci_initiator.rdata.read();
                    r_dma_bytes_count  = r_dma_bytes_count.read() + 8;;
                }

                if ( p_vci_initiator.reop.read() )  // last flit
                {
                    if ( p_vci_initiator.rerror.read() & 0x1 ) r_dma_fsm = DMA_ERROR;
                    else                                       r_dma_fsm = DMA_BUF_REGISTER;
                }
            }
            break;
        }
        //////////////////////
        case DMA_BUF_REGISTER:  // register buffer descriptor arguments
                                // initialize bursts and blocks counters
        {
            hba_cmd_buffer_t* pbuf = (hba_cmd_buffer_t*)r_dma_buffer;

            assert( ((pbuf->dba & (m_burst_size-1)) == 0) and 
            "VCI_MULTI_AHCI ERROR: buffer address not burst aligned");

            assert( ((pbuf->dbc & (m_words_per_block<<2)-1)) == 0) and 
            "VCI_MULTI_AHCI ERROR: buffer length not multiple of block size");

            uint32_t nblocks;
            if ( pbuf->dbc & 0x1FF )  nblocks = (pbuf->dbc>>9 ) + 1;
            else                      nblocks = (pbuf->dbc>>9 );
            r_dma_buf_paddr    = (((uint64_t)pbuf->dbau)<<32) | pbuf->dba;
            r_dma_buf_blocks   = nblocks;
            r_dma_bursts_count = 0;
            r_dma_blocks_count = 0;    
            r_dma_fsm          = DMA_START_BLOCK;
            break;
        }
        /////////////////////
        case DMA_START_BLOCK:   // post request to SDC_FSM if previous request completed
                                // request arguments are r_dma_lba, r_dma_rx
        {
            if ( r_dma2sdc_req.read() == false ) // no pending transfer
            {
                r_dma2src_req = true;
                if( r_dma_rx.read() ) r_dma_fsm = DMA_RX_START_BURST;
                else                  r_dma_fsm = DMA_TX_START_BURST;
            }
            break;
        }
        ////////////////////////
        case DMA_TX_START_BURST:   // wait FIFO empty to send VCI command
        {
            if (r_dma_fifo.rok() == false )  
            {
                r_dma_bytes_count = 0;
                r_dma_fsm         = DMA_TX_VCI_CMD;
            }
            break;
        }
        ////////////////////
        case DMA_TX_VCI_CMD:  // send  VCI read for one burst 
        {
            if ( p_vci_initiator.cmdack.read() ) r_dma_fsm = DMA_TX_VCI_RSP;
            break;
        }
        ////////////////////
        case DMA_TX_VCI_RSP:  // wait VCI response for one burst
        {
            if ( p_vci_initiator.rspval.read() )
            {
                fifo_put   = true;
                fifo_wdata = p_vci_initiator.rdata.read();
                if ( vci_param::B == 4 )  r_dma_bytes_count = r_dma_bytes_count.read() + 4;
                else                      r_dma_bytes_count = r_dma_bytes_count.read() + 8;

                if ( p_vci_initiator.reop.read() )  // last flit
                {
                    if ( p_vci_initiator.rerror.read() & 0x1 ) r_dma_fsm = DMA_ERROR;
                    else                                       r_dma_fsm = DMA_END_BURST;
                }
            }
            break;
        }
        ////////////////////////    
        case DMA_RX_START_BURST:   // wait FIFO full to send VCI command
        {   
            if ( r_dma_fifo.wok() == false )  
            {
                r_dma_fsm         = DMA_RX_VCI_CMD;
                r_dma_bytes_count = 0;
            }
            break;
        }
        ////////////////////
        case DMA_RX_VCI_CMD:      // send multi_flit VCI write for one burst
        {
            if ( p_vci_initiator.cmdack.read() )
            {
                fifo_get   = true;
                if ( vci_param::B == 4 )    // VCI DATA 32 bits
                {
                    if ( r_dma_bytes_count == m_burst_size - 4 ) r_dma_fsm  = DMA_RX_VCI_RSP;
                    else  r_dma_bytes_count = r_dma_bytes_count.read() + 4;
                }
                else                        // VCI DATA 64 bits
                {
                    if ( r_dma_bytes_count == m_burst_size - 8 ) r_dma_fsm  = DMA_RX_VCI_RSP;
                    else  r_dma_bytes_count = r_dma_bytes_count.read() + 8;
                }
            }
            break;
        }
        ////////////////////
        case DMA_RX_VCI_RSP:      // wait single flit VCI response for one burst
        {
            if ( p_vci_initiator.rspval.read() )  // last flit
            {
                assert( (p_vci_initiator.reop.read() == true) and
                "error in vci_ahci_sdc component: illegal VCI write burst" );
 
                if ( p_vci_initiator.rerror.read() & 0x1 ) r_dma_fsm = DMA_ERROR;
                else                                       r_dma_fsm = DMA_END_BURST;
            }
            break;
        }
        ///////////////////
        case DMA_END_BURST:   // update burst count, block count, and buffer count
        {
            if ( r_dma_bursts_count.read() 
                 < m_bursts_per_block )         // not the last burst
            {
                r_dma_bursts_count = r_dma_bursts_count.read() + 1;
                if ( r_dma_rx.read() ) r_dma_fsm = DMA_RX_START_BURST;
                else                   r_dma_fsm = DMA_TX_START_BURST;
            }
            else 
            {
                r_dma_bursts_count = 0;
                if ( r_dma_blocks_count.read() 
                     < r_dma_buf_blocks.read() )  // last burst / not last block
                {
                    r_dma_blocks_count = r_dma_blocks_count.read() + 1;
                    if ( r_dma_rx.read() ) r_dma_fsm = DMA_RX_START_BURST;
                    else                   r_dma_fsm = DMA_TX_START_BURST;
                }
                else 
                {
                    r_dma_blocks_count = 0;
                    if ( r_dma_buffers_count.read() 
                         < r_dma_prdtl.read() )  // last burst / last block / not last buffer
                    {
                        r_dma_buffers_count = r_dma_buffers_count + 1;
                        r_dma_fsm           = DMA_BUF_VCI_CMD;
                    }
                    else                         // last burst / last block / last buffer
                    {
                        r_dma_buffers_count = 0;
                        if ( r_dma2sdc_req == false ) 
                        {
                            if ( r_dma2src_rsp.read() == TOUT_BON ) r_dma_fsm = DMA_SUCCESS;
                            else                                    r_dma_fsm = ERROR;
                        }
                    }
                }
            }
            break;
        }
        /////////////////
        case DMA_SUCCESS:   // set PXIS and reset current command in PXCI
        {
            if ( not p_vci_target.cmdval.read() )  // to avoid conflict with soft access
            {
                uint32_t cmd_id   = r_dma_slot.read();
                r_dma_slot = (cmd_id + 1) % 32;
                r_dma_pxci = r_dma_pxci.read() & (~(1<<cmd_id));
                r_dma_pxis = 0x1;
                r_dma_fsm  = DMA_IDLE;
            }
            break;
        }
        ///////////////
        case DMA_ERROR:   // set PXIS with faulty cmd_id and buf_id
                          // and reset faulty command in PXCI
        {
            if ( not p_vci_target.cmdval.read() )  // to avoid conflict with soft access
            {
                uint32_t cmd_id   = r_dma_slot.read();
                uint32_t buf_id   = r_dma_bufid.read();
                r_dma_slot = (cmd_id + 1) % 32;
                r_dma_pxci = r_dma_pxci.read() & (~(1<<cmd_id));
                r_dma_pxis = 0x40000001 | cmd_id<<24 | buf_id<<8 ;
                r_dma_fsm  = DMA_BLOCKED;
            }
            break;
        }
        /////////////////
        case DMA_BLOCKED:   // dma blocked: waiting software reset on PXIS
        {
            if( r_dma_pxis.read() == 0 ) r_dma_fsm = DMA_IDLE;
            break;
        }
    }  // end switch DMA_FSM


/*

    //////////////////////////////////////////////////////////////////////////////
    // The DMA FSM executes a loop, transfering one burst per iteration.
    // data comes from or goes to fifos, the other end of the fifos is
    // feed by or eaten by the SPI fsm.
    //////////////////////////////////////////////////////////////////////////////

    switch( r_initiator_fsm.read() ) 
    {
    ////////////
    case M_IDLE: 	// poll the r_dma_count and r_read registers 
    {
	    if ( r_dma_count != 0 )
	    {
	        // start transfer
	        if ( r_read.read() )    r_initiator_fsm = M_READ_WAIT;
	        else		            r_initiator_fsm = M_WRITE_WAIT;
	    }
	    break;
    }
    /////////////////
    case M_READ_WAIT:  // wait for the FIFO to be full
    {
	    if (!r_dma_fifo_read.wok()) 
        {
		    r_burst_word = m_words_per_burst - 1;
		    r_initiator_fsm = M_READ_CMD;
	    }
	    break;
    }
    ////////////////
    case M_READ_CMD:	// multi-flits VCI WRITE command for one burst
    {
	    if ( p_vci_initiator.cmdack.read() )
	    {
	        if ( r_burst_word == 0 )      // last flit
	        {
		        r_initiator_fsm = M_READ_RSP;
	        }
	        else		    // not the last flit
	        {
		        r_burst_word = r_burst_word.read() - 1;
	        }

	        r_dma_fifo_read.simple_get(); // consume one fifo word
	        // compute next word address
	        r_buf_address = r_buf_address.read() + vci_param::B;
	    }
	    break;
    }
    ////////////////
    case M_READ_RSP: 	// Wait a single flit VCI WRITE response
    {
	    if ( p_vci_initiator.rspval.read() )
	    {
	        if ( (p_vci_initiator.rerror.read()&0x1) != 0 ) 
	        {
	            r_burst_word = 0;
                r_dma_count = 0;
                r_dma_error = true;
	            r_initiator_fsm = M_INTR;

#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "vci_bd M_READ_ERROR" << std::endl;
#endif

	        }
	        else if ( r_spi_fsm == SDC_DMA_SEND_END ) // last burst
	        {
                r_dma_count = 0;
                r_initiator_fsm = M_INTR;
	            r_dma_error = false;

#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "vci_bd M_READ_SUCCESS" << std::endl;
#endif

	        }
	        else // keep on reading
	        {
                r_dma_count = r_dma_count - 1;
		        r_initiator_fsm  = M_READ_WAIT;
	        }
	    }
	    break;
    }
    ///////////////////
    case M_INTR: 
    {
	    r_initiator_fsm = M_IDLE;
	    r_irq = true;
	    break;
    }
    ///////////////////
    case M_WRITE_WAIT:  // wait for the FIFO to be empty
	{
        if (!r_dma_fifo_write.rok()) 
        {
	        r_burst_word = m_words_per_burst - 1;
	        r_dma_count = r_dma_count - 1;
	        r_initiator_fsm = M_WRITE_CMD;
	    }
	    break;
    }
    /////////////////
    case M_WRITE_CMD:	// single flit VCI READ command for one burst
    {
	    if ( p_vci_initiator.cmdack.read() ) r_initiator_fsm = M_WRITE_RSP;
	    break;
    }
    /////////////////
    case M_WRITE_RSP:	// wait multi-words VCI READ response
    {
	    if ( p_vci_initiator.rspval.read() )
	    {
	        typename vci_param::data_t v = p_vci_initiator.rdata.read();
	        typename vci_param::data_t f = 0;
	        // byte-swap
	        for (int i = 0; i < (vci_param::B * 8); i += 8) 
            {
		        f |= ((v >> i) & 0xff) << ((vci_param::B * 8) - 8 - i);
	        }
	        r_dma_fifo_write.simple_put(f);
	        r_burst_word = r_burst_word.read() - 1;
	        if ( p_vci_initiator.reop.read() )  // last flit of the burst
	        {
		        r_buf_address = r_buf_address.read() + m_burst_size;

		        if( (p_vci_initiator.rerror.read()&0x1) != 0 ) 
		        {
		            r_dma_count = 0;
		            r_dma_error = 1;
		            r_initiator_fsm = M_WRITE_END;

#ifdef SOCLIB_MODULE_DEBUG
		    std::cout << "vci_spi M_WRITE_ERROR" << std::endl;
#endif
                }
		        else if ( r_dma_count.read() == 0) // last burst
                {
		            r_dma_error = 0;
		            r_initiator_fsm  = M_WRITE_END;
		        }
                else					  // not the last burst
                {
		            r_initiator_fsm = M_WRITE_WAIT;
		        }
	        }
	    }
	    break;
    }
    /////////////////
    case M_WRITE_END:	// wait for the write to be completed by SPI FSM
    {
	    if (r_spi_fsm == SDC_IDLE)  r_initiator_fsm  = M_INTR;
	    break;
    }
    } // end switch r_initiator_fsm
*/

}  // end transition

//////////////////////
tmpl(void)::genMoore()
{
    // p_vci_target port   
    p_vci_target.rsrcid = (sc_dt::sc_uint<vci_param::S>)r_tgt_srcid.read();
    p_vci_target.rtrdid = (sc_dt::sc_uint<vci_param::T>)r_tgt_trdid.read();
    p_vci_target.rpktid = (sc_dt::sc_uint<vci_param::P>)r_tgt_pktid.read();
    p_vci_target.reop   = true;

    switch(r_tgt_fsm) 
    {
        case TGT_IDLE:
	        p_vci_target.cmdack = true;
	        p_vci_target.rspval = false;
	        break;
        case TGT_RSP_READ:
	        p_vci_target.cmdack = false;
	        p_vci_target.rspval = true;
	        p_vci_target.rdata = r_tgt_rdata;
	        p_vci_target.rerror = VCI_READ_OK;
	        break;
        case TGT_RSP_WRITE:
	        p_vci_target.cmdack = false;
	        p_vci_target.rspval = true;
	        p_vci_target.rdata  = 0;
	        p_vci_target.rerror = 0;
	        break;
        case TGT_ERROR:
	        p_vci_target.cmdack = false;
	        p_vci_target.rspval = true;
	        p_vci_target.rdata  = 0;
	        p_vci_target.rerror = 1;
	        break;
    } // end switch p_vci_target

    // p_vci_initiator port
    p_vci_initiator.srcid  = (sc_dt::sc_uint<vci_param::S>)m_srcid;
    p_vci_initiator.trdid  = 0;
    p_vci_initiator.contig = true;
    p_vci_initiator.cons   = false;
    p_vci_initiator.wrap   = false;
    p_vci_initiator.cfixed = false;
    p_vci_initiator.clen   = 0;

    switch (r_initiator_fsm) 
    {
        case DMA_DESC_VCI_CMD:
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = true;
	        p_vci_initiator.address = (((uint64_t)(r_dma_pxclbu.read())<<32) | 
                                         r_dma_pxclb.read()) 
                                      + r_dma_slot.read() * sizeof(hba_cmd_desc_t);
	        p_vci_initiator.cmd     = vci_param::CMD_READ;
	        p_vci_initiator.pktid   = TYPE_READ_DATA_UNC; 
	        p_vci_initiator.wdata   = 0;
	        p_vci_initiator.be      = 0;
	        p_vci_initiator.plen    = sizeof(hba_cmd_desc_t);
	        p_vci_initiator.eop     = true;
            break;
        case DMA_LBA_VCI_CMD:	
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = true;
	        p_vci_initiator.address = r_dma_ctba.read(); 
	        p_vci_initiator.cmd     = vci_param::CMD_READ;
	        p_vci_initiator.pktid   = TYPE_READ_DATA_UNC; 
	        p_vci_initiator.wdata   = 0;
	        p_vci_initiator.be      = 0;
	        p_vci_initiator.plen    = sizeof(hba_cmd_header_t);
	        p_vci_initiator.eop     = true;
            break;
        case DMA_BUF_VCI_CMD:	
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = true;
	        p_vci_initiator.address = r_dma_ctba.read() + sizeof(hba_cmd_header_t)
                                      + sizeof(hba_cmd_buffer_t)*r_dma_buffers_count.read();
	        p_vci_initiator.cmd     = vci_param::CMD_READ;
	        p_vci_initiator.pktid   = TYPE_READ_DATA_UNC; 
	        p_vci_initiator.wdata   = 0;
	        p_vci_initiator.be      = 0;
	        p_vci_initiator.plen    = sizeof(hba_cmd_buffer_t);
	        p_vci_initiator.eop     = true;
            break;
        case DMA_TX_VCI_CMD:	
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = true;
	        p_vci_initiator.address = r_dma_buf_paddr.read();
	        p_vci_initiator.cmd     = vci_param::CMD_READ;
	        p_vci_initiator.pktid   = TYPE_READ_DATA_UNC; 
	        p_vci_initiator.wdata   = 0;
	        p_vci_initiator.be      = 0;
	        p_vci_initiator.plen    = m_burst_size;
	        p_vci_initiator.eop     = true;
            break;
        case DMA_RX_VCI_CMD:	
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = true;
	        p_vci_initiator.address = r_dma_buf_paddr.read();
	        p_vci_initiator.cmd     = vci_param::CMD_WRITE;
            p_vci_initiator.wdata   = r_dma_fifo.read();
	        p_vci_initiator.pktid   = TYPE_WRITE; 
	        p_vci_initiator.plen    = m_burst_size;
            if ( vci_param::B == 8 )
            {
                p_vci_initiator.be  = 0xFF;
                p_vci_initiator.eop = ( ((r_burst_size>>3) - r_dma_word.read()) == 1 );
            }
            else
            {
                p_vci_initiator.be  = 0xF;
                p_vci_initiator.eop = ( ((r_burst_size>>2) - r_dma_word.read()) == 1 );
            }
            break;
        case DMA_DESC_VCI_RSP:
        case DMA_LBA_VCI_RSP:
        case DMA_BUF_VCI_RSP:
        case DMA_TX_VCI_RSP:
	        p_vci_initiator.rspack  = true;
	        p_vci_initiator.cmdval  = false;
            break;
        default:
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = false;
            break;
    }  // end switch p_vci_initiator
     
    // ports SDC       
    p_sdc_clk         = r_sdc_clk.read();
    p_sdc_cmd_out     = r_sdc_cmd_out.read();
    p_sdc_cmd_en_out  = r_sdc_cmd_en_out.read(); 
    p_sdc_dat_out[0]  = r_sdc_dat_out[0].read();
    p_sdc_dat_out[1]  = r_sdc_dat_out[1].read();
    p_sdc_dat_out[2]  = r_sdc_dat_out[2].read();
    p_sdc_dat_out[3]  = r_sdc_dat_out[3].read();
    p_sdc_dat_en_out  = r_sdc_dat_en_out.read();

/*
    {
	typename vci_param::data_t v = 0;
	typename vci_param::data_t f;
	p_vci_initiator.rspack  = false;
	p_vci_initiator.cmdval  = true;
	p_vci_initiator.address = (sc_dt::sc_uint<vci_param::N>)r_buf_address.read();
	p_vci_initiator.cmd     = vci_param::CMD_WRITE;
	p_vci_initiator.pktid   = TYPE_WRITE;
	p_vci_initiator.plen    = (sc_dt::sc_uint<vci_param::K>)(m_burst_size);
	f = r_dma_fifo_read.read();
	// byte-swap
	for (int i = 0; i < (vci_param::B * 8); i += 8) {
		v |= ((f >> i) & 0xff) << ((vci_param::B * 8) - 8 - i);
	}
	p_vci_initiator.wdata = v;
	p_vci_initiator.eop   = ( r_burst_word.read() == 0);
	if (vci_param::B == 8)
	{
	    p_vci_initiator.be    = 0xFF;
	}
	else
	{
	    p_vci_initiator.be    = 0xF;
	}
	break;
    }
    case M_READ_RSP:
    case M_WRITE_RSP:
	p_vci_initiator.rspack  = true;
	p_vci_initiator.cmdval  = false;
	break;
    default:
	p_vci_initiator.rspack  = false;
	p_vci_initiator.cmdval  = false;
	break;
    }

    ////////////// SPI signals
    p_spi_ss = ((r_ss & 0x1) == 0);

    switch(r_spi_fsm) 
    {
    default:
	    p_spi_mosi = r_spi_out;
	    p_spi_clk = 0;
	    break;
    case SDC_XMIT:
    {
	    bool s_clk_sample = r_spi_clk ^ r_ctrl_cpha;
	    p_spi_clk = r_spi_clk ^ r_ctrl_cpol;
	    if (s_clk_sample == 0) 
        {
	        // clock low: get data directly from shift register
	        // as r_spi_out may be delayed by one clock cycle
	        p_spi_mosi = (r_sdc_data[(r_ctrl_char_len -1)/ 64] >> ((r_ctrl_char_len - 1) % 64)) & (uint64_t)0x0000000000000001ULL;
	    } 
        else 
        {
	        // clock high: get data from saved value, as the shift register
	        // may have changed
	        p_spi_mosi = r_spi_out;
	    }
	    break;
    }
    }
*/
    // IRQ signal
    p_irq[k] = (((r_dma_pxis[k].read() & 0x1)!=0) and 
                ((r_dma_pxie[k].read() & 0x1)!=0)) or         //ok 
               (((r_dma_pxis[k].read() & 0x40000000)!=0) and 
                ((r_dma_pxie[k].read() & 0x40000000)!=0));    //error

} // end GenMoore()

////////////////////////////////////////////////////////////////
tmpl(/**/)::VciAhciSdc( sc_core::sc_module_name	     name, 
				const soclib::common::MappingTable   &mt,
				const soclib::common::IntTab         &srcid,
				const soclib::common::IntTab         &tgtid,
				const uint32_t		                 burst_size)

    : caba::BaseModule( name ),
      m_seglist( mt.getSegmentList(tgtid) ),
      m_srcid( mt.indexForId(srcid) ),
      m_burst_size( burst_size ),
      m_bursts_per_block( 512 / burst_size ),

      p_clk( "p_clk" ),
      p_resetn( "p_resetn" ),
      p_irq( "p_irq" ),
      p_vci_initiator( "p_vci_initiator" ),
      p_vci_target( "p_vci_target" ),

      p_sdc_clk( "p_sdc_clk" ),
      p_sdc_cmd_out( "p_sdc_cmd_out" ),
      p_sdc_cmd_en_out( "p_sdc_cmd_en_out" ),
      p_sdc_cmd_in( "p_sdc_cmd_in" ),
      p_sdc_dat_out( alloc_elems< sc_out<bool> >( "p_sdc_dat_out" , 4 ) ),
      p_sdc_dat_en_out( "p_sdc_dat_en_out" ),
      p_sdc_dat_in( alloc_elems< sc_in<bool> >( "p_sdc_dat_in" , 4 ) ),

	  r_dma_fifo( "r_dma_fifo", burst_size / vci_param::B ) 
{
    std::cout << "  - Building VciAhciSdc " << name << std::endl;

	SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

	SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

    size_t nbsegs = 0;
    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ ) 
    {
        nbsegs++;
	
	    if ( (seg->baseAddress() & 0x00000FFF) != 0 ) 
	    {
		    std::cout << "Error in component VciAhciSdc : " << name 
		   	          << " The base address of segment " << seg->name()
		              << " must be multiple of 4 Kbytes" << std::endl;
		    exit(1);
	    }
	    if ( seg->size() < 4096 ) 
	    {
		    std::cout << "Error in component VciAhciSdc : " << name 
                      << " The size of segment " << seg->name()
		              << " cannot be smaller than 4096 bytes" << std::endl;
		    exit(1);
	    }

        std::cout << "    => segment " << seg->name()
		          << " / base = " << std::hex << seg->baseAddress()
		          << " / size = " << seg->size() << std::endl; 
    }

    if( nbsegs == 0 )
    {
		std::cout << "Error in component VciAhciSdc : " << name
                  << " No segment allocated" << std::endl;
		exit(1);
    }

    if( (burst_size != 8 ) && 
		(burst_size != 16) && 
		(burst_size != 32) && 
		(burst_size != 64) )
	{
		std::cout << "Error in component VciAhciSdc : " << name 
			  << " The burst size must be 8, 16, 32 or 64 bytes" << std::endl;
		exit(1);
	}

	if ( (vci_param::B != 4) and (vci_param::B != 8) )
	{
		std::cout << "Error in component VciAhciSdc : " << name	      
			  << " The VCI data fields must have 32 bits or 64 bits" << std::endl;
		exit(1);
	}

    if ( (sizeof( hba_cmd_desc_t ) > 16) or
         (sizeof( hba_cmd_header_t ) > 16) or
         (sizeof( hba_cmd_buffer_t ) > 16) )
    {
		std::cout << "Error in component VciAhciSdc : " << name
                  << " The 16 bytes internal buffer is too small" << std::endl;
		exit(1);
    }
     
    // allocate temporary buffer for DMA
    r_dma_store = new char[16];

} // end constructor

/////////////////////////
tmpl(/**/)::~VciAhciSdc()
{
    delete[] r_dma_buffer;
}


//////////////////////////
tmpl(void)::print_trace()
{
	const char* dma_fsm_str[] = 
    {
		"DMA_IDLE",
	};
	const char* tgt_fsm_str[] = 
    {
		"TGT_IDLE",
		"TGT_RSP_READ",
		"TGT_RSP_WRITE",
		"TGT_ERROR",
	};
	const char* sdc_fsm_str[] = 
    {
		"SDC_IDLE",
		"SDC_CMD_PUT",
		"SDC_RSP_GET", 
		"SDC_DMA_RX",
		"SDC_DMA_TX",
	};

	std::cout << name() << " _TGT : " << target_str[r_tgt_fsm.read()] 
	    << std::endl;
	std::cout << name() << " _SPI : " << spi_str[r_spi_fsm.read()] 
	    << " clk_counter " << r_spi_clk_counter.read()
	    << " r_spi_bit_count " << r_spi_bit_count.read() 
	    << " r_spi_bsy " << (int)r_spi_bsy.read() << std::endl;
	std::cout << name() << " _SPI : "
	    << " r_spi_clk " << r_spi_clk.read()
	    << " cpol " << r_ctrl_cpol.read()
	    << " cpha " << r_ctrl_cpha.read()
	    << " r_spi_clk_ignore " << r_spi_clk_ignore.read()
	    << " r_sdc_data 0x" << std::hex
	    << r_sdc_data[1].read() << " " << r_sdc_data[0].read()

	    << std::endl;
	std::cout << name() << "  _INI : " << initiator_str[r_initiator_fsm.read()] 
	  << "  buf = " << std::hex << r_buf_address.read()
	  << "  burst = " << r_burst_word.read() 
	  << "  count = " << r_dma_count.read() 
	  << "  spi_count = " << r_spi_word_count.read() 
	  <<std::endl; 
}

}} // end namespace

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


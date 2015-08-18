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
#include <assert.h>
#include "vci_ahci_sdc.h"
#include "ahci_sdc.h"
#include "alloc_elems.h"

namespace soclib { namespace caba {

#define tmpl(t) template<typename vci_param> t VciAhciSdc<vci_param>

#define  NB_EXTRA_SDC_CYCLES   1   // number of extra SDC cycles after a SDC transaction


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
	    r_sdc_aux_fsm     = SDC_AUX_IDLE;
        r_sdc_clk_period  = 2;
        r_sdc_clk         = false;
        r_sdc_cmd_enable  = false;
        r_sdc_dat_enable  = false;

        r_tgt2sdc_req     = false;
        r_dma2sdc_req     = false;

	    r_fifo->init();

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
		    bool     write   = ( p_vci_target.cmd.read() == vci_param::CMD_WRITE );
            uint32_t cell    = (uint32_t)((address & 0xFF)>>2);
            uint32_t wdata;

            // get wdata for both 32 bits and 64 bits data width
            if( (vci_param::B == 8) and (p_vci_target.be.read() == 0xF0) )
                wdata = (uint32_t)(p_vci_target.wdata.read()>>32);
            else
                wdata = (uint32_t)(p_vci_target.wdata.read());

            // check address errors
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
            else if ( p_vci_target.cmd.read() != vci_param::CMD_READ &&
		              p_vci_target.cmd.read() != vci_param::CMD_WRITE )
            {
	    	    r_tgt_fsm = TGT_ERROR;
	        } 

            //////////////// commands to SDC FSM /////////////////////
            else if ( (cell == SDC_PERIOD) and write )    // set SDC Clock period
            {
                if ( wdata < 2 )
                {
                    r_tgt_fsm = TGT_ERROR;
                }
                else
                {
		            r_tgt_fsm        = TGT_RSP_WRITE;
			        r_sdc_clk_period = wdata;
                }
		    }
            else if ( (cell == SDC_CMD_ARG) and write )   // send SDC command argument 
            {
			    r_tgt_cmd_arg = wdata;
		        r_tgt_fsm     = TGT_RSP_WRITE;
		    }
            else if ( (cell == SDC_CMD_ID) and write )    // set SDC command index, and
                                                          // request access to SD card
            {
                if ( (wdata !=  SDC_CMD0 ) and (wdata != SDC_CMD3 ) and (wdata != SDC_CMD7 ) and 
                     (wdata !=  SDC_CMD8 ) and (wdata != SDC_CMD17) and (wdata != SDC_CMD24) and 
                     (wdata !=  SDC_CMD41) )   
                {
		            r_tgt_fsm = TGT_ERROR;
                }
                else
                {
			        r_tgt_cmd_id  = wdata;
                    r_tgt2sdc_req = true;  
		            r_tgt_fsm     = TGT_RSP_WRITE;
                }
		    }
            else if ( (cell == SDC_RSP_STS) and not write )   // get response status
            {
                if ( r_tgt2sdc_req.read() == false )  // response available         
                {
                    r_tgt_rdata = r_tgt2sdc_rsp.read();
		            r_tgt_fsm   = TGT_RSP_READ;
                }
                else                                  // no response
                {
                    r_tgt_rdata = 0xFFFFFFFF;
		            r_tgt_fsm   = TGT_RSP_READ;
                }
            }

            //////////////////// commands to DMA FSM ///////////////////
            else if( (cell == AHCI_PXCLB) and write )      // set  Command List Base paddr
            {
                r_dma_pxclb = wdata; 
                r_tgt_fsm   = TGT_RSP_WRITE;
            }
            else if( (cell == AHCI_PXCLB) and not write )  // get Command List Base paddr
            {
                r_tgt_rdata = r_dma_pxclb.read(); 
                r_tgt_fsm   = TGT_RSP_READ;
            }
            else if( (cell == AHCI_PXCLBU) and write )     // set  Command List Base extension
            {
                r_dma_pxclbu = wdata; 
                r_tgt_fsm    = TGT_RSP_WRITE;
            }
            else if( (cell == AHCI_PXCLBU) and not write ) // get  Command List Base extension
            {
                r_tgt_rdata = r_dma_pxclbu.read(); 
                r_tgt_fsm   = TGT_RSP_READ;
            }
            else if( (cell == AHCI_PXIS) and write )       // reset PXIS register
            {
                r_dma_pxis = 0;
                r_tgt_fsm  = TGT_RSP_WRITE;
            }
            else if( (cell == AHCI_PXIS) and not write )   // get PXIS register
            {
                r_tgt_rdata = r_dma_pxis.read();  
                r_tgt_fsm   = TGT_RSP_READ;
            }
            else if( (cell == AHCI_PXIE) and write )       // set PXIE register
            {
                r_dma_pxie = wdata; 
                r_tgt_fsm  = TGT_RSP_WRITE;
            }
            else if( (cell == AHCI_PXIE) and not write )   // get PXIE register
            {
                r_tgt_rdata = r_dma_pxie.read();  
                r_tgt_fsm   = TGT_RSP_READ;
            }
            else if( (cell == AHCI_PXCI) and write )       // set only one bit in PXCI
            {
                r_dma_pxci = r_dma_pxci.read() | wdata;
                r_tgt_fsm  = TGT_RSP_WRITE;
            }
            else if( (cell == AHCI_PXCI) and not write )   // get PXCI register
            {
                r_tgt_rdata = r_dma_pxci.read();  
                r_tgt_fsm   = TGT_RSP_READ;
            }
            else if( (cell == AHCI_PXCMD) and write )   
            {
                if( wdata == 0 )                           // dma soft reset
                {
                    r_dma_pxci = 0;
                    r_dma_pxis = 0;
                    r_dma_slot = 0;
                    r_dma_run  = false;
                }
                else                                       // dma activation
                {
                    r_dma_run  = true;
                }
                r_tgt_fsm = TGT_RSP_WRITE;
            }
            else if( (cell == AHCI_PXCMD) and not write )  // get RUN register
            {
                r_tgt_rdata = r_dma_run.read();  
                r_tgt_fsm   = TGT_RSP_READ;
            }
            ///////////// illegal command //////////////
            else           
            {
                r_tgt_fsm  = TGT_ERROR;
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
    // - In case of DMA_TX request from DMA FSM, it send the command to the
    //   SD card on the CMD bus, get and check the response, and send the data 
    //   on the 4 bits DAT bus. It read the data in the DMA_FIFO, and pause 
    //   the SDC clock if FIFO is empty. It get the TX transfer status from
    //   the SD card to analyse posssible CRC error.
    // - In case of DMA_RX request from DMA FSM, it send the command to the
    //   SD card on the CMD bus, and get the received data on the 4 bits DAT bus. 
    //   It writes the data in the DMA_FIFO, and pause the SDC clock if FIFO is full.
    //   It activates an auxiliary SDC_AUX_FSM to receive in parallel the SD card 
    //   response on the CMD bus. 
    //
    // One SDC period is called a "flit". It is composed of N system cycles.
    // The N value is defined by the software controlled r_sdc_clk_period register.
    // The SDC clock is generated from the r_cycles_count counter, that is 
    // incremented in all SDC_FSM states other than the IDLE state.
    // It is LOW on the first half of the SDC_CLK period / HIGH on second half.
    //////////////////////////////////////////////////////////////////////////////

    // checking bus conflict
    if ( p_sdc_cmd_enable_in.read() and r_sdc_cmd_enable.read() )
    {
        std::cout << "error in vci_ahci_sdc: conflict on the SDC CMD bus" << std::endl;
        exit(0);
    }

    if ( p_sdc_dat_enable_in.read() and r_sdc_dat_enable.read() )
    {
        std::cout << "error in vci_ahci_sdc: conflict on the SDC DAT bus" << std::endl;
        exit(0);
    }

    bool cmd_in;
    bool dat_in[4];

	// compute CMD input value (pull-up resistor)
    if ( p_sdc_cmd_enable_in.read() == false ) cmd_in = true;
    else                                       cmd_in = p_sdc_cmd_value_in.read();

	// compute DAT input values (pull-up resistor)
    if ( p_sdc_dat_enable_in.read() == false )  
    {
        dat_in[0] = true;
        dat_in[1] = true;
        dat_in[2] = true;
        dat_in[3] = true;
    }
    else 
    {
        dat_in[0] = p_sdc_dat_value_in[0];
        dat_in[1] = p_sdc_dat_value_in[1];
        dat_in[2] = p_sdc_dat_value_in[2];
        dat_in[3] = p_sdc_dat_value_in[3];
    }

    // cycle, flit, and word definition 
    uint32_t cycle    = r_sdc_cycles_count.read();   // current cycle in SDC period
    uint32_t flit     = r_sdc_flits_count.read();    // current SDC period 
    uint32_t word     = r_sdc_words_count.read();    // current word 

    // significant cycles for DMA_TX or DMA_RX requests
    bool first_cycle  = (cycle == 0);
    bool sample_cycle = (cycle == (r_sdc_clk_period.read() >> 1));
    bool last_cycle   = (cycle == (r_sdc_clk_period.read() - 1));

    // significant cycles for CONFIG requests :
    // the SDC clock period is 64 times larger than for DATA requests
    bool config_first_cycle  = (cycle == 0);
    bool config_sample_cycle = (cycle == ((r_sdc_clk_period.read()   )    ));
    bool config_last_cycle   = (cycle == ((r_sdc_clk_period.read()<<1) - 1));

//  @@@ bool config_sample_cycle = (cycle == ((r_sdc_clk_period.read()<<5)    ));
//  @@@ bool config_last_cycle   = (cycle == ((r_sdc_clk_period.read()<<6) - 1));

    switch (r_sdc_fsm) 
    {
        //////////////
        case SDC_IDLE:   // poll both the DMA_FSM requests and the TGT_FSM requests
        {
            // disable SDC clock and tri-state for CMD and DAT
            r_sdc_clk        = false;
            r_sdc_cmd_enable = false;
            r_sdc_dat_enable = false;
        
            // initializes cycles, flits, and words counters
            r_sdc_cycles_count = 0;
            r_sdc_flits_count  = 0;
            r_sdc_words_count  = 0;

            // initializes CRC registers
            r_sdc_cmd_crc    = 0;
            r_sdc_dat_crc[0] = 0;
            r_sdc_dat_crc[1] = 0;
            r_sdc_dat_crc[2] = 0;
            r_sdc_dat_crc[3] = 0;

            if ( r_tgt2sdc_req.read() )                                // TGT_FSM request
            {
                // set CMD type
                r_sdc_cmd_type = CONFIG_TYPE;

                // initialises data register
                r_sdc_data = (((uint64_t)0x1)                      << 46) | // start bit
                             (((uint64_t)r_tgt_cmd_id.read())      << 40) | // CMD index
                             (((uint64_t)r_tgt_cmd_arg.read())     <<  8) | // CMD argument
                             (((uint64_t)0x1)                           ) ; // stop bit

                r_sdc_fsm = SDC_CMD_SEND;
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

                r_sdc_fsm = SDC_CMD_SEND;
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
  
		        r_sdc_fsm = SDC_CMD_SEND;
	        } 
	        break;
        }
        /////////////
        case SDC_CLK:   // extra flits with SDC clock / no DATA / no CMD
        {
            // Generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // update cycles counter
            if ( last_cycle )  
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = r_sdc_flits_count.read() + 1;
            }
            else
            {
                r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;
            }

            // next state
            if ( last_cycle and (flit == NB_EXTRA_SDC_CYCLES - 1) ) 
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = 0;
                r_sdc_fsm          = SDC_IDLE;
            }
            break;
        }
        //////////////////
        case SDC_CMD_SEND:   // send a 48 flits command
                             // => two nested loops on cycles and on flits
        {
            // frequency is 64 times slower for config access
            if ( r_sdc_cmd_type.read() == CONFIG_TYPE )
            {
                first_cycle  = config_first_cycle;
                sample_cycle = config_sample_cycle;
                last_cycle   = config_last_cycle;
            }

            // Generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // update cycles and flits counters
            if ( last_cycle )  
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = r_sdc_flits_count.read() + 1;
            }
            else
            {
                r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;
            }

            // send value on CMD bus and compute CRC 
            if ( first_cycle )
            {
                if ( flit < 48 )  // send value on CMD bus for the first 48 bits
                {
                    bool value = (bool)((r_sdc_data.read() >> (47 - flit)) & 0x1);

                    // send CMD bit
                    r_sdc_cmd_value  = value;
                    r_sdc_cmd_enable = true;

                    // compute CRC for the first 40 bits
                    if ( flit < 40 ) r_sdc_cmd_crc = crc7( r_sdc_cmd_crc.read() , value ); 
                }
                else             // send only SDC CLK at flit 48
                {
                    r_sdc_cmd_enable = false;
                }
            }

            // insert CRC in data register at last cycle of flit 39 
            if ( last_cycle and (flit == 39) )
            {
                r_sdc_data = r_sdc_data.read() | (r_sdc_cmd_crc.read() << 1) ;
            }

            // next state    
            if ( last_cycle and (flit == 47) )
            {
                if ( r_tgt_cmd_id.read() == 0 ) 
                {
                    // no required response for CMD0 => transaction completed
                    r_sdc_flits_count  = 0;
                    r_sdc_cycles_count = 0;
                    r_tgt2sdc_req      = false;
                    r_sdc_cmd_enable   = false;
                    r_sdc_fsm          = SDC_CLK;
                }
                else if ( r_sdc_cmd_type == DMA_RX_TYPE )  
                {
                    // SDC_AUX FSM activation
                    r_sdc_aux_run   = true;
                    r_sdc_aux_error = false;

                    // go to SDC_DMA_RX state
                    r_sdc_flits_count  = 0;
                    r_sdc_cycles_count = 0;
                    r_sdc_cmd_enable   = false;
                    r_sdc_fsm          = SDC_DMA_RX_START;
                }
                else     
                {
                    // response expected => go to SDC_RSP_START
                    r_sdc_flits_count  = 0;
                    r_sdc_cycles_count = 0;
                    r_sdc_cmd_enable   = false;
                    r_sdc_fsm          = SDC_RSP_START;
                }
            }  
            break;
        }
        ///////////////////
        case SDC_RSP_START:   // wait RSP start bit on CMD bus (one flit)
                              // => one single loop on cycles
        {
            // frequency is 64 times slower for config access
            if ( r_sdc_cmd_type.read() == CONFIG_TYPE )
            {
                first_cycle  = config_first_cycle;
                sample_cycle = config_sample_cycle;
                last_cycle   = config_last_cycle;
            }

            // Generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // update cycles counter
            if ( last_cycle )  r_sdc_cycles_count = 0;
            else               r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;

            // sample the CMD input on the SDC clock rising edge
            if ( sample_cycle ) 
            {
                // register CMD input
                r_sdc_data = r_sdc_data.read() << 1 | cmd_in;

                // compute CRC
                r_sdc_cmd_crc = crc7( 0 , cmd_in );
            }

            // next state
            if ( last_cycle and not cmd_in ) 
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = 1;
                r_sdc_cmd_crc      = crc7( 0 , false );
                r_sdc_data         = 0;
                r_sdc_crc_error    = false;
                r_sdc_fsm          = SDC_RSP_GET;
            }
            break;
        }
        /////////////////
        case SDC_RSP_GET:   // receive RSP token on CMD bus (47 flits)
                            // => two nested loops  on cycles and on flits
        {
            // frequency is 64 times slower for config access
            if ( r_sdc_cmd_type.read() == CONFIG_TYPE )
            {
                first_cycle  = config_first_cycle;
                sample_cycle = config_sample_cycle;
                last_cycle   = config_last_cycle;
            }

            // Generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // update cycles and flits counters
            if ( last_cycle ) 
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = r_sdc_flits_count.read() + 1;
            }
            else   // not the last cycle 
            {
                r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;
            }

            // sample the CMD input on the SDC clock rising edge
            if ( sample_cycle ) 
           {
                // register CMD input
                r_sdc_data = (r_sdc_data.read() << 1) | cmd_in;

                // compute CRC on the first 40 bits
                if ( flit < 40 ) r_sdc_cmd_crc = crc7( r_sdc_cmd_crc.read(), cmd_in );

                // check CRC from flit 40 to flit 46
                if ( (flit >= 40) and (flit<=46) )
                {
                    bool error = (cmd_in != (bool)((r_sdc_cmd_crc.read()>>(46-flit)) & 0x1));
                    r_sdc_crc_error = r_sdc_crc_error.read() or error;
                }
            }
            
            // next state    
            if ( last_cycle and (flit == 47) )
            {
                uint32_t response = (uint32_t)(r_sdc_data.read() >> 8);

                if ( r_sdc_cmd_type.read() == CONFIG_TYPE )   // return rsp to TGT_FSM
                {
                    r_sdc_cycles_count = 0;
                    r_sdc_flits_count  = 0;
                    r_tgt2sdc_req      = false;
                    r_tgt2sdc_rsp      = (r_sdc_crc_error.read()) ? 0xFFFFFFFF : response;
                    r_sdc_fsm          = SDC_CLK;
                }
                if ( r_sdc_cmd_type.read() == DMA_TX_TYPE )   
                {
                    if ( response )                           // report error to DMA_FSM
                    {
                        r_dma2sdc_req   = false;
                        r_dma2sdc_error = true;
                        r_sdc_fsm       = SDC_IDLE;
                    }
                    else                                      // continue DMA_TX transaction
                    {
                        r_sdc_cycles_count = 0;
                        r_sdc_fsm          = SDC_DMA_TX_START;
                    }
                }
                if ( r_sdc_cmd_type.read() == DMA_RX_TYPE )   
                {
                    assert( false and
                    "error in vci_ahci_sdc : for a DMA_RX, the RSP should not be handled"
                    " by the SDC_FSM, as it is handled by the SDC_AUX_FSM" );
                } 
            }
            break;
        }
        //////////////////////
        case SDC_DMA_TX_START:  // send start bit (one flit)
                                // get first word from FIFO 
        {
            // get first word from FIFO at last cycle
            // break if FIFO empty (SDC clock will be stretched)
            if ( last_cycle ) 
            {
                if ( r_fifo->rok() == false )  break;

                r_sdc_data = r_fifo->read();
                fifo_get   = true;
            }

            // generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // update cycles counter
            if ( last_cycle ) r_sdc_cycles_count = 0;
            else              r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;   
                
            // send start bit at first cycle
            if ( first_cycle )
            {
                r_sdc_dat_enable = true;
                r_sdc_dat_value[0] = false;
                r_sdc_dat_value[1] = false;
                r_sdc_dat_value[2] = false;
                r_sdc_dat_value[3] = false;
            }

            // next state 
            if ( last_cycle ) 
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = 0;
                r_sdc_words_count  = 0;
                r_sdc_dat_crc[0]   = 0;
                r_sdc_dat_crc[1]   = 0;
                r_sdc_dat_crc[2]   = 0;
                r_sdc_dat_crc[3]   = 0;
                r_sdc_fsm = SDC_DMA_TX_DATA;
            }
            break;
        }
        /////////////////////
        case SDC_DMA_TX_DATA:  // move one data block from FIFO to SD card (1024 flits)
                               // consume 32 bits or 64 bits words in the DMA FIFO.
                               // - 32 bits word =>  8 flits per word / 128 words per block
                               // - 64 bits word => 16 flits per word /  64 words per block
                               // => three nested loops: on cycles [0 to sdc_period-1]
                               //                        on flits  [0 to m_flit_max-1]
                               //                        on words  [0 to m_word_max-1]
        {
            // get one word from FIFO at last cycle and last flit, 
            // for each word but the last.
            // break if FIFO empty (SDC clock will be stretched)
            if ( last_cycle and (flit == m_flit_max-1) and (word != m_word_max-1) )
            {
                if ( r_fifo->rok() == false )  break;
                
                r_sdc_data = r_fifo->read();
                fifo_get   = true;
            }

            // generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // update cycles, flits, and words counters
            if ( last_cycle ) 
            {
                if ( flit == m_flit_max-1 )          // last cycle / last flit
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
                r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;
            }

            // send data and compute CRC, on SDC clock falling edge 
            if ( first_cycle )
            {
                uint32_t shift = (flit & 0x1) ? (flit - 1) : (flit + 1);

                bool bit0 = (bool)((r_sdc_data.read() >> ((shift<<2)  )) & 0x1);
                bool bit1 = (bool)((r_sdc_data.read() >> ((shift<<2)+1)) & 0x1);
                bool bit2 = (bool)((r_sdc_data.read() >> ((shift<<2)+2)) & 0x1);
                bool bit3 = (bool)((r_sdc_data.read() >> ((shift<<2)+3)) & 0x1);

                r_sdc_dat_value[0] = bit0;
                r_sdc_dat_value[1] = bit1;
                r_sdc_dat_value[2] = bit2;
                r_sdc_dat_value[3] = bit3;

                r_sdc_dat_crc[0] = crc16( r_sdc_dat_crc[0].read() , bit0 );
                r_sdc_dat_crc[1] = crc16( r_sdc_dat_crc[1].read() , bit1 );
                r_sdc_dat_crc[2] = crc16( r_sdc_dat_crc[2].read() , bit2 );
                r_sdc_dat_crc[3] = crc16( r_sdc_dat_crc[3].read() , bit3 );
            }

            // next state
            if ( last_cycle and (word == m_word_max-1) and (flit == m_flit_max-1) )
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = 0;
                r_sdc_fsm          = SDC_DMA_TX_CRC;
            }
            break;
        }
        ////////////////////
        case SDC_DMA_TX_CRC:  // send CRC (16 flits)  
        {
            // Generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // update cycles and flits counters
            if ( last_cycle ) 
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = r_sdc_flits_count.read() + 1;
            }
            else                        
            {
                r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;
            }

            // send data on SDC clock falling edge
            if ( first_cycle )
            {
                r_sdc_dat_value[0] = (bool)((r_sdc_dat_crc[0].read() >> (15-flit)) & 0x1);
                r_sdc_dat_value[1] = (bool)((r_sdc_dat_crc[1].read() >> (15-flit)) & 0x1);
                r_sdc_dat_value[2] = (bool)((r_sdc_dat_crc[2].read() >> (15-flit)) & 0x1);
                r_sdc_dat_value[3] = (bool)((r_sdc_dat_crc[3].read() >> (15-flit)) & 0x1);
            }

            // next state
            if ( last_cycle and (flit == 15) ) r_sdc_fsm = SDC_DMA_TX_STOP;
            break;
        }    
        /////////////////////
        case SDC_DMA_TX_STOP:   // send stop bit (1 flit)
        {
            // Generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // update cycles counter
            r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;
 
            // send stop bit on SDC clock falling edge
            if ( first_cycle ) 
            {
                r_sdc_dat_value[0] = true;
                r_sdc_dat_value[1] = true;
                r_sdc_dat_value[2] = true;
                r_sdc_dat_value[3] = true;
                }
                
            // next state
            if ( last_cycle )
            {
                r_sdc_dat_enable   = false;
                r_sdc_flits_count  = 0;
                r_sdc_cycles_count = 0;
                r_sdc_fsm          = SDC_DMA_TX_ACK0;
            }
            break;
        }
        /////////////////////
        case SDC_DMA_TX_ACK0:  // read STS start bit (one flit)
        {
            // Generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // sample bit on DAT0
            bool value = true; 
            if ( sample_cycle ) value = dat_in[0];

            // update cycles counter
            if ( last_cycle )  r_sdc_cycles_count = 0;
            else               r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;

            // next state
            if ( last_cycle and (value == false) ) 
            {
                r_sdc_flits_count  = 0;
                r_sdc_cycles_count = 0;
                r_sdc_crc_error    = false;
                r_sdc_fsm          = SDC_DMA_TX_ACK4;
            }
            break;
        }
        /////////////////////
        case SDC_DMA_TX_ACK4:  // read STS value on bus DAT0 (4 flits)
                               // signal block completion on the stop bit)
        {
            // Generate SDC clock
            if ( first_cycle  ) r_sdc_clk = false;
            if ( sample_cycle ) r_sdc_clk = true;

            // sample bit on DAT0 / expected pattern is "010"
            if ( sample_cycle and (flit == 0) )
            { 
                bool error = (dat_in[0] == true);
                r_sdc_crc_error = r_sdc_crc_error.read() or error;
            }
            if ( sample_cycle and (flit == 1) )
            { 
                bool error = (dat_in[0] == false);
                r_sdc_crc_error = r_sdc_crc_error.read() or error;
            }
            if ( sample_cycle and (flit == 2) )
            { 
                bool error = (dat_in[0] == true);
                r_sdc_crc_error = r_sdc_crc_error.read() or error;
            }

            // update cycles and flits counters
            if ( last_cycle )
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = r_sdc_flits_count.read() + 1;
            }
            else
            {
                r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;
            }

            // next state
            if ( last_cycle and (flit == 4) )  // stop bit
            {
                r_dma2sdc_req   = false;
                r_dma2sdc_error = r_sdc_crc_error.read();
                r_sdc_fsm       = SDC_IDLE;
            } 
            break;
        }
        //////////////////////
        case SDC_DMA_RX_START:   // wait start bit (one flit)
        {
            // Generate SDC clock
            if ( first_cycle  )   r_sdc_clk = false;
            if ( sample_cycle )   r_sdc_clk = true;

            // update cycles counter
            if ( last_cycle )   r_sdc_cycles_count = 0;
            else                r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;

            // next cycle
            if ( last_cycle and not dat_in[0] )
            {
                assert( not dat_in[1] and not dat_in[2] and not dat_in[3] and
                "error in vci_ahci_sdc: inconsistent start bit on DAT bus");

                r_sdc_data         = 0;
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = 0;
                r_sdc_words_count  = 0;
                r_sdc_fsm          = SDC_DMA_RX_DATA;
            }
            break;
        }   
        /////////////////////
        case SDC_DMA_RX_DATA:  // move ONE data block from SD card to FIFO (1024 flits)
                               // produce 32 bits or 64 bits words to the DMA FIFO.
                               // - 32 bits word =>  8 flits per word / 128 words per block
                               // - 64 bits word => 16 flits per word /  64 words per block
                               // => three nested loops: on cycles [0 to sdc_period-1]
                               //                        on flits  [0 to m_flit_max-1]
                               //                        on words  [0 to m_word_max-1]
        {
            // write previously received word into FIFO, at first cycle and first flit 
            // break if FIFO full => the SDC clock will be stretched
            if ( first_cycle and (flit == 0) and (word > 0) )
            {
                if ( r_fifo->wok() == false )  break;

                fifo_put   = true;
                fifo_wdata = (vci_data_t)r_sdc_data.read();
                r_sdc_data = 0;
            }

            // Generate SDC clock
            if ( first_cycle  )   r_sdc_clk = false;
            if ( sample_cycle )   r_sdc_clk = true;

            // sample DAT inputs, on the SDC clock rising edge 
            if ( sample_cycle ) 
            {
                uint64_t dat = (((uint64_t)dat_in[0])     ) |  
                               (((uint64_t)dat_in[1]) << 1) |  
                               (((uint64_t)dat_in[2]) << 2) |  
                               (((uint64_t)dat_in[3]) << 3) ;  

                // swap the two nibbles in a byte
                uint32_t shift;
                if (flit & 0x1) shift = (flit - 1);
                else            shift = (flit + 1);
                r_sdc_data = r_sdc_data.read() | (dat << (shift<<2));

                r_sdc_dat_crc[0] = crc16( r_sdc_dat_crc[0].read() , dat_in[0] );
                r_sdc_dat_crc[1] = crc16( r_sdc_dat_crc[1].read() , dat_in[1] );
                r_sdc_dat_crc[2] = crc16( r_sdc_dat_crc[2].read() , dat_in[2] );
                r_sdc_dat_crc[3] = crc16( r_sdc_dat_crc[3].read() , dat_in[3] );
            }
            
            // update cycles, flits, and words counters
            if ( last_cycle ) 
            {
                if ( flit  == m_flit_max - 1 )      // last cycle / last flit 
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
                r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;
            }

            // next state
            if ( last_cycle and (flit == m_flit_max-1) and (word == m_word_max-1) )
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = 0;
                r_sdc_crc_error    = false;
                r_sdc_fsm          = SDC_DMA_RX_CRC;
            }
            break;
        }
        ////////////////////
        case SDC_DMA_RX_CRC:   // receive CRC  and check CRC (16 flits)
        {
            // Generate SDC clock
            if ( first_cycle  )   r_sdc_clk = false;
            if ( sample_cycle )   r_sdc_clk = true;

            // write last received word into FIFO, at first cycle and first flit
            // break if FIFO full => the SDC clock will be stretched
            if ( first_cycle and (flit == 0) )
            {
               if ( r_fifo->wok() == false )  break;

               fifo_put   = true;
               fifo_wdata = (vci_data_t)r_sdc_data.read();
            }

            // sample DAT inputs and check CRC on the SDC clock rising edge 
            if ( sample_cycle ) 
            {
                r_sdc_crc_error = r_sdc_crc_error.read() or 
                   (dat_in[0] != (bool)((r_sdc_dat_crc[0].read() >> (15-flit)) & 0x1)) or
                   (dat_in[1] != (bool)((r_sdc_dat_crc[1].read() >> (15-flit)) & 0x1)) or
                   (dat_in[2] != (bool)((r_sdc_dat_crc[2].read() >> (15-flit)) & 0x1)) or
                   (dat_in[3] != (bool)((r_sdc_dat_crc[3].read() >> (15-flit)) & 0x1)) ;
            }

            // update cycles and flits counters
            if ( last_cycle ) 
            {
                r_sdc_cycles_count = 0;
                r_sdc_flits_count  = r_sdc_flits_count.read() + 1;
            }
            else  
            {
                r_sdc_cycles_count = r_sdc_cycles_count + 1;
            }
                
            // next state
            if ( last_cycle and (flit == m_flit_max-1) )
            {
                r_sdc_cycles_count = 0;
                r_sdc_fsm          = SDC_DMA_RX_STOP;
            }
            break;
        }  
        /////////////////////
        case SDC_DMA_RX_STOP:   // receive stop bit (1 flit) / signal block completion
                                // after checking CRC status error detected by SDC_AUX FSM
                                
        {
            // check SDC_AUX FSM status
            assert( (r_sdc_aux_run == false) and
            "error in vci_ahci_sdc: SDC_AUX FSM should be idle in SDC_DMA_RX_STOP state");

            // Generate SDC clock
            if ( first_cycle  )   r_sdc_clk = false;
            if ( sample_cycle )   r_sdc_clk = true;

            // update cycles counter
            r_sdc_cycles_count = r_sdc_cycles_count.read() + 1;
 
            // next state
            if ( last_cycle )
            {
                r_dma2sdc_req      = false;
                r_dma2sdc_error    = r_sdc_crc_error.read();
                r_sdc_fsm          = SDC_IDLE;
            }
            break;
        }  
    }   // end SDC_FSM



    //////////////////////////////////////////////////////////////////////////////////
    // This auxiliary SDC_AUX_FSM is used to receive rhe response to a CMD17
    // command (DMA_RX) that must be handled in parallel with the receicved data. 
    // This FSM uses the r_sdc_cycles_count counter written by the SDC_FSM.   
    // It only checks if the 32 status bits are all 0, register the result in
    // the .
    //////////////////////////////////////////////////////////////////////////////////
    switch( r_sdc_aux_fsm.read() )
    {
        //////////////////
        case SDC_AUX_IDLE:   // exit IDLE when SDC_FSM enters SDC_DMA_RX_START state
        {
            if ( r_sdc_aux_run.read() ) r_sdc_aux_fsm = SDC_AUX_RSP_START;
            break;
        }
        ///////////////////////
        case SDC_AUX_RSP_START:   // wait start bit on CMD bus (1 flit)
        {
            // next state
            if ( last_cycle and not cmd_in ) 
            {
                r_sdc_aux_flits = 0;
                r_sdc_aux_error = false;
                r_sdc_aux_fsm   = SDC_AUX_RSP_DATA;
            }
            break;
        }
        //////////////////////    
        case SDC_AUX_RSP_DATA:   // get RSP token on CMD bus (47 flits)
                                 // => two nested loops  on cycles and on flits
        {
            // sample CMD bus on SDC clock rising edge for all status bits
            uint32_t flit = r_sdc_aux_flits.read();
            if ( sample_cycle and (flit > 7) and (flit < 40) )
            {
                r_sdc_aux_error = r_sdc_aux_error.read() or cmd_in;
            }

            // update local flits counter
            if ( last_cycle ) r_sdc_aux_flits = r_sdc_aux_flits.read() + 1;

            // next state    
            if ( last_cycle and (flit == 39) )
            {
                r_sdc_aux_run = false;
                r_sdc_aux_fsm = SDC_AUX_IDLE;
            }
        }
    }  // end switch SDC_AUX_FSM

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
                r_dma_fsm         = DMA_DESC_VCI_CMD;
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
                    uint32_t*  buf = (uint32_t*)(r_dma_store + r_dma_bytes_count.read());
                    *buf = p_vci_initiator.rdata.read();
                    r_dma_bytes_count  = r_dma_bytes_count.read() + 4;
                }
                else                           // VCI DATA = 64 bits
                {
                    uint64_t*  buf = (uint64_t*)(r_dma_store + r_dma_bytes_count.read());
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
            ahci_cmd_desc_t* desc = (ahci_cmd_desc_t*)r_dma_store;
            r_dma_prdtl           = ((((uint32_t)desc->prdtl[1])<<8) | desc->prdtl[0]);
            r_dma_rx              = ( (desc->flag[0] & 0x40) == 0 );
            r_dma_ctba            = ((((uint64_t)desc->ctbau)<<32) | desc->ctba);
            r_dma_bytes_count     = 0;
            r_dma_fsm             = DMA_LBA_VCI_CMD;
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
                    uint32_t*  buf = (uint32_t*)(r_dma_store + r_dma_bytes_count.read());
                    *buf = p_vci_initiator.rdata.read();
                    r_dma_bytes_count  = r_dma_bytes_count.read() + 4;
                }
                else                           // VCI DATA = 64 bits
                {
                    uint64_t*  buf = (uint64_t*)(r_dma_store + r_dma_bytes_count.read());
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
            ahci_cmd_header_t* head = (ahci_cmd_header_t*)r_dma_store;
            r_dma_lba = (((uint32_t)head->lba3)<<24) | (((uint32_t)head->lba2)<<16) |
                        (((uint32_t)head->lba1)<<8 ) | (((uint32_t)head->lba0));
            r_dma_bytes_count   = 0;
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
                    uint32_t*  buf = (uint32_t*)(r_dma_store + r_dma_bytes_count.read());
                    *buf = p_vci_initiator.rdata.read();
                    r_dma_bytes_count  = r_dma_bytes_count.read() + 4;
                }
                else                           // VCI DATA = 64 bits
                {
                    uint64_t*  buf = (uint64_t*)(r_dma_store + r_dma_bytes_count.read());
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
            ahci_cmd_buffer_t* pbuf = (ahci_cmd_buffer_t*)r_dma_store;

            assert( ((pbuf->dba & (m_burst_size-1)) == 0) and 
            "error in vci_ahci_sdc: buffer address not burst aligned");

            assert( ((pbuf->dbc & (m_burst_size-1)) == 0) and 
            "error in vci_ahci_sdc: buffer length not multiple of block size");

            uint32_t nblocks;
            if ( pbuf->dbc & 0x1FF )  nblocks = (pbuf->dbc>>9 ) + 1;
            else                      nblocks = (pbuf->dbc>>9 );
            r_dma_buf_paddr    = ((((uint64_t)pbuf->dbau)<<32) | pbuf->dba);
            r_dma_buf_blocks   = nblocks;
            r_dma_bytes_count  = 0;
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
                r_dma2sdc_req = true;
                if( r_dma_rx.read() ) r_dma_fsm = DMA_RX_START_BURST;
                else                  r_dma_fsm = DMA_TX_START_BURST;
            }
            break;
        }
        ////////////////////////
        case DMA_TX_START_BURST:   // wait FIFO empty to send VCI command
        {
            if (r_fifo->rok() == false )  
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
            if ( r_fifo->wok() == false )  
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
                assert( r_fifo->rok() and
                "error in vci_ahci_sdc component: fifo should not be empty" );
                
                fifo_get   = true;
                if ( vci_param::B == 4 )    // VCI DATA 32 bits
                {
                    if ( r_dma_bytes_count == m_burst_size - 4 ) r_dma_fsm  = DMA_RX_VCI_RSP;
                    else  r_dma_bytes_count = r_dma_bytes_count.read() + 4;
                }
                else                        // VCI DATA 64 bits
                {
/*
Monitor data written to memory buffer
printf("\n@@@ block = %d / burst = %d / word = %d / fifo_out = %016llx\n\n",
       r_dma_blocks_count.read() , 
       r_dma_bursts_count.read() , 
       (r_dma_bytes_count.read()>>3) , 
       (uint64_t)r_fifo->read() );
*/
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
        case DMA_END_BURST:   // update burst_count, block_count, buffer_count,
                              // and update lba after each block
        {
            if ( r_dma_bursts_count.read() < m_bursts_per_block - 1 )   
            {
                // not the last burst => next burst
                r_dma_bursts_count = r_dma_bursts_count.read() + 1;
                if ( r_dma_rx.read() ) r_dma_fsm = DMA_RX_START_BURST;
                else                   r_dma_fsm = DMA_TX_START_BURST;
            }
            else 
            {
                if ( r_dma_blocks_count.read() < r_dma_buf_blocks.read() - 1 )  
                {
                    // last burst / not the last block => next block
                    r_dma_bursts_count = 0;
                    r_dma_blocks_count = r_dma_blocks_count.read() + 1;
                    r_dma_lba          = r_dma_lba.read() + 1;
                    r_dma_fsm          = DMA_START_BLOCK;
                }
                else 
                {
                    if ( r_dma_buffers_count.read() < r_dma_prdtl.read() - 1 )
                    {
                        // last burst / last block / not the last buffer => next buffer
                        r_dma_bursts_count  = 0;
                        r_dma_blocks_count  = 0;
                        r_dma_buffers_count = r_dma_buffers_count.read() + 1;
                        r_dma_lba           = r_dma_lba.read() + 1;
                        r_dma_fsm           = DMA_BUF_VCI_CMD;
                    }
                    else
                    {
                        // last burst / last block / last buffer => completed
                        if ( r_dma2sdc_req == false ) 
                        {
                            r_dma_bursts_count = 0;
                            r_dma_blocks_count = 0;
                            r_dma_buffers_count = 0;
                            if ( not r_dma2sdc_error.read() ) r_dma_fsm = DMA_SUCCESS;
                            else                              r_dma_fsm = DMA_ERROR;
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
                uint32_t buf_id   = r_dma_buffers_count.read();
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

    //////////////////////////////////////////////////////////////////////
    r_fifo->update( fifo_get , fifo_put , fifo_wdata );
    //////////////////////////////////////////////////////////////////////

}  // end transition

//////////////////////
tmpl(void)::genMoore()
{
    ////// p_vci_target port   
    p_vci_target.rsrcid = (sc_dt::sc_uint<vci_param::S>)r_tgt_srcid.read();
    p_vci_target.rtrdid = (sc_dt::sc_uint<vci_param::T>)r_tgt_trdid.read();
    p_vci_target.rpktid = (sc_dt::sc_uint<vci_param::P>)r_tgt_pktid.read();
    p_vci_target.reop   = true;

    switch( r_tgt_fsm.read() ) 
    {
        case TGT_IDLE:
	        p_vci_target.cmdack = true;
	        p_vci_target.rspval = false;
	        break;
        case TGT_RSP_READ:
	        p_vci_target.cmdack = false;
	        p_vci_target.rspval = true;
	        p_vci_target.rdata  = r_tgt_rdata;
	        p_vci_target.rerror = 0;
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

    ////// p_vci_initiator port
    p_vci_initiator.srcid  = (sc_dt::sc_uint<vci_param::S>)m_srcid;
    p_vci_initiator.trdid  = 0;
    p_vci_initiator.contig = true;
    p_vci_initiator.cons   = false;
    p_vci_initiator.wrap   = false;
    p_vci_initiator.cfixed = false;
    p_vci_initiator.clen   = 0;

    switch ( r_dma_fsm.read() ) 
    {
        case DMA_DESC_VCI_CMD:
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = true;
	        p_vci_initiator.address = (((uint64_t)(r_dma_pxclbu.read())<<32) | r_dma_pxclb.read()) 
                                      + r_dma_slot.read() * sizeof(ahci_cmd_desc_t);
	        p_vci_initiator.cmd     = vci_param::CMD_READ;
	        p_vci_initiator.pktid   = TYPE_READ_DATA_UNC; 
	        p_vci_initiator.wdata   = 0;
	        p_vci_initiator.be      = 0;
	        p_vci_initiator.plen    = sizeof(ahci_cmd_desc_t);
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
	        p_vci_initiator.plen    = sizeof(ahci_cmd_header_t);
	        p_vci_initiator.eop     = true;
            break;
        case DMA_BUF_VCI_CMD:	
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = true;
	        p_vci_initiator.address = r_dma_ctba.read() + sizeof(ahci_cmd_header_t)
                                      + sizeof(ahci_cmd_buffer_t)*r_dma_buffers_count.read();
	        p_vci_initiator.cmd     = vci_param::CMD_READ;
	        p_vci_initiator.pktid   = TYPE_READ_DATA_UNC; 
	        p_vci_initiator.wdata   = 0;
	        p_vci_initiator.be      = 0;
	        p_vci_initiator.plen    = sizeof(ahci_cmd_buffer_t);
	        p_vci_initiator.eop     = true;
            break;
        case DMA_TX_VCI_CMD:	
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = true;
	        p_vci_initiator.address = r_dma_buf_paddr.read() +
                                      (r_dma_blocks_count.read()<<9) +
                                      (r_dma_bursts_count.read()*m_burst_size);
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
	        p_vci_initiator.address = r_dma_buf_paddr.read() +
                                      (r_dma_blocks_count.read()<<9) +
                                      (r_dma_bursts_count.read()*m_burst_size) +
                                      (r_dma_bytes_count.read());
	        p_vci_initiator.cmd     = vci_param::CMD_WRITE;
            p_vci_initiator.wdata   = r_fifo->read();
	        p_vci_initiator.pktid   = TYPE_WRITE; 
	        p_vci_initiator.plen    = m_burst_size;
            if ( vci_param::B == 8 )
            {
                p_vci_initiator.be  = 0xFF;
                p_vci_initiator.eop = ( (m_burst_size - r_dma_bytes_count.read()) == 8 );
            }
            else
            {
                p_vci_initiator.be  = 0xF;
                p_vci_initiator.eop = ( (m_burst_size - r_dma_bytes_count.read()) == 4 );
            }
            break;
        case DMA_DESC_VCI_RSP:
        case DMA_LBA_VCI_RSP:
        case DMA_BUF_VCI_RSP:
        case DMA_RX_VCI_RSP:
        case DMA_TX_VCI_RSP:
	        p_vci_initiator.rspack  = true;
	        p_vci_initiator.cmdval  = false;
            break;
        default:
	        p_vci_initiator.rspack  = false;
	        p_vci_initiator.cmdval  = false;
            break;
    }  // end switch p_vci_initiator
     
    //////  SDC ports       
    p_sdc_clk               = r_sdc_clk.read();
    p_sdc_cmd_value_out     = r_sdc_cmd_value.read();
    p_sdc_cmd_enable_out    = r_sdc_cmd_enable.read(); 
    p_sdc_dat_value_out[0]  = r_sdc_dat_value[0].read();
    p_sdc_dat_value_out[1]  = r_sdc_dat_value[1].read();
    p_sdc_dat_value_out[2]  = r_sdc_dat_value[2].read();
    p_sdc_dat_value_out[3]  = r_sdc_dat_value[3].read();
    p_sdc_dat_enable_out    = r_sdc_dat_enable.read();

    ////// IRQ port
    p_irq = (((r_dma_pxis.read() & 0x1)!=0) and ((r_dma_pxie.read() & 0x1)!=0)) or 
            (((r_dma_pxis.read() & 0x40000000)!=0) and ((r_dma_pxie.read() & 0x40000000)!=0));

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
      m_flit_max( (vci_param::B == 4) ?   8 : 16 ),
      m_word_max( (vci_param::B == 4) ? 128 : 64 ),

      p_clk( "p_clk" ),
      p_resetn( "p_resetn" ),
      p_irq( "p_irq" ),
      p_vci_initiator( "p_vci_initiator" ),
      p_vci_target( "p_vci_target" ),

      p_sdc_clk( "p_sdc_clk" ),
      p_sdc_cmd_value_out( "p_sdc_cmd_value_out" ),
      p_sdc_cmd_enable_out( "p_sdc_cmd_enable_out" ),
      p_sdc_cmd_value_in( "p_sdc_cmd_value_in" ),
      p_sdc_cmd_enable_in( "p_sdc_cmd_enable_in" ),
      p_sdc_dat_enable_out( "p_sdc_dat_enable_out" ),
      p_sdc_dat_enable_in( "p_sdc_dat_enable_in" )
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

    if ( (sizeof( ahci_cmd_desc_t ) > 16) or
         (sizeof( ahci_cmd_header_t ) > 16) or
         (sizeof( ahci_cmd_buffer_t ) > 16) )
    {
		std::cout << "Error in component VciAhciSdc : " << name
                  << " The 16 bytes internal buffer is too small" << std::endl;
		exit(1);
    }
     
    // allocate temporary buffer for DMA
    r_dma_store = new char[16];

    // allocate arrays of port
    p_sdc_dat_value_out = alloc_elems<sc_out<bool> >( "p_sdc_dat_value_out" , 4 );
    p_sdc_dat_value_in  = alloc_elems<sc_in<bool> > ( "p_sdc_dat_value_in"  , 4 );

    // allocate fifo
    r_fifo = new GenericFifo<typename vci_param::data_t>( "r_fifo" , burst_size / vci_param::B );

} // end constructor

/////////////////////////
tmpl(/**/)::~VciAhciSdc()
{
    delete[] r_dma_store;
}


//////////////////////////////////////
tmpl(void)::print_trace(uint32_t mode)
{
	const char* dma_fsm_str[] = 
    {
        "DMA_IDLE",
        "DMA_DESC_VCI_CMD",
        "DMA_DESC_VCI_RSP",
        "DMA_DESC_REGISTER",
        "DMA_LBA_VCI_CMD",
        "DMA_LBA_VCI_RSP",
        "DMA_LBA_REGISTER",
        "DMA_BUF_VCI_CMD",
        "DMA_BUF_VCI_RSP",
        "DMA_BUF_REGISTER",
        "DMA_START_BLOCK",
        "DMA_TX_START_BURST",
        "DMA_TX_VCI_CMD",
        "DMA_TX_VCI_RSP",
        "DMA_RX_START_BURST",
        "DMA_RX_VCI_CMD",
        "DMA_RX_VCI_RSP",
        "DMA_END_BURST",
        "DMA_SUCCESS",
        "DMA_ERROR",
        "DMA_BLOCKED",
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
		"SDC_CMD_SEND",
		"SDC_RSP_START", 
		"SDC_RSP_GET", 
		"SDC_DMA_RX_START",
		"SDC_DMA_RX_DATA",
		"SDC_DMA_RX_CRC",
		"SDC_DMA_RX_STOP",
		"SDC_DMA_TX_START",
		"SDC_DMA_TX_DATA",
		"SDC_DMA_TX_CRC",
		"SDC_DMA_TX_STOP",
		"SDC_DMA_TX_ACK0",
		"SDC_DMA_TX_ACK4",
        "SDC_CLK",
	};

    const char* aux_fsm_str[] =
    {
        "SDC_AUX_IDLE",
        "SDC_AUX_RSP_START",
        "SDC_AUX_RSP_DATA",
    };

    const char* sdc_cmd_type_str[] =
    {
        "CONFIG_TYPE",
        "DMA_TX_TYPE",
        "DMA_RX_TYPE",
    };

	std::cout << "AHCI_SDC " << name() << std::endl;

    std::cout << "  " << tgt_fsm_str[r_tgt_fsm.read()]
              << " / REQ = " << std::dec << r_tgt2sdc_req.read()
              << " / CMD" << std::dec << r_tgt_cmd_id.read()
              << " / ARG = " << std::hex << r_tgt_cmd_arg.read() << std::endl;       

    std::cout << "  "  << dma_fsm_str[r_dma_fsm.read()];
    std::cout << " : RUN = "    << std::dec << r_dma_run.read();
    std::cout << " / SLOT = "   << std::dec << (uint32_t)r_dma_slot.read();
    std::cout << " / PXCI = "   << std::hex << (uint32_t)r_dma_pxci.read();
    std::cout << " / PXIS = "   << std::hex << (uint32_t)r_dma_pxis.read();
    std::cout << " / LBA = "    << std::hex << (uint32_t)r_dma_lba.read();
    std::cout << " / BUF = "    << std::hex << r_dma_buf_paddr.read();
    std::cout << " / RX = "     << std::dec << r_dma_rx.read();
    std::cout << " / BURST = "  << std::dec << r_dma_bursts_count.read();
    std::cout << " / REQ = "    << std::dec << r_dma2sdc_req.read();
    std::cout << " / ERROR = "  << std::dec << r_dma2sdc_error.read() << std::endl;

    uint32_t dat = ((uint32_t)p_sdc_dat_value_in[0].read()     ) |
                   ((uint32_t)p_sdc_dat_value_in[1].read() << 1) |
                   ((uint32_t)p_sdc_dat_value_in[2].read() << 2) |
                   ((uint32_t)p_sdc_dat_value_in[3].read() << 3) ;

    std::cout << "  " << sdc_fsm_str[r_sdc_fsm.read()];
    std::cout << " / " << sdc_cmd_type_str[r_sdc_cmd_type.read()];
    std::cout << " / CYCLE = " << std::dec << (uint32_t)r_sdc_cycles_count.read();
    std::cout << " / FLIT = " << std::dec << (uint32_t)r_sdc_flits_count.read();
    std::cout << " / WORD = " << std::dec << (uint32_t)r_sdc_words_count.read(); 
    if ( p_sdc_cmd_enable_in.read() ) std::cout << " / CMD_IN = " << p_sdc_cmd_value_in.read();
    if ( p_sdc_dat_enable_in.read() ) std::cout << " / DAT_IN = " << std::hex << dat;
    if ( r_sdc_crc_error.read() )     std::cout << " / CRC_ERROR_DETECTED";
    std::cout << std::endl;

    std::cout << "  " << aux_fsm_str[r_sdc_aux_fsm.read()] << std::endl;

    std::cout << "  FIFO_STATE = " << r_fifo->filled_status() << std::endl;
}

}} // end namespace

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


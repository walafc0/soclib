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
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "sd_card.h"
#include "ahci_sdc.h"
#include "alloc_elems.h"

namespace soclib { namespace caba {

using namespace soclib::caba;
using namespace soclib::common;

////////////////////////////////////////////////////
uint32_t SdCard::crc7( uint32_t current , bool val )
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

/////////////////////////////////////////////////////
uint32_t SdCard::crc16( uint32_t current , bool val )
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


/////////////////////////
void SdCard::transition()
{
    if ( p_resetn == false )
    {
        r_fsm            = CMD_START;
        r_prev_sdc_clk   = false;
        r_cmd_enable_out = false;
        r_dat_enable_out = false;
        return;
    }

    // register SDC CLOCK value and detect edges
    r_prev_sdc_clk = p_sdc_clk.read();
    bool  rising  = ( not r_prev_sdc_clk.read() ) and p_sdc_clk.read();
    bool  falling = r_prev_sdc_clk.read() and ( not p_sdc_clk.read() );

	// compute CMD input value (modeling pull-up resistor)
    bool  cmd_in;
    if ( p_sdc_cmd_enable_in.read() == false )  cmd_in = true;
    else                                        cmd_in = p_sdc_cmd_value_in.read();

	// compute DAT input values (modeling pull-up resistor)
    bool  dat_in[4];
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

    uint32_t  flit = r_flit.read();

    ///////////////////////////////////////////////////////////////////
    //          Implementation note:
    // - We call "flit" a SDC cycle.
    // - We set the output registers on the SDC CLOCK falling edge, 
    //   that come first for a given flit.
    // - We sample the input signals and set the internal registers 
    //   on the SDC CLOCK rising edge, that come second in the flit.
    ///////////////////////////////////////////////////////////////////

    switch ( r_fsm.read() ) 
    {
        ///////////////
        case CMD_START:    // get CMD start bit (flit 0)
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                if ( cmd_in == false ) 
                {
                    r_flit      = 1;
                    r_cmd_id    = 0;
                    r_cmd_arg   = 0;
                    r_crc_error = false;
                    r_cmd_crc   = crc7( 0 , cmd_in );
                    r_fsm       = CMD_GET_ID;
                }
            }
            break;
        }
        ////////////////
        case CMD_GET_ID:    // get CMD index (from flit 1 to flit 7)
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_flit    = flit + 1;
                r_cmd_crc = crc7( r_cmd_crc.read() , cmd_in );
                if ( flit > 1 )  r_cmd_id = (r_cmd_id.read() << 1) | cmd_in;
                if ( flit == 7 ) r_fsm    = CMD_GET_ARG;
            }
            break;
        }
        /////////////////
        case CMD_GET_ARG:   // get CMD argument (from flit 8 to flit 39)
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_flit    = flit + 1;
                r_cmd_crc = crc7( r_cmd_crc.read() , cmd_in );
                r_cmd_arg = (r_cmd_arg.read() << 1) | cmd_in;
                if ( flit == 39 ) r_fsm = CMD_GET_CRC;
            }
            break;
        }
        /////////////////
        case CMD_GET_CRC:   // get and check each CRC bit (from flit 40 to flit 46)
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_flit = flit + 1;
                bool error = (cmd_in != (bool)((r_cmd_crc.read()>>(46-flit)) & 0x1));
                r_crc_error = r_crc_error.read() or error;
                if ( flit == 46 ) r_fsm = CMD_STOP;
            }
            break;
        }
        //////////////
        case CMD_STOP:    // stop flit on CMD bus : check CRC and build response
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                // initialises response
                uint32_t response = ( r_crc_error.read() ) ? SDC_ERROR_CRC : 0;

                // analyse command 
                if ( r_cmd_id.read() == SDC_CMD0 )         // no response 
                {
                    r_fsm = CMD_START;
                }
                else if ( r_cmd_id.read() == SDC_CMD3 )    // request RCA => R6
                {
                    r_rsp_arg = response >> 8;
                    r_fsm     = CMD_CLEAN;
                } 
                else if ( r_cmd_id.read() == SDC_CMD7 )    // toggle mode => R1
                {
                    r_rsp_arg = response; 
                    r_fsm = CMD_CLEAN;
                } 
                else if ( r_cmd_id.read() == SDC_CMD8 )    // voltage info => R7
                {
                    r_rsp_arg = 0x00000100 | (r_cmd_arg.read() & 0xFF); 
                    r_fsm = CMD_CLEAN;
                } 
                else if ( r_cmd_id.read() == SDC_CMD41)    // voltage negociation => R1
                {
                    r_rsp_arg = response;
                    r_fsm = CMD_CLEAN;
                } 
                else if ( r_cmd_id.read() == SDC_CMD17)    // RX single block => R1
                {
                    if ( r_cmd_arg.read() > m_nblocks_max - 1 ) response |= SDC_ERROR_LBA;
                    r_rsp_arg = response;
                    r_fsm = CMD_CLEAN;
                } 
                else if ( r_cmd_id.read() == SDC_CMD24)    // TX single block => R1
                {
                    if ( r_cmd_arg.read() > m_nblocks_max - 1 ) response |= SDC_ERROR_LBA;
                    r_rsp_arg = response;
                    r_fsm = CMD_CLEAN;
                } 
                else                                       // unsupported command
                {
                    response |= SDC_ERROR_CMD;
                    r_rsp_arg = response;
                    r_fsm = CMD_CLEAN;
                } 
            }
            break;
        }
        ///////////////
        case CMD_CLEAN:   
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_flit    = 0;
                r_cmd_crc = 0;
                r_fsm     = RSP_START;
            }
            break;
        }
        ///////////////
        case RSP_START:   // send start bit on CMD bus (flit 0 to 1)
        {
            if ( falling )
            {
                r_cmd_enable_out = true;
                r_cmd_value_out  = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_cmd_crc = crc7( r_cmd_crc.read() , false );
                r_flit    = flit + 1;
                if ( flit == 1 )r_fsm = RSP_SET_ID;
            }
            break;
        }
        ////////////////
        case RSP_SET_ID:   // send command index on CMD bus (flit 2 to flit 7)
        {
            if ( falling )
            {
                r_cmd_enable_out = true;
                r_cmd_value_out  = (bool)((r_cmd_id.read()>>(7-flit)) & 0x1);
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_cmd_crc = crc7( r_cmd_crc.read() , (bool)((r_cmd_id.read()>>(7-flit)) & 0x1) );
                r_flit    = flit + 1;
                if ( flit == 7 ) r_fsm = RSP_SET_ARG;
            }
            break;
        }
        /////////////////
        case RSP_SET_ARG:  // send status on CMD bus (flit 8 to flit 39)
        {
            if ( falling )
            {
                r_cmd_enable_out = true;
                r_cmd_value_out  = (bool)((r_rsp_arg.read()>>(39-flit)) & 0x1);
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_cmd_crc = crc7( r_cmd_crc.read() , (bool)((r_rsp_arg.read()>>(39-flit)) & 0x1) );
                r_flit    = flit + 1;
                if ( flit == 39 ) r_fsm = RSP_SET_CRC;
            }
            break;
        }
        /////////////////
        case RSP_SET_CRC:  // send CRC on CMD bus (flit 40 to flit 46)
        {
            if ( falling )
            {
                r_cmd_enable_out = true;
                r_cmd_value_out  = (bool)((r_cmd_crc.read()>>(46-flit)) & 0x1);
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_flit = flit + 1;
                if ( flit == 46 ) r_fsm = RSP_STOP;
            }
            break;
        }
        //////////////
        case RSP_STOP:  // send stop bit on CMD bus (flit 47)
        {
            if ( falling )
            {
                r_cmd_enable_out = true;
                r_cmd_value_out  = true;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_flit = 0;
                r_fsm = RSP_CLEAN;
            }
            break;
        }
        ///////////////
        case RSP_CLEAN:  //  select next state
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                if      ( r_cmd_id.read() == SDC_CMD17 )  r_fsm  = RX_BLOCK;
                else if ( r_cmd_id.read() == SDC_CMD24 )  r_fsm  = TX_START;
                else                                      r_fsm  = CMD_START;
            }
            break;
        }
        //////////////
        case RX_BLOCK:   // get one block from storage
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                if ( flit == 0 )
                {  
                    uint64_t seek = (r_cmd_arg.read() << 9);  //  lba * 512

                    assert( (::lseek( m_fd, seek, SEEK_SET ) >= 0 ) and
                    "error in sd_card : cannot access virtual disk");

                    assert( (::read( m_fd, r_block , 512) >= 0 ) and
                    "error in sd_card : cannot read on virtual disk");

//@@@
/*
uint32_t line;
uint32_t word;
printf("\n******** seek = %d ***********************************************\n", 
       (uint32_t)seek );
for ( line = 0 ; line < 32 ; line++ )
{
    // display line index 
    printf("line %x : ", line );

    // display 8*4 bytes hexa
    for ( word=0 ; word<4 ; word++ )
    {
        unsigned int byte  = (line<<4) + (word<<2);
        unsigned int hexa  = (r_block[byte  ]<<24) |
                             (r_block[byte+1]<<16) |
                             (r_block[byte+2]<< 8) |
                             (r_block[byte+3]);
        printf("%08x | ", hexa );
    }
    printf("\n");
}
printf("******************************************************************\n");
*/

                }
                r_flit = flit + 1;
                if ( flit >= m_rx_latency ) 
                {
                    r_fsm  = RX_START;
                }
            }
            break;
        }
        //////////////
        case RX_START:   // send start bit on DAT bus (1 flit)
        {
            if ( falling )
            {
                r_cmd_enable_out   = false;
                r_dat_enable_out   = true;
                r_dat_value_out[0] = false;
                r_dat_value_out[1] = false;
                r_dat_value_out[2] = false;
                r_dat_value_out[3] = false;
            }
            if ( rising )
            {
                r_flit   = 0;
                r_crc[0] = 0;
                r_crc[1] = 0;
                r_crc[2] = 0;
                r_crc[3] = 0;
                r_fsm = RX_DATA;
            }
            break;
        }
        /////////////
        case RX_DATA:    // send data stream on CMD bus (flit 0 to flit 1023)
        {
            uint32_t data;
            if ( (flit & 0x1) == 0 )  data = r_block[flit>>1] >> 4;    // even flit
            else                      data = r_block[flit>>1] & 0xF;   // odd flit

            if ( falling )
            {
                r_cmd_enable_out   = false;
                r_dat_enable_out   = true;
                r_dat_value_out[0] = (bool)((data>>0) & 0x1);
                r_dat_value_out[1] = (bool)((data>>1) & 0x1);
                r_dat_value_out[2] = (bool)((data>>2) & 0x1);
                r_dat_value_out[3] = (bool)((data>>3) & 0x1);
            }
            if ( rising )
            {
                r_flit   = flit + 1;
                r_crc[0] = crc16( r_crc[0].read() , (bool)((data   ) & 0x1) );
                r_crc[1] = crc16( r_crc[1].read() , (bool)((data>>1) & 0x1) );
                r_crc[2] = crc16( r_crc[2].read() , (bool)((data>>2) & 0x1) );
                r_crc[3] = crc16( r_crc[3].read() , (bool)((data>>3) & 0x1) );
                if ( flit == 1023 )
                {
                    r_fsm  = RX_CRC;
                    r_flit = 0;
                }
            }
            break;
        }
        ////////////
        case RX_CRC:    // send CRC on DAT  bus (flit 0 to flit 15)
        {
            if ( falling )
            {
                r_cmd_enable_out   = false;
                r_dat_enable_out   = true;
                r_dat_value_out[0] = (bool)((r_crc[0].read()>>(15-flit)) & 0x1);
                r_dat_value_out[1] = (bool)((r_crc[1].read()>>(15-flit)) & 0x1);
                r_dat_value_out[2] = (bool)((r_crc[2].read()>>(15-flit)) & 0x1);
                r_dat_value_out[3] = (bool)((r_crc[3].read()>>(15-flit)) & 0x1);
            }
            if ( rising )
            {
                r_flit = flit + 1;
                if ( flit == 15 ) r_fsm = RX_STOP;
            }
            break;
        }
        /////////////
        case RX_STOP:   // send stop bit on DAT bus
        {
            if ( falling )
            {
                r_cmd_enable_out   = false;
                r_dat_enable_out   = true;
                r_dat_value_out[0] = true;
                r_dat_value_out[1] = true;
                r_dat_value_out[2] = true;
                r_dat_value_out[3] = true;
            }
            if ( rising )
            {
                r_fsm  = RX_CLEAN;
                r_flit = 0;
            }
            break;
        }
        //////////////
        case RX_CLEAN:   
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_fsm = CMD_START;
            }
            break;
        }
        //////////////
        case TX_START:   // get start bit (one flit) 
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                if ( dat_in[0] == false ) 
                {
                    assert( (dat_in[1] == false) and
                            (dat_in[2] == false) and
                            (dat_in[3] == false) and
                    "error in sd_card component : illegal start value on DAT bus for TX_DMA")
;
                    r_flit      = 0;
                    r_crc_error = false;
                    r_crc[0]    = 0;
                    r_crc[1]    = 0;
                    r_crc[2]    = 0;
                    r_crc[3]    = 0;
                    r_fsm       = TX_DATA;
                }
            }
            break;
        }
        /////////////
        case TX_DATA:   // receive data stream (1024 flits)
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_flit = flit + 1;

                if ( (flit & 0x1) == 0 )   // even flit => msb bits in byte
                {
                    r_block[flit>>1] = (((uint8_t)dat_in[0]) << 4) |
                                       (((uint8_t)dat_in[1]) << 5) |
                                       (((uint8_t)dat_in[2]) << 6) |
                                       (((uint8_t)dat_in[3]) << 7) ;
                }
                else                       // odd flit => lsb bits in byte
                {
                    r_block[flit>>1] = r_block[flit>>1]            |
                                       (((uint8_t)dat_in[0]) << 0) |
                                       (((uint8_t)dat_in[1]) << 1) |
                                       (((uint8_t)dat_in[2]) << 2) |
                                       (((uint8_t)dat_in[3]) << 3) ;
                }

                r_crc[0] = crc16( r_crc[0].read() , dat_in[0] );
                r_crc[1] = crc16( r_crc[1].read() , dat_in[1] );
                r_crc[2] = crc16( r_crc[2].read() , dat_in[2] );
                r_crc[3] = crc16( r_crc[3].read() , dat_in[3] );

                if ( flit == 1023 ) 
                {
                    r_flit = 0;
                    r_fsm  = TX_CRC;
                }
            }
            break;
        }
        ////////////
        case TX_CRC:   // receive CRC16 (flit 0 to flit 15)
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                bool err0 = (dat_in[0] != (bool)((r_crc[0].read()>>(15-flit)) & 0x1));
                bool err1 = (dat_in[1] != (bool)((r_crc[1].read()>>(15-flit)) & 0x1));
                bool err2 = (dat_in[2] != (bool)((r_crc[2].read()>>(15-flit)) & 0x1));
                bool err3 = (dat_in[3] != (bool)((r_crc[3].read()>>(15-flit)) & 0x1));

                r_crc_error = r_crc_error.read() or err0 or err1 or err2 or err3;

                r_flit = flit + 1;

                if ( flit == 15 )
                {
                    r_flit = 0;
                    r_fsm  = TX_STOP;
                }
            }
            break;
        }
        /////////////
        case TX_STOP:  // receive stop bit, do nothing
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_fsm  = TX_BLOCK;
            }
            break;
        }
        //////////////
        case TX_BLOCK:   // write block to virtual disk
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                if ( r_crc_error.read() == false )  // no CRC error => latency cost
                {
                    if ( flit == 0 )
                    {
                        uint64_t seek = (r_cmd_arg.read() << 9);  //  lba * 512

                        assert( (::lseek( m_fd , seek , SEEK_SET ) >= 0 ) and
                        "error in sd_card : cannot access virtual disk");

                        assert( (::write( m_fd, r_block , 512) >= 0 ) and
                        "error in sd_card : cannot write on virtual disk");
                    }
                    r_flit = flit + 1;
                }
                else                               // CRC error => no write / no latency 
                {
                    r_flit = m_tx_latency;
                }

                if ( flit >= m_tx_latency ) 
                {
                    r_flit = 0;
                    r_fsm  = TX_ACK;
                }
            }
            break;
        }
        ////////////
        case TX_ACK:   // send ACK / NACK  on DAT bus  (flit 1 to flit 4) 
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = true;
                if ( flit == 0 ) r_dat_value_out[0] = false;   // start bit
                if ( flit == 1 ) r_dat_value_out[0] =     r_crc_error.read();
                if ( flit == 2 ) r_dat_value_out[0] = not r_crc_error.read();
                if ( flit == 3 ) r_dat_value_out[0] =     r_crc_error.read();
                if ( flit == 4 ) r_dat_value_out[0] = true;   // stop bit
            }
            if ( rising )
            {
                r_flit = flit + 1;
                if ( flit == 4 ) r_fsm  = CMD_START;
            }
            break;
        }
        //////////////
        case TX_CLEAN:  
        {
            if ( falling )
            {
                r_cmd_enable_out = false;
                r_dat_enable_out = false;
            }
            if ( rising )
            {
                r_fsm  = CMD_START;
            }
        }
    }  // end switch FSM
}  // end transition()

///////////////////////
void SdCard::genMoore()
{
    p_sdc_cmd_value_out     = r_cmd_value_out.read();
    p_sdc_cmd_enable_out    = r_cmd_enable_out.read(); 
    p_sdc_dat_value_out[0]  = r_dat_value_out[0].read();
    p_sdc_dat_value_out[1]  = r_dat_value_out[1].read();
    p_sdc_dat_value_out[2]  = r_dat_value_out[2].read();
    p_sdc_dat_value_out[3]  = r_dat_value_out[3].read();
    p_sdc_dat_enable_out    = r_dat_enable_out.read();

} // end GenMoore()

/////////////////////////////////////////////////////////
SdCard::SdCard( sc_core::sc_module_name	  name, 
				const std::string         &filename,
                const uint32_t            rx_latency,
                const uint32_t            tx_latency )

    : caba::BaseModule( name ),

      r_fsm( "r_fsm" ),
      r_cmd_enable_out( "r_cmd_enable_out" ),
      r_dat_enable_out( "r_dat_enable_out" ),

      m_tx_latency( tx_latency ),
      m_rx_latency( rx_latency ),

      p_sdc_clk( "p_sdc_clk" ),
      p_sdc_cmd_value_out( "p_sdc_cmd_value_out" ),
      p_sdc_cmd_enable_out( "p_sdc_cmd_enable_out" ),
      p_sdc_cmd_value_in( "p_sdc_cmd_value_in" ),
      p_sdc_cmd_enable_in( "p_sdc_cmd_enable_in" ),
      p_sdc_dat_enable_out( "p_sdc_dat_enable_out" ),
      p_sdc_dat_enable_in( "p_sdc_dat_enable_in" )

{
    std::cout << "  - Building SdCard " << name << std::endl;

	SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

	SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

    // allocate arrays of ports
    p_sdc_dat_value_out = alloc_elems<sc_out<bool> >( "p_sdc_dat_value_out" , 4 );
    p_sdc_dat_value_in  = alloc_elems<sc_in<bool> > ( "p_sdc_dat_value_in"  , 4 );

    // open file containing the disk image
    m_fd = ::open( filename.c_str() , O_RDWR );
    if ( m_fd < 0 )
    {
        std::cout << "error in sd_card : cannot open file " << filename << std::endl;
        exit(1);
    }

    m_nblocks_max = lseek(m_fd , 0 , SEEK_END) >> 9;
    
    std::cout << "    => disk image " << filename << " successfully open" << std::endl;

} // end constructor

/////////////////////
SdCard::~SdCard()
{
}


//////////////////////////
void SdCard::print_trace()
{
    const char* fsm_str[] =
    {
       "CMD_START",
       "CMD_GET_ID",
       "CMD_GET_ARG",
       "CMD_GET_CRC",
       "CMD_STOP",
       "CMD_CLEAN",

       "RSP_START",
       "RSP_SET_ID",
       "RSP_SET_ARG",
       "RSP_SET_CRC",
       "RSP_STOP",
       "RSP_CLEAN",

       "RX_BLOCK",
       "RX_START",
       "RX_DATA",
       "RX_CRC",
       "RX_STOP",
       "RX_CLEAN",

       "TX_START",
       "TX_DATA",
       "TX_CRC",
       "TX_STOP",
       "TX_BLOCK",
       "TX_ACK",
       "TX_CLEAN",
    };

    uint32_t data = ((uint32_t)p_sdc_dat_value_in[0].read()     ) |
                    ((uint32_t)p_sdc_dat_value_in[1].read() << 1) |
                    ((uint32_t)p_sdc_dat_value_in[2].read() << 2) |
                    ((uint32_t)p_sdc_dat_value_in[3].read() << 3) ;

    std::cout << "SD_CARD : " << fsm_str[r_fsm.read()];
    std::cout << " / FLIT = " << std::dec << r_flit.read();
    std::cout << " / CMD" << r_cmd_id.read();
    std::cout << " / ARG = " << std::hex << r_cmd_arg.read();
    if ( r_prev_sdc_clk.read() and not p_sdc_clk.read() ) std::cout << " / CLK FALL";
    if ( not r_prev_sdc_clk.read() and p_sdc_clk.read() ) std::cout << " / CLK RISE";
    if ( p_sdc_cmd_enable_in.read() ) std::cout << " / CMD_IN = " << p_sdc_cmd_value_in.read();
    if ( p_sdc_dat_enable_in.read() ) std::cout << " / DAT_IN = " << std::hex << data;
    if ( r_crc_error.read() )         std::cout << " / CRC ERROR DETECTED";
    std::cout << std::endl;
}


}} // end namespace

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


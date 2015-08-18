
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
 *         alain.greiner@lip6.fr april 2015
 *
 * Maintainers: alain
 */

//////////////////////////////////////////////////////////////////////////////////////
// This component emulates a SD card respecting the SD bus protocol.
// The actual data storage is implemented as  
// AHCI protocol.
//
// On the SDC side, it use a frequency divider to use a frequency no higher
// than 25 MHZ. The ratio between the host frequency and the SDC frequency
// can be defined by software, but must me an integer value larger than 2.
// The SDC_FREQUENCY register...
// 
// It supports only 32 or 64 bits VCI DATA width, but all addressable registers
// contain 32 bits words. It supports VCI addresss lartger than 32 bits.
//////////////////////////////////////////////////////////////////////////////////////

#ifndef SD_CARD_H
#define SD_CARD_H

#include <systemc>
#include "caba_base_module.h"

namespace soclib {
namespace caba {

using namespace sc_core;

class SdCard
	: public caba::BaseModule
{
private:

    // registers

    sc_signal<bool>        r_prev_sdc_clk;        // to detect SDC CLK edges

    sc_signal<int>         r_fsm;	              // FSM state register
    sc_signal<uint32_t>    r_cmd_id;              // command index
    sc_signal<uint32_t>    r_cmd_arg;             // command argument
    sc_signal<uint32_t>    r_crc[4];              // computed CRC (for both DAT an CMD)
    sc_signal<bool>        r_crc_error;           // CRC error detected
    sc_signal<uint32_t>    r_cmd_crc;             // received CRC  
    sc_signal<uint32_t>    r_rsp_arg;             // response status
    sc_signal<uint32_t>    r_flit;                // flit counter (1 flit = 1 SDC cycle)

    sc_signal<bool>        r_cmd_value_out;
    sc_signal<bool>        r_dat_value_out[4];

    sc_signal<bool>        r_cmd_enable_out; 
    sc_signal<bool>        r_dat_enable_out;
    
    unsigned char          r_block[512];          // one block buffer

    // structure constants
    uint32_t               m_tx_latency;          // SD card write one block latency
    uint32_t               m_rx_latency;          // SD card read one block latency 
    uint32_t               m_nblocks_max;         // virtual disk size (blocks)
    uint32_t               m_fd;                  // file descriptor fot disk image

    // methods
    void       transition();
    void       genMoore();
    uint32_t   crc7( uint32_t current , bool bit );
    uint32_t   crc16( uint32_t current , bool bit );

    // FSM states
    enum 
    {
       CMD_START,
       CMD_GET_ID,
       CMD_GET_ARG,
       CMD_GET_CRC,
       CMD_STOP,
       CMD_CLEAN,

       RSP_START,
       RSP_SET_ID,
       RSP_SET_ARG,
       RSP_SET_CRC,
       RSP_STOP,
       RSP_CLEAN,

       RX_BLOCK,
       RX_START,
       RX_DATA,
       RX_CRC,
       RX_STOP,
       RX_CLEAN,

       TX_START,
       TX_DATA,
       TX_CRC,
       TX_STOP,
       TX_BLOCK,
       TX_ACK,
       TX_CLEAN,
    };

protected:

    SC_HAS_PROCESS(SdCard);

public:

    // ports
    sc_in<bool>                           p_clk;
    sc_in<bool>                           p_resetn;

    sc_in<bool> 					      p_sdc_clk;

    sc_out<bool> 					      p_sdc_cmd_value_out;
    sc_out<bool>                          p_sdc_cmd_enable_out;
    sc_in<bool> 					      p_sdc_cmd_value_in;
    sc_in<bool>                           p_sdc_cmd_enable_in;

    sc_out<bool>*					      p_sdc_dat_value_out;
    sc_out<bool>                          p_sdc_dat_enable_out;
    sc_in<bool>*					      p_sdc_dat_value_in;
    sc_in<bool>                           p_sdc_dat_enable_in;

    void print_trace();

    // Constructor   
    SdCard( sc_module_name      name,
	        const std::string   &filename,
            const uint32_t      rx_latency = 10,
            const uint32_t      tx_latency = 10 );

    ~SdCard();

};

}}

#endif /* SOCLIB_VCI_SPI_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4



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
// This component is a DS card controller with a VCI interface implementing the
// AHCI protocol.
//
// On the SDC side, it use a frequency divider to use a frequency no higher
// than 25 MHZ. The ratio between the host frequency and the SDC frequency
// can be defined by software, but must me an integer value larger than 2.
// 
// It supports both 32 or 64 bits VCI DATA width, but all addressable registers
// contain 32 bits words. It supports VCI addresss larger than 32 bits.
//////////////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_VCI_AHCI_SDC_H
#define SOCLIB_VCI_AHCI_SDC_H

#include <stdint.h>
#include <systemc>
#include <unistd.h>
#include "caba_base_module.h"
#include "mapping_table.h"
#include "vci_initiator.h"
#include "vci_target.h"
#include "generic_fifo.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param> class VciAhciSdc
	: public caba::BaseModule
{
private:

    typedef typename vci_param::srcid_t   vci_srcid_t;
    typedef typename vci_param::trdid_t   vci_trdid_t;
    typedef typename vci_param::pktid_t   vci_pktid_t;
    typedef typename vci_param::data_t    vci_data_t;

    // SDC_FSM registers

    sc_signal<int>		   r_sdc_fsm;	          // SDC FSM state register
    sc_signal<uint32_t>    r_sdc_clk_period;      // system cycles in one SDC clock period
    sc_signal<uint32_t>    r_sdc_cycles_count;    // system cycles counter
    sc_signal<uint32_t>    r_sdc_flits_count;     // SRC periods counter
    sc_signal<uint32_t>    r_sdc_words_count;     // VCI data word counter
    sc_signal<uint32_t>    r_sdc_cmd_crc;         // used for CRC7 in CMD and RSP
    sc_signal<uint32_t>    r_sdc_dat_crc[4];      // used for CRC16 in data transfers
    sc_signal<uint64_t>    r_sdc_data;            // data register used for DATA and CMD
    sc_signal<int>         r_sdc_cmd_type;        // CONFIG_TYPE / DMA_TX_TYPE / DMA_RX_TYPE
    sc_signal<bool>        r_sdc_crc_error;       // used for CRC error signaling

    sc_signal<bool>        r_sdc_clk;             // value written on p_sdc_clk 
    sc_signal<bool>        r_sdc_cmd_value;       // value written on p_sdc_cmd_out
    sc_signal<bool>        r_sdc_cmd_enable;      // value written on p_sdc_cmd_out_en
    sc_signal<bool>        r_sdc_dat_value[4];    // value written on p_sdc_dat_out
    sc_signal<bool>        r_sdc_dat_enable;      // value written on p_sdc_dat_out_en

    // SDC_AUX registers
    sc_signal<int>         r_sdc_aux_fsm;         // SDC_AUX FSM state register
    sc_signal<uint8_t>     r_sdc_aux_flits;       // local flits counter
    sc_signal<bool>        r_sdc_aux_error;       // reported error
    sc_signal<bool>        r_sdc_aux_run;         // set by SDC FSM / reset by SDC_AUX FSM
    
    // DMA_FSM registers
    sc_signal<int>         r_dma_fsm;             // DMA FSM state register

    sc_signal<bool>        r_dma_run;             // addressable: DMA running
    sc_signal<bool>        r_dma_pxie;            // addressable: IRQ enabled
    sc_signal<uint32_t>	   r_dma_pxci;            // addressable: pending commands
    sc_signal<uint32_t>	   r_dma_pxis;            // addressable: IRQ status
    sc_signal<uint32_t>    r_dma_pxclb;           // addressable: command list paddr lsb
    sc_signal<uint32_t>    r_dma_pxclbu;          // addressable: command list paddr msb
    sc_signal<bool>		   r_dma_rx;              // from SD card to memory transfer   
    sc_signal<uint64_t>    r_dma_ctba;            // Command Table Base Address
    sc_signal<uint64_t>    r_dma_buf_paddr;       // Data Buffer current paddr
    sc_signal<uint32_t>    r_dma_buf_blocks;      // Data Buffer size (blocks)
    sc_signal<uint32_t>    r_dma_prdtl;           // number of buffer in a command  
    sc_signal<uint32_t>    r_dma_lba;             // Logical Block Address on SDC   
    sc_signal<uint8_t>	   r_dma_slot;            // slot index in Command List
    sc_signal<uint32_t>    r_dma_bytes_count;     // bytes counter in a burst
    sc_signal<uint32_t>	   r_dma_bursts_count;    // bursts counter in a block
    sc_signal<uint32_t>	   r_dma_blocks_count;    // blocks counter in a buffer
    sc_signal<uint32_t>	   r_dma_buffers_count;   // buffers counter in a command
    char*	               r_dma_store;           // store cmd desc / header / buf desc

    // TGT_FSM registers
    sc_signal<int>         r_tgt_fsm;   	      // TGT FSM state register

    sc_signal<vci_srcid_t> r_tgt_srcid;           // save srcid
    sc_signal<vci_trdid_t> r_tgt_trdid;           // save trdid
    sc_signal<vci_pktid_t> r_tgt_pktid;           // save pktid
    sc_signal<vci_data_t>  r_tgt_rdata;           // save read data
    sc_signal<uint32_t>    r_tgt_cmd_arg;         // SDC command argument
    sc_signal<uint32_t>    r_tgt_cmd_id;          // SDC command index

    sc_signal<bool>        r_tgt2sdc_req;         // set by TGT / reset by SDC
    sc_signal<uint32_t>    r_tgt2sdc_rsp;         // SDC response 

    sc_signal<bool>        r_dma2sdc_req;         // set by DMA / reset by SDC
    sc_signal<bool>        r_dma2sdc_error;       // error reported

    GenericFifo<typename vci_param::data_t>* r_fifo;   // used for both RX & TX


    // structural parameters
    std::list<soclib::common::Segment> m_seglist;
    uint32_t                           m_srcid;        	    // initiator index
    const uint32_t                     m_burst_size;        // number of bytes
    const uint32_t                     m_bursts_per_block;  // number of words in a burst
    const uint32_t                     m_flit_max;          // max flits in a DATA transfer
    const uint32_t                     m_word_max;          // max words in a DATA transfer

    // methods
    void       transition();
    void       genMoore();
    uint32_t   crc7( uint32_t current , bool bit );
    uint32_t   crc16( uint32_t current , bool bit );
    

    //  DMA FSM states
    enum 
    {
        DMA_IDLE,
        DMA_DESC_VCI_CMD,
        DMA_DESC_VCI_RSP,
        DMA_DESC_REGISTER,
        DMA_LBA_VCI_CMD,
        DMA_LBA_VCI_RSP,
        DMA_LBA_REGISTER,
        DMA_BUF_VCI_CMD,
        DMA_BUF_VCI_RSP,
        DMA_BUF_REGISTER,
        DMA_START_BLOCK,
        DMA_TX_START_BURST,
        DMA_TX_VCI_CMD,
        DMA_TX_VCI_RSP,
        DMA_RX_START_BURST,
        DMA_RX_VCI_CMD,
        DMA_RX_VCI_RSP,
        DMA_END_BURST,
        DMA_SUCCESS,
        DMA_ERROR,
        DMA_BLOCKED,
    };

    // TGT FSM states
    enum 
    {
        TGT_IDLE,
        TGT_RSP_READ,
        TGT_RSP_WRITE,
        TGT_ERROR,
    };

    // SDC FSM states
    enum 
    {
        SDC_IDLE,
        SDC_CMD_SEND,
        SDC_RSP_START,
        SDC_RSP_GET,
        SDC_DMA_RX_START,
        SDC_DMA_RX_DATA,
        SDC_DMA_RX_CRC,
        SDC_DMA_RX_STOP,
        SDC_DMA_TX_START,
        SDC_DMA_TX_DATA,
        SDC_DMA_TX_CRC,
        SDC_DMA_TX_STOP,
        SDC_DMA_TX_ACK0,
        SDC_DMA_TX_ACK4,
        SDC_CLK,
    };

    // SDC_AUX FSM states
    enum
    {
        SDC_AUX_IDLE,
        SDC_AUX_RSP_START,
        SDC_AUX_RSP_DATA,
    };

    // SDC command type
    enum 
    {
        CONFIG_TYPE,
        DMA_TX_TYPE,
        DMA_RX_TYPE,
    };

    /* transaction type, pktid field */
    enum transaction_type_e
    {
        // b3 unused
        // b2 READ / NOT READ
        // Si READ
        //  b1 DATA / INS
        //  b0 UNC / MISS
        // Si NOT READ
        //  b1 accès table llsc type SW / other
        //  b2 WRITE/CAS/LL/SC
        TYPE_READ_DATA_UNC          = 0x0,
        TYPE_READ_DATA_MISS         = 0x1,
        TYPE_READ_INS_UNC           = 0x2,
        TYPE_READ_INS_MISS          = 0x3,
        TYPE_WRITE                  = 0x4,
        TYPE_CAS                    = 0x5,
        TYPE_LL                     = 0x6,
        TYPE_SC                     = 0x7,
    };

protected:

    SC_HAS_PROCESS(VciAhciSdc);

public:

    // ports
    sc_in<bool> 					      p_clk;
    sc_in<bool> 					      p_resetn;
    sc_out<bool> 					      p_irq;
    soclib::caba::VciInitiator<vci_param> p_vci_initiator;
    soclib::caba::VciTarget<vci_param>    p_vci_target;

    sc_out<bool> 					      p_sdc_clk;

    sc_out<bool> 					      p_sdc_cmd_value_out;
    sc_out<bool>                          p_sdc_cmd_enable_out;
    sc_in<bool> 					      p_sdc_cmd_value_in;
    sc_in<bool>                           p_sdc_cmd_enable_in;

    sc_out<bool>*					      p_sdc_dat_value_out;  // array
    sc_out<bool>                          p_sdc_dat_enable_out;
    sc_in<bool>*					      p_sdc_dat_value_in;   // array
    sc_in<bool>                           p_sdc_dat_enable_in;

    void print_trace( uint32_t mode = 0 );

    // Constructor   
    VciAhciSdc(
	sc_module_name                      name,
	const soclib::common::MappingTable  &mt,
	const soclib::common::IntTab 	    &srcid,
	const soclib::common::IntTab 	    &tgtid,
    const uint32_t 	                    burst_size = 64);

    ~VciAhciSdc();

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


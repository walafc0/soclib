
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
 * Copyright (c) UPMC, Lip6, Asim  july 2013
 * alain.greiner@lip6.fr / xiaowu.zhang@lip6.fr september 2013
 *
 * Maintainers: alain
 */

//////////////////////////////////////////////////////////////////////////////////////
// This component is a multi-channels disk controller with a VCI interface,
// respecting the ahci software interface. It supports up to 8 channels.
//
// All addressable registers contain 32 bits words. 
// It supports VCI addresss up to 64 bits.
//
// This component can perform data transfers between a virtual disk (implemented
// as a file belonging to the host system) and a buffer in the memory of the 
// virtual prototype. There is on file (one disk image) per channel.
// The filenames vector is an argument of the constructor.
// The number of vector components defines the number of channels.
//
// This component has a DMA capability, and is both a target and an initiator.
// The block size (bytes), and the burst size (bytes) must be power of 2.
// The burst size is typically a cache line. 
// The memory buffers are constrained to be aligned on a burst size boundary,
// and the buffer length must be multiple of the block size.
// Both read and write transfers are supported. An IRQ is optionally
// asserted when a transfer is completed. 
//
// In order to support various protection mechanisms, each channel
// takes 4K bytes in the address space. As the max number of channels is 8,
// the segment size is 32 Kbytes.
// - the 4 bits ADDRESS[5:2] define the target register
// - the 3 bits ADDRESS[14:12] define the selected channel
//
// There is only 6 addressable registers per channel (see ahci_sata.h)
// - AHCI_PXCLB   0x00 (read/write)  Command List Base Address 32 LSB bits
// - AHCI_PXCLBU  0x04 (read/write)  Command List Base Address 32 MSB bits
// - AHCI_PXIS    0x10 (read/write)  Transfer Status:
//                                   . bit0     operation done if no zero
//                                   . bit30    operation error if no zero
//                                   . bit24-28 value of command index if error
//                                   . bit8-23  value of buffer index  if error
// - AHCI_PXIE    0x14 (read/write)  Interrupt Enable if not zero.
// - AHCI_PXCMD   0x18 (read/write)  Channel running if not 0
// - AHCI_PXCI    0x38 (read/write)  Valid commands bit vector (circular FIFO)
//
// For each channel[k], this controler fetch the commands in a channel specific
// Command List at address defined in AHCI_PXCLBU[k] | AHCI_PXCLB[k].
///////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_CABA_VCI_MULTI_AHCI_H
#define SOCLIB_CABA_VCI_MULTI_AHCI_H

#include <stdint.h>
#include <systemc>
#include <unistd.h>
#include "caba_base_module.h"
#include "mapping_table.h"
#include "vci_initiator.h"
#include "vci_target.h"

namespace soclib {
namespace caba {

using namespace sc_core;
    
template<typename vci_param> class VciMultiAhci : public caba::BaseModule
{
private:
    typedef typename vci_param::fast_addr_t   vci_addr_t ;
    typedef typename vci_param::fast_data_t   vci_data_t ;
    typedef typename vci_param::srcid_t       vci_srcid_t;
    typedef typename vci_param::trdid_t       vci_trdid_t;
    typedef typename vci_param::pktid_t       vci_pktid_t;


    // structural constants
    uint32_t                  	   m_srcid;            // VCI initiator index
    uint32_t*                      m_fd;           	   // File descriptor (per channel)
    uint64_t*                      m_device_size;  	   // number of blocks (per channel)
    const uint32_t                 m_words_per_block;  // number of words in a block
    const uint32_t                 m_words_per_burst;  // number of words in a burst
    const uint32_t                 m_bursts_per_block; // number of burst per block
    const uint32_t                 m_latency;      	   // device average access latency
    size_t                         m_channels;         // number of channels   

	// Registers
    sc_signal<int>                 r_tgt_fsm;          // target fsm state register
    sc_signal<vci_srcid_t>         r_tgt_srcid;
    sc_signal<vci_trdid_t>         r_tgt_trdid;
    sc_signal<vci_pktid_t>         r_tgt_pktid;
    sc_signal<uint32_t>            r_rdata;            // value returned fro a read

    sc_signal<int>				   r_cmd_fsm;          // command fsm state register
    sc_signal<size_t>              r_cmd_index;        // selected channel index
    sc_signal<size_t>              r_cmd_words;        // number of words for a write

    sc_signal<int>				   r_rsp_fsm;          // response fsm state register
    sc_signal<size_t>              r_rsp_index;        // received channel index

    sc_signal<int>*                r_channel_fsm;      // channel fsm state register
    sc_signal<uint32_t>*           r_channel_pxclb;    // command list base address lsb
    sc_signal<uint32_t>*           r_channel_pxclbu;   // command list base address msb
    sc_signal<uint32_t>*           r_channel_pxis;     // interrupt status
    sc_signal<uint32_t>*           r_channel_pxie;     // interrupt enable
    sc_signal<uint32_t>*           r_channel_pxci;     // pending commands 
    sc_signal<bool>*               r_channel_run;      // channel running 
    sc_signal<uint32_t>*           r_channel_slot;     // index in Command List
    sc_signal<uint32_t>*           r_channel_offset;   // offset if dba not aligned
    sc_signal<uint64_t>*           r_channel_address;  // requested VCI paddr
    sc_signal<uint32_t>*           r_channel_length;   // requested VCI length (bytes) 
    sc_signal<uint32_t>*           r_channel_word;     // word index in local buffer
    sc_signal<uint64_t>*           r_channel_ctba;     // Command Table Base Address
    sc_signal<uint64_t>*           r_channel_dba;      // Data Buffer Address
    sc_signal<uint32_t>*           r_channel_dbc;      // Data Buffer Count (bytes)
    sc_signal<uint32_t>*           r_channel_prdtl;    // number of buffer in a command  
    sc_signal<bool>*               r_channel_write;    // operation type
    sc_signal<uint32_t>*           r_channel_count;    // byte count
    sc_signal<uint64_t>*           r_channel_lba;      // logic block address on device 
    sc_signal<uint32_t>*           r_channel_bufid;    // buffer index in a command 
    sc_signal<uint32_t>*           r_channel_latency;  // latency counter
    sc_signal<uint32_t>*           r_channel_prdbc;    // number of bytes transfered
    sc_signal<uint32_t>*           r_channel_blocks;   // blocks counter in a buffer 
    sc_signal<uint32_t>*           r_channel_bursts;   // bursts counter   in a block
    sc_signal<bool>*               r_channel_done;     // VCI transaction completed
    sc_signal<bool>*               r_channel_error;    // VCI transaction error

    uint32_t**                     r_channel_buffer;   // block_size bytes (per channel)
   
    std::list<soclib::common::Segment> m_seglist;
    
	// methods
    void transition();
	void genMoore();
    
	// Target FSM states
    enum 
    {
        T_IDLE         ,
        T_WRITE_RSP    ,
        T_READ_RSP     ,
        T_WRITE_ERROR  ,
        T_READ_ERROR   ,
    };
	// Channel FSM states
	enum 
    {
        CHANNEL_IDLE,
        CHANNEL_READ_CMD_DESC_CMD,
        CHANNEL_READ_CMD_DESC_RSP,
        CHANNEL_READ_LBA_CMD,
        CHANNEL_READ_LBA_RSP,
        CHANNEL_READ_BUF_DESC_START,
        CHANNEL_READ_BUF_DESC_CMD,
        CHANNEL_READ_BUF_DESC_RSP,
        CHANNEL_READ_START,
        CHANNEL_READ_CMD,
        CHANNEL_READ_RSP,
        CHANNEL_WRITE_BLOCK,
        CHANNEL_WRITE_START,
        CHANNEL_WRITE_CMD,
        CHANNEL_WRITE_RSP,
        CHANNEL_READ_BLOCK,
        CHANNEL_TEST_LENGTH,
        CHANNEL_SUCCESS,
        CHANNEL_ERROR,
        CHANNEL_BLOCKED,
	};
    enum cmd_fsm_state_e 
    {
        CMD_IDLE,
        CMD_READ,
        CMD_WRITE,
    };
    enum rsp_fsm_state_e 
    {
        RSP_IDLE,
        RSP_READ,
        RSP_WRITE,
    };
    // Error codes values
    enum 
    {
        VCI_READ_OK		= 0,
        VCI_READ_ERROR	= 1,
        VCI_WRITE_OK	= 2,
        VCI_WRITE_ERROR	= 3,
    };
	enum transaction_type_e // for pktid field 
    {
	    // b3 unused
	    // b2 READ / NOT READ
	    // Si READ
	    //  b1 DATA / INS
	    //  b0 UNC / MISS
	    // Si NOT READ
	    //  b1 accès table llsc type SW / other
	    //  b2 WRITE/CAS/LL/SC
	    TYPE_READ_DATA_UNC  = 0x0,
	    TYPE_READ_DATA_MISS = 0x1,
	    TYPE_READ_INS_UNC   = 0x2,
	    TYPE_READ_INS_MISS  = 0x3,
	    TYPE_WRITE          = 0x4,
	    TYPE_CAS            = 0x5,
	    TYPE_LL             = 0x6,
	    TYPE_SC             = 0x7
	};
    
protected:
    
	SC_HAS_PROCESS(VciMultiAhci);
	
public:
	
	// ports
	sc_in<bool> 		    		                      p_clk;
	sc_in<bool> 					                      p_resetn;
	soclib::caba::VciInitiator<vci_param>                 p_vci_initiator;
	soclib::caba::VciTarget<vci_param>                    p_vci_target;
	sc_out<bool>* 					                      p_channel_irq;
	
	void print_trace();

	// Constructor   
	VciMultiAhci( sc_module_name                          name,
                  const soclib::common::MappingTable      &mt,
                  const soclib::common::IntTab 		      &srcid,
                  const soclib::common::IntTab 		      &tgtid,
                  std::vector<std::string>                &filenames,
                  const uint32_t 	                      block_size = 512,
                  const uint32_t 	                      burst_size = 64,
                  const uint32_t	                      latency = 0 );
    
    ~VciMultiAhci();
};

}}

#endif 
// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


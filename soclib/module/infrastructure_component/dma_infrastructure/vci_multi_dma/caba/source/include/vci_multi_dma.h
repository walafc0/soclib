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
 * Copyright (c) UPMC, Lip6, Asim
 *         alain.greiner@lip6.fr
 *
 * Maintainers: alain
 */
#ifndef SOCLIB_VCI_MULTI_DMA_H
#define SOCLIB_VCI_MULTI_DMA_H

#include <stdint.h>
#include <systemc>
#include "vci_target.h"
#include "vci_initiator.h"
#include "caba_base_module.h"
#include "mapping_table.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciMultiDma
	: public caba::BaseModule
{
private:

    // methods
    void transition();
    void genMoore();

    // registers
    sc_signal<int>				            r_tgt_fsm;
    sc_signal<typename vci_param::srcid_t>	r_srcid;
    sc_signal<typename vci_param::trdid_t>	r_trdid;
    sc_signal<typename vci_param::pktid_t>	r_pktid;
    sc_signal<typename vci_param::data_t>	r_rdata;

    sc_signal<int>*				            r_channel_fsm;           // channel[k] state
    sc_signal<bool>*				        r_channel_activate;      // channel[k] activated
    sc_signal<size_t>*                      r_channel_src_offset;    // non aligned bytes [k]
    sc_signal<size_t>*                      r_channel_dst_offset;    // non aligned bytes [k]
    sc_signal<size_t>*                      r_channel_nbytes_first;  // first burst length [k]
    sc_signal<size_t>*                      r_channel_nbytes_second; // second burst length [k]
    sc_signal<uint64_t>*                    r_channel_src_addr;      // source address [k]
    sc_signal<uint64_t>*                    r_channel_dst_addr;      // destination address [k]
    sc_signal<size_t>*				        r_channel_length;        // buffer length (bytes) [k]	
    sc_signal<uint32_t>**	                r_channel_buf;           // local buffer [k]
    sc_signal<bool>*				        r_channel_last;          // last transaction [k]
    sc_signal<bool>*				        r_channel_done;          // transfer completed [k]
    sc_signal<bool>*				        r_channel_error;         // VCI error signaled [k]
    sc_signal<bool>*                        r_channel_no_irq;        // No IRQ when true

    sc_signal<int>				            r_cmd_fsm;
    sc_signal<size_t>				        r_cmd_count;	         // bytes counter for a command
    sc_signal<size_t>				        r_cmd_curr;	             // current byte in a write burst
    sc_signal<size_t>				        r_cmd_index;	         // channel index for a command
    sc_signal<size_t>				        r_cmd_nbytes;            // VCI packet length

    sc_signal<int>				            r_rsp_fsm;
    sc_signal<size_t>				        r_rsp_count;	         // bytes counter for a response
    sc_signal<size_t>				        r_rsp_index;	         // channel index for a response
    sc_signal<size_t>				        r_rsp_nbytes;            // VCI packet length

    // sructural parametert
    std::list<soclib::common::Segment>		m_seglist;
    const size_t				            m_burst_max_length;	     // number of bytes
    const size_t				            m_channels;		         // no more than 8
    const size_t				            m_srcid;

protected:
    SC_HAS_PROCESS(VciMultiDma);

public:
    // FSM states
    enum tgt_fsm_state_e 
    {
        TGT_IDLE,
        TGT_READ,
        TGT_WRITE,
        TGT_ERROR,
    };
    enum channel_fsm_state_e 
    {
        CHANNEL_DONE              = 0,
        CHANNEL_READ_ERROR        = 1,
        CHANNEL_IDLE              = 2,
        CHANNEL_WRITE_ERROR       = 3,
        CHANNEL_READ_START        = 4,
        CHANNEL_READ_REQ_FIRST    = 5,
        CHANNEL_READ_WAIT_FIRST   = 6,
        CHANNEL_READ_REQ_SECOND   = 7,
        CHANNEL_READ_WAIT_SECOND  = 8,
        CHANNEL_WRITE_START       = 9,
        CHANNEL_WRITE_REQ_FIRST   = 10,
        CHANNEL_WRITE_WAIT_FIRST  = 11,
        CHANNEL_WRITE_REQ_SECOND  = 12,
        CHANNEL_WRITE_WAIT_SECOND = 13,
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

    /* transaction type, pktid field, TSAR encoding */
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
        TYPE_SC                     = 0x7
    };

    // ports
    sc_in<bool> 				            p_clk;
    sc_in<bool> 				            p_resetn;
    soclib::caba::VciTarget<vci_param> 		p_vci_target;
    soclib::caba::VciInitiator<vci_param> 	p_vci_initiator;
    sc_out<bool>* 				            p_irq;

    void print_trace();

    VciMultiDma( sc_module_name 			name,
		const soclib::common::MappingTable 	&mt,
		const soclib::common::IntTab 		&srcid,
		const soclib::common::IntTab 		&tgtid,
		const size_t 				        burst_max_length,
                const size_t				channels);
    ~VciMultiDma();

};

}}

#endif /* SOCLIB_VCI_MULTI_DMA_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


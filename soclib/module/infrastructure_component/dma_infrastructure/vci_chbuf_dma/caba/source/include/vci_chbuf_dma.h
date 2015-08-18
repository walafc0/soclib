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
#ifndef SOCLIB_VCI_CHBUF_DMA_H
#define SOCLIB_VCI_CHBUF_DMA_H

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
class VciChbufDma
	: public caba::BaseModule
{
private:

    // methods
    void transition();
    void genMoore();

    // registers
    sc_signal<int>			                r_tgt_fsm;
    sc_signal<typename vci_param::srcid_t>	r_tgt_srcid;
    sc_signal<typename vci_param::trdid_t>	r_tgt_trdid;
    sc_signal<typename vci_param::pktid_t>	r_tgt_pktid;
    sc_signal<typename vci_param::data_t>	r_tgt_rdata;

    sc_signal<int> 	    r_channel_fsm[4];             // channel state
    sc_signal<bool>     r_channel_run[4];             // channel running
    sc_signal<uint32_t> r_channel_buf_size[4];        // single buffer size (bytes)

    sc_signal<uint64_t> r_channel_src_desc[4];        // SRC descriptor base address
    sc_signal<uint32_t> r_channel_src_nbufs[4];       // number of SRC buffers
    sc_signal<uint32_t> r_channel_src_buf_addr[4];    // SRC buffer 32 LSB address
    sc_signal<uint32_t> r_channel_src_sts_addr[4];    // SRC status 32 LSB address
    sc_signal<uint32_t> r_channel_src_ext[4];         // SRC buffer and status ext
    sc_signal<uint32_t> r_channel_src_index[4];       // current SRC buffer index
    sc_signal<bool>     r_channel_src_full[4];        // current SRC buffer status

    sc_signal<uint64_t> r_channel_dst_desc[4];        // DST descriptor address
    sc_signal<uint32_t> r_channel_dst_nbufs[4];       // number of DST buffers
    sc_signal<uint32_t> r_channel_dst_buf_addr[4];    // DST buffer 32 LSB address
    sc_signal<uint32_t> r_channel_dst_sts_addr[4];    // DST status 32 LSB address
    sc_signal<uint32_t> r_channel_dst_ext[4];         // DST buffer and status ext
    sc_signal<uint32_t> r_channel_dst_index[4];       // current DST buffer index
    sc_signal<bool>     r_channel_dst_full[4];        // current DST buffer status 

    sc_signal<uint32_t> r_channel_timer[4];           // cycle counter for polling
    sc_signal<uint32_t> r_channel_period[4];          // status polling period    
    sc_signal<uint32_t> r_channel_todo_bytes[4];      // number of bytes to transfer
    sc_signal<uint32_t> r_channel_read_count[4];      // pipelined read rsp counter 
    sc_signal<uint32_t> r_channel_write_count[4];     // pipelined write rsp counter
    sc_signal<bool>     r_channel_data_error[4];      // data error reported 

    sc_signal<bool>     r_channel_vci_req[4][4];      // valid request to CMD FSM
    sc_signal<int> 	    r_channel_vci_req_type[4][4]; // request type  to CMD FSM

    sc_signal<bool>     r_channel_vci_rsp[4][4];      // valid response from RSP FSM
    sc_signal<bool>     r_channel_vci_rsp_read[4][4]; // read transaction
    sc_signal<bool>     r_channel_vci_rsp_err[4][4];  // response reports an error      

    uint32_t            r_channel_vci_buf[4][4][16];  // data (one burst per buffer)

    sc_signal<int>      r_cmd_fsm;
    sc_signal<size_t>   r_cmd_count;	              // bytes counter in a burst
    sc_signal<uint64_t>	r_cmd_address;	              // VCI address for cmd
    sc_signal<uint32_t> r_cmd_channel;                // channel index for cmd
    sc_signal<uint32_t> r_cmd_burst;                  // sub_channel index for cmd
    sc_signal<uint32_t> r_cmd_bytes;                  // VCI packet length

    sc_signal<int>      r_rsp_fsm;
    sc_signal<size_t>   r_rsp_count;	              // bytes counter in a burst
    sc_signal<uint32_t> r_rsp_channel;	              // channel index for rsp
    sc_signal<uint32_t> r_rsp_burst;                  // sub_channel index for rsp

    // instrumentation counters
    uint32_t                            m_cycles;

    // sructural parameters
    std::list<soclib::common::Segment>	m_seglist;
    const uint32_t			            m_burst_length;	// number of bytes
    const uint32_t                      m_bursts;       // number of pipelined bursts
    const uint32_t			            m_channels;		// no more than 8
    const uint32_t			            m_srcid;        // DMA component SRCID

    enum vci_req_type_e 
    {
        REQ_READ_SRC_DESC,
        REQ_READ_SRC_STATUS,
        REQ_READ_DST_DESC,
        REQ_READ_DST_STATUS,
        REQ_READ_DATA,
        REQ_WRITE_DATA,
        REQ_WRITE_SRC_STATUS,
        REQ_WRITE_DST_STATUS,
    };

protected:
    SC_HAS_PROCESS(VciChbufDma);

public:
    // FSM states
    enum tgt_fsm_state_e {
        TGT_IDLE,
        TGT_READ,
        TGT_WRITE,
        TGT_ERROR,
    };
    enum channel_fsm_state_e {
        CHANNEL_IDLE,

        CHANNEL_DATA_ERROR,
        CHANNEL_SRC_DESC_ERROR,
        CHANNEL_DST_DESC_ERROR,
        CHANNEL_SRC_STATUS_ERROR,
        CHANNEL_DST_STATUS_ERROR,

        CHANNEL_READ_SRC_DESC,
        CHANNEL_READ_SRC_DESC_WAIT,
        CHANNEL_READ_SRC_STATUS,
        CHANNEL_READ_SRC_STATUS_WAIT,
        CHANNEL_READ_SRC_STATUS_DELAY,

        CHANNEL_READ_DST_DESC,
        CHANNEL_READ_DST_DESC_WAIT,
        CHANNEL_READ_DST_STATUS,
        CHANNEL_READ_DST_STATUS_WAIT,
        CHANNEL_READ_DST_STATUS_DELAY,

        CHANNEL_DATA_READ_REQ,
        CHANNEL_DATA_WRITE_REQ,
        CHANNEL_DATA_WRITE_WAIT,
        CHANNEL_DATA_END,

        CHANNEL_SRC_STATUS_WRITE,
        CHANNEL_SRC_STATUS_WRITE_WAIT,
        CHANNEL_DST_STATUS_WRITE,
        CHANNEL_DST_STATUS_WRITE_WAIT,
        CHANNEL_NEXT_BUFFERS,
    };
    enum cmd_fsm_state_e {
        CMD_IDLE,
        CMD_READ,
        CMD_WRITE,
    };
    enum rsp_fsm_state_e {
        RSP_IDLE,
        RSP_READ_SRC_DESC,
        RSP_READ_SRC_STATUS,
        RSP_READ_DST_DESC,
        RSP_READ_DST_STATUS,
        RSP_READ_DATA,
        RSP_WRITE,
    };

    // ports
    sc_in<bool> 				            p_clk;
    sc_in<bool> 				            p_resetn;
    soclib::caba::VciTarget<vci_param> 		p_vci_target;
    soclib::caba::VciInitiator<vci_param> 	p_vci_initiator;
    sc_out<bool>* 				            p_irq;

    void print_trace();

    VciChbufDma( sc_module_name 			name,
		const soclib::common::MappingTable 	&mt,
		const soclib::common::IntTab 		&srcid,
		const soclib::common::IntTab 		&tgtid,
		const uint32_t 				        burst_length,
        const uint32_t				        channels,
        const uint32_t                      bursts = 4 );
    ~VciChbufDma();
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


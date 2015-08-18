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
 *         Alain Greiner <alain.greiner@lip6.fr>
 *
 * Maintainers: alain
 *
 */

#ifndef SOCLIB_VCI_MWMR_DMA_H
#define SOCLIB_VCI_MWMR_DMA_H

#include <systemc>
#include "vci_target.h"
#include "generic_fifo.h"
#include "vci_initiator.h"
#include "caba_base_module.h"
#include "mapping_table.h"
#include "mwmr_dma.h"
#include "coproc_ports.h"

namespace soclib { namespace caba {

using namespace sc_core;
using namespace soclib::common;

////////////////////////////////////////////////////////////////////////////
template<typename vci_param>
class VciMwmrDma : public caba::BaseModule
////////////////////////////////////////////////////////////////////////////
{

typedef enum channel_fsm_states_e
{
    CHANNEL_IDLE,
    CHANNEL_ACK,
    CHANNEL_DMA_DATA_MOVE,
    CHANNEL_MWMR_TICKET_READ,
    CHANNEL_MWMR_TICKET_CAS,
    CHANNEL_MWMR_LOCK_READ,
    CHANNEL_MWMR_STATUS_READ,
    CHANNEL_MWMR_STATUS_UPDT,
    CHANNEL_MWMR_LOCK_RELEASE,
    CHANNEL_MWMR_DATA_MOVE,
    CHANNEL_SUCCESS,
    CHANNEL_ERROR_LOCK,
    CHANNEL_ERROR_DESC,
    CHANNEL_ERROR_DATA,
} ChannelFsmState;

typedef enum cmd_fsm_states_e
{
	CMD_IDLE,
	CMD_TICKET_READ,
	CMD_TICKET_CAS_OLD,
	CMD_TICKET_CAS_NEW,
    CMD_LOCK_READ,
	CMD_STATUS_READ,
	CMD_STATUS_UPDT_STS,
	CMD_STATUS_UPDT_PTR,
	CMD_STATUS_UPDT_PTW,
    CMD_LOCK_RELEASE,
	CMD_DATA_WRITE,
	CMD_DATA_READ,
} CmdFsmState;

typedef enum rsp_fsm_states_e
{
	RSP_IDLE,
	RSP_TICKET_READ,
	RSP_TICKET_CAS,
    RSP_LOCK_READ,
	RSP_STATUS_READ_STS,
	RSP_STATUS_READ_PTR,
	RSP_STATUS_READ_PTW,
	RSP_STATUS_UPDT,
    RSP_LOCK_RELEASE,
	RSP_DATA_WRITE,
	RSP_DATA_READ,
} RspFsmState;

typedef enum tgt_fsm_states_e
{
    TGT_IDLE,
    TGT_READ,
    TGT_WRITE,
} TgtFsmState;

typedef enum request_type_e
{
    TICKET_READ,
    TICKET_CAS,
    LOCK_READ,
    STATUS_READ,
    STATUS_UPDT,
    LOCK_RELEASE,
    DATA_WRITE,
    DATA_READ,
} mwmr_request_t;

typedef enum pktid_type_e
{
    PKTID_READ_DATA_UNC = 0,
    PKTID_WRITE         = 4,
    PKTID_CAS           = 5,
} pktid_types_t;

// Structure containing all registers defining the channel state
typedef struct channel_state_s 
{
    // addressable registers
    uint64_t                buffer_paddr;  // memory buffer base address
    uint64_t                desc_paddr;    // MWMR descriptor base address 
    uint64_t                lock_paddr;    // MWMR lock base address
    uint32_t                buffer_size;   // buffer size (bytes)
    channel_way_t           way;           // TO_COPROC / FROM_COPROC
    channel_mode_t          mode;          // MWMR / DMA_IRQ / DMA_NO_IRQ
    bool                    running;       // channel activated

    // channel FSM state
    sc_signal<uint32_t>     fsm;           // CHANNEL_FSM state
    uint32_t                ticket;        // ticket value for spin_lock
    uint32_t                bursts;        // requested bursts by coproc

    // hardware fifo
    GenericFifo<uint32_t>*  fifo;          // Pointer on the hardware FIFO

    // communication between channel_fsm and cmd_fsm /rsp_fsm
    bool                    request;       // valid request (to CMD_FSM)
    mwmr_request_t          reqtype;       // request type (to CMD_FSM)
    bool                    response;      // valid response (from RSP_FSM)
    bool                    rerror;        // error reported (from RSP_FSM)
    uint32_t                data;          // returned data (from RSP_FSM)

    // registers defining memory buffer state (both MWMR mode and DMA mode)
    uint32_t                ptr;           // word index for read
    uint32_t                ptw;           // word index for write
    uint32_t                sts;           // buffer content (words) 
} channel_state_t;

	typedef typename vci_param::srcid_t vci_srcid_t;
	typedef typename vci_param::trdid_t vci_trdid_t;
	typedef typename vci_param::pktid_t vci_pktid_t;
    typedef typename vci_param::cmd_t   vci_cmd_t;

private:

    void transition();
    void genMoore();

    // structure constants
    const uint32_t          m_srcid;                // VCI initiator ID
    const Segment           m_segment;              // segment (as target)
    const size_t            m_to_coproc_channels;   // number of read channels
    const size_t            m_from_coproc_channels; // number of write channels
    const size_t            m_all_channels;         // total number of channels
    const size_t            m_coproc_config_regs;   // number of config registers
    const size_t            m_coproc_status_regs;   // number of status registers
    const size_t            m_burst_size;           // burst length (bytes)

    // registers for CHANNEL_FSM[k]
	channel_state_t*        r_channel;              // array of channel states

    // registers for CMD FSM
    sc_signal<uint32_t>     r_cmd_fsm;              // CMD_FSM state
	sc_signal<uint32_t>     r_cmd_word;             // word counter for CMD FSM
	sc_signal<uint32_t>     r_cmd_k;                // current channel index 
    
    // registers for RSP FSM
	sc_signal<uint32_t>     r_rsp_fsm;              // RSP_FSM state
	sc_signal<uint32_t>     r_rsp_word;             // word counter for RSP_FSM
	sc_signal<uint32_t>     r_rsp_k;                // current channel index

    // registers for TGT FSM 
	sc_signal<uint32_t>     r_tgt_fsm;              // TGT_FSM state
    uint32_t                r_tgt_data;             // returned value for read
    vci_srcid_t             r_tgt_srcid;            // for rsrcid
    vci_trdid_t             r_tgt_trdid;            // for rtrdid
    vci_pktid_t             r_tgt_pktid;            // for rpktid

    // coprocessor configuration registers
    uint32_t                r_coproc_config[16]; 

protected:
    SC_HAS_PROCESS(VciMwmrDma);

public:
    sc_in<bool>                            p_clk;
    sc_in<bool>                            p_resetn;
    VciTarget<vci_param>                   p_vci_target;
    VciInitiator<vci_param>                p_vci_initiator;
  	ToCoprocOutput<uint32_t, uint8_t> *    p_to_coproc;      // array of ports
  	FromCoprocInput<uint32_t, uint8_t> *   p_from_coproc;    // array of ports
    sc_out<uint32_t> *                     p_config;         // array of ports
    sc_in<uint32_t> *                      p_status;         // array of ports
    sc_out<bool>                           p_irq;

    void print_trace();

	~VciMwmrDma();

	VciMwmrDma( sc_module_name     name,
                const MappingTable &mt,
		        const IntTab       &srcid,
		        const IntTab       &tgtid,
                const size_t       n_to_coproc,
                const size_t       n_from_coproc,
                const size_t       n_config,
                const size_t       n_status,
                const size_t       burst_size );
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


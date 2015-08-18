/* -*- mode: c++; coding: utf-8 -*-
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
 * Copyright (c) UPMC / Lip6, 2010
 *     Alain Greiner <alain.greiner@lip6.fr>
 *
 * Maintainers: alain 
 */

//#include "alloc_elems.h"
#include <limits>

#include "vci_xcache_wrapper_multi.h"

#define SOCLIB_MODULE_DEBUG 1

namespace soclib{ namespace tlmdt {

namespace {
const char *dcache_fsm_state_str[] = {
        "DCACHE_IDLE",
        "DCACHE_WRITE_UPDT",
        "DCACHE_WRITE_REQ",
        "DCACHE_MISS_SELECT",
        "DCACHE_MISS_INVAL",
        "DCACHE_MISS_WAIT",
        "DCACHE_XTN_HIT",
        "DCACHE_XTN_INVAL",
        "DCACHE_XTN_SYNC",
    };
const char *icache_fsm_state_str[] = {
        "ICACHE_IDLE",
        "ICACHE_MISS_SELECT",
        "ICACHE_MISS_INVAL",
        "ICACHE_MISS_WAIT",
        "ICACHE_UNC_WAIT",
    };
const char *cmd_fsm_state_str[] = {
        "CMD_IDLE",
        "CMD_INS_MISS",
        "CMD_INS_UNC",
        "CMD_DATA_MISS",
        "CMD_DATA_UNC",
        "CMD_DATA_WRITE",
    };
const char *rsp_fsm_state_str[] = {
        "RSP_IDLE",
        "RSP_INS_MISS",
        "RSP_INS_UNC",
        "RSP_DATA_MISS",
        "RSP_DATA_UNC",
        "RSP_DATA_WRITE",
    };
}

#define tmpl(x) template<typename vci_param, typename iss_t> x VciXcacheWrapperMulti<vci_param, iss_t>
   
using soclib::common::uint32_log2;

/////////////////////////////////////
tmpl (/**/)::VciXcacheWrapperMulti
/////////////////////////////////////
(
    sc_core::sc_module_name             name,
    const int                           procid,
    const soclib::common::MappingTable  &mt,
    const soclib::common::IntTab        &srcid,
    const size_t                        icache_ways,
    const size_t                        icache_sets,
    const size_t                        icache_words,
    const size_t                        dcache_ways,
    const size_t                        dcache_sets,
    const size_t                        dcache_words,
    const size_t                        wbuf_words,
    const size_t                        wbuf_lines,
    const size_t                        time_quantum,
    const size_t                        max_cycles )
       : sc_module(name)
	   , m_srcid(mt.indexForId(srcid))
	   , m_iss(this->name(), procid)
	   , m_simulation_time(max_cycles*UNIT_TIME)
	   , m_icache_ways(icache_ways)
	   , m_icache_sets(icache_sets)
	   , m_icache_words(icache_words)
	   , m_icache_yzmask((~0)<<(uint32_log2(icache_words) + 2))
	   , m_dcache_ways(dcache_ways)
	   , m_dcache_sets(dcache_sets)
	   , m_dcache_words(dcache_words)
	   , m_dcache_yzmask((~0)<<(uint32_log2(dcache_words) + 2))
	   , m_wbuf("wbuf", wbuf_words, wbuf_lines, dcache_words)
	   , m_icache("icache", icache_ways, icache_sets, icache_words)
	   , m_dcache("dcache", dcache_ways, dcache_sets, dcache_words)
	   , m_cacheability_table(mt.getCacheabilityTable())
	   , p_vci("p_vci")  
{
    assert( (wbuf_lines > 1) and (wbuf_lines < 9) and
    "ERROR in VCI_XCACHE_WRAPPER_MULTI: wbuf_lines must be in [1...8] range");

    assert( (wbuf_words > 1) and (wbuf_words < 17) and
    "ERROR in VCI_XCACHE_WRAPPER_MULTI: wbuf_words must be in [1...16] range");

    assert( ((dcache_words == 1) or (dcache_words == 2) or (dcache_words == 4) or
             (dcache_words == 8) or (dcache_words == 16)) and
    "ERROR in VCI_XCACHE_WRAPPER_MULTI: dcache_words must be in [1,2,4,8,16]");

    assert( ((icache_words == 1) or (icache_words == 2) or (icache_words == 4) or
             (icache_words == 8) or (icache_words == 16)) and
    "ERROR in VCI_XCACHE_WRAPPER_MULTI: icache_words must be in [1,2,4,8,16]");

    // bind VCI initiator port
    p_vci(*this);                     

    // create IRQ registers arrays
    m_irq_value 	= new bool[iss_t::n_irq];
    m_irq_time	    = new sc_core::sc_time[iss_t::n_irq];

    // create and bind IRQ ports array 
    for(unsigned int i=0; i<iss_t::n_irq; i++)
    {
        std::ostringstream irq_name;
        irq_name << "irq" << i;
        p_irq.push_back(new tlm_utils::simple_target_socket_tagged
        <VciXcacheWrapperMulti,32,tlm::tlm_base_protocol_types> (irq_name.str().c_str()));

        p_irq[i]->register_nb_transport_fw( this, 
                                            &VciXcacheWrapperMulti::irq_nb_transport_fw, 
                                            i );
    }

    // set cache info for ISS
    typename iss_t::CacheInfo cache_info;
    cache_info.has_mmu          = false;
    cache_info.icache_line_size = m_icache_words*sizeof(data_t);
    cache_info.icache_assoc     = m_icache_ways;
    cache_info.icache_n_lines   = m_icache_sets;
    cache_info.dcache_line_size = m_dcache_words*sizeof(data_t);
    cache_info.dcache_assoc     = m_dcache_ways;
    cache_info.dcache_n_lines   = m_dcache_sets;
    m_iss.setCacheInfo(cache_info);

    // create buffers for VCI IMISS transactions  
    // Initialize constant values for the VCI IMISS transactions
    m_imiss_data_buf = new unsigned char[m_icache_words<<2];
    m_imiss_be_buf   = new unsigned char[m_icache_words<<2];

    m_imiss_payload.set_command(tlm::TLM_IGNORE_COMMAND);
    m_imiss_payload.set_data_ptr(m_imiss_data_buf);
    m_imiss_payload.set_byte_enable_ptr(m_imiss_be_buf);
    m_imiss_payload.set_data_length( 0 );
    m_imiss_payload.set_byte_enable_length( 0 );

    m_imiss_extension.set_src_id( m_srcid );
    m_imiss_extension.set_pkt_id( 0 );
    m_imiss_payload.set_extension( &m_imiss_extension );
    m_imiss_phase = tlm::BEGIN_REQ;

    // create buffers for VCI DMISS transactions  
    // Initialize constant values for the VCI DMISS transactions
    m_dmiss_data_buf = new unsigned char[m_dcache_words<<2];
    m_dmiss_be_buf   = new unsigned char[m_dcache_words<<2];

    m_dmiss_payload.set_command(tlm::TLM_IGNORE_COMMAND);
    m_dmiss_payload.set_data_ptr(m_dmiss_data_buf);
    m_dmiss_payload.set_byte_enable_ptr(m_dmiss_be_buf);
    m_dmiss_payload.set_data_length( 0 );
    m_dmiss_payload.set_byte_enable_length( 0 );

    m_dmiss_extension.set_src_id( m_srcid );
    m_dmiss_extension.set_pkt_id( 0 );
    m_dmiss_payload.set_extension( &m_dmiss_extension );
    m_dmiss_phase = tlm::BEGIN_REQ;

    // create buffers for VCI WRITE transactions
    // Initialize constant values for the VCI DMISS transactions
    for( size_t k=0 ; k<wbuf_lines ; k++ )
    {
        m_write_data_buf[k] = new unsigned char[wbuf_words<<2];
        m_write_be_buf[k]   = new unsigned char[wbuf_words<<2];

        m_write_payload[k].set_command(tlm::TLM_IGNORE_COMMAND);
        m_write_payload[k].set_data_ptr(m_dmiss_data_buf);
        m_write_payload[k].set_byte_enable_ptr(m_dmiss_be_buf);
        m_write_payload[k].set_data_length( 0 );
        m_write_payload[k].set_byte_enable_length( 0 );

        m_write_extension[k].set_command(VCI_WRITE_COMMAND);
        m_write_extension[k].set_src_id( m_srcid );
        m_write_extension[k].set_trd_id( 0x8 + k );
        m_write_extension[k].set_pkt_id( 0 );
        m_write_payload[k].set_extension( &m_write_extension[k] );
        m_write_phase[k] = tlm::BEGIN_REQ;
    }

    // Initialize constant values for the NULL MESSAGE  transactions
    m_null_payload.set_extension(&m_null_extension);
    m_null_extension.set_null_message();
    m_null_phase = tlm::BEGIN_REQ;

    // initialize payload, and phase for an activity message
    m_activity_payload.set_extension(&m_activity_extension);
    m_activity_phase = tlm::BEGIN_REQ;
 
    // reset processor, write buffer & caches
    m_iss.reset();
    m_wbuf.reset();
    m_icache.reset();
    m_dcache.reset();
  
    // create PDES local time
    m_pdes_local_time = new pdes_local_time(time_quantum * UNIT_TIME);

    // create PDES activity status
    m_pdes_activity_status = new pdes_activity_status();

    // request flip-flops from ICACHE & DCACHE FSMs to VCI_CMD FSM
    m_icache_miss_req    = false;
    m_icache_unc_req     = false;
    m_dcache_miss_req    = false;
    m_dcache_unc_req     = false;

    // response flip-flops from VCI_RSP FSM to the ICACHE or DCACHE FSMs
    m_vci_rsp_data_rok   = false;
    m_vci_rsp_ins_rok    = false;
    m_vci_rsp_data_error = false;
    m_vci_rsp_ins_error  = false;
  
    // flip_flop between VCI RSP port and VCI_RSP FSM
    m_rsp_valid          = false;

    // cache activity counters
    m_cpt_dcache_read    = 0;
    m_cpt_dcache_write   = 0;
    m_cpt_icache_read    = 0;
    m_cpt_icache_write   = 0;

    // instrumentation counters
    m_cpt_exec_ins       = 0;
    m_pc_previous        = 0;

    m_cpt_read           = 0;
    m_cpt_write          = 0;
    m_cpt_data_miss      = 0;
    m_cpt_ins_miss       = 0;
    m_cpt_data_unc       = 0;
    m_cpt_ins_unc        = 0;
    m_cpt_write_cached   = 0;
  
    // Initialize FSM states
    m_icache_fsm        = ICACHE_IDLE;
    m_dcache_fsm        = DCACHE_IDLE;
    m_vci_cmd_fsm       = CMD_IDLE;
    m_vci_rsp_fsm       = RSP_IDLE;

    SC_THREAD(execLoop);
}

//////////////////////////
tmpl(void)::print_stats()
//////////////////////////
{
    float nb_cycles    = (float)m_pdes_local_time->get().value();
    float nb_ins       = (float)m_cpt_exec_ins;
    float nb_imiss     = (float)m_cpt_ins_miss;
    float nb_dmiss     = (float)m_cpt_data_miss;
    float nb_read      = (float)m_cpt_read;

    std::cout << name() << std::endl;
    std::cout << "- CPI                = " << nb_cycles/nb_ins << std::endl;
    std::cout << "- IMISS_RATE         = " << nb_imiss/nb_ins << std::endl;
    std::cout << "- DMISS RATE         = " << nb_dmiss/nb_read << std::endl;
}


/////////////////////////////////////////////////////////////////////////////////////
//           PDES process
/////////////////////////////////////////////////////////////////////////////////////
tmpl (void)::execLoop ()
{
    uint32_t before_time;
    uint32_t after_time;

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "######    [" << name() << "]" << " wake up / time = " 
          << std::dec << m_pdes_local_time->get().value() << std::endl;
#endif

    while( m_pdes_local_time->get() < m_simulation_time )
    {
        // update local time : one cycle
        m_pdes_local_time->add(UNIT_TIME);

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "-----------------------------------------------------------------" << std::endl;
std::cout << name() << " / time = " << std::dec << m_pdes_local_time->get().value()
          << " / " << icache_fsm_state_str[m_icache_fsm]
          << " / " << dcache_fsm_state_str[m_dcache_fsm]
          << " / " << cmd_fsm_state_str[m_vci_cmd_fsm]
          << " / " << rsp_fsm_state_str[m_vci_rsp_fsm] << std::endl;
#endif

        // get processor requests
        m_iss.getRequests( m_ireq, m_dreq );

        // activate ICACHE FSM for one cycle
        icache_fsm();

        // activate DCACHE FSM for one cycle
        dcache_fsm();

#ifdef SOCLIB_MODULE_DEBUG
size_t min, max;  // unused
std::cout << "Instruction Request : " << m_ireq << std::dec << std::endl;
std::cout << "Instruction Response: " << m_irsp << std::dec << std::endl;
std::cout << "Data        Request : " << m_dreq << std::dec << std::endl;
std::cout << "Data        Response: " << m_drsp << std::dec << std::endl;
std::cout << std::dec 
          << "imiss_req = "  << m_icache_miss_req
          << " / dmiss_req = "  << m_dcache_miss_req
          << " / iunc_req = "   << m_icache_unc_req
          << " / dunc_req = "   << m_dcache_unc_req
          << " / wbuf_rok = "   << m_wbuf.rok( &min, &max ) << std::endl;
#endif

        // compute number of executed instructions (for instrumentation)
        if ( (m_ireq.valid and m_irsp.valid)    and 
             (not m_dreq.valid or m_drsp.valid) and
             (m_ireq.addr != m_pc_previous) )
        {
            m_cpt_exec_ins++;
            m_pc_previous = m_ireq.addr;
        }

        // activate ISS for one cycle
        uint32_t	irqs = 0;
        for ( size_t i=0 ; i<iss_t::n_irq ; i++)
        {
            if ( m_irq_value[i] && (m_irq_time[i] <= m_pdes_local_time->get()) ) irqs |= 1<<i;
        }
        m_iss.executeNCycles( 1, 
                              m_irsp, 
                              m_drsp, 
                              irqs );

        // Activate VCI_CMD FSM for one cycle
        vci_cmd_fsm();

        // Activate VCI_RSP FSM for one cycle
        vci_rsp_fsm();

        // deschedule and wait on vci response
        // if processor frozen and no more VCI command to send
        if ( ((m_dcache_fsm == DCACHE_MISS_WAIT) or (m_dcache_fsm == DCACHE_UNC_WAIT) or 
              (m_icache_fsm == ICACHE_MISS_WAIT) or (m_icache_fsm == ICACHE_UNC_WAIT)) and
             (not m_dcache_miss_req)     and (not m_dcache_unc_req)     and 
             (not m_icache_miss_req)     and (not m_icache_unc_req)     and 
             (m_wbuf.empty())            and (not m_rsp_valid )         and
             (not m_vci_rsp_data_rok)    and (not m_vci_rsp_data_error) and
             (not m_vci_rsp_ins_rok)     and (not m_vci_rsp_ins_error)  and
             (m_vci_cmd_fsm == CMD_IDLE) and (m_vci_rsp_fsm == RSP_IDLE) )
        {
            before_time = m_pdes_local_time->get().value();

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "######    [" << name() << "]" << " blocked => deschedule / time = " 
          << std::dec << m_pdes_local_time->get().value() << std::endl;
#endif
            wait( m_rsp_received );

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "######    [" << name() << "]" << " wake up after blocked  / time = " 
          << std::dec << m_pdes_local_time->get().value() << std::endl;
#endif
            after_time  = m_pdes_local_time->get().value();
            if(after_time > before_time) 
            {
                struct iss_t::InstructionResponse 	meanwhile_irsp = ISS_IRSP_INITIALIZER;
                struct iss_t::DataResponse 		    meanwhile_drsp = ISS_DRSP_INITIALIZER;
                m_iss.executeNCycles( after_time - before_time,
                                      meanwhile_irsp, 
                                      meanwhile_drsp, 
                                      0 );
            }
        }

        // send NULL and deschedule if required
        if ( m_pdes_local_time->need_sync() ) 
        {
            m_pdes_local_time->reset_sync();
            m_null_time = m_pdes_local_time->get();
            p_vci->nb_transport_fw( m_null_payload, 
                                    m_null_phase, 
                                    m_null_time );
#ifdef SOCLIB_MODULE_DEBUG
std::cout << "######    [" << name() << "] need sync => send NULL & deschedule / time = " 
          << std::dec << m_pdes_local_time->get().value() << std::endl;
#endif
            wait( sc_core::SC_ZERO_TIME );

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "######    [" << name() << "]" << " wake up after NULL / time = " 
          << std::dec << m_pdes_local_time->get().value() << std::endl;
#endif
        }
    } // end while

    sc_core::sc_stop();

} // end execLoop()

///////////////////////////////////////////////////////////////////////
// This function implements the ICACHE FSM.
// Processor requests are taken into account only in the IDLE state.
// In case of MISS, or in case of uncached instruction, the FSM 
// writes the missing address line in the  m_icache_addr_save register 
// and sets the m_icache_miss_req (or the m_icache_unc_req) flip-flop.
// These request flip-flops are reset by the VCI_CMD FSM when the VCI 
// command has been sent. 
// When the missing instruction is available in the m_dmiss_payload
// buffer, the VCI_RSP FSM set the m_vci_rsp_ins_rok flip-flop.
// In case of bus error, the VCI_RSP FSM sets the m_vci_rsp_ins_error
// flip-flop. These flip-flops are reset by the ICACHE FSM.
///////////////////////////////////////////////////////////////////////
// This function computes the following values:
// - m_icache_fsm
// - m_icache_miss_req (set)
// - m_icache_unc_req (set)
// - m_irsp
///////////////////////////////////////////////////////////////////////
tmpl(void)::icache_fsm()
{
    // default values for m_irsp
    m_irsp.valid       = false;
    m_irsp.error       = false;
    m_irsp.instruction = 0;

    switch(m_icache_fsm) {
    /////////////////
    case ICACHE_IDLE:
    {
        if ( m_ireq.valid ) 
        {
            data_t  icache_ins = 0;
            bool    icache_hit = false;

            m_cpt_icache_read++;

            bool    icache_cacheable = m_cacheability_table[(uint64_t)m_ireq.addr];

            if ( icache_cacheable ) 
            {
                icache_hit = m_icache.read( m_ireq.addr, &icache_ins);
                if ( not icache_hit )       // miss
                {
                    m_cpt_ins_miss++;

                    m_icache_fsm       = ICACHE_MISS_SELECT;
                    m_icache_addr_save = m_ireq.addr;
                    m_icache_word_save = 0;
                    m_icache_miss_req  = true;
                }
                else                        // hit
                {
                    m_irsp.valid       = true;
                    m_irsp.instruction = icache_ins;
                }
            }
            else  // not cacheable
            {
                m_cpt_ins_unc++;

                m_icache_fsm       = ICACHE_UNC_WAIT;
                m_icache_addr_save = m_ireq.addr; 
                m_icache_unc_req   = true;
            } 
        }
        break;
    }
    ////////////////////////
    case ICACHE_MISS_SELECT:     // selects a way in an associative set
    {
        bool    valid;
        addr_t  victim;  // unused

        valid = m_icache.victim_select( m_icache_addr_save,
                                        &victim,
                                        &m_icache_way_save,
                                        &m_icache_set_save );

        if ( valid ) m_icache_fsm = ICACHE_MISS_INVAL;
        else         m_icache_fsm = ICACHE_MISS_WAIT;
        break;
    }
    ///////////////////////
    case ICACHE_MISS_INVAL:         // invalidate the selected slot
    {
        addr_t   nline;  // unused

        m_icache.inval( m_icache_way_save,
                        m_icache_set_save,
                        &nline );

        m_icache_fsm = ICACHE_MISS_WAIT;
        break;
    }
    //////////////////////
    case ICACHE_MISS_WAIT:        // wait response and update cache
    {
        if ( m_vci_rsp_ins_error )    // error reported
        {
            m_irsp.valid         = true;
            m_irsp.error         = true;
            m_vci_rsp_ins_error  = false;
            m_icache_fsm         = ICACHE_IDLE;
        }
        else if ( m_vci_rsp_ins_rok )
        {
            m_cpt_icache_write++;

            uint32_t data = atou( m_imiss_payload.get_data_ptr(),
                                  m_icache_word_save * vci_param::nbytes );

            m_icache.write( m_icache_way_save,
                            m_icache_set_save,
                            m_icache_word_save,
                            data );

            if ( m_icache_word_save == m_icache_words - 1 )  // last word
            {
                m_icache.victim_update_tag( m_icache_addr_save,
                                            m_icache_way_save,
                                            m_icache_set_save );

                m_icache_fsm      = ICACHE_IDLE;
                m_vci_rsp_ins_rok = false;
            }
            m_icache_word_save = m_icache_word_save + 1;
        }
        break;
    }
    /////////////////////
    case ICACHE_UNC_WAIT:
    {
        if ( m_vci_rsp_ins_error )   // error reported 
        {
            m_irsp.valid         = true;
            m_irsp.error         = true;
            m_vci_rsp_ins_error  = false;
            m_icache_fsm         = ICACHE_IDLE;
        }
        else if ( m_vci_rsp_ins_rok ) // available instruction
        {
            uint32_t data = atou( m_imiss_payload.get_data_ptr(), 0 );

            if ( m_ireq.valid and (m_ireq.addr == m_icache_addr_save) )
            {
                m_irsp.valid         = true;
                m_irsp.instruction   = data;
            }
            m_icache_fsm         = ICACHE_IDLE;
            m_vci_rsp_ins_rok = false;
        }
        break;
      
    } } // end switch m_icache_fsm
} // end icache_fsm()

///////////////////////////////////////////////////////////////////////////////////
// This function implements the DCACHE FSM.
// - In order to support write burst, the processor requests are taken 
//   into account in the WRITE_REQ state as well as in the IDLE state.
// - In IDLE state, the request is satisfied if it is a cacheable read hit, 
//   or a cacheable write. 
// - In WRITE_REQ state, the request is satisfied if it is a cacheable read hit,
//   or a cacheable write, only if the write buffer is not full.
// - Both the uncacheable read and the uncachable write requests block the processor
//   until the corresponding VCI transaction is completed.
// 
// It uses an advanced Write buffer that supports several simultaneous write bursts.
//
// In case of processor request, there is six conditions to exit the IDLE state:
//   - CACHED READ MISS       => to the MISS_SELECT state,
//   - UNCACHED READ or WRITE => to the UNC_WAIT state,
//   - XTN_INVAL              => to the XTN_HIT state,
//   - XTN_SYNC               => to the XTN_SYNC state,
//   - WRITE MISS             => to the WRITE_REQ state,
//   - WRITE HIT              => to the WRITE_UPDT state,
//
// The cache access takes into account the cacheability table.
// All LL or SC requests are handled as uncacheable.
//
// Error handling :  Read Bus Errors are synchronous events, but
// Write Bus Errors are asynchronous events (processor is not frozen).
// - If a Read Bus Error is detected, the VCI_RSP FSM sets the
//   m_vci_rsp_data_error flip-flop, and the synchronous error is signaled
//   by the DCACHE FSM.
// - If a Write Bus Error is detected, the VCI_RSP FSM  signals
//   the asynchronous error using the setWriteBerr() method.
////////////////////////////////////////////////////////////////////////
// This function computes and set the following values:
// - m_dcache_fsm
// - m_dcache_miss_req (set)
// - m_dcache_unc_req (set)
// - m_drsp
///////////////////////////////////////////////////////////////////////
tmpl(void)::dcache_fsm()
{
    // default value for m_drsp
    m_drsp.valid   = false;
    m_drsp.error   = false;
    m_drsp.rdata   = 0;

    switch ( m_dcache_fsm ) {
    //////////////////////
    case DCACHE_WRITE_REQ:  // only cacheable write are written in wbuf
    {
        if( !m_wbuf.write( m_dcache_addr_save, 
                           m_dcache_be_save, 
                           m_dcache_wdata_save,
                           true ) ) 
        {
            break;     
        }
        // If the write request is accepted by the write buffer, 
        // the next state and the processor request parameters are computed
        // as in the DCACHE_IDLE state  below ...
    }
    /////////////////
    case DCACHE_IDLE:
    {
        if ( m_dreq.valid ) 
        {
            bool        dcache_cacheable;
            bool        dcache_hit;
            data_t      dcache_rdata = 0;
            size_t      dcache_way   = 0;
            size_t      dcache_set   = 0;
            size_t      dcache_word  = 0;

            m_cpt_dcache_read++;

            // dcache_cacheable evaluation
            dcache_cacheable	= m_cacheability_table[(uint64_t)m_dreq.addr];

            // dcache_hit, dcache_way, dcache_set, dcache_word & dcache_rdata evaluation
            dcache_hit 		    = m_dcache.read( m_dreq.addr, 
                                                 &dcache_rdata,
                                                 &dcache_way,
                                                 &dcache_set,
                                                 &dcache_word );

            // Save proc request and cache response
            m_dcache_addr_save      = m_dreq.addr;
            m_dcache_type_save      = m_dreq.type;
            m_dcache_wdata_save     = m_dreq.wdata;
            m_dcache_be_save        = m_dreq.be;
            m_dcache_cacheable_save = dcache_cacheable;
            m_dcache_way_save       = dcache_way;
            m_dcache_set_save       = dcache_set;
            m_dcache_word_save      = dcache_word;

            // compute next FSM state, VCI request, and processor response 
            if( m_dreq.type  == iss_t::DATA_READ )
            {
                if( not dcache_cacheable ) 					// uncachable read 
                {
                    m_cpt_data_unc++;

                    m_dcache_unc_req   	= true;
                    m_dcache_fsm 		= DCACHE_UNC_WAIT;
	            }
                else
                {
                    m_cpt_read++;

                    if( dcache_hit )					// cacheable read hit
                    {
                        m_drsp.valid 	= true;
                        m_drsp.rdata 	= dcache_rdata;
                        m_dcache_fsm 	= DCACHE_IDLE;
                    }
                    else 		         	           // cacheable read miss
                    {
                        m_cpt_data_miss++;

                        m_dcache_word_save  = 0;
                        m_dcache_miss_req  	= true;
                        m_dcache_fsm 		= DCACHE_MISS_SELECT;
                    }
                }
            }
            else if( m_dreq.type == iss_t::DATA_WRITE )
            {
                if( not dcache_cacheable ) 	          // uncacheable write
                {
                    m_cpt_data_unc++;

                    m_dcache_unc_req   	= true;
                    m_dcache_fsm 		= DCACHE_UNC_WAIT;
                }
                else
                {
                    m_cpt_write++;

                    if( not dcache_hit ) 				// cacheable write miss 
                    {
                        m_drsp.rdata 	= 0;
                        m_drsp.valid 	= true;
                        m_dcache_fsm 	= DCACHE_WRITE_REQ;
                    }
                    else 						       // cacheable write hit 
                    {
                        m_cpt_write_cached++;

                        m_drsp.rdata 	= 0;
                        m_drsp.valid 	= true;
                        m_dcache_fsm 	= DCACHE_WRITE_UPDT;
                    }
                }
            }
            else if( (m_dreq.type == iss_t::DATA_LL) or
                     (m_dreq.type == iss_t::DATA_SC) )
            //  LL & SC requests are handled as uncacheable	
            {
                m_cpt_data_unc++;

                m_dcache_unc_req   		= true;
                m_dcache_fsm 			= DCACHE_UNC_WAIT;
            }
            else if( (m_dreq.type == iss_t::XTN_WRITE) or 
                     (m_dreq.type == iss_t::XTN_READ) ) 
            // only INVAL & SYNC requests are supported
            {
                if ( m_dreq.addr/4 == iss_t::XTN_DCACHE_INVAL ) 
                {
                    m_dcache_fsm = DCACHE_XTN_HIT;
                }
                else if ( m_dreq.addr/4 == iss_t::XTN_SYNC ) 	
                {
                    m_dcache_fsm = DCACHE_XTN_SYNC;
                }
                else
                {
                    std::cout << "error in VCI_CC_XCACHE_WRAPPER " << name() << std::endl;
                    std::cout << "unsupported  external access" << m_dreq.addr/4 << std::endl;
                    std::cout << "only XTN_DCACHE_INVAL & XTN_SYNC are supported" << std::endl;
                    exit(0);
                }
            }
        }
        else // no dreq.valid
        {
            m_dcache_fsm = DCACHE_IDLE;
        }
        break;
    }
    ///////////////////////
    case DCACHE_WRITE_UPDT:     // update local copy in dcache
    {
        m_cpt_dcache_write++;

        m_dcache.write( m_dcache_way_save,
                        m_dcache_set_save,
                        m_dcache_word_save,
                        m_dcache_wdata_save,
                        m_dcache_be_save );

        m_dcache_fsm = DCACHE_WRITE_REQ;
        break;
    }
    ////////////////////////
    case DCACHE_MISS_SELECT:     // select a slot in an associative set
    {
        bool     valid;
        addr_t   victim;   // unused

        valid = m_dcache.victim_select( m_dcache_addr_save,
                                        &victim,
                                        &m_dcache_way_save,
                                        &m_dcache_set_save );

        if ( valid ) m_dcache_fsm = DCACHE_MISS_INVAL;
        else         m_dcache_fsm = DCACHE_MISS_WAIT;
        break;
    }
    ///////////////////////
    case DCACHE_MISS_INVAL:     // invalidate the selected slot
    {
        addr_t   nline;  // unused

        m_dcache.inval( m_dcache_way_save,
                        m_dcache_set_save,
                        &nline );

        m_dcache_fsm = DCACHE_MISS_WAIT;
        break;
    }
    //////////////////////
    case DCACHE_MISS_WAIT:    // wait a response and update the dcache
    {
        if ( m_vci_rsp_data_error )     // error reported
        {
            m_drsp.valid         = true;
            m_drsp.error         = true;
            m_vci_rsp_data_error = false;
        	m_dcache_fsm         = DCACHE_IDLE;
        }
        else if ( m_vci_rsp_data_rok )  // available data
        {
            m_cpt_dcache_write++;

            uint32_t data = atou( m_dmiss_payload.get_data_ptr(),
                                  m_dcache_word_save * vci_param::nbytes );

            m_dcache.write( m_dcache_way_save,
                            m_dcache_set_save,
                            m_dcache_word_save,
                            data );
                            
            if ( m_dcache_word_save == m_dcache_words-1 )  // last word
            {
                m_dcache.victim_update_tag( m_dcache_addr_save,
                                            m_dcache_way_save,
                                            m_dcache_set_save );
           	    m_dcache_fsm       = DCACHE_IDLE;
                m_vci_rsp_data_rok = false;
            }
            m_dcache_word_save = m_dcache_word_save + 1;
        }
        break;
    }
    /////////////////////
    case DCACHE_UNC_WAIT:      // wait the response
    {
        if ( m_vci_rsp_data_error )    // error reported
        {
            m_drsp.valid         = true;
            m_drsp.error         = true;
            m_vci_rsp_data_error = false;
        	m_dcache_fsm         = DCACHE_IDLE;
        }
        else if ( m_vci_rsp_data_rok )   // available data
        {
            uint32_t data = atou( m_dmiss_payload.get_data_ptr(), 0);

            if ( m_dreq.valid and (m_dreq.addr == m_dcache_addr_save) ) // unmodified
            {
                m_drsp.valid = true;
                m_drsp.rdata = data;
            }
            m_dcache_fsm = DCACHE_IDLE;
            m_vci_rsp_data_rok = false;
        }
        break;
    }
    ////////////////////
    case DCACHE_XTN_HIT:     // checking hit for an XTN_INVAL request
    {
        uint32_t  data;    // unused
        size_t    word;    // unused

        bool      hit = m_dcache.read( m_dcache_wdata_save,
                                       &data,
                                       &m_dcache_way_save,
                                       &m_dcache_set_save,
                                       &word );

        if( hit )   // inval to be done
        {
            m_dcache_fsm      = DCACHE_XTN_INVAL;
        }
        else        // miss: nothing to do
        {
            m_drsp.valid = true;
            m_dcache_fsm = DCACHE_IDLE;
        }
        break;
    }
    //////////////////////
    case DCACHE_XTN_INVAL:    // invalidate the selected slot
    {
        addr_t nline;   // unused

        m_dcache.inval( m_dcache_way_save,
                        m_dcache_set_save,
                        &nline );

        m_dcache_fsm = DCACHE_IDLE;
        m_drsp.valid = true;
        break;
    }
    /////////////////////
    case DCACHE_XTN_SYNC:     // waiting write buffer empty
    {
        if ( m_wbuf.empty() ) 
        {
            m_dcache_fsm = DCACHE_IDLE;
            m_drsp.valid = true;
        }
        break;
    } } // end DCACHE_FSM switch
} // end dcache_fsm()


/////////////////////////////////////////////////////////////////////////////////
// This function implement the VCI CMD FSM, that handles requests
// from the DCACHE FSM, the ICACHE FSM, and the WBUF:
// There is 5 request types, with the following priorities :
// 1 - Data Read Miss          : m_dcache_miss_req   /  use DMISS    TLM buffer 
// 2 - Instruction Miss        : m_icache_miss_req   /  use IMISS    TLM buffer
// 3 - Data Write cacheable    : m_wbuf.rok()        /  use WRITE[k] TLM buffer
// 4 - Data R/W Uncacheable    : m_dcache_unc_req    /  use DMISS    TLM buffer
// 5 - Instruction Uncacheable : m_icache_unc_req    /  use IMISS    TLM buffer
/////////////////////////////////////////////////////////////////////////////////
tmpl(void)::vci_cmd_fsm()
{
    switch ( m_vci_cmd_fsm ) {
    //////////////
    case CMD_IDLE:
    {
        if ( m_dcache_miss_req and m_wbuf.miss( m_dcache_addr_save ) )  // IMISS
        {
            // set DMISS transaction fields
            m_dmiss_payload.set_address( m_dcache_addr_save & ~3 );
            m_dmiss_payload.set_data_length( m_dcache_words*vci_param::nbytes );
            m_dmiss_extension.set_read();
            m_dmiss_extension.set_trd_id( TYPE_DATA_MISS );

            // consume dcache request
            m_dcache_miss_req = false;
            m_vci_cmd_fsm = CMD_DATA_MISS;
        }
        else if ( m_icache_miss_req and m_wbuf.miss( m_icache_addr_save ) )  // IUNC
        {
            // set IMISS transaction fields
            m_imiss_payload.set_address( m_icache_addr_save & ~3 );
            m_imiss_payload.set_data_length( m_icache_words*vci_param::nbytes );
            m_imiss_extension.set_read();
            m_imiss_extension.set_trd_id( TYPE_INS_MISS );

            // consume icache request
            m_icache_miss_req = false;
            m_vci_cmd_fsm = CMD_INS_MISS;
        }
        else if ( m_wbuf.rok( &m_vci_cmd_min, &m_vci_cmd_max ) )  // WRITE CACHEABLE
        {
            size_t length = (m_vci_cmd_max - m_vci_cmd_min + 1) * vci_param::nbytes;
            size_t index  = m_wbuf.getIndex();

            // set WRITE transaction fields
            m_write_payload[index].set_address( m_wbuf.getAddress( m_vci_cmd_min ) );
            m_write_payload[index].set_data_length( length );
            m_write_extension[index].set_write();
            m_write_extension[index].set_trd_id( 0x8 + index );

            // initialise flit counter
            m_vci_cmd_cpt   = 0;
            m_vci_cmd_index = index;
            m_vci_cmd_fsm   = CMD_DATA_WRITE;
        }
        else if ( m_dcache_unc_req )          // UNCACHEABLE DATA (READ, WRITE, LL, SC)
        {
            // set DMISS transaction fields
            m_dmiss_payload.set_address( m_dcache_addr_save & ~3 );
            m_dmiss_payload.set_data_length( vci_param::nbytes );
            m_dmiss_extension.set_trd_id( TYPE_DATA_UNC );
            if ( m_dcache_type_save == iss_t::DATA_READ  ) m_dmiss_extension.set_read();
            if ( m_dcache_type_save == iss_t::DATA_WRITE ) m_dmiss_extension.set_write();
            if ( m_dcache_type_save == iss_t::DATA_LL    ) m_dmiss_extension.set_locked_read();
            if ( m_dcache_type_save == iss_t::DATA_SC    ) m_dmiss_extension.set_store_cond();
          
            // consume dcache request
            m_dcache_unc_req = false;
            m_vci_cmd_fsm = CMD_DATA_UNC;
        } 
        else if ( m_icache_unc_req )                      // UNCACHEABLE INSTRUCTION
        {
            // set IMISS transaction fields
            m_imiss_payload.set_address( m_icache_addr_save & ~3 );
            m_imiss_payload.set_data_length( vci_param::nbytes );
            m_imiss_extension.set_read();
            m_imiss_extension.set_trd_id( TYPE_INS_UNC );

            // consume icache request
            m_icache_unc_req = false;
            m_vci_cmd_fsm = CMD_INS_UNC;
        }
        break;
    }
    ///////////////////
    case CMD_DATA_MISS:    // send a DMISS transaction
    {   
#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "]" << " send DMISS COMMAND / time = " << std::dec
          << m_pdes_local_time->get().value() << " / address = " 
          << std::hex << m_dmiss_payload.get_address() << std::endl;
#endif
        // reset time quantum
        m_pdes_local_time->reset_sync();

        m_dmiss_time = m_pdes_local_time->get();
        p_vci->nb_transport_fw( m_dmiss_payload, 
                                m_dmiss_phase, 
                                m_dmiss_time);
        m_vci_cmd_fsm = CMD_IDLE;
        break;
    }
    //////////////////
    case CMD_INS_MISS:    // send a IMISS transaction
    {   
#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "]" << " send IMISS COMMAND / time = " << std::dec
          << m_pdes_local_time->get().value() << " / address = " 
          << std::hex << m_imiss_payload.get_address() << std::endl;
#endif
        // reset time quantum
        m_pdes_local_time->reset_sync();

        m_imiss_time = m_pdes_local_time->get();
        p_vci->nb_transport_fw( m_imiss_payload, 
                                m_imiss_phase, 
                                m_imiss_time );
        m_vci_cmd_fsm = CMD_IDLE;
        break;
    }
    //////////////////
    case CMD_DATA_UNC:    // send a DUNC transaction
    {   
#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "]" << " send DUNC COMMAND / time = " << std::dec
          << m_pdes_local_time->get().value() << " / address = " 
          << std::hex << m_dmiss_payload.get_address() << std::endl;
#endif
        // reset time quantum
        m_pdes_local_time->reset_sync();

        m_dmiss_time = m_pdes_local_time->get();
        p_vci->nb_transport_fw( m_dmiss_payload, 
                                m_dmiss_phase, 
                                m_dmiss_time);
        m_vci_cmd_fsm = CMD_IDLE;
        break;
    }
    /////////////////
    case CMD_INS_UNC:    // send a IUNC transaction
    {   
#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "]" << " send IUNC COMMAND / time = " << std::dec
          << m_pdes_local_time->get().value() << " / address = " 
          << std::hex << m_imiss_payload.get_address() << std::endl;
#endif
        // reset time quantum
        m_pdes_local_time->reset_sync();

        m_imiss_time = m_pdes_local_time->get();
        p_vci->nb_transport_fw( m_imiss_payload, 
                                m_imiss_phase, 
                                m_imiss_time);
        m_vci_cmd_fsm = CMD_IDLE;
        break;
    }
    ////////////////////
    case CMD_DATA_WRITE:    // Build WRITE transaction (one word per cycle)
                            // - m_vci_cmd_min is the pointer in WBUF[index]
                            // - m_vci_cmd_cpt is the pointer in data_buf[index]
                            // Send transaction at last flit.
    {   
        uint32_t data = m_wbuf.getData( m_vci_cmd_min );
	    utoa( data, 
              m_write_data_buf[m_vci_cmd_index], 
              m_vci_cmd_cpt*vci_param::nbytes );

        uint32_t be   = vci_param::be2mask( m_wbuf.getBe( m_vci_cmd_min ) );
	    utoa( be, 
              m_write_be_buf[m_vci_cmd_index],
              m_vci_cmd_cpt*vci_param::nbytes );
        
        if ( m_vci_cmd_min == m_vci_cmd_max )   // last flit
        {
#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "]" << " send WRITE COMMAND / time = " << std::dec
          << m_pdes_local_time->get().value() << " / address = " 
          << std::hex << m_write_payload[m_vci_cmd_index].get_address() << std::endl;
#endif
            // reset time quantum
            m_pdes_local_time->reset_sync();

            m_write_time[m_vci_cmd_index] = m_pdes_local_time->get();
            p_vci->nb_transport_fw( m_write_payload[m_vci_cmd_index], 
                                    m_write_phase[m_vci_cmd_index], 
                                    m_write_time[m_vci_cmd_index] );
            m_vci_cmd_fsm = CMD_IDLE;
        }
        m_vci_cmd_cpt++;
        m_vci_cmd_min++;
        break;
    } } // end switch
} // end vci_cmd_fsm()

/////////////////////////////////////////////////////////////////////////////////////
// This functions implements the VCI_RSP FSM.
// It exit IDLE state when the interface function sets the m_rsp_valid flip-flop.
// As simultaneous VCI transactions are supported, the m_rsp_type (TRDID) is decoded,
// to dispatch the response to the proper client (ICACHE, DCACHE, WBUF):
// - Non Blocking Write transactions : TRDID = wbuf_index + Ox8
//   The corresponding WBUF entry is released.
// - Blocking transactions  : TRDID = 2*cacheable + instruction
//   The data availability is signaled to ICACHE FSM (or DCACHE FSM) by the
//   m_vci_rsp_ins_rok (or m_vci_rsp_data_rok) flip-flops. 
//   The PDES local time is updated, and a m_rsp_received event is notified.
//
// Error handling:
// - In case of Write error, the error is directly signaled by the RSP FSM.
// - In case of Data Error, the VCI_RSP FSM sets the r_vci_rsp_data_error flip_flop.
// - In case of Instruction Error, it sets the r_vci_rsp_ins_error flip-flop.
//////////////////////////////////////////////////////////////////////////////////////
tmpl (void)::vci_rsp_fsm()
{
    switch (m_vci_rsp_fsm) {
    //////////////
    case RSP_IDLE:
    {
        if ( m_rsp_valid )  // valid response received on VCI port
        {
            if      ( m_rsp_type & 0x8 )            m_vci_rsp_fsm = RSP_DATA_WRITE;
            else if ( m_rsp_type == TYPE_INS_MISS)  m_vci_rsp_fsm = RSP_INS_MISS;
            else if ( m_rsp_type == TYPE_INS_UNC)   m_vci_rsp_fsm = RSP_INS_UNC;
            else if ( m_rsp_type == TYPE_DATA_MISS) m_vci_rsp_fsm = RSP_DATA_MISS;
            else if ( m_rsp_type == TYPE_DATA_UNC)  m_vci_rsp_fsm = RSP_DATA_UNC;
        }
        break;
    }
    /////////////////////
    case RSP_DATA_WRITE:      // release WBUF entry if response is in past 
                              // wait if response time is in future
    {
        if ( m_pdes_local_time->get() >= m_rsp_time ) 
        {
            size_t index = m_rsp_type - 0x8;
            if ( m_write_payload[index].is_response_error() ) 
            {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << name() << "    !!! WRITE BERR / time = " << std::dec
          << m_pdes_local_time->get().value() << std::endl;
#endif
                 m_iss.setWriteBerr();
            }	
            m_wbuf.completed( index );
            m_rsp_valid = false;
            m_vci_rsp_fsm = RSP_IDLE;
        }                                             
        break;
    }
    //////////////////
    case RSP_INS_MISS:   // signal data availability (or bus error) to ICACHE FSM
    case RSP_INS_UNC:    // Update local time 
    {
        if ( m_imiss_payload.is_response_error() ) m_vci_rsp_ins_error = true;
        else                                       m_vci_rsp_ins_rok   = true;

        // update local time
        if ( m_rsp_time > m_pdes_local_time->get() ) m_pdes_local_time->set( m_rsp_time );
        m_rsp_valid = false;
        m_vci_rsp_fsm = RSP_IDLE;
        break;
    }
    ///////////////////
    case RSP_DATA_MISS:   // signal data availability (or bus error) to DCACHE FSM
    case RSP_DATA_UNC:    // Update local time 
    {
        if ( m_dmiss_payload.is_response_error() ) m_vci_rsp_data_error = true;
        else                                       m_vci_rsp_data_rok   = true;

        // update local time
        if ( m_rsp_time > m_pdes_local_time->get() ) m_pdes_local_time->set( m_rsp_time );
        m_rsp_valid = false;
        m_vci_rsp_fsm = RSP_IDLE;
        break;
    } } // end switch
} // end vci_rsp_fsm()
    
//////////////////////////////////////////////////////////////////////////////////////
// Interface function called when receiving a VCI response
//////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::nb_transport_bw( tlm::tlm_generic_payload  &payload, 
                                            tlm::tlm_phase            &phase, 
                                            sc_core::sc_time          &time )  
{

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "] receive VCI RSP / time = " << time.value() << std::endl;
#endif

    soclib_payload_extension *extension_ptr;
    payload.get_extension(extension_ptr);
    
    // does nothing for a response to a NULL message
    if( extension_ptr->is_null_message() ) return tlm::TLM_COMPLETED;

    // signal response to VCI_RSP FSM
    m_rsp_valid = true;
    m_rsp_type  = extension_ptr->get_trd_id();
    m_rsp_time  = time;

    // notify if this is not a write
    if ( m_rsp_type < 0x8 )   m_rsp_received.notify( sc_core::SC_ZERO_TIME );

    return tlm::TLM_COMPLETED;
}  // end of VCI receive

/////////////////////////////////////////////////////////////////////////////////////
// Interface function called when receiving an IRQ command
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::irq_nb_transport_fw ( int                       id,       
                                                 tlm::tlm_generic_payload  &payload, 
                                                 tlm::tlm_phase            &phase,  
                                                 sc_core::sc_time          &time )  
{
    uint8_t	value = payload.get_data_ptr()[0];

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "] receive IRQ / time = " << std::dec << time.value()
          << " / index = " << id << " / value = " << (int)value << std::endl;
#endif

    assert(time.value() >= m_irq_time[id].value() 
    && "ERROR in VCI_XCACHE_WRAPPER : IRQ received with a strange date");

    m_irq_value[id] = ( value != 0 );
    m_irq_time[id]  = time;

    return tlm::TLM_COMPLETED;
}  // end of IRQ receive

///////////////////////////////////////////////////////////////////////////////////
// Not implemented but required by interface
///////////////////////////////////////////////////////////////////////////////////
tmpl(void)::invalidate_direct_mem_ptr ( unsigned long long start_range, 
                                        unsigned long long end_range ) 
{
}

}}  // end name spaces


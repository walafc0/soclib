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
 * Maintainers: alinev
 *
 * Copyright (c) UPMC / Lip6, 2010
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include "alloc_elems.h"
#include <limits>

#include "vci_xcache_wrapper.h"

//#define SOCLIB_MODULE_DEBUG 1

#ifdef SOCLIB_MODULE_DEBUG
namespace {
const char *dcache_fsm_state_str[] = {
        "DCACHE_IDLE",
        "DCACHE_WRITE_UPDT",
        "DCACHE_WRITE_REQ",
        "DCACHE_MISS_WAIT",
        "DCACHE_MISS_UPDT",
        "DCACHE_UNC_WAIT",
        "DCACHE_INVAL",
        "DCACHE_ERROR",
    };
const char *icache_fsm_state_str[] = {
        "ICACHE_IDLE",
        "ICACHE_MISS_WAIT",
        "ICACHE_MISS_UPDT",
        "ICACHE_UNC_WAIT",
        "ICACHE_ERROR",
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
	"RSP_DATA_WRITE_TIME_WAIT",
    };
}
#endif


namespace soclib{ namespace tlmdt {

#define tmpl(x) template<typename vci_param, typename iss_t> x VciXcacheWrapper<vci_param, iss_t>
   
using soclib::common::uint32_log2;

/////////////////////////////
tmpl (/**/)::VciXcacheWrapper
(
 sc_core::sc_module_name name,
 int cpuid,
 const soclib::common::IntTab &index,
 const soclib::common::MappingTable &mt,
 size_t icache_ways,
 size_t icache_sets,
 size_t icache_words,
 size_t dcache_ways,
 size_t dcache_sets,
 size_t dcache_words,
 sc_core::sc_time time_quantum)
  : sc_module(name)
	   , m_id(mt.indexForId(index))
	   , m_iss(this->name(), cpuid)
	   , m_simulation_time(std::numeric_limits<size_t>::max() * UNIT_TIME)
	   , m_icache_ways(icache_ways)
	   , m_icache_sets(icache_sets)
	   , m_icache_words(icache_words)
	   , m_icache_yzmask((~0)<<(uint32_log2(icache_words) + 2))
	   , m_dcache_ways(dcache_ways)
	   , m_dcache_sets(dcache_sets)
	   , m_dcache_words(dcache_words)
	   , m_dcache_yzmask((~0)<<(uint32_log2(dcache_words) + 2))
	   , m_wbuf("wbuf", dcache_words)
	   , m_icache("icache", icache_ways, icache_sets, icache_words)
	   , m_dcache("dcache", dcache_ways, dcache_sets, dcache_words)
	   , m_cacheability_table(mt.getCacheabilityTable())
	   , p_vci("socket")  
{
  init( (size_t)time_quantum.value());
}

////////////////////////////
tmpl (/**/)::VciXcacheWrapper
(
 sc_core::sc_module_name name,
 int cpuid,
 const soclib::common::MappingTable &mt,
 const soclib::common::IntTab &index,
 size_t icache_ways,
 size_t icache_sets,
 size_t icache_words,
 size_t dcache_ways,
 size_t dcache_sets,
 size_t dcache_words)
  : sc_module(name)
	   , m_id(mt.indexForId(index))
	   , m_iss(this->name(), cpuid)
	   , m_simulation_time(std::numeric_limits<size_t>::max() * UNIT_TIME)
	   , m_icache_ways(icache_ways)
	   , m_icache_sets(icache_sets)
	   , m_icache_words(icache_words)
	   , m_icache_yzmask((~0)<<(uint32_log2(icache_words) + 2))
	   , m_dcache_ways(dcache_ways)
 	   , m_dcache_sets(dcache_sets)
	   , m_dcache_words(dcache_words)
	   , m_dcache_yzmask((~0)<<(uint32_log2(dcache_words) + 2))
	   , m_wbuf("wbuf", dcache_words)
	   , m_icache("icache", icache_ways, icache_sets, icache_words)
	   , m_dcache("dcache", dcache_ways, dcache_sets, dcache_words)
	   , m_cacheability_table(mt.getCacheabilityTable())
	   , p_vci("socket")   
{
  init( 100 );
}

////////////////////////////
tmpl (/**/)::VciXcacheWrapper
(
 sc_core::sc_module_name name,
 int cpuid,
 const soclib::common::MappingTable &mt,
 const soclib::common::IntTab &index,
 size_t icache_ways,
 size_t icache_sets,
 size_t icache_words,
 size_t dcache_ways,
 size_t dcache_sets,
 size_t dcache_words,
 size_t time_quantum)
  : sc_module(name)
	   , m_id(mt.indexForId(index))
	   , m_iss(this->name(), cpuid)
	   , m_simulation_time(std::numeric_limits<size_t>::max() * UNIT_TIME)
	   , m_icache_ways(icache_ways)
	   , m_icache_sets(icache_sets)
	   , m_icache_words(icache_words)
	   , m_icache_yzmask((~0)<<(uint32_log2(icache_words) + 2))
	   , m_dcache_ways(dcache_ways)
	   , m_dcache_sets(dcache_sets)
	   , m_dcache_words(dcache_words)
	   , m_dcache_yzmask((~0)<<(uint32_log2(dcache_words) + 2))
	   , m_wbuf("wbuf", dcache_words)
	   , m_icache("icache", icache_ways, icache_sets, icache_words)
	   , m_dcache("dcache", dcache_ways, dcache_sets, dcache_words)
	   , m_cacheability_table(mt.getCacheabilityTable())
	   , p_vci("socket")   
{
  init( time_quantum );
}

/////////////////////////////
tmpl (/**/)::VciXcacheWrapper
(
 sc_core::sc_module_name name,
 int cpuid,
 const soclib::common::MappingTable &mt,
 const soclib::common::IntTab &index,
 size_t icache_ways,
 size_t icache_sets,
 size_t icache_words,
 size_t dcache_ways,
 size_t dcache_sets,
 size_t dcache_words,
 size_t time_quantum,
 size_t simulation_time)
	    : sc_module(name)
	   , m_id(mt.indexForId(index))
	   , m_iss(this->name(), cpuid)
	   , m_simulation_time(simulation_time * UNIT_TIME)
	   , m_icache_ways(icache_ways)
	   , m_icache_sets(icache_sets)
	   , m_icache_words(icache_words)
	   , m_icache_yzmask((~0)<<(uint32_log2(icache_words) + 2))
	   , m_dcache_ways(dcache_ways)
	   , m_dcache_sets(dcache_sets)
	   , m_dcache_words(dcache_words)
	   , m_dcache_yzmask((~0)<<(uint32_log2(dcache_words) + 2))
	   , m_wbuf("wbuf", dcache_words)
	   , m_icache("icache", icache_ways, icache_sets, icache_words)
	   , m_dcache("dcache", dcache_ways, dcache_sets, dcache_words)
	   , m_cacheability_table(mt.getCacheabilityTable())
	   , p_vci("socket")   
{
  init( time_quantum );
}

///////////////////////////////////////
tmpl (void)::init( size_t time_quantum)
{
  // bind initiator
  p_vci(*this);                     

  // create IRQ arrays
  m_pending_irq 	= new bool[iss_t::n_irq];
  m_pending_time	= new sc_core::sc_time[iss_t::n_irq];

  //register IRQ interface function 
  for(unsigned int i=0; i<iss_t::n_irq; i++)
  {
    std::ostringstream irq_name;
    irq_name << "irq" << i;
    p_irq.push_back(new tlm_utils::simple_target_socket_tagged<VciXcacheWrapper,32,tlm::tlm_base_protocol_types>
                                        (irq_name.str().c_str()));
    p_irq[i]->register_nb_transport_fw(this, &VciXcacheWrapper::irq_nb_transport_fw, i);
  }

  typename iss_t::CacheInfo cache_info;
  cache_info.has_mmu = false;
  cache_info.icache_line_size = m_icache_words*sizeof(data_t);
  cache_info.icache_assoc = m_icache_ways;
  cache_info.icache_n_lines = m_icache_sets;
  cache_info.dcache_line_size = m_dcache_words*sizeof(data_t);
  cache_info.dcache_assoc = m_dcache_ways;
  cache_info.dcache_n_lines = m_dcache_sets;
  m_iss.setCacheInfo(cache_info);

  m_iss.reset();

  m_icache_miss_buf = new data_t[m_icache_words];
  m_dcache_miss_buf = new data_t[m_dcache_words];

  // write buffer & caches
  m_wbuf.reset();
  m_icache.reset();
  m_dcache.reset();
  
  //PDES local time
  m_pdes_local_time = new pdes_local_time(time_quantum * UNIT_TIME);

  //PDES activity status
  m_pdes_activity_status = new pdes_activity_status();

  //create payload and extension to a normal message
  m_payload_ptr = new tlm::tlm_generic_payload();
  m_extension_ptr = new soclib_payload_extension();

  //create payload and extension to a null message
  m_null_payload_ptr = new tlm::tlm_generic_payload();
  m_null_extension_ptr = new soclib_payload_extension();

  //create payload and extension to an activity message
  m_activity_payload_ptr = new tlm::tlm_generic_payload();
  m_activity_extension_ptr = new soclib_payload_extension();

  m_nbytes = 0;
  for(int i = 0; i < MAXIMUM_PACKET_SIZE * vci_param::nbytes; i++){
    m_byte_enable_ptr[i] = '0';
    m_data_ptr[i] = '0';
  }
  
  // synchronisation flip-flops from ICACHE & DCACHE FSMs to VCI  FSMs
  m_icache_miss_req    = false;
  m_icache_unc_req     = false;
  m_dcache_miss_req    = false;
  m_dcache_unc_req     = false;
  m_dcache_write_req   = false;

  m_icache_time_req       = 0;
  m_dcache_read_time_req  = 0;
  m_dcache_write_time_req = 0;
  
  // signals from the VCI RSP FSM to the ICACHE or DCACHE FSMs
  m_dcache_buf_unc_valid = false;
  m_icache_buf_unc_valid = false;
  m_vci_rsp_data_error   = false;
  m_vci_rsp_ins_error    = false;
  
  // activity counters
  m_cpt_frz_cycles = 0;
  m_cpt_read = 0;
  m_cpt_write = 0;
  m_cpt_data_miss = 0;
  m_cpt_ins_miss = 0;
  m_cpt_unc_read = 0;
  m_cpt_write_cached = 0;
  
  m_icache_fsm = ICACHE_IDLE;
  m_dcache_fsm = DCACHE_IDLE;
  m_vci_cmd_fsm = CMD_IDLE;
  m_vci_rsp_fsm = RSP_IDLE;

  SC_THREAD(execLoop);
}

////////////////////////
tmpl(void)::print_cpi()
////////////////////////
{
    std::cout << name() << " CPU " << m_id << " : CPI = "
              << (float)((int)m_pdes_local_time->get().value())/(((int)m_pdes_local_time->get().value()) - m_cpt_frz_cycles) << std::endl;
}
////////////////////////
tmpl(void)::print_stats()
////////////////////////
{
    float run_cycles = (float)(((int)m_pdes_local_time->get().value()) - m_cpt_frz_cycles);
    std::cout << name() << std::endl;
    std::cout << "- CPI                = " << (float)((int)m_pdes_local_time->get().value())/run_cycles << std::endl ;
    std::cout << "- READ RATE          = " << (float)m_cpt_read/run_cycles << std::endl ;
    std::cout << "- WRITE RATE         = " << (float)m_cpt_write/run_cycles << std::endl;
    std::cout << "- UNCACHED READ RATE = " << (float)m_cpt_unc_read/m_cpt_read << std::endl ;
    std::cout << "- CACHED WRITE RATE  = " << (float)m_cpt_write_cached/m_cpt_write << std::endl ;
    std::cout << "- IMISS_RATE         = " << (float)m_cpt_ins_miss/run_cycles << std::endl;
    std::cout << "- DMISS RATE         = " << (float)m_cpt_data_miss/(m_cpt_read-m_cpt_unc_read) << std::endl ;
//     std::cout << "- INS MISS COST      = " << (float)m_cost_ins_miss_frz/m_cpt_ins_miss << std::endl;
//     std::cout << "- IMISS TRANSACTION  = " << (float)m_cost_imiss_transaction/m_cpt_imiss_transaction << std::endl;
//     std::cout << "- DMISS COST         = " << (float)m_cost_data_miss_frz/m_cpt_data_miss << std::endl;
//     std::cout << "- DMISS TRANSACTION  = " << (float)m_cost_dmiss_transaction/m_cpt_dmiss_transaction << std::endl;
//     std::cout << "- UNC COST           = " << (float)m_cost_unc_read_frz/m_cpt_unc_read << std::endl;
//     std::cout << "- UNC TRANSACTION    = " << (float)m_cost_unc_transaction/m_cpt_unc_transaction << std::endl;
//     std::cout << "- WRITE COST         = " << (float)m_cost_write_frz/m_cpt_write << std::endl;
//     std::cout << "- WRITE TRANSACTION  = " << (float)m_cost_write_transaction/m_cpt_write_transaction << std::endl;
//     std::cout << "- WRITE LENGTH       = " << (float)m_length_write_transaction/m_cpt_write_transaction << std::endl;
}

////////////////////////////////////////////
tmpl (void)::update_time(sc_core::sc_time t)
{
  if(t > m_pdes_local_time->get()){
    m_pdes_local_time->set(t);
  }
}

////////////////////////
tmpl (void)::execLoop ()
{
  int before_time, after_time;
  while(m_pdes_local_time->get() < m_simulation_time)
  {

    m_pdes_local_time->add(UNIT_TIME);

#ifdef SOCLIB_MODULE_DEBUG
    std::cout
        << name()
        << " clock: " << std::dec << ((int)m_pdes_local_time->get().value())
        << " dcache fsm: " << dcache_fsm_state_str[m_dcache_fsm]
        << " icache fsm: " << icache_fsm_state_str[m_icache_fsm]
        << " cmd fsm: " << cmd_fsm_state_str[m_vci_cmd_fsm]
        << " rsp fsm: " << rsp_fsm_state_str[m_vci_rsp_fsm] 
        << std::endl;
#endif

#if MY_DEBUG
    std::cout
        << name()
        << " clock: " << std::dec << ((int)m_pdes_local_time->get().value())
        << " dcache fsm: " << dcache_fsm_state_str[m_dcache_fsm]
        << " icache fsm: " << icache_fsm_state_str[m_icache_fsm]
        << " cmd fsm: " << cmd_fsm_state_str[m_vci_cmd_fsm]
        << " rsp fsm: " << rsp_fsm_state_str[m_vci_rsp_fsm] << std::endl;
#endif
    
        iss();

        before_time = m_pdes_local_time->get().value();

        cmd_fsm();

        rsp_fsm();

        after_time  = m_pdes_local_time->get().value();

        if(after_time > before_time) frozen_iss(after_time - before_time);

        if (m_pdes_local_time->need_sync())  send_null_message();
 
    } // end while

    sc_core::sc_stop();

} // end execLoop()

/////////////////
tmpl(void)::iss()
{
    /////////////////////////////////////////////////////////////////////
    // The ICACHE FSM controls the following ressources:
    // - m_icache_fsm
    // - m_icache (instruction cache access)
    // - m_icache_addr_save
    // - m_icache_buf_unc_valid 
    // - m_icache_miss_req set
    // - m_icache_unc_req set
    // - m_icache_unc_req set
    // - m_vci_rsp_ins_error reset
    // - ireq & irsp structures for communication with the processor
    //
    // Processor requests are taken into account only in the IDLE state.
    // In case of MISS, or in case of uncached instruction, the FSM 
    // writes the missing address line in the  m_icache_addr_save register 
    // and sets the m_icache_miss_req (or the m_icache_unc_req) flip-flop.
    // The request flip-flop is reset by the VCI_RSP FSM when the VCI 
    // transaction is completed. 
    // The m_icache_buf_unc_valid is set in case of uncached access.
    // In case of bus error, the VCI_RSP FSM sets the m_vci_rsp_ins_error
    // flip-flop. It is reset by the ICACHE FSM.
    ///////////////////////////////////////////////////////////////////////

    typename iss_t::InstructionRequest ireq = ISS_IREQ_INITIALIZER;
    typename iss_t::InstructionResponse irsp = ISS_IRSP_INITIALIZER;

    typename iss_t::DataRequest dreq = ISS_DREQ_INITIALIZER;
    typename iss_t::DataResponse drsp = ISS_DRSP_INITIALIZER;

    m_iss.getRequests( ireq, dreq );

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << name() << " " << std::dec << m_pdes_local_time->get().value() << " Instruction Request: " << ireq << std::dec << std::endl;
#endif
#if MY_DEBUG
    std::cout << name() << " " << std::dec << m_pdes_local_time->get().value() << " Instruction Request: " << ireq << std::dec << std::endl;
#endif

    switch(m_icache_fsm) {

    case ICACHE_IDLE:
        if ( ireq.valid ) {
            data_t  icache_ins = 0;
            bool    icache_hit = false;
            addr_t  ireq_paddr = ireq.addr;
            m_iss.virtualToPhys(ireq_paddr);

            bool    icache_cached = m_cacheability_table[ireq_paddr];

            // icache_hit & icache_ins evaluation
            if ( icache_cached ) {
                icache_hit = m_icache.read(ireq_paddr, &icache_ins);
            } else {
                icache_hit = ( m_icache_buf_unc_valid && (ireq_paddr == m_icache_addr_save) );
                icache_ins = m_icache_miss_buf[0];
            }
            if ( ! icache_hit ) {
                m_cpt_ins_miss++;
                m_cost_ins_miss_frz++;
                m_icache_addr_save = ireq_paddr;
                if ( icache_cached ) {
                    m_icache_fsm = ICACHE_MISS_WAIT;
                    m_icache_miss_req = true;
		    m_icache_time_req = m_pdes_local_time->get().value();
                } else {
                    m_icache_fsm = ICACHE_UNC_WAIT;
                    m_icache_unc_req = true;
		    m_icache_time_req = m_pdes_local_time->get().value();
                    m_icache_buf_unc_valid = false;
                } 
            } else {
                m_icache_buf_unc_valid = false;
            }
            m_cpt_icache_dir_read += m_icache_ways;
            m_cpt_icache_data_read += m_icache_ways;
            irsp.valid          = icache_hit;
            irsp.instruction    = icache_ins;
        }
        break;

    case ICACHE_MISS_WAIT:
        m_cost_ins_miss_frz++;
        if ( !m_icache_miss_req ) {
            if ( m_vci_rsp_ins_error ) {
                m_icache_fsm = ICACHE_ERROR;
                m_vci_rsp_ins_error = false;
            } else {
                m_icache_fsm = ICACHE_MISS_UPDT;
            }
        }
        break;

    case ICACHE_UNC_WAIT:
        m_cost_ins_miss_frz++;
        if ( !m_icache_unc_req ) {
            if ( m_vci_rsp_ins_error ) {
                m_icache_fsm = ICACHE_ERROR;
                m_vci_rsp_ins_error = false;
            } else {
                m_icache_fsm = ICACHE_IDLE;
                m_icache_buf_unc_valid = true;
            }
        }
        break;

    case ICACHE_ERROR:
        m_icache_fsm = ICACHE_IDLE;
        m_vci_rsp_ins_error = false;
        irsp.valid          = true;
        irsp.error          = true;
        break;

    case ICACHE_MISS_UPDT:
      {
        addr_t  ad  = m_icache_addr_save;
        data_t* buf = m_icache_miss_buf;
        data_t  victim_index = 0;
        m_cpt_icache_dir_write++;
        m_cpt_icache_data_write++;
        m_cost_ins_miss_frz++;
        m_icache.update(ad, buf, &victim_index);
        m_icache_fsm = ICACHE_IDLE;
        break;
      }
      
    } // end switch m_icache_fsm

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << name() << " " << std::dec << m_pdes_local_time->get().value() << " Instruction Response: " << irsp << std::dec << std::endl;
#endif
#if MY_DEBUG
    std::cout << name() << " " << std::dec << m_pdes_local_time->get().value() << " Instruction Response: " << irsp << std::dec << std::endl;
#endif

    ///////////////////////////////////////////////////////////////////////////////////
    // The DCACHE FSM controls the following ressources:
    // - m_dcache_fsm
    // - m_dcache (data cache access)
    // - m_dcache_addr_save
    // - m_dcache_wdata_save
    // - m_dcache_rdata_save
    // - m_dcache_type_save
    // - m_dcache_be_save
    // - m_dcache_cached_save
    // - m_dcache_buf_unc_valid
    // - m_dcache_miss_req set
    // - m_dcache_unc_req set
    // - m_dcache_write_req set
    // - m_vci_rsp_data_error reset
    // - m_wbuf write
    // - dreq & drsp structures for communication with the processor
    //
    // In order to support VCI write burst, the processor requests are taken into account
    // in the WRITE_REQ state as well as in the IDLE state.
    // - In the IDLE state, the processor request cannot be satisfied if
    //   there is a cached read miss, or an uncached read.
    // - In the WRITE_REQ state, the request cannot be satisfied if
    //   there is a cached read miss, or an uncached read,
    //   or when the write buffer is full.
    // - In all other states, the processor request is not satisfied.
    //
    // The cache access takes into account the cacheability_table.
    // In case of processor request, there is five conditions to exit the IDLE state:
    //   - CACHED READ MISS => to the MISS_WAIT state (waiting the m_miss_ok signal),
    //     then to the MISS_UPDT state, and finally to the IDLE state.
    //   - UNCACHED READ  => to the UNC_WAIT state (waiting the m_miss_ok signal),
    //     and to the IDLE state.
    //   - CACHE INVALIDATE HIT => to the INVAL state for one cycle, then to IDLE state.
    //   - WRITE MISS => directly to the WRITE_REQ state to access the write buffer.
    //   - WRITE HIT => to the WRITE_UPDT state, then to the WRITE_REQ state.
    //
    // Error handling :  Read Bus Errors are synchronous events, but
    // Write Bus Errors are asynchronous events (processor is not frozen).
    // - If a Read Bus Error is detected, the VCI_RSP FSM sets the
    //   m_vci_rsp_data_error flip-flop, and the synchronous error is signaled
    //   by the DCACHE FSM.
    // - If a Write Bus Error is detected, the VCI_RSP FSM  signals
    //   the asynchronous error using the setWriteBerr() method.
    ////////////////////////////////////////////////////////////////////////

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << name() << " " << std::dec << m_pdes_local_time->get().value() << " Data Request: " << dreq << std::dec << std::endl;
#endif
#if MY_DEBUG
    std::cout << name() << " " << std::dec << m_pdes_local_time->get().value() << " Data Request: " << dreq << std::dec << std::endl;
#endif



    if(m_dcache_fsm==DCACHE_WRITE_UPDT && m_vci_cmd_fsm != CMD_IDLE){
      m_cpt_dcache_data_write++;
      data_t mask = vci_param::be2mask(m_dcache_be_save);
      data_t wdata = (mask & m_dcache_wdata_save) | (~mask & m_dcache_rdata_save);
      m_dcache.write(m_dcache_addr_save, wdata);
      m_dcache_fsm = DCACHE_WRITE_REQ;
      m_dcache_write_time_req = m_pdes_local_time->get().value() + 1;
   }



    switch ( m_dcache_fsm ) {

    case DCACHE_WRITE_REQ:

      // try to post the write request in the write buffer
      if ( !m_dcache_write_req ) {
	// no previous write transaction
	if ( m_wbuf.wok(m_dcache_addr_save) ) {
	  // write request in the same cache line
	  m_wbuf.write(m_dcache_addr_save, m_dcache_be_save, m_dcache_wdata_save);
	  // closing the write packet if uncached
	  if ( !m_dcache_cached_save ){
	    m_dcache_write_req = true ;
	    if(m_dcache_write_time_req < m_pdes_local_time->get().value())
	      m_dcache_write_time_req = m_pdes_local_time->get().value();
	  }
	} else {
	  // close the write packet if write request not in the same cache line
	  m_dcache_write_req = true;
	  if(m_dcache_write_time_req < m_pdes_local_time->get().value())
	    m_dcache_write_time_req = m_pdes_local_time->get().value();
	  m_cost_write_frz++;
	  break;
	  //  posting not possible : stay in DCACHE_WRITEREQ state
	}
      } else {
	//  previous write transaction not completed
	m_cost_write_frz++;
	break;
	//  posting not possible : stay in DCACHE_WRITEREQ state
      }
      
      // close the write packet if the next processor request is not a write
      if ( !dreq.valid || dreq.type != iss_t::DATA_WRITE ){
	m_dcache_write_req = true ;
	if(m_dcache_write_time_req < m_pdes_local_time->get().value())
	  m_dcache_write_time_req = m_pdes_local_time->get().value();
      }

      // The next state and the processor request parameters are computed
      // as in the DCACHE_IDLE state (see below ...)
      
      
      // processor request are not accepted in the WRITE_REQUEST state
      // when the write buffer is not writeable
      if ( m_dcache_write_req || !m_wbuf.wok(m_dcache_addr_save) ) {
	drsp.valid = false;
      }
      

    case DCACHE_IDLE:

        if ( dreq.valid ) {
            bool        dcache_hit     = false;
            data_t      dcache_rdata   = 0;
            bool        dcache_cached;
            addr_t      dreq_paddr = dreq.addr;
            m_iss.virtualToPhys(dreq_paddr);

            m_cpt_dcache_data_read += m_dcache_ways;
            m_cpt_dcache_dir_read += m_dcache_ways;

            // dcache_cached evaluation
            switch (dreq.type) {
            case iss_t::DATA_LL:
            case iss_t::DATA_SC:
            case iss_t::XTN_READ:
            case iss_t::XTN_WRITE:
                dcache_cached = false;
                break;
            default:
                dcache_cached = m_cacheability_table[dreq_paddr];
            }

            // dcache_hit & dcache_rdata evaluation
            if ( dcache_cached ) {
                dcache_hit = m_dcache.read(dreq_paddr, &dcache_rdata);
            } else {
                dcache_hit = ( (dreq_paddr == m_dcache_addr_save) && m_dcache_buf_unc_valid );
                dcache_rdata = m_dcache_miss_buf[0];
            }

            switch( dreq.type ) {
                case iss_t::DATA_READ:
                case iss_t::DATA_LL:
                case iss_t::DATA_SC:
                    m_cpt_read++;
                    if ( dcache_hit ) {
                        m_dcache_fsm = DCACHE_IDLE;
                        drsp.valid = true;
                        drsp.rdata = dcache_rdata;
                        m_dcache_buf_unc_valid = false;
                    } else {
                        if ( dcache_cached ) {
                            m_cpt_data_miss++;
                            m_cost_data_miss_frz++;
                            m_dcache_miss_req = true;
 			    m_dcache_read_time_req = m_pdes_local_time->get().value();
                           m_dcache_fsm = DCACHE_MISS_WAIT;
                        } else {
                            m_cpt_unc_read++;
                            m_cost_unc_read_frz++;
                            m_dcache_unc_req = true;
			    m_dcache_read_time_req = m_pdes_local_time->get().value();
                            m_dcache_fsm = DCACHE_UNC_WAIT;
                        }
                    }
                    break;
                case iss_t::XTN_READ:
                case iss_t::XTN_WRITE:
                    // only DCACHE INVALIDATE request are supported
                    switch ( dreq_paddr/4 ) {
                    case iss_t::XTN_DCACHE_INVAL:
                        m_dcache_fsm = DCACHE_INVAL;
                    case iss_t::XTN_SYNC:
                    default:
                        drsp.valid = true;
                        drsp.rdata = 0;
                        break;
                    }
                    break;
                case iss_t::DATA_WRITE:
                    m_cpt_write++;
                    if ( dcache_hit && dcache_cached ) {
		      m_cpt_write_cached++;
		      m_dcache_fsm = DCACHE_WRITE_UPDT;
                    } else {
		      m_dcache_fsm = DCACHE_WRITE_REQ;
                    }
                    drsp.valid = true;
                    drsp.rdata = 0;
                    break;
            } // end switch dreq.type

            m_dcache_addr_save      = dreq_paddr;
            m_dcache_type_save      = dreq.type;
            m_dcache_wdata_save     = dreq.wdata;
            m_dcache_be_save        = dreq.be;
            m_dcache_rdata_save     = dcache_rdata;
            m_dcache_cached_save    = dcache_cached;

        } else {    // if no dcache_req
	  m_dcache_fsm = DCACHE_IDLE;
        }
        break;
	
    case DCACHE_WRITE_UPDT:
    {
        m_cpt_dcache_data_write++;
        data_t mask = vci_param::be2mask(m_dcache_be_save);
        data_t wdata = (mask & m_dcache_wdata_save) | (~mask & m_dcache_rdata_save);
        m_dcache.write(m_dcache_addr_save, wdata);
        m_dcache_fsm = DCACHE_WRITE_REQ;


        break;
    }

    case DCACHE_MISS_WAIT:
        if ( dreq.valid ) m_cost_data_miss_frz++;
        if ( !m_dcache_miss_req ) {
            if ( m_vci_rsp_data_error )
                m_dcache_fsm = DCACHE_ERROR;
            else
                m_dcache_fsm = DCACHE_MISS_UPDT;
        }
        break;

    case DCACHE_MISS_UPDT:
    {
        addr_t  ad  = m_dcache_addr_save;
        data_t* buf = m_dcache_miss_buf;
        data_t  victim_index = 0;
        if ( dreq.valid )
            m_cost_data_miss_frz++;
        m_cpt_dcache_data_write++;
        m_cpt_dcache_dir_write++;
        m_dcache.update(ad, buf, &victim_index);
        m_dcache_fsm = DCACHE_IDLE;
        break;
    }

    case DCACHE_UNC_WAIT:
        if ( dreq.valid ) m_cost_unc_read_frz++;
        if ( !m_dcache_unc_req ) {
            if ( m_vci_rsp_data_error ) {
                m_dcache_fsm = DCACHE_ERROR;
            } else {
                m_dcache_fsm = DCACHE_IDLE;
                // If request was a DATA_SC we need to invalidate the corresponding cache line, 
                // so that subsequent access to this line are read from RAM
                if (dreq.type == iss_t::DATA_SC) {
                    m_dcache_fsm = DCACHE_INVAL;
                    m_dcache_wdata_save = m_dcache_addr_save;
                }
                m_dcache_buf_unc_valid = true;
            }
        }
        break;

    case DCACHE_ERROR:
        m_dcache_fsm = DCACHE_IDLE;
        m_vci_rsp_data_error = false;
        drsp.error = true;
        drsp.valid = true;
        break;

    case DCACHE_INVAL:
        m_cpt_dcache_dir_read += m_dcache_ways;
        m_dcache.inval(m_dcache_wdata_save);
        m_dcache_fsm = DCACHE_IDLE;
        break;
    }


#ifdef SOCLIB_MODULE_DEBUG
    std::cout << name() << " " << std::dec << m_pdes_local_time->get().value() << " Data Response: " << drsp << std::dec << std::endl;
#endif
#if MY_DEBUG
    std::cout << name() << " " << std::dec << m_pdes_local_time->get().value() << " Data Response: " << drsp << std::dec << std::endl;
#endif

    uint32_t	irq = 0;
    
    for ( size_t i=0 ; i<iss_t::n_irq ; i++)
    {
        if ( m_pending_irq[i] && (m_pending_time[i] <= m_pdes_local_time->get()) ) irq |= 1<<i;
    }
    
    /////////// execute one iss cycle /////////////////////////////////
    m_iss.executeNCycles(1, irsp, drsp, irq);

    if ( (ireq.valid && !irsp.valid) || (dreq.valid && !drsp.valid) )
        m_cpt_frz_cycles++;

} // end iss()

///////////////////////////////////
tmpl(void)::frozen_iss(int cycles)
{
  struct iss_t::InstructionResponse 	meanwhile_irsp = ISS_IRSP_INITIALIZER;
  struct iss_t::DataResponse 		meanwhile_drsp = ISS_DRSP_INITIALIZER;

  m_iss.executeNCycles(cycles, meanwhile_irsp, meanwhile_drsp, 0);
}

//////////////////////
tmpl(void)::cmd_fsm()
{

  ////////////////////////////////////////////////////////////////////////////
  // The VCI_CMD FSM controls the following ressources:
  // - m_vci_cmd_fsm
  // - m_vci_cmd_min
  // - m_vci_cmd_max
  // - m_vci_cmd_cpt
  // - wbuf reset
  //
  // This FSM handles requests from both the DCACHE FSM & the ICACHE FSM.
  // There is 4 request types, with the following priorities :
  // 1 - Instruction Miss     : m_icache_miss_req
  // 2 - Instruction Uncached : m_icache_unc_req
  // 3 - Data Write           : m_dcache_write_req
  // 4 - Data Read Miss       : m_dcache_miss_req
  // 5 - Data Read Uncached   : m_dcache_unc_req
  // There is at most one (CMD/RSP) VCI transaction, as both CMD_FSM
  // and RSP_FSM exit simultaneously the IDLE state.
  //
  // VCI formats:
  // According to the VCI advanced specification, all read command packets
  // (Uncached, Miss data, Miss instruction) are one word packets.
  // For write burst packets, all words must be in the same cache line,
  // and addresses must be contiguous (the BE field is 0 in case of "holes").
  // The PLEN VCI field is always documented.
  //////////////////////////////////////////////////////////////////////////////
  
  switch (m_vci_cmd_fsm) {
    
  case CMD_IDLE:
    if (m_vci_rsp_fsm != RSP_IDLE)
      break;
    
    m_vci_cmd_cpt = 0;
    if ( m_icache_miss_req && m_pdes_local_time->get().value()>m_icache_time_req) {
      m_vci_cmd_fsm = CMD_INS_MISS;
      m_cpt_imiss_transaction++;
    } else if ( m_icache_unc_req && m_pdes_local_time->get().value()>m_icache_time_req) {
      m_vci_cmd_fsm = CMD_INS_UNC;
      m_cpt_imiss_transaction++;
    } else if ( m_dcache_write_req && m_pdes_local_time->get().value()>m_dcache_write_time_req) {
      m_vci_cmd_fsm = CMD_DATA_WRITE;
      m_vci_cmd_cpt = m_wbuf.getMin();
      m_vci_cmd_min = m_wbuf.getMin();
      m_vci_cmd_max = m_wbuf.getMax();
      m_cpt_write_transaction++;
      m_length_write_transaction += (m_wbuf.getMax() - m_wbuf.getMin() + 1);
    } else if ( m_dcache_miss_req && m_pdes_local_time->get().value()>m_dcache_read_time_req) {
      m_vci_cmd_fsm = CMD_DATA_MISS;
      m_cpt_dmiss_transaction++;
    } else if ( m_dcache_unc_req && m_pdes_local_time->get().value()>m_dcache_read_time_req) {
      m_vci_cmd_fsm = CMD_DATA_UNC;
      m_cpt_unc_transaction++;
    }
    break;
    
  case CMD_DATA_WRITE:
    { 
      addr_t address = m_wbuf.getAddress(m_wbuf.getMin());
      data_t be;
      size_t nwords = m_wbuf.getMax()- m_wbuf.getMin() + 1;
      
      //printf("write addr = %08x cpt = %d min = %d max = %d time = %d\n", address, nwords, m_wbuf.getMin(), m_wbuf.getMax(), ((int)m_pdes_local_time->get().value()));
      
      for(size_t i=m_wbuf.getMin(), j=0; i<m_wbuf.getMin()+nwords; i++, j+=vci_param::nbytes){
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << " ram_write( " << std::hex << m_wbuf.getAddress(i) << ", " << m_wbuf.getData(i) << ", " <<  m_wbuf.getBe(i) << ")" << std::dec << std::endl;
#endif
	
	be = vci_param::be2mask(m_wbuf.getBe(i));
	utoa(be, m_byte_enable_ptr, j);
	utoa(m_wbuf.getData(i), m_data_ptr, j);
      }

      m_nbytes = nwords * vci_param::nbytes;
      
      // set the values in tlm payload
      m_payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
      m_payload_ptr->set_address(address & ~3);
      m_payload_ptr->set_byte_enable_ptr(m_byte_enable_ptr);
      m_payload_ptr->set_byte_enable_length(m_nbytes);
      m_payload_ptr->set_data_ptr(m_data_ptr);
      m_payload_ptr->set_data_length(m_nbytes);
      // set the values in payload extension
      m_extension_ptr->set_command(VCI_WRITE_COMMAND);
      m_extension_ptr->set_src_id(m_id);
      m_extension_ptr->set_trd_id(0);
      m_extension_ptr->set_pkt_id(0);
      // set the extension to tlm payload
      m_payload_ptr->set_extension(m_extension_ptr);
      //set the tlm phase
      m_phase = tlm::BEGIN_REQ;
      //set the local time to transaction time
      m_time = m_pdes_local_time->get();

      m_vci_cmd_fsm = CMD_IDLE ;
      m_wbuf.reset() ;
      m_pdes_local_time->reset_sync();
 
      //send a write message
      p_vci->nb_transport_fw(*m_payload_ptr, m_phase, m_time);

    }
    break;
    
  case CMD_DATA_MISS:
   { 
      addr_t address = m_dcache_addr_save & m_dcache_yzmask;
      data_t be = vci_param::be2mask(0xF);;

      m_nbytes = m_dcache_words * vci_param::nbytes;
      for(data_t i=0; i< m_nbytes; i+=vci_param::nbytes){
	utoa(be, m_byte_enable_ptr, i);
	utoa(0, m_data_ptr, i);
      }
      
      // set the values in tlm payload
      m_payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
      m_payload_ptr->set_address(address);
      m_payload_ptr->set_byte_enable_ptr(m_byte_enable_ptr);
      m_payload_ptr->set_byte_enable_length(m_nbytes);
      m_payload_ptr->set_data_ptr(m_data_ptr);
      m_payload_ptr->set_data_length(m_nbytes);
      // set the values in payload extension
      m_extension_ptr->set_read();
      m_extension_ptr->set_src_id(m_id);
      m_extension_ptr->set_trd_id(0);
      m_extension_ptr->set_pkt_id(0);
      // set the extension to tlm payload
      m_payload_ptr->set_extension(m_extension_ptr);
      //set the tlm phase
      m_phase = tlm::BEGIN_REQ;
      //set the local time to transaction time
      m_time = m_pdes_local_time->get();
      m_pdes_local_time->reset_sync();

      //send a write message
      p_vci->nb_transport_fw(*m_payload_ptr, m_phase, m_time);

      m_vci_cmd_fsm = CMD_IDLE ;
    }
    break;
    
  case CMD_DATA_UNC:

    // set the values in tlm payload
    m_payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
    m_payload_ptr->set_address(m_dcache_addr_save & ~0x3);

    switch( m_dcache_type_save ) {
    case iss_t::DATA_READ:
      {
	data_t mask = vci_param::be2mask(m_dcache_be_save);

	utoa(mask, m_byte_enable_ptr, 0);
	utoa(0, m_data_ptr, 0);

	m_extension_ptr->set_read();
      }
      break;
    case iss_t::DATA_LL:
      {
	data_t mask = vci_param::be2mask(0xF);
	
	utoa(mask, m_byte_enable_ptr, 0);
	utoa(0, m_data_ptr, 0);
	m_extension_ptr->set_locked_read();
      }
      break;
    case iss_t::DATA_SC:
      {
	data_t mask = vci_param::be2mask(0xF);
	
	utoa(mask, m_byte_enable_ptr, 0);
	utoa(m_dcache_wdata_save, m_data_ptr, 0);
	m_extension_ptr->set_store_cond();
      }
      break;
    default:
      assert("this should not happen");
    }
    
    m_nbytes = vci_param::nbytes;

    m_payload_ptr->set_byte_enable_ptr(m_byte_enable_ptr);
    m_payload_ptr->set_byte_enable_length(m_nbytes);
    m_payload_ptr->set_data_ptr(m_data_ptr);
    m_payload_ptr->set_data_length(m_nbytes);
    // set the values in payload extension
    m_extension_ptr->set_src_id(m_id);
    m_extension_ptr->set_trd_id(0);
    m_extension_ptr->set_pkt_id(0);
    // set the extension to tlm payload
    m_payload_ptr->set_extension(m_extension_ptr);
    //set the tlm phase
    m_phase = tlm::BEGIN_REQ;
    //set the local time to transaction time
    m_time = m_pdes_local_time->get();
    m_pdes_local_time->reset_sync();

    //send a message
    p_vci->nb_transport_fw(*m_payload_ptr, m_phase, m_time);

    m_vci_cmd_fsm = CMD_IDLE;
    break;


  case CMD_INS_MISS:
    {
      addr_t address = m_icache_addr_save & m_icache_yzmask;
      data_t be = vci_param::be2mask(0xF);;

      m_nbytes = m_icache_words * vci_param::nbytes;
      for(data_t i=0; i< m_nbytes; i+=vci_param::nbytes){
	utoa(be, m_byte_enable_ptr, i);
	utoa(0, m_data_ptr, i);
      }
      
      // set the values in tlm payload
      m_payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
      m_payload_ptr->set_address(address);
      m_payload_ptr->set_byte_enable_ptr(m_byte_enable_ptr);
      m_payload_ptr->set_byte_enable_length(m_nbytes);
      m_payload_ptr->set_data_ptr(m_data_ptr);
      m_payload_ptr->set_data_length(m_nbytes);
      // set the values in payload extension
      m_extension_ptr->set_read();
      m_extension_ptr->set_src_id(m_id);
      m_extension_ptr->set_trd_id(0);
      m_extension_ptr->set_pkt_id(0);
      // set the extension to tlm payload
      m_payload_ptr->set_extension(m_extension_ptr);
      //set the tlm phase
      m_phase = tlm::BEGIN_REQ;
      //set the local time to transaction time
      m_time = m_pdes_local_time->get();
      m_pdes_local_time->reset_sync();

      //send a write message
      p_vci->nb_transport_fw(*m_payload_ptr, m_phase, m_time);

      m_vci_cmd_fsm = CMD_IDLE ;
    }
    break;
  case CMD_INS_UNC:
    {
      addr_t address = m_icache_addr_save & ~0x3;
      data_t be = vci_param::be2mask(0xF);;

      m_nbytes = vci_param::nbytes;
      utoa(be, m_byte_enable_ptr, 0);
      utoa(0, m_data_ptr, 0);
      
      // set the values in tlm payload
      m_payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
      m_payload_ptr->set_address(address);
      m_payload_ptr->set_byte_enable_ptr(m_byte_enable_ptr);
      m_payload_ptr->set_byte_enable_length(m_nbytes);
      m_payload_ptr->set_data_ptr(m_data_ptr);
      m_payload_ptr->set_data_length(m_nbytes);
      // set the values in payload extension
      m_extension_ptr->set_read();
      m_extension_ptr->set_src_id(m_id);
      m_extension_ptr->set_trd_id(0);
      m_extension_ptr->set_pkt_id(0);
      // set the extension to tlm payload
      m_payload_ptr->set_extension(m_extension_ptr);
      //set the tlm phase
      m_phase = tlm::BEGIN_REQ;
      //set the local time to transaction time
      m_time = m_pdes_local_time->get();
      m_pdes_local_time->reset_sync();

      //send a write message
      p_vci->nb_transport_fw(*m_payload_ptr, m_phase, m_time);

      m_vci_cmd_fsm = CMD_IDLE ;
    }
    break;
    
  } // end  switch m_vci_cmd_fsm
}    

//////////////////////
tmpl (void)::rsp_fsm()
{

  //////////////////////////////////////////////////////////////////////////
  // The VCI_RSP FSM controls the following ressources:
  // - m_vci_rsp_fsm:
  // - m_icache_miss_buf[m_icache_words]
  // - m_dcache_miss_buf[m_dcache_words]
  // - m_icache_miss_req reset
  // - m_icache_unc_req reset
  // - m_dcache_miss_req reset
  // - m_dcache_unc_req reset
  // - m_icache_write_req reset
  // - m_vci_rsp_data_error set
  // - m_vci_rsp_ins_error set
  // - m_vci_rsp_cpt
  // In order to have only one active VCI transaction, this VCI_RSP_FSM
  // is synchronized with the VCI_CMD FSM, and both FSMs exit the
  // IDLE state simultaneously.
  //
  // VCI formats:
  // This component accepts single word or multi-word response packets for
  // write response packets.
  //
  // Error handling:
  // This FSM analyzes the VCI error code and signals directly the
  // Write Bus Error.
  // In case of Read Data Error, the VCI_RSP FSM sets the m_vci_rsp_data_error
  // flip_flop and the error is signaled by the DCACHE FSM.
  // In case of Instruction Error, the VCI_RSP FSM sets the m_vci_rsp_ins_error
  // flip_flop and the error is signaled by the DCACHE FSM.
  // In case of Cleanup Error, the simulation stops with an error message...
  //////////////////////////////////////////////////////////////////////////
  
  switch (m_vci_rsp_fsm) {
    
  case RSP_IDLE:
    //if (m_vci_cmd_fsm != CMD_IDLE) break;
    
    m_vci_rsp_cpt = 0;
    if      ( m_icache_miss_req && m_pdes_local_time->get().value()>m_icache_time_req)        m_vci_rsp_fsm = RSP_INS_MISS;
    else if ( m_icache_unc_req && m_pdes_local_time->get().value()>m_icache_time_req)         m_vci_rsp_fsm = RSP_INS_UNC;
    else if ( m_dcache_write_req && m_pdes_local_time->get().value()>m_dcache_write_time_req) m_vci_rsp_fsm = RSP_DATA_WRITE;
    else if ( m_dcache_miss_req && m_pdes_local_time->get().value()>m_dcache_read_time_req)       m_vci_rsp_fsm = RSP_DATA_MISS;
    else if ( m_dcache_unc_req && m_pdes_local_time->get().value()>m_dcache_read_time_req)        m_vci_rsp_fsm = RSP_DATA_UNC;
    break;
    
  case RSP_INS_MISS:
    m_cost_imiss_transaction++;
    wait(m_rsp_received);
    for(unsigned int i=0;i<(m_payload_ptr->get_data_length()/vci_param::nbytes); i++){
      m_icache_miss_buf[i] = atou(m_payload_ptr->get_data_ptr(), (i * vci_param::nbytes));
    }
    m_icache_miss_req = false;
    m_vci_rsp_fsm = RSP_IDLE;
    m_vci_rsp_ins_error = m_payload_ptr->is_response_error();
   
    break;
    
  case RSP_INS_UNC:
    m_cost_imiss_transaction++;
    wait(m_rsp_received);
    m_icache_miss_buf[0] = atou(m_payload_ptr->get_data_ptr(), 0);
    m_icache_buf_unc_valid = true;
    m_vci_rsp_fsm = RSP_IDLE;
    m_icache_unc_req = false;
    m_vci_rsp_ins_error = m_payload_ptr->is_response_error();

    break;
    
  case RSP_DATA_MISS:
    m_cost_dmiss_transaction++;
    wait(m_rsp_received);
    for(unsigned int i=0;i<(m_payload_ptr->get_data_length()/vci_param::nbytes); i++){
      m_dcache_miss_buf[i] = atou(m_payload_ptr->get_data_ptr(), (i * vci_param::nbytes));
    }
    m_dcache_miss_req = false;
    m_vci_rsp_fsm = RSP_IDLE;
    m_vci_rsp_data_error = m_payload_ptr->is_response_error();
    break;
    
  case RSP_DATA_WRITE:
    m_cost_write_transaction++;
    wait(m_rsp_received);
    if(m_pdes_local_time->get()>=m_rsp_time){
      m_vci_rsp_fsm = RSP_IDLE;
      m_dcache_write_req = false;
      if ( m_payload_ptr->is_response_error()) {
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << " write BERR" << std::endl;
#endif
	m_iss.setWriteBerr();
      }
    }
    else
      m_vci_rsp_fsm = RSP_DATA_WRITE_TIME_WAIT;
   break;
  case RSP_DATA_WRITE_TIME_WAIT:
    if(m_pdes_local_time->get()>=m_rsp_time){
      m_vci_rsp_fsm = RSP_IDLE;
      m_dcache_write_req = false;
      if ( m_payload_ptr->is_response_error()) {
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << " write BERR" << std::endl;
#endif
	m_iss.setWriteBerr();
      }
    }
    break;
  case RSP_DATA_UNC:
    m_cost_unc_transaction++;
    wait(m_rsp_received);
    m_dcache_miss_buf[0] = atou(m_payload_ptr->get_data_ptr(), 0);
    m_vci_rsp_fsm = RSP_IDLE;
    m_dcache_unc_req = false;
    m_vci_rsp_data_error = m_payload_ptr->is_response_error();
    break;
    
  } // end switch m_vci_rsp_fsm
}

////////////////////////////////
tmpl (void)::send_null_message()
{
  // set the null message command
  m_null_extension_ptr->set_null_message();
  m_null_extension_ptr->set_src_id(m_id);
  // set the extension to tlm payload
  m_null_payload_ptr->set_extension(m_null_extension_ptr);
  //set the tlm phase
  m_null_phase = tlm::BEGIN_REQ;
  //set the local time to transaction time
  m_null_time = m_pdes_local_time->get();
   
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << name() << " send NULL MESSAGE time = " << m_null_time.value() << std::endl;
#endif

  m_pdes_local_time->reset_sync();
  //send a null message
  p_vci->nb_transport_fw(*m_null_payload_ptr, m_null_phase, m_null_time);
  //deschedule the initiator thread
  wait(m_rsp_received);

#ifdef SOCLIB_MODULE_DEBUG
  std::cout << name() << " receive time = " << m_pdes_local_time->get().value() << std::endl;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////
// Interface function called when receiving a VCI response
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::nb_transport_bw    
( tlm::tlm_generic_payload           &payload,       // payload
  tlm::tlm_phase                     &phase,         // phase
  sc_core::sc_time                   &time)          // time
{
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << name() << " Receive response time = " << time.value() << std::endl;
#endif

  //update local time if the command message is different to write
  soclib_payload_extension *extension_ptr;
  payload.get_extension(extension_ptr);
  if(!extension_ptr->is_null_message()){

    m_rsp_time = time;
    
    if(!extension_ptr->is_write()){
      update_time(time);
    }
  }

  m_rsp_received.notify (sc_core::SC_ZERO_TIME);
    
  return tlm::TLM_COMPLETED;
}

// Not implemented for this example but required by interface
tmpl(void)::invalidate_direct_mem_ptr               // invalidate_direct_mem_ptr
( sc_dt::uint64 start_range,                        // start range
  sc_dt::uint64 end_range                           // end range
) 
{
}

/////////////////////////////////////////////////////////////////////////////////////
// Interface function called when receiving an IRQ event
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::irq_nb_transport_fw
( int                      id,       
  tlm::tlm_generic_payload &payload, 
  tlm::tlm_phase           &phase,  
  sc_core::sc_time         &time)  
{
   uint8_t	value = payload.get_data_ptr()[0];

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = " << std::dec << time.value()
          << " receive Interrupt " << id << " / value = " << (int)value << std::endl;
#endif

    assert(time.value() >= m_pending_time[id].value() 
               && "IRQ event received with a wrong date");

    m_pending_irq[id] = ( value != 0 );
    m_pending_time[id] = time;

    return tlm::TLM_COMPLETED;
}  


}}


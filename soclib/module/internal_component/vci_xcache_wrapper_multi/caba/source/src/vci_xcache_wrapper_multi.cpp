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
 *         Alain Greiner <alain.greiner@lip6.fr>
 *
 * Maintainers: alain
 */

#include <cassert>
#include <limits>
#include "iss2.h"
#include "arithmetics.h"
#include "../include/vci_xcache_wrapper_multi.h"

namespace soclib {
namespace caba {

namespace {
const char *dcache_fsm_state_str[] = {
        "DCACHE_IDLE",
        "DCACHE_WRITE_UPDT",
        "DCACHE_WRITE_REQ ",
        "DCACHE_MISS_SELECT",
        "DCACHE_MISS_INVAL",
        "DCACHE_MISS_WAIT",
        "DCACHE_UNC_WAIT",
        "DCACHE_XTN_HIT",
        "DCACHE_XTN_INVAL",
        "DCACHE__XTN_SYNC",
    };
const char *icache_fsm_state_str[] = {
        "ICACHE_IDLE     ",
        "ICACHE_MISS_SELECT",
        "ICACHE_MISS_INVAL",
        "ICACHE_MISS_WAIT",
        "ICACHE_UNC_WAIT ",
    };
const char *cmd_fsm_state_str[] = {
        "CMD_IDLE      ",
        "CMD_INS_MISS  ",
        "CMD_INS_UNC   ",
        "CMD_DATA_MISS ",
        "CMD_DATA_UNC  ",
        "CMD_DATA_WRITE",
    };
const char *rsp_fsm_state_str[] = {
        "RSP_IDLE      ",
        "RSP_INS_MISS  ",
        "RSP_INS_UNC   ",
        "RSP_DATA_MISS ",
        "RSP_DATA_UNC  ",
        "RSP_DATA_WRITE",
    };
}

#define tmpl(...)  template<typename vci_param, typename iss_t> __VA_ARGS__ VciXcacheWrapperMulti<vci_param, iss_t>

using soclib::common::uint32_log2;

//////////////////////////////////
tmpl(/**/)::VciXcacheWrapperMulti(
    sc_module_name name,
    int proc_id,
    const soclib::common::MappingTable &mt,
    const soclib::common::IntTab &index,
    size_t icache_ways,
    size_t icache_sets,
    size_t icache_words,
    size_t dcache_ways,
    size_t dcache_sets,
    size_t dcache_words, 
    size_t wbuf_nwords,
    size_t wbuf_nlines )
    :
      soclib::caba::BaseModule(name),

      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci"),

      m_cacheability_table(mt.getCacheabilityTable()),
      m_srcid(mt.indexForId(index)),

      m_dcache_ways(dcache_ways),
      m_dcache_words(dcache_words),
      m_dcache_yzmask((~0)<<(uint32_log2(dcache_words) + 2)),
      m_icache_ways(icache_ways),
      m_icache_words(icache_words),
      m_icache_yzmask((~0)<<(uint32_log2(icache_words) + 2)),

      m_iss(this->name(), proc_id),

      r_dcache_fsm("r_dcache_fsm"),
      r_dcache_addr_save("r_dcache_addr_save"),
      r_dcache_wdata_save("r_dcache_wdata_save"),
      r_dcache_type_save("r_dcache_type_save"),
      r_dcache_be_save("r_dcache_be_save"),
      r_dcache_cacheable_save("r_dcache_cacheable_save"),
      r_dcache_way_save("r_dcache_way_save"),
      r_dcache_set_save("r_dcache_set_save"),
      r_dcache_word_save("r_dcache_word_save"),
      r_dcache_miss_req("r_dcache_miss_req"),
      r_dcache_unc_req("r_dcache_unc_req"),

      r_icache_fsm("r_icache_fsm"),
      r_icache_addr_save("r_icache_addr_save"),
      r_icache_miss_req("r_icache_miss_req"),
      r_icache_unc_req("r_icache_unc_req"),

      r_vci_cmd_fsm("r_vci_cmd_fsm"),
      r_vci_cmd_min("r_vci_cmd_min"),
      r_vci_cmd_max("r_vci_cmd_max"),
      r_vci_cmd_cpt("r_vci_cmd_cpt"),

      r_vci_rsp_fsm("r_vci_rsp_fsm"),
      r_vci_rsp_ins_error("r_vci_rsp_ins_error"),
      r_vci_rsp_data_error("r_vci_rsp_data_error"),
      r_vci_rsp_cpt("r_vci_rsp_cpt"),

      r_vci_rsp_fifo_ins("r_vci_rsp_fifo_ins", 2),
      r_vci_rsp_fifo_data("r_vci_rsp_fifo_data", 2),

      r_wbuf("wbuf", wbuf_nwords, wbuf_nlines, dcache_words ),
      r_icache("icache", icache_ways, icache_sets, icache_words),
      r_dcache("dcache", dcache_ways, dcache_sets, dcache_words)
{
    assert( (icache_words*vci_param::B) < (1<<vci_param::K) && 
            "I need more PLEN bits");

    assert( (vci_param::T > 2) && ((1<<(vci_param::T-1)) >= wbuf_nlines) &&
            "I need more TRDID bits");

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

    typename iss_t::CacheInfo cache_info;
    cache_info.has_mmu = false;
    cache_info.icache_line_size = icache_words*sizeof(data_t);
    cache_info.icache_assoc = icache_ways;
    cache_info.icache_n_lines = icache_sets;
    cache_info.dcache_line_size = dcache_words*sizeof(data_t);
    cache_info.dcache_assoc = dcache_ways;
    cache_info.dcache_n_lines = dcache_sets;
    m_iss.setCacheInfo(cache_info);
}

////////////////////////////////////
tmpl(/**/)::~VciXcacheWrapperMulti()
{
}

///////////////////////////////////////
tmpl(void)::cache_monitor( addr_t addr)
{
    bool        cache_hit;
    size_t      cache_way = 0;
    size_t      cache_set = 0;
    size_t      cache_word = 0;
    uint32_t    cache_rdata = 0;

    // data cache
    cache_hit = r_dcache.read_neutral( addr,
                                       &cache_rdata,
                                       &cache_way,
                                       &cache_set,
                                       &cache_word );

    if ( (cache_hit != m_debug_previous_d_hit ) or
         (cache_hit and (cache_rdata != m_debug_previous_d_rdata) ) )
    {
        std::cout << "Monitor Proc " << name()
                  << " DCACHE at cycle " << std::dec << m_cpt_total_cycles
                  << " / HIT = " << cache_hit
                  << " / ADR = " << std::hex << addr
                  << " / DATA = " << cache_rdata 
                  << " / WAY = " << cache_way << std::endl;
        m_debug_previous_d_hit   = cache_hit;
        m_debug_previous_d_rdata = cache_rdata;
    }

    // icache monitor
    cache_hit = r_icache.read_neutral( addr,
                                       &cache_rdata,
                                       &cache_way,
                                       &cache_set,
                                       &cache_word );

    if ( (cache_hit != m_debug_previous_i_hit ) or
         (cache_hit and (cache_rdata != m_debug_previous_i_rdata) ) )
        
    {
        std::cout << "Monitor Proc " << name()
                  << " ICACHE at cycle " << std::dec << m_cpt_total_cycles
                  << " / HIT = " << cache_hit
                  << " / ADR = " << std::hex << addr
                  << " / DATA = " << cache_rdata 
                  << " / WAY = " << cache_way << std::endl;
        m_debug_previous_i_hit   = cache_hit;
        m_debug_previous_i_rdata = cache_rdata;
    }
}

////////////////////////////////////
tmpl(void)::print_trace(size_t mode)
{
    std::cout << std::dec << "Proc " << name() << std::endl;

    std::cout << "  " << m_ireq << std::endl;
    std::cout << "  " << m_irsp << std::endl;
    std::cout << "  " << m_dreq << std::endl;
    std::cout << "  " << m_drsp << std::endl;

    std::cout << "  " << icache_fsm_state_str[r_icache_fsm.read()]
              << " | " << dcache_fsm_state_str[r_dcache_fsm.read()]
              << " | " << cmd_fsm_state_str[r_vci_cmd_fsm.read()]
              << " | " << rsp_fsm_state_str[r_vci_rsp_fsm.read()] << std::endl;
    if(mode & 0x1)
    {
        r_wbuf.printTrace();
    }
    if(mode & 0x2) 
    {
        std::cout << "  Data cache" << std::endl;
        r_dcache.printTrace();
    }
    if(mode & 0x4) 
    {
        std::cout << "  Instruction cache" << std::endl;
        r_icache.printTrace();
    }
}
/////////////////////////
tmpl(void)::print_stats()
{
        std::cout << "------------------------------------" << std:: dec << std::endl
        << "CPU " << m_srcid << " / cycles = " << m_cpt_total_cycles << std::endl
        << "- CPI               = " << (float)m_cpt_total_cycles/m_cpt_exec_ins << std::endl
        << "- READ RATE         = " << (float)m_cpt_read/m_cpt_exec_ins << std::endl
        << "- WRITE RATE        = " << (float)m_cpt_write/m_cpt_exec_ins << std::endl
        << "- UNC RATE          = " << (float)m_cpt_data_unc/m_cpt_exec_ins << std::endl
        << "- CACHED WRITE RATE = " << (float)m_cpt_write_cached/m_cpt_write << std::endl
        << "- IMISS_RATE        = " << (float)m_cpt_ins_miss/m_cpt_exec_ins << std::endl
        << "- DMISS RATE        = " << (float)m_cpt_data_miss/m_cpt_read << std::endl
        << "- IMISS COST        = " << (float)m_cost_ins_miss_frz/m_cpt_ins_miss << std::endl
        << "- DMISS COST        = " << (float)m_cost_data_miss_frz/m_cpt_data_miss << std::endl
        << "- DUNC COST         = " << (float)m_cost_data_unc_frz/m_cpt_data_unc << std::endl
        << "- WRITE COST        = " << (float)m_cost_write_frz/m_cpt_write << std::endl
        << "- WRITE LENGTH      = " << (float)m_length_write_transaction/m_count_write_transaction
        << std::endl;
}

//////////////////////////
tmpl(void)::transition()
{
    if ( ! p_resetn.read() ) 
    {
        m_iss.reset();

        // FSM states
        r_dcache_fsm 	= DCACHE_IDLE;
        r_icache_fsm 	= ICACHE_IDLE;
        r_vci_cmd_fsm 	= CMD_IDLE;
        r_vci_rsp_fsm 	= RSP_IDLE;

        // write buffer, caches & fifos
        r_wbuf.reset();
        r_icache.reset();
        r_dcache.reset();
        r_vci_rsp_fifo_ins.init();
        r_vci_rsp_fifo_data.init();

        // synchronisation flip-flops from ICACHE & DCACHE FSMs to CMD  FSM
        r_icache_miss_req    = false;
        r_dcache_miss_req    = false;
        r_icache_unc_req     = false;
        r_dcache_unc_req     = false;

        // synchronisation flip-flops from the RSP FSM to the ICACHE or DCACHE FSMs
        r_vci_rsp_data_error     = false;
        r_vci_rsp_ins_error      = false;

        // debug variables
        m_debug_previous_d_hit   = false;
        m_debug_previous_d_rdata = 0;
        m_debug_previous_i_hit   = false;
        m_debug_previous_i_rdata = 0;

        // activity counters
        m_cpt_dcache_read  = 0;
        m_cpt_dcache_write = 0;
        m_cpt_icache_read  = 0;
        m_cpt_icache_write = 0;

        m_cpt_exec_ins     = 0;
        m_cpt_total_cycles = 0;

        m_cpt_read         = 0;
        m_cpt_write        = 0;
        m_cpt_data_miss    = 0;
        m_cpt_ins_miss     = 0;
        m_cpt_data_unc     = 0;
        m_cpt_write_cached = 0;

        m_cost_write_frz     = 0;
        m_cost_data_miss_frz = 0;
        m_cost_data_unc_frz  = 0;
        m_cost_ins_miss_frz  = 0;

        m_length_write_transaction = 0;
        m_count_write_transaction  = 0;

        return;
    }

    // Response FIFOs default values
    bool    vci_rsp_fifo_ins_get   = false;
    bool    vci_rsp_fifo_ins_put   = false;
    data_t  vci_rsp_fifo_ins_data  = 0;

    bool    vci_rsp_fifo_data_get  = false;
    bool    vci_rsp_fifo_data_put  = false;
    data_t  vci_rsp_fifo_data_data = 0;

    // update cycles counter
    m_cpt_total_cycles++;

    // get processor requests
    m_iss.getRequests( m_ireq, m_dreq );

    /////////////////////////////////////////////////////////////////////
    // The ICACHE FSM controls the instruction cache.
    //
    // Processor requests are taken into account only in the IDLE state.
    // In case of MISS or in case of uncacheable request, the FSM 
    // writes the missing address line in the  r_icache_addr_save register 
    // and sets the r_icache_miss_req (or the r_icache_unc_req) flip-flop.
    // The request flip-flop is reset by the CMD FSM when the VCI command
    // has been send. 
    // In case of bus error, the VCI_RSP FSM sets the r_vci_rsp_ins_error
    // flip-flop. It is reset by the ICACHE FSM.
    ///////////////////////////////////////////////////////////////////////

    // default values for m_irsp
    m_irsp.valid       = false;
    m_irsp.error       = false;
    m_irsp.instruction = 0;

    switch(r_icache_fsm) {
    /////////////////
    case ICACHE_IDLE:
    {
        if (m_ireq.valid ) 
        {
            data_t  icache_ins = 0;
            bool    icache_hit = false;

            m_cpt_icache_read++;
            
            bool    icache_cacheable = m_cacheability_table[(uint64_t)m_ireq.addr];

            if ( icache_cacheable ) 
            {
                icache_hit = r_icache.read( m_ireq.addr, &icache_ins);
                if ( not icache_hit ) 	// miss
                {
                    m_cpt_ins_miss++;
                    m_cost_ins_miss_frz++;

                    r_icache_addr_save = m_ireq.addr;
                    r_icache_word_save = 0;
                    r_icache_fsm       = ICACHE_MISS_SELECT;
                    r_icache_miss_req  = true;
                }
                else                    // hit
                {
                    m_irsp.valid        = true;
                    m_irsp.instruction  = icache_ins;
                }
            }
            else                         // non cacheable
            {
                m_cpt_ins_miss++;
                m_cost_ins_miss_frz++;

                r_icache_addr_save = m_ireq.addr;
                r_icache_fsm 	   = ICACHE_UNC_WAIT;
                r_icache_unc_req   = true;
            }
        }
        break;
    }
    ////////////////////////
    case ICACHE_MISS_SELECT:          // selects a slot in an associative set
    {
        m_cost_ins_miss_frz++;

        bool    valid;
        size_t  way;
        size_t  set;
        addr_t  victim;  // unused

        valid = r_icache.victim_select( r_icache_addr_save.read(),
                                        &victim,
                                        &way,
                                        &set );
        r_icache_way_save = way;
        r_icache_set_save = set;
        if ( valid ) r_icache_fsm = ICACHE_MISS_INVAL;
        else         r_icache_fsm = ICACHE_MISS_WAIT;
        break;
    }
    ///////////////////////
    case ICACHE_MISS_INVAL:         // invalidate the selected slot
    {
        m_cost_ins_miss_frz++;

        addr_t   nline;  // unused

        r_icache.inval( r_icache_way_save.read(),
                        r_icache_set_save.read(),
                        &nline );

        r_icache_fsm = ICACHE_MISS_WAIT;
        break;
    }
    //////////////////////
    case ICACHE_MISS_WAIT:         // wait response and update dcache 
    {
        m_cost_ins_miss_frz++;
       
        if ( r_vci_rsp_ins_error.read() )  // error reported 
        {
            m_irsp.valid         = true;
            m_irsp.error         = true;
            r_vci_rsp_ins_error  = false;
            r_icache_fsm         = ICACHE_IDLE;
        }
        else if ( r_vci_rsp_fifo_ins.rok() ) // available instruction
        {
            m_cpt_icache_write++;

            vci_rsp_fifo_ins_get = true;
            r_icache_word_save   = r_icache_word_save.read() + 1;
            r_icache.write( r_icache_way_save.read(),
                            r_icache_set_save.read(),
                            r_icache_word_save.read(),
                            r_vci_rsp_fifo_ins.read() );

            if ( r_icache_word_save.read() == m_icache_words - 1 )  // last word
            {
                r_icache.victim_update_tag( r_icache_addr_save.read(),
                                            r_icache_way_save.read(),
                                            r_icache_set_save.read() );
                r_icache_fsm = ICACHE_IDLE;
            }
        }
        break;
    }
    /////////////////////
    case ICACHE_UNC_WAIT:    // wait response
    {
        if ( r_vci_rsp_ins_error.read() )   // error reported 
        {
            m_irsp.valid         = true;
            m_irsp.error         = true;
            r_vci_rsp_ins_error  = false;
            r_icache_fsm         = ICACHE_IDLE;
        }
        else if ( r_vci_rsp_fifo_ins.rok() ) // available instruction
        {
            vci_rsp_fifo_ins_get = true;
            if ( m_ireq.valid and (m_ireq.addr == r_icache_addr_save.read()) )
            {
                m_irsp.valid         = true;
                m_irsp.instruction   = r_vci_rsp_fifo_ins.read();
            }
            r_icache_fsm         = ICACHE_IDLE;
        }
        break;
    }
    } // end switch r_icache_fsm

    ///////////////////////////////////////////////////////////////////////////////////
    // The DCACHE FSM controls the data cache.
    //
    // - In order to support write burst, the processor requests are taken 
    //   into account in the WRITE_REQ state as well as in the IDLE state.
    // - In IDLE state, the request is satisfied if it is a cacheable read hit, 
    //   or a cacheable write. 
    // - In WRITE_REQ state, the request is satisfied if it is a cacheable read hit,
    //   or a cacheable write, only if the write buffer is not full.
    // - Both the uncacheable read and the uncachable write requests block the 
    //   processor until the corresponding VCI transaction is completed.
    // 
    // It uses an advanced Write buffer supporting simultaneous write bursts.
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
    //   r_vci_rsp_data_error flip-flop, and the synchronous error is signaled
    //   by the DCACHE FSM.
    // - If a Write Bus Error is detected, the VCI_RSP FSM  signals
    //   the asynchronous error using the setWriteBerr() method.
    ////////////////////////////////////////////////////////////////////////////////////

    // default value for m_drsp
   m_drsp.valid   = false;
   m_drsp.error   = false;
   m_drsp.rdata   = 0;

    switch ( r_dcache_fsm ) {
    //////////////////////
    case DCACHE_WRITE_REQ:  // only cacheable write are written in wbuf
    {
        if( !r_wbuf.write( r_dcache_addr_save.read(), 
                           r_dcache_be_save.read(), 
                           r_dcache_wdata_save.read(),
                           true ) ) 
        {
            //  stay in DCACHE_WRITEREQ state if the request is not accepted 
            m_cost_write_frz++;
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
            dcache_hit 		    = r_dcache.read( m_dreq.addr, 
                                                 &dcache_rdata,
                                                 &dcache_way,
                                                 &dcache_set,
                                                 &dcache_word );

            // Save proc request and cache response
            r_dcache_addr_save      = m_dreq.addr;
            r_dcache_type_save      = m_dreq.type;
            r_dcache_wdata_save     = m_dreq.wdata;
            r_dcache_be_save        = m_dreq.be;
            r_dcache_cacheable_save = dcache_cacheable;
            r_dcache_way_save       = dcache_way;
            r_dcache_set_save       = dcache_set;
            r_dcache_word_save      = dcache_word;

            // compute next FSM state, VCI request, and processor response 
            if( m_dreq.type  == iss_t::DATA_READ )
            {
                if( not dcache_cacheable ) 					// uncachable read 
                {
                    m_cpt_data_unc++;
                    m_cost_data_unc_frz++;

                    r_dcache_unc_req   	= true;
                    r_dcache_fsm 		= DCACHE_UNC_WAIT;
	            }
                else
                {
                    m_cpt_read++;

                    if( dcache_hit )					// cacheable read hit
                    {
                        m_drsp.valid 	= true;
                        m_drsp.rdata 	= dcache_rdata;
                        r_dcache_fsm 	= DCACHE_IDLE;
                    }
                    else 		         	           // cacheable read miss
                    {
                        m_cpt_data_miss++;
                        m_cost_data_miss_frz++;

                        r_dcache_word_save  = 0;
                        r_dcache_miss_req  	= true;
                        r_dcache_fsm 		= DCACHE_MISS_SELECT;
                    }
                }
            }
            else if( m_dreq.type == iss_t::DATA_WRITE )
            {
                if( not dcache_cacheable ) 	          // uncacheable write
                {
                    m_cpt_data_unc++;
                    m_cost_data_unc_frz++;

                    r_dcache_unc_req   	= true;
                    r_dcache_fsm 		= DCACHE_UNC_WAIT;
                }
                else
                {
                    m_cpt_write++;

                    if( not dcache_hit ) 				// cacheable write miss 
                    {
                        m_drsp.rdata 	= 0;
                        m_drsp.valid 	= true;
                        r_dcache_fsm 	= DCACHE_WRITE_REQ;
                    }
                    else 						       // cacheable write hit 
                    {
                        m_cpt_write_cached++;

                        m_drsp.rdata 	= 0;
                        m_drsp.valid 	= true;
                        r_dcache_fsm 	= DCACHE_WRITE_UPDT;
                    }
                }
            }
            else if( (m_dreq.type == iss_t::DATA_LL) or
                     (m_dreq.type == iss_t::DATA_SC) )
            //  LL & SC requests are handled as uncacheable	
            {
                m_cpt_data_unc++;
                m_cost_data_unc_frz++;

                r_dcache_unc_req   		= true;
                r_dcache_fsm 			= DCACHE_UNC_WAIT;
            }
            else if( (m_dreq.type == iss_t::XTN_WRITE) or 
                     (m_dreq.type == iss_t::XTN_READ) ) 
            // only INVAL & SYNC requests are supported
            {
                if ( m_dreq.addr/4 == iss_t::XTN_DCACHE_INVAL ) 
                {
                    r_dcache_fsm = DCACHE_XTN_HIT;
                }
                else if ( m_dreq.addr/4 == iss_t::XTN_SYNC ) 	
                {
                    r_dcache_fsm = DCACHE_XTN_SYNC;
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
            r_dcache_fsm = DCACHE_IDLE;
        }
        break;
    }
    ///////////////////////
    case DCACHE_WRITE_UPDT:     // update local copy in dcache
    {
        m_cpt_dcache_write++;

        r_dcache.write( r_dcache_way_save.read(),
                        r_dcache_set_save.read(),
                        r_dcache_word_save.read(),
                        r_dcache_wdata_save.read(),
                        r_dcache_be_save.read() );
        r_dcache_fsm = DCACHE_WRITE_REQ;
        break;
    }
    ////////////////////////
    case DCACHE_MISS_SELECT:     // select a slot in an associative set
    {
        m_cost_data_miss_frz++;

        bool     valid;
        size_t   way;
        size_t   set;
        addr_t   victim;   // unused

        valid = r_dcache.victim_select( r_dcache_addr_save.read(),
                                        &victim,
                                        &way,
                                        &set );
        r_dcache_way_save = way;
        r_dcache_set_save = set;
        if ( valid ) r_dcache_fsm = DCACHE_MISS_INVAL;
        else         r_dcache_fsm = DCACHE_MISS_WAIT;
        break;
    }
    ///////////////////////
    case DCACHE_MISS_INVAL:     // invalidate the selected slot
    {
        m_cost_data_miss_frz++;

        addr_t   nline;  // unused

        r_dcache.inval( r_dcache_way_save.read(),
                        r_dcache_set_save.read(),
                        &nline );

        r_dcache_fsm = DCACHE_MISS_WAIT;
        break;
    }
    //////////////////////
    case DCACHE_MISS_WAIT:    // wait a response and update the dcache
    {
        m_cost_data_miss_frz++;

        if ( r_vci_rsp_data_error.read() )     // error reported
        {
            m_drsp.valid         = true;
            m_drsp.error         = true;
            r_vci_rsp_data_error = false;
        	r_dcache_fsm         = DCACHE_IDLE;
        }
        else if ( r_vci_rsp_fifo_data.rok() )  // available data
        {
            m_cpt_dcache_write++;

            vci_rsp_fifo_data_get = true;
            r_dcache_word_save    = r_dcache_word_save.read() + 1;
            r_dcache.write( r_dcache_way_save.read(),
                            r_dcache_set_save.read(),
                            r_dcache_word_save.read(),
                            r_vci_rsp_fifo_data.read() );
                            
            if ( r_dcache_word_save.read() == m_dcache_words-1 )  // last word
            {
                r_dcache.victim_update_tag( r_dcache_addr_save.read(),
                                            r_dcache_way_save.read(),
                                            r_dcache_set_save.read() );
           	    r_dcache_fsm = DCACHE_IDLE;
            }
        }
        break;
    }
    /////////////////////
    case DCACHE_UNC_WAIT:      // wait the response
    {
        m_cost_data_unc_frz++;

        if ( r_vci_rsp_data_error.read() )    // error reported
        {
            m_drsp.valid         = true;
            m_drsp.error         = true;
            r_vci_rsp_data_error = false;
        	r_dcache_fsm         = DCACHE_IDLE;
        }
        else if ( r_vci_rsp_fifo_data.rok() )   // available data
        {
            vci_rsp_fifo_data_get = true;
            if ( m_dreq.valid and 
                 (m_dreq.addr == r_dcache_addr_save.read()) ) // request un modified
            {
                m_drsp.valid = true;
                m_drsp.rdata = r_vci_rsp_fifo_data.read();
            }
            r_dcache_fsm = DCACHE_IDLE;
        }
        break;
    }
    ////////////////////
    case DCACHE_XTN_HIT:     // checking hit for an XTN_INVAL request
    {
        uint32_t  data;    // unused
        size_t    word;    // unused
        size_t    way;
        size_t    set;
        bool      hit = r_dcache.read( r_dcache_wdata_save.read(),
                                       &data,
                                       &way,
                                       &set,
                                       &word );
        if( hit )   // inval to be done
        {
            r_dcache_way_save = way;
            r_dcache_set_save = set;
            r_dcache_fsm      = DCACHE_XTN_INVAL;
        }
        else        // miss: nothing to do
        {
            m_drsp.valid = true;
            r_dcache_fsm = DCACHE_IDLE;
        }
        break;
    }
    //////////////////////
    case DCACHE_XTN_INVAL:    // invalidate the selected slot
    {
        addr_t nline;   // unused
        r_dcache.inval( r_dcache_way_save.read(),
                        r_dcache_set_save.read(),
                        &nline );
        r_dcache_fsm = DCACHE_IDLE;
        m_drsp.valid = true;
        break;
    }
    /////////////////////
    case DCACHE_XTN_SYNC:     // waiting write buffer empty
    {
        if ( r_wbuf.empty() ) 
        {
            r_dcache_fsm = DCACHE_IDLE;
            m_drsp.valid = true;
        }
        break;
    }

    } // end DCACHE_FSM switch

    ////////// write buffer state update ////////////////////////////////////////////
    // The update() method must be called at each cycle to update the internal state.
	r_wbuf.update();

    /////////// execute one iss cycle i//////////////////////////////////////////////
    uint32_t it = 0;
    for (size_t i=0; i<(size_t)iss_t::n_irq; i++)
    {
        if(p_irq[i].read()) it |= (1<<i);
    }
    m_iss.executeNCycles(1, m_irsp, m_drsp, it);

    ///////// compute number of executed instructions //////////////////////////////
    if ( m_ireq.valid and m_irsp.valid and 
         (not m_dreq.valid or m_drsp.valid) and
         (m_ireq.addr != m_pc_previous) )
    {
        m_cpt_exec_ins++;
        m_pc_previous = m_ireq.addr;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // The CMD FSM handles requests from 3 clients : DCACHE, ICACHE and WBUF
    //
    // There is 5 request types, with the following priorities :
    // 1 - Data Read Miss          : r_dcache_miss_req & wbuf.miss
    // 2 - Instruction Miss        : r_icache_miss_req & wbuf.miss
    // 3 - Data Write              : r_wbuf.rok()
    // 3 - Data Uncacheable        : r_dcache_unc_req
    // 5 - Instruction Uncacheable : r_icache_unc_req
    //
    // VCI formats:
    // According to the VCI advanced specification, all read command packets
    // (uncacheable read or cachable miss) are one word packets.
    // For write burst packets, all words must be in the same write buffer line,
    // and addresses must be contiguous (the BE field is 0 in case of "holes").
    // The PLEN VCI field is always documented.
    // As simultaneous VCI transactions are supported, the TRDID field is used:
    // - Write transactions : TRDID = wbuf_index + (1<<(trdid_size-1))
    // - Read transactions  : TRDID = 2*cacheable + 1*instruction
    ///////////////////////////////////////////////////////////////////////////////////

    switch (r_vci_cmd_fsm) {
    /////////////
    case CMD_IDLE:
    {
        size_t min;
        size_t max;
        if ( (r_dcache_miss_req.read()) && (r_wbuf.miss(r_dcache_addr_save.read())) )	
        {
            r_vci_cmd_fsm = CMD_DATA_MISS;
            r_dcache_miss_req = false;
        } 
        else if ( (r_icache_miss_req.read()) && (r_wbuf.miss(r_icache_addr_save.read())) )	
        {
            r_vci_cmd_fsm = CMD_INS_MISS;
            r_icache_miss_req = false;
        } 
        else if ( r_wbuf.rok(&min, &max) ) 
        {
            r_vci_cmd_fsm   = CMD_DATA_WRITE;
            r_vci_cmd_min   = min;
            r_vci_cmd_max   = max;
            r_vci_cmd_cpt   = min;
            m_count_write_transaction++;
            m_length_write_transaction += (max-min+1);
        }
        else if ( r_dcache_unc_req ) 
        {
            r_vci_cmd_fsm = CMD_DATA_UNC;
            r_dcache_unc_req = false;
        }
        else if ( r_icache_unc_req ) 
        {
            r_vci_cmd_fsm = CMD_INS_UNC;
            r_icache_unc_req = false;
        } 
        break;
    }
    ////////////////////
    case CMD_DATA_WRITE:
    {
        if ( p_vci.cmdack.read() ) 
        {
            r_vci_cmd_cpt = r_vci_cmd_cpt + 1;
            if (r_vci_cmd_cpt == r_vci_cmd_max) 
            {
                r_vci_cmd_fsm = CMD_IDLE ;
                r_wbuf.sent() ;
            }
        }
        break;
    }
    ///////////////////
    case CMD_DATA_MISS:
    case CMD_DATA_UNC:
    case CMD_INS_MISS:
    case CMD_INS_UNC:
    {
        if ( p_vci.cmdack.read() )  r_vci_cmd_fsm = CMD_IDLE;
        break;
    }
    } // end  switch r_vci_cmd_fsm

    /////////////////////////////////////////////////////////////////////////////////
    // The RSP FSM dispatches responses to the 3 clients : DCACHE, ICACHE, WBUF
    //
    // As simultaneous VCI transactions are supported, the TRDID field is used:
    // - Write transactions : TRDID = wbuf_index + (1<<(trdid_size-1))
    // - Read transactions  : TRDID = 2*cacheable + instruction
    //
    // Error handling:
    // - In case of Write error, the error is directly signaled by the RSP FSM.
    // - In case of Read Data Error, the VCI_RSP FSM sets the r_vci_rsp_data_error 
    //   flip_flop and the error is signaled by the DCACHE FSM.  
    // - In case of Instruction Error, the VCI_RSP FSM sets the r_vci_rsp_ins_error 
    //   flip_flop and the error is signaled by the ICACHE FSM.  
    /////////////////////////////////////////////////////////////////////////////////

    switch (r_vci_rsp_fsm) {
    //////////////
    case RSP_IDLE:
    {
        if( p_vci.rspval.read() ) 
        {
            r_vci_rsp_cpt = 0;
            if ( (p_vci.rtrdid.read()>>(vci_param::T-1)) != 0 ) r_vci_rsp_fsm = RSP_DATA_WRITE;
            else if ( p_vci.rtrdid.read() == TYPE_DATA_MISS ) 	r_vci_rsp_fsm = RSP_DATA_MISS;
            else if ( p_vci.rtrdid.read() == TYPE_DATA_UNC ) 	r_vci_rsp_fsm = RSP_DATA_UNC;
            else if ( p_vci.rtrdid.read() == TYPE_INS_MISS ) 	r_vci_rsp_fsm = RSP_INS_MISS;
            else if ( p_vci.rtrdid.read() == TYPE_INS_UNC ) 	r_vci_rsp_fsm = RSP_INS_UNC;
        } 
        break;
    }
    ////////////////////
    case RSP_DATA_WRITE:
    {
        if ( p_vci.rspval.read() )
        {
            assert( p_vci.reop.read() && 
               "A VCI response packet must contain one flit for a write transaction" ); 
            r_vci_rsp_fsm = RSP_IDLE;
            r_wbuf.completed( p_vci.rtrdid.read() - (1<<(vci_param::T-1)) );
            if ( (p_vci.rerror.read() & 0x1) == 0x1 )  m_iss.setWriteBerr();
        }
        break;
    }
    //////////////////
    case RSP_INS_MISS:
    {
        if ( p_vci.rspval.read() )
        {
            if( (p_vci.rerror.read()&0x1 != 0) )  // error reported
            {
                r_vci_rsp_ins_error = true;
                if( p_vci.reop.read() ) r_vci_rsp_fsm = RSP_IDLE;
            }
            else if( r_vci_rsp_fifo_ins.wok() )   // fifo not full
            {
                assert( (r_vci_rsp_cpt < m_icache_words) &&
                "The VCI response packet for instruction miss is too long" );
                r_vci_rsp_cpt         = r_vci_rsp_cpt + 1;
                vci_rsp_fifo_ins_put  = true;
                vci_rsp_fifo_ins_data = p_vci.rdata.read();
                if ( p_vci.reop.read() ) 
                {
                    assert( (r_vci_rsp_cpt == m_icache_words - 1) &&
                    "The VCI response packet for instruction miss is too short");
                    r_vci_rsp_fsm = RSP_IDLE;
                }
            }
        }
        break;
    }
    /////////////////
    case RSP_INS_UNC:
    {
        if ( p_vci.rspval.read() )
        {
            assert( p_vci.reop.read() and
            "illegal VCI response for uncached instruction");
            if( (p_vci.rerror.read()&0x1 != 0) )  // error reported
            {
                r_vci_rsp_ins_error = true;
                r_vci_rsp_fsm       = RSP_IDLE;
            }
            else if( r_vci_rsp_fifo_ins.wok() )   // fifo not full
            {
                vci_rsp_fifo_ins_put  = true;
                vci_rsp_fifo_ins_data = p_vci.rdata.read();
                r_vci_rsp_fsm         = RSP_IDLE;
            }
        }
        break;
    }
    ///////////////////
    case RSP_DATA_MISS:
    {
        if ( p_vci.rspval.read() )
        {
            if( (p_vci.rerror.read()&0x1 != 0) )  // error reported
            {
                r_vci_rsp_data_error = true;
                if( p_vci.reop.read() ) r_vci_rsp_fsm = RSP_IDLE;
            }
            else if( r_vci_rsp_fifo_data.wok() )   // fifo not full
            {
                assert( (r_vci_rsp_cpt < m_dcache_words) &&
                "The VCI response packet for data miss is too long" );
                r_vci_rsp_cpt          = r_vci_rsp_cpt + 1;
                vci_rsp_fifo_data_put  = true;
                vci_rsp_fifo_data_data = p_vci.rdata.read();
                if ( p_vci.reop.read() ) 
                {
                    assert( (r_vci_rsp_cpt == m_dcache_words - 1) &&
                    "The VCI response packet for instruction miss is too short");
                    r_vci_rsp_fsm = RSP_IDLE;
                }
            }
        }
        break;
    }
    //////////////////
    case RSP_DATA_UNC:
    {
        if ( p_vci.rspval.read() )
        {
            assert( p_vci.reop.read() and
            "illegal VCI response for uncached data");
            if( (p_vci.rerror.read()&0x1 != 0) )  // error reported
            {
                r_vci_rsp_data_error = true;
                r_vci_rsp_fsm        = RSP_IDLE;
            }
            else if( r_vci_rsp_fifo_data.wok() )   // fifo not full
            {
                vci_rsp_fifo_data_put  = true;
                vci_rsp_fifo_data_data = p_vci.rdata.read();
                r_vci_rsp_fsm          = RSP_IDLE;
            }
        }
        break;
    }
    } // end switch r_vci_rsp_fsm

    //////////////// Response FIFOs update ////////////////////////////////////////
    r_vci_rsp_fifo_ins.update(  vci_rsp_fifo_ins_get,
                                vci_rsp_fifo_ins_put,
                                vci_rsp_fifo_ins_data );

    r_vci_rsp_fifo_data.update( vci_rsp_fifo_data_get,
                                vci_rsp_fifo_data_put,
                                vci_rsp_fifo_data_data );

} // end transition()

//////////////////////
tmpl(void)::genMoore()
{
    // VCI initiator response

    p_vci.rspack = ( r_vci_rsp_fsm != RSP_IDLE );

    // VCI initiator command

    switch (r_vci_cmd_fsm) {

    case CMD_IDLE:
        p_vci.cmdval  = false;
        p_vci.address = 0;
        p_vci.wdata   = 0;
        p_vci.be      = 0;
        p_vci.plen    = 0;
        p_vci.cmd     = vci_param::CMD_WRITE;
        p_vci.trdid   = 0;
        p_vci.pktid   = 0;
        p_vci.srcid   = 0;
        p_vci.cons    = false;
        p_vci.wrap    = false;
        p_vci.contig  = false;
        p_vci.clen    = 0;
        p_vci.cfixed  = false;
        p_vci.eop     = false;
        break;

    case CMD_DATA_WRITE:
        p_vci.cmdval  = true;
        p_vci.address = r_wbuf.getAddress(r_vci_cmd_cpt);
        p_vci.wdata   = r_wbuf.getData(r_vci_cmd_cpt);
        p_vci.be      = r_wbuf.getBe(r_vci_cmd_cpt);
        p_vci.plen    = (r_vci_cmd_max - r_vci_cmd_min + 1)<<2;
        p_vci.cmd     = vci_param::CMD_WRITE;
        p_vci.pktid   = 0;
        p_vci.trdid   = r_wbuf.getIndex() + (1<<(vci_param::T-1));
        p_vci.srcid   = m_srcid;
        p_vci.cons    = false;
        p_vci.wrap    = false;
        p_vci.contig  = true;
        p_vci.clen    = 0;
        p_vci.cfixed  = false;
        p_vci.eop     = (r_vci_cmd_cpt == r_vci_cmd_max);
        break;

    case CMD_DATA_MISS:
        p_vci.cmdval = true;
        p_vci.address = r_dcache_addr_save & m_dcache_yzmask;
        p_vci.be     = 0xF;
        p_vci.plen   = m_dcache_words << 2;
        p_vci.cmd    = vci_param::CMD_READ;
        p_vci.pktid  = 0;
        p_vci.trdid  = TYPE_DATA_MISS;
        p_vci.srcid  = m_srcid;
        p_vci.cons   = false;
        p_vci.wrap   = false;
        p_vci.contig = true;
        p_vci.clen   = 0;
        p_vci.cfixed = false;
        p_vci.eop    = true;
        break;

    case CMD_DATA_UNC:
        p_vci.cmdval = true;
        p_vci.address = r_dcache_addr_save & ~0x3;
        switch( r_dcache_type_save ) {
        case iss_t::DATA_WRITE:
            p_vci.wdata = r_dcache_wdata_save.read();
            p_vci.be  = r_dcache_be_save.read();
            p_vci.cmd = vci_param::CMD_WRITE;
            break;
        case iss_t::DATA_READ:
            p_vci.wdata = 0;
            p_vci.be  = r_dcache_be_save.read();
            p_vci.cmd = vci_param::CMD_READ;
            break;
        case iss_t::DATA_LL:
            p_vci.wdata = 0;
            p_vci.be  = 0xF;
            p_vci.cmd = vci_param::CMD_LOCKED_READ;
            break;
        case iss_t::DATA_SC:
            p_vci.wdata = r_dcache_wdata_save.read();
            p_vci.be  = 0xF;
            p_vci.cmd = vci_param::CMD_STORE_COND;
            break;
        default:
            assert("this should not happen");
        }
        p_vci.plen = 4;
        p_vci.pktid  = 0;
        p_vci.trdid  = TYPE_DATA_UNC;
        p_vci.srcid  = m_srcid;
        p_vci.cons   = false;
        p_vci.wrap   = false;
        p_vci.contig = true;
        p_vci.clen   = 0;
        p_vci.cfixed = false;
        p_vci.eop    = true;
        break;

    case CMD_INS_MISS:
        p_vci.cmdval = true;
        p_vci.address = r_icache_addr_save & m_icache_yzmask;
        p_vci.be     = 0xF;
        p_vci.plen   = m_icache_words << 2;
        p_vci.cmd    = vci_param::CMD_READ;
        p_vci.pktid  = 0;
        p_vci.trdid  = TYPE_INS_MISS;
        p_vci.srcid  = m_srcid;
        p_vci.cons   = false;
        p_vci.wrap   = false;
        p_vci.contig = true;
        p_vci.clen   = 0;
        p_vci.cfixed = false;
        p_vci.eop = true;
        break;

    case CMD_INS_UNC:
        p_vci.cmdval = true;
        p_vci.address = r_icache_addr_save & ~0x3;
        p_vci.be     = 0xF;
        p_vci.plen   = 4;
        p_vci.cmd    = vci_param::CMD_READ;
        p_vci.pktid  = 0;
        p_vci.trdid  = TYPE_INS_UNC;
        p_vci.srcid  = m_srcid;
        p_vci.cons   = false;
        p_vci.wrap   = false;
        p_vci.contig = true;
        p_vci.clen   = 0;
        p_vci.cfixed = false;
        p_vci.eop    = true;
        break;
    } // end switch r_vci_cmd_fsm

} // end genMoore()

}} // end namespace

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4





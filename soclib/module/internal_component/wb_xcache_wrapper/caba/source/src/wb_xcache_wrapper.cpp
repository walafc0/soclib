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
 * Wishbone iss2 xcache wrapper
 * Author: tarik.graba@telecom-paristech.fr
 *
 * based on vci_xcache_wrapper
 * which is copyrighted by  UPMC, Lip6, SoC
 *         Alain Greiner <alain.greiner@lip6.fr>, 2008
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: graba
 */


#include <cassert>
#include <limits>
#include "arithmetics.h"
#include "wb_xcache_wrapper.h"

namespace soclib {
    namespace caba {


#define tmpl(...)  template<typename wb_param, typename iss_t> __VA_ARGS__ WbXcacheWrapper<wb_param, iss_t>

        using soclib::common::uint32_log2;

        /////////////////////////////////
        tmpl(/**/)::WbXcacheWrapper(
                /////////////////////////////////
                sc_module_name name,
                int proc_id,
                const soclib::common::MappingTable &mt,
                const soclib::common::IntTab &index,
                size_t icache_ways,
                size_t icache_sets,
                size_t icache_words,
                size_t dcache_ways,
                size_t dcache_sets,
                size_t dcache_words)
            :
                soclib::caba::BaseModule(name),

                p_clk("clk"),
                p_resetn("resetn"),
                p_wb("wb"),

                m_cacheability_table(mt.getCacheabilityTable()),
                m_iss(this->name(), proc_id),
                m_srcid(mt.indexForId(index)),

                m_dcache_ways(dcache_ways),
                m_dcache_words(dcache_words),
                m_dcache_yzmask((~0)<<(uint32_log2(dcache_words) + 2)),
                m_icache_ways(icache_ways),
                m_icache_words(icache_words),
                m_icache_yzmask((~0)<<(uint32_log2(icache_words) + 2)),

                r_dcache_fsm("r_dcache_fsm"),
                r_dcache_addr_save("r_dcache_addr_save"),
                r_dcache_wdata_save("r_dcache_wdata_save"),
                r_dcache_rdata_save("r_dcache_rdata_save"),
                r_dcache_type_save("r_dcache_type_save"),
                r_dcache_be_save("r_dcache_be_save"),
                r_dcache_cached_save("r_dcache_cached_save"),
                r_dcache_miss_req("r_dcache_miss_req"),
                r_dcache_unc_req("r_dcache_unc_req"),
                r_dcache_write_req("r_dcache_write_req"),

                r_icache_fsm("r_icache_fsm"),
                r_icache_addr_save("r_icache_addr_save"),
                r_icache_miss_req("r_icache_miss_req"),
                r_icache_unc_req("r_icache_unc_req"),

                r_wb_trans_fsm("r_wb_trans_fsm"),
                r_wb_cmd_min("r_wb_cmd_min"),
                r_wb_cmd_max("r_wb_cmd_max"),
                r_wb_cmd_cpt("r_wb_cmd_cpt"),

                r_wb_rsp_cpt("r_wb_rsp_cpt"),
                r_wb_rsp_ins_error("r_wb_rsp_ins_error"),
                r_wb_rsp_data_error("r_wb_rsp_data_error"),

                r_icache_buf_unc_valid("r_icache_buf_unc_valid"),
                r_dcache_buf_unc_valid("r_dcache_buf_unc_valid"),

                r_wbuf("wbuf", dcache_words ),
                r_icache("icache", icache_ways, icache_sets, icache_words),
                r_dcache("dcache", dcache_ways, dcache_sets, dcache_words)
                {
                    r_icache_miss_buf = new data_t[icache_words];
                    r_dcache_miss_buf = new data_t[dcache_words];

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

        /////////////////////////////////
        tmpl(/**/)::~WbXcacheWrapper()
            /////////////////////////////////
        {
            delete [] r_icache_miss_buf;
            delete [] r_dcache_miss_buf;
        }

        ////////////////////////
        tmpl(void)::print_cpi()
            ////////////////////////
        {
            std::cout << name() << " CPU " << m_srcid << " : CPI = "
                << (float)m_cpt_total_cycles/(m_cpt_total_cycles - m_cpt_frz_cycles) << std::endl;
        }
        ////////////////////////
        tmpl(void)::print_stats()
            ////////////////////////
        {
            float run_cycles = (float)(m_cpt_total_cycles - m_cpt_frz_cycles);
            std::cout << name() << std::endl
                << "- CPI                = " << (float)m_cpt_total_cycles/run_cycles << std::endl
                << "- READ RATE          = " << (float)m_cpt_read/run_cycles << std::endl
                << "- WRITE RATE         = " << (float)m_cpt_write/run_cycles << std::endl
                << "- UNCACHED READ RATE = " << (float)m_cpt_unc_read/m_cpt_read << std::endl
                << "- CACHED WRITE RATE  = " << (float)m_cpt_write_cached/m_cpt_write << std::endl
                << "- IMISS_RATE         = " << (float)m_cpt_ins_miss/run_cycles << std::endl
                << "- DMISS RATE         = " << (float)m_cpt_data_miss/(m_cpt_read-m_cpt_unc_read) << std::endl
                << "- INS MISS COST      = " << (float)m_cost_ins_miss_frz/m_cpt_ins_miss << std::endl
                << "- IMISS TRANSACTION  = " << (float)m_cost_imiss_transaction/m_cpt_imiss_transaction << std::endl
                << "- DMISS COST         = " << (float)m_cost_data_miss_frz/m_cpt_data_miss << std::endl
                << "- DMISS TRANSACTION  = " << (float)m_cost_dmiss_transaction/m_cpt_dmiss_transaction << std::endl
                << "- UNC COST           = " << (float)m_cost_unc_read_frz/m_cpt_unc_read << std::endl
                << "- UNC TRANSACTION    = " << (float)m_cost_unc_transaction/m_cpt_unc_transaction << std::endl
                << "- WRITE COST         = " << (float)m_cost_write_frz/m_cpt_write << std::endl
                << "- WRITE TRANSACTION  = " << (float)m_cost_write_transaction/m_cpt_write_transaction << std::endl
                << "- WRITE LENGTH       = " << (float)m_length_write_transaction/m_cpt_write_transaction << std::endl;
        }

        //////////////////////////
        tmpl(void)::transition()
            //////////////////////////
        {
            if ( ! p_resetn.read() ) {
                m_iss.reset();

                // FSM states
                r_dcache_fsm = DCACHE_IDLE;
                r_icache_fsm = ICACHE_IDLE;
                r_wb_trans_fsm = TRANS_IDLE;

                // write buffer & caches
                r_wbuf.reset();
                r_icache.reset();
                r_dcache.reset();

                // synchronisation flip-flops from ICACHE & DCACHE FSMs to VCI  FSMs
                r_icache_miss_req    = false;
                r_dcache_miss_req    = false;
                r_dcache_unc_req     = false;
                r_dcache_write_req   = false;

                // signals from the VCI RSP FSM to the ICACHE or DCACHE FSMs
                r_dcache_buf_unc_valid = false;
                r_icache_buf_unc_valid = false;
                r_wb_rsp_data_error   = false;
                r_wb_rsp_ins_error    = false;

                // activity counters
                m_cpt_dcache_data_read  = 0;
                m_cpt_dcache_data_write = 0;
                m_cpt_dcache_dir_read  = 0;
                m_cpt_dcache_dir_write = 0;
                m_cpt_icache_data_read  = 0;
                m_cpt_icache_data_write = 0;
                m_cpt_icache_dir_read  = 0;
                m_cpt_icache_dir_write = 0;

                m_cpt_frz_cycles = 0;
                m_cpt_total_cycles = 0;

                m_cpt_read = 0;
                m_cpt_write = 0;
                m_cpt_data_miss = 0;
                m_cpt_ins_miss = 0;
                m_cpt_unc_read = 0;
                m_cpt_write_cached = 0;

                m_cost_write_frz = 0;
                m_cost_data_miss_frz = 0;
                m_cost_unc_read_frz = 0;
                m_cost_ins_miss_frz = 0;

                m_cpt_imiss_transaction = 0;
                m_cpt_dmiss_transaction = 0;
                m_cpt_unc_transaction = 0;
                m_cpt_write_transaction = 0;

                m_cost_imiss_transaction = 0;
                m_cost_dmiss_transaction = 0;
                m_cost_unc_transaction = 0;
                m_cost_write_transaction = 0;
                m_length_write_transaction = 0;

                return;
            }

#ifdef SOCLIB_MODULE_DEBUG
            std::cout
                << name()
                << " dcache fsm: " << dcache_fsm_state_str[r_dcache_fsm]
                << " icache fsm: " << icache_fsm_state_str[r_icache_fsm]
                << " WB transaction fsm: " << trans_fsm_state_str[r_wb_trans_fsm]
                << std::endl;
#endif

            m_cpt_total_cycles++;

            /////////////////////////////////////////////////////////////////////
            // The ICACHE FSM controls the following ressources:
            // - r_icache_fsm
            // - r_icache (instruction cache access)
            // - r_icache_addr_save
            // - r_icache_buf_unc_valid
            // - r_icache_miss_req set
            // - r_icache_unc_req set
            // - r_icache_unc_req set
            // - r_wb_rsp_ins_error reset
            // - ireq & irsp structures for communication with the processor
            //
            // Processor requests are taken into account only in the IDLE state.
            // In case of MISS, or in case of uncached instruction, the FSM
            // writes the missing address line in the  r_icache_addr_save register
            // and sets the r_icache_miss_req (or the r_icache_unc_req) flip-flop.
            // The request flip-flop is reset by the VCI_RSP FSM when the VCI
            // transaction is completed.
            // The r_icache_buf_unc_valid is set in case of uncached access.
            // In case of bus error, the VCI_RSP FSM sets the r_wb_rsp_ins_error
            // flip-flop. It is reset by the ICACHE FSM.
            ///////////////////////////////////////////////////////////////////////

            typename iss_t::InstructionRequest ireq = ISS_IREQ_INITIALIZER;
            typename iss_t::InstructionResponse irsp = ISS_IRSP_INITIALIZER;

            typename iss_t::DataRequest dreq = ISS_DREQ_INITIALIZER;
            typename iss_t::DataResponse drsp = ISS_DRSP_INITIALIZER;

            m_iss.getRequests( ireq, dreq );

#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " Instruction Request: " << ireq << std::endl;
#endif

            switch(r_icache_fsm) {

                case ICACHE_IDLE:
                    if ( ireq.valid ) {
                        data_t  icache_ins = 0;
                        bool    icache_hit = false;
                        addr_t  ireq_paddr = ireq.addr;
                        m_iss.virtualToPhys(ireq_paddr);

                        bool    icache_cached = m_cacheability_table[ireq_paddr];

                        // icache_hit & icache_ins evaluation
                        if ( icache_cached ) {
                            icache_hit = r_icache.read(ireq_paddr, &icache_ins);
                        } else {
                            icache_hit = ( r_icache_buf_unc_valid && (ireq_paddr == r_icache_addr_save) );
                            icache_ins = r_icache_miss_buf[0];
                        }
                        if ( ! icache_hit ) {
                            m_cpt_ins_miss++;
                            m_cost_ins_miss_frz++;
                            r_icache_addr_save = ireq_paddr;
                            if ( icache_cached ) {
                                r_icache_fsm = ICACHE_MISS_WAIT;
                                r_icache_miss_req = true;
                            } else {
                                r_icache_fsm = ICACHE_UNC_WAIT;
                                r_icache_unc_req = true;
                                r_icache_buf_unc_valid = false;
                            }
                        } else {
                            r_icache_buf_unc_valid = false;
                        }
                        m_cpt_icache_dir_read += m_icache_ways;
                        m_cpt_icache_data_read += m_icache_ways;
                        irsp.valid          = icache_hit;
                        irsp.instruction    = icache_ins;
                    }
                    break;

                case ICACHE_MISS_WAIT:
                    m_cost_ins_miss_frz++;
                    if ( !r_icache_miss_req ) {
                        if ( r_wb_rsp_ins_error ) {
                            r_icache_fsm = ICACHE_ERROR;
                            r_wb_rsp_ins_error = false;
                        } else {
                            r_icache_fsm = ICACHE_MISS_UPDT;
                        }
                    }
                    break;

                case ICACHE_UNC_WAIT:
                    m_cost_ins_miss_frz++;
                    if ( !r_icache_unc_req ) {
                        if ( r_wb_rsp_ins_error ) {
                            r_icache_fsm = ICACHE_ERROR;
                            r_wb_rsp_ins_error = false;
                        } else {
                            r_icache_fsm = ICACHE_IDLE;
                            r_icache_buf_unc_valid = true;
                        }
                    }
                    break;

                case ICACHE_ERROR:
                    r_icache_fsm = ICACHE_IDLE;
                    r_wb_rsp_ins_error = false;
                    irsp.valid          = true;
                    irsp.error          = true;
                    break;

                case ICACHE_MISS_UPDT:
                    {
                        addr_t  ad  = r_icache_addr_save;
                        data_t* buf = r_icache_miss_buf;
                        data_t  victim_index = 0;
                        m_cpt_icache_dir_write++;
                        m_cpt_icache_data_write++;
                        m_cost_ins_miss_frz++;
                        r_icache.update(ad, buf, &victim_index);
                        r_icache_fsm = ICACHE_IDLE;
                        break;
                    }

            } // end switch r_icache_fsm

#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " Instruction Response: " << irsp << std::endl;
#endif

            ///////////////////////////////////////////////////////////////////////////////////
            // The DCACHE FSM controls the following ressources:
            // - r_dcache_fsm
            // - r_dcache (data cache access)
            // - r_dcache_addr_save
            // - r_dcache_wdata_save
            // - r_dcache_rdata_save
            // - r_dcache_type_save
            // - r_dcache_be_save
            // - r_dcache_cached_save
            // - r_dcache_buf_unc_valid
            // - r_dcache_miss_req set
            // - r_dcache_unc_req set
            // - r_dcache_write_req set
            // - r_wb_rsp_data_error reset
            // - r_wbuf write
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
            //   - CACHED READ MISS => to the MISS_WAIT state (waiting the r_miss_ok signal),
            //     then to the MISS_UPDT state, and finally to the IDLE state.
            //   - UNCACHED READ  => to the UNC_WAIT state (waiting the r_miss_ok signal),
            //     and to the IDLE state.
            //   - CACHE INVALIDATE HIT => to the INVAL state for one cycle, then to IDLE state.
            //   - WRITE MISS => directly to the WRITE_REQ state to access the write buffer.
            //   - WRITE HIT => to the WRITE_UPDT state, then to the WRITE_REQ state.
            //
            // Error handling :  Read Bus Errors are synchronous events, but
            // Write Bus Errors are asynchronous events (processor is not frozen).
            // - If a Read Bus Error is detected, the VCI_RSP FSM sets the
            //   r_wb_rsp_data_error flip-flop, and the synchronous error is signaled
            //   by the DCACHE FSM.
            // - If a Write Bus Error is detected, the VCI_RSP FSM  signals
            //   the asynchronous error using the setWriteBerr() method.
            ////////////////////////////////////////////////////////////////////////

#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " Data Request: " << dreq << std::endl;
#endif

            switch ( r_dcache_fsm ) {

                case DCACHE_WRITE_REQ:
                    // try to post the write request in the write buffer
                    if ( !r_dcache_write_req ) {
                        // no previous write transaction
                        if ( r_wbuf.wok(r_dcache_addr_save) ) {
                            // write request in the same cache line
                            r_wbuf.write(r_dcache_addr_save, r_dcache_be_save, r_dcache_wdata_save);

                            // closing the write packet if uncached
                            if ( !r_dcache_cached_save )
                                r_dcache_write_req = true ;
                        } else {
                            // close the write packet if write request not in the same cache line
                            r_dcache_write_req = true;
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
                    if ( !dreq.valid || dreq.type != iss_t::DATA_WRITE )
                        r_dcache_write_req = true ;

                    // The next state and the processor request parameters are computed
                    // as in the DCACHE_IDLE state (see below ...)

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
                            dcache_hit = r_dcache.read(dreq_paddr, &dcache_rdata);
                        } else {
                            dcache_hit = ( (dreq_paddr == r_dcache_addr_save) && r_dcache_buf_unc_valid );
                            dcache_rdata = r_dcache_miss_buf[0];
                        }

                        switch( dreq.type ) {
                            case iss_t::DATA_READ:
                            case iss_t::DATA_LL:
                            case iss_t::DATA_SC:
                                m_cpt_read++;
                                if ( dcache_hit ) {
                                    r_dcache_fsm = DCACHE_IDLE;
                                    drsp.valid = true;
                                    drsp.rdata = dcache_rdata;
                                    r_dcache_buf_unc_valid = false;
                                } else {
                                    if ( dcache_cached ) {
                                        m_cpt_data_miss++;
                                        m_cost_data_miss_frz++;
                                        r_dcache_miss_req = true;
                                        r_dcache_fsm = DCACHE_MISS_WAIT;
                                    } else {
                                        m_cpt_unc_read++;
                                        m_cost_unc_read_frz++;
                                        r_dcache_unc_req = true;
                                        r_dcache_fsm = DCACHE_UNC_WAIT;
                                    }
                                }
                                break;
                            case iss_t::XTN_READ:
                            case iss_t::XTN_WRITE:
                                // only DCACHE INVALIDATE request are supported
                                switch ( dreq_paddr/4 ) {
                                    case iss_t::XTN_DCACHE_INVAL:
                                        r_dcache_fsm = DCACHE_INVAL;
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
                                    r_dcache_fsm = DCACHE_WRITE_UPDT;
                                    m_cpt_write_cached++;
                                } else {
                                    r_dcache_fsm = DCACHE_WRITE_REQ;
                                }
                                drsp.valid = true;
                                drsp.rdata = 0;
                                break;
                        } // end switch dreq.type

                        r_dcache_addr_save      = dreq_paddr;
                        r_dcache_type_save      = dreq.type;
                        r_dcache_wdata_save     = dreq.wdata;
                        r_dcache_be_save        = dreq.be;
                        r_dcache_rdata_save     = dcache_rdata;
                        r_dcache_cached_save    = dcache_cached;

                    } else {    // if no dcache_req
                        r_dcache_fsm = DCACHE_IDLE;
                    }
                    // processor request are not accepted in the WRITE_REQUEST state
                    // when the write buffer is not writeable
                    if ( (r_dcache_fsm == DCACHE_WRITE_REQ) &&
                            (r_dcache_write_req || !r_wbuf.wok(r_dcache_addr_save)) ) {
                        drsp.valid = false;
                    }
                    break;

                case DCACHE_WRITE_UPDT:
                    {
                        m_cpt_dcache_data_write++;
                        data_t mask = wb_param::be2mask(r_dcache_be_save);
                        data_t wdata = (mask & r_dcache_wdata_save) | (~mask & r_dcache_rdata_save);
                        r_dcache.write(r_dcache_addr_save, wdata);
                        r_dcache_fsm = DCACHE_WRITE_REQ;
                        break;
                    }

                case DCACHE_MISS_WAIT:
                    if ( dreq.valid ) m_cost_data_miss_frz++;
                    if ( !r_dcache_miss_req ) {
                        if ( r_wb_rsp_data_error )
                            r_dcache_fsm = DCACHE_ERROR;
                        else
                            r_dcache_fsm = DCACHE_MISS_UPDT;
                    }
                    break;

                case DCACHE_MISS_UPDT:
                    {
                        addr_t  ad  = r_dcache_addr_save;
                        data_t* buf = r_dcache_miss_buf;
                        data_t  victim_index = 0;
                        if ( dreq.valid )
                            m_cost_data_miss_frz++;
                        m_cpt_dcache_data_write++;
                        m_cpt_dcache_dir_write++;
                        r_dcache.update(ad, buf, &victim_index);
                        r_dcache_fsm = DCACHE_IDLE;
                        break;
                    }

                case DCACHE_UNC_WAIT:
                    if ( dreq.valid ) m_cost_unc_read_frz++;
                    if ( !r_dcache_unc_req ) {
                        if ( r_wb_rsp_data_error ) {
                            r_dcache_fsm = DCACHE_ERROR;
                        } else {
                            r_dcache_fsm = DCACHE_IDLE;
                            // If request was a DATA_SC we need to invalidate the corresponding cache line,
                            // so that subsequent access to this line are read from RAM
                            if (dreq.type == iss_t::DATA_SC) {
                                r_dcache_fsm = DCACHE_INVAL;
                                r_dcache_wdata_save = r_dcache_addr_save;
                            }
                            r_dcache_buf_unc_valid = true;
                        }
                    }
                    break;

                case DCACHE_ERROR:
                    r_dcache_fsm = DCACHE_IDLE;
                    r_wb_rsp_data_error = false;
                    drsp.error = true;
                    drsp.valid = true;
                    break;

                case DCACHE_INVAL:
                    m_cpt_dcache_dir_read += m_dcache_ways;
                    r_dcache.inval(r_dcache_wdata_save);
                    r_dcache_fsm = DCACHE_IDLE;
                    break;
            }


#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " Data Response: " << drsp << std::endl;
#endif

            /////////// execute one iss cycle /////////////////////////////////
            {
                uint32_t it = 0;
                for (size_t i=0; i<(size_t)iss_t::n_irq; i++)
                    if(p_irq[i].read())
                        it |= (1<<i);

                m_iss.executeNCycles(1, irsp, drsp, it);
            }

            if ( (ireq.valid && !irsp.valid) || (dreq.valid && !drsp.valid) )
                m_cpt_frz_cycles++;


            ////////////////////////////////////////////////////////////////////////////
            // TODO Write DOC
            // From TRANS_IDLE we start transaction with the following priority:
            //      1. r_icache_miss_req --> Instruction miss
            //      2. r_icache_unc_req ---> Uncached instruction
            //      3. r_dcache_write_req -> Write request
            //      4. r_dcache_miss_req --> Data miss
            //      5. r_dcache_unc_req ---> Uncached data
            //   This order must be preserved to avoid issues when reading after writing 
            //   as the cache is write throw.
            //////////////////////////////////////////////////////////////////////////////

            switch (r_wb_trans_fsm) {

                // nothing to do
                case TRANS_IDLE:
                    r_wb_rsp_cpt = 0;
                    if      ( r_icache_miss_req )  {
                        r_wb_trans_fsm = INS_MISS;
                        m_cpt_imiss_transaction++;
                    }
                    else if ( r_icache_unc_req )   {
                        r_wb_trans_fsm = INS_UNC;
                        m_cpt_imiss_transaction++;
                    }
                    else if ( r_dcache_write_req ) {
                        r_wb_trans_fsm = DATA_WRITE;
                        m_cpt_write_transaction++;

                        r_wb_cmd_cpt = r_wbuf.getMin();
                        r_wb_cmd_min = r_wbuf.getMin();
                        r_wb_cmd_max = r_wbuf.getMax();
                        m_length_write_transaction += (r_wbuf.getMax() - r_wbuf.getMin() + 1);

                    }
                    else if ( r_dcache_miss_req )  {
                        r_wb_trans_fsm = DATA_MISS;
                        m_cpt_dmiss_transaction++;
                    }
                    else if ( r_dcache_unc_req )   {
                        r_wb_trans_fsm = DATA_UNC;
                        m_cpt_unc_transaction++;
                    }
                    break;

                    // Read inst cache line after a miss
                case INS_MISS:
                    m_cost_imiss_transaction++;
                    if (! p_wb.ACK_I )
                        break;
                    r_icache_miss_buf[r_wb_rsp_cpt] = (data_t)p_wb.DAT_I.read();
                    // end of response
                    if (r_wb_rsp_cpt == m_icache_words - 1) {
                        r_icache_miss_req = false;
                        r_wb_trans_fsm = TRANS_IDLE;
                    }

                    r_wb_rsp_cpt = r_wb_rsp_cpt + 1;

                    if ( p_wb.ERR_I )
                        r_wb_rsp_ins_error = true;
                    break;

                    // read a single uncached data
                case INS_UNC:
                    m_cost_imiss_transaction++;
                    if (! p_wb.ACK_I )
                        break;
                    r_icache_miss_buf[0] = (data_t)p_wb.DAT_I.read();
                    r_icache_buf_unc_valid = true;
                    r_icache_unc_req = false;

                    r_wb_trans_fsm = TRANS_IDLE;

                    if ( p_wb.ERR_I )
                        r_wb_rsp_ins_error = true;
                    break;

                    // Read data cache line after a miss
                case DATA_MISS:
                    m_cost_dmiss_transaction++;
                    if (! p_wb.ACK_I )
                        break;
                    r_dcache_miss_buf[r_wb_rsp_cpt] = (data_t)p_wb.DAT_I.read();
                    // end of response
                    if (r_wb_rsp_cpt == m_dcache_words - 1) {
                        r_dcache_miss_req = false;
                        r_wb_trans_fsm = TRANS_IDLE;
                    }

                    r_wb_rsp_cpt = r_wb_rsp_cpt + 1;

                    if ( p_wb.ERR_I )
                        r_wb_rsp_data_error = true;
                    break;

                    // Read single uncached data
                case DATA_UNC:
                    m_cost_unc_transaction++;
                    if (! p_wb.ACK_I )
                        break;
                    r_dcache_miss_buf[0] = (data_t)p_wb.DAT_I.read();

                    r_dcache_unc_req = false;
                    r_wb_trans_fsm = TRANS_IDLE;

                    if ( p_wb.ERR_I )
                        r_wb_rsp_data_error = true;
                    break;

                    // write buffer to RAM
                case DATA_WRITE:
                    m_cost_write_transaction++;
                    if (! p_wb.ACK_I )
                        break;
                    if (r_wb_cmd_cpt == r_wb_cmd_max) {

                        r_wb_trans_fsm = TRANS_IDLE ;
                        r_dcache_write_req = false;

                        r_wbuf.reset() ;
                    }
                    r_wb_cmd_cpt = r_wb_cmd_cpt + 1;

                    if ( p_wb.ERR_I ) {
#ifdef SOCLIB_MODULE_DEBUG
                        std::cout << name() << " write BERR" << std::endl;
#endif
                        m_iss.setWriteBerr();
                    }
                    break;

            } // end  switch r_wb_trans_fsm

        } // end transition()

        //////////////////////////////////////////////////////////////////////////////////
        tmpl(void)::genMoore()

        {
            // WB transaction
            // default values
            p_wb.DAT_O   = 0;
            p_wb.ADR_O   = 0;
            p_wb.CYC_O   = false;
            p_wb.LOCK_O  = false;
            p_wb.SEL_O   = 0;
            p_wb.STB_O   = false;
            p_wb.WE_O    = false;

            switch (r_wb_trans_fsm) {

                case TRANS_IDLE:
                    // default values
                    break;
                case INS_MISS:
                    p_wb.CYC_O   = true;
                    p_wb.STB_O   = true;
                    p_wb.ADR_O = ( r_icache_addr_save & m_icache_yzmask ) + ( r_wb_rsp_cpt << 2);
                    p_wb.SEL_O = 0xF;
                    break;
                case INS_UNC:
                    p_wb.CYC_O   = true;
                    p_wb.STB_O   = true;
                    p_wb.ADR_O = r_icache_addr_save & ~0x3;
                    p_wb.SEL_O = 0xF;
                    break;
                case DATA_MISS:
                    p_wb.CYC_O   = true;
                    p_wb.STB_O   = true;
                    p_wb.ADR_O = ( r_dcache_addr_save & m_dcache_yzmask ) + ( r_wb_rsp_cpt << 2);
                    p_wb.SEL_O = 0xF;
                    break;
                case DATA_UNC:
                    p_wb.CYC_O   = true;
                    p_wb.STB_O   = true;
                    p_wb.ADR_O   = r_dcache_addr_save & ~0x3;
                    // what kind of data request
                    // TODO Use WB optional tags to add special data accesses
                    switch( r_dcache_type_save ) {
                        case iss_t::DATA_READ:
                            p_wb.SEL_O   = r_dcache_be_save.read();
                            // p_vci.cmd = wb_param::CMD_READ;
                            break;
                        case iss_t::DATA_LL:
                            p_wb.SEL_O  = 0xF;
                            // p_vci.cmd = vci_param::CMD_LOCKED_READ;
                            break;
                        case iss_t::DATA_SC:
                            p_wb.WE_O    = true;
                            p_wb.DAT_O = r_dcache_wdata_save.read();
                            p_wb.SEL_O  = 0xF;
                            // p_vci.cmd = vci_param::CMD_STORE_COND;
                            break;
                        default:
                            assert("Unsupported data access mode");
                    }
                    break;
                case DATA_WRITE:
                    p_wb.CYC_O   = true;
                    p_wb.STB_O   = true;
                    p_wb.WE_O    = true;
                    p_wb.ADR_O   = r_wbuf.getAddress(r_wb_cmd_cpt);
                    p_wb.DAT_O   = r_wbuf.getData(r_wb_cmd_cpt);
                    p_wb.SEL_O   = r_wbuf.getBe(r_wb_cmd_cpt);
                    break;

            } // end switch r_wb_trans_fsm

        } // end genMoore()

    }} // end namespace

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4





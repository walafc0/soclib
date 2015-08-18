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


#ifndef SOCLIB_CABA_WB_XCACHE_WRAPPER_H
#define SOCLIB_CABA_WB_XCACHE_WRAPPER_H

#include <inttypes.h>
#include <systemc>
#include "caba_base_module.h"
#include "write_buffer.h"
#include "generic_cache.h"
#include "wb_master.h"
#include "mapping_table.h"
#include "static_assert.h"

#ifdef item
#undef item
#endif

#define _DCACHE_FSM_               \
        item(DCACHE_IDLE        ), \
        item(DCACHE_WRITE_UPDT  ), \
        item(DCACHE_WRITE_REQ   ), \
        item(DCACHE_MISS_WAIT   ), \
        item(DCACHE_MISS_UPDT   ), \
        item(DCACHE_UNC_WAIT    ), \
        item(DCACHE_INVAL       ), \
        item(DCACHE_ERROR       ), \

#define _ICACHE_FSM_               \
        item(ICACHE_IDLE        ), \
        item(ICACHE_MISS_WAIT   ), \
        item(ICACHE_MISS_UPDT   ), \
        item(ICACHE_UNC_WAIT    ), \
        item(ICACHE_ERROR       ), \

#define _TRANS_FSM_                \
        item(TRANS_IDLE         ), \
        item(INS_MISS           ), \
        item(INS_UNC            ), \
        item(DATA_MISS          ), \
        item(DATA_UNC           ), \
        item(DATA_WRITE         ), \


namespace soclib {
    namespace caba {

        using namespace sc_core;

        ////////////////////////////////////////////
        template<typename wb_param, typename iss_t>
            class WbXcacheWrapper
            ///////////////////////////////////////////
            : public soclib::caba::BaseModule
            {
                typedef uint32_t    addr_t;
                typedef uint32_t    data_t;
                typedef uint32_t    tag_t;
                typedef uint32_t    be_t;

#define item(x) x
                enum dcache_fsm_state_e {
                    _DCACHE_FSM_
                };

                enum icache_fsm_state_e {
                    _ICACHE_FSM_
                };

                enum wb_trans_fsm_state_e {
                    _TRANS_FSM_
                };
#undef item

                public:

                // PORTS
                sc_in<bool>                             p_clk;
                sc_in<bool>                             p_resetn;
                sc_in<bool>                             p_irq[iss_t::n_irq];
                soclib::caba::WbMaster<wb_param>        p_wb;

                private:

                // STRUCTURAL PARAMETERS
                const soclib::common::AddressDecodingTable<uint32_t, bool>  m_cacheability_table;
                iss_t                                                       m_iss;
                const uint32_t                                              m_srcid;

                const size_t                                                m_dcache_ways;
                const size_t                                                m_dcache_words;
                const addr_t                                                m_dcache_yzmask;
                const size_t                                                m_icache_ways;
                const size_t                                                m_icache_words;
                const addr_t                                                m_icache_yzmask;

                // REGISTERS
                sc_signal<int>          r_dcache_fsm;
                sc_signal<addr_t>       r_dcache_addr_save;
                sc_signal<data_t>       r_dcache_wdata_save;
                sc_signal<data_t>       r_dcache_rdata_save;
                sc_signal<int>          r_dcache_type_save;
                sc_signal<be_t>         r_dcache_be_save;
                sc_signal<bool>         r_dcache_cached_save;
                sc_signal<bool>         r_dcache_miss_req;
                sc_signal<bool>         r_dcache_unc_req;
                sc_signal<bool>         r_dcache_write_req;

                sc_signal<int>          r_icache_fsm;
                sc_signal<addr_t>       r_icache_addr_save;
                sc_signal<bool>         r_icache_miss_req;
                sc_signal<bool>         r_icache_unc_req;

                sc_signal<int>          r_wb_trans_fsm;
                sc_signal<size_t>       r_wb_cmd_min;
                sc_signal<size_t>       r_wb_cmd_max;
                sc_signal<size_t>       r_wb_cmd_cpt;

                sc_signal<size_t>       r_wb_rsp_cpt;
                sc_signal<bool>         r_wb_rsp_ins_error;
                sc_signal<bool>         r_wb_rsp_data_error;

                data_t                  *r_icache_miss_buf;
                data_t                  *r_dcache_miss_buf;
                sc_signal<bool>         r_icache_buf_unc_valid;
                sc_signal<bool>         r_dcache_buf_unc_valid;

                WriteBuffer<addr_t>     r_wbuf;
                GenericCache<addr_t>    r_icache;
                GenericCache<addr_t>    r_dcache;

                // Activity counters
                uint32_t m_cpt_dcache_data_read;        // DCACHE DATA READ
                uint32_t m_cpt_dcache_data_write;       // DCACHE DATA WRITE
                uint32_t m_cpt_dcache_dir_read;         // DCACHE DIR READ
                uint32_t m_cpt_dcache_dir_write;        // DCACHE DIR WRITE

                uint32_t m_cpt_icache_data_read;        // ICACHE DATA READ
                uint32_t m_cpt_icache_data_write;       // ICACHE DATA WRITE
                uint32_t m_cpt_icache_dir_read;         // ICACHE DIR READ
                uint32_t m_cpt_icache_dir_write;        // ICACHE DIR WRITE

                uint32_t m_cpt_frz_cycles;	            // number of cycles where the cpu is frozen
                uint32_t m_cpt_total_cycles;	        // total number of cycles

                uint32_t m_cpt_read;                    // total number of read instructions
                uint32_t m_cpt_write;                   // total number of write instructions
                uint32_t m_cpt_data_miss;               // number of read miss
                uint32_t m_cpt_ins_miss;                // number of instruction miss
                uint32_t m_cpt_unc_read;                // number of read uncached
                uint32_t m_cpt_write_cached;            // number of cached write

                uint32_t m_cost_write_frz;              // number of frozen cycles related to write buffer
                uint32_t m_cost_data_miss_frz;          // number of frozen cycles related to data miss
                uint32_t m_cost_unc_read_frz;           // number of frozen cycles related to uncached read
                uint32_t m_cost_ins_miss_frz;           // number of frozen cycles related to ins miss

                uint32_t m_cpt_imiss_transaction;       // number of bus instruction miss transactions
                uint32_t m_cpt_dmiss_transaction;       // number of bus data miss transactions
                uint32_t m_cpt_unc_transaction;         // number of bus uncached read transactions
                uint32_t m_cpt_write_transaction;       // number of bus write transactions

                uint32_t m_cost_imiss_transaction;      // cumulated duration for bus IMISS transactions
                uint32_t m_cost_dmiss_transaction;      // cumulated duration for bus DMISS transactions
                uint32_t m_cost_unc_transaction;        // cumulated duration for bus UNC transactions
                uint32_t m_cost_write_transaction;      // cumulated duration for bus WRITE transactions
                uint32_t m_length_write_transaction;    // cumulated length for bus WRITE transactions

                protected:
                SC_HAS_PROCESS(WbXcacheWrapper);

                public:

                WbXcacheWrapper(
                        sc_module_name insname,
                        int proc_id,
                        const soclib::common::MappingTable &mt,
                        const soclib::common::IntTab &index,
                        size_t icache_ways,
                        size_t icache_sets,
                        size_t icache_words,
                        size_t dcache_ways,
                        size_t dcache_sets,
                        size_t dcache_words);

                ~WbXcacheWrapper();

                void print_cpi();
                void print_stats();

                private:

                void transition();
                void genMoore();

#ifdef SOCLIB_MODULE_DEBUG
                // FSM names
                static const char *const dcache_fsm_state_str[];
                static const char *const icache_fsm_state_str[];
                static const char *const trans_fsm_state_str[];
#endif

            };

#ifdef SOCLIB_MODULE_DEBUG
#define item(x) #x
        template<typename wb_param, typename iss_t>
            const char* const WbXcacheWrapper<wb_param, iss_t>::
            dcache_fsm_state_str[] = {
                _DCACHE_FSM_
            };
        template<typename wb_param, typename iss_t>
            const char* const WbXcacheWrapper<wb_param, iss_t>::
            icache_fsm_state_str[] = {
                _ICACHE_FSM_
            };
        template<typename wb_param, typename iss_t>
            const char* const WbXcacheWrapper<wb_param, iss_t>::
            trans_fsm_state_str[] = {
                _TRANS_FSM_
            };
#undef item
#endif
    }}

#endif /* SOCLIB_CABA_WB_XCACHE_WRAPPER_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4




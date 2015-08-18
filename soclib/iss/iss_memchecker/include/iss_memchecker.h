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
 * Copyright (c) UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 * Alexandre Becoulet <alexandre.becoulet@free.fr>, 2010
 *
 * Maintainers: nipo becoulet
 *
 * $Id$
 *
 */

#ifndef _SOCLIB_ISS_MEMCHECKER_H_
#define _SOCLIB_ISS_MEMCHECKER_H_

#include <stdint.h>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include <dpp/interval_set>

#include <systemc.h>
#include "iss2.h"
#include "exception.h"
#include "soclib_endian.h"
#include "mapping_table.h"
#include "loader.h"
#include "iss_memchecker_registers.h"

namespace soclib { namespace common {

namespace __iss_memchecker {
class ContextState;
class RegionInfo;
class MemoryState;
}

template<typename iss_t>
class IssMemchecker
    : public iss_t
{
    uint32_t m_comm_address;

    int m_fp_reg_id;
    int m_sp_reg_id;

    __iss_memchecker::ContextState *m_current_context;
    __iss_memchecker::ContextState *m_last_context;
    __iss_memchecker::RegionInfo *m_last_region_touched;
    bool m_has_data_answer;
    const uint32_t m_cpuid;
    uint32_t m_data_answer_value;
    struct iss_t::DataRequest m_last_data_access;
    struct iss_t::DataRequest m_blast_data_access;
    uint32_t m_enabled_checks;
    uint32_t m_r1;
    uint32_t m_r2;
    uint32_t m_delayed_pc_min;
    uint32_t m_delayed_pc_max;

    // processor spinlocks
    typedef std::map<uint32_t, bool /* cycle */> held_locks_map_t;
    held_locks_map_t m_held_locks;

    typedef dpp::interval_set<uint32_t, dpp::interval_bound_inclusive<uint32_t> > address_set_t;

    address_set_t m_bypass_pc;

    bool m_opt_dump_iss;
    bool m_opt_dump_access;
    bool m_opt_show_enable;
    bool m_opt_show_ctx;
    bool m_opt_show_ctxsw;
    bool m_opt_show_region;
    bool m_opt_show_lockops;
    bool m_opt_exit_on_error;

    uint32_t m_trap_mask;
    uint32_t m_report_mask;
    bool m_bypass;
    bool m_req_checked;

    uint32_t m_no_repeat_mask;

    enum magic_state_e {
        MAGIC_NONE,
        MAGIC_BE,
        MAGIC_LE,
        MAGIC_DELAYED,
    };
    enum magic_state_e m_magic_state;

    inline bool isMagicDreq(const struct iss_t::DataRequest &dreq) const
    {
        return dreq.valid &&
            ( ( m_magic_state != MAGIC_NONE && (dreq.addr & ~(uint32_t)0xff) == m_comm_address ) || 
             ( dreq.type == iss_t::DATA_WRITE && dreq.addr == m_comm_address &&
               ( dreq.wdata == ISS_MEMCHECKER_MAGIC_VAL ||
                 dreq.wdata == ISS_MEMCHECKER_MAGIC_VAL_SWAPPED ) ) );
    }

public:

    IssMemchecker(const std::string &name, uint32_t ident);

    uint32_t executeNCycles( uint32_t ncycle, struct iss_t::InstructionResponse irsp,
                             struct iss_t::DataResponse drsp, uint32_t irq_bit_field );

    inline void getRequests(
        struct iss_t::InstructionRequest &ireq,
        struct iss_t::DataRequest &dreq) const
    {
        iss_t::getRequests(ireq, dreq);
        if ( isMagicDreq(dreq) )
            dreq.valid = false;
    }

    static void init( const soclib::common::MappingTable &mt,
                      const soclib::common::Loader &loader,
                      const std::string &exclusions = "" );

private:

    void update_context(__iss_memchecker::ContextState *nc);

    void register_set( uint32_t reg_no, uint32_t value );
    uint32_t register_get( uint32_t reg_no ) const;
    void handle_comm( const struct iss_t::DataRequest &dreq );
    void check_data_access( const struct iss_t::DataRequest &dreq,
                            const struct iss_t::DataResponse &drsp );

    void report_error( uint32_t errors, uint32_t extra = 0 );
    void report_current_ctx();

    uint32_t get_cpu_sp() const;
    uint32_t get_cpu_fp() const;
    uint32_t get_cpu_pc() const;
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

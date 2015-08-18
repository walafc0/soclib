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
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: nipo
 *
 * $Id$
 */

#include "mips32.h"
#include "base_module.h"
#include "arithmetics.h"

#include <strings.h>

namespace soclib { namespace common {

/* PRID_COMP[23:16] = 0x00 (legacy - Not a "legal" MIPS32/MIPS64 implementation) */
/* PRID_IMP[15:8]   = 0x70 (TSAR - seems to available) */
/* PRID_REV[7:0]    = 0x00 */
#define MIPS32_CPUID 0x00007000

#define COPROC_REGNUM(no, sel) (((no)<<3)+sel)

enum Cp0Reg {
    INDEX = COPROC_REGNUM(0,0),
    TCCONTEXT = COPROC_REGNUM(2,5),
    USERLOCAL = COPROC_REGNUM(4,2), // r_tls_base
    HWRENA = COPROC_REGNUM(7,0),
    BAR = COPROC_REGNUM(8,0),
    COUNT = COPROC_REGNUM(9,0),
    COMPARE = COPROC_REGNUM(11,0),
    STATUS = COPROC_REGNUM(12,0),
    INTCTL = COPROC_REGNUM(12,1),
    CAUSE = COPROC_REGNUM(13,0),
    EPC = COPROC_REGNUM(14,0),
    CPUID = COPROC_REGNUM(15,0),
    EBASE = COPROC_REGNUM(15,1),
    CONFIG = COPROC_REGNUM(16,0),
    CONFIG_1 = COPROC_REGNUM(16,1),
    CONFIG_2 = COPROC_REGNUM(16,2),
    CONFIG_3 = COPROC_REGNUM(16,3),
    DEBUG = COPROC_REGNUM(23,0),
    ERROR_EPC = COPROC_REGNUM(30,0),

    // Implementation dependant,
    // count of instructions
    INS_CYCLES = COPROC_REGNUM(9,6),
    // count of pipeline cycles
    PIPE_CYCLES = COPROC_REGNUM(9,7),
    // scheduler (virtual) address
    SCHED_ADDR = COPROC_REGNUM(22,0),
};

static inline uint32_t merge(uint32_t oldval, uint32_t newval, uint32_t newmask)
{
    return (oldval & ~newmask) | (newval & newmask);
}

uint32_t Mips32Iss::cp0Get( uint32_t reg, uint32_t sel ) const
{
    switch(COPROC_REGNUM(reg,sel)) {
    case INDEX:
        return m_ident;
    case TCCONTEXT:
        return r_tc_context;
    case HWRENA:
        return r_hwrena;
    case USERLOCAL:
        return r_tls_base;
    case BAR:
        return r_bar;
    case COUNT:
        return r_cycle_count;
    case COMPARE:
        return r_compare;
    case STATUS:
        return r_status.whole;
    case INTCTL:
        return r_intctl.whole;
    case CAUSE:
        return r_cause.whole;
    case EPC:
        return r_epc;
    case CPUID:
        return MIPS32_CPUID;
    case EBASE:
        return r_ebase.whole;
    case INS_CYCLES:
        return m_instruction_count;
    case PIPE_CYCLES:
        return m_pipeline_use_count;
    case CONFIG:
        return r_config.whole;
    case CONFIG_1:
        return r_config1.whole;
    case CONFIG_2:
        return r_config2.whole;
    case CONFIG_3:
        return r_config3.whole;
    case DEBUG:
        return m_debug_mask;
    case ERROR_EPC:
        return r_error_epc;
    case SCHED_ADDR:
        return r_scheduler_addr;
    default:
        return 0;
    }
}

#define EBASE_WRITE_MASK 0xfffff000
#define INTCTL_WRITE_MASK 0xfc0
#define CAUSE_WRITE_MASK 0x8c00300

void Mips32Iss::cp0Set( uint32_t reg, uint32_t sel, uint32_t val )
{
    switch(COPROC_REGNUM(reg, sel)) {
    case TCCONTEXT:
        r_tc_context = val;
	break;
    case COMPARE:
        r_compare = val;
        break;
    case COUNT:
        r_cycle_count = val;
        break;
    case USERLOCAL:
        r_tls_base = val;
        break;
    case HWRENA:
        r_hwrena = val;
        break;
    case STATUS:
        r_status.whole = val;
        update_mode();
        return;
    case EBASE:
        r_ebase.whole = merge(r_ebase.whole, val, EBASE_WRITE_MASK);
        break;
    case INTCTL:
        r_intctl.whole = merge(r_intctl.whole, val, INTCTL_WRITE_MASK);
        break;
    case EPC:
        r_epc = val;
        break;
    case CAUSE:
        r_cause.whole = merge(r_cause.whole, val, CAUSE_WRITE_MASK);
        break;
    case DEBUG:
        m_debug_mask = val;
    case ERROR_EPC:
        r_error_epc = val;
        break;
    case SCHED_ADDR:
        r_scheduler_addr = val;
        break;
    default:
        return;
    }
}

bool Mips32Iss::isCopAccessible(int cp) const
{
    switch (cp) {
    case 0:
        if ( r_cpu_mode == MIPS32_KERNEL )
            return true;
        return r_status.cu0;
    case 1:
        return r_status.cu1;
    case 2:
        if ( r_cpu_mode == MIPS32_KERNEL )
            return true;
        return r_status.cu2;
    case 3:
        return r_status.cu3;
    }
    return false;
}

void Mips32Iss::update_mode()
{
    if ( r_status.exl || r_status.erl ) {
		r_bus_mode = MODE_KERNEL;
		r_cpu_mode = MIPS32_KERNEL;
        return;
    }
        
	switch (r_status.ksu) {
	case MIPS32_KSU_KERNEL:
		r_bus_mode = MODE_KERNEL;
		r_cpu_mode = MIPS32_KERNEL;
		break;
	case MIPS32_KSU_SUPERVISOR:
		r_bus_mode = MODE_HYPER;
		r_cpu_mode = MIPS32_SUPERVISOR;
		break;
	case MIPS32_KSU_USER:
		r_bus_mode = MODE_USER;
		r_cpu_mode = MIPS32_USER;
		break;
    default:
		assert(0&&"Invalid user mode set in status register");
	}
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

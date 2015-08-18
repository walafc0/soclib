/* -*- c++ -*-
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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007-2009
 *
 * Maintainers: nipo
 *
 * $Id: ppc405_special.cpp 1101 2009-05-12 12:16:30Z nipo $
 */

#include <stdint.h>
#include "base_module.h"
#include "ppc405.h"
#include "soclib_endian.h"
#include "arithmetics.h"

namespace soclib { namespace common {

uint32_t Ppc405Iss::sprfGet( enum Sprf sprf )
{
    if ( r_msr.pr && sprf&SPR_PRIV_MASK ) {
        m_exception = EXCEPT_PROGRAM;
        r_esr = ESR_PPR;
        return 0;
    }
    switch (sprf) {
    case SPR_XER:
        return r_xer.whole;
    case SPR_LR:
        return r_lr;
    case SPR_CTR:
        return r_ctr;
    case SPR_SRR0:
    case SPR_SRR1:
        return r_srr[PPC_SPLIT_FIELD(sprf) - 0x01a /*SPR_SRR0*/];
    case SPR_USPRG0:
    case SPR_SPRG4_RO:
    case SPR_SPRG5_RO:
    case SPR_SPRG6_RO:
    case SPR_SPRG7_RO:
        return r_sprg[PPC_SPLIT_FIELD(sprf) - 0x100 /*SPR_USPRG0*/];
    case SPR_SPRG0_RW:
    case SPR_SPRG1_RW:
    case SPR_SPRG2_RW:
    case SPR_SPRG3_RW:
    case SPR_SPRG4_RW:
    case SPR_SPRG5_RW:
    case SPR_SPRG6_RW:
    case SPR_SPRG7_RW:
        return r_sprg[PPC_SPLIT_FIELD(sprf) - 0x110 /*SPR_SPRG0_RW*/];
    case SPR_TBL:
        return r_tb;
    case SPR_TBU:
        return r_tb>>32;
    case SPR_PVR:
        return pvr;
    case SPR_ESR:
        return r_esr;
    case SPR_DEAR:
        return r_dear;
    case SPR_EVPR:
        return r_evpr;
    case SPR_SRR2:
    case SPR_SRR3:
        return r_srr[PPC_SPLIT_FIELD(sprf) - 0x3de + 2 /*SPR_SRR2+2*/];
    default:
        m_exception = EXCEPT_PROGRAM;
        r_esr = ESR_PEU;
        return 0;
    }
}

void Ppc405Iss::sprfSet( enum Sprf sprf, uint32_t val )
{
    if ( r_msr.pr && sprf&SPR_PRIV_MASK ) {
        m_exception = EXCEPT_PROGRAM;
        r_esr = ESR_PPR;
        return;
    }
    switch (sprf) {
    case SPR_XER:
        r_xer.whole = val;
        break;
    case SPR_LR:
        r_lr = val;
        break;
    case SPR_CTR:
        r_ctr = val;
        break;
    case SPR_SRR0:
    case SPR_SRR1:
        r_srr[PPC_SPLIT_FIELD(sprf) - 0x1a /*SPR_SRR0*/] = val;
        break;
    case SPR_SPRG0_RW:
    case SPR_SPRG1_RW:
    case SPR_SPRG2_RW:
    case SPR_SPRG3_RW:
    case SPR_SPRG4_RW:
    case SPR_SPRG5_RW:
    case SPR_SPRG6_RW:
    case SPR_SPRG7_RW:
        r_sprg[PPC_SPLIT_FIELD(sprf) - 0x110 /*SPR_SPRG0_RW*/] = val;
        break;
    case SPR_TBL:
        r_tb = (r_tb & 0xffffffff00000000LL) | val;
        break;
    case SPR_TBU:
        r_tb = (r_tb & 0xffffffff) | ((uint64_t)val<<32);
        break;
    case SPR_ESR:
        r_esr = val;
        break;
    case SPR_DEAR:
        r_dear = val;
        break;
    case SPR_EVPR:
        r_evpr = val & 0xffff0000;
        break;
    case SPR_SRR2:
    case SPR_SRR3:
        r_srr[PPC_SPLIT_FIELD(sprf) - 0x3de + 2 /*SPR_SRR2+2*/] = val;
        break;
    default:
        m_exception = EXCEPT_PROGRAM;
        r_esr = ESR_PEU;
    }
}

const uint32_t Ppc405Iss::except_addresses[] = {
    /* EXCEPT_NONE                 */ 0x0,
    /* EXCEPT_CRITICAL             */ 0x0100,
    /* EXCEPT_WATCHDOG             */ 0x1020,
    /* EXCEPT_DEBUG                */ 0x2000,
    /* EXCEPT_MACHINE_CHECK        */ 0x0200,
    /* EXCEPT_INSTRUCTION_STORAGE  */ 0x0400,
    /* EXCEPT_PROGRAM              */ 0x0700,
    /* EXCEPT_DATA_STORAGE         */ 0x0300,
    /* EXCEPT_DATA_TLB_MISS        */ 0x1100,
    /* EXCEPT_ALIGNMENT            */ 0x0600,
    /* EXCEPT_EXTERNAL             */ 0x0500,
    /* EXCEPT_SYSCALL              */ 0x0c00,
    /* EXCEPT_PI_TIMER             */ 0x1000,
    /* EXCEPT_FI_TIMER             */ 0x1010,
    /* EXCEPT_INSTRUCTION_TLB_MISS */ 0x1200,
};

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

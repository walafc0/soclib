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

void Mips32Iss::op_special2()
{
    enum {
        MADD = 0,
        MADDU = 1,
        MUL = 2,
        MSUB = 4,
        MSUBU = 5,
        CLZ = 0x20,
        CLO = 0x21,
    };

    switch ( m_ins.r.func ) {
    case MUL:
        r_gp[m_ins.r.rd] = r_gp[m_ins.i.rs]*r_gp[m_ins.i.rt];
        if (r_gp[m_ins.i.rt])
            setInsDelay( 3 );
        break;
    case MSUB: {
        int64_t tmp = ((int64_t)r_hi)<<32 | (int64_t)r_lo;
        tmp -= (int64_t)r_gp[m_ins.i.rs]*(int64_t)r_gp[m_ins.i.rt];
        r_hi = tmp>>32;
        r_lo = tmp;
        if (r_gp[m_ins.i.rt])
            setInsDelay( 6 );
        break;
    }
    case MSUBU: {
        uint64_t tmp = ((uint64_t)r_hi)<<32 | (uint64_t)r_lo;
        tmp -= (uint64_t)r_gp[m_ins.i.rs]*(uint64_t)r_gp[m_ins.i.rt];
        r_hi = tmp>>32;
        r_lo = tmp;
        if (r_gp[m_ins.i.rt])
            setInsDelay( 6 );
        break;
    }
    case MADD: {
        int64_t tmp = ((int64_t)r_hi)<<32 | (int64_t)r_lo;
        tmp += (int64_t)r_gp[m_ins.i.rs]*(int64_t)r_gp[m_ins.i.rt];
        r_hi = tmp>>32;
        r_lo = tmp;
        if (r_gp[m_ins.i.rt])
            setInsDelay( 6 );
        break;
    }
    case MADDU: {
        uint64_t tmp = ((uint64_t)r_hi)<<32 | (uint64_t)r_lo;
        tmp += (uint64_t)r_gp[m_ins.i.rs]*(uint64_t)r_gp[m_ins.i.rt];
        r_hi = tmp>>32;
        r_lo = tmp;
        if (r_gp[m_ins.i.rt])
            setInsDelay( 6 );
        break;
    }
    case CLO:
        if ( m_ins.r.rt != m_ins.r.rd ) {
            // Unpredictable result, as in spec
            m_exception = X_TR;
            break;
        }
        if ( r_gp[m_ins.r.rs] )
            r_gp[m_ins.r.rt] = soclib::common::clo(r_gp[m_ins.r.rs]);
        else
            r_gp[m_ins.r.rt] = 32;
        break;
    case CLZ:
        if ( m_ins.r.rt != m_ins.r.rd ) {
            // Unpredictable result, as in spec
            m_exception = X_TR;
            break;
        }
        if ( r_gp[m_ins.r.rs] )
            r_gp[m_ins.r.rt] = soclib::common::clz(r_gp[m_ins.r.rs]);
        else
            r_gp[m_ins.r.rt] = 32;
        break;
    default:
        op_ill();
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

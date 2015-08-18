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
#include "soclib_endian.h"

namespace soclib { namespace common {

void Mips32Iss::op_special3()
{
    enum {
        EXT = 0,
		INS = 0x4,
		BSHFL = 0x20,
		RDHWR = 0x3b,
    };

	enum {
		SEB = 0x10,
		SEH = 0x18,
		WSBH = 0x2,
	};

    enum {
        RDHWR_CPUNUM = 0,
        RDHWR_CC = 2,
        RDHWR_CCRES = 3,
        RDHWR_TLS = 29,
    };

    switch ( m_ins.r.func ) {
    case EXT: {
		size_t size = m_ins.r.rd + 1;
		size_t lsb = m_ins.r.sh;
        r_gp[m_ins.r.rt] = extract_bits(r_gp[m_ins.r.rs], lsb, size);
        break;
    }
    case RDHWR:
        if ( r_cpu_mode == MIPS32_USER &&
             ! ( r_hwrena & (1<<m_ins.r.rd) ) ) {
            m_exception = X_RI;
            break;
        }
        switch (m_ins.r.rd) {
        case RDHWR_CPUNUM:
            r_gp[m_ins.r.rt] = m_ident;
            break;
        case RDHWR_CC:
            r_gp[m_ins.r.rt] = r_cycle_count;
            break;
        case RDHWR_CCRES:
            r_gp[m_ins.r.rt] = 1;
            break;
        case RDHWR_TLS:
            r_gp[m_ins.r.rt] = r_tls_base;
            break;
        default:
            m_exception = X_RI;
            break;
        }
        break;
    case INS: {
		size_t lsb = m_ins.r.sh;
		size_t msb = m_ins.r.rd;
        r_gp[m_ins.r.rt] = insert_bits(r_gp[m_ins.r.rt], r_gp[m_ins.r.rs], lsb, msb-lsb+1);
        break;
	}
	case BSHFL: {
		switch ( m_ins.r.sh ) {
		case SEB:
			r_gp[m_ins.r.rd] = sign_ext(r_gp[m_ins.r.rt], 8);
			break;
		case SEH:
			r_gp[m_ins.r.rd] = sign_ext(r_gp[m_ins.r.rt], 16);
			break;
		case WSBH: {
			r_gp[m_ins.r.rd] = soclib::endian::uint32_swap16(r_gp[m_ins.r.rt]);
			break;
		}
        default:
            op_ill();
		}
        break;
	}
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

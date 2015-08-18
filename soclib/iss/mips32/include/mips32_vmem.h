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
 *         Alexandre Becoulet <alexandre.becoulet@lip6.fr>
 *
 * Maintainers: becoulet
 *
 * $Id$
 *
 * History:
 * - 2010-2-19
 *   Alexandre becoulet: Model created
 */

#ifndef _SOCLIB_MIPS32VMEM_ISS_H_
#define _SOCLIB_MIPS32VMEM_ISS_H_

#include "mips32.h"

namespace soclib { namespace common {

enum MipsVmemModel {
    MIPS_VMEM_4km,
    MIPS_VMEM_4kp,
    MIPS_VMEM_4kc,
    MIPS_VMEM_4ke,
    MIPS_VMEM_4ks,
};

template <enum MipsVmemModel vmodel>
class Mips32VmemIss
    : public Mips32Iss
{
public:

    Mips32VmemIss(const std::string &name, uint32_t ident, bool little)
        : Mips32Iss(name, ident, little)
    {
    }

private:
    bool translate_tlb(Iss2::addr_t &addr) const
    {
        switch (vmodel)
            {
            case MIPS_VMEM_4kp:
            case MIPS_VMEM_4km:         /* use fixed mapping */
                if (!(addr & 0x80000000) && !r_status.erl)
                    addr += 0x40000000;
                break;

            default:
                // TLB not implemented yet
                abort();
            }
        return true;
    }

public:
    bool virtualToPhys(Iss2::addr_t &addr) const
    {
        bool user = (r_cpu_mode == MIPS32_USER) && !r_status.erl && !r_status.exl;

        if (user && (addr & 0x80000000))
            return false;

        switch (addr >> 29)
            {
            case 4:		/* kseg0, cached unmapped */
            case 5:		/* kseg1, uncached unmapped */
                addr &= 0x1fffffff;
                return true;

            case 6:		/* kseg2, mapped */
            case 7:		/* kseg3, mapped */
                return translate_tlb(addr);

            default:	/* user, 0x00000000 to 0x7fffffff */
                return translate_tlb(addr);
            }
    }

};

typedef Mips32EndianIss<Iss2::ISS_LITTLE_ENDIAN, Mips32VmemIss<MIPS_VMEM_4kp> > Mips32El4kpIss;
typedef Mips32EndianIss<Iss2::ISS_LITTLE_ENDIAN, Mips32VmemIss<MIPS_VMEM_4km> > Mips32El4kmIss;
typedef Mips32EndianIss<Iss2::ISS_BIG_ENDIAN, Mips32VmemIss<MIPS_VMEM_4kp> > Mips32Eb4kpIss;
typedef Mips32EndianIss<Iss2::ISS_BIG_ENDIAN, Mips32VmemIss<MIPS_VMEM_4km> > Mips32Eb4kmIss;

}}

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

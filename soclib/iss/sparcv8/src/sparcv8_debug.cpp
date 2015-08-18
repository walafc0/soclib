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
 * Copyright (c) Telecom ParisTech
 *         Alexis Polti <polti@telecom-paristech.fr>
 *
 * Maintainers: Alexis Polti
 *
 * $Id$
 */

#include "sparcv8.h"
#include "soclib_endian.h"
#include <cstring>
#include <iomanip>

namespace soclib { namespace common {

#define tmpl(x) template<unsigned int NWINDOWS> x Sparcv8Iss<NWINDOWS>

tmpl(void)::dump_pc(const std::string &msg) const
{
    std::cout
        << msg << std::endl
        << std::hex << std::showbase
        << "\tPC=" << std::setw(10) << std::setfill('0') << std::internal << std::hex << std::showbase << r_pc
        << " / NPC=" << std::setw(10) << std::setfill('0') << std::internal << std::hex << std::showbase << r_npc 
        << " / next_pc=" << std::setw(10) << std::setfill('0') << std::internal << std::hex << std::showbase << m_next_pc << std::endl
        << "\tIns=" << std::setw(10) << std::setfill('0') << std::internal << std::hex << std::showbase << m_ins.ins 
        << " (" << get_ins_name() << ")" << std::endl
        ;
}


tmpl(void)::dump_regs(const std::string &msg) const
{
#define DUMP_REG(n, prefix, val)                                            \
    std::cout <<  prefix << std::setw(1) << std::dec << n << "="            \
              << "0x" << std::setw(8) << std::setfill('0') << std::internal \
              << std::hex << std::noshowbase << val << " ";
    
    std::cout 
        << msg << std::endl
        << "\tPSR=" << std::setw(10) << std::setfill('0') << std::internal << std::hex << std::showbase << r_psr.whole
        << " (N=" << r_psr.n 
        << " Z=" << r_psr.z
        << " V=" << r_psr.v 
        << " C=" << r_psr.c 
        << " EC=" << r_psr.ec
        << " EF=" << r_psr.ef 
        << " PIL=" << r_psr.pil 
        << " PS=" << r_psr.ps
        << " ET=" << r_psr.et
        << " CWP="<<r_psr.cwp 
        << ")" << std::endl
        << "\tWIM=" << r_wim << std::endl;

    std::cout << "\t";
    for ( size_t i=0; i<8; ++i ) 
        DUMP_REG(i, "g", GPR(i));
    std::cout << std::endl;
    
    std::cout << "\t";
    for ( size_t i=0; i<8; ++i ) 
        DUMP_REG(i, "o", GPR(i+8));
    std::cout << std::endl;
    
    std::cout << "\t";
    for ( size_t i=0; i<8; ++i ) 
        DUMP_REG(i, "l", GPR(i+16));
    std::cout << std::endl;
    
    std::cout << "\t";
    for ( size_t i=0; i<8; ++i ) 
        DUMP_REG(i, "i", GPR(i+24));
    std::cout << std::endl;

#if FPU
    std::cout << std::endl << "\t";
    for ( size_t i=0; i<32; ++i ) {
        DUMP_REG(i, "ff", r_f[i]);
        if (i%8 == 7)
            std::cout << std::endl << "\t";
    }
    std::cout << std::endl;
#endif
    
#undef DUMP_REG
    
}



tmpl(Iss2::debug_register_t)::debugGetRegisterValue(unsigned int reg) const
{
    switch (reg)
        {
        case 0 ... 31:
            return GPR(reg);
        case 32 ... 63:
            return r_f[reg - 32];
        case 64:
            return r_y;
        case 65:
            return r_psr.whole;
        case 66:
            return r_wim;
        case 67:
            return r_tbr.whole;
        case 68:
            return r_pc;
        case 69:
            return r_npc;
        case 70:
            return r_fsr.whole;
        case 71:
            return 0; /* no csr */

        case ISS_DEBUG_REG_IS_USERMODE:
            return r_psr.s == 0;
        case ISS_DEBUG_REG_IS_INTERRUPTIBLE:
            return r_psr.et && r_psr.pil < 15;
        case ISS_DEBUG_REG_STACK_REDZONE_SIZE:
        default:
            return 0;
        }
}

tmpl(void)::debugSetRegisterValue(unsigned int reg, debug_register_t value)
{
    switch (reg)
        {
        case 0 ... 31:
            GPR(reg) = value;
            break;
        case 32 ... 63:
            r_f[reg - 32] = value;
            break;
        case 64:
            r_y = value;
            break;
        case 65:
            r_psr.whole = value;
            break;
        case 66:
            r_wim = value;
            break;
        case 67:
            r_tbr.whole = value;
            break;
        case 68:
            r_pc = value;
            r_npc = value + 4;
            break;
        case 69:
            r_npc = value;
            break;
        case 70:
            r_fsr.whole = value;
            break;
        case 71:
            /* no csr */
            break;
        }
}

tmpl(unsigned int)::debugGetRegisterCount() const
{
    return 32 + 32 + 8;
}

tmpl(size_t)::debugGetRegisterSize(unsigned int reg) const
{
    return 32;
}

}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


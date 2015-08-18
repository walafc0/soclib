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

#include "mips32.h"
#include "base_module.h"
#include "arithmetics.h"

#include <strings.h>

namespace soclib { namespace common {

#define use(x) Mips32Iss::USE_##x
#define use4(x, y, z, t) use(x), use(y), use(z), use(t)

Mips32Iss::use_t const Mips32Iss::use_table[]= {
       use4(SPECIAL,    ST, NONE,  NONE),
       use4(     ST,    ST,    S,     S),

       use4(      S,     S,    S,     S),
       use4(      S,     S,    S,  NONE),

       use4(     ST,  NONE, NONE,  NONE),
       use4(   NONE,  NONE, NONE,  NONE),

       use4(   NONE,  NONE, NONE,  NONE),
       use4(     ST,  NONE, NONE,  NONE),

       use4(      S,     S,    S,     S),
       use4(      S,     S,    S,  NONE),

       use4(     ST,    ST,   ST,    ST),
       use4(   NONE,  NONE,   ST,    ST),

       use4(      S,  NONE, NONE,  NONE),
       use4(   NONE,  NONE, NONE,  NONE),

       use4(     ST,  NONE, NONE,  NONE),
       use4(   NONE,  NONE, NONE,  NONE),
};

Mips32Iss::use_t const Mips32Iss::use_special_table[] = {
        use4(    T, NONE,    T,    T),
        use4(    T, NONE,    T,    T),

        use4(    S,    S, NONE, NONE),
        use4( NONE, NONE, NONE, NONE),

        use4( NONE,    S, NONE,    S),
        use4( NONE, NONE, NONE, NONE),

        use4(   ST,   ST,   ST,   ST),
        use4( NONE, NONE, NONE, NONE),

        use4(   ST,   ST,   ST,   ST),
        use4(   ST,   ST,   ST,   ST),

        use4( NONE, NONE,   ST,   ST),
        use4( NONE, NONE, NONE, NONE),

        use4( NONE, NONE, NONE, NONE),
        use4( NONE, NONE, NONE, NONE),

        use4( NONE, NONE, NONE, NONE),
        use4( NONE, NONE, NONE, NONE),
};

Mips32Iss::use_t Mips32Iss::curInstructionUsesRegs()
{
    use_t use = use_table[m_ins.i.op];
    if ( use == USE_SPECIAL )
        return use_special_table[m_ins.r.func];
    return use;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

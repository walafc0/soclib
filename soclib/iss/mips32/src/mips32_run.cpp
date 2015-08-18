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

#define OPS                                                            \
    op4(special, bcond,    j,   jal),                                  \
    op4(    beq,   bne, blez,  bgtz),                                  \
                                                                       \
    op4(   addi, addiu, slti, sltiu),                                  \
    op4(   andi,   ori, xori,   lui),                                  \
                                                                       \
    op4(   cop0,  cop1, cop2,   ill),                                  \
    op4(   beql,  bnel,blezl, bgtzl),                                  \
                                                                       \
    op4(    ill,   ill,  ill,   ill),                                  \
    op4(special2,  ill,  ill,special3),                                \
                                                                       \
    op4(     lb,    lh,  lwl,    lw),                                  \
    op4(    lbu,   lhu,  lwr,   ill),                                  \
                                                                       \
    op4(     sb,    sh,  swl,    sw),                                  \
    op4(    ill,   ill,  swr, cache),                                  \
                                                                       \
    op4(     ll,  lwc1,  ill,  pref),                                  \
    op4(    ill,  ldc1,  ill,   ill),                                  \
                                                                       \
    op4(     sc,  swc1,  ill,   ill),                                  \
    op4(    ill,  sdc1,  ill,   ill),

#define op(x) &Mips32Iss::op_##x
#define op4(x, y, z, t) op(x), op(y), op(z), op(t)

Mips32Iss::func_t const Mips32Iss::opcod_table[]= {
    OPS
};

#undef op
#define op(x) #x

const char *Mips32Iss::name_table[] = {
    OPS
};
#undef op
#undef op4

void Mips32Iss::run()
{
    func_t func = opcod_table[m_ins.i.op];

    if (isHighPC() && !isPriviliged()) {
        m_exception = X_ADEL;
        
        return;
    }

    m_instruction_count++;
    (this->*func)();
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

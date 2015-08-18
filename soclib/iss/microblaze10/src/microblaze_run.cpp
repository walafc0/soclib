/*\
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
 * 01/10/2011
 * Fork from the mips32 to microblaze
 * Most code from Nicolas Pouillon
 *   Copyright (c) UPMC, Lip6
 *      Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Frédéric Pétrot <Frederic.Petrot@imag.fr>, TIMA Lab, CNRS/Grenoble-INP/UJF
\*/

#include "microblaze.h"
#include "microblaze.hpp"
#include "base_module.h"
#include "arithmetics.h"

#include <strings.h>

namespace soclib { namespace common {

#define OPS                                                            \
   op4(  add,   rsub,   addc,   rsubc),                               \
   op4( addk,   icmp,  addkc,  rsubkc),                               \
                                                                      \
   op4( addi,  rsubi,  addic,  rsubic),                               \
   op4(addik, rsubik, addikc, rsubikc),                               \
                                                                      \
   op4( mult,     bs,    div,    fsld),                               \
   op4(  ill,    ill,  float,     ill),                               \
                                                                      \
   op4( muli,    bsi,    ill,     fsl),                               \
   op4(  ill,    ill,    ill,     ill),                               \
                                                                      \
   op4( orpc,    and,  xorpc,   andpc),                               \
   op4( misc,    msr,     ub,      cb),                               \
                                                                      \
   op4(  ori,   andi,   xori,   andni),                               \
   op4(  imm, return,    ubi,     cbi),                               \
                                                                      \
   op4(  lbu,    lhu,   load,     ill),                               \
   op4(   sb,     sh,  store,     ill),                               \
                                                                      \
   op4( lbui,   lhui,    lwi,     ill),                               \
   op4(  sbi,    shi,    swi,     ill),

MicroblazeIss::func_t const MicroblazeIss::opcod_table[]= {
   OPS
};

#undef op
#define op(x) #x

const char *MicroblazeIss::name_table[] = {
   OPS
};

void MicroblazeIss::run()
{
   func_t func = opcod_table[m_ins.typeA.op];

   /* Should check zpr and tlb entries if MMU is enabled, to produce
    * a m_exception = X_U;
    */
   (this->*func)();
   /* Each instruction must reset this flag but imm whose opcode is
    * 0x2C that must set it!
    */
   m_imm = m_ins.typeB.op == 0x2C;
}

}}

// vim: filetype=cpp:expandtab:shiftwidth=3:tabstop=3:softtabstop=3

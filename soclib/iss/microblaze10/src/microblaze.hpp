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
 * Frédéric Pétrot <Frederic.Petrot@imag.fr>, TIMA Lab, CNRS/Grenoble-INP/UJF
\*/


#ifndef MICROBLAZE_HPP
#define MICROBLAZE_HPP

#include "microblaze.h"
#include "arithmetics.h"

/*\
 * Instruction decoding helper borrowed from the mips32 example
\*/
#define op(x) &MicroblazeIss::insn_##x
#define op4(x, y, z, t) op(x), op(y), op(z), op(t)

/*\
 * Immediat operand computation
\*/
#define imm_op \
   ((data_t)(m_imm ? (r_imm | m_ins.typeB.imm) \
                   : sign_ext(m_ins.typeB.imm, 16)))

/*\
 * Machine State Register bits
\*/
#define MSR_BE        0x1
#define MSR_IE        0x2
// Quite strange but so defined in the doc !
#define MSR_C         0x80000004
#define MSR_BIP       0x8
#define MSR_DZ        0x40
#define MSR_EIP       0x200
#define MSR_EE        0x100

namespace soclib { namespace common {

void MicroblazeIss::jump(addr_t dest, bool delay_slot)
{
   if (dest & 0x3) {     /* Is destination word aligned? */
      m_exception = X_U;
      m_error_addr = dest;
   } else if (!delay_slot) {
      m_next_pc = dest;
      m_jump_pc = m_next_pc + 4;
   } else {
      m_jump_pc = dest;
      /* Branch target address used by handler when returning from
       * an exception caused by an instruction in a delay slot.
       */
      r_btr = m_jump_pc = dest;
   }
}

bool MicroblazeIss::check_irq_state() const
{
   return r_msr.ie;
}

}}

#ifdef SOCLIB_MODULE_DEBUG
#define DASM_HEADER \
   std::cout << "@" << std::hex << r_pc << ": " << &__func__[5] << std::dec
#define DASM_TYPEA DASM_HEADER \
   << " r" << m_ins.typeA.rd \
   << ", r" << m_ins.typeA.ra \
   << ", r" << m_ins.typeA.rb << std::endl
#define DASM_TYPEA_B DASM_HEADER \
   << " r" << m_ins.typeA.rb << std::endl
#define DASM_TYPEA_BC DASM_HEADER \
   << " r" << m_ins.typeA.ra \
   << ", r" << m_ins.typeA.rb << std::endl
#define DASM_TYPEA_BL DASM_HEADER \
   << " r" << m_ins.typeA.rd \
   << ", r" << m_ins.typeA.rb << std::endl

#define DASM_TYPEB DASM_HEADER \
   << " r" << m_ins.typeB.rd \
   << ", r" << m_ins.typeB.ra \
   << ", " << m_ins.typeB.imm << std::endl
#define DASM_TYPEB_B DASM_HEADER \
   << " " << m_ins.typeB.imm << std::endl
#define DASM_TYPEB_BC DASM_HEADER \
   << " r" << m_ins.typeB.ra \
   << ", " << m_ins.typeB.imm << std::endl
#define DASM_TYPEB_BL DASM_HEADER \
   << " r" << m_ins.typeB.rd \
   << ", " << m_ins.typeB.imm << std::endl
#define DASM_TYPEB_IMM DASM_HEADER \
   << " " << m_ins.typeB.imm << std::endl
#define DASM_TYPEB_WI DASM_HEADER \
   << " r" << m_ins.typeB.rd \
   << ", r" << m_ins.typeB.ra << std::endl

#define DASM_TYPEMFS DASM_HEADER \
   << " r" << m_ins.typeB.rd \
   << ", " << (m_ins.typeB.imm & 0x3FFF) << std::endl

#define DASM_TYPEMTS DASM_HEADER \
   << " " << (m_ins.typeB.imm & 0x3FFF) \
   << ", r" << std::dec << m_ins.typeB.ra << std::endl

#else
#define DASM_TYPEA
#define DASM_TYPEA_B
#define DASM_TYPEA_BC
#define DASM_TYPEA_BL
#define DASM_TYPEA_WRB

#define DASM_TYPEB
#define DASM_TYPEB_B
#define DASM_TYPEB_BC
#define DASM_TYPEB_BL
#define DASM_TYPEB_IMM
#define DASM_TYPEB_WI

#define DASM_TYPEMFS
#define DASM_TYPEMTS
#endif

#endif

// vim: filetype=cpp:expandtab:shiftwidth=3:tabstop=3:softtabstop=3

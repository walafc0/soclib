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
 * Frédéric Pétrot <Frederic.Petrot@imag.fr>, TIMA Lab, CNRS/Grenoble-INP/UJF
\*/
 
#include "microblaze.h"
#include "microblaze.hpp"
#include "base_module.h"
#include <strings.h>

namespace soclib { namespace common {

void MicroblazeIss::insn_unimpl()
{
   std::cerr
      << "Instruction not yet implemented in simulator: "
      << std::hex
      << m_ins.ins
      << " at pc "
      << r_pc
      << std::endl;
   exit(1);
}

void MicroblazeIss::insn_ill()
{
   /* FIXME: good for now as a trial, to be fixed asap */
   std::cerr << "Illegal Instruction Exception "
      << std::hex
      << m_ins.ins
      << " at pc "
      << r_pc
      << std::endl;
   exit(1);
}

/*\
 * Some decoding stuff to make things more regular, even though
 * perhaps not speed optimal
\*/
void MicroblazeIss::insn_andpc()
{
   enum {ANDN = 0,
         PCMPNE = 0x400};

   switch (m_ins.typeA.sh) {
   case ANDN:
      insn_andn();
      break;
   case PCMPNE:
      insn_pcmpne();
      break;
   default:
      insn_ill();
   }
}

void MicroblazeIss::insn_orpc()
{
   enum {OR = 0,
         PCMPBF = 0x400};

   switch (m_ins.typeA.sh) {
   case OR:
      insn_or();
      break;
   case PCMPBF:
      insn_pcmpbf();
      break;
   default:
      insn_ill();
   }
}

void MicroblazeIss::insn_xorpc()
{
   enum {XOR = 0,
         PCMPEQ = 0x400};

   switch (m_ins.typeA.sh) {
   case XOR:
      insn_xor();
      break;
   case PCMPEQ:
      insn_pcmpeq();
      break;
   default:
      insn_ill();
   }
}

/*\
 * Arithmetic and logic instructions. 
 * Listed in the order of the MicroBlaze Processor Reference Guide UG081 (v10.0), EDK 11.1
\*/

void MicroblazeIss::insn_add()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb];
   r_msr.c = (r_gp[m_ins.typeA.rd] < r_gp[m_ins.typeA.ra]);
} 

void MicroblazeIss::insn_addc()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb] + r_msr.c;
   r_msr.c = (r_gp[m_ins.typeA.rd] < r_gp[m_ins.typeA.ra]);
            
}

void MicroblazeIss::insn_addk()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb];
}

void MicroblazeIss::insn_addkc()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb] + r_msr.c;
}

void MicroblazeIss::insn_addi()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = r_gp[m_ins.typeB.ra] + imm_op;
   r_msr.c = (r_gp[m_ins.typeB.rd] < r_gp[m_ins.typeB.ra]);
}

void MicroblazeIss::insn_addic()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = r_gp[m_ins.typeB.ra] + imm_op + r_msr.c;
   r_msr.c = (r_gp[m_ins.typeB.rd] < r_gp[m_ins.typeB.ra]);
}

void MicroblazeIss::insn_addik()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = r_gp[m_ins.typeB.ra] + imm_op;
}  

void MicroblazeIss::insn_addikc()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = r_gp[m_ins.typeB.ra]+ imm_op + r_msr.c;
}

void MicroblazeIss::insn_and()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd]=r_gp[m_ins.typeA.ra] & r_gp[m_ins.typeA.rb];
}

void MicroblazeIss::insn_andi()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = r_gp[m_ins.typeB.ra] & imm_op;
}

void MicroblazeIss::insn_andn()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] & ~r_gp[m_ins.typeA.rb];
}

void MicroblazeIss::insn_andni()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = r_gp[m_ins.typeB.ra] & (~imm_op);
}

void MicroblazeIss::insn_imm()
{
   DASM_TYPEB_IMM;
   r_imm = (m_ins.typeB.imm << 16);
}
      
void MicroblazeIss::insn_muli()
{
   if (C_USE_HW_MUL <= 0)
      insn_ill();

   DASM_TYPEB;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeB.ra] * imm_op;

   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(3);
}

void MicroblazeIss::insn_or()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] | r_gp[m_ins.typeA.rb];
}

void MicroblazeIss::insn_ori()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = r_gp[m_ins.typeB.ra] | imm_op;
}

void MicroblazeIss::insn_pcmpbf()
{
   if (C_USE_PCMP_INSTR != 1)
      insn_ill();
   DASM_TYPEA;
   if (~(r_gp[m_ins.typeA.rb] ^ r_gp[m_ins.typeA.ra]) & 0xFF000000)
      r_gp[m_ins.typeA.rd] = 1;
   else if (~(r_gp[m_ins.typeA.rb] ^ r_gp[m_ins.typeA.ra]) & 0x00FF0000)
         r_gp[m_ins.typeA.rd] = 2;
      else if (~(r_gp[m_ins.typeA.rb] ^ r_gp[m_ins.typeA.ra]) & 0x0000FF00)
            r_gp[m_ins.typeA.rd] = 3;
         else if (~(r_gp[m_ins.typeA.rb] ^ r_gp[m_ins.typeA.ra]) & 0x000000FF)
               r_gp[m_ins.typeA.rd] = 4;
            else
               r_gp[m_ins.typeA.rd] = 0;
}

void MicroblazeIss::insn_pcmpeq()
{
   if (C_USE_PCMP_INSTR != 1)
      insn_ill();
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] == r_gp[m_ins.typeA.rb];
}

void MicroblazeIss::insn_pcmpne()
{
   if (C_USE_PCMP_INSTR != 1)
      insn_ill();
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] != r_gp[m_ins.typeA.rb];
}


void MicroblazeIss::insn_rsub()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.rb] - r_gp[m_ins.typeA.ra];
   r_msr.c = r_gp[m_ins.typeA.ra] > r_gp[m_ins.typeA.rb];
}

void MicroblazeIss::insn_rsubc()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] =
      r_gp[m_ins.typeA.rb] + ~r_gp[m_ins.typeA.ra] + r_msr.c;
   /* FIXME: to be checked, as this comes out of my head */
   r_msr.c = r_gp[m_ins.typeA.ra] > (r_gp[m_ins.typeA.rb] + r_msr.c);
}
      

void MicroblazeIss::insn_rsubkc()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] =
      r_gp[m_ins.typeA.rb] + ~r_gp[m_ins.typeA.ra] + r_msr.c;
}

void MicroblazeIss::insn_rsubi()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = imm_op - r_gp[m_ins.typeB.ra];
   r_msr.c = r_gp[m_ins.typeB.ra] > imm_op;
}

void MicroblazeIss::insn_rsubic()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = imm_op + ~r_gp[m_ins.typeB.ra] + r_msr.c;
   r_msr.c = r_gp[m_ins.typeB.ra] > (imm_op + r_msr.c); 
}


void MicroblazeIss::insn_rsubik()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = imm_op - r_gp[m_ins.typeB.ra];
}

void MicroblazeIss::insn_rsubikc()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = imm_op + ~r_gp[m_ins.typeB.ra] + r_msr.c;
}

void MicroblazeIss::insn_rtsd()
{
   DASM_TYPEB;
   /* FIXME: doc says sext(imm) but I would believe imm_op.
    * Applies to all return insns. */
   jump(r_gp[m_ins.typeB.ra] + imm_op, true);
   setInsDelay(2);
}

void MicroblazeIss::insn_rtid()
{
   DASM_TYPEB;
   if (C_USE_MMU >= 1 && r_msr.um == 1) {
      r_esr.typeBASE.ec = 0x07;
      m_exception = X_PI;
   } else {
      r_msr.ie = 1;
      r_msr.um = r_msr.ums;
      r_msr.vm = r_msr.vms;
      jump(r_gp[m_ins.typeB.ra] + imm_op, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_rtbd()
{
   DASM_TYPEB;
   if (C_USE_MMU >= 1 && r_msr.um == 1) {
      r_esr.typeBASE.ec = 0x07;
      m_exception = X_PI;
   } else {
      r_msr.bip = 0;
      r_msr.um = r_msr.ums;
      r_msr.vm = r_msr.vms;
      jump(r_gp[m_ins.typeB.ra] + imm_op, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_rted()
{
   DASM_TYPEB;
   if (C_USE_MMU >= 1 && r_msr.um == 1) {
      r_esr.typeBASE.ec = 0x07;
      m_exception = X_PI;
   } else {
      r_msr.ee = 1;
      r_msr.eip = 0;
      r_msr.um = r_msr.ums;
      r_msr.vm = r_msr.vms;
      r_esr.esr = 0;
      jump(r_gp[m_ins.typeB.ra] + imm_op, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_return()
{
   enum {RTSD = 0x10,
         RTID = 0x11,
         RTBD = 0x12,
         RTED = 0x14};

   switch (m_ins.typeB.rd) {
   case RTSD:
      insn_rtsd();
      break;
   case RTID:
      insn_rtid();
      break;
   case RTBD:
      insn_rtbd();
      break;
   case RTED:
      insn_rted();
      break;
   default:
      insn_ill();
   }
}

void MicroblazeIss::insn_xor()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] ^ r_gp[m_ins.typeA.rb];
}

void MicroblazeIss::insn_xori()
{
   DASM_TYPEB;
   r_gp[m_ins.typeB.rd] = r_gp[m_ins.typeB.ra] ^ imm_op;
}

void MicroblazeIss::insn_fsld()
{
   enum {GETD = 0,
         PUTD = 1};

   switch (m_ins.typeFSLXD.put) {
   case PUTD:
      insn_unimpl();
      break;
   case GETD:
      insn_unimpl();
      break;
   default:
      insn_ill();
   }

}

void MicroblazeIss::insn_fsl()
{
   enum {GET = 0, PUT = 1};

   switch (m_ins.typeFSLX.put) {
   case PUT:
      insn_unimpl();
      break;
   case GET:
      insn_unimpl();
      break;
   default:
      insn_ill();
   }
}

}} // end soclib
// vim: filetype=cpp:expandtab:shiftwidth=3:tabstop=3:softtabstop=3


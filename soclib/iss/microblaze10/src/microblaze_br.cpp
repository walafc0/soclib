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

#include "microblaze.hpp"
#include "base_module.h"
#include "soclib_endian.h"

namespace soclib { namespace common {

/*\
 * Second decoding stage for the conditional branch register instructions
\*/

MicroblazeIss::func_t const MicroblazeIss::cb_table[] = {
   op4(  beq,  bne,  blt,  ble),
   op4(  bgt,  bge,  ill,  ill),

   op4(  ill,  ill,  ill,  ill),
   op4(  ill,  ill,  ill,  ill),

   op4( beqd, bned, bltd, bled),
   op4( bgtd, bged,  ill,  ill),

   op4(  ill,  ill,  ill,  ill),
   op4(  ill,  ill,  ill,  ill),
};

void MicroblazeIss::insn_cb()
{
   func_t func = cb_table[m_ins.typeA.rd];
   r_btr = r_pc - 4;    /* In case the branch is not taken */
   (this->*func)();
}

/*\
 * Second decoding stage for the conditional branch immediat instructions
\*/
MicroblazeIss::func_t const MicroblazeIss::cbi_table[] = {
   op4( beqi,  bnei,  blti,  blei),
   op4( bgti,  bgei,   ill,   ill),

   op4(  ill,   ill,   ill,   ill),
   op4(  ill,   ill,   ill,   ill),

   op4(beqid, bneid, bltid, bleid),
   op4(bgtid, bgeid,   ill,   ill),

   op4(  ill,   ill,   ill,   ill),
   op4(  ill,   ill,   ill,   ill),
};

void MicroblazeIss::insn_cbi()
{
   func_t func = cbi_table[m_ins.typeB.rd];
   r_btr = r_pc - 4;    /* In case the branch is not taken */
   (this->*func)();
}

/*\
 * Second decoding stage for the unconditionnal branch register instructions
\*/
MicroblazeIss::func_t const MicroblazeIss::ub_table[] = {
   op4(   br,  ill,  ill,  ill),
   op4(  ill,  ill,  ill,  ill),

   op4(  bra,  ill,  ill,  ill),
   op4(  brk,  ill,  ill,  ill),

   op4(  brd,  ill,  ill,  ill),
   op4( brld,  ill,  ill,  ill),

   op4(brald,  ill,  ill,  ill),
   op4(  ill,  ill,  ill,  ill),
};

void MicroblazeIss::insn_ub()
{
   func_t func = ub_table[m_ins.typeA.ra];
   r_btr = r_pc - 4;    /* In case the branch is not taken */
   (this->*func)();
}

/*\
 * Second decoding stage for the unconditionnal branch immediat instructions
\*/
MicroblazeIss::func_t const MicroblazeIss::ubi_table[] = {
   op4(  brim,  ill,  ill,  ill),
   op4(   ill,  ill,  ill,  ill),

   op4(  brai,  ill,  ill,  ill),
   op4(  brki,  ill,  ill,  ill),

   op4(  brid,  ill,  ill,  ill),
   op4( brlid,  ill,  ill,  ill),

   op4(bralid,  ill,  ill,  ill),
   op4(   ill,  ill,  ill,  ill),
};

void MicroblazeIss::insn_ubi()
{
   func_t func = ubi_table[m_ins.typeB.ra];
   r_btr = r_pc - 4;    /* In case the branch is not taken */
   (this->*func)();
}

void MicroblazeIss::insn_brim()
{
   if (m_ins.typeB.rd == 0 && m_ins.typeB.ra == 0 && m_ins.typeB.imm != 4) {
      r_btr = r_pc - 4;    /* In case the branch is not taken */
      insn_bri();
   } else if (m_ins.typeB.ra == 0 && m_ins.typeB.imm == 4)
      insn_mbar();
   else
      insn_ill();
}

/*\
 * Actual instructions execution
\*/

void MicroblazeIss::insn_beq()
{
   DASM_TYPEA_BC;
   if (r_gp[m_ins.typeA.ra] == 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_beqd()
{
   DASM_TYPEA_BC;
   if (r_gp[m_ins.typeA.ra] == 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_bge()
{
   DASM_TYPEA_BC;
   if ((int32_t)r_gp[m_ins.typeA.ra] >= 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bged()
{
   DASM_TYPEA_BC;
   if ((int32_t)r_gp[m_ins.typeA.ra] >= 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_bgt()
{
   DASM_TYPEA_BC;
   if ((int32_t)r_gp[m_ins.typeA.ra] > 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bgtd()
{
   DASM_TYPEA_BC;
   if ((int32_t)r_gp[m_ins.typeA.ra] > 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_ble()
{
   DASM_TYPEA_BC;
   if ((int32_t)r_gp[m_ins.typeA.ra] <= 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bled()
{
   DASM_TYPEA_BC;
   if ((int32_t)r_gp[m_ins.typeA.ra] <= 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_blt()
{
   DASM_TYPEA_BC;
   if ((int32_t)r_gp[m_ins.typeA.ra] < 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bltd()
{
   DASM_TYPEA_BC;
   if ((int32_t)r_gp[m_ins.typeA.ra] < 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_bne()
{
   DASM_TYPEA_BC;
   if (r_gp[m_ins.typeA.ra] != 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bned()
{
   DASM_TYPEA_BC;
   if (r_gp[m_ins.typeA.ra] != 0) {
      jump(r_gp[m_ins.typeA.rb] + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_beqi()
{
   DASM_TYPEB_BC;
   if (r_gp[m_ins.typeB.ra] == 0) {
      jump(imm_op + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_beqid()
{
   DASM_TYPEB_BC;
   if (r_gp[m_ins.typeB.ra] == 0) {
      jump(imm_op + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_bgei()
{
   DASM_TYPEB_BC;
   if ((int32_t)r_gp[m_ins.typeB.ra] >= 0) {
      jump(imm_op + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bgeid()
{
   DASM_TYPEB_BC;
   if ((int32_t)r_gp[m_ins.typeB.ra] >= 0) {
      jump(imm_op + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_bgti()
{
   DASM_TYPEB_BC;
   if ((int32_t)r_gp[m_ins.typeB.ra] > 0) {
      jump(imm_op + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bgtid()
{
   DASM_TYPEB_BC;
   if ((int32_t)r_gp[m_ins.typeB.ra] > 0) {
      jump(imm_op + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_blei()
{
   DASM_TYPEB_BC;
   if ((int32_t)r_gp[m_ins.typeB.ra] <= 0) {
      jump(imm_op + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bleid()
{
   DASM_TYPEB_BC;
   if ((int32_t)r_gp[m_ins.typeB.ra] <= 0) {
      jump(imm_op + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_blti()
{
   DASM_TYPEB_BC;
   if ((int32_t)r_gp[m_ins.typeB.ra] < 0) {
      jump(imm_op + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bltid()
{
   if ((int32_t)r_gp[m_ins.typeB.ra] < 0) {
      jump(imm_op + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_bnei()
{
   DASM_TYPEB_BC;
   if (r_gp[m_ins.typeB.ra] != 0) {
      jump(imm_op + r_pc, false);
      setInsDelay(3);
   }
}

void MicroblazeIss::insn_bneid()
{
   DASM_TYPEB_BC;
   if (r_gp[m_ins.typeB.ra] != 0) {
      jump(imm_op + r_pc, true);
      setInsDelay(2);
   }
}

void MicroblazeIss::insn_br()
{
   DASM_TYPEA_B;
   jump(r_gp[m_ins.typeA.rb] + r_pc, false);
   setInsDelay(3);
}

void MicroblazeIss::insn_bra()
{
   DASM_TYPEA_B;
   jump(r_gp[m_ins.typeA.rb], false);
   setInsDelay(3);
}

void MicroblazeIss::insn_brd()
{
   DASM_TYPEA_B;
   jump(r_gp[m_ins.typeA.rb] + r_pc, true);
   setInsDelay(2);
}

void MicroblazeIss::insn_brad()
{
   DASM_TYPEA_B;
   jump(r_gp[m_ins.typeA.rb], true);
   setInsDelay(2);
}

void MicroblazeIss::insn_brld()
{
   DASM_TYPEA_BL;
   r_gp[m_ins.typeA.rd] = r_pc;
   jump(r_gp[m_ins.typeA.rb] + r_pc, true);
   setInsDelay(2);
}

void MicroblazeIss::insn_brald()
{
   DASM_TYPEA_BL;
   r_gp[m_ins.typeA.rd] = r_pc;
   jump(r_gp[m_ins.typeA.rb], true);
   setInsDelay(2);
}

void MicroblazeIss::insn_brk()
{
   DASM_TYPEA_BL;
   /* FIXME: if MMU is enabled and in user mode, an exception
    *        should be raised
    */
   r_gp[m_ins.typeA.rd] = r_pc;
   jump(r_gp[m_ins.typeA.rb], true);
   r_msr.bip = 1;
   m_reservation = 0;
   setInsDelay(3);
}

void MicroblazeIss::insn_bri()
{
   DASM_TYPEB_B;
   jump(imm_op + r_pc, false);
   setInsDelay(3);
}

void MicroblazeIss::insn_brai()
{
   DASM_TYPEB_B;
   jump(imm_op, false);
   setInsDelay(3);
}

void MicroblazeIss::insn_brid()
{
   DASM_TYPEB_B;
   jump(imm_op + r_pc, true);
   setInsDelay(2);
}

void MicroblazeIss::insn_braid()
{
   DASM_TYPEB_B;
   jump(imm_op, true);
   setInsDelay(2);
}

void MicroblazeIss::insn_brlid()
{
   DASM_TYPEB_BL;
   r_gp[m_ins.typeA.rd] = r_pc;
   jump(imm_op + r_pc, true);
   setInsDelay(2);
}

void MicroblazeIss::insn_bralid()
{
   DASM_TYPEB_BL;
   /* User exception vector, more than strange, ...*/
   if (imm_op == 0x00000008) {
      r_msr.ums = r_msr.um;
      r_msr.um = MB_KERNEL;
      r_msr.vms = r_msr.um;
      r_msr.vm = 0;         /* TODO: Some enum may be defined somewhere */
      m_reservation = 0;
   }
   r_gp[m_ins.typeA.rd] = r_pc;
   jump(imm_op, true);
   setInsDelay(2);
}

void MicroblazeIss::insn_brki()
{
   DASM_TYPEB_BL;
   /* FIXME: if MMU is enabled and in user mode, an exception
    *        should be raised, but when imm_op == 0x8 or 0x18!
    *        Nice oddities, ...
    */
   r_gp[m_ins.typeA.rd] = r_pc;
   jump(imm_op, true);
   r_msr.bip = 1;
   m_reservation = 0;
   setInsDelay(3);
}

void MicroblazeIss::insn_mbar()
{
   DASM_TYPEB_B;
   /* The MBAR description in the doc seems to be wrong, so I believed
    * the pseudo code and not the text. */
   if ((m_ins.typeB.rd & 1) == 0)
      /* flush instruction pipeline, i.e. nothing in this ISS */
      ;
   if ((m_ins.typeB.rd & 2) == 0)
      /* TODO: empty the write buffer and ends the loads */
      ;
   jump(imm_op + r_pc, false);
   setInsDelay(3);
}

}}
// vim:filetype=cpp:expandtab:shiftwidth=3:tabstop=3:softtabstop=3

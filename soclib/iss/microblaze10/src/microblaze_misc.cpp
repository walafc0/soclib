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
 * 01/10/2011
 * Fork from the mips32 to microblaze
 * Frédéric Pétrot <Frederic.Petrot@imag.fr>, TIMA Lab, CNRS/Grenoble-INP/UJF
\*/

#include "microblaze.hpp"
#include "base_module.h"
#include "soclib_endian.h"

/*\
 * This file contains some second decoding stages.
 * As the microblaze contains quite a few of them, they are gathered
 * here under no specific logical order.
\*/

namespace soclib { namespace common {

/*\
 * Register barrel shift operations
\*/
MicroblazeIss::func_t const MicroblazeIss::bs_table[] = {
        op4(  bsrl,  ill,  bsra,  ill),
        op4(  bsll,  ill,   ill,  ill)
};

void MicroblazeIss::insn_bs()
{
   if (C_USE_BARREL !=1) 
      insn_ill();
   /* Only the 3 high order bit of the 11 bit fiels are used:
    * FIXME: should use a field mask of 0x3FF, a BS_MASK = 0x300 and
    * do some bit manipulation to obtain a 9 from it
    */
   func_t func = bs_table[m_ins.typeA.sh >> 9];
   (this->*func)();
}
      
/*\
 * Immediat barrel shift operations
\*/
MicroblazeIss::func_t const MicroblazeIss::bsi_table[] = {
        op4(  bsrli,  bsrai,  bslli, ill)
};

void MicroblazeIss::insn_bsi()
{
   if (C_USE_BARREL !=1 && (m_ins.typeB.imm & 0xF800)) 
      insn_ill();
   /* Only the 3 high order bit of the 11 bit fiels are used:
    * FIXME: should use a field mask of 0x3FF, a BS_MASK = 0x300 and
    * do some bit manipulation to obtain a 9 from it
    */
   func_t func = bsi_table[m_ins.typeB.imm >> 9];
   (this->*func)();
}

/*\
 * Integer compare operations
\*/
MicroblazeIss::func_t const MicroblazeIss::icmp_table[] = {
        op4(  rsubk,  cmp,  ill,  cmpu),
};

void MicroblazeIss::insn_icmp()
{
   func_t func = icmp_table[m_ins.typeA.sh & 0x3];
   (this->*func)();
}

/*\
 * Second decoding stage for the interger division
\*/
MicroblazeIss::func_t const MicroblazeIss::div_table[] = {
        op4( idiv,  ill,  idivu,  ill),
};

void MicroblazeIss::insn_div()
{
   func_t func = div_table[m_ins.typeA.sh];
   (this->*func)();
}

/*\
 * Second decoding stage for the special purpose registers access
\*/
MicroblazeIss::func_t const MicroblazeIss::msr_table[] = {
        op4( msrset, mfs, msrclr, mts),
};

void MicroblazeIss::insn_msr()
{
   uint32_t x;
   //Ugly, but so ut is, ...
   if ((m_ins.typeMFS.zero == 0 && m_ins.typeMFS.sel == 0x2) 
         || (m_ins.typeMTS.zero == 0 && m_ins.typeMTS.sel == 0x3))
      x = m_ins.typeMFS.sel;
   else if ((m_ins.typeMSR.cc == 0x22 || m_ins.typeMSR.cc == 0x20)) {
      x = m_ins.typeMSR.cc & 0x2;
   } else {
      insn_ill();
      return;
   }
   func_t func = msr_table[x];
   (this->*func)();
}

/*\
 * Second decoding stage for the multiplications
\*/
MicroblazeIss::func_t const MicroblazeIss::mult_table[] = {
        op4( mul, mulh, mulhsu, mulhu)
};

void MicroblazeIss::insn_mult()
{
   if ((m_ins.typeA.sh & ~0x3) != 0)
      insn_ill();
   func_t func = mult_table[m_ins.typeA.sh];
   (this->*func)();
}

/*\
 * This is really really odd, and I can by no means build a sensible
 * array out of this "beauty".
\*/
void MicroblazeIss::insn_misc()
{
   enum {SRA = 0x1,       SRC = 0x21, SRL = 0x41, SEXT8 = 0x60,
         SEXT16 = 0x61,   WIC = 0x68, WDC = 0x64, WDCFLUSH = 0x74,
         WDCCLEAR = 0x76, CLZ = 0xE0};

   switch (m_ins.typeA.sh) {
   case SRA:
      insn_sra();
      break;
   case SRC:
      insn_src();
      break;
   case SRL:
      insn_srl();
      break;
   case SEXT8:
      insn_sext8();
      break;
   case SEXT16:
      insn_sext16();
      break;
   case WIC:
      insn_wic();
      break;
   case WDC:
      insn_wdc();
      break;
   case WDCFLUSH:
      insn_wdcflush();
      break;
   case WDCCLEAR:
      insn_wdcclear();
      break;
   case CLZ:
      insn_clz();
      break;
   default:
      insn_ill();
   }
}

/*\
 * From now on, the instructions!
\*/
void MicroblazeIss::insn_bsrl()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] >> (r_gp[m_ins.typeA.rb] & 0x1F);
}

void MicroblazeIss::insn_bsra()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = ((int32_t)r_gp[m_ins.typeA.ra]) >> (r_gp[m_ins.typeA.rb] & 0x1F);
}

void MicroblazeIss::insn_bsll()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] << (r_gp[m_ins.typeA.rb] & 0x1F);
}

void MicroblazeIss::insn_bsrli()
{
   DASM_TYPEB;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] >> (m_ins.typeA.sh & 0x1F);
}

void MicroblazeIss::insn_bsrai()
{
   DASM_TYPEB;
   r_gp[m_ins.typeA.rd] = ((int32_t)r_gp[m_ins.typeA.ra]) >> (m_ins.typeA.sh & 0x1F);
}

void MicroblazeIss::insn_bslli()
{
   DASM_TYPEB;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.ra] << (m_ins.typeA.sh & 0x1F);
}


void MicroblazeIss::insn_clz()
{
#ifdef __GNUC__
#define clz(n) (((n)==0) ? 32 : __builtin_clz(n))
#else
/* Taken for bid_binarydecimal.c in gcc-4.6 distribution.
 * Strangely enough, I changed the ?: by a << and it was less
 * efficient on an x86! I can't see why. */
#define CLZ32_MASK16 0xFFFF0000ul
#define CLZ32_MASK8  0xFF00FF00ul
#define CLZ32_MASK4  0xF0F0F0F0ul
#define CLZ32_MASK2  0xCCCCCCCCul
#define CLZ32_MASK1  0xAAAAAAAAul

#define clz32_nz(n)                                               \
    (((((n) & CLZ32_MASK16) <= ((n) & ~CLZ32_MASK16)) ? 16 : 0) + \
       ((((n) & CLZ32_MASK8) <= ((n) & ~CLZ32_MASK8)) ? 8 : 0) +  \
       ((((n) & CLZ32_MASK4) <= ((n) & ~CLZ32_MASK4)) ? 4 : 0) +  \
       ((((n) & CLZ32_MASK2) <= ((n) & ~CLZ32_MASK2)) ? 2 : 0) +  \
       ((((n) & CLZ32_MASK1) <= ((n) & ~CLZ32_MASK1)) ? 1 : 0))

#define clz(n) (((n)==0) ? 32 : clz32_nz(n))
#endif
   DASM_TYPEB_WI;
   r_gp[m_ins.typeA.rd] = clz(r_gp[m_ins.typeA.ra]);
}

void MicroblazeIss::insn_rsubk()
{
   DASM_TYPEA;
   r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.rb] - r_gp[m_ins.typeA.ra];
}

/*\
 * Beware:
 * For cmp and cmpu, as rd can be one of ra or rb, it must be assigned at
 * once and not be used for intermediate computations.
\*/
void MicroblazeIss::insn_cmp()
{
   DASM_TYPEA;
   uint32_t x = (r_gp[m_ins.typeA.rb] - r_gp[m_ins.typeA.ra]) & 0x7FFFFFFF;
   uint32_t y = (int32_t)r_gp[m_ins.typeA.ra] > (int32_t)r_gp[m_ins.typeA.rb];
   r_gp[m_ins.typeA.rd] = (y << 31) | x;
}

void MicroblazeIss::insn_cmpu()
{
   DASM_TYPEA;
   uint32_t x = (r_gp[m_ins.typeA.rb] - r_gp[m_ins.typeA.ra]) & 0x7FFFFFFF;
   uint32_t y = r_gp[m_ins.typeA.ra] > r_gp[m_ins.typeA.rb];
   r_gp[m_ins.typeA.rd] = (y << 31) | x;
}

void MicroblazeIss::insn_idiv()
{
   DASM_TYPEA;
   if (r_gp[m_ins.typeA.ra] == 0) {
      r_gp[m_ins.typeA.rd] = 0;
      r_msr.dzo            = 1;
      r_esr.typeDIV.ec     = 0x05;
      r_esr.typeDIV.ess    = 0;
   } else if (r_gp[m_ins.typeA.ra] == 0xFFFFFFFF
              && r_gp[m_ins.typeA.rb] == 0x80000000) {
      r_gp[m_ins.typeA.rd] = 0x80000000;
      r_msr.dzo            = 1;
      r_esr.typeDIV.ec     = 0x05;
      r_esr.typeDIV.ess    = 1;
   } else
      r_gp[m_ins.typeA.rd] = (int32_t)r_gp[m_ins.typeA.rb]
                             / (int32_t)r_gp[m_ins.typeA.ra];
}

void MicroblazeIss::insn_idivu()
{
   DASM_TYPEA;
   if (r_gp[m_ins.typeA.ra] == 0) {
      r_gp[m_ins.typeA.rd] = 0;
      r_msr.dzo = 1;
      r_esr.typeDIV.ec = 0x05;
      r_esr.typeDIV.ess = 0;
   } else
      r_gp[m_ins.typeA.rd] = r_gp[m_ins.typeA.rb]/r_gp[m_ins.typeA.ra];             
}

void MicroblazeIss::insn_mfs()
{
   DASM_TYPEMFS;
   switch (m_ins.typeMFS.rs) {
   case 0x0000:
      r_gp[m_ins.typeMFS.rd] = r_pc;
      break;
   case 0x0001:
      r_gp[m_ins.typeMFS.rd] = r_msr.whole;
      break;
   case 0x0003:
      r_gp[m_ins.typeMFS.rd] = r_ear;
      break;
   case 0x0005:
      r_gp[m_ins.typeMFS.rd] = r_esr.esr;
      break;
   case 0x0007:
      r_gp[m_ins.typeMFS.rd] = r_fsr.whole;
      break;
   case 0x000B:
      r_gp[m_ins.typeMFS.rd] = r_btr;
      break;
   case 0x000D:
      r_gp[m_ins.typeMFS.rd] = r_edr;
      break;
   case 0x1000:
      r_gp[m_ins.typeMFS.rd] = r_pid.whole;
      break;
   case 0x1001:
      r_gp[m_ins.typeMFS.rd] = r_zpr.whole;
      break;
   case 0x1002:
      r_gp[m_ins.typeMFS.rd] = r_tlbx.whole;
      break;
   case 0x1003:
      r_gp[m_ins.typeMFS.rd] = r_tlblo.whole;
      break;
   case 0x1004:
      r_gp[m_ins.typeMFS.rd] = r_tlbhi.whole;
      break;
   default :
      if (m_ins.typeMFS.rs >= 0x2000 && m_ins.typeMFS.rs <= 0x200B)
         r_gp[m_ins.typeMFS.rd] = r_pvr[m_ins.typeMFS.rs & 0xF];
      else
         insn_ill();
   }
}

void MicroblazeIss::insn_msrclr()
{
   DASM_TYPEMFS;
   /* FIXME: strange stuff in the doc, to be fixed later on */
   if (C_USE_MMU >= 1 && r_msr.um == 1 && m_ins.typeMSR.imm15 != 0x04) {
      r_esr.typeBASE.ec = 0x07;
      m_exception = X_PI;
   } else {
      r_gp[m_ins.typeMSR.rd] = r_msr.whole;
      r_msr.whole &= ~m_ins.typeMSR.imm15;
   }
}

void MicroblazeIss::insn_msrset()
{
   DASM_TYPEMFS;
   /* FIXME: strange stuff in the doc, to be fixed later on */
   if (C_USE_MMU >= 1 && r_msr.um == 1 && m_ins.typeMSR.imm15 != 0x04) {
      r_esr.typeBASE.ec = 0x07;
      m_exception = X_PI;
   } else {
      r_gp[m_ins.typeMSR.rd] = r_msr.whole;
      r_msr.whole |= m_ins.typeMSR.imm15;
   }
}

void MicroblazeIss::insn_mts()
{
   DASM_TYPEMTS;
   if (C_USE_MMU >= 1 && r_msr.um == 1) {
      r_esr.typeBASE.ec = 0x07;
      m_exception = X_PI;
   } else {
      switch (m_ins.typeMTS.rs) {
      case 0x0001:
         r_msr.whole = r_gp[m_ins.typeMTS.ra];
         break;
      case 0x0007:
         r_fsr.whole = r_gp[m_ins.typeMTS.ra];
         break;
      case 0x1000:
         r_pid.whole = r_gp[m_ins.typeMTS.ra];
         break;
      case 0x1001:
         r_zpr.whole = r_gp[m_ins.typeMTS.ra];
         break;
      case 0x1002:
         r_tlbx.whole = r_gp[m_ins.typeMTS.ra];
         break;
      case 0x1003:
         r_tlblo.whole = r_gp[m_ins.typeMTS.ra];
         break;
      case 0x1004:
         r_tlbhi.whole = r_gp[m_ins.typeMTS.ra];
         break;
      case 0x1005:
         r_tlbsx.whole = r_gp[m_ins.typeMTS.ra];
         break;
      default:
         insn_ill();
         break;
      }
   }
}

void MicroblazeIss::insn_mul()
{
   DASM_TYPEA;
   if (C_USE_HW_MUL <= 0)
      insn_ill();
   r_gp[m_ins.typeA.rd] = (int32_t)r_gp[m_ins.typeA.ra]
                          * (int32_t)r_gp[m_ins.typeA.rb];
}

void MicroblazeIss::insn_mulh()
{
   DASM_TYPEA;
   if (C_USE_HW_MUL != 2)
      insn_ill();
   r_gp[m_ins.typeA.rd] = (((int64_t)r_gp[m_ins.typeA.ra]
                           * (int64_t)r_gp[m_ins.typeA.rb]) >> 32);
}

void MicroblazeIss::insn_mulhu()
{
   DASM_TYPEA;
   if (C_USE_HW_MUL != 2)
      insn_ill();
   r_gp[m_ins.typeA.rd] = (((uint64_t)r_gp[m_ins.typeA.ra]
                           * (uint64_t)r_gp[m_ins.typeA.rb]) >> 32);
}

void MicroblazeIss::insn_mulhsu()
{
   DASM_TYPEA;
   if (C_USE_HW_MUL != 2)
      insn_ill();
   r_gp[m_ins.typeA.rd] = ((int64_t)(((int64_t)r_gp[m_ins.typeA.ra]
                           * (uint64_t)r_gp[m_ins.typeA.rb])) >> 32);
}

void MicroblazeIss::insn_sext16()
{
   DASM_TYPEB_WI;
   r_gp[m_ins.typeA.rd] = sign_ext(r_gp[m_ins.typeA.ra], 16);
}

void MicroblazeIss::insn_sext8()
{
   DASM_TYPEB_WI;
   r_gp[m_ins.typeA.rd] = sign_ext(r_gp[m_ins.typeA.ra], 8);
}

void MicroblazeIss::insn_sra()
{
   DASM_TYPEB_WI;
   /* Beware, if rd is ra, order is important here, even though the
    * documentation is not so clear.
    * This applies for all shifts, by the way. */
   r_msr.c = r_gp[m_ins.typeA.ra] & 0x1;
   r_gp[m_ins.typeA.rd] = ((int32_t)r_gp[m_ins.typeA.ra] >> 1);
}

void MicroblazeIss::insn_src()
{
   DASM_TYPEB_WI;
   bool c = r_msr.c;
   r_msr.c = r_gp[m_ins.typeA.ra] & 0x1;
   r_gp[m_ins.typeA.rd] = ((c << 31) | (r_gp[m_ins.typeA.ra] >> 1));
}

void MicroblazeIss::insn_srl()
{
   DASM_TYPEB_WI;
   r_msr.c = r_gp[m_ins.typeA.ra] & 0x1;
   r_gp[m_ins.typeA.rd] = (r_gp[m_ins.typeA.ra] >> 1);
}

void MicroblazeIss::insn_wic()
{
   DASM_TYPEA_BC;
   insn_unimpl();
}

void MicroblazeIss::insn_wdc()
{
   DASM_TYPEA_BC;
   insn_unimpl();
}

void MicroblazeIss::insn_wdcflush()
{
   DASM_TYPEA_BC;
   insn_unimpl();
}

void MicroblazeIss::insn_wdcclear()
{
   DASM_TYPEA_BC;
   insn_unimpl();
}


}}
// vim:filetype=cpp:expandtab:shiftwidth=3:tabstop=3:softtabstop=3

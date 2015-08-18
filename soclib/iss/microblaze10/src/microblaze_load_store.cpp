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
 * This is a fork from the mips32, total reuse of the Iss2 mem access code
 *   Copyright (c) UPMC, Lip6
 *      Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Frédéric Pétrot <Frederic.Petrot@imag.fr>, TIMA Lab, CNRS/Grenoble-INP/UJF
\*/

#include "microblaze.h"
#include "microblaze.hpp"
#include "base_module.h"

#include <strings.h>

namespace soclib { namespace common {

namespace {
template<typename data_t>
data_t be_to_mask(data_t be)
{
    size_t i;
    data_t ret = 0;
    data_t be_up = (1 << (sizeof(data_t) - 1));

    for (i = 0; i < sizeof(data_t); ++i) {
        ret <<= 8;
        if (be_up & be)
            ret |= 0xff;
        be <<= 1;
    }
    return ret;
}
}

void MicroblazeIss::do_mem_access(addr_t address,
                                  int byte_count,
                                  uint32_t *dest,
                                  data_t wdata,
                                  enum DataOperationType operation)
{
   int byte_le = address & 3;
   /* The assert should never fail because the address alignement must
    * be check before calling the function
    */
   assert((byte_count + byte_le) <= 4);

   switch (operation) {
      /* These cases are not implemented, but must be to use caches */
   case Iss2::XTN_READ:
   case Iss2::XTN_WRITE:
      break;
   default:
      /* Magical stuff copied from the mips32 file */
      wdata = soclib::endian::uint32_swap(wdata) >> (8 * (4 - byte_count));
   }

   m_dreq.addr = address & (~3);
   m_dreq.be = (((1 << byte_count) - 1) << byte_le) & 0xf;

   m_dreq.valid = true;
   m_dreq.wdata = wdata << (8 * byte_le);
   m_dreq.type = operation;
   m_dreq.mode = r_bus_mode;

#ifdef SOCLIB_MODULE_DEBUG
   std::cout << name()
       << " do_mem_access: " << m_dreq
       << " off: " << byte_le
       << " dest: gp+" << dest - &r_gp[0]
       << std::endl;
#endif

   r_mem_byte_le = byte_le;
   r_mem_byte_count = byte_count;
   r_mem_dest = dest == &r_gp[0] ? NULL : dest;
}

bool MicroblazeIss::handle_dfetch(const struct DataResponse &rsp)
{
   if (!m_dreq.valid)
      return true;

   if (!rsp.valid)
      return false;

#ifdef SOCLIB_MODULE_DEBUG
   std::cout << name()
       << " setData: " << rsp
       << " dest: r_gp+" << (r_mem_dest == NULL ? 0 : r_mem_dest - &r_gp[0])
       << std::endl;
#endif

   m_dreq.valid = false;
   if (rsp.error) {
      m_exception = X_DB;
      r_ear = m_dreq.addr;
      return true;
   }

   // With destination register == 0, this is a store or a load to r0.
   if (r_mem_dest == NULL)
      return true;

   data_t data = rsp.rdata;
   int byte_count = r_mem_byte_count;

   data >>= 8 * r_mem_byte_le;

   data_t sdata = soclib::endian::uint32_swap(data) >> (8 * (4 - byte_count));
   data_t mask = be_to_mask<data_t>((1 << byte_count) - 1);
   data = sdata & mask;
#ifdef SOCLIB_MODULE_DEBUG
   std::cout << name()
       << " BE swapping"
       << " count: " << byte_count
       << " le: " << r_mem_byte_le
       << " orig data: " << rsp.rdata
       << " swapped data: " << sdata
       << " mask: " << mask << " data: " << data << std::endl;
#endif

   *r_mem_dest = data;

   return true;
}

void MicroblazeIss::insn_store()
{
   enum {
      SW = 0,
      SWX = 0x400,
   };

   switch (m_ins.typeA.sh) {
   case SW:
      insn_sw();
      break;
   case SWX:
      insn_swx();
      break;
   default:
      insn_ill();
   }

   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

#define check_align(address, align, store)  \
    if ((address)%(align)) {                \
        r_esr.typeUDA.ec = 1;               \
        r_esr.typeUDA.tw = align == 4;      \
        r_esr.typeUDA.ts = store;           \
        r_esr.typeUDA.rx = m_ins.typeA.rd;  \
        m_exception      = X_U;             \
        r_ear            = address;         \
        return;                             \
    }

/*\
 * Actual load and store instructions, no signed version exists for
 * the byte and half words loads, strange!
\*/
void MicroblazeIss::insn_lbu()
{
   DASM_TYPEA;
   uint32_t address = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb];
   do_mem_access(address, 1, &r_gp[m_ins.typeA.rd], 0, DATA_READ);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_lbui()
{
   DASM_TYPEB;
   uint32_t address = r_gp[m_ins.typeB.ra] + imm_op;
   do_mem_access(address, 1, &r_gp[m_ins.typeB.rd], 0, DATA_READ);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_lhu()
{
   DASM_TYPEA;
   uint32_t address = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb];
   check_align(address, 2, 0);
   do_mem_access(address, 2, &r_gp[m_ins.typeA.rd], 0, DATA_READ);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_lhui()
{
   DASM_TYPEB;
   uint32_t address = r_gp[m_ins.typeB.ra] + imm_op;
   check_align(address, 2, 0);
   do_mem_access(address, 2, &r_gp[m_ins.typeB.rd], 0, DATA_READ);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_lw()
{
   DASM_TYPEA;
   uint32_t address = r_gp[m_ins.typeB.ra] + r_gp[m_ins.typeA.rb];
   check_align(address, 4, 0);
   do_mem_access(address, 4, &r_gp[m_ins.typeA.rd], 0, DATA_READ);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_lwx()
{
   DASM_TYPEA;
   uint32_t address = r_gp[m_ins.typeB.ra] + r_gp[m_ins.typeA.rb];
   /* No alignment check, it is not a bug, it is a feature */
   do_mem_access(address, 4, &r_gp[m_ins.typeA.rd], 0, DATA_READ);
   m_reservation = 1;
   r_msr.c = 0;
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_load()
{
   enum {
      LW = 0,
      LWX = 0x400,
   };

   switch (m_ins.typeA.sh) {
   case LW:
      insn_lw();
      break;
   case LWX:
      insn_lwx();
      break;
   default:
      insn_ill();
   }

   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_lwi()
{
   DASM_TYPEB;
   uint32_t address = r_gp[m_ins.typeB.ra] + imm_op;
   check_align(address, 4, 0);
   do_mem_access(address, 4, &r_gp[m_ins.typeB.rd], 0, DATA_READ);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_sb()
{
   DASM_TYPEA;
   uint32_t address = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb];
   do_mem_access(address, 1, NULL, r_gp[m_ins.typeA.rd] & 0xFF, DATA_WRITE);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_sbi()
{
   DASM_TYPEB;
   uint32_t address = r_gp[m_ins.typeA.ra] + imm_op;
   do_mem_access(address, 1, NULL, r_gp[m_ins.typeA.rd] & 0xFF, DATA_WRITE);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_sh()
{
   DASM_TYPEA;
   uint32_t address = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb];
   check_align(address, 2, 1);
   do_mem_access(address, 2, NULL, r_gp[m_ins.typeA.rd] & 0xFFFF, DATA_WRITE);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_shi()
{
   DASM_TYPEB;
   uint32_t address = r_gp[m_ins.typeA.ra] + imm_op;
   check_align(address, 2, 1);
   do_mem_access(address, 2, NULL, r_gp[m_ins.typeA.rd] & 0xFFFF, DATA_WRITE);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_sw()
{
   DASM_TYPEA;
   uint32_t address = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb];
   check_align(address, 4, 1);
   do_mem_access(address, 4, NULL, r_gp[m_ins.typeA.rd], DATA_WRITE);
}

void MicroblazeIss::insn_swi()
{
   DASM_TYPEB;
   uint32_t address = r_gp[m_ins.typeA.ra] + imm_op;
   check_align(address, 4, 1);
   do_mem_access(address, 4, NULL, r_gp[m_ins.typeA.rd], DATA_WRITE);
   if (C_AREA_OPTIMIZED == 1)
      setInsDelay(2);
}

void MicroblazeIss::insn_swx()
{
   DASM_TYPEA;
   uint32_t address = r_gp[m_ins.typeA.ra] + r_gp[m_ins.typeA.rb];
   if (!m_reservation) {
      r_msr.c = 1;
   } else {
      do_mem_access(address, 4, NULL, r_gp[m_ins.typeA.rd], DATA_WRITE);
      m_reservation = 0;
      r_msr.c = 0;
   }
}

}}
// vim: filetype=cpp:expandtab:shiftwidth=3:tabstop=3:softtabstop=3

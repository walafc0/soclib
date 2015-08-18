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
#include "soclib_endian.h"
#include "arithmetics.h"
#include <iostream>
#include <iomanip>

// TODO:
// Check interruptions and exceptions
// Generate illegal instructions depending on configuration

namespace soclib { namespace common {

MicroblazeIss::MicroblazeIss(const std::string &name, uint32_t ident)
    : Iss2(name, ident)
{

    C_AREA_OPTIMIZED=0;      /* For timing only */
    C_PVR=2;                 /* Full featured support of pvr */
    C_PVR_USER1=0x00;
    C_PVR_USER2=0x00000000;  /* Proc number in multipro */
    C_RESET_MSR=0x00000400;  /* PVRs used, doc incoherent */
    C_USE_BARREL=1; 
    C_USE_DIV=1;
    C_USE_HW_MUL=1;
    C_USE_FPU=0;             /* Not yet */
    C_USE_MSR_INSTR=1;
    C_USE_PCMP_INSTR=1;
    C_UNALIGNED_EXCEPTION=1;
    C_ILL_OPCODE_EXCEPTION=1;
    C_IPLB_BUS_EXCEPTION=1;
    C_DPLB_BUS_EXCEPTION=1;
    C_IOPB_BUS_EXCEPTION=1;
    C_DOPB_BUS_EXCEPTION=1;
    C_DIV_ZERO_EXCEPTION=1;
    C_FPU_EXCEPTION=1;
    C_OPCODE_0x0_ILLEGAL=0;
    C_FSL_EXCEPTION=0;
    C_DEBUG_ENABLED=0;
    C_NUMBER_OF_RD_ADDR_BRK=1;
    C_NUMBER_OF_WR_ADDR_BRK=0;
    C_INTERRUPT_IS_EDGE=1;
    C_EDGE_IS_POSITIVE=1;
    C_FSL_LINKS=0;
    C_USE_EXTENDED_FSL_INSTR=0;
    C_ICACHE_BASEADDR=0x00000000;
    C_ICACHE_HIGHADDR=0x3FFFFFFF;
    C_USE_ICACHE=1;
    C_ALLOW_ICACHE_WR=0;
    C_ICACHE_LINELEN=4;
    C_ICACHE_ALWAYS_USED=0;
    C_ICACHE_INTERFACE=1;
    C_ADDR_TAG_BITS=17;          /* To be updated afterwards */
    C_CACHE_BYTE_SIZE=8192;      /* To be updated afterwards */
    
    C_DCACHE_BASEADDR=0x00000000;
    C_DCACHE_HIGHADDR=0x3FFFFFFF;
    C_USE_DCACHE=0;
    C_ALLOW_DCACHE_WR=1;
    C_DCACHE_LINELEN=4;
    C_DCACHE_ALWAYS_USED=0;
    C_DCACHE_INTERFACE=1;
    C_DCACHE_USE_WRITEBACK=1;
    C_DCACHE_ADDR_TAG=17;       /* To be updated afterwards */
    C_DCACHE_BYTE_SIZE=8192;    /* To be updated afterwards */

    C_USE_MMU=0;
    C_MMU_DTLB_SIZE=4;
    C_MMU_ITLB_SIZE=2;
    C_MMU_TLB_ACCESS=3;
    C_MMU_ZONES=16;
    C_USE_INTERRUPT=0;
    C_USE_EXT_BRK=0;
    C_USE_EXT_NM_BRK=0;
    
    /* No MMU for now */
    m_cache_info.has_mmu = false;
}

void MicroblazeIss::reset()
{
   struct DataRequest null_dreq = ISS_DREQ_INITIALIZER;
   r_pc = m_reset_address;
   r_npc = m_reset_address + 4;
   m_ifetch_addr = m_reset_address;
   m_next_pc = m_jump_pc = (uint32_t)-1;
   r_cpu_mode = MB_KERNEL;
   r_bus_mode = MODE_KERNEL;
	m_ibe = false;
	m_dbe = false;
   m_ins_delay = 0;
   m_dreq = null_dreq;
   r_mem_dest = NULL; 
	m_exec_cycles = 0;

   for(int i = 0; i < 32; i++)
      r_gp[i] = 0;
   r_count          = 0;
   r_compare        = 0;
   r_msr.whole      = 0;
   r_ear            = 0;
   r_esr.esr        = 0;
   m_ins.ins        = R_IR_NOP; 
   m_imm            = false;
	m_reservation    = false;
   m_exception      = NO_EXCEPTION;
	update_mode();
}

void MicroblazeIss::dump() const
{
    std::cout
        << std::hex << std::noshowbase
        << m_name
        << " PC: " << r_pc
        << " NPC: " << r_npc
        << " Ins: " << m_ins.ins << std::endl
        << std::dec
        << " ESR.ec: " << r_esr.typeBASE.ec << std::endl
        << " Mode: " << r_cpu_mode
        << " MSR.cc " << r_msr.cc
        << " .reserved " << r_msr.reserved0
        << " .vms " << r_msr.vms
        << " .vm " << r_msr.vm
        << " .ums " << r_msr.ums
        << " .um " << r_msr.um
        << " .pvr " << r_msr.pvr
        << " .eip " << r_msr.eip
        << " .ee " << r_msr.ee
        << " .dce " << r_msr.dce
        << " .dzo " << r_msr.dzo
        << " .ice " << r_msr.ice
        << " .fsl " << r_msr.fsl
        << " .bip " << r_msr.bip
        << " .c " << r_msr.c
        << " .ie " << r_msr.ie
        << " .be " << r_msr.be 
        << " .all: " << std::hex << r_msr.whole
        << std::endl
        << " op:  " << m_ins.typeA.op << " (" << name_table[m_ins.typeA.op] << ")" << std::endl
        << " a rd: " << m_ins.typeA.rd
        << " ra: "<< m_ins.typeA.ra
        << " rb: "<< m_ins.typeA.rb
        << std::endl << std::dec
        << " b rd: " << m_ins.typeB.rd
        << " ra: "<< m_ins.typeB.ra
        << " imm: "<< std::hex << m_ins.typeB.imm
        << std::endl;

    for (size_t i = 0; i < 32; ++i) {
        std::cout
			<< " " << std::dec << std::setw(2) << i << ": "
			<< std::hex << std::noshowbase << std::setw(8) << std::setfill('0')
			<< r_gp[i];
        if ( i%8 == 7 )
            std::cout << std::endl;
    }
}

void MicroblazeIss::run_for(uint32_t &ncycle, uint32_t &time_spent,
                            uint32_t in_pipe, uint32_t stalled)
{
   uint32_t total = in_pipe + stalled + 1;
   ncycle -= total;
   m_exec_cycles += total;
   time_spent += total;
   m_ins_delay -= std::min(m_ins_delay, total);
}

uint32_t MicroblazeIss::executeNCycles(uint32_t ncycle,
                                       const struct InstructionResponse &irsp,
                                       const struct DataResponse &drsp,
                                       uint32_t irq_bit_field)
{
#ifdef SOCLIB_MODULE_DEBUG
   std::cout
      << name()
      << __FUNCTION__ << "("
      << ncycle << ", "
      << irsp << ", "
      << drsp << ", "
      << irq_bit_field << ")"
      << std::endl;
#endif

   m_irqs = irq_bit_field;
   m_exception = NO_EXCEPTION;
   m_jump_pc = r_npc;
   m_next_pc = r_pc;
   r_btr = r_pc;

   uint32_t time_spent = 0;

   if (m_ins_delay)
      run_for(ncycle, time_spent, std::min(m_ins_delay, ncycle), 0);

   // The current instruction is executed in case of interrupt, but
   // the next instruction will be delayed.

   // The current instruction is not executed in case of exception,
   // and there is three types of bus error events, in order of
   // increasing priority:
   // 1 - instruction bus errors
   // 2 - read data bus errors
   // 3 - write data bus errors

   bool may_take_irq = check_irq_state();
   bool ireq_ok = handle_ifetch(irsp);
   bool dreq_ok = handle_dfetch(drsp);

   if (ncycle)
      run_for(ncycle, time_spent, 0, 1);

   if (m_exception != NO_EXCEPTION)
       goto got_exception;

   if (ncycle == 0)
       goto early_end;

   if (dreq_ok && ireq_ok) {
#ifdef SOCLIB_MODULE_DEBUG
        dump();
#endif
        run_for(ncycle, time_spent, 1, 0);

         m_next_pc = r_npc;
         m_jump_pc = r_npc+4;
         r_btr = r_pc;
         run();

        if (m_dreq.valid) {
            m_pc_for_dreq = r_pc;
			// test pour voir si on est sur un delay slot
            m_pc_for_dreq_is_ds = m_next_pc != r_pc+4;
        }
    }

    if (m_exception != NO_EXCEPTION)
        goto got_exception;

    if ( r_msr.ie
        && may_take_irq
        && check_irq_state()
        && dreq_ok )
        goto handle_irq;

    r_npc = m_jump_pc;
    r_pc = m_next_pc;
    m_ifetch_addr = m_next_pc;
    r_gp[0] = 0;
#ifdef SOCLIB_MODULE_DEBUG
   std::cout
      << std::hex << std::showbase
      << m_name
      << " m_next_pc: " << m_next_pc
      << " m_jump_pc: " << m_jump_pc
      << " m_ifetch_addr: " << m_ifetch_addr
      << std::endl;
#endif
   goto early_end;

 handle_irq:
   /*
     If we are about to take an interrupt, we know we have all data
     requests satisfied, but we may juste have posted a new one. If
     so, kill it (and reset the next instruction address) and take
     the interrupt.

     The data-access-in-delay-slot case is handled in
     handle_exception()
    */
   if (m_dreq.valid) {
       m_dreq.valid = false;
       m_jump_pc = r_npc;
       m_next_pc = r_pc;
       r_btr = r_pc;
   }
   r_btr = m_next_pc;
   m_exception = X_INT;
got_exception:
   handle_exception();
   return time_spent;
early_end:
#ifdef SOCLIB_MODULE_DEBUG
   std::cout
      << std::hex << std::showbase
      << m_name
      << " early_end:"
      << " ireq_ok=" << ireq_ok
      << " dreq_ok=" << dreq_ok
      << " m_ins_delay=" << m_ins_delay
      << " ncycle=" << ncycle
      << " time_spent=" << time_spent
      << std::endl;
#endif
   return time_spent;
}

bool MicroblazeIss::handle_ifetch(const struct InstructionResponse &irsp)
{
   if (m_ifetch_addr != r_pc || !irsp.valid)
      return false;

   if (irsp.error) {
      m_exception = X_IPLB;
      r_btr = m_ifetch_addr;
      return true;
   }

   // Microblaze is big endian
   m_ins.ins = soclib::endian::uint32_swap(irsp.instruction);

   return true;
}

void MicroblazeIss::handle_exception()
{
#ifdef SOCLIB_MODULE_DEBUG
   std::cout
       << m_name
       << " r_btr: " << std::hex << r_btr
       << " m_pc_for_dreq_is_ds: " << std::hex << m_pc_for_dreq_is_ds
       << " m_pc_for_dreq: " << std::hex << m_pc_for_dreq << std::endl;
#endif

   bool in_delay_slot = m_next_pc + 4 != m_jump_pc; /* In delay slot */
   r_esr.typeBASE.ds = in_delay_slot;
   addr_t except_address = exceptAddr(m_exception);
   r_msr.um = MB_KERNEL;
   update_mode();

#ifdef SOCLIB_MODULE_DEBUG
   std::cout
       << m_name << " exception: " << m_exception << std::endl
       << " m_ins: " << m_ins.typeA.op
       << " epc: " << r_ear
       << " ear: " << m_dreq.addr
       << " esr.ds: " << in_delay_slot
       << " .ess: " << r_esr.typeBASE.ess
       << " .ec: " << r_esr.typeBASE.ec
       << " msr.um: " << r_msr.um
       << " exception address: " << except_address << std::endl;
#endif

   r_pc = except_address;
   r_npc = except_address + 4;
   m_ifetch_addr = except_address;
}

Iss2::debug_register_t MicroblazeIss::debugGetRegisterValue(unsigned int reg) const
{
   switch (reg) {
   case 0:
      return 0;
   case 1 ... 31:
      return r_gp[reg];
   case 32:
      return r_msr.whole;
   case 33:
      return r_ear;
   case 34:
      return r_esr.esr;
   case 35:
      return r_pc;
   case ISS_DEBUG_REG_IS_USERMODE:
      return !isPriviliged();
   case ISS_DEBUG_REG_IS_INTERRUPTIBLE:
      return r_msr.ie;
   case ISS_DEBUG_REG_STACK_REDZONE_SIZE:
   default:
      return 0;
   }
}

void MicroblazeIss::debugSetRegisterValue(unsigned int reg,
                                          debug_register_t value)
{
   switch (reg) {
   case 1 ... 31:
      r_gp[reg] = value;
      break;
   case 32:
      r_msr.whole = value;
      break;
   case 33:
      r_ear = value;
      break;
   case 34:
      r_esr.esr = value;
      break;
   case 35:
      r_pc = value;
      r_npc = value + 4;
      break;
   default:
      break;
   }
}

namespace {
static size_t lines_to_s(size_t lines)
{
   return clamp<size_t>(0, uint32_log2(lines) - 6, 7);
}

static size_t line_size_to_l(size_t line_size)
{
   if (line_size == 0)
      return 0;
   return clamp<size_t>(1, uint32_log2(line_size / 4) + 1, 7);
}
}

void MicroblazeIss::setCacheInfo(const struct CacheInfo &info)
{
   pvr4_t icache;
   pvr5_t dcache;

   /* Not really compatible with a generic cache, ... 
    * Should however be updated to allow proper behavior of the cache
    * related instructions */
   icache.icu   = C_USE_ICACHE;
   dcache.dcu   = C_USE_DCACHE;
}

MicroblazeIss::addr_t MicroblazeIss::exceptAddr(enum ExceptCause cause) const
{
   switch (cause) {
   case X_INT: return INTERRUPT_VECTOR;
   case X_ENMB:
   case X_EMB: return BREAK_VECTOR;
   case X_D:
   case X_U:
   case X_DB:
   case X_IPLB:
   case X_FPU:
   case X_IOP: return EXCEPTION_VECTOR;
   default :
       std::cerr << "Unknown exception cause, quitting" << std::endl;
       exit(1);
   }
}

void MicroblazeIss::update_mode()
{
 	r_bus_mode = (r_msr.um) ? MODE_USER  : MODE_KERNEL;
	r_cpu_mode = (r_msr.um) ? MB_USER    : MB_KERNEL;
	r_cpu_mode = (r_msr.vm) ? MB_VIRTUAL : r_cpu_mode;
}

}}

// vim: filetype=cpp:expandtab:shiftwidth=3:tabstop=3:softtabstop=3

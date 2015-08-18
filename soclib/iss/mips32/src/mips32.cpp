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
 *    Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: nipo
 *
 * $Id$
 */

#include "mips32.h"
#include "mips32.hpp"
#include "base_module.h"
#include "soclib_endian.h"
#include "arithmetics.h"
#include <iostream>
#include <iomanip>

namespace soclib { namespace common {

Mips32Iss::Mips32Iss(const std::string &name, uint32_t ident, bool default_little_endian)
    : Iss2(name, ident),
      m_little_endian(default_little_endian)
{
    r_config.whole = 0;
    r_config.m = 1; // presence of Config1 register
    r_config.be = m_little_endian ? 0 : 1;
    r_config.ar = 1; // MIPS32R2
    r_config.mt = 7; // reserved, let's say it's soclib generic MMU :)

    r_config1.whole = 0;
    r_config1.m = 1; // presence of Config2 register
    r_config1.c2 = 1; // Cop2 presence, i.e. generic MMU access

    r_config2.whole = 0;
    r_config2.m = 1; // presence of Config3 register

    r_config3.whole = 0;
    r_config3.ulri = 1; // presence of UserLocal registers
    r_config3.vint = 1; // vectored interrupts implemented

    m_cache_info.has_mmu = false;

#ifdef SOCLIB_MODULE_DEBUG
    m_debug_mask = 0xffffffff; // for backward compat
#endif
}

void Mips32Iss::reset()
{
    struct DataRequest null_dreq = ISS_DREQ_INITIALIZER;
    r_ebase.whole = 0;
    r_ebase.exception_base = 0x80000; // will be expanded to 0x80000000
    r_ebase.cpunum = m_ident;
    r_pc = m_reset_address;
    r_npc = m_reset_address + 4;
    m_ifetch_addr = m_reset_address;
    m_next_pc = m_jump_pc = (uint32_t)-1;
    r_cpu_mode = MIPS32_KERNEL;
    m_ibe = false;
    m_dbe = false;
    m_dreq = null_dreq;
    r_mem_dest = NULL;
    m_ins_delay = 0;
    r_status.whole = 0;
    r_status.bev = 1; // BEV (Bootstrap Exception Vector mode)
    r_status.erl = 1; // ERL (ERror Level set when reset)
    r_cause.whole = 0;
    m_instruction_count = 0;
    m_pipeline_use_count = 0;
    r_gp[0] = 0;
    m_microcode_func = m_bootstrap_cpu_id < 0 || m_bootstrap_cpu_id == (int)m_ident
                                            ? NULL : &Mips32Iss::do_microcoded_sleep;
    r_cycle_count = 0;
    r_compare = 0;
    r_tls_base = 0;
    r_hwrena = 0;

    r_bus_mode = MODE_KERNEL;

	r_status.fr = 0;

    r_fir.whole = 0;
	r_fir.w = 1;
	r_fir.d = 1;
	r_fir.s = 1;
	r_fir.processorID = m_ident;

	r_fcsr.whole = 0;

	// Default values
	r_fcsr.enables_v = 1;
	r_fcsr.enables_z = 1;
	r_fcsr.enables_o = 1;
	r_fcsr.enables_u = 1;
	r_fcsr.enables_i = 1;

    for(int i = 0; i<32; i++)
        r_gp[i] = 0;

    m_hazard=false;
    m_exception = NO_EXCEPTION;
    update_mode();
}


void Mips32Iss::dump() const
{
    std::cout
        << std::hex << std::noshowbase
        << m_name
        << " PC: " << r_pc
        << " NPC: " << r_npc
        << " Ins: " << m_ins.ins << std::endl
        << std::dec
        << " Cause.xcode: " << r_cause.xcode << std::endl
        << " Mode: " << r_cpu_mode
        << " Status.ksu " << r_status.ksu
        << " .exl: " << r_status.exl
        << " .erl: " << r_status.erl
        << " .whole: " << std::hex << r_status.whole
        << std::endl
        << " op:  " << m_ins.i.op << " (" << name_table[m_ins.i.op] << ")" << std::endl
        << " i rs: " << m_ins.i.rs
        << " rt: "<<m_ins.i.rt
        << " i: "<<std::hex << m_ins.i.imd
        << std::endl << std::dec
        << " r rs: " << m_ins.r.rs
        << " rt: "<<m_ins.r.rt
        << " rd: "<<m_ins.r.rd
        << " sh: "<<m_ins.r.sh << std::hex
        << " func: "<<m_ins.r.func
        << std::endl
        << " V rs: " << r_gp[m_ins.i.rs]
        << " rt: "<<r_gp[m_ins.i.rt]
        << std::endl;
    for ( size_t i=0; i<32; ++i ) {
        std::cout
			<< " " << std::dec << std::setw(2) << i << ": "
			<< std::hex << std::noshowbase << std::setw(8) << std::setfill('0')
			<< r_gp[i];
        if ( i%8 == 7 )
            std::cout << std::endl;
    }
}

void Mips32Iss::run_for(uint32_t &ncycle, uint32_t &time_spent,
             uint32_t in_pipe, uint32_t stalled)
{
    uint32_t total = in_pipe + stalled;
    ncycle -= total;
    r_cycle_count += total;
    time_spent += total;
    m_ins_delay -= std::min(m_ins_delay, total);
    m_pipeline_use_count += in_pipe;
}

uint32_t Mips32Iss::executeNCycles(
                                   uint32_t ncycle,
                                   const struct InstructionResponse &irsp,
                                   const struct DataResponse &drsp,
                                   uint32_t irq_bit_field )
{
#ifdef SOCLIB_MODULE_DEBUG
    if (m_debug_mask & MIPS32_DEBUG_ISS) {
	std::cout
	    << name()
	    << " executeNCycles( "
	    << ncycle << ", "
	    << irsp << ", "
	    << drsp << ", "
	    << irq_bit_field << ")"
	    << std::endl;
    }
#endif

    m_irqs = irq_bit_field;
    r_cause.ripl = irq_bit_field;
    m_exception = NO_EXCEPTION;
    m_jump_pc = r_npc;
    m_next_pc = r_pc;
    m_hazard = false;
    m_resume_pc = r_pc;

    uint32_t time_spent = 0;

    if ( m_ins_delay )
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

    if ( m_hazard && ncycle )
        run_for(ncycle, time_spent, 0, 1);

    if ( m_exception != NO_EXCEPTION )
        goto got_exception;

    if ( ncycle == 0 )
        goto early_end;

    if ( dreq_ok && ireq_ok ) {
#ifdef SOCLIB_MODULE_DEBUG
	if (m_debug_mask & MIPS32_DEBUG_CPU) {
	    dump();
	}
#endif
        run_for(ncycle, time_spent, 1, 0);

        if ( m_microcode_func ) {
            (this->*m_microcode_func)();
        } else {
            m_next_pc = r_npc;
            m_jump_pc = r_npc+4;
            m_resume_pc = r_pc;
            run();
        }
        if ( m_dreq.valid ) {
            m_pc_for_dreq = r_pc;
            m_pc_for_dreq_is_ds = m_next_pc != r_pc+4; 
        }
    } else {
        run_for(ncycle, time_spent, 0, 1);
    }

    if ( m_exception != NO_EXCEPTION )
        goto got_exception;

    if ( m_dbe && dreq_ok ) {
        m_exception = X_DBE;
        m_dbe = false;
        goto handle_wdbe_irq;
    }

    if ( (r_status.im & r_cause.ip)
         && may_take_irq
         && check_irq_state()
         && dreq_ok ) {
        m_exception = X_INT;
        goto handle_wdbe_irq;
    }

    r_npc = m_jump_pc;
    r_pc = m_next_pc;
    m_ifetch_addr = m_next_pc;
    r_gp[0] = 0;
#ifdef SOCLIB_MODULE_DEBUG
    if (m_debug_mask & MIPS32_DEBUG_INTERNAL) {
        std::cout
            << std::hex << std::showbase
            << m_name
            << " m_next_pc: " << m_next_pc
            << " m_jump_pc: " << m_jump_pc
            << " m_ifetch_addr: " << m_ifetch_addr
            << std::endl;
     }
#endif
    goto early_end;

    /*
      If we are about to take an interrupt or an async write berr,
      we know we have all data
      requests satisfied, but we may juste have posted a new one. If
      so, kill it (and reset the next instruction address) and take
      the interrupt.

      The data-access-in-delay-slot case is handled in
      handle_exception()
     */
  handle_wdbe_irq:
    if ( m_dreq.valid ) {
        m_dreq.valid = false;
        m_jump_pc = r_npc;
        m_next_pc = r_pc;
        m_resume_pc = r_pc;
    }
    m_resume_pc = m_next_pc;
 got_exception:
    handle_exception();
    return time_spent;
 early_end:
#ifdef SOCLIB_MODULE_DEBUG
    if (m_debug_mask & MIPS32_DEBUG_INTERNAL) {
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
     }
#endif
    return time_spent;
}

bool Mips32Iss::handle_ifetch(
                              const struct InstructionResponse &irsp
                              )
{
    if ( m_microcode_func ) {
        return true;
    }

    if ( m_ifetch_addr != r_pc || !irsp.valid ) {
        return false;
    }

    if ( irsp.error ) {
        m_exception = X_IBE;
        m_resume_pc = m_ifetch_addr;
        return true;
    }

    m_ins.ins = irsp.instruction;
    if ( ! m_little_endian )
        m_ins.ins = soclib::endian::uint32_swap(irsp.instruction);

    return true;
}

void Mips32Iss::handle_exception()
{
    m_microcode_func = NULL;
    ExceptionClass ex_class = EXCL_FAULT;
    ExceptionCause ex_cause = EXCA_OTHER;

    switch (m_exception) {
    case X_INT:
        ex_class = EXCL_IRQ;
        break;
    case X_SYS:
        ex_class = EXCL_SYSCALL;
        break;
    case X_BP:
    case X_TR:
        ex_class = EXCL_TRAP;
        break;

    case X_MOD:
    case X_reserved:
        abort();

    case X_TLBL:
    case X_TLBS:
        ex_cause = EXCA_PAGEFAULT;
        break;
    case X_ADEL:
    case X_ADES:
        ex_cause = EXCA_ALIGN;
        break;
    case X_IBE:
    case X_DBE:
        ex_cause = EXCA_BADADDR;
        break;
    case X_RI:
    case X_CPU:
        ex_cause = EXCA_ILL;
        break;
    case X_OV:
    case X_FPE:
        ex_cause = EXCA_FPU;
        break;

    default:
        assert(!"This must not happen");
    }

    if ( debugExceptionBypassed( ex_class, ex_cause ) )
        return;

#ifdef SOCLIB_MODULE_DEBUG
    if (m_debug_mask & MIPS32_DEBUG_INTERNAL) {
        std::cout
            << m_name
            << " m_resume_pc: " << std::hex << m_resume_pc
            << " m_pc_for_dreq_is_ds: " << std::hex << m_pc_for_dreq_is_ds
            << " m_pc_for_dreq: " << std::hex << m_pc_for_dreq
            << std::endl;
     }
#endif

    addr_t except_address = exceptBaseAddr();
    bool branch_taken = m_next_pc+4 != m_jump_pc;

    if ( m_resume_pc == r_pc )
        branch_taken = r_pc+4 != r_npc;

    if ( m_exception == X_DBE ) {
        branch_taken = m_pc_for_dreq_is_ds;
        m_resume_pc = m_pc_for_dreq;
    }

    if ( r_status.exl ) {
        except_address += 0x180;
    } else {
        r_cause.bd = branch_taken;
        r_epc = m_resume_pc - 4*branch_taken;
        except_address += exceptOffsetAddr(m_exception);
    }
    r_cause.xcode = m_exception;
    r_status.exl = 1;
    update_mode();

#ifdef SOCLIB_MODULE_DEBUG
    if (m_debug_mask & MIPS32_DEBUG_IRQ) {
        std::cout
            << m_name <<" exception: "<<m_exception<<std::endl
            << " m_ins: " << m_ins.j.op
            << " epc: " << r_epc
            << " error_epc: " << r_error_epc
            << " bar: " << m_dreq.addr
            << " cause.xcode: " << r_cause.xcode
            << " .bd: " << r_cause.bd
            << " .ip: " << r_cause.ip
            << " status.exl: " << r_status.exl
            << " .erl: " << r_status.erl
            << " exception address: " << except_address
            << std::endl;
     }
#endif

    r_pc = except_address;
    r_npc = except_address+4;
    m_ifetch_addr = except_address;
}

Iss2::debug_register_t Mips32Iss::debugGetRegisterValue(unsigned int reg) const
{
    switch (reg)
        {
        case 0:
            return 0;
        case 1 ... 31:
            return r_gp[reg];
        case 32:
            return r_status.whole;
        case 33:
            return r_lo;
        case 34:
            return r_hi;
        case 35:
            return r_bar;
        case 36:
            return r_cause.whole;
        case 37:
            return r_pc;
        case 38 ... 69:
            return r_f[reg-38];
        case ISS_DEBUG_REG_IS_USERMODE:
            return !isPriviliged();
        case ISS_DEBUG_REG_IS_INTERRUPTIBLE:
            return r_status.ie && !r_status.exl && !r_status.erl;
        case ISS_DEBUG_REG_STACK_REDZONE_SIZE:
        default:
            return 0;
        }
}

void Mips32Iss::debugSetRegisterValue(unsigned int reg, debug_register_t value)
{
    switch (reg)
        {
        case 1 ... 31:
            r_gp[reg] = value;
            break;
        case 32:
            r_status.whole = value;
            break;
        case 33:
            r_lo = value;
            break;
        case 34:
            r_hi = value;
            break;
        case 35:
            r_bar = value;
            break;
        case 36:
            r_cause.whole = value;
            break;
        case 37:
            r_pc = value;
            r_npc = value+4;
            break;
        case 38 ... 69:
            r_f[reg-38] = value;
        default:
            break;
        }
}

namespace {
static size_t lines_to_s( size_t lines )
{
    return clamp<size_t>(0, uint32_log2(lines)-6, 7);
}
static size_t line_size_to_l( size_t line_size )
{
    if ( line_size == 0 )
        return 0;
    return clamp<size_t>(1, uint32_log2(line_size/4)+1, 7);
}
}

void Mips32Iss::setCacheInfo( const struct CacheInfo &info )
{
    r_config1.ia = info.icache_assoc-1;
    r_config1.is = lines_to_s(info.icache_n_lines);
    r_config1.il = line_size_to_l(info.icache_line_size);
    r_config1.da = info.dcache_assoc-1;
    r_config1.ds = lines_to_s(info.dcache_n_lines);
    r_config1.dl = line_size_to_l(info.dcache_line_size);
    m_cache_info = info;
}

Mips32Iss::addr_t Mips32Iss::exceptOffsetAddr( enum ExceptCause cause ) const
{
    if ( r_cause.iv ) {
        if ( r_status.bev || !r_intctl.vs )
            return 0x200;
        else {
            int vn;

            if ( r_config3.veic )
                vn = r_cause.ripl;
            else {
                int ip = r_cause.ip;
                assert(ip && "r_cause.ip should be not null!");
                vn = soclib::common::fls(ip) - 1;
            }
            return 0x200 + vn * (r_intctl.vs << 5);
        }
    } else {
        return 0x180;
    }
}



Mips32Iss::addr_t Mips32Iss::exceptBaseAddr() const
{
    if ( r_status.bev )
        return m_reset_address + 0x200;
    else
        return r_ebase.whole & 0xfffff000;
}

void Mips32Iss::do_microcoded_sleep()
{
    // The sleep doesnt count as a pipeline usage... we do nothing.
    --m_pipeline_use_count;
    if ( m_irqs ) {
        m_microcode_func = NULL;
#ifdef SOCLIB_MODULE_DEBUG
        if (m_debug_mask & MIPS32_DEBUG_IRQ) {
            std::cout << name() << " IRQ while sleeping" << std::endl;
	}
#endif
    }
}

uint32_t Mips32Iss::m_reset_address = 0xbfc00000;
int Mips32Iss::m_bootstrap_cpu_id = -1;

#ifdef SOCVIEW3
void Mips32Iss::register_debugger(tracer &t)
{    t.add(m_next_pc,name()+"_"+"m_next_pc");
    t.add(m_instruction_count,name()+"_"+"processor_is_running");
    t.add(m_pipeline_use_count, name()+"_"+"m_pipeline_use_count");
    t.add(m_instruction_count, name()+"_"+"m_instruction_count");
    t.add(r_cycle_count, name()+"_"+"r_cycle_count");
   
    }
#endif



}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

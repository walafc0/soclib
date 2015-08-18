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
 *         Alexandre Becoulet <alexandre.becoulet@free.fr>, 2009
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo becoulet
 *
 * $Id: arm.cpp 2344 2013-05-29 15:08:56Z becoulet $
 *
 */

#include "arm.h"
#include "arithmetics.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>

namespace soclib { namespace common {

const ArmIss::ArmMode ArmIss::psr_to_mode[32] = {
	MOD_Count, MOD_Count, MOD_Count, MOD_Count,
	MOD_Count, MOD_Count, MOD_Count, MOD_Count,
	MOD_Count, MOD_Count, MOD_Count, MOD_Count,
	MOD_Count, MOD_Count, MOD_Count, MOD_Count,

	MOD_USER32, MOD_FIQ32, MOD_IRQ32, MOD_SUPER32,
	MOD_Count,  MOD_Count, MOD_Count, MOD_ABORT32,
	MOD_Count,  MOD_Count, MOD_Count, MOD_UNDEF32,
	MOD_Count,  MOD_Count, MOD_Count, MOD_Count,
};

const ArmIss::ArmPsrMode ArmIss::mode_to_psr[MOD_Count] = { 
	MOD_PSR_USER32,
	MOD_PSR_FIQ32,
	MOD_PSR_IRQ32,
	MOD_PSR_SUPER32,
	MOD_PSR_ABORT32,
	MOD_PSR_UNDEF32,
};

const ArmIss::except_info_s ArmIss::except_info[EXCEPT_Count] = {
	/* EXCEPT_NONE */ {},
	/* EXCEPT_UNDEF*/ { "UNDEF", false, MOD_PSR_UNDEF32, 0x04, 4 },
	/* EXCEPT_SWI  */ { "SWI",   false, MOD_PSR_SUPER32, 0x08, 4 },
	/* EXCEPT_FIQ  */ { "FIQ",   true,  MOD_PSR_FIQ32,   0x1C, 4 },
	/* EXCEPT_IRQ  */ { "IRQ",   false, MOD_PSR_IRQ32,   0x18, 4 },
	/* EXCEPT_PABT */ { "PABT",  false, MOD_PSR_ABORT32, 0x0C, 4 },
	/* EXCEPT_DABT */ { "DABT",  false, MOD_PSR_ABORT32, 0x10, 8 },
};

void ArmIss::cpsr_update(psr_t psr)
{
	ArmMode cur_mode = psr_to_mode[r_cpsr.mode];
	ArmMode new_mode = psr_to_mode[psr.mode];

#if defined(SOCLIB_MODULE_DEBUG)
	std::cout
		<< name() << " cpsr update "
		<< std::hex << r_cpsr.whole << " -> " << psr.whole
		<< ((new_mode != cur_mode) ? " new mode" : " no mode change")
		<< std::endl;
#endif

	if (new_mode == MOD_Count)
		return reset();

	if (new_mode != cur_mode) {
		// swap r13 and r14
		std::memcpy(r_r13_r14[cur_mode],  r_gp + 13,           sizeof(data_t) * 2);
		std::memcpy(r_gp + 13,            r_r13_r14[new_mode], sizeof(data_t) * 2);

		// swap r13 to r12 for FIQ mode
		if (cur_mode == MOD_FIQ32) {
			// From FIQ to other modes
			std::memcpy(r_r8_r12[1], r_gp + 8,    sizeof(data_t) * 5);
			std::memcpy(r_gp + 8,    r_r8_r12[0], sizeof(data_t) * 5);
		} else if (new_mode == MOD_FIQ32) {
			// From other modes to FIQ
			std::memcpy(r_r8_r12[0], r_gp + 8,    sizeof(data_t) * 5);
			std::memcpy(r_gp + 8,    r_r8_r12[1], sizeof(data_t) * 5);
		}

		r_spsr[new_mode] = r_cpsr;
	}
	r_cpsr = psr;
}

void ArmIss::getRequests(
	Iss2::InstructionRequest &ireq,
	Iss2::DataRequest &dreq
	) const
{
	ireq.valid = m_microcode_func == NULL;
	ireq.addr = m_current_pc & ~3;

	dreq = m_dreq;
}

uint32_t ArmIss::executeNCycles(
	uint32_t ncycle,
	const Iss2::InstructionResponse &irsp,
	const Iss2::DataResponse &drsp,
	uint32_t irq_state
	)
{
	m_irq_in = irq_state & 0x1;
	m_fiq_in = irq_state & 0x2;

	bool accept_external_interrupts = !r_cpsr.irq_disabled && !m_microcode_func;
	bool accept_fast_external_interrupts = !r_cpsr.fiq_disabled && !m_microcode_func;
	bool instruction_asked = m_microcode_func == NULL;
	
	bool data_req_nok = m_dreq.valid;

    /* handle instruction fetch response */
	if ( instruction_asked && irsp.valid ) {
		m_ins_error |= irsp.error;
		m_opcode.ins = irsp.instruction;
        m_thumb_op.ins = irsp.instruction >> ((m_current_pc & 2) ? 16 : 0);
		instruction_asked = false;
	}

    bool r15_changed = m_current_pc != r_gp[15];

    /* handle data fetch response */
	if ( data_req_nok && drsp.valid ) {
		m_data_error |= drsp.error;
		data_req_nok = false;

        r15_changed |= handle_data_response(drsp);
	}

    if (r15_changed) {
        /* if r15 changed, we need to fetch an other instruction */
        r_cpsr.thumb = r_gp[15] & 1;
        r_gp[15] &= ~1;
        m_current_pc = r_gp[15];

        /* discard fetch error due to wrong ifetch address */
        m_ins_error = false;

        return ncycle;
	}

    /* no cycle to spend? */
	if ( ncycle == 0 ) {
		return 0;

    /* waiting on ifetch or dfecth? */
    } else if ( instruction_asked || data_req_nok ) {
		m_cycle_count += ncycle;
		return ncycle;

    /* have an ifetch error pending? */
    } else if ( m_ins_error ) {
		m_exception = EXCEPT_PABT;
		m_ins_error = false;

    /* have a dfetch error pending? */
	} else if ( m_data_error ) {
		m_exception = EXCEPT_DABT;
		m_data_error = false;

    /* currently inside an instruction microcode? */
	} else if ( m_microcode_func ) {
        (this->*m_microcode_func)();

    /* process pending fast irq? */
    } else if ( m_fiq_in && accept_fast_external_interrupts &&
                !r_cpsr.fiq_disabled ) {
        m_exception = EXCEPT_FIQ;

    /* process pending irq? */
	} else if ( m_irq_in && accept_external_interrupts &&
                !r_cpsr.irq_disabled ) {
        m_exception = EXCEPT_IRQ;

    /* then, execute the current instruction. */
    } else {

        m_run_count += 1;
        m_ldstm_sp_offset = 0;

        /* r15 actually points to what has been fetched? */
        assert( r_gp[15] == m_current_pc );

        /* move r15 to next instruction */
        r_gp[15] += r_cpsr.thumb ? 2 : 4;

        /* perform instruction operation */
        if ( r_cpsr.thumb )
            run_thumb();
        else
            run();

#ifdef SOCLIB_MODULE_DEBUG
		dump();
#endif
	}

    /* some pending exception to process? */
    if ( m_exception != EXCEPT_NONE ) {

        m_ldstm_sp_offset = 0;
        m_microcode_func = NULL;

        ExceptionClass ex_class;

        switch (m_exception) {
        case EXCEPT_SWI:
            ex_class = EXCL_SYSCALL;
            break;

        case EXCEPT_FIQ:
        case EXCEPT_IRQ:
            ex_class = EXCL_IRQ;
            break;

        case EXCEPT_DABT:
            m_exception_dptr = m_dreq.addr;
        case EXCEPT_PABT:
            m_exception_pc = m_current_pc;
        case EXCEPT_UNDEF:
        default:
            ex_class = EXCL_FAULT;
            break;
        }

        /* gdbserver hook */
        if ( debugExceptionBypassed( ex_class ) )
            return 1;

#if defined(SOCLIB_MODULE_DEBUG)
        std::cout << name() << " exception "
                  << except_info[m_exception].name
                  << std::endl;
#endif

        const struct except_info_s & info = except_info[m_exception];

        /* operating mode switch */
        psr_t new_psr = r_cpsr;
        new_psr.fiq_disabled |= info.disable_fiq;
        new_psr.irq_disabled = true;
        new_psr.mode = info.new_mode;
        new_psr.endian = r_sctlr.ee; 

        cpsr_update(new_psr);

        /* exception link register is relative to current instruction address */
        r_gp[14] = m_current_pc + info.return_offset + r_cpsr.thumb;

        r_gp[15] = info.vector_address;
        r_cpsr.thumb = 0;
        m_exception = EXCEPT_NONE;
    }

    /* next instruction to fetch is pointed to by r15 */
    m_current_pc = r_gp[15];
	m_cycle_count += 1;

    return 1;
}

void ArmIss::reset()
{
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << " reset" << std::endl;
#endif
	for ( size_t i=0; i<16; ++i )
		r_gp[i] = 0;
	for ( size_t i=0; i<3; ++i )
		m_tls_regs[i] = 0;

	r_cpsr.whole = 0x0;
	r_cpsr.fiq_disabled = true;
	r_cpsr.irq_disabled = true;
	r_cpsr.mode = mode_to_psr[MOD_SUPER32];
	m_current_pc = r_gp[15] = ARM_RESET_ADDR;
	m_ins_error = false;
	m_data_error = false;
	m_microcode_func = m_bootstrap_cpu_id < 0 || m_bootstrap_cpu_id == (int)m_ident
                                            ? NULL : &ArmIss::do_sleep;
    m_ldstm_sp_offset = 0;
	m_cycle_count = 0;
	m_run_count = 0;
	m_exception = EXCEPT_NONE;
	m_cache_info.whole = 0;
	m_cache_info.separated = 1;
	DataRequest dreq = ISS_DREQ_INITIALIZER;
	m_dreq = dreq;
	r_bus_mode = Iss2::MODE_KERNEL;
    r_sctlr.whole = 0;
}

static const char * const cond_code[16] = {
	"EQ","NE","CS","CC","MI","PL","VS","VC",
	"HI","LS","GE","LT","GT","LE","AL","NV",
};

static const char * const flag_code[16] = {
	"    ","   V","  C ","  CV",
	" Z  "," Z V"," ZC "," ZCV",
	"-   ","-  V","- C ","- CV",
	"-Z  ","-Z V","-ZC ","-ZCV",
};

static const char *const mode_code[] = {
	"User", "FIQ", "IRQ", "Super", "Abort", "Undef"
};

void ArmIss::dump() const
{
    std::cout
        << m_name
		<< std::hex << std::noshowbase << std::setfill('0')
        << " PC: " << std::setw(8) << m_current_pc;
    if ( r_cpsr.thumb ) {
        int8_t id = thumb_func_main(m_thumb_op.ins);
        std::cout
            << " Thumb: "
            << std::setw(4) << m_thumb_op.ins
            << " [" << flag_code[r_cpsr.flags] << "]"
            << " " << thumb_func_names[id];
    } else {
        int8_t id = arm_func_main(m_opcode.ins);
        std::cout
            << " Arm: "
            << std::setw(8) << m_opcode.ins
            << " [" << flag_code[r_cpsr.flags] << "]"
            << " + " << cond_code[m_opcode.dp.cond]
            << " = ";
        if (!cond_eval())
            std::cout << " NO (";
        std::cout << arm_func_names[id];
        if (!cond_eval())
            std::cout << ")";
    }
	std::cout
		<< std::endl << std::dec
        << " Mode: "   << mode_code[psr_to_mode[r_cpsr.mode]]
        << " IRQ: "   << (r_cpsr.irq_disabled ? "disabled" : "enabled")
        << " FIQ: "   << (r_cpsr.fiq_disabled ? "disabled" : "enabled")
		<< std::endl << std::dec;
    if ( r_cpsr.thumb ) {
        std::cout
            << " N rn: "   << m_thumb_op.reg3.rn
            << "  rd: "    << m_thumb_op.reg3.rd
            << "  rm: "    << m_thumb_op.reg3.rm
            << std::endl
            << " V rn: "   << r_gp[m_thumb_op.reg3.rn]
            << "  rd: "    << r_gp[m_thumb_op.reg3.rd]
            << "  rm: "    << r_gp[m_thumb_op.reg3.rm]
            << std::endl;
    } else {
        std::cout
            << " N rn: "   << m_opcode.dp.rn
            << "  rd: "    << m_opcode.dp.rd
            << "  rm: " << m_opcode.mul.rm
            << std::endl
            << " V rn: "   << r_gp[m_opcode.dp.rn]
            << "  rd: "    << r_gp[m_opcode.dp.rd]
            << "  rm: "    << r_gp[m_opcode.mul.rm]
            << "  shift: " << (m_opcode.ins & 0xfff)
            << std::endl;
    }
    for ( size_t i=0; i<16; ++i ) {
        std::cout
			<< " " << std::dec << std::setw(2) << i << ": "
			<< std::hex << std::noshowbase << std::setw(8) << std::setfill('0')
			<< r_gp[i];
        if ( i%4 == 3 )
            std::cout << std::endl;
    }
}

ArmIss::debug_register_t ArmIss::debugGetRegisterValue(unsigned int reg) const
{
    if ( reg <= 15 )
        return r_gp[reg];

    switch ( reg )
        {
        case 25:
            return r_cpsr.whole;
        case ISS_DEBUG_REG_IS_USERMODE:
            return r_cpsr.mode == MOD_PSR_USER32;
        case ISS_DEBUG_REG_IS_INTERRUPTIBLE:
            return !r_cpsr.irq_disabled || !r_cpsr.fiq_disabled;
        case ISS_DEBUG_REG_STACK_REDZONE_SIZE:
            // sp is pre-decremented during pop
            return m_ldstm_sp_offset;
        default:
            return 0;
        }
}

ArmIss::ArmIss( const std::string &name, uint32_t cpuid )
	: Iss2(name, cpuid)
{
	reset();
}

int ArmIss::m_bootstrap_cpu_id = -1;

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

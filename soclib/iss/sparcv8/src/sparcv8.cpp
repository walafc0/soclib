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
 * Copyright (c) Telecom ParisTech
 *         Alexis Polti <polti@telecom-paristech.fr>
 *
 * Maintainers: Alexis Polti
 *
 * $Id$
 */

#include "sparcv8.h"
#include "base_module.h"
#include "soclib_endian.h"
#include "arithmetics.h"
#include <iomanip>
#include <cassert>
#include <cstring>

namespace soclib { namespace common {

#define tmpl(x) template<unsigned int NWINDOWS> x Sparcv8Iss<NWINDOWS>

tmpl(/**/)::Sparcv8Iss(const std::string &name, uint32_t ident)
    : Iss2(name, ident)
{
    if (NWINDOWS > 32 || NWINDOWS < 2) {
        std::cout << "Error in the soclib sparcv8 component " <<  name << "." << std::endl;
        std::cout << "The windows number must be larger than 2, and no larger than 32." << std::endl;
        exit(1);
    }

    // WIM : unimplemented windows bit are read-only and must read as 0
    r_wim      = 0xffffffff & ~(0xffffffff << NWINDOWS);
}

tmpl(void)::reset()
{
    struct DataRequest null_dreq = ISS_DREQ_INITIALIZER;

    m_ibe                       = false;
    m_dbe                       = false;
    m_dreq.req                  = null_dreq;
    m_dreq.ext_req              = null_dreq;
    m_cancel_next_ins           = false;
    m_ins_delay                 = 0;
    m_hazard                    = false;
    m_wait_irq                  = false;
    m_exception                 = false;
    m_exception_cause           = TP_RESET;

    r_psr.impl  = 0xf;   // Soclib sparcv implementation
    r_psr.ver   = 0x1;   // Implementation version
    r_psr.et    = 1;     // Enable reset trap.
    r_psr.s     = 1;     // Supervisor mode
    r_psr.ec    = 0;     // No co-processor
#if FPU
    r_psr.ef    = 1;     // floating-point unit
#else
    r_psr.ef    = 0;     // No floating-point unit
#endif
    r_psr.pil   = 0xf;   // No ext IRQ enabled
    r_psr.cwp   = 0;     // Current window pointer.

    r_psr_delay = 0;

    // Simulate a reset exception
    r_tbr.whole = 0;
    r_tbr.tba   = DEFAULT_TBA;
    r_tbr.tt    = TP_RESET;
    r_pc        = r_tbr.whole;
    r_npc       = r_pc + 4;
    r_psr.et    = 0;

    std::memset(r_gp, 0, sizeof(r_gp));
    std::memset(r_f, 0, sizeof(r_f));

    r_error_mode = false;
    m_reset_wait_irq = m_bootstrap_cpu_id >= 0 && m_bootstrap_cpu_id != (int)m_ident;
  }

// The processor always asks for an instruction.
// TODO : this is suboptimal, could be optimised out (only ask for an intruction
//        if it is necessary)
tmpl(void)::getRequests( struct InstructionRequest &ireq,
                         struct DataRequest &dreq ) const
{
    ireq.valid = !m_wait_irq && !m_reset_wait_irq;
    ireq.addr = r_pc;
    ireq.mode = r_psr.s ? MODE_KERNEL : MODE_USER;

    if (m_dreq.req.valid)
        dreq = m_dreq.req;
    else if(m_dreq.ext_req.valid)
        dreq = m_dreq.ext_req;
}

tmpl(uint32_t)::executeNCycles( uint32_t ncycle,
                                const struct InstructionResponse &irsp,
                                const struct DataResponse &drsp,
                                uint32_t irq_bit_field )
{
#ifdef SOCLIB_MODULE_DEBUG
    std::cout << m_name << " " << __FUNCTION__
              << '(' << ncycle
              << ", " << irsp
              << ", " << drsp
              << ", " << irq_bit_field
              << ')' << std::endl;
#endif

    if (m_reset_wait_irq && !irq_bit_field)
        return ncycle;
    else
        m_reset_wait_irq = false;

    {
        // Is there a response ?
        m_ireq_pending = !irsp.valid && !m_wait_irq;
        if (irsp.valid) {
            // Swap instruction bytes (sparc is big endian), and store it for later use
            m_ins.ins = soclib::endian::uint32_swap(irsp.instruction);

            // Instruction bus erros are signaled synchronously
            m_ibe = irsp.error;

            // An instruction may use the destination register of a previous LOAD / LDSTUB / SWAP
            // In this case, there is a pipeline hazard, which has to be simulated : we then stall
            // the pipeline.
            // To detect pipeline stall, we use the absolute number of the registers (ie. we take in
            // consideration the CWP), as the CWP may change between the LOAD and the faulty instruction.
            // To be honnest, with the current SocLib implementation, this is not strictly necessary,
            //  but things may change, and it doesn't cost much. So let's do it the most versatile way.
            uint32_t dest_reg = get_absolute_dest_reg(m_ins);
            m_hazard = ((dest_reg == m_dreq.dest_reg) && m_dreq.req.valid)
                || ((dest_reg == m_dreq.ext_dest_reg) && m_dreq.ext_req.valid);
        }
    }

    setData(drsp);


    // At each cycle we execute an instruction and the data transfert for the preceding instruction.
    // If either following condition is true :
    //    - no instruction response (m_ireq_pending),
    //    - a data request pending and still no data reponse (m_dreq_pending == true)
    //    - we are in the middle of a multicycle instruction,
    // then do nothing and advance time as far as possible :
    //    - if we are not in the middle of a long instruction : ncycle
    //    - if we are in the middle of a long instruction : to the end of current instruction
    if (m_ireq_pending || m_dreq_pending || m_ins_delay || ncycle == 0) {
      uint32_t t = ncycle;
      if (m_ins_delay) {
        if (m_ins_delay < ncycle)
          t = m_ins_delay;
        m_ins_delay -= t;
      }

#ifdef SOCLIB_MODULE_DEBUG
      std::cout << name() << " frozen :"
                << " m_ireq_pending=" << m_ireq_pending
                << " mdreq_pending=" << m_dreq_pending
                << " m_ins_delay=" << m_ins_delay
                << std::endl;
#endif
      return t;
    }

    // Pipeline is 2 deep. So hazard only last 2 cycles.
    // If there is a hazard and we were asked to execute two cycles or more, then
    // we pretend that the first cycle was executed, and that we now do the second with
    // no hazard anymore.
    // If there is a hazard and we were asked to execute only one cycle, then continue (to
    // check for exceptions), but we don't clear the hazard flag : the instruction will
    // not be executed yet.
    if ( m_hazard && ncycle > 1 ) {
      ncycle = 2;
      m_hazard = false;
    }
    else {
      ncycle = 1;
    }

    if (r_psr_delay) {
        if (!--r_psr_delay)
            r_psr.whole = r_psr_write.whole;
    }

    // Initialize future value of npc.
    // In case of jump, the instruction will modify it.
    m_next_pc = r_npc+4;
    m_cancel_next_ins = false;

    // If there is an instruction bus error, then handle it !
    if (m_ibe) {
      m_exception = true;
      m_exception_cause = TP_INSTRUCTION_ACCESS_ERROR;
      goto check_except;
    }

    // Instruction bus exceptions are set / reset synchronously at each cycle by wrapper.
    // Data bus exceptions are only SET by wrapper (and maybe asynchronously).
    // So we need to record that an exception has occured, then RESET the flip flop ourselves.
    if ( m_dbe ) {
      m_exception = true;
      m_exception_cause = TP_DATA_ACCESS_ERROR;
      m_dbe = false;
      goto check_except;
    }

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << m_name << std::endl;
    dump_pc("Before instruction execution :");
    dump_regs("");
#endif

    // If there is no hazard, the execute the current instruction.
    // If there is one, then don't do anything !
    if ( m_hazard ) {
#ifdef SOCLIB_MODULE_DEBUG
      std::cout << name() << " hazard, seeing next cycle" << std::endl;
#endif
      goto house_keeping;
    }

    // Let's now handle external interrupts if there is any
    if (r_psr.et && ((irq_bit_field == 0xf) || (irq_bit_field > r_psr.pil))) {
      m_exception = true;
      m_exception_cause = TP_INTERRUPT_LEVEL(irq_bit_field);
#ifdef SOCLIB_MODULE_DEBUG
      std::cout << name() << " Taking irqs " << irq_bit_field << std::endl;
#endif
      goto check_except;
    } else {
#ifdef SOCLIB_MODULE_DEBUG
      if (irq_bit_field)
        std::cout << name() << " Ignoring irqs " << irq_bit_field << std::endl;
#endif
    }

    if (m_wait_irq)
        return ncycle;

    // Execute currrent instruction
    run();

 check_except:

    // Let's handle the case where the current instruction caused an exception
    {
        ExceptionClass ex_class = EXCL_FAULT;
        ExceptionCause ex_cause = EXCA_OTHER;

        switch (m_exception_cause) {

        case TP_INSTRUCTION_ACCESS_ERROR:
        case TP_INSTRUCTION_ACCESS_EXCEPTION:
        case TP_DATA_ACCESS_ERROR:
        case TP_DATA_ACCESS_EXCEPTION:
            ex_cause = EXCA_BADADDR;
            break;

        case TP_ILLEGAL_INSTRUCTION:
        case TP_PRIVILEGED_INSTRUCTION:
        case TP_UNIMPLEMENTED_INSTRUCTION:
            ex_cause = EXCA_ILL;
            break;

        case TP_DIVISION_BY_ZERO:
        case TP_FP_EXCEPTION:
        case TP_FP_DISABLED:
        case TP_CP_DISABLED:
        case TP_TAG_OVERFLOW:
            ex_cause = EXCA_FPU;
            break;

        case TP_MEM_ADDRESS_NOT_ALIGNED:
            ex_cause = EXCA_ALIGN;
            break;

        case TP_INTERRUPT_LEVEL(0) ... TP_INTERRUPT_LEVEL(0xf):
        case TP_RESET :
            ex_class = EXCL_IRQ;
            break;

        case TP_TRAP_INSTRUCTION(0) ... TP_TRAP_INSTRUCTION(0x7f):
            ex_class = EXCL_TRAP;
            break;

        case TP_INSTRUCTION_ACCESS_MMU_MISS:
        case TP_DATA_ACCESS_MMU_MISS:
            ex_cause = EXCA_PAGEFAULT;
            break;

        case TP_WINDOW_OVERFLOW:
        case TP_WINDOW_UNDERFLOW:
            ex_cause = EXCA_REGWINDOW;
            break;

        default:
            ;
        }
        // Let's now handle traps... or let GDB handle them for us :)
        if (m_exception && debugExceptionBypassed( ex_class, ex_cause )) {
                m_exception = false;
        }
    }

    if (!m_exception)
      goto no_except;

    if (r_psr_delay) {
        r_psr.whole = r_psr_write.whole;
        r_psr_delay = 0;
    }

    {
      m_wait_irq = false;
      m_exception = false;

      // Double exception ?
      if (r_psr.et == 0) {
        std::cout
          << m_name
          << " A precise trap (" << GetTrapName(m_exception_cause) << ")"
          << " occured at pc = " << std::setw(10) << std::setfill('0') << std::internal << std::hex << std::showbase << r_pc
          << " and the Enable Trap flag is not set." << std::endl
          << " Entering Error Mode and rebooting."
          << std::endl;

        if ( debugExceptionBypassed( EXCL_FAULT, EXCA_OTHER ) )
            goto no_except;

          r_error_mode = true;

        // Reset processor
        m_exception_cause = TP_RESET;
      }
      else {
        // Disable traps
        r_psr.et = 0;

        // Save processor state
        r_psr.ps = r_psr.s;

        // Enter supervisor mode
        r_psr.s = 1;

        // Allocate a new window
        r_psr.cwp = (r_psr.cwp - 1) % NWINDOWS;

        // Save PC and NPC.
        // A synchronous DBE is related to the *previous* instruction. So in this case, we save the
        // previous PC, hopping that the trap handler will have a mean to redo the faulty instruction.
        // If the DBE was asynchronous, we're lost. This is the best we can do...
        if (m_exception_cause == TP_DATA_ACCESS_ERROR) {
          GPR(17) = r_prev_pc;
          GPR(18) = r_pc;
        }
        else {
          GPR(17) = r_pc;
          GPR(18) = r_npc;
        }
      }

      // Save Trap type
      r_tbr.tt = m_exception_cause;

      // Branch to trap handler location
      if (m_exception_cause == TP_RESET)
        m_next_pc = 0;
      else
        m_next_pc = r_tbr.whole;

#ifdef SOCLIB_MODULE_DEBUG
      std::cout
        << m_name
        << " exception: "
        << std::setw(10) << std::setfill('0') << std::internal << std::hex << std::showbase << ((unsigned long)m_exception_cause)
        << " (" << GetTrapName(m_exception_cause) << ")"
        << std::endl
        << " tbr: " << r_tbr.whole
        << " pc: " << r_pc
        << " npc: " << r_npc
#if FPU
        << " r_fsr: " << r_fsr.whole
        << " (RD=" << r_fsr.rd
        << " TEM=" << r_fsr.tem
        << " NS=" << r_fsr.ns
        << " ftt=" << r_fsr.ftt
        << " qne=" << r_fsr.qne
        << " fcc=" << r_fsr.fcc
        << " aexc=" << r_fsr.aexc
        << " cexc=" << r_fsr.cexc
        << ")"
#endif
        << std::endl;
#endif
      m_cancel_next_ins = true;
    }

  no_except:
    r_prev_pc = r_pc;
    if (m_cancel_next_ins) {
      r_pc = m_next_pc;
      r_npc = m_next_pc+4;
      m_cancel_next_ins = false;
    }
    else {
      r_pc = r_npc;
      r_npc = m_next_pc;
    }

  house_keeping:
    // Keep g0 always null
    r_gp[0] = 0;

#ifdef SOCLIB_MODULE_DEBUG
    dump_pc("After execution : ");
    dump_regs("");
#endif

    return ncycle;
}

tmpl()::~Sparcv8Iss()
{
}

tmpl(int)::m_bootstrap_cpu_id = -1;

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

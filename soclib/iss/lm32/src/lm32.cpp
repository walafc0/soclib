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
 * Copyright (c) TelecomParisTECH
 *         Tarik Graba <tarik.graba@telecom-paristech.fr>, 2009
 *
 * Based on sparcv8 and mips32 code
 *         Alexis Polti <polti@telecom-paristech.fr>, 2008
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *         Alain Greiner <alain.greiner@lip6.fr>, 2007
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 *
 * $Id$
 *
 * History:
 * - 2011-02-10
 *   Tarik Graba : The instructions issu delay are grouped in the opcode table
 * - 2010-04-16
 *   Tarik Graba : Added a template parameter to specify the endianess
 * - 2009-07-08
 *   Tarik Graba : the iss is now sensitive to high level irqs
 * - 2009-02-15
 *   Tarik Graba : Forked mips32 and sparcv8 to begin lm32
 */

#include "lm32.h"

// TODO implement Break Points and Watch Points


namespace soclib { namespace common {

#define tmpl(x) template<bool lEndianInterface> x  LM32Iss<lEndianInterface>

    tmpl(/**/)::LM32Iss(const std::string &name, uint32_t ident)
        :Iss2(name,ident)
    {
        r_CFG.M = 1;       // multiplier
        r_CFG.D = 1;       // divider
        r_CFG.S = 1;       // barrel shifter
        r_CFG.U = 0;       // User defined instructions
        r_CFG.X = 1;       // sign extension
        r_CFG.CC = 1;      // cycle counter
        r_CFG.IC = 1;      // inst. cache
        r_CFG.DC = 1;      // data  cache
        r_CFG.G = 0;       // debug
        r_CFG.H = 0;       // H/W debug
        r_CFG.R = 0;       // ROM debug
        r_CFG.J = 0;       // JTAG uart
        r_CFG.INT = 32;    // number of interupt (0-32)
        r_CFG.BP = 4;      // number of break points (0-4)
        r_CFG.WP = 4;      // number of watch points (0-4)
        r_CFG.REV = 63;    // Processor Rev. number (0-63) 
        //TODO define soclib's rev. number
    }

    tmpl(void)::reset()
    {
        struct DataRequest null_dreq = ISS_DREQ_INITIALIZER;

        r_pc = RESET_ADDRESS;
        r_npc = RESET_ADDRESS + 4;

        m_ibe =             false;
        m_dbe =             false;
        m_dreq.req =        null_dreq;
        m_ins_delay =       0;
        m_hazard =          false;
        m_exception =       false;
        m_exception_cause = X_RESET;

        // The REG32_BITFIELD is a packed struct aligned to an uint32_t named "whole"
        r_IE.whole = 0; // interrupt enable
        r_IM = 0; // interrupt mask
        r_IP = 0; // interrupt pending
        r_CC = 0; // cycle counter
        r_EBA = RESET_ADDRESS; // Exception base
        r_DC.whole = 0; // debug control
        r_DEBA = DEBA_RESET;   // Debug Exception base
        for (unsigned int i = 0; i<4; i++) { 
            r_BP[i].whole = 0; // break points
            r_WP[i] = 0; // watch points
        }

        // GP registers 
        // The LM32 doc says that the gp register are not initialised
        // for(unsigned int i = 0; i<32; i++) r_gp[i] = 0;
    }

    /*
     * CABA :
     *   m_iss.getRequest( ireq, dreq );
     *   m_iss.executeNCycles(1, iresp, dresp, it);
     *
     */

    // get instruction and data requests from iss
    tmpl(void)::getRequests( 
            struct InstructionRequest &ireq,
            struct DataRequest &dreq 
            ) const
    {
        struct DataRequest null_dreq = ISS_DREQ_INITIALIZER;

        ireq.valid = true; // always request the next instruction
        ireq.addr = r_pc;
        ireq.mode = MODE_USER; // LM32 doesnt have previlegied modes

        if (m_dreq.req.valid)
            dreq = m_dreq.req;
        else dreq = null_dreq;
    } //getRequest


    // Execute N cycles given instruction, data and irq
    tmpl(uint32_t)::executeNCycles( 
            uint32_t ncycle, 
            const struct InstructionResponse &irsp,
            const struct DataResponse &drsp,
            uint32_t irq_bit_field 
            ) 
    {

        // ret instruction and data responses
        _setInstruction( irsp );
        _setData ( drsp );

        // Interrupt pending register
        r_IP = r_IP | (r_IM & irq_bit_field);

        // if instruction or data request is pending
        // or if we are in the time slot of a long instruction
        if (m_ireq_pending || m_dreq_pending || m_ins_delay) {
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
            r_CC = r_CC + t;
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
#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " Hazard detected!"
                << std::endl;
#endif
        }
        else {
            ncycle = 1;
        }
        // Initialize future value of npc.
        // In case of jump, the instruction will modify it.
        m_next_pc = r_pc + 4;

#undef LM32_HARDWARE_BP
#ifdef LM32_HARDWARE_BP
        // Check if we reached a hardware break points
        for (unsigned int i = 0; i<4;i++) {
            if (r_BP[i].E && ((r_pc >> 2) == r_BP[i].A)) {
#ifdef SOCLIB_MODULE_DEBUG
                std::cout
                    << name()
                    << " Hardware breakpoint "
                    << i
                    <<" reached"
                    << std::endl;
#endif
                m_exception = true;
                m_exception_cause = X_BREAK_POINT;
                goto handle_except;
            }
        }
#endif // LM32_HARDWARE_BP

        // If there is an instruction bus error, then handle it !
        if (m_ibe) {
            m_exception = true;
            m_exception_cause = X_INST_BUS_ERROR;
            goto handle_except;
        }

        // Instruction bus exceptions are set / reset synchronously at each cycle by wrapper.
        // Data bus exceptions are only SET by wrapper (and maybe asynchronously).
        // So we need to record that an exception has occured, then RESET the flip flop ourselves.
        if ( m_dbe ) {
            m_exception = true;
            m_exception_cause = X_DATA_BUS_ERROR;
            m_dbe = false;
            goto handle_except;
        }

#ifdef SOCLIB_MODULE_DEBUG
        std::cout << m_name << std::endl;
        dump_pc("Before instruction execution :");
        dump_regs("");
        std::cout << "IRQ Bitfield : " << irq_bit_field << std::endl;
        std::cout << "IE : " << r_IE.whole << std::endl;
        std::cout << "IP : " << r_IP << std::endl;
        std::cout << "IM : " << r_IM << std::endl;
#endif

        // Let's now handle external interrupts.
        // we do this before running instructions to avoid exec hazards with loads/stores
        // bits of r_IM set to '1' means the corresponding interrupt is allowed
        if (r_IE.IE && (r_IP & r_IM)){ 
            m_exception = true;      
            m_exception_cause = X_INTERRUPT ;
#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " Taking irqs " << std::hex << irq_bit_field << std::endl 
                << "Interrupt enable bit : " << r_IE.IE << std::endl
                << "Interrupt mask : " << r_IM
                << std::endl;
#endif
            goto handle_except; 
#ifdef SOCLIB_MODULE_DEBUG
        } else {
            if ( irq_bit_field )
                std::cout << name() << " Ignoring irqs " << std::hex << irq_bit_field << std::endl
                    << "Interrupt enable bit : " << r_IE.IE << std::endl
                    << "Interrupt mask : " << r_IM
                    << std::endl;
#endif
        }

        // If there is no hazard, the execute the current instruction.
        // If there is one, then don't do anything !
        if ( m_hazard ) {
#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " hazard, seeing next cycle" << std::endl;
#endif
            goto house_keeping;
        }
        else {
            run();
        } 

handle_except:
        {
            ExceptionClass ex_class = EXCL_FAULT;
            ExceptionCause ex_cause = EXCA_OTHER;

            switch (m_exception_cause) {

                case  X_RESET             :
                case  X_INTERRUPT         :
                    ex_class = EXCL_IRQ;
                    break;

                case  X_WATCH_POINT       :
                case  X_BREAK_POINT       :
                    ex_class = EXCL_TRAP;
                    break;

                case  X_DATA_BUS_ERROR    :
                case  X_INST_BUS_ERROR    :
                    ex_cause = EXCA_BADADDR;
                    break;

                case  X_DIVISION_BY_ZERO  :
                    ex_cause = EXCA_DIVBYZERO;
                    break;

                case  X_SYSTEM_CALL       :
                    ex_class = EXCL_SYSCALL;
                    break;

                default:  // unkown exception!!
                    ;
            }

            // Let's now handle traps... or let GDB handle them for us :)
            if (m_exception && debugExceptionBypassed( ex_class, ex_cause ))
                m_exception = false;
        }

        if (!m_exception)
            goto no_except;

        {
            m_exception = false;
            switch ( m_exception_cause ) {
                // debug exceptions
                case ( X_BREAK_POINT):
                case ( X_WATCH_POINT):
                    r_gp[ba] = r_pc;
                    m_next_pc = r_DEBA + 8*4*m_exception_cause;
                    //disable interrupts and store interrupt enable
                    r_IE.BIE = r_IE.IE ;
                    r_IE.IE  = 0;
                    break;
                    // exceptions
                case ( X_RESET             ):
                case ( X_INST_BUS_ERROR    ):
                case ( X_DATA_BUS_ERROR    ):
                case ( X_DIVISION_BY_ZERO  ):
                case ( X_INTERRUPT         ):
                case ( X_SYSTEM_CALL       ):
                    r_gp[ea] = r_pc;
                    m_next_pc = (r_DC.RE ? r_DEBA :r_EBA )+8*4*m_exception_cause;
                    //disable interrupts and store interrupt enable
                    r_IE.EIE = r_IE.IE ;
                    r_IE.IE  = 0;
                    break;

            }
#ifdef SOCLIB_MODULE_DEBUG
            std::cout
                << name() << " exception: " << GetExceptioName(m_exception_cause) << ", "
                << std::setw(10) << std::setfill('0') << std::internal << std::hex << std::showbase 
                << " pc: "  << r_pc
                << " npc: " << r_npc
                << " next_pc: " << m_next_pc
                << std::endl;
#endif

        }
no_except:
        r_pc  = m_next_pc;
        r_npc = m_next_pc+4;

        // hazard refers to simulation hazard, not functionnal hazard
        // Thus, cycle counter should not be incremented
        r_CC = r_CC + ncycle;
house_keeping:
        // Keep g0 always null
        //     r_gp[0] = 0;

#ifdef SOCLIB_MODULE_DEBUG
        dump_pc("After execution : ");
        dump_regs("");
        std::cout << "IP : " << r_IP << std::endl;
        std::cout << "IM : " << r_IM << std::endl;
        std::cout << "CC : " << r_CC << std::endl;
#endif

        return ncycle;

    } // executeNcycles

#undef tmpl

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


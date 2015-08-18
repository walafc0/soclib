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
 *    Nicolas Pouillon <nipo@ssji.net>, 2007
 *    Alain Greiner <alain.greiner@lip6.fr>, 2007
 *
 * Maintainers: nipo
 *
 * $Id$
 *
 * History:
 * - 2007-06-15
 *     Nicolas Pouillon, Alain Greiner: Model created
 */

#include "mips.h"
#include "soclib_endian.h"
#include "arithmetics.h"

namespace soclib { namespace common {

static const uint32_t NO_EXCEPTION = (uint32_t)-1;

namespace {

static inline std::string mkname(uint32_t no)
{
    char tmp[32];
    snprintf(tmp, 32, "mips_iss%d", (int)no);
    return std::string(tmp);
}

}

MipsIss::MipsIss(uint32_t ident, bool mips_little_endian)
    : Iss(mkname(ident), ident),
      m_little_endian(mips_little_endian)
{
    m_config.whole = 0;
//    m_config.m = 1;
    m_config.be = m_little_endian ? 0 : 1;
}

void MipsIss::reset()
{
    r_pc = RESET_ADDRESS;
    r_npc = RESET_ADDRESS + 4;
    r_ppc = RESET_ADDRESS - 4; // just for init
    r_branch = false;
    r_branch_delay = false;
    m_ibe = false;
    m_dbe = false;
    r_mem_req = false;
    r_mem_unsigned = false;
    r_mem_addr = 0;
    r_mem_wdata = 0;
    r_mem_dest = 0;
    r_mem_shift = 0;
    m_ins_delay = 0;
    m_ins_delay = 0;
    r_status.whole = 0;
    r_cause.whole = 0;
    m_rs = m_rt = 0;
    r_gp[0] = 0;
    r_count = 0;

    for(int i = 0; i<32; i++)
        r_gp[i] = 0;

    m_hazard=false;
    m_irq=0;
    m_exception = NO_EXCEPTION;
}

void MipsIss::dump() const
{
    std::cout
        << std::hex << std::showbase
        << m_name
        << " PC: " << r_pc
        << " Ins: " << m_ins.ins << std::endl
        << std::dec
        << " Cause.xcode: " << r_cause.xcode << std::endl
        << " Status.ie " << r_status.iec
        << " .im " << r_status.im
        << " .um " << r_status.kuc << std::endl
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
        << " V rs: " << m_rs
        << " rt: "<<m_rt
        << " delayed slot: " << std::boolalpha << r_branch_delay
        << std::endl;
    for ( size_t i=0; i<32; ++i ) {
        std::cout << " " << std::dec << i << ": " << std::hex << std::showbase << r_gp[i];
        if ( i%8 == 7 )
            std::cout << std::endl;
    }
}

void MipsIss::setDataResponse(bool error, uint32_t data)
{
    m_dbe = error;
    r_mem_req = false;
    if ( error ) {
        return;
    }

    // We write the  r_gp[i], and we detect a possible data dependency,
    // in order to implement the delayed load behaviour.
    if ( isReadAccess(r_mem_type) ) {
        uint32_t reg_use = curInstructionUsesRegs();
        if ( (reg_use & USE_S && r_mem_dest == m_ins.r.rs) ||
             (reg_use & USE_T && r_mem_dest == m_ins.r.rt) )
            m_hazard = true;
#ifdef SOCLIB_MODULE_DEBUG
        std::cout
            << m_name
            << " read to " << r_mem_dest
            << "(" << dataAccessTypeName(r_mem_type) << ")"
            << " from " << std::hex << r_mem_addr
            << ": " << data
            << " hazard: " << m_hazard
            << std::endl;
#endif
    }
#ifdef SOCLIB_MODULE_DEBUG
    if ( isWriteAccess(r_mem_type) )
        std::cout
            << m_name
            << " write "
            << "(" << dataAccessTypeName(r_mem_type) << ")"
            << " to " << std::hex << r_mem_addr
            << " OK"
            << std::endl;
#endif

    switch (r_mem_type) {
    case WRITE_BYTE:
    case WRITE_WORD:
    case WRITE_HALF:
    case LINE_INVAL:
        m_hazard = false;
        break;
    case READ_WORD:
        if ( r_mem_shift ) {
            uint32_t mask = (uint32_t)-1;

            if ( r_mem_shift < 0 ) {
                int shift = -r_mem_shift;
                mask >>= (8*shift);
                data >>= (8*shift);
            } else {
                mask <<= (8*r_mem_shift);
                data <<= (8*r_mem_shift);
            }
            data |= r_gp[r_mem_dest]&~mask;
        }
        r_gp[r_mem_dest] = data;
        break;
    case READ_LINKED:
        r_gp[r_mem_dest] = data;
        break;
    case STORE_COND:
        r_gp[r_mem_dest] = !data;
        break;
    case READ_BYTE:
        r_gp[r_mem_dest] = r_mem_unsigned ?
            (data & 0xff) :
            sign_ext(data, 8);
        break;
    case READ_HALF:
        r_gp[r_mem_dest] = r_mem_unsigned ?
            (data & 0xffff) :
            sign_ext(data, 16);
        break;
    }
}

void MipsIss::step()
{
    ++r_count;

    // The current instruction is executed in case of interrupt, but
    // the next instruction will be delayed.
    // The current instruction is not executed in case of exception,
    // and there is three types of bus error events,
    // 1 - instruction bus errors are synchronous events, signaled in
    // the m_ibe variable
    // 2 - read data bus errors are synchronous events, signaled in
    // the m_dbe variable
    // 3 - write data bus errors are asynchonous events signaled in
    // the m_dbe flip-flop
    // Instuction bus errors are related to the current instruction:
    // lowest priority.
    // Read Data bus errors are related to the previous instruction:
    // intermediatz priority
    // Write Data bus error are related to an older instruction:
    // highest priority

    m_next_pc = r_npc+4;
    m_exception = NO_EXCEPTION;

    if (m_ibe) {
        r_branch_delay = r_branch;
        m_exception = X_IBE;
        m_ibe = false;
        goto handle_except;
    }

    if ( m_dbe ) {
        m_exception = X_DBE;
        m_dbe = false;
        r_bar = r_mem_addr;
        goto handle_except;
    }

    r_branch_delay = r_branch;

#ifdef SOCLIB_MODULE_DEBUG
    dump();
#endif
    // run() can modify the following registers: r_gp[i], r_mem_req,
    // r_mem_type, r_mem_addr; r_mem_wdata, r_mem_dest, r_hi, r_lo,
    // m_exception, m_next_pc
    if ( m_hazard ) {
        m_hazard = false;
        goto house_keeping;
    } else {
        run();
        r_branch = curInstructionIsBranch();
    }

    if ( m_exception != NO_EXCEPTION )
        goto handle_except;

    if ( m_irq
         && r_status.im & m_irq
         && r_status.iec ) {
        m_exception = X_INT;
        goto handle_except;
    }
    
    goto no_except;

 handle_except:

    if ( exceptionBypassed( m_exception ) )
        goto stick;

    {
        r_cause.xcode = m_exception;
        r_cause.irq  = m_irq;
        r_cause.bd = r_branch_delay;
        r_status.kuo = r_status.kup;
        r_status.ieo = r_status.iep;
        r_status.kup = r_status.kuc;
        r_status.iep = r_status.iec;
        r_status.kuc = 0;
        r_status.iec = 0;

        /////////////////////////////////////////////////////////////////////
        // EPC Affectation :                                               //
        //                                                                 //
        // i)  System call, Break-Point or Non-masked interrupts           //
        //        EPC <- PC + 4  : ( the return adress )                   //
        //                                                                 //
        // ii) Others exceptions                                           //
        //        The general rule                                         //
        //             EPC <- PC : ( the faulty instruction )              //
        //                                                                 //
        //        Special case : the current instruction is a delayed slot //
        //             EPC <- PC - 4 : ( the branch instruction )          //
        /////////////////////////////////////////////////////////////////////

        switch(m_exception)
        {
          case X_SYS:
          case X_BP:
              r_epc = r_npc;
              break;
          case X_INT:
              r_epc = (r_branch) ? r_pc : r_npc;
              break;
          case X_DBE:
              r_epc = (r_branch_delay) ? r_ppc - 4 : r_ppc;
              break;
          default:
              r_epc = (r_branch_delay) ? r_pc - 4 : r_pc; 
        }

#ifdef SOCLIB_MODULE_DEBUG
        std::cout
            << m_name <<" exception: "<<m_exception<<std::endl
            << " epc: " << r_epc
            << " bar: " << r_mem_addr
            << " cause.xcode: " << r_cause.xcode
            << " .bd: " << r_cause.bd
            << " .irq: " << r_cause.irq
            << std::endl;
#endif
    }
    r_pc = EXCEPT_ADDRESS;
    r_npc = EXCEPT_ADDRESS + 4;
    goto house_keeping;
 no_except:
    r_ppc = r_pc;
    r_pc = r_npc;
    r_npc = m_next_pc;
 house_keeping:
 stick:
    r_gp[0] = 0;
}

int MipsIss::cpuCauseToSignal( uint32_t cause ) const
{
    switch (cause) {
    case X_INT:
        return 2; // Interrupt
    case X_MOD:
    case X_TLBL:
    case X_TLBS:
        return 5; // Trap (nothing better)
    case X_ADEL:
    case X_ADES:
    case X_IBE:
    case X_DBE:
        return 11; // SEGV
    case X_SYS:
    case X_BP:
    case X_TR:
    case X_reserved:
        return 5; // Trap/breakpoint
    case X_RI:
    case X_CPU:
        return 4; // Illegal instruction
    case X_OV:
    case X_FPE:
        return 8; // Floating point exception
    };
    return 5;       // GDB SIGTRAP                                                                                                                                                                
}

uint32_t MipsIss::getDebugRegisterValue(unsigned int reg) const
{
    switch (reg)
        {
        case 0:
            return 0;
        case 1 ... 31:
            return soclib::endian::uint32_swap(r_gp[reg]);
        case 32:
            return soclib::endian::uint32_swap(r_status.whole);
        case 33:
            return soclib::endian::uint32_swap(r_lo);
        case 34:
            return soclib::endian::uint32_swap(r_hi);
        case 35:
            return soclib::endian::uint32_swap(r_bar);
        case 36:
            return soclib::endian::uint32_swap(r_cause.whole);
        case 37:
            return soclib::endian::uint32_swap(r_pc);
        default:
            return 0;
        }
}

void MipsIss::setDebugRegisterValue(unsigned int reg, uint32_t value)
{
    value = soclib::endian::uint32_swap(value);

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
        default:
            break;
        }
}

void MipsEbIss::setDataResponse(bool error, uint32_t data)
{
    MipsIss::setDataResponse(error, soclib::endian::uint32_swap(data));
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

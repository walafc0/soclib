/* -*- c++ -*-
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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 *
 * $Id$
 */

#include "ppc405.h"
#include "soclib_endian.h"
#include "arithmetics.h"
#include <iostream>
#include <iomanip>

namespace soclib { namespace common {

static const uint32_t NO_EXCEPTION = (uint32_t)-1;

namespace {

static inline uint32_t align( uint32_t data, int shift, int width )
{
	uint32_t mask = (1<<width)-1;
	uint32_t ret = data >>= shift*width;
	return ret & mask;
}

static inline std::string crTrad( uint32_t cr )
{
    const char *orig = "o=><";
    char dest[5] = "    ";
    
    for ( size_t i=0; i<4; ++i )
        if ( cr & (1<<i) )
            dest[i] = orig[i];

    return dest;
}

}

Ppc405Iss::Ppc405Iss(const std::string &name, uint32_t ident)
	: Iss2(name, ident)
{
    reset();
}

void Ppc405Iss::reset()
{
    struct DataRequest init_dreq = ISS_DREQ_INITIALIZER;

    m_microcode_func = NULL;
    m_exception = EXCEPT_NONE;
    m_dreq = init_dreq;
    m_exec_cycles = 0;
    m_ins_delay = 0;
    r_pc = RESET_ADDR;
    r_dbe = false;
    m_ibe = false;
    m_dbe = false;
    r_evpr = 0xdead0000;
    r_tb = 0;
    r_esr = 0;
    r_msr.whole = 0;
    for ( size_t i=0; i<DCR_MAX; ++i )
        r_dcr[i] = 0;
    for ( size_t i=0; i<32; ++i )
        r_gp[i] = 0;
    r_dcr[DCR_PROCNUM] = m_ident;
    r_dcr[DCR_EXEC_CYCLES] = 0;

    m_reset_wait_irq = m_bootstrap_cpu_id >= 0 && m_bootstrap_cpu_id != (int)m_ident;
}

void Ppc405Iss::dump() const
{
    std::cout
        << m_name << std::hex
        << " PC: " << r_pc
        << " Ins: " << m_ins.ins
        << " lr: " << r_lr
        << " msr " << r_msr.whole
        << " .ce " << r_msr.ce
        << " .ee " << r_msr.ee
        << " .pr " << r_msr.pr
        << std::endl;
    for ( size_t i=0; i<32; ++i ) {
        std::cout
			<< " " << std::dec << std::setw(2) << i << ": "
			<< std::hex << std::noshowbase << std::setw(8) << std::setfill('0')
			<< r_gp[i];
        if ( i%8 == 7 )
            std::cout << std::endl;
    }
    for ( size_t i=0; i<8; ++i ) {
        std::cout << " " << std::dec << i << ": " << crTrad(crGet(i));
    }
    std::cout
        << " ctr: " << std::hex << r_ctr
        << std::endl;
    std::cout << std::endl;
}

uint32_t Ppc405Iss::executeNCycles(
    uint32_t ncycle,
    const struct Iss2::InstructionResponse &irsp,
    const struct Iss2::DataResponse &drsp,
    uint32_t irq_bit_field )
{
    if (m_reset_wait_irq && !irq_bit_field)
        return ncycle;
    else
        m_reset_wait_irq = false;

    if ( drsp.valid )
        setDataResponse(drsp);

    if ( ( m_microcode_func == NULL && !irsp.valid && !r_msr.we)
         || ( m_dreq.valid && ! drsp.valid ) ) {
        if ( m_ins_delay ) {
            if ( m_ins_delay > ncycle )
                m_ins_delay -= ncycle;
            else
                m_ins_delay = 0;
        }
        r_tb += ncycle;
        return ncycle;
    }

    if ( irsp.valid ) {
        m_ibe = irsp.error;
        m_ins.ins = soclib::endian::uint32_swap(irsp.instruction);
    }

    if ( ncycle == 0 )
        return 0;

    r_tb++;

    m_next_pc = r_pc+4;

    m_exception = EXCEPT_NONE;

    if (m_ibe) {
        m_exception = EXCEPT_INSTRUCTION_STORAGE;
        m_ibe = false;
        r_esr = ESR_DIZ;
        goto handle_except;
    }

    if ( m_dbe ) {
        m_exception = EXCEPT_MACHINE_CHECK;
        m_dbe = false;
        r_esr = ESR_MCI;
        goto handle_except;
    }

    if ( r_dbe ) {
        m_exception = EXCEPT_DATA_STORAGE;
        r_dbe = false;
        r_esr = ESR_DST;
        r_dear = m_dreq.addr;
        goto handle_except;
    }

    r_dcr[DCR_EXEC_CYCLES]++;
#ifdef SOCLIB_MODULE_DEBUG
    dump();
#endif

    if ( m_microcode_func ) {
        m_next_pc = r_pc;
        (this->*m_microcode_func)();

    } else {

        // IRQs
        if ( irq_bit_field&(1<<IRQ_CRITICAL_INPUT) ) {
            r_dcr[DCR_CRITICAL] = true;
            if ( r_msr.ce ) {
                m_next_pc = r_pc;
                m_exception = EXCEPT_CRITICAL;
                goto handle_except;
            }
        }

        if ( irq_bit_field&(1<<IRQ_EXTERNAL) ) {
            r_dcr[DCR_EXTERNAL] = true;
            if ( r_msr.ee ) {
                m_next_pc = r_pc;
                m_exception = EXCEPT_EXTERNAL;
                goto handle_except;
            }
        }

        if ( r_msr.we )
            return 1;

        run();
    }

    if (m_exception == EXCEPT_NONE)
        goto no_except;

  handle_except:
#ifdef SOCLIB_MODULE_DEBUG
    std::cout << m_name << " except: " << m_exception << std::endl;
#endif

    m_microcode_func = NULL;
    r_msr.we = 0;

    {
        ExceptionClass ex_class = EXCL_FAULT;
        ExceptionCause ex_cause = EXCA_OTHER;

        switch (m_exception)
            {
            case EXCEPT_PI_TIMER:
            case EXCEPT_FI_TIMER:
            case EXCEPT_CRITICAL:
            case EXCEPT_EXTERNAL:
                ex_class = EXCL_IRQ;
                break;
            case EXCEPT_SYSCALL:
                ex_class = EXCL_SYSCALL;
                break;
            case EXCEPT_DEBUG:
                ex_class = EXCL_TRAP;
                break;
            
            case EXCEPT_PROGRAM:
                ex_cause = EXCA_ILL;
                break;
            case EXCEPT_ALIGNMENT:
                ex_cause = EXCA_ALIGN;
                break;
            case EXCEPT_INSTRUCTION_TLB_MISS:
            case EXCEPT_DATA_TLB_MISS:
                ex_cause = EXCA_PAGEFAULT;
                break;
 
            default:
                ;
           }

        if (debugExceptionBypassed( ex_class, ex_cause ))
            goto no_except;
    }

    // 1/2 : Save status to SRR
    {
        int except_base = 0;
        uint32_t ra = r_pc;
        switch (m_exception) {
        case EXCEPT_SYSCALL:
        case EXCEPT_EXTERNAL:
            ra = m_next_pc;
            break;
        case EXCEPT_CRITICAL:
            except_base = 2;
            ra = m_next_pc;
            break;
        case EXCEPT_WATCHDOG:
        case EXCEPT_DEBUG:
        case EXCEPT_MACHINE_CHECK:
            except_base = 2;
            break;
        default:
            break;
        }
        r_srr[except_base+0] = ra;
        r_srr[except_base+1] = r_msr.whole;
    }

    // 3: Update ESR (Done)
    // 4: Update DEAR (Done)

    // 5: Load new program state in MSR
    {
        msr_t new_msr;
        new_msr.whole = 0;
        if ( m_exception != EXCEPT_CRITICAL &&
             m_exception != EXCEPT_MACHINE_CHECK &&
             m_exception != EXCEPT_WATCHDOG &&
             m_exception != EXCEPT_DEBUG ) {
            new_msr.ce = r_msr.ce;
            new_msr.de = r_msr.de;
        }
        if ( m_exception != EXCEPT_MACHINE_CHECK )
            new_msr.me = r_msr.me;
        r_msr = new_msr;
    }

    // 7: Load next instruction address
    m_next_pc = r_evpr+except_addresses[m_exception];

  no_except:
    r_pc = m_next_pc;
    return 1;
}

void Ppc405Iss::setDataResponse(const struct DataResponse &drsp)
{
    if ( ! m_dreq.valid )
        return;

    m_dreq.valid = false;
    m_dbe = drsp.error;
    int nb = 0;

    switch ( m_dreq.be ) {
    case 1:
    case 2:
    case 4:
    case 8:
        nb = 1;
        break;
    case 3:
    case 0xc:
        nb = 2;
        break;
    case 0xf:
        nb = 4;
        break;
    }

    uint32_t data = drsp.rdata;

    data >>= 8 * (m_dreq.addr % 4);

    // Swap if PPC does _not_ want reversed data (BE)
    if ( ! r_mem_reversed ) {
        data = soclib::endian::uint32_swap(data);
        data >>= 8 * (4 - nb);
    }

    if ( drsp.error ) {
        return;
    }

    switch ( m_dreq.type ) {
    case DATA_WRITE:
    case XTN_WRITE:
        break;
    case DATA_SC:
    {
        int cr = 0;
        if ( data == 0 ) cr |= CMP_EQ;
        if ( r_xer.so ) cr |= CMP_SO;
        crSet( 0, cr );
        break;
    }
    case DATA_READ:
    case XTN_READ:
    case DATA_LL:
        if ( r_mem_dest != NULL ) {
            switch ( nb ) {
            case 4:
                *r_mem_dest = data;
                break;
            case 2:
                *r_mem_dest = r_mem_unsigned ?
                    (data & 0xffff) :
                    sign_ext(data, 16);
                break;
            case 1:
                *r_mem_dest = r_mem_unsigned ?
                    (data & 0xff) :
                    sign_ext(data, 8);
                break;
            }
        }
    }

#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": "
        << __FUNCTION__ << ": " << m_dreq
        << "->" << r_mem_dest << " Rev:" << r_mem_reversed << " U:" << r_mem_unsigned
        << " " << drsp << " dest: " << r_mem_dest
        << " (r_gp+" << r_mem_dest-&r_gp[0] << "): ";
    if ( r_mem_dest )
        std::cout << *r_mem_dest;
    else
        std::cout << "*NULL";
    std::cout << std::endl;
#endif

}

uint32_t Ppc405Iss::debugGetRegisterValue(unsigned int reg) const
{
    switch (reg)
        {
        case 0 ... 31:
            return r_gp[reg];
        case 32 ... 63:         // FPU
            return 0;
        case 64:                // pc
            return r_pc;
        case 65:                // ps
            return r_msr.whole;
        case 66:                // cnd
            return r_cr;
        case 67:                // lr
            return r_lr;
        case 68:                // cnt
            return r_ctr;
        case 69:                // xer
            return r_xer.whole;
        case 70:                // mq
            return 0;
        case 71:                // fpscr
            return 0;
        case ISS_DEBUG_REG_IS_USERMODE:
            return r_msr.pr;
        case ISS_DEBUG_REG_IS_INTERRUPTIBLE:
            return r_msr.ee || r_msr.ce;
        case ISS_DEBUG_REG_STACK_REDZONE_SIZE:
            return 224;
        default:
            return 0;
        }
}

size_t Ppc405Iss::debugGetRegisterSize(unsigned int reg) const
{
    switch (reg)
        {
        case 32 ... 63:         // FPU
            return 64;
        default:
            return 32;
        }
}

void Ppc405Iss::debugSetRegisterValue(unsigned int reg, uint32_t value)
{
    switch (reg)
        {
        case 0 ... 31:
            r_gp[reg] = value;
            break;
        case 64:                // pc
            r_pc = value;
            break;
        case 65:                // msr
            r_msr.whole = value;
            break;
        case 66:                // cnd
            r_cr = value;
            break;
        case 67:                // lr
            r_lr = value;
            break;
        case 68:                // cnt
            r_ctr = value;
            break;
        case 69:                // xer
            r_xer.whole = value;
            break;
        }
}

int Ppc405Iss::m_bootstrap_cpu_id = -1;

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

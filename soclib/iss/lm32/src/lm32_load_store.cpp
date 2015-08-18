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

namespace soclib { namespace common {
#define tmpl(x) template<bool lEndianInterface> x  LM32Iss<lEndianInterface>

    // Check if memory access is aligned
    // The LM32 spec says that the behavior is undefined and no hardware check is done
#define CHECK_ALIGNED_ADDR(addr, byte_count)                                 \
    do {                                                                     \
        if (addr & (byte_count - 1)) {                                       \
            std::cout << name() << "WARNING: Unaligned memory access!!!"     \
            << std::endl                                                     \
            << " Adress : " << std::hex << addr                              \
            << " byte count : "  << std::dec << byte_count                   \
            << std::endl;                                                    \
        }                                                                    \
    } while(0)


    // Initialize the request
#define INIT_REQ(address, s_ext, dreg)                                       \
    do {                                                                     \
        struct DataRequest null_dreq = ISS_DREQ_INITIALIZER;                 \
        m_dreq.req = null_dreq;                                              \
        m_dreq.sign_extend = s_ext;                                          \
        m_dreq.dest_reg = dreg;                                              \
        m_dreq.addr = address;                                               \
    } while(0)

    // Initialize the sub request, the one that is forwarded to the iss2
    // Here the address must be word (4bytes) aligned
#define BUILD_SUBREQ(req, address, byte_count, data, operation )        \
    do {                                                                \
        assert (((byte_count) == 1)                                     \
                || ((byte_count) == 2)                                  \
                || ((byte_count) == 4));                                \
        uint offset = (address) & 0x3;                                  \
        req.valid = true;                                               \
        req.addr = (address) & (~3);                                    \
        if ( lEndianInterface ) {                                       \
        req.wdata = soclib::endian::uint32_swap(data);                  \
        req.wdata = (req.wdata) >> (8*(4-byte_count));                  \
        req.wdata = (req.wdata) << (8*offset);                          \
        req.be = (((1 << (byte_count))-1) << offset) & 0xf;             \
        }                                                               \
        else  {                                                         \
        req.wdata = data ;                                              \
        req.wdata = (req.wdata) << (8*(4 -byte_count - offset));        \
        req.be = (((1 << (byte_count))-1)                               \
                              << (4 -byte_count - offset)) & 0xf;       \
        }                                                               \
        req.type = (operation);                                         \
        req.mode = MODE_USER;                                           \
    } while(0)


    // LM32 STORE/LOAD intructions
#define LM32_function(x) tmpl(void)::OP_LM32_##x()
    //!Instruction lb behavior method.
    LM32_function( lb ){
        addr_t addr;
        uint32_t rd;
        addr = r_gp[m_inst.I.rY] + sign_ext(m_inst.I.imd, 16);
        rd  = m_inst.I.rX;
        INIT_REQ (addr, true , rd);
        BUILD_SUBREQ(m_dreq.req, addr, 1, 0, DATA_READ );
    }

    //!Instruction lbu behavior method.
    LM32_function( lbu ){
        addr_t addr;
        uint32_t rd;
        addr = r_gp[m_inst.I.rY] + sign_ext(m_inst.I.imd, 16);
        rd  = m_inst.I.rX;
        INIT_REQ (addr, false, rd);
        BUILD_SUBREQ(m_dreq.req, addr, 1, 0, DATA_READ );
    }

    //!Instruction lh behavior method.
    LM32_function( lh ){
        addr_t addr;
        uint32_t rd;
        addr = r_gp[m_inst.I.rY] + sign_ext(m_inst.I.imd, 16);
        rd  = m_inst.I.rX;
        CHECK_ALIGNED_ADDR (addr, 2);
        INIT_REQ (addr, true , rd);
        BUILD_SUBREQ(m_dreq.req, addr, 2, 0, DATA_READ );
    }

    //!Instruction lhu behavior method.
    LM32_function( lhu ){
        addr_t addr;
        uint32_t rd;
        addr = r_gp[m_inst.I.rY] + sign_ext(m_inst.I.imd, 16);
        rd  = m_inst.I.rX;
        CHECK_ALIGNED_ADDR (addr, 2);
        INIT_REQ (addr, false, rd);
        BUILD_SUBREQ(m_dreq.req, addr, 2, 0, DATA_READ );
    }

    //!Instruction lw behavior method.
    LM32_function( lw ){
        addr_t addr;
        uint32_t rd;
        addr = r_gp[m_inst.I.rY] + sign_ext(m_inst.I.imd, 16);
        rd  = m_inst.I.rX;
        CHECK_ALIGNED_ADDR (addr, 4);
        INIT_REQ (addr, false, rd);
        BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_READ );
    }

    //!Instruction sb behavior method.
    LM32_function( sb ){
        addr_t addr;
        uint32_t wd;
        addr = r_gp[m_inst.I.rY] + sign_ext(m_inst.I.imd, 16);
        wd  = r_gp[m_inst.I.rX] & 0xFF;
        INIT_REQ (addr, false, 0);
        BUILD_SUBREQ(m_dreq.req, addr, 1, wd, DATA_WRITE);
    }

    //!Instruction sh behavior method.
    LM32_function( sh ){
        addr_t addr;
        uint32_t wd;
        addr = r_gp[m_inst.I.rY] + sign_ext(m_inst.I.imd, 16);
        wd  = r_gp[m_inst.I.rX] & 0xFFFF;
        CHECK_ALIGNED_ADDR (addr, 2);
        INIT_REQ (addr, false, 0);
        BUILD_SUBREQ(m_dreq.req, addr, 2, wd, DATA_WRITE);
    }

    //!Instruction sw behavior method.
    LM32_function( sw ){
        addr_t addr;
        uint32_t wd;
        addr = r_gp[m_inst.I.rY] + sign_ext(m_inst.I.imd, 16);
        wd  = r_gp[m_inst.I.rX];
        CHECK_ALIGNED_ADDR (addr, 4);
        INIT_REQ (addr, false, 0);
        BUILD_SUBREQ(m_dreq.req, addr, 4, wd, DATA_WRITE);
    }

#undef LM32_function
#undef BUILD_SUBREQ
#undef INIT_REQ
#undef CHECK_ALIGNED_ADDR
#undef tmpl

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


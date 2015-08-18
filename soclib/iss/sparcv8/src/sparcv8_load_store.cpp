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
#include "arithmetics.h"
#include <cassert>
#include <strings.h>

namespace soclib { namespace common {

#define tmpl(x) template<unsigned int NWINDOWS> x Sparcv8Iss<NWINDOWS>

#define op(x) tmpl(void)::op_##x() {                                    \
        std::cout <<  #x << " : no co-processor!" << std::endl;         \
        m_exception = true;                                             \
        m_exception_cause = TP_CP_DISABLED;                             \
    }
  op(ldc);
  op(ldcsr);
  op(lddc);
  op(stc);
  op(stcse);
  op(stdcq);
  op(stdc);
#undef op


#define GET_OPERANDS(ins, addr, rd)                                     \
    do {                                                                \
        addr = GPR(ins.format3a.rs1);                                   \
        if (ins.format3a.i)                                             \
            addr += (uint32_t)sign_ext(ins.format3b.imm, 13);           \
        else                                                            \
            addr += GPR(ins.format3a.rs2);                              \
        rd = RR(ins.format3a.rd);                                       \
    } while(0)

#define GET_OPERANDSF(ins, addr, rd)                                    \
    do {                                                                \
        addr = GPR(ins.format3a.rs1);                                   \
        if (ins.format3a.i)                                             \
            addr += (uint32_t)sign_ext(ins.format3b.imm, 13);           \
        else                                                            \
            addr += GPR(ins.format3a.rs2);                              \
        rd = ins.format3a.rd;                                           \
    } while(0)


#define GET_OPERANDS_ASI(ins, addr, asi, rd)                            \
    do {                                                                \
        addr = GPR(ins.format3a.rs1);                                   \
        if(ins.format3a.i == 1) {                                       \
            m_exception = true;                                         \
            m_exception_cause = TP_ILLEGAL_INSTRUCTION;                 \
            return;                                                     \
        }                                                               \
        addr += GPR(ins.format3a.rs2);                                  \
        asi = ins.format3a.asi;                                         \
        rd = RR(ins.format3a.rd);                                       \
    } while(0)


#define INIT_REQ(address, s_ext, dreg, ext_dreg, reg_type)              \
    do {                                                                \
        struct DataRequest null_dreq = ISS_DREQ_INITIALIZER;            \
        m_dreq.req = null_dreq;                                         \
        m_dreq.ext_req = null_dreq;                                     \
        m_dreq.sign_extend = s_ext;                                     \
        m_dreq.dest_reg = dreg;                                         \
        m_dreq.ext_dest_reg = ext_dreg;                                 \
        m_dreq.addr = address;                                          \
        m_dreq.regtype = reg_type;                                      \
    } while(0)


#define BUILD_SUBREQ(req, address, byte_count, data, operation )        \
    do {                                                                \
        assert (((byte_count) == 1)                                     \
                || ((byte_count) == 2)                                  \
                || ((byte_count) == 4));                                \
        uint offset = (address) & 0x3;                                  \
        req.valid = true;                                               \
        req.addr = (address) & (~3);                                    \
        req.wdata = soclib::endian::uint32_swap(data);                  \
        req.wdata = (req.wdata) >> (8*(4-byte_count));                  \
        req.wdata = (req.wdata) << (8*offset);                          \
        req.be = (((1 << (byte_count))-1) << offset) & 0xf;             \
        req.type = (operation);                                         \
        req.mode = r_psr.s ? MODE_KERNEL : MODE_USER;                   \
    } while(0)


#define ENSURE_ALIGNED_ADDR(addr, byte_count)                           \
    do {                                                                \
        if (addr & (byte_count - 1)) {                                  \
            m_exception = true;                                         \
            m_exception_cause = TP_MEM_ADDRESS_NOT_ALIGNED;             \
            return;                                                     \
        }                                                               \
    } while(0)


#define ENSURE_PRIVILEDGED_MODE()                                       \
    do {                                                                \
        if (r_psr.s != 1) {                                             \
            m_exception = true;                                         \
            m_exception_cause = TP_PRIVILEGED_INSTRUCTION;              \
            return;                                                     \
        }                                                               \
    } while(0)

#define ENSURE_EF_SET()                                                 \
    do {                                                                \
        if (r_psr.ef != 1) {                                            \
            m_exception = true;                                         \
            m_exception_cause = TP_FP_DISABLED;                         \
            return;                                                     \
        }                                                               \
    } while(0)




tmpl(void)::setData(const struct DataResponse &rsp)
{
    // If no request pending, then everything is ok on the data side !
    if (!m_dreq.req.valid && !m_dreq.ext_req.valid) {
        m_dreq_pending = false;
        return;
    }

    // Fom now on, a data request is pending.
    // If it isn't yet satisyed, then mark it as pending
    // and don't do anything...
    if (!rsp.valid) {
        m_dreq_pending = true;
        return;
    }

    // We got an answer.
    // Mark the data subrequest as satisfyed, and eventually de-assert data request
    struct DataRequest& req = m_dreq.req.valid ? m_dreq.req : m_dreq.ext_req;
    bool ext_req = m_dreq.req.valid ? false : true;
    if (m_dreq.req.valid)
        m_dreq.req.valid = false;
    else if(m_dreq.ext_req.valid)
        m_dreq.ext_req.valid = false;
    m_dreq_pending = m_dreq.req.valid | m_dreq.ext_req.valid;

    // If there was a data bus error, then record it, and don't do anything.
    // It will be handled later (in executeNcycles()).
    m_dbe = rsp.error;
    if ( rsp.error )
        return;

    data_t data = soclib::endian::uint32_swap(rsp.rdata);

    // If request was a read (LOAD), then get data, swap bytes and
    // store them into destination register
    switch (req.type) {
    case DATA_LL:
            if (m_dreq.cmp_reg >= 0) {
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << name() << " compare " << data << " <=> " << m_dreq.cmp_reg << " " << r_gp[m_dreq.cmp_reg] << "\n";
#endif
                if (data == r_gp[m_dreq.cmp_reg]) {
                    if (data != soclib::endian::uint32_swap(m_dreq.ext_req.wdata))
                        // send DATA_SC prepared in ext_req and postpone write to rd register
                        m_dreq_pending = m_dreq.ext_req.valid = true;
                    else
                        // memory already contains value to write, do not write to memory
                        r_gp[m_dreq.dest_reg] = data;
                }

                break;
            }

    case DATA_READ:
    case XTN_READ: {
        // Extract data from word on data bus
        switch (req.be) {
        case 8 :
            data = data & 0xff;
            if (m_dreq.sign_extend)
                data = soclib::common::sign_ext(data, 8);
            break;
        case 4 :
            data = (data>>8) & 0xff;
            if (m_dreq.sign_extend)
                data = soclib::common::sign_ext(data, 8);
            break;

        case 2 :
            data = (data>>16) & 0xff;
            if (m_dreq.sign_extend)
                data = soclib::common::sign_ext(data, 8);
            break;

        case 1 :
            data = (data>>24) & 0xff;
            if (m_dreq.sign_extend)
                data = soclib::common::sign_ext(data, 8);
            break;

        case 12 :
            data = data & 0xffff;
            if (m_dreq.sign_extend)
                data = soclib::common::sign_ext(data, 16);
            break;

        case 3 :
            data = (data>>16) & 0xffff;
            if (m_dreq.sign_extend)
                data = soclib::common::sign_ext(data, 16);
            break;
        }

        // Store value in reg (if reg is not g0...)
        uint dest_reg = (ext_req) ? m_dreq.ext_dest_reg : m_dreq.dest_reg;
        switch(m_dreq.regtype) {
        case TYPE_GP :
            if (dest_reg != 0) 
                r_gp[dest_reg] = data;
            break;
#if FPU
        case TYPE_F :
            r_f[dest_reg] = data;
            break;
        case TYPE_FSR : {
            // LDFSR does not affect qne, ftt, ver, unused, reserved
            unsigned char ftt = r_fsr.ftt;
            unsigned char qne = r_fsr.qne;
            r_fsr.whole = data;
            r_fsr.ftt = ftt;
            r_fsr.qne = qne;
            r_fsr.unused = 0;
            r_fsr.reserved = 0;
            r_fsr.ver = 0;
        }
            break;
#else
        case TYPE_F:
        case TYPE_FSR:
            std::cout << name()
                      << "(setData) : no floating point unit!" << std::endl;
            assert(false);
            break;
#endif
        case TYPE_C :
        case TYPE_CSR :
            std::cout << name()
                      << "(setData) : no coprocessor!" << std::endl;
            assert(false);
            break;
        default : 
            // Do nothing...
            break;
        }

#ifdef SOCLIB_MODULE_DEBUG
    std::cout
        << name()
        << " setData: " << rsp
        << " type: " << req.type
        << " sign_ext: " << m_dreq.sign_extend
        << " dest: " << dest_reg
        << " data: " << data
        << std::endl;
#endif
    break;
    }

    case DATA_SC:
        // DATA_SC returns 0 on success.

        // CASA instruction case
        if (m_dreq.cmp_reg >= 0) {
#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " casa ok " << r_gp[m_dreq.cmp_reg] << " => " << m_dreq.dest_reg << "\n";
#endif
            if (rsp.rdata == 0)
                r_gp[m_dreq.dest_reg] = r_gp[m_dreq.cmp_reg];
        }

        // Handle special case of DATA_SC (used to try to mimic SWAP and LDSTUB)
        else if (rsp.rdata != 0) {
                // If unsuccess, we may either send a TRAP or try the SWAP again...
#ifdef SPARC_SWAP_TRAPS
                // We raise a custom trap, to give the underlying system the possibility to
                // catch the exception, take notice of the problem and take appropriate
                // measures (try again, abort, ...)
                // The trap is arbitrarly choosen as 0x0f...
                m_exception = true;
                m_exception_cause = TP_TRAP_INSTRUCTION(0x0f);
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << "SWAP / LDSTUB : unsuccess. Trapping..." << std::endl;
#endif
#else
                // Try again the swap. May deadlock :/
                m_dreq.req.valid = true;
                m_dreq.ext_req.valid = true;
                m_dreq_pending = true;
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << "SWAP / LDSTUB : unsuccess. Retrying..." << std::endl;
#endif
#endif
        }
        break;

    default: {
        // Transaction was a write... Just dump it !
#ifdef SOCLIB_MODULE_DEBUG
        uint dest_reg = (ext_req) ? m_dreq.ext_dest_reg : m_dreq.dest_reg;
        std::cout
            << name()
            << " setData: " << rsp
            << " type: " << req.type
            << " sign_ext: " << m_dreq.sign_extend
            << " dest: " << dest_reg
            << std::endl;
#endif
    }
    }
}

tmpl(void)::op_ldsb()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 1);

    INIT_REQ(addr, true, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 1, 0, DATA_READ);
}

tmpl(void)::op_ldsh()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 2);

    INIT_REQ(addr, true, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 2, 0, DATA_READ);
}

tmpl(void)::op_ldub()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 1);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 1, 0, DATA_READ);
}

tmpl(void)::op_lduh()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 2);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 2, 0, DATA_READ);
}

tmpl(void)::op_ld()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_READ);
}

tmpl(void)::op_ldd()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 8);
    rd = rd & 0xfffffffe;

    INIT_REQ(addr, false, rd, rd+1, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_READ);
    BUILD_SUBREQ(m_dreq.ext_req, (addr+4), 4, 0, DATA_READ);
    setInsDelay(1);
}

tmpl(void)::op_ldsba()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 1);

    INIT_REQ(addr, true, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 1, 0, DATA_READ);
}

tmpl(void)::op_ldsha()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 2);

    INIT_REQ(addr, true, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 2, 0, DATA_READ);
}

tmpl(void)::op_lduba()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 1);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 1, 0, DATA_READ);
}

tmpl(void)::op_lduha()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 2);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 2, 0, DATA_READ);
}

tmpl(void)::op_lda()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_READ);
}

tmpl(void)::op_ldda()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 8);
    rd = rd & 0xfffffffe;

    INIT_REQ(addr, false, rd, rd+1, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_READ);
    BUILD_SUBREQ(m_dreq.ext_req, addr+4, 4, 0, DATA_READ);
    setInsDelay(1);
}

tmpl(void)::op_stb()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 1);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 1, r_gp[rd], DATA_WRITE);
}

tmpl(void)::op_sth()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 2);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 2, r_gp[rd], DATA_WRITE);
}

tmpl(void)::op_st()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 4, r_gp[rd], DATA_WRITE);
}

tmpl(void)::op_std()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 8);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 4, r_gp[rd & 0xfffffffe], DATA_WRITE);
    BUILD_SUBREQ(m_dreq.ext_req, (addr+4), 4, r_gp[(rd & 0xfffffffe)+1], DATA_WRITE);
    setInsDelay(1);
}

tmpl(void)::op_stba()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 1);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 1, r_gp[rd], DATA_WRITE);
}

tmpl(void)::op_stha()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 2);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 2, r_gp[rd], DATA_WRITE);
}

tmpl(void)::op_sta()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 4, r_gp[rd], DATA_WRITE);
}

tmpl(void)::op_stda()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 8);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 4, r_gp[rd & 0xfffffffe], DATA_WRITE);
    BUILD_SUBREQ(m_dreq.ext_req, (addr+4), 4, r_gp[(rd & 0xfffffffe)+1], DATA_WRITE);
    setInsDelay(1);
}

tmpl(void)::op_swap()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_LL);
    BUILD_SUBREQ(m_dreq.ext_req, addr, 4, r_gp[rd], DATA_SC);
    setInsDelay(1);
}

tmpl(void)::op_ldstub()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 1);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 1, 0, DATA_LL);
    BUILD_SUBREQ(m_dreq.ext_req, addr, 1, 0xff, DATA_SC);
    m_dreq.cmp_reg = -1;
    setInsDelay(1);
}

tmpl(void)::op_swapa()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_LL);
    BUILD_SUBREQ(m_dreq.ext_req, addr, 4, r_gp[rd], DATA_SC);
    m_dreq.cmp_reg = -1;
    setInsDelay(1);
}

tmpl(void)::op_casa()  // casa is a v9 instruction also present in leon sparc v8
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();

    addr = GPR(m_ins.format3a.rs1);
    asi = m_ins.format3a.asi;
    rd = RR(m_ins.format3a.rd);

    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_LL);
    BUILD_SUBREQ(m_dreq.ext_req, addr, 4, r_gp[rd], DATA_SC);
    m_dreq.cmp_reg = RR(m_ins.format3a.rs2);
    m_dreq.ext_req.valid = false; // prepare SC but don't send it yet
    setInsDelay(1);
}

tmpl(void)::op_ldstuba()
{
    __attribute__((unused)) addr_t addr, asi;
    uint32_t rd;
    ENSURE_PRIVILEDGED_MODE();
    GET_OPERANDS_ASI(m_ins, addr, asi, rd);
    ENSURE_ALIGNED_ADDR(addr, 1);

    INIT_REQ(addr, false, rd, 0, TYPE_GP);
    BUILD_SUBREQ(m_dreq.req, addr, 1, 0, DATA_LL);
    BUILD_SUBREQ(m_dreq.ext_req, addr, 1, 0xff, DATA_SC);
    m_dreq.cmp_reg = -1;
    setInsDelay(1);
}

tmpl(void)::op_flush() 
{
    addr_t addr;
    __attribute__((unused)) uint32_t rd;
    GET_OPERANDS(m_ins, addr, rd);
    ENSURE_ALIGNED_ADDR(addr, 4);
    
    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, 4*XTN_DCACHE_INVAL, 4, addr, XTN_WRITE);
}

#if FPU
tmpl(void)::op_ldf()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDSF(m_ins, addr, rd);
    ENSURE_EF_SET();
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, rd, 0, TYPE_F);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_READ);
}

tmpl(void)::op_lddf()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDSF(m_ins, addr, rd);
    ENSURE_EF_SET();
    ENSURE_ALIGNED_ADDR(addr, 8);
    rd = rd & 0xfffffffe;

    INIT_REQ(addr, false, rd, rd+1, TYPE_F);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_READ);
    BUILD_SUBREQ(m_dreq.ext_req, (addr+4), 4, 0, DATA_READ);
    setInsDelay(1);
}

tmpl(void)::op_ldfsr()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDSF(m_ins, addr, rd);
    ENSURE_EF_SET();
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, rd, 0, TYPE_FSR);
    BUILD_SUBREQ(m_dreq.req, addr, 4, 0, DATA_READ);
}

tmpl(void)::op_stdf()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDSF(m_ins, addr, rd);
    ENSURE_EF_SET();
    ENSURE_ALIGNED_ADDR(addr, 8);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 4, r_f[rd & 0xfffffffe], DATA_WRITE);
    BUILD_SUBREQ(m_dreq.ext_req, (addr+4), 4, r_f[(rd & 0xfffffffe)+1], DATA_WRITE);
    setInsDelay(1);
}

tmpl(void)::op_stf()
{
    addr_t addr;
    uint32_t rd;
    GET_OPERANDSF(m_ins, addr, rd);
    ENSURE_EF_SET();
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 4, r_f[rd], DATA_WRITE);
}

tmpl(void)::op_stfsr()
{
    addr_t addr;
    __attribute__((unused)) uint32_t rd;
    GET_OPERANDSF(m_ins, addr, rd);
    ENSURE_EF_SET();
    ENSURE_ALIGNED_ADDR(addr, 4);

    INIT_REQ(addr, false, 0, 0, TYPE_NONE);
    BUILD_SUBREQ(m_dreq.req, addr, 4, r_fsr.whole, DATA_WRITE);
    // Zero ftt (optional)
    r_fsr.ftt = 0;
}

tmpl(void)::op_stdfq()
{
    ENSURE_EF_SET();
    // No FP queue
    m_exception = true;
    m_exception_cause = TP_FP_EXCEPTION;
    r_fsr.ftt = TP_SEQUENCE_ERROR;
}

#else

#define op(x) tmpl(void)::op_##x() {                                    \
        std::cout <<  #x << " : no floating point unit!" << std::endl;  \
        m_exception = true;                                             \
        m_exception_cause = TP_FP_DISABLED;                             \
    }
op(ldf);
op(ldfsr);
op(lddf);
op(stf);
op(stfsr);
op(stdfq);
op(stdf);
#undef op

#endif

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


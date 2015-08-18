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
 * NIOSII Instruction Set Simulator for the Altera NIOSII processor core
 * developed for the SocLib Projet
 *
 * Copyright (C) IRISA/INRIA, 2007-2008
 *         François Charot <charot@irisa.fr>
 *
 * Contributing authors:
 * 				Delphine Reeb
 * 				François Charot <charot@irisa.fr>
 *
 * Maintainer: charot
 *
 * History:
 * - summer 2006: First version developed on a first SoCLib template by Reeb, Charot.
 * - september 2007: the model has been completely rewritten and adapted to the SocLib
 * 						rules defined during the first months of the SocLib ANR project
 *
 * Functional description:
 * Four files:
 * 		nios2_fast.h
 * 		nios2_ITypeInst.cpp
 * 		nios2_RTypeInst.cpp
 * 		nios2_customInst.cpp
 * define the Instruction Set Simulator for the NIOSII processor.
 *
 *
 */

#include "base_module.h"
#include "niosII.h"
#include <cstdlib>

namespace soclib {
namespace common {

#define op(x) & Nios2fIss::RType_##x
#define op4(x, y, z, t) op(x), op(y), op(z), op(t)
Nios2fIss::func_t const Nios2fIss::RTypeTable[] = { op4(illegal, eret, roli, rol),
                                                    op4( flushp,     ret,     nor,  mulxuu),
                                                    op4(  cmpge,    bret, illegal,     ror),
                                                    op4( flushi,     jmp,     and, illegal),

                                                    op4(  cmplt, illegal,    slli,     sll),
                                                    op4(illegal, illegal,      or,  mulxsu),
                                                    op4(  cmpne, illegal,    srli,     srl),
                                                    op4( nextpc,   callr,     xor,  mulxss),

                                                    op4(  cmpeq, illegal, illegal, illegal),
                                                    op4(   divu,     div,   rdctl,     mul),
                                                    op4( cmpgeu,   initi, illegal, illegal),
                                                    op4(illegal,    trap,   wrctl, illegal),

                                                    op4( cmpltu,     add, illegal, illegal),
                                                    op4(  break, illegal,    sync, illegal),
                                                    op4(illegal,     sub,    srai,     sra),
                                                    op4(illegal, illegal, illegal, illegal),

};

/*
 * Avoid duplication of source code, this kind of op
 * is easy to bug, and should be easy to debug ;)
 */
static inline uint32_t sll( uint32_t reg, uint32_t sh )
{
	return reg << sh;
}

static inline uint32_t srl( uint32_t reg, uint32_t sh )
{
//	return (reg >> sh) & ((1<<(32-sh))-1);
	return (reg >> sh);
}

static inline uint32_t sra( uint32_t reg, uint32_t sh )
{
	if ( (int32_t)reg < 0 )
        return (reg >> sh) | (~((1<<(32-sh))-1));
	else
 //       return (reg >> sh) & ((1<<(32-sh))-1);
    return (reg >> sh);
}

void Nios2fIss::RType_add()
{
	// Add(rC<-rA+rB) p.8-9
    uint64_t tmp = (uint64_t)m_gprA + (uint64_t)m_gprB;
    // 05/06/07 : overflow detection is not working properly
    //      if ( (bool)(tmp&(uint64_t)((uint64_t)1<<32)) != (bool)(tmp&(1<<31)) )
    //	exceptionSignal = X_OV;
    //      else
    r_gpr[m_instruction.r.c] = tmp;
}

void Nios2fIss::RType_and()
{
    //Bitwise logical and p.8-11
    r_gpr[m_instruction.r.c] = m_gprA & m_gprB;
}

void Nios2fIss::RType_break()
{
    //Debugging breakpoint p.8-25
	m_exceptionSignal = X_BR;

	// 4 cycles per instruction
    setInsDelay( 4 );
}

void Nios2fIss::RType_bret()
{
    if (r_status.u) {
        m_exceptionSignal = X_SOINST;
        return;
    }

    //Breakpoint return p.8-26
    r_status.whole = r_bstatus; /* update status register */
    if (m_instruction.r.a == BA) {
        m_branchAddress = r_gpr[BA]; /* GPR[ins_rs]=ba=r30 */
        m_branchTaken = true;
    }

    // 4 cycles per instruction
    setInsDelay( 4 );
}

void Nios2fIss::RType_callr()
{
    //Call subroutine in Register (ra<-PC + 4, PC <-reg) p.8-28
    r_gpr[RA] = r_pc + 4; /* return address register (ra = r31) */
    m_branchAddress = r_gpr[m_instruction.r.a];
    m_branchTaken = true;

    // 3 cycles per instruction
    setInsDelay( 3 );
}

void Nios2fIss::RType_cmpeq()
{
    // Compare Equal p.8-29
    r_gpr[m_instruction.r.c] = (bool) (m_gprA == m_gprB);
}

void Nios2fIss::RType_cmpge()
{
    // Compare greater than or equal signed p. 8-31
    r_gpr[m_instruction.r.c] = (bool) ((int32_t)m_gprA >= (int32_t)m_gprB );
}

void Nios2fIss::RType_cmpgeu()
{
    // Compare greater than or equal unsigned p.8-33
    r_gpr[m_instruction.r.c] = (bool) (m_gprA >= m_gprB);
}

void Nios2fIss::RType_cmplt()
{
    // Compare less than signed p.8-43
    r_gpr[m_instruction.r.c] = (bool) ((int32_t)m_gprA < (int32_t)m_gprB );
}

void Nios2fIss::RType_cmpltu()
{
    // Compare less than unsigned p.8-45
    r_gpr[m_instruction.r.c] = (bool) (m_gprA < m_gprB);
}

void Nios2fIss::RType_cmpne()
{
    // Compare not equal p.8-47
    r_gpr[m_instruction.r.c] = (bool) (m_gprA != m_gprB);
}

void Nios2fIss::RType_div()
{
    // Divide p.8-50
    r_gpr[m_instruction.r.c] = (int32_t)m_gprA / (int32_t)m_gprB;

    // 4-66 cycles per instruction
    setInsDelay( 33 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_divu()
{
    // Divide unsigned p.8-51
    r_gpr[m_instruction.r.c] = m_gprA / m_gprB;

    // 4-66 cycles per instruction
    setInsDelay( 33 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_eret()
{
    if (r_status.u) {
        m_exceptionSignal = X_SOINST;
        return;
    }

    //Exception Return p.8-52
	r_status.whole = r_estatus;
    m_branchAddress = r_gpr[m_instruction.r.a]; /* exception return address ea=r29 */
    m_branchTaken = true;

    // 4 cycles per instruction
    setInsDelay( 4 );
}

void Nios2fIss::RType_flushi()
{
    // 4 cycles per instruction
    setInsDelay( 4 );
}

void Nios2fIss::RType_flushp()
{
    // 4 cycles per instruction
    setInsDelay( 4 );
}

void Nios2fIss::RType_initi()
{
    // 4 cycles per instruction
    setInsDelay( 4 );
}

void Nios2fIss::RType_jmp()
{
    // Computed Jump p.8-59
    /* to jump to r31 (return adress (ra) register) use ret instruction */
    m_branchAddress = m_gprA;
    m_branchTaken=true;

    // 3 cycles per instruction
    setInsDelay( 3 );
}

void Nios2fIss::RType_mul()
{
    // Multiply rC<-(rA*rB)(31..0) p.8-70
    int64_t res, a, b;
    a = m_gprA;
    b = m_gprB;
    res = a * b;
    r_gpr[m_instruction.r.c] = res;

    // 1 cycles per instruction with embedded multiplier
    setInsDelay( 1);

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_mulxss()
{
    // Multiply extended signed/signed (rC<-rA*rB)(32 MSB bits) p.8-72
    uint64_t res, a, b;
    a = (int32_t)m_gprA;
    b = (int32_t)m_gprB;
    res = a * b;
    r_gpr[m_instruction.r.c] = res >> 32;

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);

}

void Nios2fIss::RType_mulxsu()
{
    // Multiply extended signed/unsigned (rC<-rA*rB)(32 MSB bits) p.8-73
    uint64_t res, a, b;
    a = (int32_t)m_gprA;
    b = m_gprB;
    res = a * b;
    r_gpr[m_instruction.r.c] = res >> 32;

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_mulxuu()
{
    // Multiply extended unsigned/unsigned (rC<-rA*rB)(32 MSB bits) p.8-74
    uint64_t res, a, b;
    a = m_gprA;
    b = m_gprB;
    res = a * b;
    r_gpr[m_instruction.r.c] = res >> 32;

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_nextpc()
{
    //Get address of following instruction p.8-75
    r_gpr[m_instruction.r.c] = r_pc + 4;
}

void Nios2fIss::RType_nor()
{
    // Bitwise logical nor (rC<-no(rA|rB)) p.8-77
    r_gpr[m_instruction.r.c] = ~(m_gprA | m_gprB);
}

void Nios2fIss::RType_or()
{
    // Bitwise logical or (rC<-(rA|rB) p.8-78
    r_gpr[m_instruction.r.c] = m_gprA | m_gprB;
}

void Nios2fIss::RType_rdctl()
{
    if (r_status.u) {
        m_exceptionSignal = X_SOINST;
        return;
    }

    //Read from control register p.8-81
	//r_gpr[m_instruction.r.c] = r_ctl[m_instruction.r.sh];
	r_gpr[m_instruction.r.c] = controlRegisterGet(m_instruction.r.sh);

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_ret()
{
    //Return from subroutine p.8-82
    m_branchAddress = m_gprA;
    m_branchTaken=true;

    // 3 cycles per instruction
    setInsDelay( 3 );
}

void Nios2fIss::RType_rol()
{
    // Rotate Left (rC<-rA[rB[4..0]]:rA[32-rB[4..0]]) p.8-83
    int shift = m_gprB & 0x1F;
    r_gpr[m_instruction.r.c] = (m_gprA << shift) | m_gprA >> (32 - shift);

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_roli()
{
    // Rotate Left Immediate (rC<-rA[IMM5]:rA[32-IMM5]) p.8-84
    int shift = m_instruction.r.sh;
    r_gpr[m_instruction.r.c] = (m_gprA << shift) | m_gprA >> (32 - shift);

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_ror()
{
    // Rotate Right (rC<-rA[32-rB[4..0]]:rA[rB[4..0]]) p.8-85
    int shift = m_gprB & 0x1F;
    r_gpr[m_instruction.r.c] = (m_gprA >> shift) | m_gprA << (32 - shift);

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_sll()
{
    // Shift left logical (rC<-rA<<rB[4..0]) p.8-86
    r_gpr[m_instruction.r.c] = sll(m_gprA, m_gprB & 0x1F);

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);

}

void Nios2fIss::RType_slli()
{
    // Shift left logical immediate (rC<-rA<<IMM5) p.8-87
    r_gpr[m_instruction.r.c] = sll(m_gprA, m_instruction.r.sh);

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_sra()
{
    // Shift right arithmetic (rC<-(signed)rA>>rB[4..0]) p.8-88
    r_gpr[m_instruction.r.c] = sra(m_gprA, m_gprB & 0x1F);

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_srai()
{
    // Shift right arithmetic immediate (rC<-(signed)rA>>IMM5) p.8-89
    r_gpr[m_instruction.r.c] = sra(m_gprA, m_instruction.r.sh);

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_srl()
{
    // Shift right logical (rC<-(unsigned)rA>>rB[4..0]) p.8-90
    r_gpr[m_instruction.r.c] = srl(m_gprA, m_gprB & 0x1F);

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_srli()
{
    // Shift right logical immediate (rC<-(unsigned)rA>>IMM5) p.8-91
    r_gpr[m_instruction.r.c] = srl(m_gprA, m_instruction.r.sh);

    // 1 cycles per instruction with embedded multiplier
    // setInsDelay( 1 );

    // late result management
    m_listOfLateResultInstruction.add(m_instruction.r.c);
}

void Nios2fIss::RType_sub()
{
    // Subtract (rC<-rA-rB) p.8-95
    uint64_t tmp = (uint64_t)m_gprA - (uint64_t)m_gprB;
    //      if ( (bool)(tmp&(uint64_t)((uint64_t)1<<32)) != (bool)(tmp&(1<<31)) )
    //	exceptionSignal = X_OV;
    //      else
    r_gpr[m_instruction.r.c] = tmp;
}

void Nios2fIss::RType_sync()
{

}

void Nios2fIss::RType_trap()
{
    //Exception intruction p.8-98
    m_exceptionSignal = X_TR;

    setInsDelay( 4 );
}

void Nios2fIss::RType_wrctl()
{
    if (r_status.u) {
        m_exceptionSignal = X_SOINST;
        return;
    }

    //Write to control register p.8-99
	//r_ctl[m_instruction.r.sh] = r_gpr[m_instruction.r.a];
	controlRegisterSet(m_instruction.r.sh, r_gpr[m_instruction.r.a]);

    // 4 cycles per instruction
    setInsDelay( 4 );
}

void Nios2fIss::RType_xor()
{
    // Bitwise logical exclusive or (rC<-rA xor rB) p.8-100
    r_gpr[m_instruction.r.c] = m_gprA ^ m_gprB;
}

void Nios2fIss::RType_illegal()
{
    m_exceptionSignal = X_ILLEGAL;
}

void Nios2fIss::op_RType()
{
    func_t func = RTypeTable[m_instruction.r.opx];
    (this->*func)();
}

}
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


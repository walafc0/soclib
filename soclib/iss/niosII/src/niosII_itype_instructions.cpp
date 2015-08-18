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

#include <stdint.h>
#include "base_module.h"
#include "niosII.h"
#include "arithmetics.h"

namespace soclib {
namespace common {

#define op(x) & Nios2fIss::op_##x
#define op4(x, y, z, t) op(x), op(y), op(z), op(t)

Nios2fIss::func_t const Nios2fIss::opcodeTable[] = {
    op4( call, jmpi, illegal, ldbu),
    op4(   addi,     stb,      br,     ldb),
    op4( cmpgei, illegal, illegal,    ldhu),
    op4(   andi,     sth,     bge,     ldh),

    op4( cmplti, illegal, illegal, illegal),
    op4(    ori,     stw,     blt,     ldw),
    op4( cmpnei, illegal, illegal, flushda),
    op4(   xori, illegal,     bne, illegal),

    op4( cmpeqi, illegal, illegal,  ldbuio),
    op4(   muli,   stbio,     beq,   ldbio),
    op4(cmpgeui, illegal, illegal,  ldhuio),
    op4(  andhi,   sthio,    bgeu,   ldhio),

    op4(cmpltui, illegal,  custom,   initd),
    op4(   orhi,   stwio,    bltu,   ldwio),
    op4(illegal, illegal,   RType,  flushd),
    op4(  xorhi, illegal, illegal, illegal), };

#undef op

//#if SOCLIB_MODULE_DEBUG
#define op(x) #x
const char * Nios2fIss::opNameTable[] = {
    op4( call, illegal, illegal, ldbu),
    op4( addi, stb, br, ldb),
    op4( cmpgei, illegal, illegal, ldhu),
    op4( andi, sth, bge, ldh),

    op4( cmplti, illegal, illegal, illegal),
    op4( ori, stw, blt, ldw),
    op4( cmpnei, illegal, illegal, flushda),
    op4( xori, illegal, bne, illegal),

    op4( cmpeqi, illegal, illegal, ldbuio),
    op4( muli, stbio, beq, ldbio),
    op4(cmpgeui, illegal, illegal, ldhuio),
    op4( andhi, sthio, bgeu, ldhio),

    op4(cmpltui, illegal, custom, initd),
    op4( orhi, stwio, bltu, ldwio),
    op4(illegal, illegal, RType, flushd),
    op4( xorhi, illegal, illegal, illegal),
};

const char * Nios2fIss::opxNameTable[] = {
    op4(illegal, eret, roli, rol),
    op4( flushp, ret, nor, mulxuu),
    op4( cmpge, bret, illegal, ror),
    op4( flushi, jmp, and, illegal),

    op4( cmplt, illegal, slli, sll),
    op4(illegal, illegal, or, mulxsu),
    op4( cmpne, illegal, srli, srl),
    op4( nextpc, callr, xor, mulxss),

    op4( cmpeq, illegal, illegal, illegal),
    op4( divu, div, rdctl, mul),
    op4( cmpgeu, initi, illegal, illegal),
    op4(illegal, trap, wrctl, illegal),

    op4( cmpltu, add, illegal, illegal),
    op4( break, illegal, sync, illegal),
    op4(illegal, sub, srai, sra),
    op4(illegal, illegal, illegal, illegal),

};
#undef op
#undef op4
//#endif

void Nios2fIss::run()
{
    func_t func = opcodeTable[m_instruction.i.op];
    m_gprA = r_gpr[m_instruction.i.a];
    m_gprB = r_gpr[m_instruction.i.b];
    m_branchTaken = false;

    (this->*func)();

#if SOCLIB_MODULE_DEBUG
    std::cout
		<< m_name << std::hex
		<< " PC: " << r_pc << " branchAddress: " <<m_branchAddress << " branchTaken: " << m_branchTaken
        << std::endl
        << " exceptionSignal: " << std::hex << m_exceptionSignal
        << std::endl << std::endl << std::dec;
#endif

}

void  Nios2fIss::dump() const
{
	Nios2fIss::dumpInstruction();
	Nios2fIss::dumpRegisters();
}


void  Nios2fIss::dumpInstruction() const
{
	std::cout
        << m_name   	<< std::hex << std::showbase
        << " PC: " << r_pc << " (r_count: " << std::dec << r_count << std::hex << std::showbase << ")"
        << " Ins: " << m_instruction.ins << std::endl
        << "    op:  " << m_instruction.j.op << " (" << opNameTable[m_instruction.j.op] << ")" << std::endl
        << std::dec
        << "    i type - a: " << m_instruction.i.a << " b: "<<m_instruction.i.b<< " i: "<<m_instruction.i.imm16 << std::endl << std::dec
        << "    r type - a: " << m_instruction.r.a << " b: "<<m_instruction.r.b<< " c: "<<m_instruction.r.c<< " sh: "<<m_instruction.r.sh<< " opx: "<<m_instruction.r.opx << " (" << opxNameTable[m_instruction.r.opx] << ")"
        << std::endl << std::hex << "        V m_gprA: " << m_gprA << " m_gprB: " << m_gprB << std::dec
        << std::endl;
}


void  Nios2fIss::dumpRegisters() const
{
	for (int i=0;i<8;i++) {
		for (int j=0;j<4;j++)
			printf("R%2.2d=%8.8x ",i*4+j,(int)r_gpr[i*4+j]);
		printf("\n");
	}
//	for (int j=0;j<6;j++)
//		printf("CT%d=%8.8x ",j,(int)r_ctl[j]);
	printf("\n");
	printf("pc=%8.8x ",r_pc);
	printf("\n");
}


//static inline int32_t sign_ext16(uint16_t i)
//{
//	return (int16_t)i;
//}

void Nios2fIss::do_mem_access( addr_t address,
                               int byte_count,
                               int sign_extend,
                               int dest_reg,
                               int dest_byte_in_reg,
                               data_t wdata,
                               enum DataOperationType operation )
{

    int byte_le = address&3;
    assert( (byte_count + byte_le) <= 4 );

    m_dreq.addr = address & (~3);
    m_dreq.be = (((1<<byte_count)-1) << byte_le) & 0xf;

    m_dreq.valid = true;
    m_dreq.wdata = wdata << (8 * byte_le);
    m_dreq.type = operation;
    m_dreq.mode = r_bus_mode;

#ifdef SOCLIB_MODULE_DEBUG
    std::cout
        << name()
        << " do_mem_access: " << m_dreq
        << " off: " << byte_le
        << " sign_ext: " << sign_extend
        << " dest: " << dest_reg
        << std::endl;
#endif

    r_mem_byte_le = byte_le;
    r_mem_byte_count = byte_count;
    r_mem_offset_byte_in_reg = dest_byte_in_reg;
    r_mem_do_sign_extend = sign_extend;
    r_mem_dest = dest_reg;
    r_mem_unsigned = (sign_extend<0) ? true : false;
}

void  Nios2fIss::op_addi()
{
	// ADD with immediat value (rB<-rA+IMM16) p.8-10
	uint64_t tmp = (uint64_t)m_gprA + (uint64_t)sign_ext(m_instruction.i.imm16, 16);
	//      if ( (bool)(tmp & (uint64_t)((uint64_t)1<<32)) != (bool)(tmp & (1<<31)) )
	//	exceptionSignal = X_OV;
	//      else
	r_gpr[m_instruction.i.b] = tmp;

}

void  Nios2fIss::op_andhi()
{
	// AND(rB<-rA&(IMM16:0x0000)) p.8-12
	r_gpr[m_instruction.i.b] = m_gprA & (m_instruction.i.imm16 << 16);
}

void  Nios2fIss::op_andi()
{
	// Bitwise logical and immediate (rB<-rA&(0x0000:IMM16)) p.8-13
	r_gpr[m_instruction.i.b] = m_gprA & m_instruction.i.imm16;
}

void  Nios2fIss::op_beq()
{
	// Branch if EQual p.8-14
	if ( m_gprA == m_gprB ) {
		m_branchAddress = sign_ext(m_instruction.i.imm16, 16) + r_pc + 4;
		m_branchTaken = true;
	}
}

void  Nios2fIss::op_bge()
{
	// Branch if Greater or Equal signed p.8-15
	if ( (int32_t)m_gprA >= (int32_t)m_gprB ) {
		m_branchAddress = sign_ext(m_instruction.i.imm16, 16) + r_pc + 4;
		m_branchTaken = true;
	}
}

void  Nios2fIss::op_bgeu()
{
	// Branch if Greater or Equal Unsigned p.8-16
	if ( m_gprA >= m_gprB ) {
		m_branchAddress = sign_ext(m_instruction.i.imm16, 16) + r_pc + 4;
		m_branchTaken = true;
	}
}

void  Nios2fIss::op_blt()
{
	// Branch if Less Than signed p.8-21
	if ( (int32_t)m_gprA < (int32_t)m_gprB ) {
		m_branchAddress = sign_ext(m_instruction.i.imm16, 16) + r_pc + 4;
		m_branchTaken = true;
	}
}

void  Nios2fIss::op_bltu()
{
	// Branch if Less Than Unsigned p.8-22
	if ( m_gprA < m_gprB ) {
		m_branchAddress = sign_ext(m_instruction.i.imm16, 16) + r_pc + 4;
		m_branchTaken = true;
	}
}

void  Nios2fIss::op_bne()
{
	// Branch if Not Equal p.8-23
	if ( m_gprA != m_gprB ) {
		m_branchAddress = sign_ext(m_instruction.i.imm16, 16) + r_pc + 4;
		m_branchTaken = true;
	}
}

void  Nios2fIss::op_br()
{
	// Unconditional Branch p.8-24
	m_branchAddress = sign_ext(m_instruction.i.imm16, 16) + r_pc + 4;
	m_branchTaken = true;
}

void  Nios2fIss::op_call()
{
	// call subroutine p.8-27
	r_gpr[RA] = r_pc + 4; /* return address register (ra=r31)<-PC + 4  */
	m_branchAddress = ((0xF0000000 & r_pc) | (m_instruction.j.immed26 * 4)); /* PC<-(PC[31..28]:IMM26x4) */
	m_branchTaken = true;

	// 2 cycles per instruction
	setInsDelay( 2);
}

void  Nios2fIss::op_cmpeqi()
{
	// Compare Equal Immediate p.8-30
	r_gpr[m_instruction.i.b] = (bool) ( (int32_t)m_gprA == (int32_t)sign_ext(m_instruction.i.imm16, 16) );
}

void  Nios2fIss::op_cmpgei()
{
	// Compare greater than or equal signed immediate p.8-32
	r_gpr[m_instruction.i.b] = (bool) ( (int32_t)m_gprA >= (int32_t)sign_ext(m_instruction.i.imm16, 16) );
}

void  Nios2fIss::op_cmpgeui()
{
	// Compare Greater than or Equal Ubnsigned Immediate p.8-34
	r_gpr[m_instruction.i.b] = (bool) ( m_gprA >= m_instruction.i.imm16 );
}

void  Nios2fIss::op_cmplti()
{
	// Compare Less Than Signed Immediate p.8-44
	r_gpr[m_instruction.i.b] = (bool) ( (int32_t)m_gprA < (int32_t)sign_ext(m_instruction.i.imm16, 16) );
}

void  Nios2fIss::op_cmpltui()
{
	// Compare Less Than Unsigned Immediate p.8-46
	r_gpr[m_instruction.i.b] = (bool) ( m_gprA < m_instruction.i.imm16);
}

void  Nios2fIss::op_cmpnei()
{
	// Compare Not Equal Immediate p.8-48
	r_gpr[m_instruction.i.b] = (bool) ( (int32_t)m_gprA != (int32_t)sign_ext(m_instruction.i.imm16, 16) );
}

void  Nios2fIss::op_flushd()
{
	// Flush the data cache line p.8-53
	/* instruction implemented in genMoore() */
}

void  Nios2fIss::op_flushda()
{
	// Flush the data cache line p.8-53
}

void  Nios2fIss::op_initd()
{
    if (r_status.u) {
        m_exceptionSignal = X_SOINST;
        return;
    }

	// Initialize data cache line p.8-57
	/* instruction implemented in genMoore() */
}

void  Nios2fIss::op_jmpi()
{
    // Computed Jump p.8-58
    /* to jump to r31 (return adress (ra) register) use ret instruction */
    m_branchAddress = ((0xF0000000 & r_pc) | (m_instruction.j.immed26 * 4));
    m_branchTaken=true;

    // 2 cycles per instruction
    setInsDelay( 2 );
}

void  Nios2fIss::op_ldb()
{
	// Load rB<-Mem8[rA+IMM16] p.8-60
	uint32_t address = m_gprA + sign_ext(m_instruction.i.imm16, 16);
    do_mem_access(address, 1, 1, m_instruction.i.b, 0, 0, DATA_READ);

#if SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " load byte @" << address
        << " -> r" << std::dec << m_instruction.i.b
        << std::endl;
#endif

	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);
}

void  Nios2fIss::op_ldbio()
{
	// Load byte from I/O peripheral p.8-60
	SOCLIB_WARNING("LDBIO Not implementable");

	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);
}

void  Nios2fIss::op_ldbu()
{
	// Load rB<-0x000000:Mem8[rA+IMM16] p.8-61
	uint32_t address = m_gprA + sign_ext(m_instruction.i.imm16, 16);
    do_mem_access(address, 1, -1, m_instruction.i.b, 0, 0, DATA_READ);

#if SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " load byte unsigned @" << address
        << " -> r" << std::dec << m_instruction.i.b
        << std::endl;
#endif
	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);
}

void  Nios2fIss::op_ldbuio()
{
	// Load unsigned byte from I/O peripheral p.8-61
	SOCLIB_WARNING("LDBUIO Not implementable");

}

void  Nios2fIss::op_ldh()
{
	// Load rB<-Mem16[rA+IMM16] p.8-62
	uint32_t address = m_gprA + sign_ext(m_instruction.i.imm16, 16);
	if (check_align(address, 2, X_MALLDATAADR))
        return;
    do_mem_access(address, 2, 2, m_instruction.i.b, 0, 0, DATA_READ);

#if SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " load half @" << address
        << " -> r" << std::dec << m_instruction.i.b
        << std::endl;
#endif
	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);
}

void  Nios2fIss::op_ldhio()
{
	// Load unsigned halfword from I/O peripheral p.8-62
	SOCLIB_WARNING("LDHIO Not implementable");

	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);

}

void  Nios2fIss::op_ldhu()
{
	// Load rB<-0x0000:Mem16[rA+IMM16] p.8-63
	uint32_t address = m_gprA + sign_ext(m_instruction.i.imm16, 16);
	if (check_align(address, 2, X_MALLDATAADR))
        return;
    do_mem_access(address, 2, -2, m_instruction.i.b, 0, 0, DATA_READ);

#if SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " load half unsigned @" << address
        << " -> r" << std::dec << m_instruction.i.b
        << std::endl;
#endif
	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);
}

void  Nios2fIss::op_ldhuio()
{
	// Load rB<-0x0000:Mem16[rA+IMM16] p.8-63
	SOCLIB_WARNING("LDHUIO Not implementable");

	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);
}

void  Nios2fIss::op_ldw()
{
	// Load(rB<_Mem32[rA+IMM16]) p.8.64
	uint32_t address = m_gprA + sign_ext(m_instruction.i.imm16, 16);
	if (check_align(address, 4, X_MALLDATAADR))
        return;
    do_mem_access(address, 4, 0, m_instruction.i.b, 0, 0, DATA_READ);

#if SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " load word @" << address
        << " -> r" << std::dec << m_instruction.i.b
        << std::endl;
#endif
	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);
}

void  Nios2fIss::op_ldwio()
{
	// Load(rB<_Mem32[rA+IMM16]) p.8.64
	SOCLIB_WARNING("LDWIO Not implementable");

	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);
}

void  Nios2fIss::op_muli()
{
	// Mulitply immediate p.8-71
	uint64_t res, a, b;
	a = (uint64_t)m_gprA;
	b = (uint64_t)sign_ext(m_instruction.i.imm16, 16);
	res = a*b;
	r_gpr[m_instruction.i.b] = res;

	// late result management
	m_listOfLateResultInstruction.add(m_instruction.i.b);
}

void  Nios2fIss::op_orhi()
{
	// Bitwise logical or immediate into high halfword (rB<-rA|(IMM16:0x0000)) p.8-79
	r_gpr[m_instruction.i.b] = m_gprA | (m_instruction.i.imm16 << 16);
}

void  Nios2fIss::op_ori()
{
	// Bitwise logical or immediate (rB<-rA|(0x0000:IMM16)) p.8-80
	r_gpr[m_instruction.i.b] = m_gprA  | m_instruction.i.imm16;
}

void  Nios2fIss::op_stb()
{
	// Store Mem8[rA+IMM16]<-rB(7..0) p.8-92
	uint8_t tmp = m_gprB&0xff;
	uint32_t address = m_gprA + sign_ext(m_instruction.i.imm16, 16);
	do_mem_access(address, 1, 0, 0, 0, tmp, DATA_WRITE);

#if SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " store byte @" << address
        << ": " << tmp
        << std::endl;
#endif
}

void  Nios2fIss::op_stbio()
{
	SOCLIB_WARNING("STBIO Not implementable");
}

void  Nios2fIss::op_sth()
{
	// Store Mem16[rA+IMM16]<-rB(15..0) p.8-93
	uint16_t tmp = m_gprB&0xffff;
	uint32_t address = m_gprA + sign_ext(m_instruction.i.imm16, 16);
	if (check_align(address, 2, X_MALLDESTADR))
        return;
	do_mem_access(address, 2, 0, 0, 0, tmp, DATA_WRITE);

#if SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " store half @" << address
        << ": " << tmp
        << std::endl;
#endif
}

void  Nios2fIss::op_sthio()
{
	SOCLIB_WARNING("STHIO Not implementable");
}

void  Nios2fIss::op_stw()
{
	// Store Mem32[rA+IMM16]<-rB p.8-94
	uint32_t address = m_gprA + sign_ext(m_instruction.i.imm16, 16);
	if (check_align(address, 4, X_MALLDESTADR))
        return;
	do_mem_access(address, 4, 0, 0, 0, m_gprB, DATA_WRITE);

#if SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " store word @" << address
        << ": " << m_gprB
        << std::endl;
#endif
}

void  Nios2fIss::op_stwio()
{
	SOCLIB_WARNING("STWIO Not implementable");
}

void  Nios2fIss::op_xorhi()
{
	// Bitwise logical exclusive or immediate into high halfword (rB<-rA xor (IMM16:0x0000)) p.8-101
	r_gpr[m_instruction.i.b] = m_gprA ^ (m_instruction.i.imm16 << 16);
}

void  Nios2fIss::op_xori()
{
	// Bitwise logical exclusive or immediate (rB<-rA xor (0x0000:IMM16)) p.8-102
	r_gpr[m_instruction.i.b] = m_gprA ^ m_instruction.i.imm16;
}

void  Nios2fIss::op_illegal()
{
	m_exceptionSignal = X_ILLEGAL;
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

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
 * - summer 2006: First version developed on a first SoCLib template by Reeb and Charot.
 * - september 2007: the model has been completely rewritten and adapted to the SocLib
 * 						rules defined during the first months of the SocLib ANR project.
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

#ifndef _SOCLIB_NIOS2_ISS_H_
#define _SOCLIB_NIOS2_ISS_H_

#include <fstream>
#include <cassert>

#include "iss2.h"
#include "soclib_endian.h"
#include "register.h"

//#define DEBUGCustom

// this class is used to manage a list of late result instructions
class lateResultInstruction {
public:
	uint32_t reg;
	uint32_t cycle;

	lateResultInstruction *next;

	void add(uint32_t);
	void update();
	void print();
};

extern lateResultInstruction *m_startOflriList;

namespace soclib {
namespace common {

class Nios2fIss : public Iss2 {
public:
	static const int n_irq = 32;

private:
	enum Addresses {
		RESET_ADDRESS = 0x00802000,
		EXCEPTION_HANDLER_ADDRESS = 0x00800820,
		BREAK_HANDLER_ADDRESS = 0x01000020
	};

	// general-purpose registers: some registers having names recognized by the assembler
	// they can be used by the simulator
	enum genPurposeRegName {
		ET = 24,
		BT = 25,
		GP = 26,
		SP = 27,
		FP = 28,
		EA = 29,
		BA = 30,
		RA = 31
	};

	// NiosII processor handbook page 3-35
	enum ExceptCause {
		X_RESET  = 0, // Interrupt
		X_PRESET = 1, // Processor-only Interrupt
		X_INT    = 2, // Interrupt
		X_TR     = 3, // Trap
		X_UIINST = 4, // Unimplemented instruction
		X_ILLEGAL = 5, // Illegal instruction
		X_MALLDATAADR = 6, // Misaligned data address
		X_MALLDESTADR = 7, // Misaligned destination address
		X_DIV = 8, // Division
		X_SOINSTADDR = 9, // Supervisor-only instruction address
		X_SOINST = 10, // Supervisor-only instruction
		X_SODATAADDR = 11, // Supervisor-only data address
		X_FTLBMISS = 12, // SFast TLB miss
		X_TLBPVIOLE = 13, // TLB permission violation (execute)
		X_TLBPVIOLR = 14, // TLB permission violation (read)
		X_TLBPVIOLW = 15, // TLB permission violation (write)
		X_MPURVIOLI = 16, // MPU Region Violation (instruction)
		X_MPURVIOLD = 17, // MPU Region Violation (data)
        X_BR = 18,
        NO_EXCEPTION,
	};

//	enum Iss2::ExecMode r_bus_mode;

	// member variables (internal registers)

	uint32_t r_gpr[32]; // General Registers
	uint32_t r_cr[32]; // Coprocessor Registers used by custom instructions

	uint32_t r_pc; // Program Counter

	struct DataRequest m_dreq;
	int r_mem_do_sign_extend;
	int r_mem_byte_le;
	int r_mem_byte_count;
	int r_mem_offset_byte_in_reg;
	uint32_t r_mem_dest; // Data Cache destination register (read)
	bool r_mem_unsigned; // unsigned or signed memory request

	addr_t r_ebase;

	typedef union {
		struct {
			union {
				PACKED_BITFIELD(
                                uint32_t immed26:26,
                                uint32_t op:6,
                                ) j;
				PACKED_BITFIELD(
                                uint32_t a:5,
                                uint32_t b:5,
                                uint32_t imm16:16,
                                uint32_t op:6,
                                ) i;
				PACKED_BITFIELD(
                                uint32_t a:5,
                                uint32_t b:5,
                                uint32_t c:5,
                                uint32_t opx:6,
                                uint32_t sh:5,
                                uint32_t op:6,
                                ) r;
			}__attribute__((packed));
		}__attribute__((packed));
		uint32_t ins;
	} ins_t;

    enum UMode {
        NIOS2_UM_SUPERVISOR,
        NIOS2_UM_USER,
    };

    enum Nios2Mode {
        NIOS2_SUPERVISOR,
        NIOS2_USER,
    };

    typedef REG32_BITFIELD(
    	uint32_t reserved:8,
        uint32_t rsie:1,
        uint32_t nmi:1,
        uint32_t prs:6,
        uint32_t crs:6,
        uint32_t il:6,
        uint32_t ih:1,
        uint32_t eh:1,
        uint32_t u:1,
        uint32_t pie:1,
        ) status_t;

    enum Iss2::ExecMode r_bus_mode;
    enum Nios2Mode r_cpu_mode;

	status_t r_status; // control registers
	uint32_t r_estatus;
	uint32_t r_bstatus;
	uint32_t r_ienable;
	uint32_t r_ipending;
	uint32_t r_cpuid;
	uint32_t r_reserved_6;
	uint32_t r_exception;
	uint32_t r_pteaddr;
	uint32_t r_tlbacc;
	uint32_t r_tlbmisc;
	uint32_t r_reserved_11;
	uint32_t r_badaddr;
	uint32_t r_config;
	uint32_t r_mpubase;
	uint32_t r_mpuacc;
	uint32_t r_reserved_16;
	uint32_t r_reserved_17;
	uint32_t r_reserved_18;
	uint32_t r_reserved_19;
	uint32_t r_reserved_20;
	uint32_t r_reserved_21;
	uint32_t r_reserved_22;
	uint32_t r_reserved_23;
	uint32_t r_reserved_24;
	uint32_t r_reserved_25;
	uint32_t r_reserved_26;
	uint32_t r_reserved_27;
	uint32_t r_reserved_28;
	uint32_t r_reserved_29;
	uint32_t r_reserved_30;
	uint32_t r_reserved_31; // used to manage r_count

    uint32_t r_count;

    uint32_t m_ins_delay; // Instruction latency simulation

	// member variables used for communication between
	// member functions (they are not registers)

	ins_t m_instruction;
	enum ExceptCause m_exceptionSignal;
	uint32_t m_gprA;
	uint32_t m_gprB;
	uint32_t m_branchAddress;
	uint32_t m_next_pc;
	bool m_branchTaken;
	bool m_hazard;

	uint32_t m_rdata;
	uint32_t m_irq;
	bool m_ibe;
	bool m_dbe;

	bool m_ireq_ok;
    bool m_dreq_ok;
    static int m_bootstrap_cpu_id;
    bool m_reset_wait_irq;       // procrssor in idle state after reset

    lateResultInstruction m_listOfLateResultInstruction;

	std::ofstream m_log;

	// control signal and data used by custom instruction
	union {
		int32_t i;
		float f;
	} m_resultCustomInstruction, m_operandA, m_operandB;
	int m_readra, m_readrb, m_writerc;
	bool loadingFromCustomRegister;

    inline bool check_align(addr_t address, unsigned align, ExceptCause fault)
    {
        if ( (address)%(align) ) {
            m_dreq.addr = address;
            m_exceptionSignal = fault;
            r_badaddr = address;
            return true;
        }
        return false;
    }

public:
	Nios2fIss(const std::string &name, uint32_t ident);
	~Nios2fIss();

    void dump() const;
	void reset();

	uint32_t executeNCycles(uint32_t ncycle,
                            const struct InstructionResponse &, const struct DataResponse &,
                            uint32_t irq_bit_field);

	inline void getRequests(struct InstructionRequest &ireq,
                            struct DataRequest &dreq) const {
		ireq.valid = !m_reset_wait_irq;
		ireq.addr = r_pc;

		dreq = m_dreq;
		dreq.addr &= ~(uint32_t)3;
	}

	void setDataResponse(const struct DataResponse &);

	inline void setWriteBerr() {
		m_dbe = true;
	}

	// processor internal registers access API, used by debugger
	static const unsigned int s_sp_register_no = 27;
	static const unsigned int s_fp_register_no = 28;
	static const unsigned int s_pc_register_no = 32;

	static const Iss2::debugCpuEndianness s_endianness = Iss2::ISS_LITTLE_ENDIAN;

	inline unsigned int debugGetRegisterCount() const {
		return 32+1+16;
	}

	uint32_t debugGetRegisterValue(unsigned int reg) const;

	size_t debugGetRegisterSize(unsigned int reg) const {
		return 32;
	}

	void debugSetRegisterValue(unsigned int reg, uint32_t value);

    static inline void setBoostrapCpuId(int id = -1)
    {
        m_bootstrap_cpu_id = id;
    }

private:
	void run();
    void dumpInstruction() const;
    void dumpRegisters() const;
    bool handle_exception();

	inline void setInsDelay(uint32_t delay) {
		assert(delay > 0);
		m_ins_delay = delay-1;
	}

	typedef void (Nios2fIss::*func_t)();

	static func_t const opcodeTable[64];
	static func_t const RTypeTable[64];
	static func_t const customInstTable[256];

	void do_mem_access( addr_t address,
                        int byte_count,
                        int sign_extend,
                        int dest_reg,
                        int dest_byte_in_reg,
                        data_t wdata,
                        enum DataOperationType operation );

    addr_t exceptBaseAddr() const;

    void op_addi();
	void op_andhi();
	void op_andi();
	void op_beq();
	void op_bge();
	void op_bgeu();
	void op_blt();
	void op_bltu();
	void op_bne();
	void op_br();
	void op_call();
	void op_cmpeqi();
	void op_cmpgei();
	void op_cmpgeui();
	void op_cmplti();
	void op_cmpltui();
	void op_cmpnei();
	void op_custom();
	void op_flushd();
	void op_flushda();
	void op_initd();
	void op_jmpi();
	void op_ldb();
	void op_ldbio();
	void op_ldbu();
	void op_ldbuio();
	void op_ldh();
	void op_ldhio();
	void op_ldhu();
	void op_ldhuio();
	void op_ldw();
	void op_ldwio();
	void op_muli();
	void op_ori();
	void op_orhi();
	void op_stb();
	void op_stbio();
	void op_sth();
	void op_sthio();
	void op_stw();
	void op_stwio();
	void op_xorhi();
	void op_xori();
	void op_RType();
	void op_illegal();

	void RType_add();
	void RType_and();
	void RType_break();
	void RType_bret();
	void RType_callr();
	void RType_cmpeq();
	void RType_cmpge();
	void RType_cmpgeu();
	void RType_cmplt();
	void RType_cmpltu();
	void RType_cmpne();
	void RType_eret();
	void RType_div();
	void RType_divu();
	void RType_flushi();
	void RType_flushp();
	void RType_initi();
	void RType_jmp();
	void RType_mul();
	void RType_mulxss();
	void RType_mulxsu();
	void RType_mulxuu();
	void RType_nextpc();
	void RType_nor();
	void RType_or();
	void RType_rdctl();
	void RType_ret();
	void RType_rol();
	void RType_roli();
	void RType_ror();
	void RType_sll();
	void RType_slli();
	void RType_sra();
	void RType_srai();
	void RType_srl();
	void RType_srli();
	void RType_sub();
	void RType_sync();
	void RType_trap();
	void RType_wrctl();
	void RType_xor();
	void RType_illegal();

    // custom to be added here
	void custom_illegal();
	void custom_sc();
	void custom_ll();

	static const char *opNameTable[64];
	static const char *opxNameTable[];

	uint32_t controlRegisterGet( uint32_t reg ) const;
	void controlRegisterSet( uint32_t reg, uint32_t value );

    void update_mode();

};

}}

#endif // _SOCLIB_NIOS2_ISS_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

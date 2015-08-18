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
 * TMS320C6X Instruction Set Simulator for the TMS320C6X processor core
 * developed for the SocLib Projet
 *
 * Copyright (C) IRISA/INRIA, 2008
 *         Francois Charot <charot@irisa.fr>
 *
 *
 * Maintainer: charot
 *
 * Functional description:
 * The following files:
 * 				tms320c62.h
 *              tms320c62.cpp
 *              tms320c62_instructions.cpp
 *              tms320c62_decoding.cpp
 *
 * define the Instruction Set Simulator for the TMS320C62 processor.
 *
 *
 */

#ifndef SOCLIB_TMS320C62_ISS_H_
#define SOCLIB_TMS320C62_ISS_H_

#include <systemc>
#include <iostream>
#include <iomanip>
#include <list>

#include "iss.h"
#include "soclib_endian.h"
#include "register.h"

#define TMS320C62_DEBUG 0

#define FETCH_PACKET_SIZE       8
#define FETCH_SIZE       32
#define FETCH_MASK       0xffffffe0

// control registers of the processor
#define AMR                0
#define CSR                1
#define IFR                2
#define ISR                2
#define ICR                3
#define IER                4
#define ISTP               5
#define IRP                6
#define NRP                7
#define PCE1              16
#define FADCR             18
#define FAUCR             19
#define FMCR              20

// allows choosing between the two register files
#define sideA                0
#define sideB                1

// define different execution unit types
#define LUNIT             0x06
#define MUNIT             0x00
#define DUNIT             0x10
#define DUNIT_LDSTOFFSET  0x03
#define DUNIT_LDSTBASEROFFSET  0x01
#define SUNIT             0x08
#define SUNIT_ADDK        0x14
#define SUNIT_IMMED       0x02
#define SUNIT_MVK         0x0a
#define SUNIT_BCOND       0x04
#define IDLEOP            0x78
#define NOP               0x000
#define IDLEUNIT          0x00
#define IDLEINST          0x7800
#define NOP1              0x00000000
#define NOP1_PARALLEL     0x00000001

// define condition register codes
#define CREG_B0           0x01
#define CREG_B1           0x02
#define CREG_B2           0x03
#define CREG_A1           0x04
#define CREG_A2           0x05

// serve to extract information from instructions
#define getUnit_2bit(inst)      (inst >> 2 & 0x03)
#define getUnit_3bit(inst)      (inst >> 2 & 0x07)
#define getUnit_4bit(inst)      (inst >> 2 & 0x0f)
#define getUnit_5bit(inst)      (inst >> 2 & 0x1f)
#define getUnit_11bit(inst)     (inst >> 2 & 0x07ff)
#define getUnit_16bit(inst)     (inst >> 2 & 0xffff)

// macros for often done stuff
#define SLSB16(DATA)           ((DATA << 16) >> 16) // take care of sign extension
#define ULSB16(DATA)           (DATA & 0x0000ffff)
#define MSB16(DATA)            (DATA >> 16)
#define BK0                    (state.c_regfile[AMR] >> 16 & 0x1f)
#define BK1                    (state.c_regfile[AMR] >> 21 & 0x1f)
#define ADDRESSING_MODE(REG,FILE) (state.c_regfile[AMR] >> ((REG-4)*2 + FILE*8) & 0x3)
#define MAX(a,b)               ((a)>(b)?(a):(b))
#define MIN(a,b)               ((a)<(b)?(a):(b))

// macros for grabbing components from the instructions
#define inst_creg(inst)        ((inst >> 29) & 0x07)
#define inst_z(inst)           ((inst >> 28) & 0x01)
#define inst_dst(inst)         ((inst >> 23) & 0x1f)
#define inst_src2(inst)        ((inst >> 18) & 0x1f)
#define inst_src1(inst)        ((inst >> 13) & 0x1f)
#define inst_x(inst)           ((inst >> 12) & 0x01)
#define inst_s(inst)           ((inst >>  1) & 0x01)

namespace soclib {
namespace common {

typedef union {
	struct {
		union {
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t src2:5,
					uint32_t src1:5,
					uint32_t x:1,
					uint32_t op:7,
					uint32_t unit:3,
					uint32_t s:1,
					uint32_t p:1,
			) l; // operations on the .L unit
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t src2:5,
					uint32_t src1:5,
					uint32_t x:1,
					uint32_t op:5,
					uint32_t unit:5,
					uint32_t s:1,
					uint32_t p:1,
			) m; // operations on the .M unit
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t src2:5,
					uint32_t src1:5,
					uint32_t op:6,
					uint32_t unit:5,
					uint32_t s:1,
					uint32_t p:1,
			) d; // operations on the .D unit
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t ucst15:15,
					uint32_t y:1,
					uint32_t op:3,
					uint32_t unit:2,
					uint32_t s:1,
					uint32_t p:1,
			) d_ldstOffset; // load/store with 18-bit offset on the .D unit
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t baseR:5,
					uint32_t offsetR:5,
					uint32_t mode:4,
					uint32_t r:1,
					uint32_t y:1,
					uint32_t op:3,
					uint32_t unit:2,
					uint32_t s:1,
					uint32_t p:1,
			) d_ldstBaseROffset; // load/store baseR + offseteR/cst on the .D unit
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t src2:5,
					uint32_t src1:5,
					uint32_t x:1,
					uint32_t op:6,
					uint32_t unit:4,
					uint32_t s:1,
					uint32_t p:1,
			) s; // operations on the .S unit
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t cst:16,
					uint32_t unit:5,
					uint32_t s:1,
					uint32_t p:1,
			) s_addk; // ADDK operations on the .S unit
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t src2:5,
					uint32_t csta:5,
					uint32_t cstb:5,
					uint32_t op:2,
					uint32_t unit:4,
					uint32_t s:1,
					uint32_t p:1,
			) s_immed; // bitfield operations (immediate form) on the .S unit
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t cst:16,
					uint32_t op:1,
					uint32_t unit:4,
					uint32_t s:1,
					uint32_t p:1,
			) s_mvk; // MVK and MVKH operations on the .S unit
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t cst:21,
					uint32_t unit:5,
					uint32_t s:1,
					uint32_t p:1,
			) s_bcond; // BCond disp on the .S unit
			PACKED_BITFIELD(
					uint32_t reserved:14,
					uint32_t zero:1,
					uint32_t src:4,
					uint32_t unit:12,
					uint32_t p:1
			) nop; // NOP format
			PACKED_BITFIELD(
					uint32_t reserved:14,
					uint32_t unit:16,
					uint32_t s:1,
					uint32_t p:1,
			) idle; // IDLE format
			PACKED_BITFIELD(
					uint32_t creg:3,
					uint32_t z:1,
					uint32_t dst:5,
					uint32_t src2:5,
					uint32_t src1:5,
					uint32_t x:1,
					uint32_t junk:10,
					uint32_t s:1,
					uint32_t p:1,
			) common; // common fields
		}__attribute__((packed));
	}__attribute__((packed));
	uint32_t ins;
} instruction_t;

class InstructionState {

public:
	instruction_t ins;
	bool execute;
	uint32_t result;
	uint32_t data[2];
	uint32_t address[2];
	bool firstLoadStore;
	bool secondLoadStore;
	uint32_t loadStorePosition;
	bool readInst;
	bool writeInst;

public:
	InstructionState & operator =(InstructionState const & instr) {
		ins = instr.ins;
		execute = instr.execute;
		result = instr.result;
		data[0] = instr.data[0];
		data[1] = instr.data[1];
		address[0] = instr.address[0];
		address[1] = instr.address[1];
		firstLoadStore = instr.firstLoadStore;
		secondLoadStore = instr.secondLoadStore;
		loadStorePosition = instr.loadStorePosition;
		readInst = instr.readInst;
		writeInst = instr.writeInst;
		return * this;
	}

	inline void print() const {
		std::cout << "Inst: " << std::hex << ins.ins << std::dec
				<< " Execute: " << execute << " " << " Result: " << result
				<< " " << " Data: " << std::hex << data[0] << " " << data[1]
				<< " Address: " << address[0] << " " << address[1] << std::dec
				<< " Position: " << loadStorePosition << " readInst: "
				<< readInst << " " << " writeInst: " << writeInst << " "
				<< std::endl;
	}
	inline void setExecute(bool x) {
		execute = x;
	}
	inline void setResult(uint32_t x) {
		result = x;
	}
	inline void setData(uint32_t x, uint32_t i) {
		data[i] = x;
	}
	inline void setAddress(uint32_t x, uint32_t i) {
		address[i] = x;
	}
	inline void setInstruction(instruction_t in) {
		ins = in;
	}
	inline void setFirstLoadStore(bool x) {
		firstLoadStore = x;
	}
	inline void setSecondLoadStore(bool x) {
		secondLoadStore = x;
	}
	inline void setLoadStorePosition(uint32_t x) {
		loadStorePosition = x;
	}
	inline void setIns(uint32_t in) {
		ins.ins = in;
		execute = false;
		data[0] = 0;
		data[1] = 0;
		address[0] = 0;
		address[1] = 0;
		readInst = false;
		writeInst = false;
	}
	inline void setReadInstInPacket(bool x) {
		readInst = x;
	}
	inline void setWriteInstInPacket(bool x) {
		writeInst = x;
	}

	inline bool isExecute() const {
		return execute;
	}
	inline uint32_t getResult() const {
		return result;
	}
	inline uint32_t getData(uint32_t i) const {
		return data[i];
	}
	inline uint32_t getAddress(uint32_t i) const {
		return address[i];
	}
	inline instruction_t getInstruction() const {
		return ins;
	}
	inline bool isFirstLoadStore() const {
		return firstLoadStore;
	}
	inline bool isSecondLoadStore() const {
		return secondLoadStore;
	}
	inline uint32_t getLoadStorePosition() const {
		return loadStorePosition;
	}
	inline bool isReadInstInPacketExecute() const {
		return readInst;
	}
	inline bool isWriteInstInPacketExecute() const {
		return writeInst;
	}

	inline bool isParallelInstruction(InstructionState inst) {
		return ((inst.ins.ins & 0x1 ) ? true : false);
	}
};

class PipelinePhase {

protected:
	uint32_t pc;
	uint32_t branch_address;
	bool branch_flag; // set if branch_address is valid
	bool stall;
	bool next_cycle_stall;
	bool issuing_nops;
	uint32_t inst_left;
	InstructionState inst_pack[FETCH_PACKET_SIZE];
	uint32_t loadStoreNumber;
	bool registerFileUpdate_delayed;

public:
	inline void setPC(uint32_t address) {
		pc = address;
	}
	inline void setBranchAddress(uint32_t address) {
		branch_address = address;
	}
	inline void setBranchFlag(bool flag) {
		branch_flag = flag;
	}
	inline void setStall(bool x) {
		stall = x;
	}
	inline void setNextCycleStall(bool x) {
		next_cycle_stall = x;
	}
	inline void setIssuingNop(bool flag) {
		issuing_nops = flag;
	}
	inline void setInstLeft(uint32_t value) {
		inst_left = value;
	}
	inline void setLoadStoreNumber(uint32_t value) {
		loadStoreNumber = value;
	}

	inline void setInstPack(InstructionState * inst) {
		for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
			inst_pack[i].ins.ins = inst[i].ins.ins;
			inst_pack[i].execute = inst[i].execute;
			inst_pack[i].result = inst[i].result;
			inst_pack[i].data[0] = inst[i].data[0];
			inst_pack[i].data[1] = inst[i].data[1];
			inst_pack[i].address[0] = inst[i].address[0];
			inst_pack[i].address[1] = inst[i].address[1];
			inst_pack[i].firstLoadStore = inst[i].firstLoadStore;
			inst_pack[i].secondLoadStore = inst[i].secondLoadStore;
			inst_pack[i].loadStorePosition = inst[i].loadStorePosition;
			inst_pack[i].readInst = inst[i].readInst;
			inst_pack[i].writeInst = inst[i].writeInst;
		}
	}

	inline void setInstInPack(InstructionState & inst, int i) {
		inst_pack[i] = inst;
	}
	inline void setLoadStoreDelayed(bool flag) {
		registerFileUpdate_delayed = flag;
	}

	inline uint32_t getPC() const {
		return pc;
	}
	inline uint32_t getBranchAddress() const {
		return branch_address;
	}
	inline bool isBranchFlag() const {
		return branch_flag;
	}
	inline bool isStall() const {
		return stall;
	}
	inline bool isNextCycleStall() const {
		return next_cycle_stall;
	}
	inline bool isIssuingNop() const {
		return issuing_nops;
	}
	inline uint32_t getInstLeft() const {
		return inst_left;
	}
	inline uint32_t getLoadStoreNumber() const {
		return loadStoreNumber;
	}
	inline uint32_t getLoadStoreDelayed() const {
		return registerFileUpdate_delayed;
	}

	inline InstructionState * getInstPack() {
		return (InstructionState *) inst_pack;
	}

	inline InstructionState * getInst(int i) {
		return &(inst_pack[i]);
	}

	inline void initState() {
		for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
			inst_pack[i].execute = false;
			inst_pack[i].result = 0;
			inst_pack[i].data[0] = 0;
			inst_pack[i].address[0] = 0;
			inst_pack[i].data[1] = 0;
			inst_pack[i].address[1] = 0;
			inst_pack[i].readInst = false;
			inst_pack[i].writeInst = false;
		}
	}

};

class ProcessorState : public PipelinePhase {
public:
	uint32_t regfile[2][16]; // the two register banks A and B
	uint32_t regfile_temp[2][16]; // regfile_temp is written to when instruction is executed
								  // its contents is written back to regfile each cycle
	uint32_t c_regfile[21]; // the control register file

	PipelinePhase PG;
	PipelinePhase PGPS;
	PipelinePhase PSPW;
	PipelinePhase PWPR;
	PipelinePhase DP;
	PipelinePhase PRDP;
	PipelinePhase DPDC;
	PipelinePhase DCE1;
	PipelinePhase E1E2;
	PipelinePhase E2E3;
	PipelinePhase E3E4;
	PipelinePhase E4E5;
	PipelinePhase E5;
	PipelinePhase E5n;
};

class register_entry {
public:
	uint32_t bank;
	uint32_t reg;
	uint32_t value;
};

class Tms320C6xIss : public soclib::common::Iss {

public:
	static const int n_irq = 32;

private:
	//-------To be defined
	enum Tms320Addresses {
		RESET_VECTOR = 0x00000020,
		EXCEPTION_VECTOR = 0x01000020,
		NOP_INS = 0x00000000,
		ERET_INS = 0xE800083A
	};
	//--------

	enum processor_state_fsm {
		idle_state = 0,
		usual_state = 1,
		two_datamemory_ldst_state = 2,
		two_datamemory_ldst_bis_state = 3,
		flush_pipeline_state = 4
	};

	ProcessorState state;

	uint32_t r_pc; // Program Counter
	bool newPC;
	uint32_t m_offset;
	bool m_instruction_packet_ready;

	enum DataAccessType r_mem_type; // Data Cache access type
	uint32_t r_mem_addr; // Data Cache address
	uint32_t r_mem_wdata; // Data Cache data value (write)
	uint32_t r_mem_dest; // Data Cache destination register (read)
	bool r_dbe; // Asynchronous Data Bus Error (write)
	bool r_mem_req; // memory request ?
	bool r_mem_unsigned; // unsigned or signed memory request

	uint32_t data_from_mem;
	uint32_t data_from_mem_e3[2];
	bool ldstAlreadyExecuted;
	uint32_t ldstore;

	bool multiple_loadstore_instructions;
	bool e3_phase_stalled;
	bool e4_phase_stalled;
	bool e5_phase_stalled;
	bool end_of_loop;
	uint32_t r_count;
	uint32_t temp_pc;
	uint32_t temp_next_pc;
	bool restoreState;
	bool delayed;
	bool registerUpdatePostponed;
	uint32_t m_interrupt_delayed;
	uint32_t cycle_to_flush_pipeline;
	uint32_t m_run_state;
	bool tworeadinst_in_packet;
	bool twowriteinst_in_packet;

	int id;

	uint32_t m_ins_delay; // Instruction latency simulation
	uint32_t m_rdata;
	uint32_t m_irq;
	bool m_ibe;
	bool m_dbe;

	instruction_t m_instruction[FETCH_PACKET_SIZE];
	instruction_t m_temp_instruction[FETCH_PACKET_SIZE];
	InstructionState instructionState;

	uint32_t m_exceptionSignal;
	uint32_t m_branchAddress;
	uint32_t m_next_pc;
	bool m_branchTaken;
	bool m_hazard;
	uint32_t m_exec_cycles; // number of executed instructions

	std::list <register_entry> m_list_reg;

public:
	Tms320C6xIss(uint32_t ident);

	void step();
	void reset();

	inline void nullStep(uint32_t time_passed = 1) {
		if (m_ins_delay) {
			if (m_ins_delay > time_passed)
				m_ins_delay -= time_passed;
			else
				m_ins_delay = 0;
		}
		m_hazard = false;
		r_count += time_passed;
	}

	inline uint32_t isBusy() {
		return m_ins_delay || !isInstPacketReady();
	}

	inline bool isInstPacketReady() {
		return m_instruction_packet_ready;
	}

	inline void getInstructionRequest(bool &req, uint32_t &address) const {
#if TMS320C62_DEBUG
		std::cout << "getInstructionRequest"<< std::endl;
#endif
		req = true;
		address = (r_pc & FETCH_MASK) + m_offset*4;
#if TMS320C62_DEBUG
		std::cout << " inst at @ " << std::hex << address << std::dec << " requested (offset " << m_offset << ")" << std::endl;
#endif
	}

	virtual inline void getDataRequest(bool &valid, enum DataAccessType &type,
			uint32_t &address, uint32_t &wdata) const {
		valid = r_mem_req;
		address = r_mem_addr;
		wdata = r_mem_wdata;
		type = r_mem_type;
	}

	inline void setWriteBerr() {
		r_dbe = true;
	}

	inline void setIrq(uint32_t irq) {
		m_irq = irq;
	}

	inline void setInstruction(bool error, uint32_t val) {
#if TMS320C62_DEBUG
		std::cout << "setInstruction"<< std::endl;
#endif
		if (newPC || m_offset >= FETCH_PACKET_SIZE) {
			m_offset = 0;
			newPC = false;
		}
		m_ibe = error;
		m_temp_instruction[m_offset].ins = val;
		m_instruction_packet_ready = false;

#if TMS320C62_DEBUG
		std::cout << " instruction read: " << std::hex << val << std::dec << " (offset " << m_offset << ")" << std::endl;
#endif

		assert(m_offset <= FETCH_PACKET_SIZE);
		if (m_offset == FETCH_PACKET_SIZE - 1) {
			m_instruction_packet_ready = true;
			m_offset = 0;
		}
		else m_offset++;
	}

	inline void setInstructionPacket() {
#if TMS320C62_DEBUG
		std::cout << "setInstructionPacket"<< std::endl;
#endif
		for (int i = 0; i < FETCH_PACKET_SIZE; i++)
			m_instruction[i].ins = m_temp_instruction[i].ins;
#if TMS320C62_DEBUG
		std::cout << " instruction:" << std::hex;
		for (int i = 0; i < FETCH_PACKET_SIZE; i++)
		std::cout << m_instruction[i].ins << " ";
		std::cout << std::dec << std::endl;
#endif
	}

	void setDataResponse(bool error, uint32_t rdata);

	// processor internal registers access API, used by debugger
	inline unsigned int getDebugRegisterCount() const {
		return 32;
	}

	uint32_t getDebugRegisterValue(unsigned int reg) const;

	inline size_t getDebugRegisterSize(unsigned int reg) const {
		return 32;
	}

	void setDebugRegisterValue(unsigned int reg, uint32_t value);

	inline uint32_t getDebugPC() const {
		return r_pc;
	}

	inline void setDebugPC(uint32_t pc) {
		r_pc = pc;
	}

private:
	void run();

	inline void setInsDelay(uint32_t delay) {
		assert(delay > 0);
		m_ins_delay = delay-1;
	}

	typedef void (Tms320C6xIss::*func_t)(InstructionState *instState);

	static func_t const l_function_e1[128];
	static func_t const s_function_e1[64];
	static func_t const s_immed_function_e1[4];
	static func_t const s_mvk_function_e1[4];
	static func_t const s_addk_function_e1[1];
	static func_t const s_bcond_function_e1[1];
	static func_t const d_function_e1[64];
	static func_t const d_ldstOffset_function_e1[8];
	static func_t const d_ldstBaseROffset_function_e1[8];
	static func_t const d_ldstOffset_function_e2[8];
	static func_t const d_ldstBaseROffset_function_e2[8];
	static func_t const d_ldstOffset_function_e3[8];
	static func_t const d_ldstBaseROffset_function_e3[8];
	static func_t const d_ldstOffset_function_e4[8];
	static func_t const d_ldstBaseROffset_function_e4[8];
	static func_t const d_ldstOffset_function_e5[8];
	static func_t const d_ldstBaseROffset_function_e5[8];
	static func_t const m_function_e1[32];
	static func_t const m_function_e2[32];

	void op_abs_l_1a_e1(InstructionState *instState);
	void op_abs_l_38_e1(InstructionState *instState);
	void op_add_l_03_e1(InstructionState *instState);
	void op_add_l_23_e1(InstructionState *instState);
	void op_add_l_21_e1(InstructionState *instState);
	void op_add_l_02_e1(InstructionState *instState);
	void op_add_l_20_e1(InstructionState *instState);
	void op_addu_l_2b_e1(InstructionState *instState);
	void op_addu_l_29_e1(InstructionState *instState);
	void op_and_l_7b_e1(InstructionState *instState);
	void op_and_l_7a_e1(InstructionState *instState);
	void op_cmpeq_l_53_e1(InstructionState *instState);
	void op_cmpeq_l_52_e1(InstructionState *instState);
	void op_cmpeq_l_51_e1(InstructionState *instState);
	void op_cmpeq_l_50_e1(InstructionState *instState);
	void op_cmpgt_l_47_e1(InstructionState *instState);
	void op_cmpgt_l_46_e1(InstructionState *instState);
	void op_cmpgt_l_45_e1(InstructionState *instState);
	void op_cmpgt_l_44_e1(InstructionState *instState);
	void op_cmpgtu_l_4f_e1(InstructionState *instState);
	void op_cmpgtu_l_4e_e1(InstructionState *instState);
	void op_cmpgtu_l_4d_e1(InstructionState *instState);
	void op_cmpgtu_l_4c_e1(InstructionState *instState);
	void op_cmplt_l_57_e1(InstructionState *instState);
	void op_cmplt_l_56_e1(InstructionState *instState);
	void op_cmplt_l_55_e1(InstructionState *instState);
	void op_cmplt_l_54_e1(InstructionState *instState);
	void op_cmpltu_l_5f_e1(InstructionState *instState);
	void op_cmpltu_l_5e_e1(InstructionState *instState);
	void op_cmpltu_l_5d_e1(InstructionState *instState);
	void op_cmpltu_l_5c_e1(InstructionState *instState);
	void op_lmbd_l_6b_e1(InstructionState *instState);
	void op_lmbd_l_6a_e1(InstructionState *instState);
	void op_norm_l_63_e1(InstructionState *instState);
	void op_norm_l_60_e1(InstructionState *instState);
	void op_or_l_7f_e1(InstructionState *instState);
	void op_or_l_7e_e1(InstructionState *instState);
	void op_sadd_l_13_e1(InstructionState *instState);
	void op_sadd_l_31_e1(InstructionState *instState);
	void op_sadd_l_12_e1(InstructionState *instState);
	void op_sadd_l_30_e1(InstructionState *instState);
	void op_sat_l_40_e1(InstructionState *instState);
	void op_ssub_l_0f_e1(InstructionState *instState);
	void op_ssub_l_1f_e1(InstructionState *instState);
	void op_ssub_l_0e_e1(InstructionState *instState);
	void op_ssub_l_2c_e1(InstructionState *instState);
	void op_sub_l_07_e1(InstructionState *instState);
	void op_sub_l_17_e1(InstructionState *instState);
	void op_sub_l_27_e1(InstructionState *instState);
	void op_sub_l_37_e1(InstructionState *instState);
	void op_subu_l_2f_e1(InstructionState *instState);
	void op_subu_l_3f_e1(InstructionState *instState);
	void op_sub_l_06_e1(InstructionState *instState);
	void op_sub_l_24_e1(InstructionState *instState);
	void op_subc_l_4b_e1(InstructionState *instState);
	void op_xor_l_6f_e1(InstructionState *instState);
	void op_xor_l_6e_e1(InstructionState *instState);
	void op_illegal_l_e1(InstructionState *instState);

	void op_add_s_07_e1(InstructionState *instState);
	void op_add_s_06_e1(InstructionState *instState);
	void op_add2_s_01_e1(InstructionState *instState);
	void op_and_s_1f_e1(InstructionState *instState);
	void op_and_s_1e_e1(InstructionState *instState);
	void op_b_s_0d_e1(InstructionState *instState);
	void op_b_s_03_e1(InstructionState *instState);
	void op_clr_s_3f_e1(InstructionState *instState);
	void op_ext_s_2f_e1(InstructionState *instState);
	void op_extu_s_2b_e1(InstructionState *instState);
	void op_mvu_s_0f_e1(InstructionState *instState);
	void op_mvc_s_0e_e1(InstructionState *instState);
	void op_or_s_1b_e1(InstructionState *instState);
	void op_or_s_1a_e1(InstructionState *instState);
	void op_set_s_3b_e1(InstructionState *instState);
	void op_shl_s_33_e1(InstructionState *instState);
	void op_shl_s_31_e1(InstructionState *instState);
	void op_shl_s_13_e1(InstructionState *instState);
	void op_shl_s_32_e1(InstructionState *instState);
	void op_shl_s_30_e1(InstructionState *instState);
	void op_shl_s_12_e1(InstructionState *instState);
	void op_shr_s_37_e1(InstructionState *instState);
	void op_shr_s_35_e1(InstructionState *instState);
	void op_shr_s_36_e1(InstructionState *instState);
	void op_shr_s_34_e1(InstructionState *instState);
	void op_shru_s_27_e1(InstructionState *instState);
	void op_shru_s_25_e1(InstructionState *instState);
	void op_shru_s_26_e1(InstructionState *instState);
	void op_shru_s_24_e1(InstructionState *instState);
	void op_sshl_s_23_e1(InstructionState *instState);
	void op_sshl_s_22_e1(InstructionState *instState);
	void op_sub_s_17_e1(InstructionState *instState);
	void op_sub_s_16_e1(InstructionState *instState);
	void op_sub2_s_11_e1(InstructionState *instState);
	void op_xor_s_0b_e1(InstructionState *instState);
	void op_xor_s_0a_e1(InstructionState *instState);
	void op_illegal_s_e1(InstructionState *instState);

	void op_addk_s_addk_e1(InstructionState *instState);
	void op_bcond_s_bcond_e1(InstructionState *instState);
	void op_clr_s_immed_03_e1(InstructionState *instState);
	void op_ext_s_immed_01_e1(InstructionState *instState);
	void op_extu_s_immed_00_e1(InstructionState *instState);
	void op_set_s_immed_02_e1(InstructionState *instState);
	void op_mvk_s_mvk_00_e1(InstructionState *instState);
	void op_mvkh_s_mvk_01_e1(InstructionState *instState);

	void op_mpy_m_19_e1(InstructionState *instState);
	void op_mpyu_m_1f_e1(InstructionState *instState);
	void op_mpyus_m_1d_e1(InstructionState *instState);
	void op_mpysu_m_1b_e1(InstructionState *instState);
	void op_mpy_m_18_e1(InstructionState *instState);
	void op_mpysu_m_1e_e1(InstructionState *instState);
	void op_mpyh_m_01_e1(InstructionState *instState);
	void op_mpyhu_m_07_e1(InstructionState *instState);
	void op_mpyhus_m_05_e1(InstructionState *instState);
	void op_mpyhsu_m_03_e1(InstructionState *instState);
	void op_mpyhl_m_09_e1(InstructionState *instState);
	void op_mpyhlu_m_0f_e1(InstructionState *instState);
	void op_mpyhuls_m_0d_e1(InstructionState *instState);
	void op_mpyhslu_m_0b_e1(InstructionState *instState);
	void op_mpylh_m_11_e1(InstructionState *instState);
	void op_mpylhu_m_17_e1(InstructionState *instState);
	void op_mpyluhs_m_15_e1(InstructionState *instState);
	void op_mpylshu_m_13_e1(InstructionState *instState);
	void op_smpy_m_1a_e1(InstructionState *instState);
	void op_smpyhl_m_0a_e1(InstructionState *instState);
	void op_smpylh_m_12_e1(InstructionState *instState);
	void op_smpyh_m_02_e1(InstructionState *instState);
	void op_illegal_m_e1(InstructionState *instState);

	void op_add_d_10_e1(InstructionState *instState);
	void op_add_d_12_e1(InstructionState *instState);
	void op_addab_d_30_e1(InstructionState *instState);
	void op_addah_d_34_e1(InstructionState *instState);
	void op_addaw_d_38_e1(InstructionState *instState);
	void op_addab_d_32_e1(InstructionState *instState);
	void op_addah_d_36_e1(InstructionState *instState);
	void op_addaw_d_3a_e1(InstructionState *instState);
	void op_sub_d_11_e1(InstructionState *instState);
	void op_sub_d_13_e1(InstructionState *instState);
	void op_subab_d_31_e1(InstructionState *instState);
	void op_subah_d_35_e1(InstructionState *instState);
	void op_subaw_d_39_e1(InstructionState *instState);
	void op_subab_d_33_e1(InstructionState *instState);
	void op_subah_d_37_e1(InstructionState *instState);
	void op_subaw_d_3b_e1(InstructionState *instState);
	void op_illegal_d_e1(InstructionState *instState);

	void op_ldb_d_ldstOffset_02_e1(InstructionState *instState);
	void op_ldbu_d_ldstOffset_01_e1(InstructionState *instState);
	void op_ldh_d_ldstOffset_04_e1(InstructionState *instState);
	void op_ldhu_d_ldstOffset_00_e1(InstructionState *instState);
	void op_ldw_d_ldstOffset_06_e1(InstructionState *instState);
	void op_stb_d_ldstOffset_03_e1(InstructionState *instState);
	void op_sth_d_ldstOffset_05_e1(InstructionState *instState);
	void op_stw_d_ldstOffset_07_e1(InstructionState *instState);
	void op_ldb_d_ldstBaseROffset_02_e1(InstructionState *instState);
	void op_ldbu_d_ldstBaseROffset_01_e1(InstructionState *instState);
	void op_ldh_d_ldstBaseROffset_04_e1(InstructionState *instState);
	void op_ldhu_d_ldstBaseROffset_00_e1(InstructionState *instState);
	void op_ldw_d_ldstBaseROffset_06_e1(InstructionState *instState);
	void op_stb_d_ldstBaseROffset_03_e1(InstructionState *instState);
	void op_sth_d_ldstBaseROffset_05_e1(InstructionState *instState);
	void op_stw_d_ldstBaseROffset_07_e1(InstructionState *instState);

	void op_mpy_m_19_e2(InstructionState *instState);
	void op_mpyu_m_1f_e2(InstructionState *instState);
	void op_mpyus_m_1d_e2(InstructionState *instState);
	void op_mpysu_m_1b_e2(InstructionState *instState);
	void op_mpy_m_18_e2(InstructionState *instState);
	void op_mpysu_m_1e_e2(InstructionState *instState);
	void op_mpyh_m_01_e2(InstructionState *instState);
	void op_mpyhu_m_07_e2(InstructionState *instState);
	void op_mpyhus_m_05_e2(InstructionState *instState);
	void op_mpyhsu_m_03_e2(InstructionState *instState);
	void op_mpyhl_m_09_e2(InstructionState *instState);
	void op_mpyhlu_m_0f_e2(InstructionState *instState);
	void op_mpyhuls_m_0d_e2(InstructionState *instState);
	void op_mpyhslu_m_0b_e2(InstructionState *instState);
	void op_mpylh_m_11_e2(InstructionState *instState);
	void op_mpylhu_m_17_e2(InstructionState *instState);
	void op_mpyluhs_m_15_e2(InstructionState *instState);
	void op_mpylshu_m_13_e2(InstructionState *instState);
	void op_smpy_m_1a_e2(InstructionState *instState);
	void op_smpyhl_m_0a_e2(InstructionState *instState);
	void op_smpylh_m_12_e2(InstructionState *instState);
	void op_smpyh_m_02_e2(InstructionState *instState);
	void op_illegal_m_e2(InstructionState *instState);

	void op_ldb_d_ldstOffset_02_e2(InstructionState *instState);
	void op_ldbu_d_ldstOffset_01_e2(InstructionState *instState);
	void op_ldh_d_ldstOffset_04_e2(InstructionState *instState);
	void op_ldhu_d_ldstOffset_00_e2(InstructionState *instState);
	void op_ldw_d_ldstOffset_06_e2(InstructionState *instState);
	void op_stb_d_ldstOffset_03_e2(InstructionState *instState);
	void op_sth_d_ldstOffset_05_e2(InstructionState *instState);
	void op_stw_d_ldstOffset_07_e2(InstructionState *instState);
	void op_ldb_d_ldstBaseROffset_02_e2(InstructionState *instState);
	void op_ldbu_d_ldstBaseROffset_01_e2(InstructionState *instState);
	void op_ldh_d_ldstBaseROffset_04_e2(InstructionState *instState);
	void op_ldhu_d_ldstBaseROffset_00_e2(InstructionState *instState);
	void op_ldw_d_ldstBaseROffset_06_e2(InstructionState *instState);
	void op_stb_d_ldstBaseROffset_03_e2(InstructionState *instState);
	void op_sth_d_ldstBaseROffset_05_e2(InstructionState *instState);
	void op_stw_d_ldstBaseROffset_07_e2(InstructionState *instState);

	void op_ldb_d_ldstOffset_02_e3(InstructionState *instState);
	void op_ldbu_d_ldstOffset_01_e3(InstructionState *instState);
	void op_ldh_d_ldstOffset_04_e3(InstructionState *instState);
	void op_ldhu_d_ldstOffset_00_e3(InstructionState *instState);
	void op_ldw_d_ldstOffset_06_e3(InstructionState *instState);
	void op_stb_d_ldstOffset_03_e3(InstructionState *instState);
	void op_sth_d_ldstOffset_05_e3(InstructionState *instState);
	void op_stw_d_ldstOffset_07_e3(InstructionState *instState);
	void op_ldb_d_ldstBaseROffset_02_e3(InstructionState *instState);
	void op_ldbu_d_ldstBaseROffset_01_e3(InstructionState *instState);
	void op_ldh_d_ldstBaseROffset_04_e3(InstructionState *instState);
	void op_ldhu_d_ldstBaseROffset_00_e3(InstructionState *instState);
	void op_ldw_d_ldstBaseROffset_06_e3(InstructionState *instState);
	void op_stb_d_ldstBaseROffset_03_e3(InstructionState *instState);
	void op_sth_d_ldstBaseROffset_05_e3(InstructionState *instState);
	void op_stw_d_ldstBaseROffset_07_e3(InstructionState *instState);

	void op_ldb_d_ldstOffset_02_e4(InstructionState *instState);
	void op_ldbu_d_ldstOffset_01_e4(InstructionState *instState);
	void op_ldh_d_ldstOffset_04_e4(InstructionState *instState);
	void op_ldhu_d_ldstOffset_00_e4(InstructionState *instState);
	void op_ldw_d_ldstOffset_06_e4(InstructionState *instState);
	void op_stb_d_ldstOffset_03_e4(InstructionState *instState);
	void op_sth_d_ldstOffset_05_e4(InstructionState *instState);
	void op_stw_d_ldstOffset_07_e4(InstructionState *instState);
	void op_ldb_d_ldstBaseROffset_02_e4(InstructionState *instState);
	void op_ldbu_d_ldstBaseROffset_01_e4(InstructionState *instState);
	void op_ldh_d_ldstBaseROffset_04_e4(InstructionState *instState);
	void op_ldhu_d_ldstBaseROffset_00_e4(InstructionState *instState);
	void op_ldw_d_ldstBaseROffset_06_e4(InstructionState *instState);
	void op_stb_d_ldstBaseROffset_03_e4(InstructionState *instState);
	void op_sth_d_ldstBaseROffset_05_e4(InstructionState *instState);
	void op_stw_d_ldstBaseROffset_07_e4(InstructionState *instState);

	void op_ldb_d_ldstOffset_02_e5(InstructionState *instState);
	void op_ldbu_d_ldstOffset_01_e5(InstructionState *instState);
	void op_ldh_d_ldstOffset_04_e5(InstructionState *instState);
	void op_ldhu_d_ldstOffset_00_e5(InstructionState *instState);
	void op_ldw_d_ldstOffset_06_e5(InstructionState *instState);

	void op_ldb_d_ldstBaseROffset_02_e5(InstructionState *instState);
	void op_ldbu_d_ldstBaseROffset_01_e5(InstructionState *instState);
	void op_ldh_d_ldstBaseROffset_04_e5(InstructionState *instState);
	void op_ldhu_d_ldstBaseROffset_00_e5(InstructionState *instState);
	void op_ldw_d_ldstBaseROffset_06_e5(InstructionState *instState);

	void op_nothing_to_be_done(InstructionState *instState);

	std::ofstream memTrace;

	void print_creg(uint32_t inst);
	void iprint(const char *inst, int32_t src1, int32_t src2, int32_t dst,
	const char *unit, uint8_t s, char x, char src1or2);
	void iprintc1(const char *inst, int32_t src1, int32_t src2, int32_t dst,
	const char *unit, uint8_t s, char x, char src1or2);
	void iprintc2(const char *inst, int32_t src1, int32_t src2, int32_t dst,
	const char *unit, uint8_t s, char x, char src1or2);
	void iprintld(const char *inst, int32_t src1, int32_t src2, int32_t dst,
	const char *unit, uint8_t s,uint8_t x);
	void iprintldc(const char *inst, int32_t src1, int32_t src2, int32_t dst,
	const char *unit, uint8_t s, uint8_t x);
	void iprintst(const char *inst, int32_t src1, int32_t src2, int32_t dst,
	const char *unit, uint8_t s, uint8_t x);
	void iprintstc(const char *inst, int32_t src1, int32_t src2, int32_t dst,
	const char *unit, uint8_t s, uint8_t x);
	void instruction_print_mpy(uint32_t inst);
	void iprint4(const char *inst, int32_t csta, int32_t cstb, int32_t src2,int32_t dst,
		   const char *unit, uint8_t s, char x, char src1or2);
      void iprint2(const char *inst, int32_t src2, int32_t dst,
		   const char *unit, uint8_t s, char x, char src1or2);
      void iprint2c(const char *inst, int32_t src2, int32_t dst,
		    const char *unit, uint8_t s, char x, char src1or2);
      void iprint1(const char *inst, int32_t dst, const char *unit,
		   uint8_t s, char x, char src1or2);
      void iprint1c(const char *inst, int32_t dst, const char *unit,
		    char s, char x, char src1or2);
      void iprint0(const char *inst);
      void iprint01(const char *inst, uint32_t num);
      void instruction_print(uint32_t inst);
      void instruction_print_ld(uint32_t inst);
      void instruction_print_st(uint32_t inst);

      void pg_pipeline_phase();
      void ps_pipeline_phase();
      void pw_pipeline_phase();
      void pr_pipeline_phase();
      void dp_pipeline_phase();
      void dc_pipeline_phase();
      void e1_pipeline_phase();
      void e2_pipeline_phase();
      void e2bis_pipeline_phase();
      void e3_pipeline_phase();
      void e3bis_pipeline_phase();
      void e4_pipeline_phase();
      void e4bis_pipeline_phase();
      void e5_pipeline_phase();
      void e5bis_pipeline_phase();

      void registerFileUpdate();
      void tempRegisterFileUpdate();
      void loadStoreDelayed();

      void registerToBeUpdateLater(uint32_t bank, uint32_t reg, uint32_t  value);
      void writeRegister();

      void dumpRegisterFile() const;
      void dumpSavedRegisterFile() const;

      bool isConditionalInstruction(InstructionState state);

      static const char* name_l_function_e1[128];
      static const char* name_s_function_e1[64];
      static const char* name_s_immed_function_e1[4];
      static const char* name_s_mvk_function_e1[4];
      static const char* name_s_addk_function_e1[1];
      static const char* name_s_bcond_function_e1[1];
      static const char* name_d_function_e1[64];
      static const char* name_d_ldstOffset_function_e1[8];
      static const char* name_d_ldstBaseROffset_function_e1[8];
      static const char* name_d_ldstOffset_function_e2[8];
      static const char* name_d_ldstBaseROffset_function_e2[8];
      static const char* name_d_ldstOffset_function_e3[8];
      static const char* name_d_ldstBaseROffset_function_e3[8];
      static const char* name_d_ldstOffset_function_e4[8];
      static const char* name_d_ldstBaseROffset_function_e4[8];
      static const char* name_d_ldstOffset_function_e5[8];
      static const char* name_d_ldstBaseROffset_function_e5[8];
      static const char* name_m_function_e1[32];
      static const char* name_m_function_e2[32];

    };//class Tms320C6x

  }//namespace common
}//namespace soclib


#endif /* SOCLIB_CABA_TMS320C62_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

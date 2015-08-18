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
 * 		tms320c6x.h
 *              tms320c6x.cc
 *              tms320c6x_instructions.cc
 *              tms320c6x_decoding.cc
 *
 * define the Instruction Set Simulator for the Tms320C6xIss processor.
 *
 * 
 */

#include "tms320c62.h"
#include "arithmetics.h"

namespace soclib {
namespace common {

static const uint32_t NO_EXCEPT = (uint32_t)-1;

namespace {

static inline uint32_t align_data(uint32_t data, int shift, int width) {
	uint32_t mask = (1<<width)-1;
	uint32_t ret = data;

	ret >>= shift*width;
	return ret & mask;
}

static inline std::string mkname(uint32_t no) {
	char tmp[32];
	snprintf(tmp, 32, "Tms320c62_iss%d", (int)no);
	return std::string(tmp);
}
}

Tms320C6xIss::Tms320C6xIss(uint32_t ident) :
	Iss(mkname(ident), ident) {

}

void Tms320C6xIss::reset() {

	r_pc = 0;

	state.PG.setBranchAddress(0);
	state.PG.setBranchFlag(true);

	r_dbe = false;
	m_ibe = false;
	m_dbe = false;
	r_mem_req = false;
	m_ins_delay = 0;
	m_exec_cycles = 0;
	multiple_loadstore_instructions = false;
	cycle_to_flush_pipeline = 0;
	e3_phase_stalled = false;
	e4_phase_stalled = false;
	e5_phase_stalled = false;
	end_of_loop = false;
	registerUpdatePostponed = false;
	m_run_state = two_datamemory_ldst_state;
}

void Tms320C6xIss::setDataResponse(bool error, uint32_t data) {
	m_dbe = error;
	r_mem_req = false;
	if (error) {
		return;
	}

	// We write the  r_gp[i], and we detect a possible data dependency,
	// in order to implement the delayed load behaviour.
	if (isReadAccess(r_mem_type) ) {
#if TMS320C62_DEBUG
		std::cout
		<< m_name
		<< " in setDataResponse : "
		<< " read to " << r_mem_dest
		<< "(" << dataAccessTypeName(r_mem_type) << ")"
		<< " from " << std::hex << r_mem_addr
		<< ": " << data << std::dec
		<< std::endl;
#endif
	}
#if TMS320C62_DEBUG
	if ( isWriteAccess(r_mem_type) )
	std::cout
	<< m_name
	<< " write "
	<< "(" << dataAccessTypeName(r_mem_type) << ")"
	<< " to " << std::hex << r_mem_addr << std::dec
	<< " OK"
	<< std::endl;
#endif

	switch (r_mem_type) {
	case WRITE_BYTE:
	case WRITE_WORD:
	case WRITE_HALF:
	case LINE_INVAL:
		//        m_hazard = false;
		break;
	case READ_WORD:
		data_from_mem = data;
#if TMS320C62_DEBUG
		std::cout << " dataFromMem : " << std::hex << data_from_mem << std::dec << std::endl;
#endif
		break;
	case READ_LINKED:
		data_from_mem = data;
		break;
	case STORE_COND:
		data_from_mem = !data;
		break;
	case READ_BYTE:
		data_from_mem = r_mem_unsigned ? (data & 0xff) : sign_ext(data, 8);
		break;
	case READ_HALF:
		data_from_mem = r_mem_unsigned ? (data & 0xffff) : sign_ext(data, 16);
		break;
	}
}

void Tms320C6xIss::step() {
    setInstructionPacket();
	++r_count;
#if TMS320C62_DEBUG
	std::cout << " r_count: "<< r_count << std::endl;
#endif
	run();
	m_exec_cycles++;
#if TMS320C62_DEBUG
	std::cout << " m_exec_cycles: "<<m_exec_cycles << std::endl;
#endif
	r_pc = state.PSPW.getPC();
#if TMS320C62_DEBUG
	std::cout << " r_pc " << std::hex << r_pc << std::dec <<"  newPC=" << newPC << std::endl;
#endif
	newPC = true;
}

void Tms320C6xIss::dumpRegisterFile() const {
	std::cout << m_name << std::hex << std::showbase << std::endl;
	for (size_t bank=0; bank<=1; ++bank) {
		if (bank == 0)
			std::cout << "A bank ";
		else
			std::cout << "B bank ";
		for (size_t i=0; i<16; ++i)
			std::cout << " " << std::dec << i << ": " << std::hex
					<< std::showbase << state.regfile[bank][i];
		std::cout << std::dec << std::endl;
	}
}

uint32_t Tms320C6xIss::getDebugRegisterValue(unsigned int reg) const {

	return 0;
}

void Tms320C6xIss::setDebugRegisterValue(unsigned int reg, uint32_t value) {

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

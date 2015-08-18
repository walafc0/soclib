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

#include "niosII.h"
#include <math.h>
#include "arithmetics.h"

namespace soclib {
namespace common {

#define ci(x) &Nios2fIss::custom_##x
#define ci4(x, y, z, t) ci(x), ci(y), ci(z), ci(t)
Nios2fIss::func_t const Nios2fIss::customInstTable[] = {
	     ci4(ll, sc, illegal, illegal),
	     //opcode table 1
};
#undef ci

#ifdef DEBUGCustom
#define ci(x) #x
static const char *customNameTable[] = {
	     ci4(ll, sc, illegal, illegal),
    //opcode table 2
};
#endif

//custom p.8-49
void Nios2fIss::op_custom() {
	// opx field carrries readra, readrb, m_writerc information
	m_readra = (m_instruction.r.opx >> 5) & 1;
	m_readrb = (m_instruction.r.opx >> 4) & 1;
	m_writerc = (m_instruction.r.opx >> 3) & 1;
#ifdef DEBUGCustom
	std::cout << "CU - m_readra: " << m_readra << " m_readrb: " << m_readrb<< " m_writerc: " << m_writerc << std::endl;
#endif
	//opx carries part of N field (3 bits)
	//this part has to be concatenenated to sh field
	uint32_t nField = (uint32_t(m_instruction.r.opx & 0x07) << 3)
        | (uint32_t) m_instruction.r.sh;
#ifdef DEBUGCustom
	std::cout << "CU - N: " << nField << std::endl;
#endif

	func_t custom = customInstTable[nField];
	(this ->* custom)();

#ifdef DEBUGCustom
	std::cout << " execution of " << customNameTable[nField] << " custom instruction " << std::endl << std::endl;
#endif
}


// Atomic accesses are implemented by the way of custom instruction

void Nios2fIss::custom_ll()
{
    uint32_t address =  m_gprA;
    if (check_align(address, 4, X_MALLDATAADR))
        return;
    do_mem_access(address, 4, 0, m_instruction.r.c, 0, 0, DATA_LL);
#ifdef SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " load word (ll inst) @" << address
        << " -> r" << std::dec << m_instruction.r.c
        << std::endl;
#endif
	// late result management
	m_listOfLateResultInstruction.add(m_instruction.r.c);

}

void Nios2fIss::custom_sc()
{
    uint32_t address =  m_gprA;
    if (check_align(address, 4, X_MALLDESTADR))
        return;
    do_mem_access(address, 4, 8, m_instruction.r.c, 0, r_gpr[m_instruction.r.c], DATA_SC);
#ifdef SOCLIB_MODULE_DEBUG
	std::cout
        << m_name << std::hex
        << " store word (sc inst) @" << address
        << ": " << m_instruction.r.c
        << std::endl;
#endif
	}

void Nios2fIss::custom_illegal() {
}


//functions implementing custom instructions to be added here



}
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

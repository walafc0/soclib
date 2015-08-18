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
#include "base_module.h"
#include "arithmetics.h"

#include <strings.h>

namespace soclib { namespace common {

#define CTRL_REGNUM(no) (no)

enum ControlRegister {
    STATUS = CTRL_REGNUM(0),
    ESTATUS = CTRL_REGNUM(1),
    BSTATUS = CTRL_REGNUM(2),
    IENABLE = CTRL_REGNUM(3),
    IPENDING = CTRL_REGNUM(4),
    CPUID = CTRL_REGNUM(5),
    RESERVED_6 = CTRL_REGNUM(6),
    EXCEPTION = CTRL_REGNUM(7),
    PTEADDR = CTRL_REGNUM(8),
    TLBACC = CTRL_REGNUM(9),
    TLBMISC = CTRL_REGNUM(10),
    RESERVED_11 = CTRL_REGNUM(11),
    BADADDR = CTRL_REGNUM(12),
    CONFIG = CTRL_REGNUM(13),
    MPUBASE = CTRL_REGNUM(14),
    MPUACC = CTRL_REGNUM(15),
    RESERVED_16 = CTRL_REGNUM(16),
    RESERVED_17 = CTRL_REGNUM(17),
    RESERVED_18 = CTRL_REGNUM(18),
    RESERVED_19 = CTRL_REGNUM(19),
    RESERVED_20 = CTRL_REGNUM(20),
    RESERVED_21 = CTRL_REGNUM(21),
    RESERVED_22 = CTRL_REGNUM(22),
    RESERVED_23 = CTRL_REGNUM(23),
    RESERVED_24 = CTRL_REGNUM(24),
    RESERVED_25 = CTRL_REGNUM(25),
    RESERVED_26 = CTRL_REGNUM(26),
    RESERVED_27 = CTRL_REGNUM(27),
    RESERVED_28 = CTRL_REGNUM(28),
    RESERVED_29 = CTRL_REGNUM(29),
    RESERVED_30 = CTRL_REGNUM(30),
    RESERVED_31 = CTRL_REGNUM(31),
};


uint32_t Nios2fIss::controlRegisterGet( uint32_t reg) const
{
    switch(CTRL_REGNUM(reg)) {
    case STATUS:
        return r_status.whole;
    case ESTATUS:
        return r_estatus;
    case IPENDING:
        return r_ipending;
    case IENABLE:
        return r_ienable;
    case EXCEPTION:
        return r_exception;
    case CPUID:
    	return r_cpuid;
    case RESERVED_17:
        return r_ebase;
    case RESERVED_31:
        return r_count;
    default:
        return 0;
    }
}


void Nios2fIss::controlRegisterSet( uint32_t reg, uint32_t val )
{
    switch(CTRL_REGNUM(reg)) {
    case STATUS:
        r_status.whole = val;
        break;
    case ESTATUS:
        r_estatus = val;
        break;
    case IENABLE:
        r_ienable = val;
        break;
    case RESERVED_17:
    	r_reserved_17 = val;
    	r_ebase = val;
    	break;
    case RESERVED_31:
        r_count = val;
        break;
    default:
        return;
    }
}

void Nios2fIss::update_mode()
{
	switch (r_status.u) {
	case NIOS2_UM_SUPERVISOR:
		r_cpu_mode = NIOS2_SUPERVISOR;
		break;
	case NIOS2_UM_USER:
		r_cpu_mode = NIOS2_USER;
		break;
    default:
		assert(0&&"Invalid user mode set in status register");
	}
}

}}



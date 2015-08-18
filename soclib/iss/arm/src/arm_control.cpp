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
 * Copyright (c) UPMC, Lip6
 *         Alexandre Becoulet <alexandre.becoulet@free.fr>, 2009
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo becoulet
 *
 * $Id: arm_control.cpp 2333 2013-05-27 14:05:09Z becoulet $
 *
 */

#include "arm.h"

namespace soclib { namespace common {

void ArmIss::run()
{
    data_t r15 = r_gp[15];
    func_t func;

    if ((m_opcode.ins & 0xf0000000) == 0xf0000000) {
        int8_t id = arm_uncond_func_main(m_opcode.ins & 0x0ff000f0);
        func = arm_uncond_funcs[id];
    } else {
        int8_t id = arm_func_main(m_opcode.ins & 0x0ff000f0);
        func = arm_funcs[id];
    }
	(this->*func)();

	if (r_gp[15] == ARM_RESET_ADDR) {
        std::cerr << name() << " Jump to reset vector" << std::endl;
        r_gp[15] = r15;
        m_exception = EXCEPT_UNDEF;
        return;
	}
}

void ArmIss::run_thumb()
{
    data_t r15 = r_gp[15];

	int8_t id = thumb_func_main(m_thumb_op.ins);
	func_t func = thumb_funcs[id];
	(this->*func)();

	if (r_gp[15] == ARM_RESET_ADDR) {
        std::cerr << name() << " Jump to reset vector" << std::endl;
        r_gp[15] = r15;
        m_exception = EXCEPT_UNDEF;
        return;
	}
}

uint16_t const ArmIss::cond_table[16] = {
    /* EQ */ 0xf0f0, /* NE */ 0x0f0f, /* CS */ 0xcccc, /* CC */ 0x3333,
    /* MI */ 0xff00, /* PL */ 0x00ff, /* VS */ 0xaaaa, /* VC */ 0x5555,
    /* HI */ 0x0c0c, /* LS */ 0xf3f3, /* GE */ 0xaa55, /* LT */ 0x55aa,
    /* GT */ 0x0a05, /* LE */ 0xf5fa, /* AL */ 0xffff, /* NV */ 0x0000,
};

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

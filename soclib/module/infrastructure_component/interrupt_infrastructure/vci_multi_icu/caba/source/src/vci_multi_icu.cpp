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
 * Copyright (c) UPMC, Lip6, Asim
 *
 * Based on previous works by Franck Wajsburt, 2005
 * and Nicolas Pouillon <nipo@ssji.net>, 2007
 */

#include <strings.h>
#include <cassert>

#include "alloc_elems.h"

#include "multi_icu.h"
#include "register.h"
#include "../include/vci_multi_icu.h"

namespace soclib {
namespace caba {

using namespace soclib;

#define tmpl(t) template<typename vci_param> t VciMultiIcu<vci_param>

//////////////////////////////
tmpl(bool)::on_write( int seg, 
                      typename vci_param::addr_t addr, 
                      typename vci_param::data_t data, 
                      int be)
{
	int cell = (int)addr / vci_param::B;
    int reg = cell % ICU_SPAN;
    int icu_no = cell / ICU_SPAN;

	switch (reg) {
	case ICU_MASK_SET:
		r_mask[icu_no] = r_mask[icu_no].read() | data;
		break;

	case ICU_MASK_CLEAR:
		r_mask[icu_no] = r_mask[icu_no].read() & ~(data);
		break;

	default:
		return false;
	}
	return true;
}

/////////////////////////////
tmpl(bool)::on_read( int seg, 
                     typename vci_param::addr_t addr, 
                     typename vci_param::data_t &data )
{
	int cell = (int)addr / vci_param::B;
    int reg = cell % ICU_SPAN;
    int icu_no = cell / ICU_SPAN;


	switch (reg) {
	case ICU_INT:
		data = r_interrupt.read();
		break;

	case ICU_MASK:
		data = r_mask[icu_no].read();
		break;

	case ICU_IT_VECTOR:
		// give the highest priority interrupt
        data = ffs(
            (int)(r_interrupt.read() & r_mask[icu_no].read())
            )-1;
		break;

	default:
		return false;
	}
	return true;
}

/////////////////////////
tmpl(void)::print_trace()
{
    std::cout << std::dec << name() << std::endl;

    for (unsigned i = 0; i < m_nirq_out; i++)
        std::cout << std::hex << " r_mask = " << r_mask[i].read() 
                  << " for channel " << std::dec << i << std::endl;

    std::cout << std::dec << " r_interrupt = " << r_interrupt.read() << std::endl;
}

////////////////////////
tmpl(void)::transition()
{
	if (!p_resetn.read()) {
		m_vci_fsm.reset();

		// default all interrupt are masked
        r_interrupt = 0;
        for (unsigned int i = 0; i < m_nirq_out; i++)
            r_mask[i] = 0;
		return;
	}

	m_vci_fsm.transition();

	unsigned long tmp = 0;
	for (int i = (m_nirq_in-1); i>=0; --i)
		tmp = (tmp<<1) | (unsigned int)(p_irq_in[i].read());
	r_interrupt = tmp;
}

//////////////////////
tmpl(void)::genMoore()
{
	m_vci_fsm.genMoore();

    for (unsigned int i = 0; i < m_nirq_out; i++)
        p_irq_out[i] = (bool)(unsigned long)(r_interrupt.read() & r_mask[i].read());
}
/////////////////////////////////////////////////
tmpl(/**/)::VciMultiIcu( sc_module_name     name,
                         const IntTab       &index,
                         const MappingTable &mt,
                         size_t             nirq_in,
                         size_t             nirq_out)
	: caba::BaseModule(name),
	  m_vci_fsm(p_vci, mt.getSegmentList(index)),
      m_nirq_in(nirq_in),
      m_nirq_out(nirq_out),
      r_interrupt("interrupt"),
      r_mask(soclib::common::alloc_elems<sc_signal<unsigned int> >("mask", nirq_out)),
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci"),
      p_irq_in(soclib::common::alloc_elems<sc_in<bool> >("irq_in", nirq_in)),
      p_irq_out(soclib::common::alloc_elems<sc_out<bool> >("irq_out", nirq_out))
{
    assert(nirq_in <= 32 );
    assert(nirq_out <= 8 );

	m_vci_fsm.on_read_write(on_read, on_write);

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


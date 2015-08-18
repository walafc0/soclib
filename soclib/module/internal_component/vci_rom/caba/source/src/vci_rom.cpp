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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 */

#include "vci_rom.h"
#include "soclib_endian.h"
#include "loader.h"
#include <cstring>

namespace soclib {
namespace caba {

using namespace soclib;

#define tmpl(x) template<typename vci_param> x VciRom<vci_param>

tmpl(/**/)::VciRom(
	sc_module_name insname,
	const IntTab &index,
	const MappingTable &mt,
    const common::Loader &loader
    )
	: caba::BaseModule(insname),
	  m_vci_fsm(p_vci, mt.getSegmentList(index)),
      p_resetn("resetn"),
      p_clk("clk"),
      p_vci("vci")
{
	m_vci_fsm.on_read_write(on_read, on_write);
	
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();
	
	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
	
	m_contents = new typename vci_param::fast_data_t*[m_vci_fsm.nbSegments()];
	
	size_t word_size = vci_param::B; // B is VCI's cell size
	for ( size_t i=0; i<m_vci_fsm.nbSegments(); ++i ) {
		m_contents[i] = new typename vci_param::fast_data_t[(m_vci_fsm.getSize(i)+word_size-1)/word_size];
        loader.load(&m_contents[i][0], m_vci_fsm.getBase(i), m_vci_fsm.getSize(i));
        for ( size_t addr = 0; addr < m_vci_fsm.getSize(i)/vci_param::B; ++addr )
            m_contents[i][addr] = le_to_machine(m_contents[i][addr]);
	}
}

tmpl(/**/)::~VciRom()
{
	for (size_t i=0; i<m_vci_fsm.nbSegments(); ++i)
		delete [] m_contents[i];
	delete [] m_contents;
}

tmpl(bool)::on_write(size_t seg, vci_addr_t addr, vci_data_t data, int be)
{
    return false;
}

tmpl(bool)::on_read(size_t seg, vci_addr_t addr, vci_data_t &data )
{
	data = m_contents[seg][addr / vci_param::B];

	return true;
}

tmpl(void)::transition()
{
	if (!p_resetn) {
		m_vci_fsm.reset();
		return;
	}
	m_vci_fsm.transition();
}

tmpl(void)::genMoore()
{
	m_vci_fsm.genMoore();
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


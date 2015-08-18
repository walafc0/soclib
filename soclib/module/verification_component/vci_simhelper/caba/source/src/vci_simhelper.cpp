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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 */

#include <cstdlib>
#include <sstream>
#include <signal.h>

#include "simhelper.h"
#include "register.h"
#include "../include/vci_simhelper.h"

namespace soclib { namespace caba {

using namespace soclib;

#define tmpl(t) template<typename vci_param> t VciSimhelper<vci_param>

tmpl(bool)::on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be)
{
    int cell = (int)addr / vci_param::B;

	switch (cell) {
    case SIMHELPER_SC_STOP:
        sc_stop();
        break;
    case SIMHELPER_END_WITH_RETVAL:
        std::cout << "Simulation exiting, retval=" << (uint32_t)data << std::endl;
        ::exit(data);
    case SIMHELPER_EXCEPT_WITH_VAL:
    {
        std::ostringstream o;
        o << "Simulation yielded error level " << (uint32_t)data;
        throw soclib::exception::RunTimeError(o.str());
    }
    case SIMHELPER_PAUSE_SIM:
    {
        std::cout << "Simulation paused, press ENTER" << std::endl;
        std::string a;
        std::cin >> a;
    }
    case SIMHELPER_SIGINT:
    {
        std::cout << "Simulation interrupted with signal interrupt (SIGINT)" << std::endl;
        raise(SIGINT);
    }
	}
	return true;
}

tmpl(bool)::on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data)
{

    int cell = (int)addr / vci_param::B;

	switch (cell) {
    case SIMHELPER_CYCLES:
		data = m_cycles;
		return true;
	}
	return false;
}

tmpl(void)::transition()
{
	if (!p_resetn) {
		m_vci_fsm.reset();
		m_cycles = 0;
	}

	m_vci_fsm.transition();
	m_cycles++;
}

tmpl(void)::genMoore()
{
	m_vci_fsm.genMoore();
}

tmpl(/**/)::VciSimhelper(
    sc_module_name name,
    const IntTab &index,
    const MappingTable &mt)
	: caba::BaseModule(name),
	  m_vci_fsm(p_vci, mt.getSegmentList(index)),
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci")
{
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


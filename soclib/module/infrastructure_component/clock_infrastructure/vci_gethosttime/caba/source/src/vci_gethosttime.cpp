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
 *         Clement Devigne <clement.devigne@lip6.fr>
 *         Sylvain Leroy <sylvain.leroy@lip6.fr>
 */

#include <cstdlib>
#include <sstream>
#include <cassert>
#include <time.h>

#include "gethosttime.h"
#include "register.h"
#include "../include/vci_gethosttime.h"

namespace soclib { namespace caba {

using namespace soclib;

#define tmpl(t) template<typename vci_param> t VciGetHostTime<vci_param>

tmpl(bool)::on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be)
{
    assert(false and "WRITE ACCESS FORBIDDEN!");

	return true;
}

tmpl(bool)::on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data)
{
    int cell = (int)addr / vci_param::B;

	switch (cell)
        {
        case GETHOSTTIME_TIME:
            data = time(NULL);
            // if (data == ((time_t) -1)) 
            //     {
            //         perror();
            //     }
            return true;
            break;
        default:
            assert(false and
                   "ERROR in VCI_GETHOSTTIME : Read out of register map");
        }

	return false;
}

tmpl(void)::transition()
{
	if (!p_resetn)
        {
            m_vci_fsm.reset();
        }

	m_vci_fsm.transition();
}

tmpl(void)::genMoore()
{
	m_vci_fsm.genMoore();
}

tmpl(/**/)::VciGetHostTime(sc_module_name name,
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


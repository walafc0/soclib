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
 *         Nicolas Pouillon <nipo@ssji.net>, 2010
 */

#include <strings.h>

#include "../include/vci_wti_generator.h"

namespace soclib {
namespace caba {

#define tmpl(...) template<typename vci_param> __VA_ARGS__ VciWtiGenerator<vci_param>

tmpl(void)::transition()
{
    if ( p_resetn.read() == 0 ) {
        r_fsm = I_IDLE;
        r_last_irq = 0;

        return;
    }

    switch ( (enum state_e)r_fsm.read() ) {
    case I_IDLE:
        if ( p_irq && ! r_last_irq ) {
#if defined(SOCLIB_MODULE_DEBUG)
            std::cout << name() << " Detected raising edge" << std::endl;
#endif
            r_fsm = I_SEND_FLIT;
        }
        break;

    case I_SEND_FLIT:
        if ( p_vci.cmdack.read() ) {
#if defined(SOCLIB_MODULE_DEBUG)
            std::cout << name() << " Flit gone" << std::endl;
#endif
            r_fsm = I_WAIT_RSP;
        }
        break;

    case I_WAIT_RSP:
        if ( p_vci.iAccepted() ) {
#if defined(SOCLIB_MODULE_DEBUG)
            std::cout << name() << " Got response" << std::endl;
#endif
            r_fsm = I_IDLE;
        }
        break;
    }

    r_last_irq = p_irq.read();
}

tmpl(void)::genMoore()
{
    switch ( (enum state_e)r_fsm.read() ) {
    case I_IDLE:
        p_vci.rspack = false;
        p_vci.cmdval = false;
        break;

    case I_SEND_FLIT:
        p_vci.rspack = false;
        p_vci.cmdval = true;
        p_vci.address = m_addr;
        p_vci.be = (1<<vci_param::B) - 1;
        p_vci.cmd = vci_param::CMD_WRITE;
        p_vci.contig = 1;
        p_vci.wdata = 0x90711120;
        p_vci.eop = 1;
        p_vci.cons = 1;
        p_vci.plen = vci_param::B;
        p_vci.wrap = 0;
        p_vci.cfixed = 1;
        p_vci.clen = 0;
        p_vci.srcid = m_srcid;
        p_vci.trdid = 0;
        p_vci.pktid = 0;
        break;

    case I_WAIT_RSP:
        p_vci.rspack = true;
        p_vci.cmdval = false;
        break;
    }
}

tmpl()::VciWtiGenerator(
    sc_core::sc_module_name name,
    const soclib::common::MappingTable &mt,
    const soclib::common::IntTab &index,
    typename vci_param::fast_addr_t address)
       : caba::BaseModule(name),
       m_srcid(mt.indexForId(index)),
       m_addr(address),
       p_clk("clk"),
       p_resetn("resetn"),
       p_vci("vci"),
       p_irq("irq")
{
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

tmpl()::~VciWtiGenerator()
{
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


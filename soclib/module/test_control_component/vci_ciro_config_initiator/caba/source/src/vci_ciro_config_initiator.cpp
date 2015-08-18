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
 * Copyright (c) UPMC, Lip6, SoC
 *         Etienne Le Grand <etilegr@hotmail.com>, 2009
 */
 
#include <limits>
#include <cassert>
#include "vci_ciro_config_initiator.h"

namespace soclib { 
namespace caba {

#define tmpl(x)  template<typename vci_param> x VciCiroConfigInitiator<vci_param>

tmpl(/**/)::VciCiroConfigInitiator(
    sc_module_name name )
    : soclib::caba::BaseModule(name),

      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci")
{
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
  
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

    r_reg1 = 0;
}

tmpl(/**/)::~VciCiroConfigInitiator()
{
}

tmpl(void)::transition()
{
    int recv ;
    if ( ! p_resetn.read() ) {
        r_initiator_fsm = INITIATOR_IDLE;
        return;
    }

    switch (r_initiator_fsm.read()) {

    case INITIATOR_IDLE:
        r_initiator_fsm = INITIATOR_REQ_READ;
        r_reg1 = (r_reg1 + 1)%1024;
        break;

    case INITIATOR_REQ_READ:
	if (p_vci.cmdack == true)
        	r_initiator_fsm = INITIATOR_RSP_READ;
            
	break;

    case INITIATOR_RSP_READ:
	if (p_vci.rspval == true)
        	r_initiator_fsm = INITIATOR_IDLE;
            recv = (int)p_vci.rdata.read() ;
            printf("Initiator received : %d\n", recv);
	break;

    } // end switch r_dcache_fsm

}

tmpl(void)::genMoore()
{
    switch (r_initiator_fsm.read()) {
    case INITIATOR_IDLE:
        p_vci.cmdval  = false;
        p_vci.address = 0;
        p_vci.wdata   = 0;
        p_vci.be      = 0;
        p_vci.plen    = 0;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.trdid   = 0;
        p_vci.pktid   = 0;
        p_vci.srcid   = 0;
        p_vci.cons    = false;
        p_vci.wrap    = false;
        p_vci.contig  = false;
        p_vci.clen    = 0;
        p_vci.cfixed  = false;
        p_vci.eop     = false;

        p_vci.rspack  = false;
        break;

    case INITIATOR_REQ_READ:

        p_vci.cmdval  = true;
        p_vci.address = (addr_t)r_reg1.read();
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.plen    = 1 << 2;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.trdid   = 0;
        p_vci.pktid   = 0;
        p_vci.srcid   = 0;
        p_vci.cons    = false;
        p_vci.wrap    = false;
        p_vci.contig  = false;
        p_vci.clen    = 0;
        p_vci.cfixed  = false;
        p_vci.eop     = false;

        p_vci.rspack  = false;
        break;

    case INITIATOR_RSP_READ:
        p_vci.cmdval  = false;
        p_vci.address = 0;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.plen    = 1 << 2;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.trdid   = 0;
        p_vci.pktid   = 0;
        p_vci.srcid   = 0;
        p_vci.cons    = false;
        p_vci.wrap    = false;
        p_vci.contig  = false;
        p_vci.clen    = 0;
        p_vci.cfixed  = false;
        p_vci.eop     = false;

        p_vci.rspack  = true;
        break;

    } // end switch r_initiator_fsm
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


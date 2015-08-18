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
#include "vci_testelg_target.h"
#define VCI_READOK 0				// Successful read error code
#define VCI_WRITEOK 2				// Successful write error code
namespace soclib { 
namespace caba {

#define tmpl(x)  template<typename vci_param> x VciTestelgTarget<vci_param>

tmpl(/**/)::VciTestelgTarget(
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
    for (int i = 0 ; i < 1024 ; i++){
      ROM[i] = 1024 - i;
    }

}

tmpl(/**/)::~VciTestelgTarget()
{
}
tmpl(void)::transition()
{
    if ( ! p_resetn.read() ) {
		r_testelg_target_fsm = TARGET_IDLE;
        return;
    }
    switch (r_testelg_target_fsm.read()) {
	case TARGET_EOP:
    case TARGET_EOPWRITE:
    case TARGET_IDLE:
		if (p_vci.rspack == true || r_testelg_target_fsm.read()==TARGET_IDLE){
			if (p_vci.cmdval == true){
				r_srcid=p_vci.srcid.read();
				r_trdid=p_vci.trdid.read();
				r_pktid=p_vci.pktid.read();
				if (p_vci.cmd.read() == vci_param::CMD_READ || p_vci.cmd.read() == vci_param::CMD_LOCKED_READ){
					r_reg1 = (long long)p_vci.address.read() % 1024;
					if (p_vci.plen.read() == 4)
						r_testelg_target_fsm = TARGET_EOP;
					else{
						r_testelg_target_fsm = TARGET_RSP;
						r_testelg_target_dec=((p_vci.plen.read()>>2)-2);
					}
				}else if (p_vci.cmd.read() == vci_param::CMD_WRITE || p_vci.cmd.read() == vci_param::CMD_STORE_COND){
					ROM[(long long)p_vci.address.read() % 1024]	=ROM[(long long)p_vci.address.read() % 1024] & ~vci_param::be2mask((int)p_vci.be.read()) +
																		   (      int)p_vci.wdata.read() &  vci_param::be2mask((int)p_vci.be.read());
					
					if (p_vci.eop.read() == true)
						r_testelg_target_fsm = TARGET_EOPWRITE;
					else
						r_testelg_target_fsm = TARGET_IDLE;
				}else
					r_testelg_target_fsm = TARGET_IDLE;
			}else
				r_testelg_target_fsm = TARGET_IDLE;
		}
		break;
    case TARGET_RSP:
		if (p_vci.rspack == true){
			r_reg1 = (r_reg1.read()+4)%1024;
			if (r_testelg_target_dec.read()==0)
				r_testelg_target_fsm = TARGET_EOP;
			r_testelg_target_dec=r_testelg_target_dec.read()-1;
		}
		break;
    } // end switch r_testelg_target_fsm
}

tmpl(void)::genMoore()
{
	
	p_vci.rsrcid=r_srcid.read();
	p_vci.rtrdid=r_trdid.read();
	p_vci.rpktid=r_pktid.read();
   
    switch (r_testelg_target_fsm.read()) {
    case TARGET_IDLE:
        p_vci.cmdack = true;
        p_vci.rspval = false;
	break;

    case TARGET_RSP:
        p_vci.cmdack = false;
        p_vci.rspval = true;
        p_vci.rdata = (data_t)(ROM[r_reg1.read()].read());
        p_vci.rerror = VCI_READOK;
        p_vci.reop = false;
	break;

    case TARGET_EOP:
        p_vci.cmdack = true;
        p_vci.rspval = true;
        p_vci.rdata = (data_t)(ROM[r_reg1.read()].read());
        p_vci.rerror = VCI_READOK;
        p_vci.reop = true;
        break;
	case TARGET_EOPWRITE:
        p_vci.cmdack = true;
        p_vci.rspval = true;
        p_vci.rdata = 1;
        p_vci.rerror = VCI_WRITEOK;
        p_vci.reop = true;
        break;
    }
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


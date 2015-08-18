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
 
#include "hht_testelg_target.h"

namespace soclib { 
namespace caba {

#define tmpl(x)  template<typename hht_param> x HhtTestelgTarget<hht_param>

tmpl(/**/)::HhtTestelgTarget(
    sc_module_name name )
    : soclib::caba::BaseModule(name),
      p_clk("clk"),
      p_resetn("resetn"),
	  
	  p_hht("p_hht"),
	  
	  f_ctrlPCO("f_ctrlPCO",2),
	  f_ctrlNPCO("f_ctrlNPCO",2),
	  f_dataPCO("f_dataPCO",2),
	  f_dataNPCO("f_dataNPCO",2),
	  f_ctrlRI("f_ctrlRI",2),
	  f_dataRI("f_dataRI",2)
{
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
  
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

}

tmpl(/**/)::~HhtTestelgTarget()
{

}

tmpl(void)::transition()
{
	if ( ! p_resetn.read() ) {
		r_target_fsm = TARGET_IDLE;
		f_ctrlPCO.init();
        f_ctrlNPCO.init();
        f_dataPCO.init();
        f_dataNPCO.init();
        f_ctrlRI.init();
        f_dataRI.init();
        return;
	}
	
	switch (r_target_fsm.read()) {
		case TARGET_IDLE:
		break;
    }
	
	HhtCmdFlit<hht_param>	f_ctrlNPCO_read=f_ctrlNPCO.read();
	switch (r_target_fsm.read()){
	case TARGET_IDLE:
		if (f_ctrlNPCO.rok() && f_ctrlRI.wok() && f_dataRI.wok()){
			f_ctrlRI.write.unitid=f_ctrlNPCO_read.unitid;
			f_ctrlRI.write.srctag=f_ctrlNPCO_read.srctag;
			f_ctrlRI.write.bridge=0;
			f_ctrlRI.write.rvcset=0;
			f_ctrlRI.write.error=0;
			if ((f_ctrlNPCO_read.cmd & hht_param::CMD_WRITE_MASK)==hht_param::CMD_WRITE){ 		// Write
				f_ctrlNPCO.get=true;
				f_ctrlRI.write.cmd=hht_param::CMD_TARGETDONE;
				f_ctrlRI.write.count=f_ctrlNPCO_read.mskcnt;
				f_ctrlRI.write.passpw=0;
				f_ctrlRI.write.isoc  =(f_ctrlNPCO_read.cmd & hht_param::CMD_ISOC_FLAG)?1:0;
				f_ctrlRI.put=true;
			}else if ((f_ctrlNPCO_read.cmd & hht_param::CMD_READ_MASK)==hht_param::CMD_READ){ 	// Read 
				f_ctrlNPCO.get=true;
				if (!(f_ctrlNPCO_read.cmd & hht_param::CMD_DWORD_FLAG) || (f_ctrlNPCO_read.mskcnt)==0){ // Sends in 1 clock cycle
					f_ctrlRI.write.count=0;
				}else{
					f_ctrlRI.write.count=f_ctrlNPCO_read.mskcnt;
					r_target_dec=f_ctrlNPCO_read.mskcnt-1;
					r_target_fsm=TARGET_SENDING;
				}
				f_ctrlRI.write.cmd=hht_param::CMD_READRSP;
				f_ctrlRI.write.isoc  =(f_ctrlNPCO_read.cmd & hht_param::CMD_ISOC_FLAG)?1:0;
				f_ctrlRI.write.passpw=(f_ctrlNPCO_read.cmd & hht_param::CMD_RPASSPW_FLAG)?1:0;
				f_dataRI.write=101;
				f_ctrlRI.put=true;
				f_dataRI.put=true;
			}
		}
		break;
	case TARGET_SENDING:
		if (f_dataRI.wok()){
			f_dataRI.write=102;
			if (r_target_dec.read()==0)
				r_target_fsm=TARGET_IDLE;
			r_target_dec=r_target_dec.read()-1;
			f_dataRI.put=true;
		}
		break;
	} // end switch r_target_fsm
	
	f_dataNPCO.get=f_dataNPCO.rok();
	
	f_ctrlPCO.get=f_ctrlPCO.rok();
	f_dataPCO.get=f_dataPCO.rok();
	
	f_ctrlNPCO.put=p_hht.ctrlNPC.rok;
	f_ctrlNPCO.write.set_ctrl(p_hht.ctrlNPC.data);
	f_dataNPCO.put=p_hht.dataNPC.rok;
	f_dataNPCO.write=p_hht.dataNPC.data;
	f_ctrlPCO.put=p_hht.ctrlPC.rok;
	f_ctrlPCO.write.set_ctrl(p_hht.ctrlPC.data);
	f_dataPCO.put=p_hht.dataPC.rok;
	f_dataPCO.write=p_hht.dataPC.data;
	f_ctrlRI.get=p_hht.ctrlR.wok;
	f_dataRI.get=p_hht.dataR.wok;
	
	
	f_ctrlPCO.fsm();
    f_ctrlNPCO.fsm();
    f_dataPCO.fsm();
    f_dataNPCO.fsm();
    f_ctrlRI.fsm();
    f_dataRI.fsm();
}

tmpl(void)::genMoore()
{
	p_hht.ctrlPC.r=f_ctrlPCO.wok();
	p_hht.ctrlNPC.r=f_ctrlNPCO.wok();
	p_hht.dataPC.r=f_dataPCO.wok();
	p_hht.dataNPC.r=f_dataNPCO.wok();
	
	p_hht.ctrlR.w=f_ctrlRI.rok();
	p_hht.dataR.w=f_dataRI.rok();
	HhtRspFlit<hht_param>	f_ctrlRI_read=f_ctrlRI.read();
	p_hht.ctrlR.data=f_ctrlRI_read.get_ctrl();
	p_hht.dataR.data=f_dataRI.read();
	
	switch (r_target_fsm.read()) {
		case TARGET_IDLE:
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


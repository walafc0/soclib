/* -*- c++ -*-
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
 * Authors  : Franck WAJSBÜRT, Abdelmalek SI MERABET 
 * Date     : january 2009
 * Copyright: UPMC - LIP6
 */

#ifndef SOCLIB_CABA_GATE_PORTS_H_
#define SOCLIB_CABA_GATE_PORTS_H_

#include "gate_signals.h"

namespace soclib { namespace caba {

class GateTarget
{
public:

	sc_out<sc_uint<37> > cmd_data;
	sc_in<bool> cmd_wok;
	sc_out<bool> cmd_w;

	sc_in<sc_uint<33> > rsp_data;
	sc_out<bool> rsp_r;
	sc_in<bool> rsp_rok;

	GateTarget(const std::string &name = sc_gen_unique_name("gate_target_"))
		:   cmd_data     ((name+"cmd_data").c_str()),
		    cmd_wok      ((name+"cmd_wok").c_str()),
                    cmd_w        ((name+"cmd_w").c_str()),

		    rsp_data     ((name+"rsp_data").c_str()),
		    rsp_r        ((name+"rsp_r").c_str()),
                    rsp_rok      ((name+"rsp_rok").c_str())
		{ }
    
	void operator () (GateSignals &sig)
	{
		
                cmd_data    		(sig.cmd_data);	
		cmd_wok    		(sig.cmd_r_wok);	
		cmd_w    		(sig.cmd_w_rok);	
		
		rsp_data    		(sig.rsp_data);
		rsp_r    		(sig.rsp_r_wok);
		rsp_rok    		(sig.rsp_w_rok);
		
	}

        void operator () (GateTarget &port)
	{
	      	cmd_data    		(port.cmd_data);	
		cmd_wok    		(port.cmd_wok);	
		cmd_w    		(port.cmd_w);	
		
		rsp_data    		(port.rsp_data);
		rsp_r    		(port.rsp_r);
		rsp_rok    		(port.rsp_rok);
  		
	}

};


class GateInitiator
{
public:
	
	sc_in<sc_uint<37> > cmd_data;
	sc_out<bool> cmd_r;
	sc_in<bool> cmd_rok;

	sc_out<sc_uint<33> > rsp_data;
	sc_in<bool> rsp_wok;
	sc_out<bool> rsp_w;
	
	GateInitiator(const std::string &name = sc_gen_unique_name("gate_initiator_"))
		:  

		cmd_data     ((name+"cmd_data").c_str()),
		cmd_r        ((name+"cmd_r").c_str()),
                cmd_rok      ((name+"cmd_rok").c_str()),

		rsp_data     ((name+"rsp_data").c_str()),
		rsp_wok      ((name+"rsp_wok").c_str()),
                rsp_w        ((name+"rsp_w").c_str())
    		{ }
    
	void operator () (GateSignals &sig)
	{
		cmd_data    		(sig.cmd_data);
		cmd_r    		(sig.cmd_r_wok);
		cmd_rok    		(sig.cmd_w_rok);
		
                rsp_data    		(sig.rsp_data);	
		rsp_wok    		(sig.rsp_r_wok);	
		rsp_w    		(sig.rsp_w_rok);
	}

        void operator () (GateInitiator &port)
	{

		cmd_data    		(port.cmd_data);
		cmd_r    		(port.cmd_r);
		cmd_rok    		(port.cmd_rok);

	      	rsp_data    		(port.rsp_data);	
		rsp_wok    		(port.rsp_wok);	
		rsp_w    		(port.rsp_w);
	}
	    
	
};

}}

#endif // SOCLIB_CABA_GATE_PORTS_H_



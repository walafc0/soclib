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
 * Author   : Franck WAJSBÜRT, Abdelmalek SI MERABET 
 * Date     : september 2008
 * Copyright: UPMC - LIP6
 */

#ifndef SOCLIB_CABA_RING_PORTS_H_
#define SOCLIB_CABA_RING_PORTS_H_

#include "ring_signals.h"

namespace soclib { namespace caba {

//template<int ring_data_size>
class RingIn
{
public:
    sc_core::sc_in<bool>     	                cmd_grant;
    sc_core::sc_in<sc_dt::sc_uint<37> >                 cmd_data;   
    sc_core::sc_in<bool>                         cmd_rok;
    sc_core::sc_in<bool>                         cmd_wok;
    sc_core::sc_in<bool>     	                rsp_grant;
    sc_core::sc_in<sc_dt::sc_uint<33> >                 rsp_data;   
    sc_core::sc_in<bool>                         rsp_rok;
    sc_core::sc_in<bool>                         rsp_wok;
    
    RingIn(const std::string &name = sc_core::sc_gen_unique_name("ring_in_"))
        :   cmd_grant    ((name+"cmd_grant").c_str()),
            cmd_data     ((name+"cmd_data").c_str()),
            cmd_rok  	 ((name+"cmd_rok").c_str()),
            cmd_wok    	 ((name+"cmd_wok").c_str()),
            rsp_grant    ((name+"rsp_grant").c_str()),
            rsp_data     ((name+"rsp_data").c_str()),
            rsp_rok  	 ((name+"rsp_rok").c_str()),
            rsp_wok    	 ((name+"rsp_wok").c_str()) 
    { }
    
    void operator () (RingSignals &sig)
    {
        cmd_grant   		(sig.cmd_grant);	
        cmd_data    		(sig.cmd_data);			
        cmd_rok          	(sig.cmd_w);		
        cmd_wok            	(sig.cmd_r);	
        rsp_grant   		(sig.rsp_grant);		
        rsp_data    		(sig.rsp_data);			
        rsp_rok          	(sig.rsp_w);		
        rsp_wok            	(sig.rsp_r);	
    }

    void operator () (RingIn &port)
    {
        cmd_grant   		(port.cmd_grant);	
        cmd_data    		(port.cmd_data);		
        cmd_rok          	(port.cmd_rok);		
        cmd_wok            	(port.cmd_wok);	
        rsp_grant   		(port.rsp_grant);		
        rsp_data    		(port.rsp_data);		
        rsp_rok          	(port.rsp_rok);		
        rsp_wok            	(port.rsp_wok);	
    }

};


//template<int ring_data_size>
class RingOut
{
public:
    sc_core::sc_out<bool>     	                cmd_grant;
    sc_core::sc_out<sc_dt::sc_uint<37> >                cmd_data;   
    sc_core::sc_out<bool>                        cmd_r;
    sc_core::sc_out<bool>                        cmd_w;
    sc_core::sc_out<bool>     	                rsp_grant;
    sc_core::sc_out<sc_dt::sc_uint<33> >                rsp_data;   
    sc_core::sc_out<bool>                        rsp_w;
    sc_core::sc_out<bool>                        rsp_r;


    RingOut(const std::string &name = sc_core::sc_gen_unique_name("ring_out_"))
        :   cmd_grant    ((name+"cmd_grant").c_str()),
            cmd_data     ((name+"cmd_data").c_str()),
            cmd_r  	 ((name+"cmd_r").c_str()),
            cmd_w    	 ((name+"cmd_w").c_str()),
            rsp_grant    ((name+"rsp_grant").c_str()),
            rsp_data     ((name+"rsp_data").c_str()),
            rsp_w  	 ((name+"rsp_w").c_str()),
            rsp_r    	 ((name+"rsp_r").c_str()) 
    { }
    
    void operator () (RingSignals &sig)
    {
        cmd_grant   		(sig.cmd_grant);	
        cmd_data    		(sig.cmd_data);			
        cmd_r           	(sig.cmd_r);		
        cmd_w            	(sig.cmd_w);	
        rsp_grant   		(sig.rsp_grant);		
        rsp_data    		(sig.rsp_data);			
        rsp_w           	(sig.rsp_w);		
        rsp_r            	(sig.rsp_r);	
    }

    void operator () (RingOut &port)
    {
        cmd_grant   		(port.cmd_grant);	
        cmd_data    		(port.cmd_data);		
        cmd_r           	(port.cmd_r);		
        cmd_w            	(port.cmd_w);	
        rsp_grant   		(port.rsp_grant);		
        rsp_data    		(port.rsp_data);		
        rsp_w           	(port.rsp_w);		
        rsp_r            	(port.rsp_r);	
    }


};

}}

#endif 



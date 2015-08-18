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

#include <systemc>

#ifndef SOCLIB_CABA_RING_SIGNALS_H_
#define SOCLIB_CABA_RING_SIGNALS_H_

namespace soclib { namespace caba {

//template<int ring_data_size>
class RingSignals
{
public:
    sc_core::sc_signal<bool>                    cmd_grant;
    sc_core::sc_signal<sc_dt::sc_uint<37> >     cmd_data;
    sc_core::sc_signal<bool>                    cmd_w;
    sc_core::sc_signal<bool>                    cmd_r;
    sc_core::sc_signal<bool>                    rsp_grant;
    sc_core::sc_signal<sc_dt::sc_uint<33> >     rsp_data;
    sc_core::sc_signal<bool>                    rsp_w;
    sc_core::sc_signal<bool>                    rsp_r;

	RingSignals(std::string name = (std::string)sc_core::sc_gen_unique_name("ring_signals_"))
        : 	cmd_grant	((name+"cmd_grant").c_str()),
            	cmd_data	((name+"cmd_data").c_str()),
	    	cmd_w		((name+"cmd_w").c_str()),
	    	cmd_r		((name+"cmd_r").c_str()),
	    	rsp_grant	((name+"rsp_grant").c_str()),
	    	rsp_data	((name+"rsp_data").c_str()),
	    	rsp_w		((name+"rsp_w").c_str()),
	    	rsp_r		((name+"rsp_r").c_str()) 
    { }
};

}} // end namespace

#endif /* SOCLIB_CABA_RING_SIGNALS_H_ */

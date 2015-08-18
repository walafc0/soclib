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

#ifndef SOCLIB_CABA_GATE_SIGNALS_H_
#define SOCLIB_CABA_GATE_SIGNALS_H_

namespace soclib { namespace caba {

class GateSignals
{
public:

	sc_signal<sc_uint<37> > cmd_data;
	sc_signal<bool> cmd_r_wok;
	sc_signal<bool> cmd_w_rok;

	sc_signal<sc_uint<33> > rsp_data;
	sc_signal<bool> rsp_r_wok;
	sc_signal<bool> rsp_w_rok;

	GateSignals(std::string name = (std::string)sc_gen_unique_name("gate_signals_"))
	  : 	cmd_data	((name+"cmd_data").c_str()),
		cmd_r_wok       ((name+"cmd_r_wok").c_str()),
		cmd_w_rok	((name+"cmd_w_rok").c_str()),
		
		rsp_data	((name+"rsp_data").c_str()),
		rsp_r_wok       ((name+"rsp_r_wok").c_str()),
		rsp_w_rok	((name+"rsp_w_rok").c_str())
	  { }
};

}} // end namespace

#endif /* SOCLIB_CABA_GATE_SIGNALS_H_ */

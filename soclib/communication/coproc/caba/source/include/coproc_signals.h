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
 *         alain.greiner@lip6.fr 2015
 *
 * Maintainers: alain
 */

#ifndef SOCLIB_CABA_COPROC_SIGNALS_H
#define SOCLIB_CABA_COPROC_SIGNALS_H

#include <systemc>

namespace soclib { namespace caba {

//////////////////////////////////////////////////////////////
template <typename word_t, typename cmd_t> class CoprocSignals
{
public:
    sc_core::sc_signal<word_t> data;
    sc_core::sc_signal<bool>   r_wok;
    sc_core::sc_signal<bool>   w_rok;
    sc_core::sc_signal<bool>   req;
    sc_core::sc_signal<bool>   ack;
    sc_core::sc_signal<cmd_t>  bursts;

#define __ren(x) x((insname+"_" #x).c_str())
	CoprocSignals(std::string insname = sc_core::sc_gen_unique_name("coproc_signals"))
		: __ren(data),
		  __ren(r_wok),
		  __ren(w_rok),
		  __ren(req),
		  __ren(ack),
		  __ren(bursts)
	{
	}
#undef __ren

    void trace( sc_core::sc_trace_file* tf, const std::string &name )
    {
#define __trace(x) sc_core::sc_trace(tf, x, name+"_"+#x)
		__trace(data);
		__trace(r_wok);
		__trace(w_rok);
		__trace(req);
		__trace(ack);
		__trace(bursts);
#undef __trace
    }
};

}}

#endif 

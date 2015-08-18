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
 *         alain.greiner@lip6.fr  2015
 *
 * Maintainers: alain
 */
#ifndef SOCLIB_CABA_COPROC_PORTS_H
#define SOCLIB_CABA_COPROC_PORTS_H

#include <systemc>
#include "coproc_signals.h"

namespace soclib { namespace caba {

	using namespace sc_core;

/////////////////////////////////////////////////////
//  Coprocessor port for a TO_COPROC channel
/////////////////////////////////////////////////////
template <typename word_t, typename cmd_t>
struct ToCoprocInput 
{
	sc_in<word_t>  data;
    sc_out<bool>   r;
	sc_in<bool>    rok;
	sc_out<bool>   req;
	sc_in<bool>    ack;
    sc_out<cmd_t>  bursts;

#define __ren(x) x((name+"_" #x).c_str())
	ToCoprocInput(const std::string &name = sc_gen_unique_name("to_coproc_input"))
		: __ren(data),
          __ren(r),
          __ren(rok),
          __ren(req),
          __ren(ack),
          __ren(bursts)
	{}
#undef __ren

	void operator() ( CoprocSignals<word_t, cmd_t> &sig )
	{
		data(sig.data);
		r(sig.r_wok);
        rok(sig.w_rok);
		req(sig.req);
		ack(sig.ack);
		bursts(sig.bursts);
	}

	void operator() ( ToCoprocInput<word_t, cmd_t> &port)
	{
		data(port.data);
		r(port.r);
		rok(port.rok);
		req(port.req);
		ack(port.ack);
		bursts(port.bursts);
	}
};

////////////////////////////////////////////////
//   DMA port for a TO_COPROC channel
////////////////////////////////////////////////
template <typename word_t, typename cmd_t>
struct ToCoprocOutput 
{
	sc_out<word_t> data;
	sc_out<bool>   w;
    sc_in<bool>    wok;
	sc_in<bool>    req;
	sc_out<bool>   ack;
	sc_in<cmd_t>   bursts;

#define __ren(x) x((name+"_" #x).c_str())
	ToCoprocOutput(const std::string &name = sc_gen_unique_name("to_coproc_output"))
		: __ren(data),
          __ren(w),
          __ren(wok),
          __ren(req),
          __ren(ack),
          __ren(bursts)
	{}
#undef __ren

	void operator() ( CoprocSignals<word_t, cmd_t> &sig )
	{
		data(sig.data);
		w(sig.w_rok);
		wok(sig.r_wok);
		req(sig.req);
        ack(sig.ack);
        bursts(sig.bursts);
	}

	void operator() ( ToCoprocOutput<word_t, cmd_t> &port )
	{
		data(port.data);
	    w(port.w);
	    wok(port.wok);
		req(port.req);
        ack(port.ack);
        bursts(port.bursts);
	}
};

/////////////////////////////////////////////////////
//  Coprocessor port for a FROM_COPROC channel
/////////////////////////////////////////////////////
template <typename word_t, typename cmd_t>
struct FromCoprocOutput 
{
	sc_out<word_t> data;
	sc_out<bool>   w;
    sc_in<bool>    wok;
	sc_out<bool>   req;
	sc_in<bool>    ack;
    sc_out<cmd_t>  bursts;

#define __ren(x) x((name+"_" #x).c_str())
	FromCoprocOutput(const std::string &name = sc_gen_unique_name("from_coproc_output"))
		: __ren(data),
          __ren(w),
          __ren(wok),
          __ren(req),
          __ren(ack),
          __ren(bursts)
	{}
#undef __ren

	void operator() ( CoprocSignals<word_t, cmd_t> &sig )
	{
		data(sig.data);
		w(sig.w_rok);
        wok(sig.r_wok);
		req(sig.req);
		ack(sig.ack);
		bursts(sig.bursts);
	}

	void operator() ( FromCoprocOutput<word_t, cmd_t> &port)
	{
		data(port.data);
		w(port.w);
		wok(port.wok);
		req(port.req);
		ack(port.ack);
		bursts(port.bursts);
	}
};

////////////////////////////////////////////////
//   DMA port for a FROM_COPROC channel
////////////////////////////////////////////////
template <typename word_t, typename cmd_t>
struct FromCoprocInput 
{
	sc_in<word_t>  data;
	sc_out<bool>   r;
    sc_in<bool>    rok;
	sc_in<bool>    req;
	sc_out<bool>   ack;
	sc_in<cmd_t>   bursts;

#define __ren(x) x((name+"_" #x).c_str())
	FromCoprocInput(const std::string &name = sc_gen_unique_name("from_coproc_input"))
		: __ren(data),
          __ren(r),
          __ren(rok),
          __ren(req),
          __ren(ack),
          __ren(bursts)
	{}
#undef __ren

	void operator() ( CoprocSignals<word_t, cmd_t> &sig )
	{
		data(sig.data);
		r(sig.r_wok);
		rok(sig.w_rok);
		req(sig.req);
        ack(sig.ack);
        bursts(sig.bursts);
	}

	void operator() ( FromCoprocOutput<word_t, cmd_t> &port )
	{
		data(port.data);
	    r(port.r);
	    rok(port.rok);
		req(port.req);
        ack(port.ack);
        bursts(port.bursts);
	}
};

}}

#endif

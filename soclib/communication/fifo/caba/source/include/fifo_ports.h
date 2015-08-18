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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_CABA_FIFO_PORTS_H
#define SOCLIB_CABA_FIFO_PORTS_H

#include <systemc>
#include "fifo_signals.h"

namespace soclib { namespace caba {

	using namespace sc_core;

template <typename word_t>
struct FifoInput {
	sc_in<word_t> data;
	sc_out<bool> r;
	sc_in<bool> rok;

#define __ren(x) x((name+"_" #x).c_str())
	FifoInput(const std::string &name = sc_gen_unique_name("fifo_input"))
		: __ren(data),
          __ren(r),
          __ren(rok)
	{}
#undef __ren

	void operator() (FifoSignals<word_t> &sig)
	{
		data(sig.data);
		r(sig.r_wok);
		rok(sig.w_rok);
	}

	void operator() (FifoInput<word_t> &port)
	{
		data(port.data);
		r(port.r);
		rok(port.rok);
	}
};

template <typename word_t>
struct FifoOutput {
	sc_out<word_t> data;
	sc_in<bool> wok;
	sc_out<bool> w;

#define __ren(x) x((name+"_" #x).c_str())
	FifoOutput(const std::string &name = sc_gen_unique_name("fifo_output"))
		: __ren(data),
          __ren(wok),
          __ren(w)
	{}
#undef __ren

	void operator() (FifoSignals<word_t> &sig)
	{
		data(sig.data);
		wok(sig.r_wok);
		w(sig.w_rok);
	}

	void operator() (FifoOutput<word_t> &port)
	{
		data(port.data);
		wok(port.wok);
		w(port.w);
	}
};

}}

#endif /* SOCLIB_CABA_FIFO_PORTS_H */

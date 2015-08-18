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
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: nipo
 */

#ifndef SOCLIB_CABA_SIGNAL_XCACHE_PORTS_H_
#define SOCLIB_CABA_SIGNAL_XCACHE_PORTS_H_

#include <systemc>
#include "xcache_signals.h"

namespace soclib { namespace caba {

using namespace sc_core;

/*
 * DCACHE cache port
 */
class DCacheCachePort
{
public:
	sc_in<bool>          req;
	sc_in<sc_dt::sc_uint<4> >   type;
	sc_in<sc_dt::sc_uint<32> >  wdata;
	sc_in<sc_dt::sc_uint<32> >  adr;
	sc_out<bool>         frz;
	sc_out<sc_dt::sc_uint<32> > rdata;
	sc_out<bool>         berr; 

    DCacheCachePort(const std::string &name = sc_gen_unique_name("dcache_cache"))
		: req    ((name+"_req").c_str()),
		  type   ((name+"_type").c_str()),
		  wdata  ((name+"_wdata").c_str()),
		  adr    ((name+"_adr").c_str()),
		  frz    ((name+"_frz").c_str()),
		  rdata  ((name+"_rdata").c_str()),
		  berr   ((name+"_berr").c_str())
	{
	}
    
	void operator () (DCacheSignals &sig)
	{
		req    (sig.req);
		type   (sig.type);
		wdata  (sig.wdata);
		adr    (sig.adr);
		frz    (sig.frz);
		rdata  (sig.rdata);
		berr   (sig.berr);
	}
};

/*
 * ICACHE cache port
 */
class ICacheCachePort
{
public:
	sc_in<bool> 	        req;
	sc_in<sc_dt::sc_uint<32> >     adr; 
	sc_out<bool> 	        frz;
	sc_out<sc_dt::sc_uint<32> >    ins;
	sc_out<bool>            berr;
    
    ICacheCachePort(const std::string &name = sc_gen_unique_name("icache_cache"))
		: req    ((name+"_req").c_str()),
		  adr    ((name+"_adr").c_str()),
		  frz    ((name+"_frz").c_str()),
		  ins    ((name+"_ins").c_str()),
		  berr   ((name+"_berr").c_str())
	{
	}

	void operator () (ICacheSignals &sig) {
		req   (sig.req);
		adr   (sig.adr);
		frz  (sig.frz);
		ins   (sig.ins);
		berr  (sig.berr);
	}
};

}}

#endif 

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

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

#ifndef SOCLIB_CABA_SIGNAL_XCACHE_SIGNALS_H_
#define SOCLIB_CABA_SIGNAL_XCACHE_SIGNALS_H_

#include <systemc>

namespace soclib { namespace caba {

using namespace sc_core;

struct CacheInfo {
    uint32_t line_bytes;
    uint32_t associativity;
    uint32_t n_lines;
};

struct XCacheInfo {
    struct CacheInfo icache;
    struct CacheInfo dcache;
};

/*
 * DCACHE signals
 */
class DCacheSignals
{
public:
	enum req_type_e {
		READ_WORD,
        READ_HALF,
        READ_BYTE,
		LINE_INVAL,
		WRITE_WORD,
		WRITE_HALF,
		WRITE_BYTE,
		STORE_COND,
		READ_LINKED,
	};

	sc_signal<bool>            req;   // valid request
	sc_signal<sc_dt::sc_uint<4> >     type;  // request type
	sc_signal<sc_dt::sc_uint<32> >    wdata; // data from processor
	sc_signal<sc_dt::sc_uint<32> >    adr;   // address
	sc_signal<bool>            frz;   // request not accepted
	sc_signal<sc_dt::sc_uint<32> >    rdata; // data from cache
	sc_signal<bool>            berr;  // bus or memory error

	DCacheSignals(std::string name_ = (std::string)sc_gen_unique_name("dcache"))
		: req     (((std::string) (name_ + "_req"   )).c_str()),
		  type    (((std::string) (name_ + "_type"  )).c_str()),
		  wdata   (((std::string) (name_ + "_wdata" )).c_str()),
		  adr     (((std::string) (name_ + "_adr"   )).c_str()),
		  frz     (((std::string) (name_ + "_frz"   )).c_str()),
		  rdata   (((std::string) (name_ + "_rdata" )).c_str()),
		  berr    (((std::string) (name_ + "_berr"  )).c_str())
	{}
};

/*
 * ICACHE signals
 */
class ICacheSignals
{
public:
	sc_signal<bool>             req;  // valid read request
	sc_signal<sc_dt::sc_uint<32> >     adr;  // instruction address
	sc_signal<bool>             frz;  // instruction not valid
	sc_signal<sc_dt::sc_uint<32> >     ins;  // instruction
	sc_signal<bool>             berr; // bus or memory error

	ICacheSignals (std::string name_ = (std::string) sc_gen_unique_name ("icache"))
		: req    (((std::string) (name_ + "_req" )).c_str()),
		  adr    (((std::string) (name_ + "_adr" )).c_str()),
		  frz    (((std::string) (name_ + "_frz" )).c_str()),
		  ins    (((std::string) (name_ + "_ins" )).c_str()),
		  berr   (((std::string) (name_ + "_berr")).c_str())
	{
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

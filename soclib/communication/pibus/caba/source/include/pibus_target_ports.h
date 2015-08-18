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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2006
 *
 * Maintainers: alain
 */

#ifndef SOCLIB_CABA_PIBUS_TARGET_PORTS_H_
#define SOCLIB_CABA_PIBUS_TARGET_PORTS_H_

#include "pibus_signals.h"

namespace soclib { namespace caba {

class PibusTarget
{
public:
	sc_in<sc_dt::sc_uint<32> >		a;	// address
	sc_in<sc_dt::sc_uint<4> >		opc;	// codop
	sc_in<bool>			read;	// read transaction
	sc_in<bool>			lock;	// burst transaction
	sc_out<sc_dt::sc_uint<2> >		ack;	// response code
	sc_inout<sc_dt::sc_uint<32> >		d;	// data
	sc_in<bool>			tout;	// time_out

#define __ren(x) x((name+"_" #x).c_str())
    PibusTarget(const std::string &name = sc_gen_unique_name("pibus_target"))
		: __ren(a),
          __ren(opc),
          __ren(read),
          __ren(lock),
          __ren(ack),
          __ren(d),
          __ren(tout)
	{}
#undef __ren

        void operator() (Pibus &sig)
        {
                a       (sig.a);
                opc     (sig.opc);
                read    (sig.read);
                lock    (sig.lock);
                ack     (sig.ack);
                d       (sig.d);
                tout    (sig.tout);
        }

};

}} // end namespace

#endif // SOCLIB_CABA_PIBUS_TARGET_PORTS_H_ 

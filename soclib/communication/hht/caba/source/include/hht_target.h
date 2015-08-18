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
 * Copyright (c) UPMC, Lip6, Asim
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_CABA_SIGNAL_HHT_TARGET_H_
#define SOCLIB_CABA_SIGNAL_HHT_TARGET_H_

#include <systemc>
#include "hht_signals.h"
#include "hht_param.h"
#include "fifo_ports.h"
namespace soclib { namespace caba {

using namespace sc_core;

/**
 * HHT Target port
 */
template <typename hht_param>
class HhtTarget
{
public:
	soclib::caba::FifoInput<typename hht_param::ctrl_t> ctrlPC;
    soclib::caba::FifoInput<typename hht_param::ctrl_t> ctrlNPC;
    soclib::caba::FifoInput<typename hht_param::data_t> dataPC;
    soclib::caba::FifoInput<typename hht_param::data_t> dataNPC;
	soclib::caba::FifoOutput<typename hht_param::ctrl_t> ctrlR;
    soclib::caba::FifoOutput<typename hht_param::data_t> dataR;

#define __ren(x) x((name+"_" #x).c_str())
    HhtTarget(const std::string &name = sc_gen_unique_name("hht_target"))
		: __ren(ctrlPC),
          __ren(ctrlNPC),
          __ren(dataPC),
          __ren(dataNPC),
          __ren(ctrlR),
          __ren(dataR)
	{
	}
#undef __ren

	void operator()(HhtSignals<hht_param> &sig)
	{
		ctrlPC	(sig.ctrlPC);
		ctrlNPC	(sig.ctrlNPC);
		dataPC	(sig.dataPC);
		dataNPC	(sig.dataNPC);
		ctrlR	(sig.ctrlR);
		dataR	(sig.dataR);
	}

	void operator()(HhtTarget<hht_param> &ports)
	{
		ctrlPC	(ports.ctrlPC);
		ctrlNPC	(ports.ctrlNPC);
		dataPC	(ports.dataPC);
		dataNPC	(ports.dataNPC);
		ctrlR	(ports.ctrlR);
		dataR	(ports.dataR);
	}
};

}}

#endif /* SOCLIB_CABA_SIGNAL_HHT_TARGET_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


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
#ifndef SOCLIB_CABA_SIGNAL_VCI_MONITOR_OUT_H_
#define SOCLIB_CABA_SIGNAL_VCI_MONITOR_OUT_H_

#include <systemc>
#include "vci_signals.h"
#include "vci_param.h"

namespace soclib { namespace caba {

using namespace sc_core;

/**
 * VCI Monitor port
 */
template <typename vci_param>
class VciMonitorOut
{
public:
	sc_out<typename vci_param::ack_t>     rspack;
	sc_out<typename vci_param::val_t>     rspval;
	sc_out<typename vci_param::data_t>    rdata;
	sc_out<bool>                          reop;
	sc_out<typename vci_param::rerror_t>  rerror;
	sc_out<typename vci_param::srcid_t>   rsrcid;
	sc_out<typename vci_param::trdid_t >  rtrdid;
	sc_out<typename vci_param::pktid_t >  rpktid;

	sc_out<typename vci_param::ack_t>     cmdack;
	sc_out<typename vci_param::val_t>     cmdval;
	sc_out<typename vci_param::addr_t>    address;
	sc_out<typename vci_param::be_t>      be;
	sc_out<typename vci_param::cmd_t>     cmd;
	sc_out<typename vci_param::contig_t>  contig;
	sc_out<typename vci_param::data_t>    wdata;
	sc_out<typename vci_param::eop_t>     eop;
	sc_out<typename vci_param::const_t>   cons;
	sc_out<typename vci_param::plen_t>    plen;
	sc_out<typename vci_param::wrap_t>    wrap;
	sc_out<typename vci_param::cfixed_t>  cfixed;
	sc_out<typename vci_param::clen_t>    clen;
	sc_out<typename vci_param::srcid_t>   srcid;
	sc_out<typename vci_param::trdid_t>   trdid;
	sc_out<typename vci_param::pktid_t>   pktid;

#define __ren(x) x((name+"_" #x).c_str())
    VciMonitorOut(const std::string &name = sc_gen_unique_name("vci_monitor"))
		: __ren(rspack),
          __ren(rspval),
          __ren(rdata),
          __ren(reop),
          __ren(rerror),
          __ren(rsrcid),
          __ren(rtrdid),
          __ren(rpktid),
          __ren(cmdack),
          __ren(cmdval),
          __ren(address),
          __ren(be),
          __ren(cmd),
          __ren(contig),
          __ren(wdata),
          __ren(eop),
          __ren(cons),
          __ren(plen),
          __ren(wrap),
          __ren(cfixed),
          __ren(clen),
          __ren(srcid),
          __ren(trdid),
          __ren(pktid)
	{
	}
#undef __ren

	void operator()(VciSignals<vci_param> &sig)
	{
		cmdack  (sig.cmdack);
		address (sig.address);
		be      (sig.be);
		cfixed  (sig.cfixed);
		clen    (sig.clen);
		cmd     (sig.cmd);
		cmdval  (sig.cmdval);
		cons    (sig.cons);
		contig  (sig.contig);
		eop     (sig.eop);
		pktid   (sig.pktid);
		plen    (sig.plen);
		rdata   (sig.rdata);
		reop    (sig.reop);
		rerror  (sig.rerror);
		rpktid  (sig.rpktid);
		rsrcid  (sig.rsrcid);
		rspack  (sig.rspack);
		rspval  (sig.rspval);
		rtrdid  (sig.rtrdid);
		srcid   (sig.srcid);
		trdid   (sig.trdid);
		wdata   (sig.wdata);
		wrap    (sig.wrap);
	}

	void operator()(VciMonitorOut<vci_param> &ports)
	{
		cmdack  (ports.cmdack);
		address (ports.address);
		be      (ports.be);
		cfixed  (ports.cfixed);
		clen    (ports.clen);
		cmd     (ports.cmd);
		cmdval  (ports.cmdval);
		cons    (ports.cons);
		contig  (ports.contig);
		eop     (ports.eop);
		pktid   (ports.pktid);
		plen    (ports.plen);
		rdata   (ports.rdata);
		reop    (ports.reop);
		rerror  (ports.rerror);
		rpktid  (ports.rpktid);
		rsrcid  (ports.rsrcid);
		rspack  (ports.rspack);
		rspval  (ports.rspval);
		rtrdid  (ports.rtrdid);
		srcid   (ports.srcid);
		trdid   (ports.trdid);
		wdata   (ports.wdata);
		wrap    (ports.wrap);
	}
};

}}

#endif /* SOCLIB_CABA_SIGNAL_VCI_MONITOR_OUT_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


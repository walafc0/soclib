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
#ifndef SOCLIB_CABA_SIGNAL_VCI_SIGNALS_H_
#define SOCLIB_CABA_SIGNAL_VCI_SIGNALS_H_

#include <string>
#include <systemc>
#include "vci_param.h"

namespace soclib { namespace caba {

/**
 * VCI Initiator port
 */
template <typename vci_param>
class VciSignals
{
public:
	sc_core::sc_signal<typename vci_param::ack_t>     rspack;
	sc_core::sc_signal<typename vci_param::val_t>     rspval;
	sc_core::sc_signal<typename vci_param::data_t>    rdata;
	sc_core::sc_signal<bool>                          reop;
	sc_core::sc_signal<typename vci_param::rerror_t>  rerror;
	sc_core::sc_signal<typename vci_param::srcid_t>   rsrcid;
	sc_core::sc_signal<typename vci_param::trdid_t >  rtrdid;
	sc_core::sc_signal<typename vci_param::pktid_t >  rpktid;

	sc_core::sc_signal<typename vci_param::ack_t>     cmdack;
	sc_core::sc_signal<typename vci_param::val_t>     cmdval;
	sc_core::sc_signal<typename vci_param::addr_t>    address;
	sc_core::sc_signal<typename vci_param::be_t>      be;
	sc_core::sc_signal<typename vci_param::cmd_t>     cmd;
	sc_core::sc_signal<typename vci_param::contig_t>  contig;
	sc_core::sc_signal<typename vci_param::data_t>    wdata;
	sc_core::sc_signal<typename vci_param::eop_t>     eop;
	sc_core::sc_signal<typename vci_param::const_t>   cons;
	sc_core::sc_signal<typename vci_param::plen_t>    plen;
	sc_core::sc_signal<typename vci_param::wrap_t>    wrap;
	sc_core::sc_signal<typename vci_param::cfixed_t>  cfixed;
	sc_core::sc_signal<typename vci_param::clen_t>    clen;
	sc_core::sc_signal<typename vci_param::srcid_t>   srcid;
	sc_core::sc_signal<typename vci_param::trdid_t>   trdid;
	sc_core::sc_signal<typename vci_param::pktid_t>   pktid; 

#define ren(x) x(((std::string)(name_ + "_"#x)).c_str())

    VciSignals(std::string name_ = (std::string)sc_core::sc_gen_unique_name("vci"))
        : ren(rspack),
          ren(rspval),
          ren(rdata), 
          ren(reop),  
          ren(rerror),
          ren(rsrcid),
          ren(rtrdid),
          ren(rpktid),
          ren(cmdack),
          ren(cmdval),
          ren(address),
          ren(be),    
          ren(cmd),   
          ren(contig),
          ren(wdata), 
          ren(eop),   
          ren(cons),  
          ren(plen),  
          ren(wrap),  
          ren(cfixed),
          ren(clen),  
          ren(srcid), 
          ren(trdid), 
          ren(pktid)
    {
    }
#undef ren

    void trace( sc_core::sc_trace_file* tf, const std::string &name ) const
    {
#define __trace(x) sc_core::sc_trace(tf, x, name+"_"+#x)
        __trace(rspack);
        __trace(rspval);
        __trace(rdata); 
        __trace(reop);  
        __trace(rerror);
        __trace(rsrcid);
        __trace(rtrdid);
        __trace(rpktid);
        __trace(cmdack);
        __trace(cmdval);
        __trace(address);
        __trace(be);    
        __trace(cmd);   
        __trace(contig);
        __trace(wdata); 
        __trace(eop);   
        __trace(cons);  
        __trace(plen);  
        __trace(wrap);  
        __trace(cfixed);
        __trace(clen);  
        __trace(srcid); 
        __trace(trdid); 
        __trace(pktid);
#undef __trace
    }

    void print_trace(std::string name)
    {
        if ( cmdval )
        {
            std::cout << name << std::hex << " CMD VCI :";
            if ( cmd.read() == 1 )      std::cout << " RD";
            if ( cmd.read() == 2 )      std::cout << " WR";
            if ( cmd.read() == 3 )      std::cout << " LL";
            if ( cmd.read() == 0 )      std::cout << " SC";
            std::cout << " | @ = " << address
                      << " | wdata = " << wdata
                      << " | be = " << be
                      << " | srcid = " << srcid
                      << " | trdid = " << trdid
                      << " | pktid = " << pktid
                      << std::dec
                      << " | plen = " << plen 
                      << " | eop = "   << eop
                      << " | ack = "   << cmdack << std::endl;
        }
        if ( rspval )
        {
            std::cout << name << std::hex << " RSP VCI :"
                      << " rerror = "   << rerror
                      << " | rdata = "  << rdata
                      << " | rsrcid = " << rsrcid
                      << " | rtrdid = " << rtrdid
                      << " | rpktid = " << rpktid
                      << " | reop = "   << reop
                      << " | ack = "    << rspack << std::endl;
        }
    } // end print_trace()
    
};

}}


namespace sc_core {
    // sc_trace function
    template <typename vci_param>
    void sc_trace( sc_core::sc_trace_file* tf, const soclib::caba::VciSignals<vci_param>& vci, const std::string& name )
    {
        vci.trace(tf, name);
    }
}

#endif /* SOCLIB_CABA_SIGNAL_VCI_SIGNALS_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


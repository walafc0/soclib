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
 * Maintainers: nipo joel.porquet@lip6.fr
 */
#ifndef SOCLIB_CABA_SIGNAL_VCI_BUFFERS_H_
#define SOCLIB_CABA_SIGNAL_VCI_BUFFERS_H_

#include <inttypes.h>
#include <iostream>
#include <systemc>
#include "address_masking_table.h"
#include "address_decoding_table.h"
#include "vci_param.h"
#include "vci_initiator.h"
#include "vci_target.h"
#include "vci_monitor.h"

namespace soclib { namespace caba {

using namespace sc_core;

template <typename vci_param> class VciSnooper;
template <typename vci_param> class VciLoggerElem;

/////////////////////////////////////////////////////////////////////
template <typename vci_param>
class VciRspBuffer 
/////////////////////////////////////////////////////////////////////
{
    friend class VciSnooper<vci_param>;
    friend class VciLoggerElem<vci_param>;

	bool                          rspval;
	typename vci_param::data_t    rdata;
	bool                          reop;
	typename vci_param::rerror_t  rerror;
	typename vci_param::srcid_t   rsrcid;
	typename vci_param::trdid_t   rtrdid;
	typename vci_param::pktid_t   rpktid;

public:
    VciRspBuffer()
    {}

    typedef VciInitiator<vci_param>           input_port_t;
    typedef VciTarget<vci_param>              output_port_t;

    ////////////////////////
    inline bool val() const
    {
        return rspval;
    }
    /////////////////////////////////
    inline void set_val( bool value )
    {
        rspval = value;
    }
    ///////////////////////
    inline bool eop() const
    {
        return reop;
    }
    ////////////////////////////////
    inline bool is_broadcast() const
    {
        return false;  // no broacast for responses...
    }
    ///////////////////////////////////////////////
    inline typename vci_param::srcid_t dest() const
    {
        return rsrcid;
    }
    ////////////////////////////////////////////////
	inline void writeTo( output_port_t &port ) const
	{
		port.rspval = rspval;
		if (rspval) {
			port.rdata  = rdata;
			port.reop   = reop;
			port.rerror = rerror;
			port.rsrcid = rsrcid;
			port.rtrdid = rtrdid;
			port.rpktid = rpktid;
		}
	}
    ////////////////////////////////////////////////
	inline void readFrom( const input_port_t &port )
	{
		rspval = port.rspval;
		if (port.rspval) {
			rdata  = (typename vci_param::data_t)port.rdata;
			reop   = port.reop;
			rerror = (typename vci_param::rerror_t)port.rerror;
			rsrcid = (typename vci_param::srcid_t)port.rsrcid;
			rtrdid = (typename vci_param::trdid_t)port.rtrdid;
			rpktid = (typename vci_param::pktid_t)port.rpktid;
		}
	}
    ////////////////////////////////////////////////
	inline void readFrom( const VciMonitor<vci_param> &port )
	{
		rspval = port.rspval;
		if (port.rspval) {
			rdata  = (typename vci_param::data_t)port.rdata;
			reop   = port.reop;
			rerror = (typename vci_param::rerror_t)port.rerror;
			rsrcid = (typename vci_param::srcid_t)port.rsrcid;
			rtrdid = (typename vci_param::trdid_t)port.rtrdid;
			rpktid = (typename vci_param::pktid_t)port.rpktid;
		}
	}
    /////////////////////////////////////////////////////////////////////////
    friend std::ostream &operator << (std::ostream &o, const VciRspBuffer &b)
    {
        b.print(o);
        return o;
    }
    ///////////////////////////////////
    void print( std::ostream &o ) const
    {
        o << "VciRspBuffer" << std::hex << std::endl
          << " rspval: " << rspval << std::endl
          << " rdata : " << rdata << std::endl
          << " reop  : " << reop << std::endl
          << " rerror: " << rerror << std::endl
          << " rsrcid: " << rsrcid << std::endl
          << " rtrdid: " << rtrdid << std::endl
          << " rpktid: " << rpktid << std::endl;
    }
};

////////////////////////////////////////////////////////////////////////
template <typename vci_param>
class VciCmdBuffer 
////////////////////////////////////////////////////////////////////////
{
    friend class VciSnooper<vci_param>;
    friend class VciLoggerElem<vci_param>;

	bool                               cmdval;
	typename vci_param::addr_t         address;
	typename vci_param::be_t           be;
	typename vci_param::cmd_t          cmd;
	typename vci_param::contig_t       contig;
	typename vci_param::data_t         wdata;
	bool                               _eop;
	typename vci_param::const_t        cons;
	typename vci_param::plen_t         plen;
	typename vci_param::wrap_t         wrap;
	typename vci_param::cfixed_t       cfixed;
	typename vci_param::clen_t         clen;
	typename vci_param::srcid_t        srcid;
	typename vci_param::trdid_t        trdid;
	typename vci_param::pktid_t        pktid;

public:
    VciCmdBuffer()
    {}

    typedef VciInitiator<vci_param>           output_port_t;
    typedef VciTarget<vci_param>              input_port_t;

    ///////////////////////
    inline bool val() const
    {
        return cmdval;
    }
    /////////////////////////////////
    inline void set_val( bool value )
    {
        cmdval = value;
    }
    ///////////////////////
    inline bool eop() const
    {
        return _eop;
    }
    ////////////////////////////////
    inline bool is_broadcast() const
    {
        return ( (address&0x3) != 0);
    }
    //////////////////////////////////////////////
    inline typename vci_param::addr_t dest() const
    {
        return address;
    }
    /////////////////////////////////////////////////////////
	inline void readFrom( const VciMonitor<vci_param> &port )
	{
		cmdval  = port.cmdval;
		if (port.cmdval) {
			address = (typename vci_param::addr_t)port.address;
			be      = (typename vci_param::be_t)port.be;
			cmd     = (typename vci_param::cmd_t)port.cmd;
			contig  = (typename vci_param::contig_t)port.contig;
			wdata   = (typename vci_param::data_t)port.wdata;
			_eop    = port.eop;
			cons    = (typename vci_param::const_t)port.cons;
			plen    = (typename vci_param::plen_t)port.plen;
			wrap    = (typename vci_param::wrap_t)port.wrap;
			cfixed  = (typename vci_param::cfixed_t)port.cfixed;
			clen    = (typename vci_param::clen_t)port.clen;
			srcid   = (typename vci_param::srcid_t)port.srcid;
			trdid   = (typename vci_param::trdid_t)port.trdid;
			pktid   = (typename vci_param::pktid_t)port.pktid;
		}
	}
    ////////////////////////////////////////////////
	inline void readFrom( const input_port_t &port )
	{
		cmdval  = port.cmdval;
		if (port.cmdval) {
			address = (typename vci_param::addr_t)port.address;
			be      = (typename vci_param::be_t)port.be;
			cmd     = (typename vci_param::cmd_t)port.cmd;
			contig  = (typename vci_param::contig_t)port.contig;
			wdata   = (typename vci_param::data_t)port.wdata;
			_eop    = port.eop;
			cons    = (typename vci_param::const_t)port.cons;
			plen    = (typename vci_param::plen_t)port.plen;
			wrap    = (typename vci_param::wrap_t)port.wrap;
			cfixed  = (typename vci_param::cfixed_t)port.cfixed;
			clen    = (typename vci_param::clen_t)port.clen;
			srcid   = (typename vci_param::srcid_t)port.srcid;
			trdid   = (typename vci_param::trdid_t)port.trdid;
			pktid   = (typename vci_param::pktid_t)port.pktid;
		}
	}
    ////////////////////////////////////////////////
	inline void writeTo( output_port_t &port ) const
	{
		port.cmdval  = cmdval;
		if (cmdval) {
			port.address = address;
			port.be      = be;
			port.cmd     = cmd;
			port.contig  = contig;
			port.wdata   = wdata;
			port.eop     = _eop;
			port.cons    = cons;
			port.plen    = plen;
			port.wrap    = wrap;
			port.cfixed  = cfixed;
			port.clen    = clen;
			port.srcid   = srcid;
			port.trdid   = trdid;
			port.pktid   = pktid;
		}
	}
    /////////////////////////////////////////////////////////////////////////
    friend std::ostream &operator << (std::ostream &o, const VciCmdBuffer &b)
    {
        b.print(o);
        return o;
    }
    ///////////////////////////////////
    void print( std::ostream &o ) const
    {
        o << "VciCmdBuffer" << std::hex << std::endl
          << " cmdval : " << cmdval << std::endl
          << " address: " << address << std::endl
          << " be     : " << be << std::endl
          << " cmd    : " << cmd << std::endl
          << " contig : " << contig << std::endl
          << " wdata  : " << wdata << std::endl
          << " eop    : " << _eop << std::endl
          << " cons   : " << cons << std::endl
          << " plen   : " << plen << std::endl
          << " wrap   : " << wrap << std::endl
          << " cfixed : " << cfixed << std::endl
          << " clen   : " << clen << std::endl
          << " srcid  : " << srcid << std::endl
          << " trdid  : " << trdid << std::endl
          << " pktid  : " << pktid << std::endl;
    }

};

}}

#endif /* SOCLIB_CABA_SIGNAL_VCI_BUFFERS_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


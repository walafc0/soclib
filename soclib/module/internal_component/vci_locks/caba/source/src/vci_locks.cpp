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
 * Copyright (c) UPMC, Lip6
 *         Alain Greiner <alain.greiner@lip6.fr>, 2003-2007
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "../include/vci_locks.h"

namespace soclib {
namespace caba {

#define tmpl(x) template<typename vci_param> x VciLocks<vci_param>

tmpl(/**/)::VciLocks(
	sc_module_name insname,
	const soclib::common::IntTab &index,
	const soclib::common::MappingTable &mt)
	: soclib::caba::BaseModule(insname),
      r_vci_fsm("vci_fsm"),
      r_buf_srcid("buf_srcid"),
      r_buf_trdid("buf_trdid"),
      r_buf_pktid("buf_pktid"),
      r_buf_eop("buf_eop"),
      r_buf_value("buf_value"),
	  m_segment(mt.getSegment(index)),
           m_contents(new bool[m_segment.size() / vci_param::B]),
#if SOCLIB_CABA_VCI_LOCKS_DEBUG
           m_taker_srcid(new uint32_t[m_segment.size() / vci_param::B]),
           m_max_seen(0),
#endif
      p_resetn("resetn"),
      p_clk("clk"),
      p_vci("vci")
{
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();
	
	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

tmpl(/**/)::~VciLocks()
{
	delete [] m_contents;
#if SOCLIB_CABA_VCI_LOCKS_DEBUG
    delete [] m_taker_srcid;
#endif
}

tmpl(void)::transition()
{
	if (!p_resetn) {
		for (size_t i=0; i<m_segment.size() / vci_param::B; ++i)
			m_contents[i] = false;
		r_vci_fsm = IDLE;
        m_cpt_read = 0; 
        m_cpt_write = 0;
        m_cpt_error = 0;
        m_cpt_idle = 0; 
		return;
	}

	typename vci_param::addr_t address = p_vci.address.read();
	uint32_t cell = (address-m_segment.baseAddress()) / vci_param::B;

#if SOCLIB_CABA_VCI_LOCKS_DEBUG
    if ( m_max_seen ) {
        std::cout << name() << " ";
        for ( size_t i=0; i<m_max_seen; ++i )
            if ( m_contents[i] )
                std::cout << std::hex << std::setw(2) << std::noshowbase << m_taker_srcid[i];
            else
                std::cout << "--";
        std::cout << std::endl;
    }
#endif

	switch (r_vci_fsm) {
	case IDLE:
		if ( ! p_vci.cmdval.read() )
			break;
		/*
		 * We only accept 1-word request packets
		 * and we check for segmentation violations
		 */
		if ( ! p_vci.eop.read() ||
			 ! m_segment.contains(address) )
			r_vci_fsm = ERROR_RSP;
		else {
			switch (p_vci.cmd.read()) {
			case vci_param::CMD_READ:
				r_buf_value = m_contents[cell];
#if SOCLIB_CABA_VCI_LOCKS_DEBUG
                std::cout << name() << " srcid " << p_vci.srcid.read()
                          << " getting lock " << cell
                          << ( m_contents[cell] ? " already taken" : " ok")
                          << std::endl;
                if ( m_contents[cell] == false )
                    m_taker_srcid[cell] = p_vci.srcid.read();
                if ( cell >= m_max_seen )
                    m_max_seen = cell+1;
#endif
				m_contents[cell] = true;
				r_vci_fsm = READ_RSP;
                m_cpt_read++;
				break;
			case vci_param::CMD_WRITE:
				m_contents[cell] = false;
#if SOCLIB_CABA_VCI_LOCKS_DEBUG
                std::cout << name() << " srcid " << p_vci.srcid.read()
                          << " releasing lock " << cell
                          << std::endl;
                m_taker_srcid[cell] = -1;
#endif
				r_vci_fsm = WRITE_RSP;
                m_cpt_write++;
				break;
			default:
				r_vci_fsm = ERROR_RSP;
                m_cpt_error++;
				break;
			}
		}
		r_buf_srcid = p_vci.srcid.read();
		r_buf_trdid = p_vci.trdid.read();
		r_buf_pktid = p_vci.pktid.read();
		r_buf_eop = p_vci.eop.read();
		break;
	case WRITE_RSP:
	case READ_RSP:
	case ERROR_RSP:
		if ( p_vci.rspack.read() )
            r_vci_fsm = rand() % 2 ? IDLE : WAIT_IDLE;
		break;

        // randomly add a dummy cycle to get rid of live locks
	case WAIT_IDLE:
        r_vci_fsm = IDLE;
        m_cpt_idle++;
        break;
	}
}

tmpl(void)::genMoore()
{
	p_vci.rspSetIds( r_buf_srcid.read(), r_buf_trdid.read(), r_buf_pktid.read() );

	switch (r_vci_fsm) {
	case WAIT_IDLE:
	case IDLE:
		p_vci.rspNop();
		break;
	case WRITE_RSP:
		p_vci.rspWrite( r_buf_eop.read() );
		break;
	case READ_RSP:
		p_vci.rspRead( r_buf_eop.read(), r_buf_value.read() ? -1 : 0 );
		break;
	case ERROR_RSP:
		p_vci.rspError( r_buf_eop.read() );
		break;
	}

    p_vci.cmdack = (r_vci_fsm == IDLE);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


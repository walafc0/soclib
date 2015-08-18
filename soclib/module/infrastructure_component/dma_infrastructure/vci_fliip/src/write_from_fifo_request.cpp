/*
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
 * Copyright (c) CITI/INSA, 2009
 * 
 * Authors:
 * 	Ludovic L'Hours <ludovic.lhours@insa-lyon.fr>
 * 	Antoine Fraboulet <antoine.fraboulet@insa-lyon.fr>
 * 	Tanguy Risset <tanguy.risset@insa-lyon.fr>
 * 
 */

#include <write_from_fifo_request.h>

#define tmpl(x) template<typename vci_param> x WriteFromFifoRequest<vci_param>

namespace soclib { namespace caba {

// has to be done in transition process
tmpl(void)::setOnRead(BaseModule *module, on_read_func_t func) {
	m_on_read_module = module;
	m_on_read = func;
	(m_on_read_module->*m_on_read)(m_data);
}

tmpl(bool)::putCmd( VciInitiator<vci_param> &p_vci, uint32_t id ) const {
    const uint32_t packet = VciInitiatorReq<vci_param>::m_packet;
    const uint32_t thread = VciInitiatorReq<vci_param>::m_thread;
	bool ending = (m_index + vci_param::B) >= m_len;

	//std::cout << "ending=" << ending << " index=" << m_index << std::endl;

	p_vci.cmdval = true;
	p_vci.address = (m_source+m_index)&~(vci_param::B-1);
	p_vci.be = (1<<vci_param::B)-1;
	p_vci.cmd = vci_param::CMD_WRITE;
	p_vci.wdata = m_data;
	p_vci.eop = ending;
	p_vci.cons = m_srcMode == CONST_MODE ? true : false;
	p_vci.contig = m_srcMode == CONTIG_MODE ? true : false;
	p_vci.plen = m_len; // (m_len + (m_source%vci_param::B) + vci_param::B - 1)&~(vci_param::B-1);
	p_vci.wrap = 0;
	p_vci.cfixed = 1;
	//	TODO: clen
    p_vci.clen = 0;
	p_vci.srcid = id;
	p_vci.trdid = thread;
	p_vci.pktid = packet;

    return true;
}

tmpl(void)::cmdOk(bool last) {
	VciInitiatorReq<vci_param>::cmdOk(last);
	if (m_srcMode != CONST_MODE)
		m_index += vci_param::B;

	if (m_index < m_len)
		(m_on_read_module->*m_on_read)(m_data);
	//std::cout << "cmdok failed=" << m_failed << std::endl;
}

tmpl(void)::gotRsp( const VciInitiator<vci_param> &p_vci ) {

    VciInitiatorReq<vci_param>::gotRsp( p_vci );
}

}} // end of soclib::caba

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

#ifndef READ_TO_FIFO_REQUEST_H
#define READ_TO_FIFO_REQUEST_H

#include <vci_initiator_fsm.h>

#define ON_WRITE_FIFO(x) \
(bool (soclib::caba::BaseModule::*)(typename vci_param::data_t))(&SC_CURRENT_USER_MODULE::x)

namespace soclib { namespace caba {

template <typename vci_param>
class ReadToFifoRequest : public VciInitiatorReq<vci_param> {

public:
	typedef bool (BaseModule::*on_write_func_t)(typename vci_param::data_t data);

	enum {
		CONST_MODE  = 0,
		CONTIG_MODE = 1,
	};

private:
	unsigned int m_source;
	unsigned int m_srcMode;
	unsigned int m_len;
	unsigned int m_index;
	
	BaseModule *m_on_write_module;
	on_write_func_t m_on_write;

public:

	ReadToFifoRequest(unsigned int source, int srcMode, int len) {
		m_source = source; // XXX should be aligned on words
		m_srcMode = srcMode;
		m_len = len * vci_param::B; // XXX plen count is in byte
		m_index = 0;
		VciInitiatorReq<vci_param>::m_expected_packets =
				(m_source % vci_param::B + m_len + vci_param::B-1) / vci_param::B;
	}

	void setOnWrite(BaseModule *, on_write_func_t);

	bool putCmd( VciInitiator<vci_param> &p_vci, uint32_t id ) const;

	void cmdOk(bool last);

	void gotRsp( const VciInitiator<vci_param> &p_vci );

};

}} // end of soclib::caba

#endif

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

#include "vci_initiator_fsm.h"
#include "register.h"
#include "base_module.h"
#ifdef SOCLIB_MODULE_DEBUG
# include "vci_buffers.h"
#endif

namespace soclib {
namespace caba {

#undef tmpl
#define tmpl(x) template<typename vci_param>\
x VciInitiatorFsm<vci_param>

tmpl(/**/)::VciInitiatorFsm(
    soclib::caba::VciInitiator<vci_param> &vci,
    const uint32_t index )
    : p_vci(vci),
      m_ident( index ),
      m_current_req(NULL)
{
}

tmpl(void)::doReq( VciInitiatorReq<vci_param> *req )
{
    m_current_req = req;
    m_current_req_gone = false;
}

tmpl(/**/)::~VciInitiatorFsm()
{
}

tmpl(void)::reset()
{
    m_current_req = NULL;
}

tmpl(void)::transition()
{
	if ( m_current_req == NULL )
        return;

    if ( p_vci.peerAccepted() ) {
#ifdef SOCLIB_MODULE_DEBUG
        std::cout << "ifsm peer accepted command" << std::endl;
#endif
        m_current_req->cmdOk(p_vci.eop.read());
        if ( p_vci.eop.read() )
            m_current_req_gone = true;
    }

    if ( p_vci.iAccepted() ) {
#ifdef SOCLIB_MODULE_DEBUG
        VciRspBuffer<vci_param> buf;
        buf.readFrom(p_vci);
        std::cout
            << "ifsm got response: "
            << buf
            << std::endl;
#endif
        VciInitiatorReq<vci_param> *req = m_current_req;
        if ( p_vci.reop.read() )
            m_current_req = NULL;
        req->gotRsp( p_vci );
    }
}

tmpl(void)::genMoore()
{
    if ( m_current_req && !m_current_req_gone ) {
        m_current_req->putCmd( p_vci, m_ident );
        p_vci.rspack = true;
    } else {
        p_vci.cmdval = false;
        p_vci.rspack = m_current_req != NULL;
    }
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


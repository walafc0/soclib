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

namespace soclib {
namespace caba {

#define tmpl(x) template<typename vci_param>\
x VciInitiatorReq<vci_param>

tmpl(/**/)::VciInitiatorReq()
    : m_thread(0),
      m_packet(0),
      m_failed(false),
      m_on_done_module(NULL),
      m_on_done_func(NULL),
      m_expected_packets(0),
      m_sent_packets(0),
      m_received_packets(0)
{
}

tmpl(/**/)::~VciInitiatorReq()
{
}

tmpl(void)::setThread( trd_t thread )
{
    m_thread = thread;
}

tmpl(void)::setPacket( pkt_t packet )
{
    m_packet = packet;
}

tmpl(void)::setDone( BaseModule *module, on_t callback )
{
    m_on_done_module = module;
    m_on_done_func = callback;
}

tmpl(void)::cmdOk( bool last )
{
    m_sent_packets++;
    if ( last && m_expected_packets == 0 )
        m_expected_packets = m_sent_packets;
}

tmpl(void)::gotRsp( const VciInitiator<vci_param> &p_vci )
{
    m_received_packets++;
    if ( p_vci.rerror.read() )
        m_failed = true;

    if ( p_vci.reop.read() ) {
        if ( m_received_packets != m_expected_packets ) {
            std::cout << "Sent and received cells are not as numerous"
                      << " expected " << m_expected_packets
                      << " and received " << m_received_packets
                      << std::endl;
            abort();
        }
        // Beware this call equals `delete this` dont call anything
        // afterwards
        (m_on_done_module->*m_on_done_func)(this);
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


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

#include <stdint.h>
#include <cassert>
#include "vci_initiator_fsm.h"
#include "register.h"
#include "base_module.h"

namespace soclib {
namespace caba {

#undef tmpl
#define tmpl(x) template<typename vci_param>\
x VciInitiatorSimpleReq<vci_param>

tmpl(/**/)::VciInitiatorSimpleReq(
    uint8_t *local_buffer, uint32_t base_addr, size_t len )
    : m_dest_buffer(local_buffer),
      m_base_addr(base_addr),
      m_len(len),
      m_cmd_ptr(0),
      m_rsp_ptr(0)
{
    if ( vci_param::K )
        assert((len < (1<<vci_param::K)) &&
               "You must use initiator requests in chuncks "
               "when more than 1<<PLEN bytes are transfered");
}

tmpl(/**/)::~VciInitiatorSimpleReq()
{
}

tmpl(void)::cmdOk(bool last)
{
    VciInitiatorReq<vci_param>::cmdOk(last);
    m_cmd_ptr = next_addr(m_cmd_ptr);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


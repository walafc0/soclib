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
#include "vci_initiator_fsm.h"
#include "register.h"
#include "base_module.h"

namespace soclib {
namespace caba {

#undef tmpl
#define tmpl(x) template<typename vci_param>\
x VciInitSimpleReadReq<vci_param>

tmpl(/**/)::VciInitSimpleReadReq(
    uint8_t *local_buffer, uint32_t base_addr, size_t len )
           : VciInitiatorSimpleReq<vci_param>( local_buffer, base_addr, len )
{
    VciInitiatorReq<vci_param>::m_expected_packets =
        (base_addr%vci_param::B+len+vci_param::B-1)/vci_param::B;
}

tmpl(/**/)::~VciInitSimpleReadReq()
{
}

tmpl(bool)::putCmd( VciInitiator<vci_param> &p_vci, uint32_t id ) const
{
    const uint32_t cmd_ptr = VciInitiatorSimpleReq<vci_param>::m_cmd_ptr;
    const uint32_t base_addr = VciInitiatorSimpleReq<vci_param>::m_base_addr;
    const size_t len = VciInitiatorSimpleReq<vci_param>::m_len;
    const uint32_t packet = VciInitiatorReq<vci_param>::m_packet;
    const uint32_t thread = VciInitiatorReq<vci_param>::m_thread;
    bool ending =
        true;
//    this->next_addr(cmd_ptr)>=len;

    p_vci.cmdval = true;
    p_vci.address = (base_addr+cmd_ptr)&~(vci_param::B-1);
	p_vci.be = (1<<vci_param::B)-1;
	p_vci.cmd = vci_param::CMD_READ;
	p_vci.contig = 1;
//  p_vci.wdata;
	p_vci.eop = ending;
	p_vci.cons = true;
	p_vci.plen = (len + (base_addr%vci_param::B) + vci_param::B - 1)&~(vci_param::B-1);
	p_vci.wrap = 0;
	p_vci.cfixed = 1;
//	TODO: clen
    p_vci.clen = 0;
	p_vci.srcid = id;
	p_vci.trdid = thread;
	p_vci.pktid = packet;

    return ending;
}

tmpl(void)::gotRsp( const VciInitiator<vci_param> &p_vci )
{
    size_t rsp_ptr = VciInitiatorSimpleReq<vci_param>::m_rsp_ptr;
    const size_t len = VciInitiatorSimpleReq<vci_param>::m_len;
    uint8_t * const dest_buffer = VciInitiatorSimpleReq<vci_param>::m_dest_buffer;
    const uint32_t base_addr = VciInitiatorSimpleReq<vci_param>::m_base_addr;

    typename VciInitiatorReq<vci_param>::data_t data = p_vci.rdata;

    const uint32_t vci_addr = (base_addr+rsp_ptr)&~(vci_param::B-1);
    const uint32_t delta = vci_addr-base_addr;

    for ( uint32_t i=0; i<(uint32_t)vci_param::B; ++i ) {
        const uint32_t addr = vci_addr+i;
        if ( addr >= base_addr &&
            addr < base_addr+len ) {
            dest_buffer[delta+i] = data;
        }
        data >>= 8;
    }
    
    VciInitiatorSimpleReq<vci_param>::m_rsp_ptr
        = VciInitiatorSimpleReq<vci_param>::next_addr(rsp_ptr);

    VciInitiatorReq<vci_param>::gotRsp( p_vci );
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


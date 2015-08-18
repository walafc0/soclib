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
x VciInitSimpleWriteReq<vci_param>

tmpl(/**/)::VciInitSimpleWriteReq(
    uint32_t base_addr, uint8_t *local_buffer, size_t len )
    : VciInitiatorSimpleReq<vci_param>( local_buffer, base_addr, len )
{
#if SOCLIB_MODULE_DEBUG
    std::cout << "Initiating write req " << std::hex
        << "vci: " << base_addr
        << " <- buf: " << (uint32_t)local_buffer
        << " / " << len << std::endl;
#endif
    VciInitiatorReq<vci_param>::m_expected_packets = 1;
}

tmpl(/**/)::~VciInitSimpleWriteReq()
{

}

tmpl(bool)::putCmd( VciInitiator<vci_param> &p_vci, uint32_t id ) const
{
    const size_t cmd_ptr = VciInitiatorSimpleReq<vci_param>::m_cmd_ptr;
    const uint32_t base_addr = VciInitiatorSimpleReq<vci_param>::m_base_addr;
    const size_t len = VciInitiatorSimpleReq<vci_param>::m_len;
    const uint32_t packet = VciInitiatorReq<vci_param>::m_packet;
    const uint32_t thread = VciInitiatorReq<vci_param>::m_thread;
    uint8_t * const dest_buffer = VciInitiatorSimpleReq<vci_param>::m_dest_buffer;
    bool ending = this->next_addr(cmd_ptr)>=len;
    typename VciInitiatorReq<vci_param>::data_t data = 0;
    typename vci_param::be_t be = 0;

    const uint32_t vci_addr = (base_addr+cmd_ptr)&~3;
    const uint32_t delta = vci_addr-base_addr;

    for ( int i=vci_param::B-1; i>=0; --i ) {
        const uint32_t addr = vci_addr+i;
        data <<= 8;
        be <<= 1;
        if ( addr >= base_addr &&
            addr < base_addr+len ) {
            be |= 1;
            data |= dest_buffer[delta+i];
        }
    }

    p_vci.cmdval = true;
    p_vci.address = (base_addr+cmd_ptr)&~3;
	p_vci.be = be;
	p_vci.cmd = vci_param::CMD_WRITE;
	p_vci.contig = 1;
    p_vci.wdata = data;
	p_vci.eop = ending;
	p_vci.cons = true;
	p_vci.plen = len;
	p_vci.wrap = 0;
	p_vci.cfixed = 1;
//	TODO: clen
    p_vci.clen = 0;
	p_vci.srcid = id;
	p_vci.trdid = thread;
	p_vci.pktid = packet;

#if SOCLIB_MODULE_DEBUG
    std::cout << std::hex
        << "Putting write command: "
        << "@ " << vci_addr
        << ": " << data << '/' << be
        << " (plen=" << len
        << ", eop=" << ending
        << ")" << std::endl;
#endif

    return ending;
}

tmpl(void)::gotRsp( const VciInitiator<vci_param> &p_vci )
{
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


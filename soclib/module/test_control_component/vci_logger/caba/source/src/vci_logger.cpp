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
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo
 */

#include "vci_logger.h"
#include <cstring>
#include "vci_buffers.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>

namespace soclib {
namespace caba {

#define tmpl(...) template<typename vci_param> __VA_ARGS__ VciLoggerElem<vci_param>

tmpl()::VciLoggerElem()
{
    m_cmd_ended = false;
    m_rsp_ended = false;
}

tmpl(void)::takeRsp( const VciMonitor<vci_param> &port)
{
    assert( !m_rsp_ended );
    VciRspBuffer<vci_param> r;
    r.readFrom(port);
    m_rsp_packets.push_back(r);

//    std::cout << r << std::endl;

    m_rsp_ended |= port.reop.read();
}

tmpl(void)::takeCmd( const VciMonitor<vci_param> &port)
{
    assert( !m_cmd_ended );
    VciCmdBuffer<vci_param> r;
    r.readFrom(port);
    m_cmd_packets.push_back(r);

//    std::cout << r << std::endl;

    m_cmd_ended |= port.eop.read();
}

tmpl(void)::print( std::ostream &o ) const
{
    static const char *cmd_str[] = {"STORE_COND", "READ", "WRITE", "LOCKED_READ"};
    assert( ! m_cmd_packets.empty() && ! m_rsp_packets.empty() );
    o << cmd_str[m_cmd_packets[0].cmd]
      << " " << std::noshowbase << std::dec << m_cmd_packets.size() << " cells"
      << " @" << std::hex << std::showbase << m_cmd_packets[0].address;
    if ( m_cmd_packets.size() == 1 ) {
        if ( m_cmd_packets[0].plen )
            o << " plen = " << std::noshowbase << std::dec << m_cmd_packets[0].plen;
            o << " const = " << m_cmd_packets[0].cons;
            o << " (r)srcid = " << m_cmd_packets[0].srcid;
            o << " (r)trdid = " << m_cmd_packets[0].trdid;
            o << " be = " << std::hex << std::showbase << m_cmd_packets[0].be;
    }
    if ( m_cmd_packets[0].cmd == vci_param::CMD_WRITE
         || m_cmd_packets[0].cmd == vci_param::CMD_STORE_COND ) {
        for ( typename cmd_list_t::const_iterator i = m_cmd_packets.begin();
              i != m_cmd_packets.end();
              ++i ) {
            if ( (i->address - m_cmd_packets[0].address) % 32 == 0 )
                o << std::endl << std::hex << std::setfill('0') << std::setw(8) << i->address << ": ";
            typename vci_param::fast_data_t data = i->wdata;
            uint8_t be = i->be;
            for ( uint8_t j = 0; j<vci_param::B; ++j ) {
                if ( be & 1 )
                    o << std::hex << std::noshowbase << std::setfill('0') << std::setw(2)
                      << (uint32_t)(data & 0xff);
                else
                    o << "XX";
                be >>= 1;
                data >>= 8;
            }
            o << " ";
        }
    }

    o << std::endl;
    o << "Response: " << (m_rsp_packets[0].rerror ? "ERROR" : "OK")
      << " " << m_rsp_packets.size() << " cells";

    if ( m_cmd_packets[0].cmd == vci_param::CMD_STORE_COND )
        o << " " << (m_rsp_packets[0].rdata == vci_param::STORE_COND_ATOMIC
                     ? "atomic" : "non atomic");

    if ( m_cmd_packets[0].cmd == vci_param::CMD_READ
         || m_cmd_packets[0].cmd == vci_param::CMD_LOCKED_READ ) {
        typename vci_param::fast_addr_t address = m_cmd_packets[0].address;
        for ( typename rsp_list_t::const_iterator i = m_rsp_packets.begin();
              i != m_rsp_packets.end();
              ++i ) {
            if ( (address - m_cmd_packets[0].address) % 32 == 0 )
                o << std::endl << std::hex << std::setfill('0') << std::setw(8) << address << ": ";
            typename vci_param::fast_data_t data = i->rdata;
            for ( uint8_t j = 0; j<vci_param::B; ++j ) {
                o << std::hex << std::noshowbase << std::setfill('0') << std::setw(2)
                  << (uint32_t)(data & 0xff);
                data >>= 8;
            }
            o << " ";
            address += vci_param::B;
        }
    }
    o << std::endl;
}

tmpl(void)::reset()
{
    m_rsp_packets.clear();
    m_cmd_packets.clear();
    m_cmd_ended = false;
    m_rsp_ended = false;
}

#undef tmpl
#define tmpl(...) template<typename vci_param> __VA_ARGS__ VciLogger<vci_param>

tmpl()::VciLogger(
	sc_core::sc_module_name insname,
	const soclib::common::MappingTable &mt )
	: BaseModule(insname),
       m_pending_commands(new VciLoggerElem<vci_param>[1<<(vci_param::S+vci_param::T)]),
      p_resetn("resetn"),
      p_clk("clk"),
      p_vci("vci")
{
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();
}

tmpl()::~VciLogger()
{
    delete [] m_pending_commands;
}

tmpl(void)::handle_txn(const VciLoggerElem<vci_param> &elem)
{
    std::cout << name() << ' ' << elem << std::endl;
}

tmpl(void)::transition()
{
    size_t cindex = (p_vci.srcid.read() << vci_param::T) | p_vci.trdid.read();
    size_t rindex = (p_vci.rsrcid.read() << vci_param::T) | p_vci.rtrdid.read();

    if ( p_vci.cmdval.read() && p_vci.cmdack.read() )
        m_pending_commands[cindex].takeCmd(p_vci);

    if ( p_vci.rspval.read() && p_vci.rspack.read() ) {
        m_pending_commands[rindex].takeRsp(p_vci);
        if ( p_vci.reop.read() ) {
            handle_txn(m_pending_commands[rindex]);
            m_pending_commands[rindex].reset();
        }
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


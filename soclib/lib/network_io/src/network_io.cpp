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
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 */

#include <algorithm>
#include <string.h>
#include <sstream>
#include <cassert>

#include "exception.h"
#include "network_io.h"
#include "network_packet_io.hh"

namespace soclib { namespace common {

NetworkIo::NetworkIo( const std::string &method,
                      const std::string &arg1,
                      const std::string &arg2 )
    : m_method(method),
      m_arg1(arg1),
      m_arg2(arg2)
{
    factory_map_t &map = factories();
    factory_map_t::iterator i = map.find( method );
    std::cout << "Available methods:";
    for ( factory_map_t::const_iterator i = map.begin();
          i != map.end();
          ++i )
        std::cout << " " << i->first;
    std::cout << std::endl;
    if ( i == map.end() )
        throw soclib::exception::RunTimeError(std::string("Method not found: ")+method);
    factory_t f = i->second;
    m_io = f(arg1, arg2);
    assert(m_io);
    m_io->ref();
}

NetworkIo::NetworkIo( const NetworkIo& ref )
    : m_method(ref.m_method),
      m_arg1(ref.m_arg1),
      m_arg2(ref.m_arg2),
      m_io(ref.m_io)
{
    m_io->ref();
}

const NetworkIo& NetworkIo::operator=( const NetworkIo& ref )
{
    m_method = ref.m_method;
    m_arg1 = ref.m_arg1;
    m_arg2 = ref.m_arg2;
    m_io->unref();
    m_io = ref.m_io;
    m_io->ref();
    return *this;
}

NetworkIo::~NetworkIo()
{
    m_io->unref();
}

bool NetworkIo::has_packet() const
{
    return m_io->has_packet();
}

NetworkPacket* NetworkIo::get_packet()
{
    return m_io->get_packet();
}

void NetworkIo::put_packet(NetworkPacket*packet)
{
    m_io->put_packet(packet);
}

NetworkIo::factory_map_t &NetworkIo::factories()
{
    static factory_map_t f;
    return f;
}

void NetworkIo::register_factory( const std::string &name, factory_t factory )
{
    factory_map_t &fs = factories();
    std::cout << "Registering a factory for method " << name << std::endl;
    fs[name] = factory;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


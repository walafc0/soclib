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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_NETWORK_IO_H_
#define SOCLIB_NETWORK_IO_H_

#include <string>
#include <ios>
#include <map>
#include <stdint.h>

namespace soclib { namespace common {

class NetworkPacketIo;

class NetworkPacket
{
    friend class NetworkPacketIo;

protected:
    uint8_t *m_data;
    size_t m_size;

    void print( std::ostream & ) const;
public:
    NetworkPacket( const NetworkPacket &ref );
    NetworkPacket( const void *data, size_t size );

    ~NetworkPacket();

    const NetworkPacket & operator=( const NetworkPacket &ref );

    const void *data() const;
    const size_t size() const;

    friend std::ostream &operator<<( std::ostream &o, const NetworkPacket &p )
    {
        p.print(o);
        return o;
    }
};

class NetworkIo
{
private:
	std::string m_method;
	std::string m_arg1;
	std::string m_arg2;

    NetworkPacketIo *m_io;

public:
	NetworkIo( const NetworkIo &ref );
	const NetworkIo &operator=( const NetworkIo &ref );
	NetworkIo( const std::string &method, const std::string &arg1, const std::string &arg2 );
	~NetworkIo();

    bool has_packet() const;
    NetworkPacket* get_packet();
    void put_packet(NetworkPacket*);

    typedef NetworkPacketIo *(*factory_t)( const std::string &, const std::string &);
    static void register_factory( const std::string &name, factory_t factory );
private:
    typedef std::map<std::string, factory_t> factory_map_t;

    static factory_map_t &factories();
};

}}

#endif /* SOCLIB_NETWORK_IO_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


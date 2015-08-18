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
 *
 * Maintainers: nipo
 */

#include "../include/network_io.h"
#include "network_packet_io.hh"
#include "static_init_code.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

namespace soclib { namespace common {

namespace __hex_ {

class HexInput
{
	std::string m_filename;
	std::ifstream m_input;
public:
	HexInput( const std::string &filename )
		: m_filename(filename),
		  m_input(filename.c_str(), std::ifstream::in)
	{
	}

	bool has_packet() const
	{
		return !m_input.eof();
	}

	NetworkPacket *get_packet()
	{
		size_t size;
		std::string packet_hex;
		m_input >> std::dec >> size >> packet_hex;
		if ( packet_hex.size() / 2 != size ) {
			std::cerr << "Spurious frame in " << m_filename << ": "
					  << size << " " << packet_hex << std::endl;
			return NULL;
		}

		uint8_t *adata = new uint8_t[size];
        uint8_t *data = adata;
		for ( size_t i=0; i<size; ++i ) {
			const std::string chr = packet_hex.substr( 2*i, 2 );
			long chri = strtol( chr.c_str(), NULL, 16 );
			*data++ = (char)chri;
		}
		NetworkPacket *packet = new NetworkPacket( adata, size );
		delete [] adata;
		return packet;
	}
};

class HexOutput
{
	std::string m_filename;
	std::ofstream m_output;
public:
	HexOutput( const std::string &filename )
		: m_filename(filename),
		  m_output(filename.c_str(), std::ofstream::out)
	{
	}

	void put_packet(NetworkPacket *packet)
	{
		m_output << std::dec << std::setw(2) << packet->size() << " ";
		const uint8_t *data = (uint8_t*)(packet->data());
		for ( size_t i=0; i<packet->size(); ++i ) {
			m_output << std::hex << std::setw(2) << std::setfill('0') << (((unsigned int)data[i])&0xff);
		}
		m_output << std::endl;
	}
};

}

using namespace __hex_;

class NetworkPacketIoHex
	: public NetworkPacketIo
{
	HexInput *m_input;
	HexOutput *m_output;

public:
	NetworkPacketIoHex( HexInput *input, HexOutput *output )
		: m_input(input),
		  m_output(output)
	{
	}

	~NetworkPacketIoHex()
	{
		delete m_input;
		delete m_output;
	}

	bool has_packet() const
	{
		if ( m_input )
			return m_input->has_packet();
		return false;
	}

	void put_packet(NetworkPacket *packet)
	{
		if ( m_output )
			return m_output->put_packet(packet);
	}

	NetworkPacket *get_packet()
	{
		if ( m_input )
			return m_input->get_packet();
		return NULL;
	}
};

NetworkPacketIo *NetworkPacketIoHex_factory( const std::string &arg1, const std::string &arg2 )
{
    HexInput *in = NULL;
    if ( arg1 != "" )
        in = new HexInput(arg1);
    HexOutput *out = NULL;
    if ( arg2 != "" )
        out = new HexOutput(arg2);
	return new NetworkPacketIoHex(in, out);
}

STATIC_INIT_CODE({
	NetworkIo::register_factory( "hex", &NetworkPacketIoHex_factory );
})

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


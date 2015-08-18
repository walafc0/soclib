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

#include "soclib_endian.h"
#include "exception.h"

#include <iostream>
#include <cstdlib>
#include <cstdio>

namespace soclib { namespace common {

namespace __pcap_ {

typedef struct pcap_hdr_s {
    uint32_t magic_number;   /* magic number */
    uint16_t version_major;  /* major version number */
    uint16_t version_minor;  /* minor version number */
    int32_t  thiszone;       /* GMT to local correction */
    uint32_t sigfigs;        /* accuracy of timestamps */
    uint32_t snaplen;        /* max length of captured packets, in octets */
    uint32_t network;        /* data link type */
} pcap_hdr_t;

typedef struct pcaprec_hdr_s {
    uint32_t ts_sec;         /* timestamp seconds */
    uint32_t ts_usec;        /* timestamp microseconds */
    uint32_t incl_len;       /* number of octets of packet saved in file */
    uint32_t orig_len;       /* actual length of packet */
} pcaprec_hdr_t;

class PcapInput
{
	std::string m_filename;
	std::FILE * m_input;
    bool m_must_swap;

    inline void swap_header( pcap_hdr_t &header )
    {
        if ( !m_must_swap )
            return;

        header.magic_number = soclib::endian::uint32_swap(header.magic_number);
        header.version_major = soclib::endian::uint16_swap(header.version_major);
        header.version_minor = soclib::endian::uint16_swap(header.version_minor);
        header.thiszone = soclib::endian::uint32_swap(header.thiszone);
        header.sigfigs = soclib::endian::uint32_swap(header.sigfigs);
        header.snaplen = soclib::endian::uint32_swap(header.snaplen);
        header.network = soclib::endian::uint32_swap(header.network);
    }

    inline void swap_rec_header( pcaprec_hdr_t &header )
    {
        if ( !m_must_swap )
            return;
        header.ts_sec = soclib::endian::uint32_swap(header.ts_sec);
        header.ts_usec = soclib::endian::uint32_swap(header.ts_usec);
        header.incl_len = soclib::endian::uint32_swap(header.incl_len);
        header.orig_len = soclib::endian::uint32_swap(header.orig_len);
    }

public:
	PcapInput( const std::string &filename )
		: m_filename(filename)
	{
        m_input = std::fopen( filename.c_str(), "rb" );
        if ( ! m_input )
            throw soclib::exception::RunTimeError(std::string("File not found: ")+filename);

        pcap_hdr_t header;
        ssize_t len = std::fread(&header, sizeof(header), 1, m_input );
        if (len != 1)
            throw soclib::exception::RunTimeError(std::string("Unable to read PCAP header in ")+filename);
        if ( header.magic_number == 0xa1b2c3d4 )
            m_must_swap = false;
        else if ( header.magic_number == 0xd4c3b2a1 )
            m_must_swap = true;
        else
            throw soclib::exception::RunTimeError(std::string("Invalid PCAP magic number in ")+filename);

        swap_header(header);

        if ( header.version_major != 2 || header.version_minor != 4 )
            throw soclib::exception::RunTimeError(std::string("Unknown PCAP dump version in ")+filename);
	}

	bool has_packet() const
	{
		return ! std::feof(m_input);
	}

	NetworkPacket *get_packet()
	{
        pcaprec_hdr_t header;
        if ( std::fread( &header, sizeof(pcaprec_hdr_t), 1, m_input ) == 1 ) {
            swap_rec_header(header);
            size_t size = header.incl_len;

            uint8_t *adata = new uint8_t[size];
            NetworkPacket *packet = NULL;
            if ( std::fread( adata, size, 1, m_input ) == 1 )
                packet = new NetworkPacket( adata, size );
            delete [] adata;
            return packet;
        }
        return NULL;
	}
};

class PcapOutput
{
	std::string m_filename;
	std::FILE * m_output;
public:
	PcapOutput( const std::string &filename )
		: m_filename(filename)
	{
        m_output = std::fopen( filename.c_str(), "wb" );
        if ( ! m_output )
            throw soclib::exception::RunTimeError(std::string("Unable to open ")+filename+" for output");

        pcap_hdr_t header = {0xa1b2c3d4, 2, 4, 0, 0, 65535, 1};
        std::fwrite(&header, sizeof(header), 1, m_output);
	}

	void put_packet(NetworkPacket *packet)
	{
        pcaprec_hdr_t header = {0, 0, packet->size(), packet->size()};
        std::fwrite(&header, sizeof(header), 1, m_output);
        std::fwrite(packet->data(), 1, packet->size(), m_output);
	}
};

}

using namespace __pcap_;

class NetworkPacketIoPcap
	: public NetworkPacketIo
{
	PcapInput *m_input;
	PcapOutput *m_output;

public:
	NetworkPacketIoPcap( PcapInput *input, PcapOutput *output )
		: m_input(input),
		  m_output(output)
	{
	}

	~NetworkPacketIoPcap()
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

NetworkPacketIo *NetworkPacketIoPcap_factory( const std::string &arg1, const std::string &arg2 )
{
    PcapInput *in = NULL;
    if ( arg1 != "" )
        in = new PcapInput(arg1);
    PcapOutput *out = NULL;
    if ( arg2 != "" )
        out = new PcapOutput(arg2);
	return new NetworkPacketIoPcap(in, out);
}

STATIC_INIT_CODE({
	NetworkIo::register_factory( "pcap", &NetworkPacketIoPcap_factory );
})

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


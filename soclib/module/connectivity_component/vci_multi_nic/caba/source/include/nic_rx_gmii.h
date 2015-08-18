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
 *         Alain Greiner <alain.greiner@lip6.fr> July 2008
 *         Clement Devigne <clement.devigne@etu.upmc.fr>
 *         Sylvain Leroy <sylvain.leroy@lip6.fr>
 *         Cassio Fraga <cassio.fraga@lip6.fr>
 *
 * Maintainers: alain
 */

//////////////////////////////////////////////////////////////////////////////////
// File         : gmii_rx.h
// Date         : 01/06/2012
// Authors      : Alain Greiner
//////////////////////////////////////////////////////////////////////////////////
// This class implements a packet receiver, acting as a PHY component,
// and respecting the GMII protocol (one byte per cycle).
// It implements the NIC_MODE_FILE & NIC_MODE_SYNTHESIS RX backend 
// (depending on the "use_file" constructor parameter).
// The inter-packet gap is constructor argument.
// - In NIC_MODE_FILE , it reads packets in the file "nic_rx_file.txt", 
//   that must be stored in the same directory as the "top.ccp" file.
// - In NIC_MODE_SYNTHESIS , the packet length is a pseudo random value
//   (between 64 and 1084 bytes). The 12 first bytes are computed as following:
//   * B0  to B5  ( DST MAC ) defined by the DST_MAC_4 & DST_MAC_2 parameters.
//   * B6  to B11 ( SRC MAC ) B6 and B5 contain a 16 bits packet index
//////////////////////////////////////////////////////////////////////////////////
// It has 2 constructor parameters:
// - uint32_t gap      : number of cycles between packets
// - bool     use_file : NIC_MODE_FILE when true.
//////////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_CABA_GMII_RX_H
#define SOCLIB_CABA_GMII_RX_H

#include <inttypes.h>
#include <systemc>
#include <assert.h>
#include <string>
#include <cstring>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "ethernet_crc.h"
#include "nic_rx_backend.h"

#define DST_MAC_4    0x12345678
#define DST_MAC_2    0xBEBE

namespace soclib {
namespace caba {

using namespace sc_core;

/////////////////////////////////////
class NicRxGmii : public NicRxBackend
{
    // structure constants
    const std::string       m_name;          // Component name
    const uint32_t	        m_gap;           // inter packets gap (cycles)
    const bool              m_use_file;      // NIC_MODE_FILE when true
    std::ifstream           m_file;          // input file descriptor 
    EthernetChecksum        m_crc;           // checksum computer

    // registers
    bool                    r_fsm_gap;       // inter_packet state when true
    uint32_t                r_counter;       // cycles counter (both gap and plen)
    uint8_t 	            r_buffer[2048];  // local buffer containing one packet
    uint32_t                r_plen;          // actual packet length (in bytes)
    uint32_t                r_pid;           // packet index (for debug)

    ///////////////////////////////////////////////////////////////////
    // This function is used to convert ascii to hexa.
    ///////////////////////////////////////////////////////////////////

    uint8_t atox (uint8_t el)
    {
        if((el >= 48) and (el <= 57))
            return (el - 48);
        else if ((el >= 65) and (el <= 70))
            return ((el - 65)+10);
        else if((el >= 97) and (el <= 102))
            return ((el-97)+10);
        else
            return -1;
    }

    ///////////////////////////////////////////////////////////////////
    // This function synthetize one packet.
    // It updates the r_pid, r_buffer, and r_plen variables.
    // The packet length is randomly generated:
    // - 64 <= length <= 1084 (multiple of 4 bytes)
    ///////////////////////////////////////////////////////////////////
    virtual void build_one_packet()
    {
        uint32_t n;
        uint32_t rand = random();

        // define packet length 
        r_plen = 64 + (rand & 0x3FC); 

        // store MAC destination
        r_buffer[0]  = (uint8_t)(DST_MAC_2 >>  8);
        r_buffer[1]  = (uint8_t)(DST_MAC_2      );
        r_buffer[2]  = (uint8_t)(DST_MAC_4 >> 24);
        r_buffer[3]  = (uint8_t)(DST_MAC_4 >> 16);
        r_buffer[4]  = (uint8_t)(DST_MAC_4 >>  8);
        r_buffer[5]  = (uint8_t)(DST_MAC_4      );

        // store MAC source
        r_buffer[6]  = (uint8_t)(r_pid>>8); 
        r_buffer[7]  = (uint8_t)(r_pid   );
        r_buffer[8]  = 0;
        r_buffer[9]  = 0;
        r_buffer[10] = 0;
        r_buffer[11] = 0;

        // store payload
        for ( n = 12 ; n < (r_plen - 4 ) ; n++ ) r_buffer[n] = 0xBE;

        // compute checksum
        uint32_t checksum = 0;
        for ( n = 0 ; n < (r_plen - 4) ; n++ )
        {
            checksum = m_crc.update( checksum , (uint32_t)r_buffer[n] );
        }

        // store checksum
        r_buffer[r_plen - 1] = (checksum & 0xFF000000) >> 24;
        r_buffer[r_plen - 2] = (checksum & 0x00FF0000) >> 16;
        r_buffer[r_plen - 3] = (checksum & 0x0000FF00) >> 8;
        r_buffer[r_plen - 4] = (checksum & 0x000000FF);

        // update packet index
        r_pid++;
    }

    ///////////////////////////////////////////////////////////////////
    // This function is used to read one packet from the input file
    // It updates the r_buffer and the r_plen variables.
    ///////////////////////////////////////////////////////////////////
    virtual void read_one_packet()
    {
        uint32_t      data = 0;
        std::string   string;

        // string contains one packet and r_plen contains number of bytes
        m_file >> r_plen >> string;

        // check end of file and restart it
        if (m_file.eof())
        {
            m_file.clear();
            m_file.seekg(0, std::ios::beg);
            m_file >> r_plen >> string;
        }

        // Preamble consumption (at least 0xD5 as preamble is mandatory)
        size_t preamb_cpt = 0;  //counts bytes on the preamble

        while (atox(string[2*preamb_cpt])==0x5 and atox(string[2*preamb_cpt+1])==0x5)
        {
            preamb_cpt++;
        }
        // SFD consumption
        if (atox(string[2*preamb_cpt])==0xD and atox(string[2*preamb_cpt+1])==0x5)
        {
            preamb_cpt++;
        }
        else
        {
            // Packet without SFD
            std::cout << "RX_GMII ERROR: must contain SFD byte 0xD5" << std::endl;
            exit(0);
        }

        // convert two hexadecimal characters into one uint8_t
        for (size_t n = 0; n < (r_plen << 1) ; n++)
        {
            data = (data << 4)| atox(string[n+2*preamb_cpt]);
            if (n%2)
            {
                r_buffer[n>>1] = data;
                data = 0;
            }
        }
    } // end read_one packet()

public:

    ////////////////////
    virtual void reset()
    {
        r_fsm_gap   = true;
        r_counter   = m_gap;
        memset( r_buffer, 0, 2048 );
    }

    ///////////////////////////////////////////////////////////////////
    // To reach the 1 Gbyte/s throughput, this method must be called
    // by the NIC component at all cycles of a 125MHz clock.
    // It is therefore written as a transition and contains a
    // two states FSM to introduce the inter-packet waiting cycles.
    ///////////////////////////////////////////////////////////////////
    virtual void get( bool*     dv,         // data valid
                      bool*     er,         // data error
                      uint8_t*  dt  )       // data value
    {
        if ( r_fsm_gap )    // inter-packet state
        {
            *dv = false;
            *er = false;
            *dt = 0;

            r_counter = r_counter - 1;

            if (r_counter == 0 ) // end of gap
            {
                r_fsm_gap = false;
                if ( m_use_file ) read_one_packet();
                else              build_one_packet();
            }
        }
        else    // running packet state
        {
            *dv = true;
            *er = false;
            *dt = r_buffer[r_counter];

            r_counter = r_counter + 1;

            if ( r_counter == r_plen ) // end of packet
            {
                r_counter   = m_gap;
                r_fsm_gap   = true;
            }
        }
    } // end get()


    ////////////////////////////////////
    //  constructor
    /////////////////////////////////////
    NicRxGmii( uint32_t           gap,
               bool               use_file )

        : m_gap( gap ),
          m_use_file( use_file ),
          m_file( "nic_rx_file.txt" ),
          r_pid( 0 )
    {
        if ( use_file and (m_file == 0) )
        {
            std::cout << "[NIC ERROR] in NicRxGmii : cannot open file nic_rx_file.txt"
                      << std::endl;
            exit(0);
        }
    } 

    //////////////////
    // destructor)
    //////////////////
    virtual ~NicRxGmii()
    {
        m_file.close();
    }

}; // end NicRxGmii

}}

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4




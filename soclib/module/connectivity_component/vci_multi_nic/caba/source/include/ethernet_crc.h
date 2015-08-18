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
 *         Alain Greiner <alain.greiner@lip6.fr> November 2014
 */

    
#ifndef ETHERNET_CRC_TABLE_H
#define ETHERNET_CRC_TABLE_H

namespace soclib {
namespace caba {

//////////////////////
class EthernetChecksum
{

    /////////////////////////////////////////////////////////////////////////////////
    // This table is Ethernet compatible if the crc_register initialized with zeros
    // as described on IEEE 802.3 standard. 
    /////////////////////////////////////////////////////////////////////////////////

    uint32_t crc_table[16];

public:

    ////////////////////////////////////////////////////////////////////////////////
    // This function updates the current crc value for one byte
    ////////////////////////////////////////////////////////////////////////////////
    uint32_t update( uint32_t crc,
                     uint32_t byte )
    {
        uint32_t  tmp = (crc >> 4) ^ crc_table[(crc ^ (byte >> 0)) & 0xF];
        return          (tmp >> 4) ^ crc_table[(tmp ^ (byte >> 4)) & 0xF];
    }

    ///////////////////////////////
    //     constructor
    ///////////////////////////////
    EthernetChecksum()
    {
        crc_table[0x0] = 0x4DBDF21C;
        crc_table[0x1] = 0x500AE278;
        crc_table[0x2] = 0x76D3D2D4;
        crc_table[0x3] = 0x6B64C2B0;
        crc_table[0x4] = 0x3B61B38C;
        crc_table[0x5] = 0x26D6A3E8;
        crc_table[0x6] = 0x000F9344;
        crc_table[0x7] = 0x1DB88320;
        crc_table[0x8] = 0xA005713C;
        crc_table[0x9] = 0xBDB26158;
        crc_table[0xA] = 0x9B6B51F4;
        crc_table[0xB] = 0x86DC4190;
        crc_table[0xC] = 0xD6D930AC;
        crc_table[0xD] = 0xCB6E20C8;
        crc_table[0xE] = 0xEDB71064;
        crc_table[0xF] = 0xF0000000;
    }
};  // end EthernetChecksum

}}

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


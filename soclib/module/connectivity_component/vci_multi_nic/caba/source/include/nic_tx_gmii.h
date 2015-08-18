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
// File         : nic_tx_gmii.h
// Date         : 01/06/2012
// Authors      : Alain Greiner
//////////////////////////////////////////////////////////////////////////////////
// This class implements a packet transmitter, acting as a PHY component,
// and respecting the GMII protocol (one byte per cycle).
// It implements the NIC_MODE_FILE & NIC_MODE_SYNTHESIS TX backend.
// In both modes, it writes packets in the file "nic_tx_file.txt",
// that must be stored in the same directory as the "top.ccp" file.
//////////////////////////////////////////////////////////////////////////////////
// It has 0 constructor parameter.
//////////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_CABA_GMII_TX_H
#define SOCLIB_CABA_GMII_TX_H

#include <inttypes.h>
#include <systemc>
#include <assert.h>

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>

#include "nic_tx_backend.h"

#define PREAMBLE_SIZE 8
#define PREAMBLE "55555555555555D5"

namespace soclib { 
namespace caba {

using namespace sc_core;

/////////////////////////////////////
class NicTxGmii : public NicTxBackend
{
    // structure constants
    const std::string   m_name;          // Component name
    std::ofstream       m_file;          // output file descriptor

    // registers
    uint32_t            r_counter;       // cycles counter (used for both gap and plen)
    uint8_t	            r_buffer[2048];  // local buffer containing one packet

    ///////////////////////////////////////////////////////////////////
    // This function is used to write one packet to the input file
    ///////////////////////////////////////////////////////////////////
    virtual void write_one_packet()
    { 
        if (m_file)
        {
#ifdef SOCLIB_NIC_DEBUG
std::cout << "[NIC][" << __func__ << "] Writing one packet" << std::endl;
#endif
            m_file << std::dec << r_counter + PREAMBLE_SIZE << ' ';
            m_file << PREAMBLE;

            for ( size_t cpt = 0; cpt < r_counter ; cpt++ )
            {
                m_file << std::setfill('0') << std::setw(2) << std::hex 
                       << (uint32_t)r_buffer[cpt];
            }
            m_file << std::dec << std::endl;
        }
    } // end write_one_paclet()

public:

    ////////////////////
    virtual void reset()
    {
        r_counter = 0;
    }

    ///////////////////////////////////////////////////////////////////
    // To reach the 1 Gbyte/s throughput, this method must be called 
    // at all cycles of a 125MHz clock.
    ///////////////////////////////////////////////////////////////////
    virtual void put(bool     dv,          // data valid
                     uint8_t  dt)          // data value
    {
        if ( not dv and (r_counter != 0) )    // end of packet
        {
#ifdef SOCLIB_NIC_DEBUG
std::cout << "[NIC][" << __func__ << "] Putting " << std::hex << dt << std::dec << std::endl;
#endif
            write_one_packet();
            r_counter = 0;
        }
        else if ( dv )    // running packet
        {
            r_buffer[r_counter] = dt;
            r_counter           = r_counter + 1;
        }
    } // end put()
    

    /*!
     * \brief This method returns true if the RX chain is to be frozen.
     * \attention This function is only here to reflect the VHDL version of the NIC
     */
    virtual bool frz()
    {
        return false;
    }
                
    /////////////////////////////////////
    //    constructor 
    /////////////////////////////////////
    NicTxGmii( )
        : m_file( "nic_tx_file.txt", std::ios::out )
    {
        if ( m_file == 0 )
        {
            std::cout << "[NIC ERROR] in NicTxGmii : cannot open file nic_tx_file.txt" 
                      << std::endl;
            exit(0);
        }
    }

    ////////////////////
    // destructor
    ////////////////////
    virtual ~NicTxGmii()
    {
        m_file.close();
    }

}; // end NicTxGmii

}}

#endif 

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4




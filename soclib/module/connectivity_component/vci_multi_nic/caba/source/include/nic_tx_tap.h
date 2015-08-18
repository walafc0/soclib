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
 *         Sylvain Leroy <sylvain.leroy@lip6.fr>
 *
 * Maintainers: sylvain
 */

//////////////////////////////////////////////////////////////////////////////////
// File         : nic_tx_tap.h
// Date         : 01/06/2013
// Authors      : Sylvain Leroy
//////////////////////////////////////////////////////////////////////////////////
// This class implements a packet transmitter, acting as a PHY component,
// and respecting the GMII protocol (one byte per cycle).
// It implements the NIC_MODE_TAP RX backend.
// It writes packets to a TAP interface.
//////////////////////////////////////////////////////////////////////////////////
// It has 0 constructor parameter.
//////////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_CABA_TX_TAP_H
#define SOCLIB_CABA_TX_TAP_H

#if !defined(__APPLE__) || !defined(__MACH__)

#include <inttypes.h>
#include <systemc>
#include <assert.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>

#include "nic_tx_backend.h"

namespace soclib { 
namespace caba {

using namespace sc_core;

////////////////////////////////////
class NicTxTap : public NicTxBackend
{
    // structure constants
    int32_t             m_tap_fd;        // File descriptor for the TAP interface
    struct ifreq        *m_tap_ifr;      // TAP interface

    // registers
    uint32_t            r_counter;       // cycles counter (used for both gap and plen)
    uint8_t	            r_buffer[2048];  // local buffer containing one packet
    

    ///////////////////////////////////////////////////////////////////
    // This function is used to write one packet to the input file
    // The CRC is removed...
    ///////////////////////////////////////////////////////////////////
    void write_one_packet()
    {
        if (m_tap_fd)
        {

#ifdef SOCLIB_NIC_DEBUG
printf("[NIC_TX_TAP] sent packet : %u bytes on TAP fd %d\n", r_counter, m_tap_fd);
for (size_t i = 0; i < r_counter ; i++ ) 
{
    if (i != 0)
    {
        if ((i % 4) == 0)   printf(" ");
        if ((i % 72) == 0)  printf("\n");
    }
    printf("%02x", r_buffer[i]);
}
printf("\n");
#endif

            ::write(m_tap_fd, r_buffer, r_counter - 4);
        }
    }

public:

    ///////////////////////////////////////////////////////////////////
    // This function is used to set the value of the TAP file descriptor
    ///////////////////////////////////////////////////////////////////
    void set_fd(int     fd)
    {
        m_tap_fd = fd;
    }

    ///////////////////////////////////////////////////////////////////
    // This function is used to set the value of the TAP file descriptor
    ///////////////////////////////////////////////////////////////////
    void set_ifr(struct ifreq     *ifr)
    {
        m_tap_ifr = ifr;
    }

    ////////////////////
    virtual void reset()
    {
        r_counter = 0;
        memset( r_buffer, 0, 2048 );
    }

    ///////////////////////////////////////////////////////////////////
    // To reach the 1 Gbyte/s throughput, this method must be called 
    // at all cycles of a 125MHz clock.
    ///////////////////////////////////////////////////////////////////
    virtual void put(bool     dv,         // data valid
                     uint8_t  dt)         // data value
    {
        if (not dv and (r_counter != 0))    // end of packet
        {
            write_one_packet();
            r_counter = 0;
        }
        else
        {
            if (dv)    // start or running packet
            {
                r_buffer[r_counter] = dt;
                r_counter           = r_counter + 1;
            }
        }
    } // end put()

    
    ///////////////////////////////////
    //     constructor
    ///////////////////////////////////
    NicTxTap() { } 

    ///////////////////////////////////
    // destructor
    ///////////////////////////////////
    virtual ~NicTxTap() { }

}; // end NicTxTap

}}

#endif /* !defined(__APPLE__) || !defined(__MACH__) */

#endif /* SOCLIB_CABA_TX_TAP_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4




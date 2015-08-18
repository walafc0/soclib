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
// File         : nic_rx_tap.h
// Date         : 01/06/2013
// Authors      : Sylvain Leroy
//////////////////////////////////////////////////////////////////////////////////
// This class implements a packet receiver, acting as a PHY component,
// and respecting the GMII protocol (one byte per cycle).
// It implements the NIC_MODE_TAP TX backend.
// It reads packets from a TAP interface.
//////////////////////////////////////////////////////////////////////////////////
// It has 1 constructor parameters
// - uint32_t gap     : number of cycles between packets
//////////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_CABA_NIC_RX_TAP_H
#define SOCLIB_CABA_NIC_RX_TAP_H

#if !defined(__APPLE__) || !defined(__MACH__)

#include <inttypes.h>
#include <systemc>
#include <assert.h>

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

#include <errno.h>

#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>

#include "nic_rx_backend.h"

namespace soclib {
namespace caba {

using namespace sc_core;

////////////////////////////////////
class NicRxTap : public NicRxBackend
{
    // structure constants
    const uint32_t	    m_gap;
    int                 m_tap_fd;        // File descriptor for the TAP interface
    struct ifreq        *m_tap_ifr;      // TAP interface

    // registers
    bool                r_fsm_gap;       // inter_packet state when true
    int32_t             r_counter;       // cycles counter (used for both gap and plen)
    uint8_t 	        r_buffer[2048];  // local buffer containing one packet
    int32_t             r_plen;          // packet length (in bytes)


    ///////////////////////////////////////////////////////////////////
    // This function is used to read one packet from the input file
    // It must update the r_buffer and the r_plen variables.
    ///////////////////////////////////////////////////////////////////
    void read_one_packet()
    {
        if (m_tap_fd > 0)
        {
            r_plen = ::read(m_tap_fd, r_buffer, 2048);

            if (r_plen < 0) // Error during reading
            {
                std::cout << "[NIC ERROR] in nic_rx_tap" << std::endl;
                exit(0);
            }          
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
    int get_fd()
    {
        return m_tap_fd;
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
        r_fsm_gap   = true;
        r_counter   = m_gap;
        memset(r_buffer,0,2048);
    }

    ///////////////////////////////////////////////////////////////////
    // To reach the 1 Gbyte/s throughput, this method must be called
    // at all cycles of a 125MHz clock.
    // It is therefore written as a transition and contains a
    // two states FSM to introduce the Inter-Frame Gap waiting cycles.
    ///////////////////////////////////////////////////////////////////
    virtual void get(bool*     dv,         // data valid
                     bool*     er,         // data error
                     uint8_t*  dt)        // data value
    {
        if (r_fsm_gap)    // inter-packet state (IFG or waiting for the next trame)
        {
            *dv = false;
            *er = false;
            *dt = 0;

            if (r_counter == 0) // end of gap - wait for a packet
            {
                read_one_packet();
                if (r_plen <= 0) r_fsm_gap = true;
                else             r_fsm_gap = false;

            }
            else
            {
                r_counter--;
            }
        }
        else    // start or running packet
        {
            *dv = true;
            *er = false;
            *dt = r_buffer[r_counter];

            r_counter++;

            if ( r_counter == r_plen ) // end of packet
            {
                r_counter   = m_gap;
                r_fsm_gap   = true;
            }
        }
    } // end get()

    ///////////////////////////////////
    //    Constructor
    ///////////////////////////////////
    NicRxTap( uint32_t gap)
        : m_gap( gap ),
          m_tap_fd( -1 )
    {
    } 

    //////////////////
    // destructor
    //////////////////
    virtual ~NicRxTap()
    {
    }

}; // end NicRxTap

}}

#endif /* !defined(__APPLE__) || !defined(__MACH__) */

#endif /* SOCLIB_CABA_NIC_RX_TAP_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4




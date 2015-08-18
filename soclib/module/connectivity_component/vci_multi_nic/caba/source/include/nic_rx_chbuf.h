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
 *         Alain Greiner <alain.greiner@lip6.fr> July 2012
 *         Sylvain Leroy <sylvain.leroy@lip6.fr>
 *         Cassio Fraga <cassio.fraga@lip6.fr>
 *
 *
 * Maintainers: alain
 */

/**********************************************************************
 * File         : nic_rx_chbuf.h
 * Date         : 01/10/2012
 * Authors      : Alain Greiner
 **********************************************************************
 * This object implements an hardware chained buffer controler.
 * It is used to store received packets (from NIC to software),
 * by the multi-channels, network controller.
 * It contains two 4K bytes containers, acting as a two slots
 * "standard" chained buffer that can be accessed by software.
 *
 * Read and write accesses are not symetrical:
 *
 * - On the writer side (NIC), the number of containers is not visible:
 *   The WOK flag is true if there is a writable container.
 *   Both container and word addressing is handled by the chbuf itself.
 *   An internal timer defines the maximum waiting time for
 *   a partially filled container: This timer is re-initialised each
 *   time the container is released, and it is decremented at all cycles
 *   where no word is written, as soon as the first word has been written.
 *   The NIC writes one or several packets in an open container,
 *   after checking - for each packet - that there is enough free space
 *   and enough time to write the packet in the container.
 *   It releases the container if there is not enough space or time.
 *   The container header is automatically filled by the chbuf itself.
 * - On the reader side (soft), the two containers are visible,
 *   and each container is adressable as a classical synchronous
 *   memory bank supporting bursts: address registered at cycle (n),
 *   data available at cycle (n+1).
 **********************************************************************/

#ifndef SOCLIB_CABA_NIC_RX_CHBUF
#define SOCLIB_CABA_NIC_RX_CHBUF

#include <inttypes.h>
#include <systemc>
#include <assert.h>

namespace soclib {
namespace caba {

using namespace sc_core;

#define NIC_CONTAINER_SIZE          1024
#define MAX_PACKET                  ((NIC_CONTAINER_SIZE*4-4)/62)

// writer commands (from the NIC)
enum rx_chbuf_wcmd_t
    {
        RX_CHBUF_WCMD_NOP,         // no operation
        RX_CHBUF_WCMD_WRITE,       // write one packet word
        RX_CHBUF_WCMD_LAST,        // write last word in a packet
        RX_CHBUF_WCMD_RELEASE,     // release (close) container
    };

// reader commands (from the software)
enum rx_chbuf_rcmd_t
    {
        RX_CHBUF_RCMD_NOP,         // no operation
        RX_CHBUF_RCMD_READ,        // read one word in selected container
        RX_CHBUF_RCMD_RELEASE,     // release selected container
    };

class NicRxChbuf
{
    // structure constants
    const uint32_t      m_timeout;
    const uint32_t      m_debug;

    // internal registers
    uint32_t            r_ptw_word;          // word write pointer (in container)
    uint32_t            r_ptw_cont;          // container write pointer
    uint32_t            r_ptr_word;          // word read pointer (in container)
    uint32_t            r_ptr_cont;          // container read pointer
    uint32_t            r_pkt_index;         // packet index in a container
    uint32_t            r_pkt_length;        // packet length counter
    int32_t             r_timer;             // cycle counter for timeout
    bool                r_full[2];           // containers status

    // containers
    uint32_t**          r_cont;              // data[2][1024]

public:

    /////////////
    void reset()
    {
        r_full[0]      = false;
        r_full[1]      = false;
        r_cont[0][0]   = 0;
        r_cont[1][0]   = 0;
        r_ptw_word     = (MAX_PACKET>>1)+1;
        r_ptw_cont     = 0;
        r_ptr_word     = 0;
        r_ptr_cont     = 0;
        r_pkt_index    = 0;
        r_pkt_length   = 0;
        r_timer        = m_timeout;
    }

    /////////////////////////////////////////////////////////////////////////////
    // This method updates  the internal chbuf state,
    // depending on both cmd_w and cmd_r commands.
    // It must be called at each cycle.
    /////////////////////////////////////////////////////////////////////////////
    void update(rx_chbuf_wcmd_t      cmd_w,      // writer command
                uint32_t             wdata,      // data to be written
                //uint32_t             padding,    // number of padding bytes
                uint32_t             pkt_length, // current packet number of bytes
                rx_chbuf_rcmd_t      cmd_r,      // reader command
                uint32_t             ptr_cont,   // container index for read
                uint32_t             ptr_word)   // word index for read
    {
        // update registers depending on cmd_w

        if ( cmd_w == RX_CHBUF_WCMD_WRITE )      // write one packet word
        {
            assert( (r_full[r_ptw_cont] == false ) and 
            "ERROR in NIX_RX_CHBUF : WRITE request in a full container");

            assert( (r_ptw_word < NIC_CONTAINER_SIZE) and
            "ERROR in NIC_RX_CHBUF : write pointer overflow" );

            r_cont[r_ptw_cont][r_ptw_word] = wdata;
            r_ptw_word                     = r_ptw_word + 1;
            r_pkt_length                   = pkt_length;
        }
        else if ( cmd_w == RX_CHBUF_WCMD_LAST )  // write last word in packet
                                                 // and write packet length
        {
            assert( (r_full[r_ptw_cont] == false ) and 
            "ERROR in NIX_RX_CHBUF : WRITE request in a full container");

            assert( (r_ptw_word < NIC_CONTAINER_SIZE) and
            "ERROR in NIC_RX_CHBUF : write pointer overflow" );

            assert( (r_pkt_index < MAX_PACKET) and
            "ERROR in NIC_RX_CHBUF : packet index larger than MAX_PACKET-1" );

            uint32_t    plen         = r_pkt_length;                // final packet size 
            bool        odd          = (r_pkt_index & 0x1);         // odd packet index
            uint32_t    word         = (r_pkt_index >> 1) + 1;      // for container header 

            r_cont[r_ptw_cont][r_ptw_word] = wdata;
            r_ptw_word                     = r_ptw_word + 1;
            if (odd)
            {
                r_cont[r_ptw_cont][word]   = (r_cont[r_ptw_cont][word] & 0xFFFF0000) | plen;
            }
            else
            {
                r_cont[r_ptw_cont][word]   = (r_cont[r_ptw_cont][word] & 0x0000FFFF) | plen<<16;
            }
            r_pkt_index                    = r_pkt_index + 1;
            r_pkt_length                   = 0;
        }
        else if ( cmd_w == RX_CHBUF_WCMD_RELEASE ) // release the current container
                                                   // and update packet number & word number
        {
            assert( (r_full[r_ptw_cont] == false ) and 
            "ERROR in NIX_RX_CHBUF : RELEASE request on a full container");

            if ( m_debug )
            printf("<NIC_RX_CHBUF> Container %d released by hardware\n", r_ptw_cont );

            r_full[r_ptw_cont]     = true;
            r_cont[r_ptw_cont][0]  = (r_ptw_word << 16) | (r_pkt_index & 0x0000FFFF);

            // reset to default values
            r_ptw_word             = ((MAX_PACKET>>1)+1);
            r_ptw_cont             = (r_ptw_cont + 1) % 2;
            r_pkt_index            = 0;
            r_timer                = m_timeout;
        }
        else // cmd_w == RX_CHBUF_WCMD_NOP
        {
            if ( r_timer <= 0 ) // time_out : release the current container
                                // and update packet number
            {
                if ( m_debug )
                printf("<NIC_RX_CHBUF> Container %d released by timeout\n", r_ptw_cont );

                r_full[r_ptw_cont]     = true;
                r_cont[r_ptw_cont][0]  = (r_ptw_word<<16) | r_pkt_index;
                r_ptw_word             =((MAX_PACKET>>1)+1);
                r_ptw_cont             = (r_ptw_cont + 1) % 2;
                r_pkt_index            = 0;
                r_timer                = m_timeout;
            }
            else if ( r_ptw_word > ((MAX_PACKET>>1)+1) ) // decrement after first word
            {
                r_timer                = r_timer - 1;
            }
        }

        // update registers depending on cmd_r

        if ( cmd_r == RX_CHBUF_RCMD_READ )       // register a read request
        {
            assert( (ptr_cont < 2) and 
            "ERROR in NIX_RX_CHBUF : READ request with container index > 2");

            assert( (r_full[ptr_cont] == true ) and
            "ERROR in NIX_RX_CHBUF : READ request in a container not full");

            assert( (ptr_word < NIC_CONTAINER_SIZE) and
            "ERROR in NIC_RX_CHBUF : READ pointer overflow" );

            r_ptr_word = ptr_word;
            r_ptr_cont = ptr_cont;
        }
        else if ( cmd_r == RX_CHBUF_RCMD_RELEASE ) // release a full container
        {
            assert( (ptr_cont < 2) and 
            "ERROR in NIX_RX_CHBUF : RELEASE request with container index > 2");

            assert( (r_full[ptr_cont] == true ) and
            "ERROR in NIX_RX_CHBUF : RELEASE request on a container not full");

            if( m_debug )
            printf("<NIC_RX_CHBUF> Container %d released by software\n", ptr_cont );

            r_full[ptr_cont]    = false;
            r_cont[ptr_cont][0] = 0;
        }
    } // end update()

    //////////////////////////////////////////////////////////////////////////
    // This method returns the word value in the container addressed
    // in the previous cycle. It does not modify the chbuf state.
    //////////////////////////////////////////////////////////////////////////
    uint32_t data()
    {
        return r_cont[r_ptr_cont][r_ptr_word];
    }

    //////////////////////////////////////////////////////////////////////////
    // This method is analogous to the above, but returns 64 bit word
    //////////////////////////////////////////////////////////////////////////
    uint64_t data64()
    {
        return  ((uint64_t)r_cont[r_ptr_cont][r_ptr_word+1]<<32)|
            ((uint64_t)r_cont[r_ptr_cont][r_ptr_word]);
    }

    /////////////////////////////////////////////////////////////////////////
    // This method returns the number of free bytes in the container
    // currently written. It does not modify the chbuf state.
    /////////////////////////////////////////////////////////////////////////
    uint32_t space()
    {
        return (1024 - r_ptw_word)<<2;
    }

    /////////////////////////////////////////////////////////////
    // This method returns true if there is a full container.
    // It does not modify the chbuf state.
    /////////////////////////////////////////////////////////////
    bool full(uint32_t container)
    {
        return r_full[container];
    }

    /////////////////////////////////////////////////////////////
    // This method returns true if there is a free container.
    // It does not modify the chbuf state.
    /////////////////////////////////////////////////////////////
    bool wok()
    {
        return ( not (r_full[0] and r_full[1]) );
    }
    /////////////////////////////////////////////////////////////
    // This method returns the current value of timer.
    // It does not modify the chbuf state.
    /////////////////////////////////////////////////////////////
    uint32_t time()
    {
        return r_timer;
    }

    /////////////////////////////////////////////////////////////
    // This method prints the chbuf state, 
    // including the two containers headers.
    /////////////////////////////////////////////////////////////
    void print_trace( uint32_t channel,
                      uint32_t detail )
    {
        bool     display;
        uint32_t packets;
        uint32_t words;

        for ( size_t cont=0 ; cont<2 ; cont++ )
        {
            if ( r_full[cont] )
            {
                packets = r_cont[cont][0] & 0x0000FFFF;
                words   = r_cont[cont][0] >> 16;
                display = true;
            }
            else if ( r_ptw_cont == cont )
            {
                packets = r_pkt_index;
                words   = r_ptw_word;
                display = true;
            }
            else
            {
                display = false;
            }

            if ( display )
            {
                std::cout << std::dec << "RX_CHBUF[" << channel
                          << "] / container[" << cont
                          << "] / full = " << r_full[cont]
                          << " / words = " << words
                          << " / packets = " << packets << std::endl;
            }

            if ( display && detail )
            {            
                for ( size_t p = 0 ; p < packets ; p++ )
                {
                    uint32_t word = 1 + (p>>1);
                    uint32_t plen;

                    if ( (p&0x1) == 0x1 ) plen = r_cont[cont][word] >> 16;
                    else                  plen = r_cont[cont][word] & 0x0000FFFF;
                    std::cout << "- plen[" << p << "] = " << plen << std::endl;
                }
            }
        }
    } // end print_trace()

    //////////////////////////////////////////////////////////////
    // constructor allocates the memory for the containers.
    //////////////////////////////////////////////////////////////
    NicRxChbuf( const uint32_t timeout,    
                const uint32_t debug )
        : m_timeout( timeout ),
          m_debug( debug )
    {
        r_cont    = new uint32_t*[2];
        r_cont[0] = new uint32_t[NIC_CONTAINER_SIZE];
        r_cont[1] = new uint32_t[NIC_CONTAINER_SIZE];
    }

    //////////////////
    // destructor
    //////////////////
    ~NicRxChbuf()
    {
        delete [] r_cont[0];
        delete [] r_cont[1];
        delete [] r_cont;
    }

}; // end NicRxChbuf

}}

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4




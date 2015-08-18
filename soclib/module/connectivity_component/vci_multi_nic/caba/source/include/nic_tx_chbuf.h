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
 * Maintainers: alain
 */

/**********************************************************************
 * File         : nic_tx_chbuf.h
 * Date         : 01/10/2012
 * Authors      : Alain Greiner
 **********************************************************************
 * This object implements an hardware chained buffer controler,
 * It is used to store transmited packets (from software to NIC),
 * by a multi-channels, GMII compliant, network controller.
 * It contains two  4K bytes containers, acting as a two slots
 * chained buffer that can be accessed by software.
 *
 * Read an write accesses are not symetrical:
 *
 * - On the writer side (soft), the two containers are visible,
 *   and each container is adressable as a classical synchronous
 *   memory bank: address & data registered at cycle (n) / actual
 *   write done at cycle (n+1).
 * - On the reader side (NIC), the number of containers is not visible:
 *   The ROK flag is true if there is a readable container.
 *   The NIC reads all paquets in the container, after checking the number
 *   of packets, and the packet lengths in the container header,
 *   using the plen() and npkt() access methods.
 *   It releases the container when all packets have been transmitted.
 **********************************************************************/

#ifndef SOCLIB_CABA_NIC_TX_CHBUF
#define SOCLIB_CABA_NIC_TX_CHBUF

#include <inttypes.h>
#include <systemc>
#include <assert.h>

namespace soclib {
namespace caba {

using namespace sc_core;

#define NIC_CONTAINER_SIZE      1024                            // words
#define MAX_PACKET              ((NIC_CONTAINER_SIZE*4-4)/62)

// writer commands (software)
enum tx_chbuf_wcmd_t
{
    TX_CHBUF_WCMD_NOP,       // no operation
    TX_CHBUF_WCMD_WRITE,     // write one word in selected container
    TX_CHBUF_WCMD_RELEASE,   // release selected container
} ;

// reader commands (NIC)
enum tx_chbuf_rcmd_t
{
    TX_CHBUF_RCMD_NOP,       // no operation
    TX_CHBUF_RCMD_READ,      // read one word
    TX_CHBUF_RCMD_LAST,      // read one word
    TX_CHBUF_RCMD_RELEASE,   // release container
    TX_CHBUF_RCMD_SKIP,      // skip current packet
};

//////////////////
class NicTxChbuf
//////////////////
{
    // structure constants
    const uint32_t   m_debug;

    // registers
    uint32_t            r_ptr_word;          // word read pointer (in container)
    uint32_t            r_ptr_cont;          // container read pointer
    uint32_t            r_pkt_index;         // current packet index in a container
    uint32_t            r_ptr_first;         // pointer on first word in current packet
    bool                r_full[2];           // containers status

    // containers
    uint32_t**          r_cont;              // data[2][1024]

public:

    /////////////
    void reset()
    {
        r_full[0]     = false;
        r_full[1]     = false;
        r_cont[0][0]  = 0;
        r_cont[1][0]  = 0;
        r_ptr_word    = (MAX_PACKET/2)+1;
        r_ptr_first   = (MAX_PACKET/2)+1;
        r_ptr_cont    = 0;
        r_pkt_index   = 0;
    }

    ///////////////////////////////////////////////////////////////////////
    // This method updates  the internal channel state,
    // depending on both cmd_w and cmd_r commands.
    // It must be called at each cycle.
    ///////////////////////////////////////////////////////////////////////
    void update( tx_chbuf_wcmd_t      cmd_w,     // writer command
                 uint32_t             wdata,     // data to be written
                 uint32_t             ptw_cont,  // container index for write
                 uint32_t             ptw_word,  // word index for write
                 tx_chbuf_rcmd_t      cmd_r )    // reader command
    {
        // update registers depending on cmd_w (software)

        if ( cmd_w == TX_CHBUF_WCMD_WRITE )       // write one word
            {
                assert( (ptw_cont < 2) and
                        "ERROR in NIC_TX_CHBUF : WRITE request with container index > 2" );

                assert( (r_full[ptw_cont] == false) and
                        "ERROR in NIC_TX_CHBUF : WRITE request on container full" );

                assert( (ptw_word < NIC_CONTAINER_SIZE) and
                        "ERROR in NIC_TX_CHBUF : WRITE pointer overflow" );

                r_cont[ptw_cont][ptw_word]   = wdata;
            }
        else if ( cmd_w == TX_CHBUF_WCMD_RELEASE ) // release the container
            {
                assert( (ptw_cont < 2) and
                        "ERROR in NIC_TX_CHBUF : RELEASE request with container index > 2" );

                assert( (r_full[ptw_cont] == false) and
                        "ERROR in NIC_TX_CHBUF : RELEASE request on container full" );

                r_full[ptw_cont] = true;
            }

        // update registers depending on cmd_r (NIC)

        if ( cmd_r == TX_CHBUF_RCMD_READ )       // read one word
            {
                assert( (r_ptr_word < NIC_CONTAINER_SIZE) and
                        "ERROR in NIC_TX_CHBUF : READ pointer overflow" );

                assert( ( r_full[r_ptr_cont] == true) and
                        "ERROR in NIC_TX_CHBUF : READ request on container not full" );

                assert( (r_pkt_index < MAX_PACKET) and
                        "ERROR in NIC_TX_CHBUF : READ packet index larger than 61" );

                r_ptr_word = r_ptr_word + 1;
            }
        else if ( cmd_r == TX_CHBUF_RCMD_LAST )  // read last word in a packet
                                                 // and updates packet index
            {
                assert( (r_ptr_word < NIC_CONTAINER_SIZE) and
                        "ERROR in NIC_TX_CHBUF : READ LAST pointer overflow" );

                assert( ( r_full[r_ptr_cont] == true) and
                        "ERROR in NIC_TX_CHBUF : READ LAST request on container not full" );

                assert( (r_pkt_index < MAX_PACKET) and
                        "ERROR in NIC_TX_CHBUF : READ LAST packet index larger than 61" );

                r_ptr_word               = r_ptr_word + 1;
                r_pkt_index              = r_pkt_index + 1;
                r_ptr_first              = r_ptr_word;
            }
        else if ( cmd_r == TX_CHBUF_RCMD_RELEASE ) // release the current container
            {
                assert( ( r_full[r_ptr_cont] == true) and
                        "ERROR in NIC_TX_CHBUF : RELEASE request on container not full" );

                r_full[r_ptr_cont] = false;
                r_pkt_index = 0;
                r_ptr_word  = (MAX_PACKET/2)+1;
                r_ptr_first  = (MAX_PACKET/2)+1;
                r_ptr_cont  = (r_ptr_cont + 1) % 2;
            }
        else if (cmd_r == TX_CHBUF_RCMD_SKIP) // skip packet in current container
            {
                assert( ( r_full[r_ptr_cont] == true) and
                        "ERROR in NIC_TX_CHBUF : RELEASE request on container not full" );

                uint32_t plen_tmp = this->plen();
                uint32_t words;
                if ( (plen_tmp & 0x3) == 0 ) words = plen_tmp >> 2;
                else                         words = (plen_tmp >> 2) + 1;

                r_ptr_word  = r_ptr_first + words ;
                r_pkt_index = r_pkt_index + 1;
                r_ptr_first = r_ptr_word;
            }
    } // end update()

    /////////////////////////////////////////////////////////////////////////////
    // This method returns the current word value in the current container.
    // It does not modify the channel state.
    ///i/////////////////////////////////////////////////////////////////////////
    uint32_t data()
    {
        return r_cont[r_ptr_cont][r_ptr_word];
    }

    /////////////////////////////////////////////////////////////////////////////
    // This method returns the number of bytes in a packet.
    // It does not modify the channel state.
    /////////////////////////////////////////////////////////////////////////////
    uint32_t plen()
    {
        bool        odd     = (r_pkt_index & 0x1);
        uint32_t    word    = (r_pkt_index / 2) + 1;

        if (odd) return (r_cont[r_ptr_cont][word] & 0x0000FFFF);
        else     return (r_cont[r_ptr_cont][word] >> 16);
    }
    ////////////////////////////////////////////////////////////////////////////
    // This method returns the number of packets in the current container.
    // It does not modify the channel state.
    ////////////////////////////////////////////////////////////////////////////
    uint32_t npkt()
    {
        return r_cont[r_ptr_cont][0] & 0x0000FFFF;
    }

    /////////////////////////////////////////////////////////////
    // This method returns true if there is a full container.
    // It does not modify the channel state.
    /////////////////////////////////////////////////////////////
    bool rok()
    {
        return (r_full[0] or r_full[1]);
    }

    /////////////////////////////////////////////////////////////
    // This method returns true if there is a full container.
    // It does not modify the channel state.
    /////////////////////////////////////////////////////////////
    bool full( uint32_t container )
    {
        if (container) return r_full[1];
        else           return r_full[0];
    }

    /////////////////////////////////////////////////////////////
    // This method prints the chbuf state,
    // including the two containers headers.
    /////////////////////////////////////////////////////////////
    void print_trace( uint32_t channel,
                      uint32_t detail )
    {
        uint32_t packets;
        uint32_t words;

        for ( size_t cont=0 ; cont<2 ; cont++ )
        {
            if ( r_full[cont] )
            {
                packets = r_cont[cont][0] & 0x0000FFFF;
                words   = r_cont[cont][0] >> 16;

                std::cout << std::dec << "TX_CHBUF[" << channel
                          << "] / container[" << cont
                          << "] / full = " << r_full[cont]
                          << " / words = " << words
                          << " / packets = " << packets << std::endl;

                if ( detail )
                {
                    for ( size_t p = 0 ; p < packets ; p++ )
                    {
                        uint32_t word = 1 + (p>>1);
                        uint32_t plen;

                        if ( (p&0x1) == 0x1 ) plen = r_cont[cont][word] >> 16;
                        else                  plen = r_cont[cont][word] & 0x0000FFFF;
                        std::cout << "_ plen[" << p << "] = " << plen << std::endl;
                    }
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////
    // constructor allocates the memory for the containers.
    //////////////////////////////////////////////////////////////
    NicTxChbuf( const uint32_t debug )
        : m_debug( debug )
    {
        r_cont    = new uint32_t*[2];
        r_cont[0] = new uint32_t[NIC_CONTAINER_SIZE];
        r_cont[1] = new uint32_t[NIC_CONTAINER_SIZE];
    }

    //////////////////
    // destructor
    //////////////////
    ~NicTxChbuf()
    {
        delete [] r_cont[0];
        delete [] r_cont[1];
        delete [] r_cont;
    }

}; // end NicTxChbuf

}}

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4




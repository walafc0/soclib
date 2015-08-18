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
 *         Alain Greiner <alain.greiner@lip6.fr> Juin 2012
 *         Clement Devigne <clement.devigne@etu.upmc.fr>
 *         Marc Kakou <marc.kakou@etu.upmc.fr>
 *         Sylvain Leroy <sylvain.leroy@lip6.fr>
 *         Cassio Fraga <cassio.fraga@lip6.fr>
 */

//////////////////////////////////////////////////////////////////////////////////
// This component is a multi-channels, GMII compliant, NIC controller.
// If the system clock frequency is larger or equal to the GMII clock
// frequency (ie 125 MHz), it can support a throughput of 1 Gigabit/s).
// This component makes the assumption that the VCI RDATA & WDATA fields
// have 32 bits. The number of channels is a constructor parameter,
// and cannot be larger than 8.
// 
// Regarding the physical interface, this simulation model supports three modes
// of operation, defined by a constructor parameter:
// - NIC_MODE_FILE: Both the RX packets stream an the TX packets stream are 
//   read an writtent from / to dedicated files "nic_rx_file.txt" and 
//   "nic_tx_dile.txt", stored in teh same directory as the top.cpp file.
// - NIC_MODE_SYNTHESIS: The TX packet stream is still the "nic_tx_file.txt",
//   but the RX packet stream is synththised with pseudo-random packet
//   length (between 64 and 1538 bytes), and a pseudo random source  
//   MAC addresses (8 possible values).
// - NIC_MODE_TAP: The TX and RX packet streams are send and received to
//   and from the physical network controller of the workstation
//   running the simulation.
//
// It is a VCI target with no DMA capability : All data transfers must be
// performed by software, or by an external DMA engine.
//
// The packet length can have any value, between 64 to 1538 bytes.
// The data transfer unit between software and the NIC is a container,
// containing an integer number of variable size packets.
//
// Each channel contains two RX containers and two TX containers,
// organised as standard multi_chained_buffers, in order to be accessed by
// an external CMA engine.
//
// A container is a 4 Kbytes buffer.
// The max number of packets in a container is 66 packets.
// The first 34 words of a container are the container header :
//
//     word0  	| 	NB_WORDS	|	NB_PACKETS	|
//     word1	|	PLEN[0]		|	PLEN[1]		|
//      ...	    |	.......		|	........	|
//     word33	|	PLEN[64]	|	PLEN[65]	|
//
//  - NB_PACKETS is the actual number of packets in the container.
//	- NB_WORDS   is the number of useful words in the container.
//	- PLEN[i]    is the number of bytes for the packet[i].
//
// The packets are stored in the (1024 - 34) following words,
// and the packets are word-aligned.
//
// In a virtualized environment each channel segment will be mapped in
// the address space of a different virtual machine.
// Each channel takes a segment of 32 Kbytes in the address space,
// to simplify the address decoding, but only 20K bytes are used.
//
// 	- The first 4 Kbytes contain the RX_0 container data
// 	- The next  4 Kbytes contain the RX_1 container data
// 	- The next  4 Kbytes contain the TX_0 container data
// 	- The next  4 Kbytes contain the TX_1 container data
// 	- The next  4 Kbytes contain the channel addressable registers
//      * NIC_RX_STS_0      : RX_0 status               (read/write)
//      * NIC_RX_STS_1      : RX_1 status               (read/write)
//      * NIC_TX_STS_0      : TX_0 status               (read/write)
//      * NIC_TX_STS_1      : TX_1 status               (read/write)
//      * NIC_RX_DESC_LO_0  : RX_0 descriptor low word  (read/write)
//      * NIC_RX_DESC_HI_0  : RX_0 descriptor high word (read/write)
//      * NIC_RX_DESC_LO_1  : RX_1 descriptor low word  (read/write)
//      * NIC_RX_DESC_HI_1  : RX_1 descriptor high word (read/write)
//      * NIC_TX_DESC_LO_0  : TX_0 descriptor low word  (read/write)
//      * NIC_TX_DESC_HI_0  : TX_0 descriptor high word (read/write)
//      * NIC_TX_DESC_LO_1  : TX_1 descriptor low word  (read/write)
//      * NIC_TX_DESC_HI_1  : TX_1 descriptor high word (read/write)
// 		* NIC_MAC_4         : MAC @ 32 LSB bits         (read_only)
// 		* NIC_MAC_2         : MAC @ 16 MSB bits         (read_only)
//      * NIC_RX_RUN        : RX channel X activated    (write_only)
//      * NIC_TX_RUN        : TX channel X activated    (write_only)
//
// A container descriptor occupies 8 bytes (unsigned long long):
// - the 26 LSB bits of LOW WORD contain bits[6:31] of the buffer satus paddr 
// - bits[26:31] of LOW WORD and bits[0:19] of HIGH WORD contain bits[6:31] of the
//   buffer paddr
// - the 12 MSB bits of HIGH WORD contain the common address extension of the buffer
//   and its status
// The buffer status occupies 64 bytes but only the last bit is useful (1 for full and 0 for empty)      
// The buffer address and its status address must be 64 bytes aligned (bits[0:5] equal to 0)
//
// On top of the channels segments is the hypervisor segment, taking 4 Kbytes:
// It cannot be accessed by the virtual machines.
//
// 	- It contains global configuration registers (read/write)
// 		* NIC_G_VIS             : bitfield / bit N = 0 -> channel N is disabled
// 		* NIC_G_ON 		        : NIC active if non zero (inactive at reset)
// 		* NIC_G_BC_ENABLE 	    : boolean / broadcast enabled if true (disabled at reset)
// 		* NIC_G_TDM_ENABLE 	    : boolean / enable TDM dor TX if true (disabled at reset)
// 		* NIC_G_TDM_PERIOD      : value of TDM time slot
//      * NIC_G_PYPASS_ENABLE   : boolean / enable bypass for TX  (enabled at reset)
// 		* NIC_G_MAC_4[8]        : initialize the channels MAC_4 (wired value at reset)
// 		* NIC_G_MAC_2[8]        : initialize the channels MAC_2 (wired value at reset)
//
// - It contains various event counters for statistics (read/write)
//
//      * NIC_G_NPKT_RX_G2S_RECEIVED       : number of packets received on GMII RX port
//      * NIC_G_NPKT_RX_G2S_DISCARDED      : number of RX packets discarded by RX_G2S FSM
//
//      * NIC_G_NPKT_RX_DES_SUCCESS        : number of RX packets transmited by RX_DES FSM
//      * NIC_G_NPKT_RX_DES_TOO_SMALL      : number of discarded too small RX packets
//      * NIC_G_NPKT_RX_DES_TOO_BIG        : number of discarded too big Renforces locality and parallelismX packets
//      * NIC_G_NPKT_RX_DES_MFIFO_FULL     : number of discarded RX packets for fifo full
//      * NIC_G_NPKT_RX_DES_CRC_FAIL       : number of discarded RX packets for checksum
//
//      * NIC_G_NPKT_RX_DISPATCH_RECEIVED  : number of packets received by RX_DISPATCH FSM
//      * NIC_G_NPKT_RX_DISPATCH_BROADCAST : number of broadcast RX packets received
//      * NIC_G_NPKT_RX_DISPATCH_DST_FAIL  : number of discarded RX packets for DST MAC
//      * NIC_G_NPKT_RX_DISPATCH_CH_FULL   : number of discarded RX packets for channel full
//
//      * NIC_G_NPKT_TX_DISPATCH_RECEIVED  : number of packets received by TX_DISPATCH FSM
//      * NIC_G_NPKT_TX_DISPATCH_TOO_SMALL : number of discarded too small TX packets
//      * NIC_G_NPKT_TX_DISPATCH_TOO_BIG   : number of discarded too big TX packets
//      * NIC_G_NPKT_TX_DISPATCH_SRC_FAIL  : number of discarded TX packets because SRC MAC
//      * NIC_G_NPKT_TX_DISPATCH_BROADCAST : number of broadcast TX packets received
//      * NIC_G_NPKT_TX_DISPATCH_BYPASS    : number of bypassed TX->RX packets
//      * NIC_G_NPKT_TX_DISPATCH_TRANSMIT  : number of transmit TX packets
//
////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <ethernet_crc.h>
#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "alloc_elems.h"
#include "../include/vci_multi_nic.h"
#include "../../../include/soclib/multi_nic.h"

namespace soclib {
namespace caba {


////////////////////////////////////////////////////////////////
//   Hidden hardware parameters
////////////////////////////////////////////////////////////////

#define DEFAULT_TDM_PERIOD             50000
#define RX_TIMEOUT                     100000

////////////////////////////////////////////////////////////////
//   Detailed debug parameters
////////////////////////////////////////////////////////////////

#define RX_G2S_DEBUG        0
#define RX_DISPATCH_DEBUG   0
#define RX_CHBUF_DEBUG      0

#define TX_DISPATCH_DEBUG   0
#define TX_CHBUF_DEBUG      0

////////////////////////////////////////////////////////////////

#define tmpl(t) template<typename vci_param> t VciMultiNic<vci_param>

////////////////////////////////////////////////////////////////
// return total number Bytes count in rx_gmii
////////////////////////////////////////////////////////////////
tmpl(uint32_t)::get_total_len_rx_gmii()
{
    return r_total_len_rx_gmii.read();
}

////////////////////////////////////////////////////////////////
// return total number Bytes successfully writen in rx_chbuf
////////////////////////////////////////////////////////////////
tmpl(uint32_t)::get_total_len_rx_chan()
{
    return r_total_len_rx_chan.read();
}

////////////////////////////////////////////////////////////////
// return total number Bytes writen in the output_file
////////////////////////////////////////////////////////////////
tmpl(uint32_t)::get_total_len_tx_gmii()
{
    return r_total_len_tx_gmii.read();
}

///////////////////////////////////////////////////////////////
// return total number Bytes read from a tx_chbuf
///////////////////////////////////////////......//////////////
tmpl(uint32_t)::get_total_len_tx_chan()
{
    return r_total_len_tx_chan.read();
}

///////////////////////////////////////////////////////////////
// This function returns value stored in hyper registers
///////////////////////////////////////////////////////////////
tmpl(uint32_t)::read_hyper_register(uint32_t addr)
{
    uint32_t word = (addr & 0x00000FFF) >> 2;
    uint32_t data = 0;

    if ( (word >= NIC_G_MAC_4) and (word < NIC_G_MAC_4 + 8) )
    {
        uint32_t channel = word - NIC_G_MAC_4;
        return r_channel_mac_4[channel];
    }

    if ( (word >= NIC_G_MAC_2) and (word < NIC_G_MAC_2 + 8) )
    {
        uint32_t channel = word - NIC_G_MAC_2;
        return r_channel_mac_2[channel];
    }

    switch(word)
    {
        case NIC_G_BC_ENABLE:
            data = r_global_bc_enable.read();
            break;
        case NIC_G_ON:
            data = r_global_nic_on.read();
            break;
        case NIC_G_NB_CHAN:
            data = r_global_nic_nb_chan.read();
            break;
        case NIC_G_TDM_ENABLE:
            data = r_global_tdm_enable.read();
            break;
        case NIC_G_TDM_PERIOD:
            data = r_global_tdm_period.read();
            break;
        case NIC_G_VIS:
            data = r_global_active_channels.read();
            break;
        case NIC_G_BYPASS_ENABLE:
            data = r_global_bypass_enable.read();
            break;

        case NIC_G_NPKT_RX_G2S_RECEIVED :
            data = r_rx_g2s_npkt_received.read();
            break;
        case NIC_G_NPKT_RX_G2S_DISCARDED :
            data = r_rx_g2s_npkt_discarded.read();
            break;

        case NIC_G_NPKT_RX_DES_SUCCESS :
            data = r_rx_des_npkt_success.read();
            break;
        case NIC_G_NPKT_RX_DES_TOO_SMALL :
            data = r_rx_des_npkt_too_small.read();
            break;
        case NIC_G_NPKT_RX_DES_TOO_BIG :
            data = r_rx_des_npkt_too_big.read();
            break;
        case NIC_G_NPKT_RX_DES_MFIFO_FULL :
            data = r_rx_des_npkt_mfifo_full.read();
            break;
        case NIC_G_NPKT_RX_DES_CRC_FAIL :
            data = r_rx_des_npkt_cs_fail.read();
            break;

        case NIC_G_NPKT_RX_DISPATCH_RECEIVED :
            data = r_rx_dispatch_npkt_received.read();
            break;
        case NIC_G_NPKT_RX_DISPATCH_BROADCAST :
            data = r_rx_dispatch_npkt_broadcast.read();
            break;
        case NIC_G_NPKT_RX_DISPATCH_DST_FAIL :
            data = r_rx_dispatch_npkt_dst_fail.read();
            break;
        case NIC_G_NPKT_RX_DISPATCH_CH_FULL :
            data = r_rx_dispatch_npkt_channel_full.read();
            break;

        case NIC_G_NPKT_TX_DISPATCH_RECEIVED :
            data = r_tx_dispatch_npkt_received.read();
            break;
        case NIC_G_NPKT_TX_DISPATCH_TOO_SMALL :
            data = r_tx_dispatch_npkt_too_small.read();
            break;
        case NIC_G_NPKT_TX_DISPATCH_TOO_BIG :
            data = r_tx_dispatch_npkt_too_big.read();
            break;
        case NIC_G_NPKT_TX_DISPATCH_SRC_FAIL :
            data = r_tx_dispatch_npkt_src_fail.read();
            break;
        case NIC_G_NPKT_TX_DISPATCH_BROADCAST :
            data = r_tx_dispatch_npkt_broadcast.read();
            break;
        case NIC_G_NPKT_TX_DISPATCH_BYPASS :
            data = r_tx_dispatch_npkt_bypass.read();
            break;
        case NIC_G_NPKT_TX_DISPATCH_TRANSMIT :
            data = r_tx_dispatch_npkt_transmit.read();
            break;

        default:
            std::cout << "ERROR in VCI_MULTI_NIC : illegal hyper register read"
                      << " : address = " << std::hex << addr << std::endl;
    }
    return data;
} // end read_hyper_register()

///////////////////////////////////////////////////////////////////////
// This function returns value stored in channel registers
///////////////////////////////////////////////////////////////////////
tmpl(uint32_t)::read_channel_register(uint32_t addr)
{

    uint32_t channel = (addr & 0x00038000) >> 15;
    uint32_t word    = (addr & 0x00000FFF) >> 2;
    uint32_t data    = 0;

    switch(word)
        {
        case NIC_RX_STS_0:
            data = (uint32_t)r_rx_chbuf[channel]->full(0);
            break;
        case NIC_RX_STS_1:
            data = (uint32_t)r_rx_chbuf[channel]->full(1);
            break;
        case NIC_TX_STS_0:
            data = (uint32_t)r_tx_chbuf[channel]->full(0);
            break;            
        case NIC_TX_STS_1:
            data = (uint32_t)r_tx_chbuf[channel]->full(1);
            break;
        case NIC_RX_DESC_LO_0:
            data = (uint32_t)(r_channel_rx_desc_0[channel].read() & 0xFFFFFFFF);
            break;
        case NIC_RX_DESC_LO_1:
            data = (uint32_t)(r_channel_rx_desc_1[channel].read() & 0xFFFFFFFF);
            break;
        case NIC_TX_DESC_LO_0:
            data = (uint32_t)(r_channel_tx_desc_0[channel].read() & 0xFFFFFFFF);
            break;
        case NIC_TX_DESC_LO_1:
            data = (uint32_t)(r_channel_tx_desc_1[channel].read() & 0xFFFFFFFF);
            break;
        case NIC_RX_DESC_HI_0:
            data = (uint32_t)(r_channel_rx_desc_0[channel].read() >> 32);
            break;
        case NIC_RX_DESC_HI_1:
            data = (uint32_t)(r_channel_rx_desc_1[channel].read() >> 32);
            break;
        case NIC_TX_DESC_HI_0:
            data = (uint32_t)(r_channel_tx_desc_0[channel].read() >> 32);
            break;
        case NIC_TX_DESC_HI_1:
            data = (uint32_t)(r_channel_tx_desc_1[channel].read() >> 32);
            break;
        case NIC_MAC_4:
            data = r_channel_mac_4[channel].read();
            break;
        case NIC_MAC_2:
            data = r_channel_mac_2[channel].read();
            break;
        default:
            std::cout << "ERROR in VCI_MULTI_NIC : illegal channel register read"
                      << " : address = " << std::hex << addr << std::endl;
            exit(0);
        }
    return data;
} // end read_channel_register()

/////////////////////////
tmpl(void)::transition()
/////////////////////////
{
    if (!p_resetn)
    {
        r_global_bc_enable              = false;
        r_global_nic_on                 = false;
        r_global_nic_nb_chan            = m_channels;
        r_global_active_channels        = 0;
        r_global_tdm_enable             = false;
        r_global_tdm_period             = DEFAULT_TDM_PERIOD;
        r_global_tdm_timer              = DEFAULT_TDM_PERIOD;
        r_global_tdm_channel            = 0;
        r_global_bypass_enable          = true;

        for ( size_t k=0 ; k<m_channels ; k++ )
        {
            r_channel_rx_run[k]         = false;
            r_channel_tx_run[k]         = false;
            r_channel_mac_4[k]          = m_default_mac_4 + k;
            r_channel_mac_2[k]          = m_default_mac_2;
        }

        r_vci_fsm                       = VCI_IDLE;

        r_rx_g2s_fsm                    = RX_G2S_IDLE;
        r_rx_g2s_npkt_received          = 0;
        r_rx_g2s_npkt_discarded         = 0;

        r_rx_des_fsm                    = RX_DES_IDLE;
        r_rx_des_npkt_success           = 0;
        r_rx_des_npkt_too_small         = 0;
        r_rx_des_npkt_too_big           = 0;
        r_rx_des_npkt_mfifo_full        = 0;
        r_rx_des_npkt_cs_fail           = 0;

        r_rx_dispatch_fsm               = RX_DISPATCH_IDLE;
        r_rx_dispatch_bp                = false;
        r_rx_dispatch_npkt_received     = 0;
        r_rx_dispatch_npkt_broadcast    = 0;
        r_rx_dispatch_npkt_dst_fail     = 0;
        r_rx_dispatch_npkt_channel_full = 0;

        r_tx_dispatch_fsm               = TX_DISPATCH_IDLE;
        r_tx_dispatch_channel           = 0;
        r_tx_dispatch_npkt_received     = 0;
        r_tx_dispatch_npkt_too_small    = 0;
        r_tx_dispatch_npkt_too_big      = 0;
        r_tx_dispatch_npkt_src_fail     = 0;
        r_tx_dispatch_npkt_broadcast    = 0;
        r_tx_dispatch_npkt_bypass       = 0;
        r_tx_dispatch_npkt_transmit     = 0;

        r_tx_ser_fsm                    = TX_SER_IDLE;
        r_tx_ser_ifg                    = m_inter_frame_gap;

        r_tx_s2g_fsm                    = TX_S2G_IDLE;
        r_tx_s2g_checksum               = 0;

        r_rx_fifo_stream.init();
        r_tx_fifo_stream.init();
        r_rx_fifo_multi.reset();
        r_tx_fifo_multi.reset();
        r_bp_fifo_multi.reset();

        r_backend_rx->reset();
        r_backend_tx->reset();

        for ( size_t k = 0 ; k < m_channels ; k++ )
        {
            r_rx_chbuf[k]->reset();
            r_tx_chbuf[k]->reset();
        }

        r_total_cycles      = 0;
        r_total_len_rx_gmii = 0;
        r_total_len_rx_chan = 0;
        r_total_len_tx_chan = 0;
        r_total_len_tx_gmii = 0;

        return;
    } // end reset

    r_total_cycles = r_total_cycles.read() + 1;

    // rx_chbuf and tx_chbuf commands
    tx_chbuf_wcmd_t tx_chbuf_wcmd[m_channels];
    uint32_t        tx_chbuf_wdata     = 0;
    bool            tx_chbuf_two_words = false;    // two 32 bits words to be registered 
    uint32_t        tx_chbuf_wdata2    = 0;        // used when tx_chbuf_two_words is true
    uint32_t        tx_chbuf_cont      = 0;
    uint32_t        tx_chbuf_word      = 0;
    tx_chbuf_rcmd_t tx_chbuf_rcmd[m_channels];

    rx_chbuf_wcmd_t rx_chbuf_wcmd[m_channels];
    uint32_t        rx_chbuf_wdata     = 0;
    uint32_t        rx_chbuf_plen      = 0;
    rx_chbuf_rcmd_t rx_chbuf_rcmd[m_channels];
    uint32_t        rx_chbuf_cont      = 0;
    uint32_t        rx_chbuf_word      = 0;

    // default values for rx_chbuf and tx chbuf commands
    for ( size_t k=0 ; k<m_channels ; k++)
        {
            tx_chbuf_wcmd[k] = TX_CHBUF_WCMD_NOP;
            tx_chbuf_rcmd[k] = TX_CHBUF_RCMD_NOP;
            rx_chbuf_wcmd[k] = RX_CHBUF_WCMD_NOP;
            rx_chbuf_rcmd[k] = RX_CHBUF_RCMD_NOP;
        }

    // multi_fifos commands
    fifo_multi_wcmd_t rx_fifo_multi_wcmd    = FIFO_MULTI_WCMD_NOP;
    uint32_t          rx_fifo_multi_wdata   = 0;
    uint32_t          rx_fifo_multi_padding = 0;
    fifo_multi_rcmd_t rx_fifo_multi_rcmd    = FIFO_MULTI_RCMD_NOP;

    fifo_multi_wcmd_t tx_fifo_multi_wcmd    = FIFO_MULTI_WCMD_NOP;
    uint32_t          tx_fifo_multi_wdata   = 0;
    uint32_t          tx_fifo_multi_padding = 0;
    fifo_multi_rcmd_t tx_fifo_multi_rcmd    = FIFO_MULTI_RCMD_NOP;

    fifo_multi_wcmd_t bp_fifo_multi_wcmd    = FIFO_MULTI_WCMD_NOP;
    uint32_t          bp_fifo_multi_wdata   = 0;
    uint32_t          bp_fifo_multi_padding = 0;
    fifo_multi_rcmd_t bp_fifo_multi_rcmd    = FIFO_MULTI_RCMD_NOP;

    // stream_fifos commands
    bool     rx_fifo_stream_read            = true;    // always  try to get data
    bool     rx_fifo_stream_write           = false;
    uint16_t rx_fifo_stream_wdata           = 0;

    bool     tx_fifo_stream_read            = false;
    bool     tx_fifo_stream_write           = false;
    uint16_t tx_fifo_stream_wdata           = 0;

    //////////////////////////////////////////////////////////////////////////////
    // This VCI_FSM controls the VCI TARGET port.
    // The VCI DATA field can be 32 or 64 bits.
    // The VCI PLEN field and the VCI ADDRESS field must be multiple of 4.
    // We acknowledge the VCI command, and decode it in the IDLE state.
    // There is three types of transactions:
    // - The configuration (write) and status (read) accesses must be one flit,
    //   and the VCI PLEN must be 4 bytes.
    // - The read data transfers (from RX channel container) can use burst.
    //   The number of flits depends only on the VCI PLEN value, and does not
    //   use the VCI BE value.
    // - The write data transfers (to TX channel container) can use bursts.
    //   The number of flits depends on the VCI EOP and on the VCI BE fields.
    //   The VCI BE must be 0xF for 32 bits DATA, and can be 0x0F / 0xF0 / 0xFF 
    //   for 64 bits DATA.
    //////////////////////////////////////////////////////////////////////////////

    switch(r_vci_fsm.read())
    {
        //////////////
        case VCI_IDLE:  // decode the VCI command
        {
            if (p_vci.cmdval.read() )
            {
                typename vci_param::addr_t	address = p_vci.address.read();
                typename vci_param::cmd_t	cmd     = p_vci.cmd.read();
                typename vci_param::plen_t  plen    = p_vci.plen.read();
                typename vci_param::be_t    be      = p_vci.be.read();

                bool found = false;
                std::list<soclib::common::Segment>::iterator seg;
                for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ )
                {
                    if ( seg->contains(address) ) found = true;
                }

                assert ( ((plen & 0x3) == 0) and
                "ERROR in VCI_MULTI_NIC : PLEN field must be multiple of 4 bytes");

                assert ( found  and
                "ERROR in VCI_MULTI_NIC : ADDRESS is out of segment");

                uint32_t channel = (uint32_t)((address & 0x00038000) >> 15);
                bool     hyper   =            (address & 0x00040000);
                bool     burst   = not        (address & 0x00004000);
                bool     tx      =            (address & 0x00002000);
                uint32_t cont    = (uint32_t)((address & 0x00001000) >> 12);
                uint32_t word    = (uint32_t)((address & 0x00000FFF) >> 2);

                r_vci_address  = (uint32_t)address;
                r_vci_srcid	   = p_vci.srcid.read();
                r_vci_trdid	   = p_vci.trdid.read();
                r_vci_pktid	   = p_vci.pktid.read();
                r_vci_wdata    = p_vci.wdata.read();
                r_vci_be       = p_vci.be.read();
                r_vci_nwords   = p_vci.plen.read()>>2;  // number of 32 bits words

                // checking channel index
                if (not hyper)
                {
                    assert( (channel < m_channels) and
                    "VCI_MULTI_NIC error : The channel index (ADDR[17:15] is too large");
                }

                // decoding access type
                if (hyper)                // global register read or write
                {
                    assert( p_vci.eop.read() and
                    "ERROR in VCI_MULTI_NIC : global register access must be one flit");

                    assert( (plen == 4)  and
                           "ERROR in VCI_MULTI_NIC : global register access must have plen = 4");

                    if ( cmd==vci_param::CMD_WRITE )
                    {
                        assert( ( ((vci_param::B==8) and ((be==0x0F) or (be==0xF0))) or 
                                  ((vci_param::B==4) and (be==0xF)) ) and 
                        "ERROR in VCI_MULTI_NIC : global register write access must be 32 bits");

                        r_vci_fsm = VCI_WRITE_HYPER_REG;
                    }
                    else
                    {
                        r_vci_fsm = VCI_READ_HYPER_REG;
                    }
                }
                else if ( not burst )   // channel register read or write
                {
                    if (cmd==vci_param::CMD_WRITE)
                    {
                        assert( p_vci.eop.read() and
                        "ERROR in VCI_MULTI_NIC : channel register write access must be one flit");

                        assert( (plen == 4) and
                        "ERROR in VCI_MULTI_NIC : channel register write access must have plen = 4");

                        assert( ( ((vci_param::B==8) and ((be==0x0F) or (be==0xF0))) or 
                                  ((vci_param::B==4) and (be==0xF)) ) and 
                        "ERROR in VCI_MULTI_NIC : channel register write access must be 32 bits");

                        r_vci_fsm = VCI_WRITE_CHANNEL_REG;
                    }
                    else
                    {
                        assert( ((plen == 4 and p_vci.eop.read()) ||
                                ( word == NIC_RX_DESC_LO_0 ||
                                  word == NIC_RX_DESC_LO_1 ||
                                  word == NIC_TX_DESC_LO_0 ||
                                  word == NIC_TX_DESC_LO_1 )) and
                        "ERROR in VCI_MULTI_NIC : channel register read access must have one flit and plen = 4 except for registers containing buffer descriptors");

                        r_vci_fsm = VCI_READ_CHANNEL_REG;
                    }
                }
                else if ( tx )       // TX_BURST write transfer
                {
                    assert( (cmd==vci_param::CMD_WRITE) and
                    "ERROR in VCI_MULTI_NIC : TX buffer access must be write");

                    assert( not r_tx_chbuf[channel]->full(cont) and
                    "ERROR in VCI_MULTI_NIC : TX_BURST write access in full container");

                    assert( ( ((vci_param::B==8) and ((be==0x0F) or (be==0xF0) or (be==0xFF)))
                              or ((vci_param::B==4) and (be==0xF)) ) and 
                    "ERROR in VCI_MULTI_NIC : TX buffer write must be 32 bits words");

                    if ( p_vci.eop.read() ) r_vci_fsm = VCI_WRITE_TX_LAST;
                    else                    r_vci_fsm = VCI_WRITE_TX_BURST;
                }
                else if ( not tx )   // RX_BURST read transfer
                {
                    assert( (cmd==vci_param::CMD_READ) and
                    "ERROR in VCI_MULTI_NIC : RX buffer access must be read");

                    assert( ( ((vci_param::B==8) and ((be==0x0F) or (be==0xF0) or (be==0xFF)))
                              or ((vci_param::B==4) and (be==0xF)) ) and 
                    "ERROR in VCI_MULTI_NIC : RX buffer access must be 32 bits words");

                    assert( r_rx_chbuf[channel]->full(cont) and
                    "ERROR in VCI_MULTI_NIC : RX_BURST read access in container not full");

                    rx_chbuf_rcmd[channel] = RX_CHBUF_RCMD_READ;
                    rx_chbuf_cont          = cont;
                    rx_chbuf_word          = word;
                    r_vci_fsm              = VCI_READ_RX_BURST;
                }
                else     // illegal VCI command
                {
                    assert( false and
                    "ERROR in VCI_MULTI_NIC : illegal VCI command");
                }
            }
            break;
        }
        ////////////////////////
        case VCI_WRITE_TX_BURST: // write data[i-1] in tx_chbuf[k]
                                 // and read data[i] on VCI port
        {
            if ( p_vci.cmdval.read() )
            {
                // write data[i-1]
                uint32_t address       = r_vci_address.read();
                uint32_t channel       = (address & 0x00038000) >> 15;
                uint32_t be            = (uint32_t)r_vci_be.read();

                tx_chbuf_wcmd[channel] = TX_CHBUF_WCMD_WRITE;
                tx_chbuf_cont   = (address & 0x00001000) >> 12;
                tx_chbuf_word   = (address & 0x00000FFF) >> 2;

                if ( vci_param::B == 4 )
                {
                    tx_chbuf_wdata         = (uint32_t)r_vci_wdata.read();
                }
                else  // vci_param::B == 8
                {
                    if ( be == 0x0F) 
                    {
                        tx_chbuf_wdata     = (uint32_t)r_vci_wdata.read();
                    }
                    else if ( be == 0xF0 )
                    {
                        tx_chbuf_wdata     = (uint32_t)(r_vci_wdata.read()>>32);
                    }
                    else if ( be == 0xFF )
                    {
                        tx_chbuf_wdata     = (uint32_t)r_vci_wdata.read();
                        tx_chbuf_wdata2    = (uint32_t)(r_vci_wdata.read()>>32);
                        tx_chbuf_two_words = true;
                    }
                }

                // read data[i]
                r_vci_address = p_vci.address.read();
                r_vci_wdata   = p_vci.wdata.read();

                if ( p_vci.eop.read() ) r_vci_fsm = VCI_WRITE_TX_LAST;
            }
            break;
        }
        ///////////////////////
        case VCI_WRITE_TX_LAST: // write last word of burst in tx_chbuf[k]
                                // and send VCI write response
        {
            if ( p_vci.rspack.read() )
            {
                uint32_t address       = r_vci_address.read();
                uint32_t channel       = (address & 0x00038000) >> 15;
                uint32_t be            = (uint32_t)r_vci_be.read();

                tx_chbuf_wcmd[channel] = TX_CHBUF_WCMD_WRITE;
                tx_chbuf_cont          = (address & 0x00001000) >> 12;
                tx_chbuf_word          = (address & 0x00000FFF) >> 2;

                if ( vci_param::B == 4 )
                {
                    tx_chbuf_wdata         = (uint32_t)r_vci_wdata.read();
                }
                else  // vci_param::B == 8
                {
                    if ( be == 0x0F) 
                    {
                        tx_chbuf_wdata     = (uint32_t)r_vci_wdata.read();
                    }
                    else if ( be == 0xF0 )
                    {
                        tx_chbuf_wdata     = (uint32_t)(r_vci_wdata.read()>>32);
                    }
                    else if ( be == 0xFF )
                    {
                        tx_chbuf_wdata     = (uint32_t)r_vci_wdata.read();
                        tx_chbuf_wdata2    = (uint32_t)(r_vci_wdata.read()>>32);
                        tx_chbuf_two_words = true;
                    }
                }

                r_vci_fsm = VCI_IDLE;
            }
            break;
        }
        ///////////////////////
        case VCI_READ_RX_BURST: // return data[i] from rx_chbuf[k] in VCI response
                                // and send read command for data[i+1] to rx_chbuf[k]
                                // depending on nwords. 
        {
            if ( p_vci.rspack.read() )
            {
                if ( vci_param::B == 4 )
                {
                    if (r_vci_nwords.read() > 1 )  // not the last flit
                    {
                        uint32_t address       = r_vci_address.read() + 4;
                        uint32_t channel       = (address & 0x00038000) >> 15;
                        rx_chbuf_rcmd[channel] = RX_CHBUF_RCMD_READ;
                        rx_chbuf_cont          = (size_t)((address & 0x00001000) >> 12);
                        rx_chbuf_word          = (size_t)((address & 0x00000FFF) >> 2);
                        r_vci_address          = address;
                        r_vci_nwords           = r_vci_nwords.read() - 1;
                    }
                    else
                    {
                        r_vci_fsm = VCI_IDLE;
                    }
                }
                else   // vci_param::B == 8 )
                {
                    if ( r_vci_nwords.read() > 2 ) // not the last flit
                    {
                        uint32_t address       = r_vci_address.read() + 8;
                        uint32_t channel       = (address & 0x00038000) >> 15;
                        rx_chbuf_rcmd[channel] = RX_CHBUF_RCMD_READ;
                        rx_chbuf_cont          = (size_t)((address & 0x00001000) >> 12);
                        rx_chbuf_word          = (size_t)((address & 0x00000FFF) >> 2);
                        r_vci_address          = address;
                        r_vci_nwords           = r_vci_nwords.read() - 2;
                    }
                    else
                    {
                        r_vci_fsm = VCI_IDLE;
                    }
                }
            }
            break;
        }
        /////////////////////////
        case VCI_WRITE_HYPER_REG:
        {
            if ( p_vci.rspack.read() )
            {
                uint32_t address = r_vci_address.read();
                uint32_t word    = (address & 0x00000FFF) >> 2;
                uint32_t wdata;
                if      ( vci_param::B == 4 )
                {
                    wdata = r_vci_wdata.read();
                }
                else // vci_param::B == 8
                {
                    if ( r_vci_be.read() == 0x0F )  wdata = (uint32_t)r_vci_wdata.read();
                    else                            wdata = (uint32_t)(r_vci_wdata.read()>>32);
                }

                if ( (word < (NIC_G_MAC_4 + 8) ) and (word >= NIC_G_MAC_4 ) )
                {
                    uint32_t channel = word - NIC_G_MAC_4;
                    r_channel_mac_4[channel] = wdata;
                }
                else if ( (word < (NIC_G_MAC_2 + 8) ) and (word >= NIC_G_MAC_2 ) )
                {
                    uint32_t channel = word - NIC_G_MAC_2;
                    r_channel_mac_2[channel] = wdata;
                }
                else
                {
                    switch(word)
                    {
                        case NIC_G_TDM_ENABLE :
                            r_global_tdm_enable = wdata;
                            break;
                        case NIC_G_TDM_PERIOD :
                            r_global_tdm_period = wdata;
                            break;
                        case NIC_G_BC_ENABLE :
                            r_global_bc_enable = wdata;
                            break;
                        case NIC_G_ON :
                            r_global_nic_on = wdata;
                            break;
                        case NIC_G_VIS :
                            r_global_active_channels = wdata;
                            break;
                        case NIC_G_BYPASS_ENABLE :
                            r_global_bypass_enable = wdata;
                            break;

                        case NIC_G_NPKT_RX_G2S_RECEIVED :
                            r_rx_g2s_npkt_received = wdata;
                            break;
                        case NIC_G_NPKT_RX_G2S_DISCARDED :
                            r_rx_g2s_npkt_discarded = wdata;
                            break;

                        case NIC_G_NPKT_RX_DES_SUCCESS :
                            r_rx_des_npkt_success = wdata;
                            break;
                        case NIC_G_NPKT_RX_DES_TOO_SMALL :
                            r_rx_des_npkt_too_small = wdata;
                            break;
                        case NIC_G_NPKT_RX_DES_TOO_BIG :
                            r_rx_des_npkt_too_big = wdata;
                            break;
                        case NIC_G_NPKT_RX_DES_MFIFO_FULL :
                            r_rx_des_npkt_mfifo_full = wdata;
                            break;
                        case NIC_G_NPKT_RX_DES_CRC_FAIL :
                            r_rx_des_npkt_cs_fail = wdata;
                            break;

                        case NIC_G_NPKT_RX_DISPATCH_RECEIVED :
                            r_rx_dispatch_npkt_received = wdata;
                            break;
                        case NIC_G_NPKT_RX_DISPATCH_BROADCAST :
                            r_rx_dispatch_npkt_broadcast = wdata;
                            break;
                        case NIC_G_NPKT_RX_DISPATCH_DST_FAIL :
                            r_rx_dispatch_npkt_dst_fail = wdata;
                            break;
                        case NIC_G_NPKT_RX_DISPATCH_CH_FULL :
                            r_rx_dispatch_npkt_channel_full = wdata;
                            break;

                        case NIC_G_NPKT_TX_DISPATCH_RECEIVED :
                            r_tx_dispatch_npkt_received = wdata;
                            break;
                        case NIC_G_NPKT_TX_DISPATCH_TOO_SMALL :
                            r_tx_dispatch_npkt_too_small = wdata;
                            break;
                        case NIC_G_NPKT_TX_DISPATCH_TOO_BIG :
                            r_tx_dispatch_npkt_too_big = wdata;
                            break;
                        case NIC_G_NPKT_TX_DISPATCH_SRC_FAIL :
                            r_tx_dispatch_npkt_src_fail = wdata;
                            break;
                        case NIC_G_NPKT_TX_DISPATCH_BROADCAST :
                            r_tx_dispatch_npkt_broadcast = wdata;
                            break;
                        case NIC_G_NPKT_TX_DISPATCH_BYPASS :
                            r_tx_dispatch_npkt_bypass = wdata;
                            break;
                        case NIC_G_NPKT_TX_DISPATCH_TRANSMIT :
                            r_tx_dispatch_npkt_transmit = wdata;
                            break;

                        default:
                            std::cout << "ERROR in VCI_MULTI_NIC : illegal hyper register write"
                                      << " : address = " << std::hex << address << std::endl;
                            exit(0);
                    }
                }
                r_vci_fsm = VCI_IDLE;
            }
            break;
        }
        ///////////////////////////
        case VCI_WRITE_CHANNEL_REG:
        {
            if ( p_vci.rspack.read() )
            {
                uint32_t address = r_vci_address.read();
                uint32_t word    = (address & 0x00000FFF) >> 2;
                uint32_t channel = (address & 0x00038000) >> 15;
                uint32_t wdata;

                if      ( vci_param::B == 4 )
                {
                    wdata = r_vci_wdata.read();
                }
                else // vci_param::B == 8
                {
                    if ( r_vci_be.read() == 0x0F )  wdata = (uint32_t)r_vci_wdata.read();
                    else                            wdata = (uint32_t)(r_vci_wdata.read()>>32);
                }

                switch(word)
                {

                    case NIC_RX_STS_0:   // release container RX 0 if it has been emptied
                        if( (wdata == 0) and r_channel_rx_run[channel].read())
                        {
                            rx_chbuf_rcmd[channel] = RX_CHBUF_RCMD_RELEASE;
                            rx_chbuf_cont          = 0;
                        }
                        break;
                    case NIC_RX_STS_1:  // release container RX 1 if it has been emptied
                        if( (wdata == 0) and r_channel_rx_run[channel].read())
                        {
                            rx_chbuf_rcmd[channel] = RX_CHBUF_RCMD_RELEASE;
                            rx_chbuf_cont          = 1;
                        }
                        break;
                    case NIC_TX_STS_0:  // release container TX 0 if it is full
                        if( wdata == 0x1 )
                        {
                            tx_chbuf_wcmd[channel] = TX_CHBUF_WCMD_RELEASE;
                            tx_chbuf_cont          = 0;
                        }
                        break;
                    case NIC_TX_STS_1:  // release container TX 1 if it is full
                        if( wdata == 0x1 )
                        {
                            tx_chbuf_wcmd[channel] = TX_CHBUF_WCMD_RELEASE;
                            tx_chbuf_cont          = 1;
                        }
                        break;
                    case NIC_RX_DESC_LO_0:   // set LSB of RX[channel][0] descriptor
                        r_channel_rx_desc_0[channel] = 
                           ((r_channel_rx_desc_0[channel].read() >> 32) << 32)
                           + (uint64_t)wdata;
                        break;
                    case NIC_RX_DESC_LO_1:    // set LSB of RX[channel][1] descriptor
                        r_channel_rx_desc_1[channel] = 
                           ((r_channel_rx_desc_1[channel].read() >> 32) << 32)
                           + (uint64_t)wdata;
                        break;
                    case NIC_TX_DESC_LO_0:    // set LSB of TX[channel][0] descriptor
                        r_channel_tx_desc_0[channel] = 
                           ((r_channel_tx_desc_0[channel].read() >> 32) << 32)
                           + (uint64_t)wdata;
                        break;
                    case NIC_TX_DESC_LO_1:    // set LSB of TX[channel][1] descriptor
                        r_channel_tx_desc_1[channel] = 
                           ((r_channel_tx_desc_1[channel].read() >> 32) << 32)
                           + (uint64_t)wdata;
                        break;
                    case NIC_RX_DESC_HI_0:    // set MSB of RX[channel][0] descriptor
                        r_channel_rx_desc_0[channel] = 
                           (r_channel_rx_desc_0[channel].read() & 0xFFFFFFFF) 
                           + ((uint64_t)wdata << 32);
                        break;
                    case NIC_RX_DESC_HI_1:    // set MSB of RX[channel][1] descriptor
                        r_channel_rx_desc_1[channel] = 
                           (r_channel_rx_desc_1[channel].read() & 0xFFFFFFFF) 
                           + ((uint64_t)wdata << 32);
                        break;
                    case NIC_TX_DESC_HI_0:    // set MSB of TX[channel][0] descriptor
                        r_channel_tx_desc_0[channel] = 
                           (r_channel_tx_desc_0[channel].read() & 0xFFFFFFFF)
                           + ((uint64_t)wdata << 32);
                        break;
                    case NIC_TX_DESC_HI_1:    // set MSB of TX[channel][1] descriptor
                        r_channel_tx_desc_1[channel] = 
                           (r_channel_tx_desc_1[channel].read() & 0xFFFFFFFF)
                           + ((uint64_t)wdata << 32);
                        break;
                    case NIC_RX_RUN:       // activate/desactivate RX[channel]
                        r_channel_rx_run[channel] = wdata;
                        break;
                    case NIC_TX_RUN:       // activate/desactivate TX[channel]
                        r_channel_tx_run[channel] = wdata;
                        break;
                    default:
                        std::cout << "ERROR in VCI_MULTI_NIC : illegal channel register write"
                                  << " : address = " << std::hex << address << std::endl;
                        exit(0);
                }
                r_vci_fsm = VCI_IDLE;
            }
            break;
        }
        ////////////////////////
        case VCI_READ_HYPER_REG:   // send hyper REG value in VCI response
        {
            if ( p_vci.rspack.read() )
            {
                r_vci_fsm = VCI_IDLE;
            }
            break;
        }

        /////////////////////////
        case VCI_READ_CHANNEL_REG:  // send channel REG value(s) in VCI response
        {
            if ( p_vci.rspack.read() )
            {
                assert( (r_vci_nwords.read() <= 2) and
                "ERROR in VCI_MULTI_NIC : Register read access must have plen = 4 or 8");

                uint32_t address = r_vci_address.read();
                uint32_t word    = (address & 0x00000FFF) >> 2;

                if ( vci_param::B == 4 )
                {
                    if (r_vci_nwords.read() > 1 &&     // read first register of descriptor
                        ( word == NIC_RX_DESC_LO_0 ||
                          word == NIC_RX_DESC_LO_1 ||
                          word == NIC_TX_DESC_LO_0 ||
                          word == NIC_TX_DESC_LO_1 ) )
                    {
                        r_vci_address          = address + 4; // prepare to read DESC_HI values
                        r_vci_nwords           = r_vci_nwords.read() - 1;
                    }
                    else
                    {
                        r_vci_fsm = VCI_IDLE;
                    }
                }
                else  // vci_param::B == 8 )
                {
                    r_vci_fsm = VCI_IDLE;
                }
            }
            break;
        }
    } // end switch vci_fsm

    ////////////////////////////////////////////////////////////////////////////
    // Get data from the backend (PHY) component and fill the rx_g2s pipe-line.
    // (only when the NIC is running)
    ////////////////////////////////////////////////////////////////////////////

    bool    gmii_rx_dv = false;
    bool    gmii_rx_er = false;
    uint8_t gmii_rx_data;

    if ( r_global_nic_on.read() )
    {
        // get one byte from rx_backend
        r_backend_rx->get( &gmii_rx_dv,
                           &gmii_rx_er,
                           &gmii_rx_data);

        // update rx_g2s data pipe-line
        r_rx_g2s_dt0 = gmii_rx_data;
        r_rx_g2s_dt1 = r_rx_g2s_dt0.read();
        r_rx_g2s_dt2 = r_rx_g2s_dt1.read();
        r_rx_g2s_dt3 = r_rx_g2s_dt2.read();
        r_rx_g2s_dt4 = r_rx_g2s_dt3.read();
        r_rx_g2s_dt5 = r_rx_g2s_dt4.read();
    }

    ///////////////////////////////////////////////////////////////////////////
    // This RX_G2S module makes the BACKEND to STREAM format conversion.
    // It checks the checksum, and signals a possible error.
    // The input is the backend module.
    // The output is the rx_fifo_stream, but this fifo is only used for
    // clock boundary handling, and should never be full, as the consumer
    // (RX_DES module) read all available bytes at all cycles.
    ///////////////////////////////////////////////////////////////////////////

    assert( r_rx_fifo_stream.wok() and
            "ERROR in VCI_MULTI_NIC : the rs_fifo_stream should never be full");

    switch(r_rx_g2s_fsm.read())
    {
        /////////////////
        case RX_G2S_IDLE:   // waiting start of packet
        {
            if (r_global_nic_on.read() and gmii_rx_dv and not gmii_rx_er ) // start of packet
            {
                r_rx_g2s_fsm           = RX_G2S_DELAY;
                r_rx_g2s_delay         = 0;
            }
            break;
        }
        //////////////////
        case RX_G2S_DELAY:  // entering bytes in the pipe (4 cycles)
        {
            if ( not gmii_rx_dv or gmii_rx_er ) // data invalid or error
            {
                r_rx_g2s_npkt_discarded = r_rx_g2s_npkt_discarded.read() + 1;
                r_rx_g2s_fsm            = RX_G2S_IDLE;
            }
            else if ( r_rx_g2s_delay.read() == 3 )
            {
                r_rx_g2s_fsm        = RX_G2S_LOAD;
                r_rx_g2s_checksum   = 0x00000000;      // reset checksum register
                r_total_len_rx_gmii = r_total_len_rx_gmii.read() + 1;
            }
            else
            {
                r_rx_g2s_delay      = r_rx_g2s_delay.read() + 1;
                r_total_len_rx_gmii = r_total_len_rx_gmii.read() + 1;
            }
            break;
        }
        /////////////////
        case RX_G2S_LOAD:   // initialize checksum accu
        {
            if ( gmii_rx_dv and not gmii_rx_er ) // data valid / no error
            {
                // update CRC
                r_rx_g2s_checksum   = m_crc.update( r_rx_g2s_checksum.read(),
                                                    (uint32_t)r_rx_g2s_dt4.read() );

                r_rx_g2s_fsm        = RX_G2S_SOS;
                r_total_len_rx_gmii = r_total_len_rx_gmii.read() + 1;
            }
            else
            {
                r_rx_g2s_npkt_discarded = r_rx_g2s_npkt_discarded.read() + 1;
                r_rx_g2s_fsm            = RX_G2S_IDLE;
            }
            break;
        }
        ////////////////
        case RX_G2S_SOS:    // write first byte in fifo_stream: SOS
        {
            if ( gmii_rx_dv and not gmii_rx_er ) // data valid / no error
            {
                rx_fifo_stream_write = true;
                rx_fifo_stream_wdata = r_rx_g2s_dt5.read() | (STREAM_TYPE_SOS << 8);

                // update CRC
                r_rx_g2s_checksum   = m_crc.update( r_rx_g2s_checksum.read(),
                                                    (uint32_t)r_rx_g2s_dt4.read() );

                r_rx_g2s_fsm        = RX_G2S_LOOP;
                r_total_len_rx_gmii = r_total_len_rx_gmii.read() + 1;

#if RX_G2S_DEBUG
r_rx_g2s_nbytes = 1;
r_rx_g2s_pktid  = 0;
#endif
            }
            else
            {
                r_rx_g2s_npkt_discarded = r_rx_g2s_npkt_discarded.read() + 1;
                r_rx_g2s_fsm            = RX_G2S_IDLE;
            }
            break;
        }
        /////////////////
        case RX_G2S_LOOP:   // write one byte in fifo_stream : NEV
        {
            rx_fifo_stream_write = true;
            rx_fifo_stream_wdata = r_rx_g2s_dt5.read() | (STREAM_TYPE_NEV << 8);

#if RX_G2S_DEBUG
unsigned int byte = (unsigned int)r_rx_g2s_dt5.read();
if ( r_rx_g2s_nbytes.read() == 6 ) r_rx_g2s_pktid = r_rx_g2s_pktid.read() | (byte<<8);
if ( r_rx_g2s_nbytes.read() == 7 ) r_rx_g2s_pktid = r_rx_g2s_pktid.read() | (byte   );
r_rx_g2s_nbytes = r_rx_g2s_nbytes.read() + 1;
#endif
            // update CRC
            r_rx_g2s_checksum   = m_crc.update( r_rx_g2s_checksum.read(),
                                                (uint32_t)r_rx_g2s_dt4.read() );

            r_total_len_rx_gmii = r_total_len_rx_gmii.read() + 1;

            if ( not gmii_rx_dv and not gmii_rx_er ) // end of paquet
            {
                r_rx_g2s_fsm = RX_G2S_END;
            }
            else if ( gmii_rx_dv and gmii_rx_er ) // error
            {
                r_rx_g2s_fsm = RX_G2S_FAIL;
            }
            else if ( not gmii_rx_dv and gmii_rx_er ) // error extend
            {
                r_rx_g2s_fsm = RX_G2S_ERR;
            }
            break;
        }
        ////////////////
        case RX_G2S_END:    // signal end of packet: EOS or ERR depending on checksum
        {
            uint32_t check = (uint32_t)r_rx_g2s_dt4.read()       |
                             (uint32_t)r_rx_g2s_dt3.read() << 8  |
                             (uint32_t)r_rx_g2s_dt2.read() << 16 |
                             (uint32_t)r_rx_g2s_dt1.read() << 24 ;

            r_total_len_rx_gmii = r_total_len_rx_gmii.read() + 1;

            if ( r_rx_g2s_checksum.read() == check )
            {

#if RX_G2S_DEBUG
printf("<NIC RX_G2S_END> : good packet at cycle %d / length = %d / index = %d\n",
       r_total_cycles.read(), r_rx_g2s_nbytes.read()+1 , r_rx_g2s_pktid.read() );
#endif
                rx_fifo_stream_write = true;
                rx_fifo_stream_wdata = r_rx_g2s_dt5.read() | (STREAM_TYPE_EOS << 8);
            }
            else
            {
#if RX_G2S_DEBUG
printf("<NIC RX_G2S_END> : error packet at cycle %d / expected crc = %x / received crc = %x\n",
       r_total_cycles.read(), r_rx_g2s_checksum.read(), check );
#endif
                rx_fifo_stream_write = true;
                rx_fifo_stream_wdata = r_rx_g2s_dt5.read() | (STREAM_TYPE_ERR << 8);
            }

            // increment number of received packets
            r_rx_g2s_npkt_received = r_rx_g2s_npkt_received.read() + 1;

            if ( gmii_rx_dv and not gmii_rx_er ) // start of packet / no error
            {
                r_rx_g2s_fsm           = RX_G2S_DELAY;
                r_rx_g2s_delay         = 0;
            }
            else
            {
                r_rx_g2s_fsm   = RX_G2S_IDLE;
            }
            break;
        }
        ////////////////
        case RX_G2S_ERR:  // signal error: ERR
        {
            rx_fifo_stream_write = true;
            rx_fifo_stream_wdata = r_rx_g2s_dt5.read() | (STREAM_TYPE_ERR << 8);

            if ( gmii_rx_dv and not gmii_rx_er ) // start of packet / no error
            {
                r_rx_g2s_npkt_received = r_rx_g2s_npkt_received.read() + 1;
                r_rx_g2s_fsm           = RX_G2S_DELAY;
                r_rx_g2s_delay         = 0;
            }
            else
            {
                r_rx_g2s_fsm   = RX_G2S_IDLE;
            }
            break;
        }
        /////////////////
        case RX_G2S_FAIL: // waiting end of paquet to signal error
        {
            if ( not gmii_rx_dv )
            {
                r_rx_g2s_fsm   = RX_G2S_ERR;
            }
            break;
        }
    } // end switch rx_g2s_fsm

    ///////////////////////////////////////////////////////////////////////////
    // This RX_DES module is in charge of deserialisation (4 bytes -> 1 word).
    // - The input is the rx_fifo_stream, respecting the stream format:
    //   8 bits data + 2 bits type.
    // - The output is the rx_fifo_multi that can store a full paquet.
    // It is also in charge of discarding input packets in four cases:
    // - if a packet is too small (64 - 4)B
    // - if a packet is too long (1518 - 4)B
    // - if a checksum error is reported by the RS_G2S FSM
    // - if there not space in the rx_fifo_multi
    // Implementation note:
    // - The FSM try to read a byte in rx_fifo_stream at all cycles.
    // - It test if the rx_fifo_multi is full in the state where it reads
    //   the last byte of a 32 bits word.
    ///////////////////////////////////////////////////////////i////////////////

    switch (r_rx_des_fsm.read())
    {
        /////////////////
        case RX_DES_IDLE:     // try to read first byte in rx_fifo_stream
        {
            uint16_t data = r_rx_fifo_stream.read();
            uint32_t type = (data >> 8) & 0x3;

            if ( r_rx_fifo_stream.rok() )   // do nothing if we cannot read
            {
                r_rx_des_data[0] = (uint8_t)(data & 0xFF);
                r_rx_des_counter_bytes = 1;
                if ( type == STREAM_TYPE_SOS ) r_rx_des_fsm = RX_DES_READ_1;
            }
            break;
        }
        ///////////////////
        case RX_DES_READ_1:     // read second byte in rx_fifo_stream
        {
            uint16_t data = r_rx_fifo_stream.read();
            uint32_t type = (data >> 8) & 0x3;

            if ( r_rx_fifo_stream.rok() )   // do nothing if we cannot read
            {
                r_rx_des_data[1] = (uint8_t)(data & 0xFF);
                r_rx_des_counter_bytes = r_rx_des_counter_bytes.read() + 1;
                if ( type == STREAM_TYPE_NEV )
                {
                    r_rx_des_fsm = RX_DES_READ_2;
                }
                else
                {
                    r_rx_des_npkt_too_small = r_rx_des_npkt_too_small.read() + 1;
                    r_rx_des_fsm = RX_DES_IDLE;
                }
            }
            break;
        }
        ///////////////////
        case RX_DES_READ_2:     // read third byte in rx_fifo_stream
        {
            uint16_t data = r_rx_fifo_stream.read();
            uint32_t type = (data >> 8) & 0x3;

            if ( r_rx_fifo_stream.rok() )   // do nothing if we cannot read
            {
                r_rx_des_data[2] = (uint8_t)(data & 0xFF);
                r_rx_des_counter_bytes = r_rx_des_counter_bytes.read() + 1;
                if ( type == STREAM_TYPE_NEV )
                {
                    r_rx_des_fsm = RX_DES_READ_3;
                }
                else
                {
                    r_rx_des_npkt_too_small = r_rx_des_npkt_too_small.read() + 1;
                    r_rx_des_fsm = RX_DES_IDLE;
                }
            }
            break;
        }
        ///////////////////
        case RX_DES_READ_3:    // read fourth byte in rx_fifo_stream
                               // and test if rx_fifo_multi can be written
        {
            uint16_t data = r_rx_fifo_stream.read();
            uint32_t type = (data >> 8) & 0x3;

            if ( r_rx_fifo_stream.rok() )   // do nothing if we cannot read
            {
                r_rx_des_data[3]	= (uint8_t)(data & 0xFF);
                r_rx_des_counter_bytes = r_rx_des_counter_bytes.read() + 1;

                if ( (type == STREAM_TYPE_NEV) and r_rx_fifo_multi.wok() )
                {
                    r_rx_des_fsm = RX_DES_READ_WRITE_0;
                }
                else if ( type != STREAM_TYPE_NEV )
                {
                    r_rx_des_npkt_too_small = r_rx_des_npkt_too_small.read() + 1;
                    r_rx_des_fsm = RX_DES_IDLE;
                }
                else  // fifo_multi full
                {
                    r_rx_des_npkt_mfifo_full = r_rx_des_npkt_mfifo_full.read() + 1;
                    r_rx_des_fsm = RX_DES_IDLE;
                }
            }
            break;
        }
        /////////////////////////
        case RX_DES_READ_WRITE_0:   // write previous word in rx_fifo_multi
                                    // and read first byte in rx_fifo_stream
        {
            // write previous word into fifo_multi (wok has been previouly checked)
            rx_fifo_multi_wcmd  = FIFO_MULTI_WCMD_WRITE;
            rx_fifo_multi_wdata = (uint32_t)(r_rx_des_data[0].read() << 24) |
                                  (uint32_t)(r_rx_des_data[1].read() << 16) |
                                  (uint32_t)(r_rx_des_data[2].read() << 8 ) |
                                  (uint32_t)(r_rx_des_data[3].read()      ) ;

            // Read new byte
            uint16_t data = r_rx_fifo_stream.read();
            uint32_t type = (data >> 8) & 0x3;
            uint32_t counter_bytes;

            if ( r_rx_fifo_stream.rok() )     // do nothing if we cannot read
            {
                r_rx_des_data[0]	   = (uint8_t)(data & 0xFF);
                counter_bytes          = r_rx_des_counter_bytes.read() + 1;
                r_rx_des_counter_bytes = counter_bytes;
                r_rx_des_padding	   = 3;

                if ( counter_bytes > (1518-4) )
                {
                    r_rx_des_npkt_too_big = r_rx_des_npkt_too_big.read() + 1;
                    r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                }
                else if ( type == STREAM_TYPE_NEV )
                {
                    r_rx_des_fsm = RX_DES_READ_WRITE_1;
                }
                else if ( (type == STREAM_TYPE_EOS) )
                {
                    if ( not r_rx_fifo_multi.wok() )
                    {
                        r_rx_des_npkt_mfifo_full = r_rx_des_npkt_mfifo_full.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                    }
                    else if ( counter_bytes < (64-4) )
                    {
                        r_rx_des_npkt_too_small = r_rx_des_npkt_too_small.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                    }
                    else // success: EOS and WOK and not TOO_SMALL
                    {
                        r_rx_des_npkt_success = r_rx_des_npkt_success.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_LAST;
                    }
                }
                else // TYPE == ERR or SOS
                {
                    r_rx_des_npkt_cs_fail = r_rx_des_npkt_cs_fail.read() + 1;
                    r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                }
            }
            break;
        }
        /////////////////////////
       	case RX_DES_READ_WRITE_1:   // read second byte in rx_fifo_stream
        {
            uint16_t data = r_rx_fifo_stream.read();
            uint32_t type = (data >> 8) & 0x3;
            uint32_t counter_bytes;

            if ( r_rx_fifo_stream.rok() )     // do nothing if we cannot read
            {
                r_rx_des_data[1]	   = (uint8_t)(data & 0xFF);
                counter_bytes          = r_rx_des_counter_bytes.read() + 1;
                r_rx_des_counter_bytes = counter_bytes;
                r_rx_des_padding	   = 2;

                if ( counter_bytes > (1518-4) )
                {
                    r_rx_des_npkt_too_big = r_rx_des_npkt_too_big.read() + 1;
                    r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                }
                else if ( type == STREAM_TYPE_NEV )
                {
                    r_rx_des_fsm = RX_DES_READ_WRITE_2;
                }
                else if ( (type == STREAM_TYPE_EOS) )
                {
                    if ( not r_rx_fifo_multi.wok() )
                    {
                        r_rx_des_npkt_mfifo_full = r_rx_des_npkt_mfifo_full.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                    }
                    else if ( counter_bytes < (64-4) )
                    {
                        r_rx_des_npkt_too_small = r_rx_des_npkt_too_small.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                    }
                    else // success: EOS and WOK and not TOO_SMALL
                    {
                        r_rx_des_npkt_success = r_rx_des_npkt_success.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_LAST;
                    }
                }
                else // TYPE == ERR or SOS
                {
                    r_rx_des_npkt_cs_fail = r_rx_des_npkt_cs_fail.read() + 1;
                    r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                }
            }
            break;
        }
        /////////////////////////
       	case RX_DES_READ_WRITE_2:   // read third byte in rx_fifo_stream
        {
            uint16_t data = r_rx_fifo_stream.read();
            uint32_t type = (data >> 8) & 0x3;
            uint32_t counter_bytes;

            if ( r_rx_fifo_stream.rok() )     // do nothing if we cannot read
            {
                r_rx_des_data[2]	   = (uint8_t)(data & 0xFF);
                counter_bytes          = r_rx_des_counter_bytes.read() + 1;
                r_rx_des_counter_bytes = counter_bytes;
                r_rx_des_padding	   = 1;

                if ( counter_bytes > (1518-4) )
                {
                    r_rx_des_npkt_too_big = r_rx_des_npkt_too_big.read() + 1;
                    r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                }
                else if ( type == STREAM_TYPE_NEV )
                {
                    r_rx_des_fsm = RX_DES_READ_WRITE_3;
                }
                else if ( (type == STREAM_TYPE_EOS) )
                {
                    if ( not r_rx_fifo_multi.wok() )
                    {
                        r_rx_des_npkt_mfifo_full = r_rx_des_npkt_mfifo_full.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                    }
                    else if ( counter_bytes < (64-4) )
                    {
                        r_rx_des_npkt_too_small = r_rx_des_npkt_too_small.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                    }
                    else // success: EOS and WOK and not TOO_SMALL
                    {
                        r_rx_des_npkt_success = r_rx_des_npkt_success.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_LAST;
                    }
                }
                else // TYPE == ERR or SOS
                {
                    r_rx_des_npkt_cs_fail = r_rx_des_npkt_cs_fail.read() + 1;
                    r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                }
            }
            break;
        }
        /////////////////////////
        case RX_DES_READ_WRITE_3:    // read fourth byte in rx_fifo_stream
        {
            uint16_t data = r_rx_fifo_stream.read();
            uint32_t type = (data >> 8) & 0x3;
            uint32_t counter_bytes;

            if ( r_rx_fifo_stream.rok() )     // do nothing if we cannot read
            {
                r_rx_des_data[3]	   = (uint8_t)(data & 0xFF);
                counter_bytes          = r_rx_des_counter_bytes.read() + 1;
                r_rx_des_counter_bytes = counter_bytes;
                r_rx_des_padding	   = 0;

                if ( counter_bytes > (1518-4) )
                {
                    r_rx_des_npkt_too_big = r_rx_des_npkt_too_big.read() + 1;
                    r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                }
                else if ( type == STREAM_TYPE_NEV )
                {
                    r_rx_des_fsm = RX_DES_READ_WRITE_0;
                }
                else if ( (type == STREAM_TYPE_EOS) )
                {
                    if ( not r_rx_fifo_multi.wok() )
                    {
                        r_rx_des_npkt_mfifo_full = r_rx_des_npkt_mfifo_full.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                    }
                    else if ( counter_bytes < (64-4) )
                    {
                        r_rx_des_npkt_too_small = r_rx_des_npkt_too_small.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                    }
                    else // success : EOS and WOK and not TOO_SMALL
                    {
                        r_rx_des_npkt_success = r_rx_des_npkt_success.read() + 1;
                        r_rx_des_fsm = RX_DES_WRITE_LAST;
                    }
                }
                else // TYPE == ERR or SOS
                {
                    r_rx_des_npkt_cs_fail = r_rx_des_npkt_cs_fail.read() + 1;
                    r_rx_des_fsm = RX_DES_WRITE_CLEAR;
                }
            }
            break;
        }
        ///////////////////////
        case RX_DES_WRITE_LAST:     // write last word in rx_fifo_multi
                                    // depending on r_rx_des_padding
        {
            if ( r_rx_des_padding.read() == 0 )
            {
                rx_fifo_multi_wdata 	= (uint32_t)(r_rx_des_data[0].read() << 24) |
                                          (uint32_t)(r_rx_des_data[1].read() << 16) |
                                          (uint32_t)(r_rx_des_data[2].read() << 8 ) |
                                          (uint32_t)(r_rx_des_data[3].read()      ) ;
            }
            else if ( r_rx_des_padding.read() == 1 )
            {
                rx_fifo_multi_wdata 	= (uint32_t)(r_rx_des_data[0].read() << 24) |
                                          (uint32_t)(r_rx_des_data[1].read() << 16) |
                                          (uint32_t)(r_rx_des_data[2].read() << 8 ) ;
            }
            else if ( r_rx_des_padding.read() == 2 )
            {
                rx_fifo_multi_wdata 	= (uint32_t)(r_rx_des_data[0].read() << 24) |
                                          (uint32_t)(r_rx_des_data[1].read() << 16) ;
            }
            else  // padding = 3
            {
                rx_fifo_multi_wdata 	= (uint32_t)(r_rx_des_data[0].read() << 24) ;
            }

            rx_fifo_multi_wcmd    = FIFO_MULTI_WCMD_LAST;
            rx_fifo_multi_padding = r_rx_des_padding.read();
            r_rx_des_fsm = RX_DES_IDLE;
            break;
        }
        ////////////////////////
	    case RX_DES_WRITE_CLEAR:
        {
            rx_fifo_multi_wcmd  = FIFO_MULTI_WCMD_CLEAR;
            r_rx_des_fsm        = RX_DES_IDLE;
        }
    } // end swich rx_des_fsm

    ///////////////////////////////////////////////////////////////////////
    // The RX_DISPATCH FSM performs the actual transfer of
    // a packet from the rx_fifo_multi or bp_fifo_multi to one or several
    // rx_chbuf containers selected by the MAC address decoding.
    // It implements a round-robin priority between the two multi-fifos.
    // Allocation is only done when a complete packet has been
    // transfered, and the DISPATCH FSM is IDLE.
    // To select the destination(s), all channels are checked in parallel.
    // Packets with unexpected MAC address are discarded.
    // In case of a broadcast packet, the packet is transfered in parallel
    // to all writable running channels (but the sender).
    ///////////////////////////////////////////////////////////////////////

    switch( r_rx_dispatch_fsm.read() )
    {
        //////////////////////
        case RX_DISPATCH_IDLE:  // round-robin allocation
        {
            if ( r_rx_dispatch_bp.read() )  // previously allocated to bp_fifo
            {
                if ( r_rx_fifo_multi.rok() ) r_rx_dispatch_bp = false;
            }
            else                            // previously allocated to rx_fifo
            {
                if ( r_bp_fifo_multi.rok() ) r_rx_dispatch_bp = true;
            }

            if ( r_bp_fifo_multi.rok() or r_rx_fifo_multi.rok() ) // packet available
            {
                r_rx_dispatch_npkt_received = r_rx_dispatch_npkt_received.read() + 1;
                r_rx_dispatch_fsm = RX_DISPATCH_GET_PLEN;
            }
            break;
        }
        //////////////////////////
        case RX_DISPATCH_GET_PLEN: // get packet length from fifo_multi
        {
            uint32_t nbytes;
            if ( r_rx_dispatch_bp.read() ) nbytes = r_bp_fifo_multi.plen();
            else                           nbytes = r_rx_fifo_multi.plen();
            r_rx_dispatch_nbytes = nbytes;
            r_rx_dispatch_plen   = nbytes;
            r_rx_dispatch_fsm    = RX_DISPATCH_READ_FIRST;
            break;
        }
        ////////////////////////////
        case RX_DISPATCH_READ_FIRST: // read first word from fifo_multi
                                     // containing the 4 first bytes of MAC address
        {
            uint32_t word;
            if ( r_rx_dispatch_bp.read() )
            {
                bp_fifo_multi_rcmd = FIFO_MULTI_RCMD_READ;
                word               = r_bp_fifo_multi.data();
            }
            else
            {
                rx_fifo_multi_rcmd = FIFO_MULTI_RCMD_READ;
                word               = r_rx_fifo_multi.data();
            }
            r_rx_dispatch_nbytes   = r_rx_dispatch_nbytes.read() - 4;
            r_rx_dispatch_data0    = word;
            r_rx_dispatch_fsm      = RX_DISPATCH_READ_SECOND;
            break;
        }
        /////////////////////////////
        case RX_DISPATCH_READ_SECOND: // read second word from fifo_multi
                                      // containing the next 2 bytes of MAC address
        {
            uint32_t word;
            if ( r_rx_dispatch_bp.read() )
            {
                bp_fifo_multi_rcmd = FIFO_MULTI_RCMD_READ;
                word               = r_bp_fifo_multi.data();
            }
            else
            {
                rx_fifo_multi_rcmd = FIFO_MULTI_RCMD_READ;
                word               = r_rx_fifo_multi.data();
            }
            r_rx_dispatch_nbytes   = r_rx_dispatch_nbytes.read() - 4;
            r_rx_dispatch_data1    = word;
            r_rx_dispatch_fsm      = RX_DISPATCH_CHECK_BC;
            break;
        }
        //////////////////////////
        case RX_DISPATCH_CHECK_BC:    // analyse broadcast
        {
            uint32_t data0 = r_rx_dispatch_data0.read();                  // dst_mac 4 MSB bytes
            uint32_t data1 = (r_rx_dispatch_data1.read() >> 16) & 0xFFFF; // dst_mac 2 LSB bytes

            if ( (data0 == 0xFFFFFFFF) and (data1 == 0xFFFF)
                 and r_global_bc_enable.read() )          // broadcast
            {
                r_rx_dispatch_dest           = 0;
                r_rx_dispatch_fsm            = RX_DISPATCH_SELECT_BC;
                r_rx_dispatch_npkt_broadcast = r_rx_dispatch_npkt_broadcast.read() + 1;
            }
            else                                        // unicast
            {
                r_rx_dispatch_dest           = 0;
                r_rx_dispatch_fsm            = RX_DISPATCH_SELECT;
            }
            break;
        }
        ////////////////////////
        case RX_DISPATCH_SELECT:  // For all channels we test in parallel:
                                  // if the channel is running,
                                  // if the mac address matches, and if there is an open
                                  // container with enough space and time
                                  // at most one channel is selected for unicast
        {
            bool            found = false;
            uint32_t        dst_mac_2 =  (r_rx_dispatch_data0.read() & 0xFFFF0000) >> 16; 
            uint32_t        dst_mac_4 = ((r_rx_dispatch_data0.read() & 0x0000FFFF) << 16) |     
                                        ((r_rx_dispatch_data1.read() & 0xFFFF0000) >> 16) ;     

            for ( size_t k=0 ; (k<m_channels) && not found ; k++ )
            {
                bool run   = ((r_global_active_channels.read() >> k) & 0x1) && r_channel_rx_run[k];
                bool wok   = r_rx_chbuf[k]->wok();
                bool space = ( r_rx_chbuf[k]->space() > (r_rx_dispatch_nbytes.read() + 8) );
                bool time  = ( r_rx_chbuf[k]->time() > (r_rx_dispatch_nbytes.read()>>2) );
                bool mac   = ( (dst_mac_4 == r_channel_mac_4[k].read()) and
                               (dst_mac_2 == r_channel_mac_2[k].read()) );

                if ( run and mac ) // matching channel found
                {
                    found = true;
                    r_rx_dispatch_dest = 1<<k;
                    if ( not wok )  // container full => discard packet
                    {

#if RX_DISPATCH_DEBUG
printf("<NIC RX_DISPATCH_SELECT> at cycle %d : channel %d full / skip packet"
       " / plen = %d / mac_dst = %x|%x / mac_src_2 = %x\n",
       r_total_cycles.read(),
       (int)k, 
       r_rx_dispatch_nbytes.read() + 8,
       dst_mac_2, dst_mac_4,
       r_rx_dispatch_data1.read() & 0x0000FFFF );     
#endif
                        r_rx_dispatch_fsm = RX_DISPATCH_PACKET_SKIP;
                        r_rx_dispatch_npkt_channel_full = r_rx_dispatch_npkt_channel_full.read() + 1;

                    }
                    else if (space and time)    // transfer possible
                    {

#if RX_DISPATCH_DEBUG
printf("<NIC RX_DISPATCH_SELECT> at cycle %d : write packet to channel %d"
       " / plen = %d / mac_dst = %x|%x / mac_src_2 = %x\n",
       r_total_cycles.read(),
       (int)k, 
       r_rx_dispatch_nbytes.read() + 8,
       dst_mac_2, dst_mac_4,
       r_rx_dispatch_data1.read() & 0x0000FFFF );     
#endif
                        r_rx_dispatch_fsm  = RX_DISPATCH_WRITE_FIRST;
                    }
                    else    // not enough space or time => close container and retry
                    {
                        r_rx_dispatch_fsm = RX_DISPATCH_CLOSE_CONT;
                    }
                }
            } // end for channels

            if ( not found )    // no active channel found => discard packet
            {

#if RX_DISPATCH_DEBUG
printf("<NIC RX_DISPATCH_SELECT> at cycle %d : no active channel / skip packet"
       " / plen = %d / mac_dst = %x|%x / mac_src_2 = %x\n",
       r_total_cycles.read(),
       r_rx_dispatch_nbytes.read() + 8,
       dst_mac_2, dst_mac_4,
       r_rx_dispatch_data1.read() & 0x0000FFFF );     
#endif
                r_rx_dispatch_fsm = RX_DISPATCH_PACKET_SKIP;
                r_rx_dispatch_npkt_dst_fail = r_rx_dispatch_npkt_dst_fail.read() + 1;
            }
            break;
        }
        ////////////////////////////
        case RX_DISPATCH_SELECT_BC:  // For all channels, we test in parallel
                                     // if the channel is running,
                                     // if the MAC address doesn't match
                                     // and if it there is an open container
                                     // with enough space and time.
                                     // Several channels can be selected for broadcast
        {
            uint32_t    channels = 0;
            uint32_t    src_mac_2 = (r_rx_dispatch_data0.read() & 0xFFFF);  // 2 MSB bytes
            uint32_t    src_mac_4;                             // 4 LSB bytes
            if ( r_rx_dispatch_bp.read() )  src_mac_4 = r_bp_fifo_multi.data();
            else                            src_mac_4 = r_rx_fifo_multi.data();

            for ( size_t k=0 ; (k<m_channels) ; k++ )
            {
                bool run   = ((r_global_active_channels.read()>>k) & 0x1) && r_channel_rx_run[k];
                bool wok   = r_rx_chbuf[k]->wok();
                bool space = ( r_rx_chbuf[k]->space() > (r_rx_dispatch_nbytes.read() + 8) );
                bool time  = ( r_rx_chbuf[k]->time() > (r_rx_dispatch_nbytes.read()>>2) );
                bool mac   = ( (src_mac_4 == r_channel_mac_4[k].read()) and
                               (src_mac_2 == r_channel_mac_2[k].read()) );

                // the sender does not receive the packet
                if ( run and wok and space and time and not mac) // transfer possible
                {
                    channels = channels | (1<<k);
                }
            } // end for channels

            if ( channels )         // at least one channel selected
            {
                r_rx_dispatch_dest = channels;
                r_rx_dispatch_fsm  = RX_DISPATCH_WRITE_FIRST;
            }
            else                    // no channel selected => discard packet
            {
                r_rx_dispatch_fsm = RX_DISPATCH_PACKET_SKIP;
                r_rx_dispatch_npkt_channel_full = r_rx_dispatch_npkt_channel_full.read() + 1;
            }
            break;
        }
        /////////////////////////////
        case RX_DISPATCH_PACKET_SKIP:	// clear an unexpected packet in fifo_multi
        {
            if ( r_rx_dispatch_bp.read() )  bp_fifo_multi_rcmd = FIFO_MULTI_RCMD_SKIP;
            else                            rx_fifo_multi_rcmd = FIFO_MULTI_RCMD_SKIP;
            r_rx_dispatch_fsm = RX_DISPATCH_IDLE;
            break;
        }
        ////////////////////////////
        case RX_DISPATCH_CLOSE_CONT:  // Not enough space or time to write: close container
        {
            for ( size_t k = 0 ; k < m_channels ; k++ )
            {
                if ( (r_rx_dispatch_dest.read() >> k) & 0x1 )
                {
                    rx_chbuf_wcmd[k]   = RX_CHBUF_WCMD_RELEASE;
                    r_rx_dispatch_fsm = RX_DISPATCH_SELECT;
                    break;
                }
            }
            break;
        }
        /////////////////////////////
        case RX_DISPATCH_WRITE_FIRST: // write first word (data0) 
        {
            // write data0 to one or several chbufs
            for ( size_t k = 0 ; (k < m_channels); k++ )
            {
                if ( (r_rx_dispatch_dest.read() >> k) & 0x1 )
                {
                    rx_chbuf_wcmd[k] = RX_CHBUF_WCMD_WRITE;
                    rx_chbuf_wdata    = r_rx_dispatch_data0.read();
                    rx_chbuf_plen    = r_rx_dispatch_plen.read();
                }
            }

            r_total_len_rx_chan = r_total_len_rx_chan.read() + 4;
            r_rx_dispatch_fsm   = RX_DISPATCH_READ_WRITE;
            break;
        }
        ////////////////////////////
        case RX_DISPATCH_READ_WRITE: // read a new word from fifo and write previous word
                                     // to selected channel(s) for both unicast and broadcast
        {
            // write data[i-1] to one or several chbufs
            for ( size_t k = 0 ; (k < m_channels); k++ )
            {
                if ( (r_rx_dispatch_dest.read() >> k) & 0x1 )
                {
                    rx_chbuf_wcmd[k] = RX_CHBUF_WCMD_WRITE;
                    rx_chbuf_wdata    = r_rx_dispatch_data1.read();
                }
            }

            // read data[i]
            if ( r_rx_dispatch_bp.read() ) // read from bp_fifo
            {
                r_rx_dispatch_data1   = r_bp_fifo_multi.data();
                if (r_rx_dispatch_nbytes.read() <= 4) bp_fifo_multi_rcmd = FIFO_MULTI_RCMD_LAST;
                else                                  bp_fifo_multi_rcmd = FIFO_MULTI_RCMD_READ;
            }
            else                          // read from rx_fifo
            {
                r_rx_dispatch_data1   = r_rx_fifo_multi.data();
                if (r_rx_dispatch_nbytes.read() <= 4) rx_fifo_multi_rcmd = FIFO_MULTI_RCMD_LAST;
                else                                  rx_fifo_multi_rcmd = FIFO_MULTI_RCMD_READ;
            }

            r_total_len_rx_chan = r_total_len_rx_chan.read() + 4;

            if (r_rx_dispatch_nbytes.read() <= 4)   // last word
            {
                r_rx_dispatch_fsm = RX_DISPATCH_WRITE_LAST;
            }
            else                                    // not the last word
            {
                r_rx_dispatch_nbytes = r_rx_dispatch_nbytes.read() - 4;
            }
            break;
        }
        ////////////////////////////
        case RX_DISPATCH_WRITE_LAST:  // write last word to selected channel(s)
                                      // for both unicast and broadcast
        {
            for ( size_t k = 0 ; (k < m_channels); k++ )
            {
                if ( ( r_rx_dispatch_dest.read() >> k) & 0x1 )
                {
                    rx_chbuf_wcmd[k] = RX_CHBUF_WCMD_LAST;
                    rx_chbuf_wdata   = r_rx_dispatch_data1.read();
                }
            }

            r_total_len_rx_chan = r_total_len_rx_chan.read() + r_rx_dispatch_nbytes.read();

            r_rx_dispatch_fsm   = RX_DISPATCH_IDLE;
            break;
        }
    } // end switch r_rx_dispatch_fsm

    /////////////////////////////////////////////////////////////////////
    // The TX_DISPATCH FSM performs the  transfer of a packet
    // from a tx_chbuf[k] container to the tx_fifo or bp_fifo,
    // depending on the MAC address.
    // Both the bp_fifo and the tx_fifo are multi-buffer fifos (4 Kbytes).
    // A new allocation is only done when a complete container has been
    // transmitted, and the TX_DISPATCH FSM is IDLE.
    // Two scheduling policy between the output channels are supported,
    // depending on the r_global_tx_tdm flip-flop:
    // - Time dependant multiplexing (r_global_tx_tdm true)
    // - Round-Robin (r_global_tx_tdm false)
    /////////////////////////////////////////////////////////////////////

    // update the selected channel if TDM activated
    if ( r_global_tdm_enable.read() )
    {
        r_global_tdm_timer = r_global_tdm_timer.read() - 1;
        if( r_global_tdm_timer.read() == 0 )
        {
            r_global_tdm_channel = (r_global_tdm_channel.read()+1) % m_channels;
            r_global_tdm_timer   = r_global_tdm_period.read();
        }
    }

    switch( r_tx_dispatch_fsm.read() )
    {
        //////////////////////
        case TX_DISPATCH_IDLE:  // ready to start a new container transfer
                                // channel allocation is done for a full container
        {
            if ( r_global_tdm_enable.read() ) // TDM scheduler
            {
                uint32_t channel = r_global_tdm_channel.read();
                if( r_tx_chbuf[channel]->rok() and
                    r_global_nic_on.read() and
                    ((r_global_active_channels.read()>>channel)&0x1) and
                    r_channel_tx_run[channel] )
                {
                    r_tx_dispatch_channel = channel;
                    r_tx_dispatch_fsm     = TX_DISPATCH_GET_NPKT;
                }
            }
            else                             // RR scheduler
            {
                for ( size_t x = 0 ; x < m_channels ; x++ )
                {
                    uint32_t channel = (x + 1 + r_tx_dispatch_channel.read()) % m_channels;
                    if ( r_tx_chbuf[channel]->rok() and
                         r_global_nic_on.read() and
                         ((r_global_active_channels.read()>>channel)&0x1) and
                         r_channel_tx_run[channel] )
                    {
                        r_tx_dispatch_channel = channel;
                        r_tx_dispatch_fsm     = TX_DISPATCH_GET_NPKT;
                        break;
                    }
                }
            }
            break;
        }
        //////////////////////////
        case TX_DISPATCH_GET_NPKT: // get packet number from tx_chbuf
        {
            uint32_t    channel   = r_tx_dispatch_channel.read();
            uint32_t    npkt      = r_tx_chbuf[channel]->npkt();
            r_tx_dispatch_packets = npkt;

            if ((npkt == 0) or (npkt > 66))  r_tx_dispatch_fsm = TX_DISPATCH_RELEASE_CONT; 
            else                             r_tx_dispatch_fsm = TX_DISPATCH_GET_PLEN;
            break;
        }
        //////////////////////////
        case TX_DISPATCH_GET_PLEN: // get packet length from tx_chbuf
        {
            uint32_t channel    = r_tx_dispatch_channel.read();
            uint32_t plen       = r_tx_chbuf[channel]->plen();

            r_tx_dispatch_bytes = plen & 0x3;
            if ( (plen & 0x3) == 0 ) r_tx_dispatch_words = plen >> 2;
            else                     r_tx_dispatch_words = (plen >> 2) + 1;

            r_tx_dispatch_npkt_received = r_tx_dispatch_npkt_received.read()+1;
            if (plen < 60 ) // pkt too small
            {

#if TX_DISPATCH_DEBUG
printf("<NIC TX_DISPATCH_GET_PLEN> at cycle %d : packet too small for channel %d / length = %d\n",
       r_total_cycles.read(), channel , plen );
#endif
                r_tx_dispatch_fsm            = TX_DISPATCH_SKIP_PKT;
                r_tx_dispatch_npkt_too_small = r_tx_dispatch_npkt_too_small.read() + 1;
            }
            else if (plen > 1514) // pkt too long
            {

#if TX_DISPATCH_DEBUG
printf("<NIC TX_DISPATCH_GET_PLEN> at cycle %d : packet too long for channel %d / length = %d\n",
       r_total_cycles.read(), channel , plen );
#endif
                r_tx_dispatch_fsm            = TX_DISPATCH_SKIP_PKT;
                r_tx_dispatch_npkt_too_big   = r_tx_dispatch_npkt_too_big.read() + 1;
            }
            else
            {

#if TX_DISPATCH_DEBUG
printf("<NIC TX_DISPATCH_GET_PLEN> at cycle %d : get packet for channel %d / length = %d\n",
       r_total_cycles.read(), channel , plen );
#endif
                r_tx_dispatch_fsm  = TX_DISPATCH_READ_FIRST;
            }
            break;
        }
        //////////////////////////
        case TX_DISPATCH_SKIP_PKT:  // discard too small or too large packets
        {
            uint32_t  channel = r_tx_dispatch_channel.read();
            uint32_t  packets = r_tx_dispatch_packets.read();

            tx_chbuf_rcmd[channel] = TX_CHBUF_RCMD_SKIP;
            r_tx_dispatch_packets  = packets - 1;

            if(packets == 1)
            {
                r_tx_dispatch_fsm = TX_DISPATCH_RELEASE_CONT;
            }
            else
            {
                r_tx_dispatch_fsm = TX_DISPATCH_GET_PLEN;
            }
            break;
        }
        ////////////////////////////
        case TX_DISPATCH_READ_FIRST: // read first word from tx_chbuf
        {
            uint32_t  channel = r_tx_dispatch_channel.read();

            r_tx_dispatch_data0    = r_tx_chbuf[channel]->data();
            tx_chbuf_rcmd[channel] = TX_CHBUF_RCMD_READ;
            r_tx_dispatch_words    = r_tx_dispatch_words.read() - 1;
            r_tx_dispatch_fsm      = TX_DISPATCH_READ_SECOND;
            break;
        }
        /////////////////////////////
        case TX_DISPATCH_READ_SECOND: // read second word from tx_chbuf
        {
            uint32_t  channel = r_tx_dispatch_channel.read();

            r_tx_dispatch_data1    = r_tx_chbuf[channel]->data();
            tx_chbuf_rcmd[channel] = TX_CHBUF_RCMD_READ;
            r_tx_dispatch_words    = r_tx_dispatch_words.read() - 1;
            r_tx_dispatch_fsm      = TX_DISPATCH_FIFO_SELECT;
            break;
        }
        /////////////////////////////
        case TX_DISPATCH_FIFO_SELECT: // check SRC & DST MAC addresses
                                      // we read the third data word in tx_chbuf
                                      // without modifying the channel state
        {
            uint32_t channel       = r_tx_dispatch_channel.read();

            uint32_t data0         = r_tx_dispatch_data0.read();
            uint32_t data1         = r_tx_dispatch_data1.read();
            uint32_t data2         = r_tx_chbuf[channel]->data();

            uint32_t mac_dst_2 = ((data0 & 0xFFFF0000) >> 16);
            uint32_t mac_dst_4 = ((data0 & 0x0000FFFF) << 16) |
                                 ((data1 & 0xFFFF0000) >> 16) ;
            uint32_t mac_src_2 =  (data1 & 0x0000FFFF);
            uint32_t mac_src_4 =   data2;

            // check source mac address
            if ( not (mac_src_4 == r_channel_mac_4[channel]) or
                 not (mac_src_2 == r_channel_mac_2[channel]) )
            {

#if TX_DISPATCH_DEBUG
printf("<NIC TX_DISPATCH_FIFO_SELECT> Illegal MAC_SRC for channel %d"
       " / mac_dst = %x|%x / mac_src %x|%x\n",
       channel, mac_dst_2, mac_dst_4, mac_src_2, mac_src_4);
#endif
                r_tx_dispatch_fsm = TX_DISPATCH_SKIP_PKT;
                r_tx_dispatch_npkt_src_fail = r_tx_dispatch_npkt_src_fail.read() + 1;
                break;
            }

            if( (mac_dst_4 == 0xFFFFFFFF) and (mac_dst_2 == 0x0000FFFF) ) // broadcast
            {

#if TX_DISPATCH_DEBUG
printf("<NIC TX_DISPATCH_FIFO_SELECT> Broadcast packet for channel %d"
       " / mac_dst = %x|%x / mac_src %x|%x\n", 
       channel, mac_dst_2, mac_dst_4, mac_src_2, mac_src_4);
#endif
                r_tx_dispatch_npkt_broadcast  = r_tx_dispatch_npkt_broadcast.read() + 1;
                r_tx_dispatch_write_bp = true;
                r_tx_dispatch_write_tx = true;
                r_tx_dispatch_fsm      = TX_DISPATCH_WRITE_FIRST;
            }
            else                                                           // non broadcast
            {
                // compute bypass condition
                bool     bypass_found   = false;
                size_t   k;
                for ( k = 0 ; (k < m_channels) and (not bypass_found) ; k++ )
                {
                    if ( (mac_dst_4 == r_channel_mac_4[k]) and
                         (mac_dst_2 == r_channel_mac_2[k]) )
                    {
                        bypass_found   = true;
                    }
                }

                // dispatch 
                if ( bypass_found  and r_global_bypass_enable.read() )   // BYPASS => to BP fifo
                {

#if TX_DISPATCH_DEBUG
printf("<NIC TX_DISPATCH_FIFO_SELECT> Bypass packet from channel %d to channel %d"
       " / mac_dst = %x|%x / mac_src %x|%x\n", 
       channel , k , mac_dst_2 , mac_dst_4 , mac_src_2 , mac_src_4 );
#endif
                    r_tx_dispatch_npkt_bypass = r_tx_dispatch_npkt_bypass.read() + 1;
                    r_tx_dispatch_write_tx    = false;
                    r_tx_dispatch_write_bp    = true;
                    r_tx_dispatch_fsm         = TX_DISPATCH_WRITE_FIRST;
                }
                else                   // No BYPASS => to TX fifo
                {

#if TX_DISPATCH_DEBUG
printf("<NIC TX_DISPATCH_FIFO_SELECT> Send packet for channel %d"
       " / mac_dst = %x|%x / mac_src %x|%x\n", 
       channel, mac_dst_2, mac_dst_4, mac_src_2, mac_src_4);
#endif
                    r_tx_dispatch_npkt_transmit = r_tx_dispatch_npkt_transmit.read() + 1;
                    r_tx_dispatch_write_tx      = true;
                    r_tx_dispatch_write_bp      = false;
                    r_tx_dispatch_fsm           = TX_DISPATCH_WRITE_FIRST;
                }
            }
            break;
        }
        /////////////////////////////
        case TX_DISPATCH_WRITE_FIRST: // write first word (data0) in multi_fifos
        {
            if ( (r_bp_fifo_multi.wok() or ( not r_tx_dispatch_write_bp.read()) ) and
                 (r_tx_fifo_multi.wok() or ( not r_tx_dispatch_write_tx.read()) ) )
            {
                if ( r_tx_dispatch_write_bp.read() )
                {
                    bp_fifo_multi_wcmd = FIFO_MULTI_WCMD_WRITE;
                    bp_fifo_multi_wdata = r_tx_dispatch_data0.read();
                }
                if ( r_tx_dispatch_write_tx.read() )
                {
                    tx_fifo_multi_wcmd = FIFO_MULTI_WCMD_WRITE;
                    tx_fifo_multi_wdata = r_tx_dispatch_data0.read();
                }
                r_tx_dispatch_fsm = TX_DISPATCH_READ_WRITE;
                }
            break;
        }
        ////////////////////////////
        case TX_DISPATCH_READ_WRITE: // write previous word (data1) in multi_fifos
                                     // and read a new word from selected chbuf
        {
            uint32_t  channel = r_tx_dispatch_channel.read();

            if ( (r_bp_fifo_multi.wok() or ( not r_tx_dispatch_write_bp.read()) ) and
                 (r_tx_fifo_multi.wok() or ( not r_tx_dispatch_write_tx.read()) ) )
            {
                // write word (i-1)
                if ( r_tx_dispatch_write_bp.read() )
                {
                    bp_fifo_multi_wcmd = FIFO_MULTI_WCMD_WRITE;
                    bp_fifo_multi_wdata = r_tx_dispatch_data1.read();
                }
                if ( r_tx_dispatch_write_tx.read() )
                {
                    tx_fifo_multi_wcmd = FIFO_MULTI_WCMD_WRITE;
                    tx_fifo_multi_wdata = r_tx_dispatch_data1.read();
                }

                // read word (i)
                if ( r_tx_dispatch_words.read() == 1 )  // last word in packet
                {
                    tx_chbuf_rcmd[channel] = TX_CHBUF_RCMD_LAST;
                    r_tx_dispatch_data1    = r_tx_chbuf[channel]->data();
                    r_tx_dispatch_fsm      = TX_DISPATCH_WRITE_LAST;
                }
                else                                   // not the last word
                {
                    tx_chbuf_rcmd[channel] = TX_CHBUF_RCMD_READ;
                    r_tx_dispatch_data1    = r_tx_chbuf[channel]->data();
                    r_tx_dispatch_words    = r_tx_dispatch_words.read() - 1;
                }
            }
            break;
        }
        ////////////////////////////
        case TX_DISPATCH_WRITE_LAST:  // write last word (data1) in multi_fifos
        {
            uint32_t  bytes        = r_tx_dispatch_bytes.read();

            if ( (r_bp_fifo_multi.wok() or ( not r_tx_dispatch_write_bp.read()) ) and
                 (r_tx_fifo_multi.wok() or ( not r_tx_dispatch_write_tx.read()) ) )
            {
                if ( r_tx_dispatch_write_bp.read() )
                {
                    bp_fifo_multi_wcmd  = FIFO_MULTI_WCMD_LAST;
                    bp_fifo_multi_wdata = r_tx_dispatch_data1.read();
                    if ( bytes == 0 ) bp_fifo_multi_padding = 0;
                    else              bp_fifo_multi_padding = 4 - bytes;
                }
                if ( r_tx_dispatch_write_tx.read() )
                {
                    tx_fifo_multi_wcmd  = FIFO_MULTI_WCMD_LAST;
                    tx_fifo_multi_wdata = r_tx_dispatch_data1.read();
                    if ( bytes == 0 ) tx_fifo_multi_padding = 0;
                    else              tx_fifo_multi_padding = 4 - bytes;
                }
                if ( r_tx_dispatch_packets.read() == 1 )  // last packet in container
                {
                    r_tx_dispatch_fsm = TX_DISPATCH_RELEASE_CONT;
                }
                else                              // not the last packet
                {
                    r_tx_dispatch_packets = r_tx_dispatch_packets.read() - 1;
                    r_tx_dispatch_fsm     = TX_DISPATCH_GET_PLEN;
                }
            }
            break;
        }
        //////////////////////////////
        case TX_DISPATCH_RELEASE_CONT: // release the container in tx_chbuf
        {
            uint32_t channel       = r_tx_dispatch_channel.read();
            tx_chbuf_rcmd[channel] = TX_CHBUF_RCMD_RELEASE;
            r_tx_dispatch_fsm      = TX_DISPATCH_IDLE;
            break;
        }
    } // end switch tx_dispatch_fsm

    ////////////////////////////////////////////////////////////////////////////
    // This TX_SER FSM performs the serialisation (1 word => 4 bytes),
    // The input is the tx_fifo_multi.
    // The output is the tx_fifo_stream.
    ////////////////////////////////////////////////////////////////////////////

    switch(r_tx_ser_fsm.read())
    {
        /////////////////
        case TX_SER_IDLE:   // get packet length (bytes) if fifo_multi not empty
        {
            if ( r_tx_fifo_multi.rok() )
            {
                uint32_t plen = r_tx_fifo_multi.plen();

                r_tx_ser_bytes = plen & 0x3;
                if ( (plen & 0x3) == 0 ) r_tx_ser_words = plen>>2;
                else                     r_tx_ser_words = (plen>>2) + 1;
                r_tx_ser_fsm = TX_SER_READ_FIRST;
            }
            break;
        }
        ///////////////////////
        case TX_SER_READ_FIRST: // read first word
        {
            tx_fifo_multi_rcmd = FIFO_MULTI_RCMD_READ;
            r_tx_ser_words     = r_tx_ser_words.read() - 1;
            r_tx_ser_data      = r_tx_fifo_multi.data();
            r_tx_ser_first     = true;
            r_tx_ser_fsm       = TX_SER_WRITE_B0;
            break;
        }
        /////////////////////
        case TX_SER_WRITE_B0: // write first byte (MSB) from current word
        {
            if ( r_tx_fifo_stream.wok() )
            {
                uint32_t words = r_tx_ser_words.read();
                uint32_t bytes = r_tx_ser_bytes.read();
                bool     first = r_tx_ser_first.read();

                if ( first )                              // first byte in packet
                {
                    tx_fifo_stream_write = true;
                    tx_fifo_stream_wdata = (uint16_t)((r_tx_ser_data.read() & 0xFF000000)>>24
                                                                  | (STREAM_TYPE_SOS << 8));
                    r_tx_ser_fsm = TX_SER_WRITE_B1;
                }
                else if ( (words == 0) and (bytes == 1) ) // last byte in packet
                {
                    tx_fifo_stream_write = true;
                    tx_fifo_stream_wdata = (uint16_t)((r_tx_ser_data.read() & 0xFF000000)>>24
                                                                  | (STREAM_TYPE_EOS << 8));
                    r_tx_ser_fsm = TX_SER_GAP;
                }
                else                                     // simple byte
                {
                    tx_fifo_stream_write = true;
                    tx_fifo_stream_wdata = (uint16_t)((r_tx_ser_data.read() & 0xFF000000)>>24
                                                                  | (STREAM_TYPE_NEV << 8));
                    r_tx_ser_fsm = TX_SER_WRITE_B1;
                }
            }
            break;
        }
        /////////////////////
        case TX_SER_WRITE_B1:  // write second byte (MSB->LSB) from current word
        {
            if ( r_tx_fifo_stream.wok() )
            {
                uint32_t words = r_tx_ser_words.read();
                uint32_t bytes = r_tx_ser_bytes.read();

                if ( (words == 0) and (bytes == 2) ) // last byte in packet
                {
                    tx_fifo_stream_write = true;
                    tx_fifo_stream_wdata = (uint16_t)((r_tx_ser_data.read()&0x00FF0000)>>16
                                                                  | (STREAM_TYPE_EOS << 8));
                    r_tx_ser_fsm = TX_SER_GAP;
                }
                else                                 // simple byte
                {
                    tx_fifo_stream_write = true;
                    tx_fifo_stream_wdata = (uint16_t)((r_tx_ser_data.read()&0x00FF0000)>>16
                                                                  | (STREAM_TYPE_NEV << 8));
                    r_tx_ser_fsm = TX_SER_WRITE_B2;
                }
            }
            break;
        }
        ////////////////////
        case TX_SER_WRITE_B2:  // write third byte (MSB->LSB) from current word
        {
            if ( r_tx_fifo_stream.wok() )
            {
                uint32_t words = r_tx_ser_words.read();
                uint32_t bytes = r_tx_ser_bytes.read();

                if ( (words == 0) and (bytes == 3) ) // last byte in packet
                {
                    tx_fifo_stream_write = true;
                    tx_fifo_stream_wdata = (uint16_t)((r_tx_ser_data.read()&0x0000FF00)>>8
                                                                  | (STREAM_TYPE_EOS << 8));
                    r_tx_ser_fsm = TX_SER_GAP;
                }
                else                                     // simple byte
                {
                    tx_fifo_stream_write = true;
                    tx_fifo_stream_wdata = (uint16_t)((r_tx_ser_data.read()&0x0000FF00)>>8
                                                                  | (STREAM_TYPE_NEV << 8));
                    r_tx_ser_fsm = TX_SER_READ_WRITE;
                }
            }
            break;
        }
        ///////////////////////
        case TX_SER_READ_WRITE:  // write fourth byte from current word
                                 // and read next word in tx_fifo_multi
                                 // if packet not completed
        {
            if ( r_tx_fifo_stream.wok() )
            {
                uint32_t words = r_tx_ser_words.read();

                if ( words == 0 )  // last byte in packet
                {
                    tx_fifo_stream_write = true;
                    tx_fifo_stream_wdata = (uint16_t)((r_tx_ser_data.read()&0x000000FF)
                                                                  | (STREAM_TYPE_EOS << 8));
                    r_tx_ser_fsm = TX_SER_GAP;
                }
                else                                     // simple byte
                {
                    // read next word from fifo_multi
                    if ( words == 1 ) tx_fifo_multi_rcmd = FIFO_MULTI_RCMD_LAST;
                    else              tx_fifo_multi_rcmd = FIFO_MULTI_RCMD_READ;
                    r_tx_ser_words     = words - 1;
                    r_tx_ser_data      = r_tx_fifo_multi.data();
                    r_tx_ser_first     = false;
                    r_tx_ser_fsm       = TX_SER_WRITE_B0;
                    tx_fifo_stream_write = true;
                    tx_fifo_stream_wdata = (uint16_t)((r_tx_ser_data.read()&0x000000FF)
                                                                  | (STREAM_TYPE_NEV << 8));
                }
            }
            break;
        }
        ////////////////
        case TX_SER_GAP: // Inter Frame Gap delay
        {
            r_tx_ser_ifg = r_tx_ser_ifg.read() - 1;
            if (r_tx_ser_ifg.read() == 1)
            {
                r_tx_ser_ifg = m_inter_frame_gap;
                r_tx_ser_fsm = TX_SER_IDLE;
            }
            break;
        }
    } // end switch r_tx_ser_fsm

    ////////////////////////////////////////////////////////////////////////////
    // This TX_S2G FSM performs the STREAM to GMII format conversion,
    // computes the checksum, and append this checksum to the packet.
    // The input is the tx_fifo_stream.
    // The output is the r_backend_tx module.
    ////////////////////////////////////////////////////////////////////////////

    switch(r_tx_s2g_fsm.read())
    {
        /////////////////
        case TX_S2G_IDLE:           // read one byte from stream fifo
        {
            if ( r_tx_fifo_stream.rok() )
            {
                uint32_t data = r_tx_fifo_stream.read();
                uint32_t type = (data >> 8) & 0x3;

                assert ( (type == STREAM_TYPE_SOS) and
                "ERROR in VCI_MULTI_NIC : illegal type received in TX_S2G_IDLE");

                tx_fifo_stream_read = true;
                r_tx_s2g_fsm        = TX_S2G_WRITE_DATA;
                r_tx_s2g_data       = data & 0xFF;
                r_tx_s2g_checksum   = 0x00000000;      // reset checksum register
            }

            // no data written
            r_backend_tx->put( false, 0 );
            break;
        }
        //////////////////////
        case TX_S2G_WRITE_DATA:     // write data[i-1] into gmii_tx
                                    // and read data[i] from fifo_stream
        {
            if ( r_tx_fifo_stream.rok() )
            {
                // write data[i-1]
                r_backend_tx->put( true, r_tx_s2g_data.read() );

                // read data[i]
                uint32_t data = r_tx_fifo_stream.read();
                uint32_t type = (data >> 8) & 0x3;

                assert ( (type != STREAM_TYPE_SOS) and (type != STREAM_TYPE_ERR) and
                "ERROR in VCI_MULTI_NIC : illegal type received in TX_S2G_WRITE_DATA");

                tx_fifo_stream_read = true;
                r_tx_s2g_data       = data & 0xFF;

                // update CRC
                r_tx_s2g_checksum   = m_crc.update( r_tx_s2g_checksum.read(),
                                                    (uint32_t)r_tx_s2g_data.read() );

                r_total_len_tx_gmii = r_total_len_tx_gmii.read() + 1;

                if ( type == STREAM_TYPE_EOS )
                {
                    r_tx_s2g_fsm = TX_S2G_WRITE_LAST_DATA;
                }
            }
            else
            {
                assert (false and
                "ERROR in VCI_MULTI_NIC : tx_fifo should not be empty");
            }
            break;
        }
        ////////////////////////////
        case TX_S2G_WRITE_LAST_DATA:
        {
            // write last data
            r_backend_tx->put( true, r_tx_s2g_data.read() );

            // update CRC
            r_tx_s2g_checksum   = m_crc.update( r_tx_s2g_checksum.read(),
                                                (uint32_t)r_tx_s2g_data.read() );

            r_total_len_tx_gmii = r_total_len_tx_gmii.read() + 1 + m_inter_frame_gap;

            r_tx_s2g_index = 0;
            r_tx_s2g_fsm   = TX_S2G_WRITE_CS;
            break;
        }
        /////////////////////
        case TX_S2G_WRITE_CS:       // write one cs byte into gmii_out
        {
            uint8_t gmii_data;
            if ( r_tx_s2g_index.read() == 0 )
            {
                gmii_data      = r_tx_s2g_checksum.read() & 0xFF;
                r_tx_s2g_index = 1;
            }
            else if ( r_tx_s2g_index.read() == 1 )
            {
                gmii_data      = (r_tx_s2g_checksum.read() >> 8) & 0xFF;
                r_tx_s2g_index = 2;
            }
            else if ( r_tx_s2g_index.read() == 2 )
            {
                gmii_data      = (r_tx_s2g_checksum.read() >> 16) & 0xFF;
                r_tx_s2g_index = 3;
            }
            else // r_tx_s2g_index == 3
            {
                gmii_data      = (r_tx_s2g_checksum.read() >> 24) & 0xFF;
                r_tx_s2g_fsm   = TX_S2G_IDLE;
                r_tx_s2g_checksum = 0x00000000;
            }
            r_backend_tx->put( true, gmii_data );
            break;
        }
    } // end switch tx_s2g_fsm

    // update multi_fifos

    r_rx_fifo_multi.update( rx_fifo_multi_wcmd,
                            rx_fifo_multi_rcmd,
                            rx_fifo_multi_wdata,
                            rx_fifo_multi_padding );

    r_tx_fifo_multi.update( tx_fifo_multi_wcmd,
                            tx_fifo_multi_rcmd,
                            tx_fifo_multi_wdata,
                            tx_fifo_multi_padding );

    r_bp_fifo_multi.update( bp_fifo_multi_wcmd,
                            bp_fifo_multi_rcmd,
                            bp_fifo_multi_wdata,
                            bp_fifo_multi_padding );

    // update stream_fifos

    r_rx_fifo_stream.update( rx_fifo_stream_read,
                             rx_fifo_stream_write,
                             rx_fifo_stream_wdata );

    r_tx_fifo_stream.update( tx_fifo_stream_read,
                             tx_fifo_stream_write,
                             tx_fifo_stream_wdata );

    // update rx_chbuf for all channels

    for ( size_t k = 0 ; k < m_channels ; k++ )
    {
        r_rx_chbuf[k]->update( rx_chbuf_wcmd[k],
                               rx_chbuf_wdata,
                               rx_chbuf_plen,
                               rx_chbuf_rcmd[k],
                               rx_chbuf_cont,
                               rx_chbuf_word );
    }

    // update tx_chbuf for all channels
    // if VCI DATA WIDTH is 64 bits and VCI BE == 0XFF 
    // we must write two 32 bits words in tx_chbuf

    for ( size_t k = 0 ; k < m_channels ; k++ )
    {
        r_tx_chbuf[k]->update( tx_chbuf_wcmd[k],
                               tx_chbuf_wdata,
                               tx_chbuf_cont,
                               tx_chbuf_word,
                               tx_chbuf_rcmd[k] );

        if ( tx_chbuf_two_words )
        {
            r_tx_chbuf[k]->update( tx_chbuf_wcmd[k],
                                   tx_chbuf_wdata2,
                                   tx_chbuf_cont,
                                   tx_chbuf_word+1,
                                   TX_CHBUF_RCMD_NOP );
        }
    }
} // end transition

//////////////////////
tmpl(void)::genMoore()
{
    ///////////  Interrupts ////////

    for ( size_t k = 0 ; k < m_channels ; k++ )
    {
        p_rx_irq[k] =  ( r_rx_chbuf[k]->full(0) and r_rx_chbuf[k]->full(1) );
        p_tx_irq[k] =  ( r_tx_chbuf[k]->full(0) and r_tx_chbuf[k]->full(1) );
    }

    ////// VCI TARGET port ///////

    switch( r_vci_fsm.read() ) 
    {
        case VCI_IDLE:
        case VCI_WRITE_TX_BURST:
        {
            p_vci.cmdack = true;
            p_vci.rspval = false;
            p_vci.reop   = false;
            p_vci.rdata  = 0;
            break;
        }
        case VCI_READ_RX_BURST:
        {
            uint32_t channel = (r_vci_address.read() & 0x00038000) >> 15;

            p_vci.cmdack = false;
            p_vci.rspval = true;

            if ( (vci_param::B == 8) and (r_vci_nwords.read() > 1) )
                p_vci.rdata  = (typename vci_param::data_t)r_rx_chbuf[channel]->data64();
            else
                p_vci.rdata  = (typename vci_param::data_t)r_rx_chbuf[channel]->data();

            p_vci.rerror = vci_param::ERR_NORMAL;
            p_vci.rsrcid = r_vci_srcid.read();
            p_vci.rtrdid = r_vci_trdid.read();
            p_vci.rpktid = r_vci_pktid.read();
            p_vci.reop   = (r_vci_nwords == 1) or ((r_vci_nwords == 2) and (vci_param::B == 8));
            break;
        }
        case VCI_WRITE_TX_LAST:
        case VCI_WRITE_HYPER_REG:
        case VCI_WRITE_CHANNEL_REG:
        {
            p_vci.cmdack = false;
            p_vci.rspval = true;
            p_vci.rdata  = 0;
            p_vci.rerror = vci_param::ERR_NORMAL;
            p_vci.rsrcid = r_vci_srcid.read();
            p_vci.rtrdid = r_vci_trdid.read();
            p_vci.rpktid = r_vci_pktid.read();
            p_vci.reop   = true;
            break;
        }
        case VCI_READ_HYPER_REG:
        case VCI_READ_CHANNEL_REG:
        {
            if ( r_vci_fsm.read() == VCI_READ_HYPER_REG )
                p_vci.rdata  = (typename vci_param::data_t)read_hyper_register((uint32_t)r_vci_address);
            else
            {
                // if two channel reg are read in the same transaction (low word and high word of buffer
                // descritor), use successive addresses
                if ( (vci_param::B == 8) and (r_vci_nwords.read() == 2) )
                    p_vci.rdata  = (typename vci_param::data_t)
                        (read_channel_register((uint32_t)r_vci_address) & 0xFFFFFFFF) +
                        ((uint64_t)(read_channel_register((uint32_t)r_vci_address + 4) & 0xFFFFFFFF) << 32);
                else
                    p_vci.rdata  = (typename vci_param::data_t)read_channel_register((uint32_t)r_vci_address);
            }

            p_vci.cmdack = false;
            p_vci.rspval = true;
            p_vci.rerror = vci_param::ERR_NORMAL;
            p_vci.rsrcid = r_vci_srcid.read();
            p_vci.rtrdid = r_vci_trdid.read();
            p_vci.rpktid = r_vci_pktid.read();
            p_vci.reop   = (r_vci_nwords == 1) or ((r_vci_nwords == 2) and (vci_param::B == 8));
            break;
        }
        case VCI_ERROR:
        {
            p_vci.cmdack = false;
            p_vci.rspval = true;
            p_vci.rdata  = 0;
            p_vci.rerror = vci_param::ERR_GENERAL_DATA_ERROR;
            p_vci.rsrcid = r_vci_srcid.read();
            p_vci.rtrdid = r_vci_trdid.read();
            p_vci.rpktid = r_vci_pktid.read();
            p_vci.reop   = true;
            break;
        }
        default:
            assert(false);

    } // end switch vci_fsm

} // end genMore()

//////////////////////////////////////
tmpl(void)::print_trace(uint32_t mode)
{
    static const char* vci_state_str[] =
    {
            "VCI_IDLE",
            "VCI_WRITE_TX_BURST",
            "VCI_WRITE_TX_LAST",
            "VCI_WRITE_HYPER_REG",
            "VCI_WRITE_CHANNEL_REG",
            "VCI_READ_RX_BURST",
            "VCI_READ_HYPER_REG",
            "VCI_READ_CHANNEL_REG",
            "VCI_ERROR",
    };
    static const char* rx_g2s_state_str[] =
    {
            "RX_G2S_IDLE",
            "RX_G2S_DELAY",
            "RX_G2S_LOAD",
            "RX_G2S_SOS",
            "RX_G2S_LOOP",
            "RX_G2S_END",
            "RX_G2S_ERR",
            "RX_G2S_FAIL",
    };
    static const char* rx_des_state_str[] =
    {
            "RX_DES_IDLE",
            "RX_DES_READ_1",
            "RX_DES_READ_2",
            "RX_DES_READ_3",
            "RX_DES_READ_WRITE_0",
            "RX_DES_READ_WRITE_1",
            "RX_DES_READ_WRITE_2",
            "RX_DES_READ_WRITE_3",
            "RX_DES_WRITE_LAST",
            "RX_DES_WRITE_CLEAR",
    };
    static const char* rx_dispatch_state_str[] =
    {
            "RX_DISPATCH_IDLE",
            "RX_DISPATCH_GET_PLEN",
            "RX_DISPATCH_READ_FIRST",
            "RX_DISPATCH_READ_SECOND",
            "RX_DISPATCH_CHECK_BC",
            "RX_DISPATCH_SELECT",
            "RX_DISPATCH_SELECT_BC",
            "RX_DISPATCH_PACKET_SKIP",
            "RX_DISPATCH_CLOSE_CONT",
            "RX_DISPATCH_WRITE_FIRST",
            "RX_DISPATCH_READ_WRITE",
            "RX_DISPATCH_WRITE_LAST",
    };
    static const char* tx_dispatch_state_str[] =
    {
            "TX_DISPATCH_IDLE",
            "TX_DISPATCH_GET_NPKT",
            "TX_DISPATCH_GET_PLEN",
            "TX_DISPATCH_SKIP_PKT",
            "TX_DISPATCH_READ_FIRST",
            "TX_DISPATCH_READ_SECOND",
            "TX_DISPATCH_FIFO_SELECT",
            "TX_DISPATCH_WRITE_FIRST",
            "TX_DISPATCH_READ_WRITE",
            "TX_DISPATCH_WRITE_LAST",
            "TX_DISPATCH_RELEASE_CONT",
    };
    static const char* tx_ser_state_str[] =
    {
            "TX_SER_IDLE",
            "TX_SER_READ_FIRST",
            "TX_SER_WRITE_B0",
            "TX_SER_WRITE_B1",
            "TX_SER_WRITE_B2",
            "TX_SER_READ_WRITE",
            "TX_SER_GAP",
    };
    static const char* tx_s2g_state_str[] =
    {
            "TX_S2G_IDLE",
            "TX_S2G_WRITE_DATA",
            "TX_S2G_WRITE_LAST_DATA",
            "TX_S2G_WRITE_CS",
    };

    std::cout << "MULTI_NIC " << name() << " : "
              << vci_state_str[r_vci_fsm.read()]                 << " | "
              << rx_g2s_state_str[r_rx_g2s_fsm.read()]           << " | "
              << rx_des_state_str[r_rx_des_fsm.read()]           << " | "
              << rx_dispatch_state_str[r_rx_dispatch_fsm.read()] << " | "
              << tx_dispatch_state_str[r_tx_dispatch_fsm.read()] << " | "
              << tx_ser_state_str[r_tx_ser_fsm.read()]           << " | "
              << tx_s2g_state_str[r_tx_s2g_fsm.read()]           << std::endl;

    if ( mode & 0x0001 ) // configuration & instrumentation registers
    {
        std::cout << "---- Instrumentation Registers" << std::dec           << std::endl
                  << "r_total_len_rx_gmii : " << r_total_len_rx_gmii.read() << std::endl
                  << "r_total_len_rx_chan : " << r_total_len_rx_chan.read() << std::endl
                  << "r_total_len_tx_chan : " << r_total_len_tx_chan.read() << std::endl
                  << "r_total_len_tx_gmii : " << r_total_len_tx_gmii.read() << std::endl;

        std::cout << "---- Global config Registers" << std::hex                     << std::endl
                  << "r_global_nic_on        : " << r_global_nic_on.read()          << std::endl
                  << "r_global_active        : " << r_global_active_channels.read() << std::endl
                  << "r_global_bc_enable     : " << r_global_bc_enable.read()       << std::endl
                  << "r_global_bypass_enable : " << r_global_bypass_enable.read()   << std::endl
                  << "r_global_tdm_enable    : " << r_global_tdm_enable.read()      << std::endl;

        for (size_t k = 0; k < m_channels; k++)
        {
            std::cout << "---- Channel[" << std::hex << k << "] config registers"     << std::endl
                      << "r_channel_mac_4        : " << r_channel_mac_4[k].read()     << std::endl
                      << "r_channel_mac_2        : " << r_channel_mac_2[k].read()     << std::endl
                      << "r_channel_rx_buf0_desc : " << r_channel_rx_desc_0[k].read() << std::endl
                      << "r_channel_rx_buf1_desc : " << r_channel_rx_desc_1[k].read() << std::endl
                      << "r_channel_tx_buf0_desc : " << r_channel_tx_desc_0[k].read() << std::endl
                      << "r_channel_tx_buf1_desc : " << r_channel_tx_desc_1[k].read() << std::endl
                      << "r_channel_rx_run       : " << r_channel_rx_run[k].read()    << std::endl
                      << "r_channel_tx_run       : " << r_channel_tx_run[k].read()    << std::endl;
        }
    }

    if ( mode & 0x0010 ) // display RX registers
    {
        std::cout << "---- RX_G2S Registers" << std::hex                          << std::endl
                  << "r_rx_g2s_checksum      : " << r_rx_g2s_checksum.read()      << std::endl
                  << "r_rx_g2s_dt0           : " << (uint32_t)r_rx_g2s_dt0.read() << std::endl
                  << "r_rx_g2s_dt1           : " << (uint32_t)r_rx_g2s_dt1.read() << std::endl
                  << "r_rx_g2s_dt2           : " << (uint32_t)r_rx_g2s_dt2.read() << std::endl
                  << "r_rx_g2s_dt3           : " << (uint32_t)r_rx_g2s_dt3.read() << std::endl
                  << "r_rx_g2s_dt4           : " << (uint32_t)r_rx_g2s_dt4.read() << std::endl
                  << "r_rx_g2s_dt5           : " << (uint32_t)r_rx_g2s_dt5.read() << std::endl
                  << "r_rx_g2s_delay         : " << r_rx_g2s_delay.read()         << std::endl;

        std::cout << "---- RX_DES Registers" << std::hex                              << std::endl
                  << "r_rx_des_counter       : " << r_rx_des_counter_bytes.read()     << std::endl
                  << "r_rx_des_padding       : " << r_rx_des_padding.read()           << std::endl
                  << "r_rx_des_data[0]       : " << (uint32_t)r_rx_des_data[0].read() << std::endl
                  << "r_rx_des_data[1]       : " << (uint32_t)r_rx_des_data[1].read() << std::endl
                  << "r_rx_des_data[2]       : " << (uint32_t)r_rx_des_data[2].read() << std::endl
                  << "r_rx_des_data[3]       : " << (uint32_t)r_rx_des_data[3].read() << std::endl;

        std::cout << "---- RX_MULTI_FIFO" << std::endl;
        r_rx_fifo_multi.print_trace( 1 );

        std::cout << "---- RX_DISPATCH Registers" << std::hex                   << std::endl
                  << "r_rx_dispatch_bp       : " << r_rx_dispatch_bp.read()     << std::endl
                  << "r_rx_dispatch_data0    : " << r_rx_dispatch_data0.read()  << std::endl
                  << "r_rx_dispatch_data1    : " << r_rx_dispatch_data1.read()  << std::endl
                  << "r_rx_dispatch_nbytes   : " << r_rx_dispatch_nbytes.read() << std::endl
                  << "r_rx_dispatch_dest     : " << r_rx_dispatch_dest.read()   << std::endl;

        for ( size_t k = 0 ; k < m_channels ; k++ )
        {
            std::cout << "---- RX_CHBUF[" << std::dec << k << "] state" << std::endl;
            r_rx_chbuf[k]->print_trace( k , 0 );
        }
    }

    if ( mode & 0x0100 ) // display TX registers
    {
        for ( size_t k = 0 ; k < m_channels ; k++ )
        {
            std::cout << "---- TX_CHBUF[" << std::dec << k << "] state" << std::endl;
            r_tx_chbuf[k]->print_trace( k , 0 );
        }

        std::cout << "---- TX_DISPATCH Registers" << std::hex                     << std::endl
                  << "r_tx_dispatch_channel  : " << r_tx_dispatch_channel.read()  << std::endl
                  << "r_tx_dispatch_data0    : " << r_tx_dispatch_data0.read()    << std::endl
                  << "r_tx_dispatch_data1    : " << r_tx_dispatch_data1.read()    << std::endl
                  << "r_tx_dispatch_packets  : " << r_tx_dispatch_packets.read()  << std::endl
                  << "r_tx_dispatch_words    : " << r_tx_dispatch_words.read()    << std::endl
                  << "r_tx_dispatch_bytes    : " << r_tx_dispatch_bytes.read()    << std::endl
                  << "r_tx_dispatch_write_bp : " << r_tx_dispatch_write_bp.read() << std::endl
                  << "r_tx_dispatch_write_tx : " << r_tx_dispatch_write_tx.read() << std::endl;

        std::cout << "---- TX_MULTI_FIFO" << std::endl;
        r_tx_fifo_multi.print_trace( 1 );

        std::cout << "---- TX_SER Registers" << std::hex                          << std::endl
                  << "r_tx_ser_words         : " << r_tx_ser_words.read()         << std::endl
                  << "r_tx_ser_bytes         : " << r_tx_ser_bytes.read()         << std::endl
                  << "r_tx_ser_first         : " << r_tx_ser_first.read()         << std::endl
                  << "r_tx_ser_ifg           : " << r_tx_ser_ifg.read()           << std::endl
                  << "r_tx_ser_data          : " << r_tx_ser_data.read()          << std::endl;

        std::cout << "---- TX_S2G Registers" << std::hex                           << std::endl
                  << "r_tx_s2g_checksum      : " << r_tx_s2g_checksum.read()       << std::endl
                  << "r_tx_s2g_data          : " << (uint32_t)r_tx_s2g_data.read() << std::endl
                  << "r_tx_s2g_index         : " << r_tx_s2g_index.read()          << std::endl;
    }

    if ( mode & 0x1000 ) // display RX and TX chbuf only
    {
        for ( size_t k = 0 ; k < m_channels ; k++ )
        {
            std::cout << "---- RX_CHBUF[" << std::dec << k << "] state" << std::endl;
            r_rx_chbuf[k]->print_trace( k , 0 );
            std::cout << "---- TX_CHBUF[" << std::dec << k << "] state" << std::endl;
            r_tx_chbuf[k]->print_trace( k , 0 );
        }
    }
} // end print_trace()

////////////////////////////////////////////////////////////////////
//        Constructor
////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciMultiNic(sc_core::sc_module_name 		        name,
                        const soclib::common::IntTab 		    &tgtid,
                        const soclib::common::MappingTable      &mt,
                        const size_t 				            channels,
                        const uint32_t                          mac_4,
                        const uint32_t                          mac_2,
                        const int                               mode,
                        const uint32_t                          inter_frame_gap)
           : caba::BaseModule(name),

           r_total_len_rx_gmii("r_total_len_rx_gmii"),
           r_total_len_rx_chan("r_total_len_rx_chan"),
           r_total_len_tx_chan("r_total_len_tx_chan"),
           r_total_len_tx_gmii("r_total_len_tx_gmii"),

           r_global_bc_enable("r_global_bc_enable"),
           r_global_nic_on("r_global_nic_on"),
           r_global_nic_nb_chan("r_global_nic_nb_chan"),
           r_global_active_channels("r_global_active_channels"),
           r_global_tdm_enable("r_global_tdm_enable"),
           r_global_tdm_period("r_global_tdm_period"),
           r_global_tdm_timer("r_global_tdm_timer"),
           r_global_tdm_channel("r_global_tdm_channel"),
           r_global_bypass_enable("r_global_bypass_enable"),

           r_channel_mac_4(soclib::common::alloc_elems<sc_signal<uint32_t> >
                           ("r_channel_mac_4", 8)),
           r_channel_mac_2(soclib::common::alloc_elems<sc_signal<uint32_t> >
                           ("r_channel_mac_2", 8)),
           r_channel_rx_desc_0(soclib::common::alloc_elems<sc_signal<uint64_t> >
                               ("r_channel_rx_bufaddr_0", 8)),
           r_channel_rx_desc_1(soclib::common::alloc_elems<sc_signal<uint64_t> >
                                  ("r_channel_rx_bufaddr_1", 8)),
           r_channel_rx_run(soclib::common::alloc_elems<sc_signal<bool> >
                            ("r_channel_rx_run", 8)),
           r_channel_tx_desc_0(soclib::common::alloc_elems<sc_signal<uint64_t> >
                                  ("r_channel_tx_bufaddr_0", 8)),
           r_channel_tx_desc_1(soclib::common::alloc_elems<sc_signal<uint64_t> >
                                  ("r_channel_tx_bufaddr_1", 8)),
           r_channel_tx_run(soclib::common::alloc_elems<sc_signal<bool> >
                            ("r_channel_tx_run", 8)),

           r_vci_fsm("r_vci_fsm"),
           r_vci_srcid("r_vci_srcid"),
           r_vci_trdid("r_vci_trdid"),
           r_vci_pktid("r_vci_pktid"),
           r_vci_wdata("r_vci_wdata"),
           r_vci_be("r_vci_be"), 
           r_vci_nwords("r_vci_nwords"),
           r_vci_address("r_vci_address"),

           r_rx_g2s_fsm("r_rx_g2s_fsm"),
           r_rx_g2s_checksum("r_rx_g2s_checksum"),
           r_rx_g2s_dt0("r_rx_g2s_dt0"),
           r_rx_g2s_dt1("r_rx_g2s_dt1"),
           r_rx_g2s_dt2("r_rx_g2s_dt2"),
           r_rx_g2s_dt3("r_rx_g2s_dt3"),
           r_rx_g2s_dt4("r_rx_g2s_dt4"),
           r_rx_g2s_dt5("r_rx_g2s_dt5"),
           r_rx_g2s_delay("r_rx_g2s_delay"),

           r_rx_g2s_npkt_received("r_rx_g2s_npkt_received"),
           r_rx_g2s_npkt_discarded("r_rx_g2s_npkt_discarded"),

           r_rx_des_fsm("r_rx_des_fsm"),
           r_rx_des_counter_bytes("r_rx_des_counter_bytes"),
           r_rx_des_padding("r_rx_des_padding"),
           r_rx_des_data(soclib::common::alloc_elems<sc_signal<uint8_t> >
                         ("r_rx_des_data", 4)),

           r_rx_des_npkt_success("r_rx_des_npkt_success"),
           r_rx_des_npkt_too_small("r_rx_des_npkt_too_small"),
           r_rx_des_npkt_too_big("r_rx_des_npkt_too_big"),
           r_rx_des_npkt_mfifo_full("r_rx_des_npkt_mfifo_full"),
           r_rx_des_npkt_cs_fail("r_rx_des_npkt_cs_fail"),

           r_rx_dispatch_fsm("r_rx_dispatch_fsm"),
           r_rx_dispatch_bp("r_rx_dispatch_bp"),
           r_rx_dispatch_data0("r_rx_dispatch_data0"),
           r_rx_dispatch_data1("r_rx_dispatch_data1"),
           r_rx_dispatch_nbytes("r_rx_dispatch_nbytes"),
           r_rx_dispatch_dest("r_rx_dispatch_dest"),

           r_rx_dispatch_npkt_received("r_rx_dispatch_npkt_received"),
           r_rx_dispatch_npkt_broadcast("r_rx_dispatch_npkt_broadcast"),
           r_rx_dispatch_npkt_dst_fail("r_rx_dispatch_npkt_dst_fail"),
           r_rx_dispatch_npkt_channel_full("r_rx_dispatch_npkt_channel_full"),

           r_tx_dispatch_fsm("r_tx_dispatch_fsm"),
           r_tx_dispatch_channel("r_tx_dispatch_channel"),
           r_tx_dispatch_data0("r_tx_dispatch_data0"),
           r_tx_dispatch_data1("r_tx_dispatch_data1"),
           r_tx_dispatch_packets("r_tx_dispatch_packets"),
           r_tx_dispatch_words("r_tx_dispatch_words"),
           r_tx_dispatch_bytes("r_tx_dispatch_bytes"),
           r_tx_dispatch_write_bp("r_tx_dispatch_write_bp"),
           r_tx_dispatch_write_tx("r_tx_dispatch_write_tx"),

           r_tx_dispatch_npkt_received("r_tx_dispatch_npkt_received"),
           r_tx_dispatch_npkt_too_small("r_tx_dispatch_npkt_too_small"),
           r_tx_dispatch_npkt_too_big("r_tx_dispatch_npkt_too_big"),
           r_tx_dispatch_npkt_src_fail("r_tx_dispatch_npkt_src_fail"),
           r_tx_dispatch_npkt_broadcast("r_tx_dispatch_npkt_broadcast"),
           r_tx_dispatch_npkt_bypass("r_tx_dispatch_npkt_bypass"),
           r_tx_dispatch_npkt_transmit("r_tx_dispatch_npkt_transmit"),

           r_tx_ser_fsm("r_tx_ser_fsm"),
           r_tx_ser_words("r_tx_ser_words"),
           r_tx_ser_bytes("r_tx_ser_bytes"),
           r_tx_ser_first("r_tx_ser_first"),
           r_tx_ser_ifg("r_tx_ser_ifg"),
           r_tx_ser_data("r_tx_ser_data"),

           r_tx_s2g_fsm("r_tx_s2g_fsm"),
           r_tx_s2g_checksum("r_tx_s2g_checksum"),
           r_tx_s2g_data("r_tx_s2g_data"),
           r_tx_s2g_index("r_tx_s2g_index"),

           r_rx_fifo_stream("r_rx_fifo_stream", 2),      // 2 slots of one byte
           r_tx_fifo_stream("r_tx_fifo_stream", 2),      // 2 slots of one byte
           r_rx_fifo_multi("r_rx_fifo_multi", 32, 32),   // 1024 slots of one word
           r_tx_fifo_multi("r_tx_fifo_multi", 32, 32),   // 1024 slots of one word
           r_bp_fifo_multi("r_bp_fifo_multi", 32, 32),   // 1024 slots of one word

           m_seglist(mt.getSegmentList(tgtid)),
           m_channels(channels),
           m_default_mac_4(mac_4),
           m_default_mac_2(mac_2),
           m_inter_frame_gap(inter_frame_gap),

           p_clk("p_clk"),
           p_resetn("p_resetn"),
           p_vci("p_vci"),
           p_rx_irq(soclib::common::alloc_elems<sc_core::sc_out<bool> >("p_rx_irq", channels)),
           p_tx_irq(soclib::common::alloc_elems<sc_core::sc_out<bool> >("p_tx_irq", channels))
{
    assert( ((mode == NIC_MODE_FILE) || (mode == NIC_MODE_SYNTHESIS) || (mode == NIC_MODE_TAP)) and
            "VCI_MULTI_NIC error : Illegal mode for backend");

    assert( (channels <= 8 ) and
            "VCI_MULTI_NIC error : No more than 8 channels");

    assert( (inter_frame_gap >= 12 ) and
            "VCI_MULTI_NIC error : Inter frame gap cannot be smaller than 12 cycles");

    if ( mode == NIC_MODE_FILE )
    {
        std::cout << "  - Building VciMultiNic - File version " << name << std::endl;

        r_backend_rx = new NicRxGmii( inter_frame_gap, true );
        r_backend_tx = new NicTxGmii();
    }

    // allocating one RX_CHBUF and one TX_CHBUF per channel
    for ( size_t k = 0 ; k < channels ; k++ )
    {
        r_rx_chbuf[k] = (NicRxChbuf*)new NicRxChbuf( RX_TIMEOUT, RX_CHBUF_DEBUG );
        r_tx_chbuf[k] = (NicTxChbuf*)new NicTxChbuf( TX_CHBUF_DEBUG );
    }

    // allocating RX and TX backend
    if ( mode == NIC_MODE_SYNTHESIS )
    {
        std::cout << "  - Building VciMultiNic - Synthesis version " << name << std::endl;

        srandom( 0xBABEF00D );  // for reproductible synthesis

        r_backend_rx = new NicRxGmii( inter_frame_gap, false );
        r_backend_tx = new NicTxGmii();
    }
    if ( mode == NIC_MODE_TAP )
    {
        std::cout << "  - Building VciMultiNic - TAP version " << name << std::endl;

#if !defined(__APPLE__) || !defined(__MACH__)
        r_backend_rx = new NicRxTap( inter_frame_gap );
        r_backend_tx = new NicTxTap();

        // get a TAP fd
        uint32_t tmp_fd = open_tap_fd();

        if (tmp_fd < 0) 
        {
            std::cerr << name << " : Cannot open the TAP interface " << std::endl;
            exit(-1);
        }

        dynamic_cast<NicRxTap*>(r_backend_rx)->set_fd(tmp_fd);
        dynamic_cast<NicRxTap*>(r_backend_rx)->set_ifr(&m_tap_ifr);

        dynamic_cast<NicTxTap*>(r_backend_tx)->set_fd(tmp_fd);
        dynamic_cast<NicTxTap*>(r_backend_tx)->set_ifr(&m_tap_ifr);
#else
        std::cout << "     ... but the TAP backend is not supported" << std::endl;
        exit(0);
#endif
    }
    
    size_t nbsegs = 0;

    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ )
    {
        nbsegs++;
        assert( ( (seg->baseAddress() & 0x7FFFF) == 0 ) and
                   "VCI_MULTI_NIC Error : The segment base address must be multiple of 512 Kbytes");

        assert( ( seg->size() >= 0x80000 ) and
                  "VCI_MULTI_NIC Error : The segment size cannot be smaller than 512 Kbytes");

        std::cout << "    => segment " << seg->name()
                  << " / base = " << std::hex << seg->baseAddress()
                  << " / size = " << seg->size() << std::endl;
    }

    assert ( (nbsegs != 0) and
             "VCI_MULTI_NIC error : no segment allocated");

    assert( ((vci_param::B == 4) or (vci_param::B == 8)) and
            "VCI_MULTI_NIC error : The VCI DATA field must be 32 or 64 bits");

    assert( (channels <= 8)  and
            "VCI_MULTI_NIC error : The number of channels cannot be larger than 8");

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
}

#if !defined(__APPLE__) || !defined(__MACH__)

////////////////////////////
tmpl(int32_t)::open_tap_fd()
{
    int32_t tap_fd = -1;

    tap_fd = open("/dev/net/tun", O_RDWR);

    if (tap_fd < 0) // error
    {
        std::cerr << name() << ": Unable to open /dev/net/tun" << std::endl;
    }
    else // we can continue
    {
        int flags = fcntl(tap_fd, F_GETFL, 0);
        fcntl(tap_fd, F_SETFL, flags | O_NONBLOCK);
        memset((void*)&m_tap_ifr, 0, sizeof(m_tap_ifr));
        m_tap_ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

        if (ioctl(tap_fd, TUNSETIFF, (void *) &m_tap_ifr) < 0)
        {
            ::close(tap_fd);
            tap_fd = -1;
            std::cerr << name() << ": Unable to setup tap interface, check privileges."
#ifdef __linux__
                              << " (try: sudo setcap cap_net_admin=eip ./system.x)"
#endif
                              << std::endl;
            return -1;
        }
        else
        {
            std::cout << name() << ": TAP interface succesfully created: " << m_tap_ifr.ifr_name << "\n";
        }
    }
    return tap_fd;
} // end open_tap_fd

#endif  /*  !defined(__APPLE__) || !defined(__MACH__) */


////////////////////////////
//  Destructor
////////////////////////////
tmpl(/**/)::~VciMultiNic() 
{
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_mac_4, 8);
    soclib::common::dealloc_elems<sc_signal<uint32_t> >(r_channel_mac_2, 8);
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_rx_desc_0, 8);
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_rx_desc_1, 8);
    soclib::common::dealloc_elems<sc_signal<bool> >(r_channel_rx_run, 8);
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_tx_desc_0, 8);
    soclib::common::dealloc_elems<sc_signal<uint64_t> >(r_channel_tx_desc_1, 8);
    soclib::common::dealloc_elems<sc_signal<bool> >(r_channel_tx_run, 8);
    soclib::common::dealloc_elems<sc_signal<uint8_t> >(r_rx_des_data, 4);
//  soclib::common::dealloc_elems<NicRxChbuf>(r_rx_chbuf, m_channels);
//  soclib::common::dealloc_elems<NicTxChbuf>(r_tx_chbuf, m_channels);
    soclib::common::dealloc_elems<sc_core::sc_out<bool> >(p_rx_irq, m_channels);
    soclib::common::dealloc_elems<sc_core::sc_out<bool> >(p_tx_irq, m_channels);
}

}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


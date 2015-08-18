/*
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
 * Copyright (c) Telecom ParisTech
 *         Alexandre Becoulet <alexandre.becoulet@enst.fr>, 2012
 *
 * Maintainers: becoulet
 */

#ifndef ETHERNET_REGS_H
#define ETHERNET_REGS_H

enum SoclibEthernetRegisters {
    ETHERNET_TX_SIZE  = 0,       /*< on write: set size of packet to send for next adress push */
    ETHERNET_TX_FIFO  = 1,       /*< on write: push address and size of packet to send on tx fifo,
                                     on read:  pop status of sent packet from tx fifo. 0 if empty */
    ETHERNET_RX_SIZE  = 2,       /*< on write: set size of rx buffer for next adress push,
                                     on read:  get size of last rx packet, 0 if empty. */
    ETHERNET_RX_FIFO  = 3,       /*< on write: push address and size of buffer on rx fifo,
                                     on read:  pop status of last rx packet. 0 if empty */
    ETHERNET_STATUS   = 4,       /*< on read:  contains device status flags */
    ETHERNET_CTRL     = 4,       /*< on write: device control actions */
    ETHERNET_FIFO_SIZE = 5,      /*< contains size of TX & RX FIFOs */
    ETHERNET_MAC_LOW   = 6,      /*< contains mac address bytes 0, 1, 2 and 3 */
    ETHERNET_MAC_HIGH  = 7,      /*< contains mac address bytes 4 and 5 */
};

enum SoclibEthernetRxStatus {
	ETHERNET_RX_EMPTY = 0,
	ETHERNET_RX_DONE = 1,
	ETHERNET_RX_DMA_ERR = 2,
	ETHERNET_RX_PHY_ERR = 3,
};

enum SoclibEthernetTxStatus {
	ETHERNET_TX_EMPTY = 0,
	ETHERNET_TX_DONE = 1,
	ETHERNET_TX_DMA_ERR = 2,
	ETHERNET_TX_PHY_ERR = 3,
};

enum SoclibEthernetStatus {
	ETHERNET_ST_LINK_UP = 1,
	ETHERNET_ST_TX_DONE = 2,     /*< The ETHERNET_TX_FIFO register content is valid */
	ETHERNET_ST_RX_DONE = 4,     /*< The ETHERNET_RX_FIFO register content is valid */
};

enum SoclibEthernetCtrl {
	ETHERNET_CTRL_RESET  = 1,     //< fifos become empty, disable interrupts
	ETHERNET_CTRL_TX_IRQ = 2,     //< enable TX irq when written to 1
	ETHERNET_CTRL_RX_IRQ = 4,     //< enable RX irq when written to 1
	ETHERNET_CTRL_LINK_IRQ = 8,   //< enable link status change irq when written to 1
};

#endif /* ETHERNET_REGS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


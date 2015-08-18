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
 * Copyright (c) UPMC, Lip6, Asim
 *         alain.greiner@lip6.fr
 *
 * Maintainers: alain
 */

#ifndef CHBUF_DMA_REGS_H
#define CHBUF_DMA_REGS_H

// each channel define 8 addressable registers (32 bits),
// but the channel sub-segment is aligned on a 4Kbytes page boundary

enum SoclibChbufDmaRegisters 
{
    CHBUF_RUN           = 0,    // write-only : channel activated
    CHBUF_STATUS        = 1,    // read-only  : channel fsm state
    CHBUF_SRC_DESC      = 2,    // read/write : source chbuf : descriptor base address
    CHBUF_DST_DESC      = 3,    // read/write : destination chbuf : descriptor base address,
    CHBUF_SRC_NBUFS     = 4,    // read/write : source chbuf : number of buffers,
    CHBUF_DST_NBUFS     = 5,    // read/write : destination chbuf : number of buffers,
    CHBUF_BUF_SIZE      = 6,    // read/write : buffer size for both source & destination  
    CHBUF_PERIOD        = 7,    // read/write : period for status polling 
    CHBUF_SRC_EXT       = 8,    // read/write : source chbuf : descriptor base address
    CHBUF_DST_EXT       = 9,    // read/write : destination chbuf : descriptor base address,
    CHBUF_SRC_INDEX     = 10,   // read-only  : source chbuf : buffer index,
    CHBUF_DST_INDEX     = 11,   // read-only  : destination chbuf : buffer index,
    /****/
    CHBUF_CHANNEL_SPAN	= 1024,
};

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


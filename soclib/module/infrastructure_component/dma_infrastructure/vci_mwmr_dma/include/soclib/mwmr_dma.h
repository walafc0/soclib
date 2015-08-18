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
 *         Alain Greiner <alain.greiner@lip6.fr>
 *
 * Maintainers: alain
 */

#ifndef MWMR_DMA_REGISTERS_H
#define MWMR_DMA_REGISTERS_H

enum MwmrDmaRegisters 
{
    MWR_CHANNEL_BUFFER_LSB   = 0,     // Data Buffer paddr 32 LSB bits
    MWR_CHANNEL_BUFFER_MSB   = 1,     // Data Buffer paddr extension
    MWR_CHANNEL_DESC_LSB     = 2,     // MWMR descriptor paddr 32 LSB bits
    MWR_CHANNEL_DESC_MSB     = 3,     // MWMR descriptor padr extension
    MWR_CHANNEL_LOCK_LSB     = 4,     // MWMR lock paddr 32 LSB bits
    MWR_CHANNEL_LOCK_MSB     = 5,     // MWMR lock paddr extension
    MWR_CHANNEL_WAY          = 6,     // TO_COPROC / FROMCOPROC        (Read-only)
    MWR_CHANNEL_MODE         = 7,     // MWMR / DMA / DMA_IRQ    
    MWR_CHANNEL_SIZE         = 8,     // Data Buffer size (bytes)
    MWR_CHANNEL_RUNNING      = 9,     // Channel running
    MWR_CHANNEL_STATUS       = 10,    // channel FSM state             (Read-only)
    //
    MWR_CHANNEL_SPAN         = 16,
};

typedef enum MwmrDmaModes 
{
    MODE_MWMR       = 0,
    MODE_DMA_IRQ    = 1,
    MODE_DMA_NO_IRQ = 2,
} channel_mode_t;

typedef enum MwmrDmaWays
{
    TO_COPROC       = 0,
    FROM_COPROC     = 1,
} channel_way_t;

typedef enum MwmrChannelStatus
{
    MWR_CHANNEL_SUCCESS    = 0,
    MWR_CHANNEL_ERROR_DATA = 1,
    MWR_CHANNEL_ERROR_LOCK = 2,
    MWR_CHANNEL_ERROR_DESC = 3,
    MWR_CHANNEL_BUSY       = 4,
} channel_status_t;

#endif 

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


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
 * Copyright (c) INPG/GIPSA-Lab
 *         Dominique Houzet <houzet@lis.inpg.fr>, 2008
 *
 */
#ifndef PCI_REGS_H
#define PCI_REGS_H

enum SoclibPciRegisters {
    PCI_VALUE    = 0,
    PCI_MODE     = 1,
    PCI_ADR   = 2,
    PCI_RESETIRQ = 3,
    PCI_NB     = 4,
};

enum SoclibPciMode {
	PCI_DMA_WRITE_IRQ = 1,
	PCI_DMA_WRITE_NO_IRQ = 2,
	PCI_DMA_READ_IRQ = 3,
	PCI_DMA_READ_NO_IRQ = 4,	
	PCI_DMA_LOOPBACK = 5,
};

#endif /* PCI_REGS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


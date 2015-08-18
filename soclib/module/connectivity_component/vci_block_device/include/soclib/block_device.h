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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 */
#ifndef BLOCK_DEVICE_REGS_H
#define BLOCK_DEVICE_REGS_H

enum SoclibBlockDeviceRegisters {
    BLOCK_DEVICE_BUFFER,
    BLOCK_DEVICE_LBA,
    BLOCK_DEVICE_COUNT,
    BLOCK_DEVICE_OP,
    BLOCK_DEVICE_STATUS,
    BLOCK_DEVICE_IRQ_ENABLE,
    BLOCK_DEVICE_SIZE,
    BLOCK_DEVICE_BLOCK_SIZE,
};

enum SoclibBlockDeviceOp {
	BLOCK_DEVICE_NOOP,
	BLOCK_DEVICE_READ,
	BLOCK_DEVICE_WRITE,
};

enum SoclibBlockDeviceStatus {
	BLOCK_DEVICE_IDLE,
	BLOCK_DEVICE_BUSY,
	BLOCK_DEVICE_READ_SUCCESS,
	BLOCK_DEVICE_WRITE_SUCCESS,
	BLOCK_DEVICE_READ_ERROR,
	BLOCK_DEVICE_WRITE_ERROR,
	BLOCK_DEVICE_ERROR,
};

#endif /* BLOCK_DEVICE_REGS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


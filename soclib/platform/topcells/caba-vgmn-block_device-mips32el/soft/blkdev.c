/*
 *
 * SOCLIB_GPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU GPLv2.
 * 
 * SoCLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 * 
 * SOCLIB_GPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: nipo
 */

#include "system.h"
#include "soclib/block_device.h"

#include "blkdev.h"

int blkdev_read( const size_t lba, void *buffer, const size_t len )
{
	uint32_t i;
	for (i=0; i<len+16; i+=4){
#if __mips >= 32
		asm volatile(
			" cache %0, %1"
			: : "i" (0x11) , "R" (*(uint8_t*)(buffer+i))
			: "memory"
			);
#elif __mips
		asm volatile("lw $0, 0(%0)"::"r"((uint32_t)buffer+i));
#endif
	}
	soclib_io_set(base(BD), BLOCK_DEVICE_LBA, lba);
	soclib_io_set(base(BD), BLOCK_DEVICE_BUFFER, (uint32_t)buffer);
	soclib_io_set(base(BD), BLOCK_DEVICE_COUNT, len);
	soclib_io_set(base(BD), BLOCK_DEVICE_OP, BLOCK_DEVICE_READ);

	int state;
	do {
		state = soclib_io_get(base(BD), BLOCK_DEVICE_STATUS);
	} while (state == BLOCK_DEVICE_BUSY);
	return state;
}

int blkdev_write( const size_t lba, const void *buffer, const size_t len )
{
	soclib_io_set(base(BD), BLOCK_DEVICE_LBA, lba);
	soclib_io_set(base(BD), BLOCK_DEVICE_BUFFER, (uint32_t)buffer);
	soclib_io_set(base(BD), BLOCK_DEVICE_COUNT, len);
	soclib_io_set(base(BD), BLOCK_DEVICE_OP, BLOCK_DEVICE_WRITE);

	int state;
	do {
		state = soclib_io_get(base(BD), BLOCK_DEVICE_STATUS);
	} while (state == BLOCK_DEVICE_BUSY);
	return state;
}

uint32_t blkdev_size()
{
	return soclib_io_get(base(BD), BLOCK_DEVICE_SIZE);
}

uint32_t blkdev_block_size()
{
	return soclib_io_get(base(BD), BLOCK_DEVICE_BLOCK_SIZE);
}

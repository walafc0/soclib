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

#include "soclib/simhelper.h"
#include "stdio.h"
#include "system.h"
#include "blkdev.h"

#include "../segmentation.h"
#include "crc32.h"
#include "expected_crc.h"

#define BLOCKS 2

void et_hexdump( uint8_t *data, size_t len )
{
	size_t i;
	for ( i=0; i<len; i+=16 ) {
		printf(" %03x |", (unsigned int)i);
		size_t end = (i+16>len)?len:(i+16), j;
		for ( j=i; j<end; ++j )
			printf(" %02x", (int)data[j]);
		for ( ; j<i+16; ++j) 
			printf("   ");
		printf(" | ");
		for ( j=i; j<end; ++j )
			printf("%c", ((int)data[j]<32)?'.':data[j]);
		printf("\n");
	}
}

int main(void)
{
	uint32_t i;
	const size_t block_size = blkdev_block_size();
	const uint32_t n_sectors = blkdev_size();
	const size_t crc_block_mul = block_size / 512;
	uint8_t data[BLOCKS*block_size];
	uint32_t crc = 0;

	printf("Hello, world\n");

	for ( i=0; i<n_sectors; i+=BLOCKS ) {
		uint32_t x = expected_crc[(i+BLOCKS-1)*crc_block_mul];
		blkdev_read(i, data, BLOCKS);
//		et_hexdump(data, 512*BLOCKS);
	    crc = do_crc32(crc, data, block_size*BLOCKS);
		printf("CRC of %d first blocks is 0x%x, expected: 0x%x\n", i, crc, x);
		assert(crc == x);
	}
	printf("CRC of %d first blocks is 0x%x\n", n_sectors, crc);

	for ( i=0; i<100000; ++i )
		asm volatile("nop");

	soclib_io_set(
		base(SIMHELPER),
		SIMHELPER_SC_STOP,
		0);
	while (1);
	return 0;
}

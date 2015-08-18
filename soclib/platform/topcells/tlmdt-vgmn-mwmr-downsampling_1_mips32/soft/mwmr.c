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
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: nipo
 */

#include "mwmr.h"
#include "soclib_io.h"
#include "soclib/mwmr_controller.h"

static void *memcpy( void *_dst, void *_src, unsigned long size )
{
	uint32_t *dst = _dst;
	uint32_t *src = _src;
	if ( ! ((uint32_t)dst & 3) && ! ((uint32_t)src & 3) )
		while (size > 3) {
			*dst++ = *src++;
			size -= 4;
		}

	unsigned char *cdst = (unsigned char*)dst;
	unsigned char *csrc = (unsigned char*)src;

	while (size--) {
		*cdst++ = *csrc++;
	}
	return _dst;
}

static inline void mwmr_lock( volatile uint32_t *lock )
{
	__asm__ __volatile__(
		".set push        \n\t"
		".set noreorder   \n\t"
		"1:               \n\t"
		"ll    $2, %0     \n\t"
		"bnez  $2, 1b     \n\t"
		"ori   $3, $0, 1  \n\t"
		"sc    $3, %0     \n\t"
		"beqz  $3, 1b     \n\t"
		"nop              \n\t"
		"2:               \n\t"
		".set pop         \n\t"
		:
		: "m"(lock)
		: "$3", "$2", "memory"
		);
}

static inline void mwmr_unlock( volatile uint32_t *lock )
{
	*lock = 0;
}

static inline void busy_wait( ncycles )
{
	int i;
	for ( i=0; i<ncycles; i+=2 )
		asm volatile("nop");
}

static inline size_t min(size_t a, size_t b)
{
	if (a<b)
		return a;
	return b;
}


void
mwmr_hw_init( void *coproc, enum SoclibMwmrWay way,
			  unsigned int no, const mwmr_t *mwmr )
{
	soclib_io_set( coproc, MWMR_CONFIG_FIFO_WAY, way );
	soclib_io_set( coproc, MWMR_CONFIG_FIFO_NO, no );
	soclib_io_set( coproc, MWMR_CONFIG_STATUS_ADDR, (uint32_t)&mwmr->status );
	soclib_io_set( coproc, MWMR_CONFIG_DEPTH, mwmr->gdepth );
	soclib_io_set( coproc, MWMR_CONFIG_WIDTH, mwmr->width );
	soclib_io_set( coproc, MWMR_CONFIG_BUFFER_ADDR, (uint32_t)mwmr->buffer );
	soclib_io_set( coproc, MWMR_CONFIG_RUNNING, 1 );
}

void mwmr_config( void *coproc, unsigned int no, const uint32_t val )
{
	// assert(no < MWMR_IOREG_MAX);
	soclib_io_set( coproc, no, val );
}

uint32_t mwmr_status( void *coproc, unsigned int no )
{
	// assert(no < MWMR_IOREG_MAX);
	return soclib_io_get( coproc, no );
}

void mwmr_read( mwmr_t *fifo, void *_ptr, size_t lensw )
{
	uint8_t *ptr = _ptr;
    volatile soclib_mwmr_status_s *status = &fifo->status;

	mwmr_lock( &status->lock );
    while ( lensw ) {
        size_t len;
        while (status->usage < fifo->width) {
            mwmr_unlock( &status->lock );
			busy_wait(1000);
            mwmr_lock( &status->lock );
        }
        while ( lensw && status->usage >= fifo->width ) {
			void *sptr;

            if ( status->rptr < status->wptr )
                len = status->usage;
            else
                len = (fifo->gdepth - status->rptr);
            len = min(len, lensw);
			sptr = &((uint8_t*)fifo->buffer)[status->rptr];
            memcpy( ptr, sptr, len );
            status->rptr += len;
            if ( status->rptr == fifo->gdepth )
                status->rptr = 0;
            ptr += len;
            status->usage -= len;
            lensw -= len;
        }
    }
	mwmr_unlock( &status->lock );
}

void mwmr_write( mwmr_t *fifo, const void *_ptr, size_t lensw )
{
	uint8_t *ptr = _ptr;
    volatile soclib_mwmr_status_s *status = &fifo->status;

	mwmr_lock( &status->lock );
    while ( lensw ) {
        size_t len;
        while (status->usage >= fifo->gdepth) {
            mwmr_unlock( &status->lock );
			busy_wait(1000);
            mwmr_lock( &status->lock );
        }
        while ( lensw && status->usage < fifo->gdepth ) {
			void *dptr;

            if ( status->rptr <= status->wptr )
                len = (fifo->gdepth - status->wptr);
            else
                len = fifo->gdepth - status->usage;
            len = min(len, lensw);
			dptr = &((uint8_t*)fifo->buffer)[status->wptr];
            memcpy( dptr, ptr, len );
            status->wptr += len;
            if ( status->wptr == fifo->gdepth )
                status->wptr = 0;
            ptr += len;
            status->usage += len;
            lensw -= len;
        }
    }
	mwmr_unlock( &status->lock );
}

void mwmr_initialize_pointer (mwmr_t *p_mwmr_t, unsigned int width, unsigned int depth, uint32_t *const buffer) {
    p_mwmr_t->width         = width;
    p_mwmr_t->gdepth        = width*depth;
    p_mwmr_t->buffer        = buffer;
    p_mwmr_t->status.rptr  = 0;
    p_mwmr_t->status.wptr  = 0;
    p_mwmr_t->status.usage = 0;
}

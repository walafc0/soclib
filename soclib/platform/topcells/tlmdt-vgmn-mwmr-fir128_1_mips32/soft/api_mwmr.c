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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors : Ivan MIRO-PANADES
 * 
 * History :
 *
 * Comment : Based on : 
 *             soclib/runtime_netlist/mwmr/soft/mwmr.c
 *             mutekh/libmwmr/mwmr_soclib.c
 *
 */

#include "api_mwmr.h"
#include "soclib_io.h"
#include "soclib/mwmr_controller.h"

static void *memcpy( void *_dst, const void *_src, unsigned long size )
{
	uint32_t *dst = _dst;
	const uint32_t *src = _src;
	if ( ! ((uint32_t)dst & 3) && ! ((uint32_t)src & 3) )
		while (size > 3) {
			*dst++ = *src++;
			size -= 4;
		}

	unsigned char *cdst = (unsigned char*)dst;
	const unsigned char *csrc = (unsigned char*)src;

	while (size--) {
		*cdst++ = *csrc++;
	}
	return _dst;
}

static inline void mwmr_lock( volatile uint32_t *lock )
{
	while (*lock != 0);
}
static inline void mwmr_unlock( volatile uint32_t *lock )
{
	*lock = 0;
}
static inline uint32_t mwmr_try_lock( volatile uint32_t *lock )
{
	return (*lock);
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

typedef struct {
	uint32_t usage, wptr, rptr, modified;
} local_mwmr_status_t;

static inline void rehash_status( mwmr_t *fifo, local_mwmr_status_t *status )
{
	volatile soclib_mwmr_status_s *fstatus = &fifo->status;
	status->usage = fstatus->usage;
	status->wptr  = fstatus->wptr;
	status->rptr  = fstatus->rptr;
	status->modified = 0;
}
static inline void writeback_status( mwmr_t *fifo, local_mwmr_status_t *status )
{
	volatile soclib_mwmr_status_s *fstatus = &fifo->status;
	if ( !status->modified )
		return;
	fstatus->usage = status->usage;
	fstatus->wptr  = status->wptr;
	fstatus->rptr  = status->rptr;
}


void
mwmr_hw_init( void *coproc, enum SoclibMwmrWay way,
			  unsigned int no, const mwmr_t *mwmr )
{
	soclib_io_set( coproc, MWMR_CONFIG_FIFO_WAY, way );
	soclib_io_set( coproc, MWMR_CONFIG_FIFO_NO, no );
	soclib_io_set( coproc, MWMR_CONFIG_STATUS_ADDR, (uint32_t)&mwmr->status );
	soclib_io_set( coproc, MWMR_CONFIG_WIDTH, mwmr->width );
	soclib_io_set( coproc, MWMR_CONFIG_DEPTH, mwmr->gdepth );
	soclib_io_set( coproc, MWMR_CONFIG_BUFFER_ADDR, (uint32_t)mwmr->buffer );
	soclib_io_set( coproc, MWMR_CONFIG_LOCK_ADDR, (uint32_t)mwmr->lock );
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
    local_mwmr_status_t status;
	mwmr_lock( fifo->lock );
        ///printf("Je suis las0\n");
	rehash_status( fifo, &status );
         ///printf("status8.usage = %d\n", status.usage);
         ///printf("fifoi8->width = %d\n", fifo->width);

    while ( lensw ) {
        size_t len;
        while (status.usage < fifo->width) {
            //printf("status.usage = %d\n", status.usage);
            //printf("fifo->width = %d\n", fifo->width);
	    writeback_status( fifo, &status );
            //printf("status.usage1 = %d\n", status.usage);
            //printf("fifo->width1 = %d\n", fifo->width);
            ///printf("Je suis las1\n");
            mwmr_unlock( fifo->lock );
	    busy_wait(1000);
            mwmr_lock( fifo->lock );
            rehash_status( fifo, &status );
            ///printf("Je suis las2\n");
        }
        while ( lensw && status.usage >= fifo->width ) {
			void *sptr;

            if ( status.rptr < status.wptr )
                len = status.usage;
            else
                len = (fifo->gdepth - status.rptr);
            ///printf("Je suis las1\n");
	    len = min(len, lensw);
	    sptr = &((uint8_t*)fifo->buffer)[status.rptr];
            memcpy( ptr, sptr, len );
            status.rptr += len;
            if ( status.rptr == fifo->gdepth )
                status.rptr = 0;
            ptr += len;
            status.usage -= len;
            lensw -= len;
            status.modified = 1;
	    ///printf("Je suis las2\n");
        }
    }
	///printf("Je suis las5\n");
	writeback_status( fifo, &status );
	mwmr_unlock( fifo->lock );
}

void mwmr_write( mwmr_t *fifo, const void *_ptr, size_t lensw )
{
	const uint8_t *ptr = _ptr;
    local_mwmr_status_t status;
	mwmr_lock( fifo->lock );
	rehash_status( fifo, &status );
    while ( lensw ) {
        size_t len;
        while (status.usage >= fifo->gdepth) {
               ///printf("statusi2.usage = %d\n", status.usage);
               ///printf("fifo2->width = %d\n", fifo->width);
	        writeback_status( fifo, &status );
               ///printf("status3.usage = %d\n", status.usage);
               ///printf("fifo3->width = %d\n", fifo->width);
            mwmr_unlock( fifo->lock );
			busy_wait(1000);
            mwmr_lock( fifo->lock );
            rehash_status( fifo, &status );
        }
        while ( lensw && status.usage < fifo->gdepth ) {
			void *dptr;

            if ( status.rptr <= status.wptr )
                len = (fifo->gdepth - status.wptr);
            else
                len = fifo->gdepth - status.usage;
            len = min(len, lensw);
			dptr = &((uint8_t*)fifo->buffer)[status.wptr];
            memcpy( dptr, ptr, len );
            status.wptr += len;
            if ( status.wptr == fifo->gdepth )
                status.wptr = 0;
            ptr += len;
            status.usage += len;
            lensw -= len;
            status.modified = 1;
            ///printf("status4.usage = %d\n", status.usage);
            ///printf("fifo4->width = %d\n", fifo->width);
        }
    }
	writeback_status( fifo, &status );
        ///printf("status5.usage = %d\n", status.usage);
        ///printf("fifo5->width = %d\n", fifo->width);
	mwmr_unlock( fifo->lock );
}

size_t mwmr_try_read( mwmr_t *fifo, void *_ptr, size_t lensw )
{
	uint8_t *ptr = _ptr;
	size_t done = 0;
    local_mwmr_status_t status;
	if ( mwmr_try_lock( fifo->lock ) )
		return done;
	rehash_status( fifo, &status );
	while ( lensw && status.usage >= fifo->width ) {
        size_t len;
		void *sptr;

		if ( status.rptr < status.wptr )
			len = status.usage;
		else
			len = (fifo->gdepth - status.rptr);
		len = min(len, lensw);
		sptr = &((uint8_t*)fifo->buffer)[status.rptr];
		memcpy( ptr, sptr, len );
		status.rptr += len;
		if ( status.rptr == fifo->gdepth )
			status.rptr = 0;
		ptr += len;
		status.usage -= len;
		lensw -= len;
		done += len;
		status.modified = 1;
	}
	writeback_status( fifo, &status );
	mwmr_unlock( fifo->lock );
	return done;
}


size_t mwmr_try_write( mwmr_t *fifo, const void *_ptr, size_t lensw )
{
	const uint8_t *ptr = _ptr;
	size_t done = 0;
    local_mwmr_status_t status;
	if (mwmr_try_lock( fifo->lock ) )
		return done;
	rehash_status( fifo, &status );
	while ( lensw && status.usage < fifo->gdepth ) {
        size_t len;
		void *dptr;

		if ( status.rptr <= status.wptr )
			len = (fifo->gdepth - status.wptr);
		else
			len = fifo->gdepth - status.usage;
		len = min(len, lensw);
		dptr = &((uint8_t*)fifo->buffer)[status.wptr];
		memcpy( dptr, ptr, len );
		status.wptr += len;
		if ( status.wptr == fifo->gdepth )
			status.wptr = 0;
		ptr += len;
		status.usage += len;
		lensw -= len;
		done += len;
		status.modified = 1;
    }
	writeback_status( fifo, &status );
	mwmr_unlock( fifo->lock );
	return done;
}

void mwmr_initialize_pointer (mwmr_t *p_mwmr_t, unsigned int width, unsigned int depth, uint32_t *const buffer, uint32_t *lock) {
    p_mwmr_t->width         = width;
    p_mwmr_t->gdepth        = width*depth;
    p_mwmr_t->buffer        = buffer;
    p_mwmr_t->lock          = lock;
    p_mwmr_t->status.rptr  = 0;
    p_mwmr_t->status.wptr  = 0;
    p_mwmr_t->status.usage = 0;
    p_mwmr_t->status.lock  = 0;
}

void mwmr_wait_fifo_empty( void *coproc, enum SoclibMwmrWay way, unsigned int no, mwmr_t *fifo )
{
    //Wait until the RAM fifo is empty
    local_mwmr_status_t status;
	mwmr_lock( fifo->lock );
	rehash_status( fifo, &status );
    while (status.usage > 0) {
        mwmr_unlock( fifo->lock );
        busy_wait(1000);
        mwmr_lock( fifo->lock );
        rehash_status( fifo, &status );
    }
    mwmr_unlock( fifo->lock );
    //Wait untill all the data is consumed by the coprocessor
    int value;
    do {
    soclib_io_set( coproc, MWMR_CONFIG_FIFO_WAY, way );
	soclib_io_set( coproc, MWMR_CONFIG_FIFO_NO, no );
	value = soclib_io_get( coproc, /**MWMR_FIFO_STATUS**/MWMR_FIFO_FILL_STATUS );
    } while (value > 0);
}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

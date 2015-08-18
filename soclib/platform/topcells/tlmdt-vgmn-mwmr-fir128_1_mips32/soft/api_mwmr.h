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
 *             soclib/runtime_netlist/mwmr/soft/mwmr.h
 *             mutekh/libmwmr/include/mwmr.h
 *
 */

#ifndef API_MWMR_H
#define API_MWMR_H

#include "stdint.h"
#include "soclib/mwmr_controller.h"

typedef struct mwmr_s {
    unsigned int width;
	unsigned int gdepth;
    uint32_t *buffer;
	uint32_t *lock;
	volatile soclib_mwmr_status_s status;
} mwmr_t;


#define MWMR_INITIALIZER(width, depth, data, lock)						   \
	{ width, width*depth, data, (lock), SOCLIB_MWMR_STATUS_INITIALIZER }

void
//Initialize the MWMR controller
mwmr_hw_init( void *coproc, enum SoclibMwmrWay way, unsigned int no, const mwmr_t *mwmr );

//Initialize a mwmr_t structure
void mwmr_initialize_pointer (mwmr_t *p_mwmr_t, unsigned int width, unsigned int depth, uint32_t *const buffer, uint32_t *lock);

//Wait until the RAM FIFO and the MWMR FIFO is empty
void mwmr_wait_fifo_empty( void *coproc, enum SoclibMwmrWay way, unsigned int no, mwmr_t *fifo );

//Write on the configuration registers
void mwmr_config( void *coproc, unsigned int no, const uint32_t val );

//Read from status registers
uint32_t mwmr_status( void *coproc, unsigned int no );

//Blocking Write and Read functions. Size is defined in bytes.
void mwmr_write( mwmr_t *mwmr, const void *buffer, size_t size );
void mwmr_read( mwmr_t *mwmr, void *buffer, size_t size );
//Non-Blocking Write and Read functions. Size is defined in bytes. Retun value is the number of written bytes
size_t mwmr_try_read( mwmr_t *fifo, void *_ptr, size_t lensw );
size_t mwmr_try_write( mwmr_t *fifo, const void *_ptr, size_t lensw );

#endif /* API_MWMR_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

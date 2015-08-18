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
 * Based on previous works by Etienne Faure & Alain Greiner, 2005
 *
 * Maintainers: nipo
 */
#ifndef MWMR_CONTROLLERLF_REGISTERS_H
#define MWMR_CONTROLLERLF_REGISTERS_H

enum SoclibMwmrRegisters {
    MWMR_IOREG_MAX = 16,
    MWMR_RESET = MWMR_IOREG_MAX,
    MWMR_CONFIG_FIFO_WAY,
    MWMR_CONFIG_FIFO_NO,
    MWMR_CONFIG_STATUS_ADDR,
    MWMR_CONFIG_DEPTH, // bytes
    MWMR_CONFIG_BUFFER_ADDR,
    MWMR_CONFIG_RUNNING,
    MWMR_CONFIG_WIDTH, // bytes
    MWMR_CONFIG_ENDIANNESS, // Write 0x11223344 here
    MWMR_FIFO_FILL_STATUS,
};

enum SoclibMwmrWay {
    MWMR_TO_COPROC,
    MWMR_FROM_COPROC,
};

typedef struct
{
	uint32_t free_tail; // bytes
	uint32_t free_head; // bytes
	uint32_t free_size; // bytes

	uint32_t data_tail; // bytes
	uint32_t data_head; // bytes
	uint32_t data_size; // bytes
} soclib_mwmr_status_s;

#define SOCLIB_MWMR_STATUS_INITIALIZER(w, d) {0,0,(w*d),0,0,0}

#endif /* MWMR_CONTROLLERLF_REGISTERS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


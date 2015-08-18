/* -*- c++ -*-
 *
 * DSX is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * DSX is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DSX; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Copyright (c) UPMC, Lip6, Asim
 */


#ifndef SRL_HW_HELPERS_H
#define SRL_HW_HELPERS_H

#include <sys/time.h>

namespace dsx { namespace caba {

#define BASE_ADDR_OF(id) 0
#define srl_dcache_flush_addr(n) do{}while(0)
#define srl_dcache_flush_zone(n, m) do{}while(0)
#define srl_mwmr_config(base, no, val) do{}while(0)
#define srl_mwmr_status(base, no)      do{}while(0)

#define srl_sleep_cycles( n ) do{}while(0)


}}

#endif

/* -*- c++ -*-
 *
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
 * Maintainers: fpecheux, alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <limits>
#include <tlmdt>	          

#define MAX_SIZE (1 << 9)      //2^9 = 512

#if defined(__APPLE__)
# include <libkern/OSAtomic.h>
# define ATOMIC_ADD(addr, val) OSAtomicAdd32(val, addr)
# define ATOMIC_SUB(addr, val) OSAtomicAdd32(-val, addr)
#else
# define ATOMIC_ADD __sync_fetch_and_add
# define ATOMIC_SUB __sync_fetch_and_sub
#endif

namespace soclib { namespace tlmdt {

class circular_buffer
{
private:

  struct transaction{
    tlm::tlm_generic_payload *payload;
    tlm::tlm_phase           *phase;
    sc_core::sc_time         *time;
  };

  transaction            *buffer;
  int                     front;
  int                     rear;
  volatile int            buffer_length;
  int                     max_size;
  std::string             name;

public:
  circular_buffer(int n);

  circular_buffer();

  ~circular_buffer();

  bool push
  ( tlm::tlm_generic_payload &payload,
    tlm::tlm_phase           &phase,
    sc_core::sc_time         &time);

  bool pop
  ( tlm::tlm_generic_payload *&payload,
    tlm::tlm_phase           *&phase,
    sc_core::sc_time         *&time);

  bool get_front
  ( tlm::tlm_generic_payload *&payload,
    tlm::tlm_phase           *&phase,
    sc_core::sc_time         *&time);

  inline const bool is_empty() { return (buffer_length == 0); }

  inline const bool is_full()  { return (buffer_length == max_size); }
                                   
  inline const int get_size() { return buffer_length; }

  inline const int get_max_size() { return max_size; }

  void set_name(std::string n);

};

}}

#endif

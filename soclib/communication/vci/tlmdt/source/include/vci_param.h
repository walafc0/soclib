/* -*- mode: c++; coding: utf-8 -*-
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
 * Maintainers: fpecheux, nipo
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Nicolas Pouillon <nipo@ssji.net>
 */

#ifndef SOCLIB_TLMT_VCI_PARAM_H
#define SOCLIB_TLMT_VCI_PARAM_H

#include "static_fast_int.h"

namespace soclib { namespace tlmdt {

template<typename _addr_t, typename _data_t>
class VciParams
{
public:
  typedef _addr_t addr_t;
  typedef _data_t data_t;
  static const int nbytes = sizeof(data_t);
  typedef typename ::soclib::common::fast_int_t<nbytes*8>::int_t fast_data_t;

  static inline data_t be2mask( data_t be )
  {
    const data_t m = (1<<nbytes);
    data_t r = 0;
    
    for ( int i=0; i<nbytes; ++i ) {
      r <<= 8;
      be <<= 1;
      if ( be & m )
	r |= 0xff;
    }
    return r;
  }

};

}}

#endif

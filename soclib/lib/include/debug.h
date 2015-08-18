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
 * Copyright (c) UPMC, Lip6, Asim
 *
 * Maintainers: kane
 */
#ifndef SOCLIB_DEBUG_H_
#define SOCLIB_DEBUG_H_

#include <cassert>
#include <stdio.h>

#define PRINTF_COND(cond,msg...) do { if (cond) printf(msg); } while (0); 

#ifdef ASSERT_VERBOSE

# ifndef ASSERT_NCYCLES
# error "ASSERT_NCYCLES undefine"
# endif

# define ASSERT(cond,msg)                                   \
    do {                                                    \
      if (not (cond))                                       \
          {                                                 \
              printf("%d : %s\n",ASSERT_NCYCLES,msg);       \
              assert(false);                                \
          }                                                 \
  } while (0)
#else
# define ASSERT(cond,msg) assert ((cond) and msg);
#endif

#endif /* SOCLIB_DEBUG_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


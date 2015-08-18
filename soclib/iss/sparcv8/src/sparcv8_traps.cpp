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
 * Copyright (c) Telecom ParisTech
 *         Alexis Polti <polti@telecom-paristech.fr>
 *
 * Maintainers: Alexis Polti
 *
 * $Id$
 */

#include "sparcv8.h"

namespace soclib { namespace common {

#define tmpl(x) template<unsigned int NWINDOWS> x Sparcv8Iss<NWINDOWS>

  tmpl(std::string)::GetTrapName(int type)
  {
    switch (type) {
#define rstr(x) case x: return #x
      rstr(TP_RESET);
      rstr(TP_INSTRUCTION_ACCESS_MMU_MISS);
      rstr(TP_INSTRUCTION_ACCESS_ERROR);
      rstr(TP_INSTRUCTION_ACCESS_EXCEPTION);
      rstr(TP_PRIVILEGED_INSTRUCTION);
      rstr(TP_ILLEGAL_INSTRUCTION);
      rstr(TP_FP_DISABLED);
      rstr(TP_CP_DISABLED);
      rstr(TP_WINDOW_OVERFLOW);
      rstr(TP_WINDOW_UNDERFLOW);
      rstr(TP_MEM_ADDRESS_NOT_ALIGNED);
      rstr(TP_FP_EXCEPTION);
      rstr(TP_CP_EXCEPTION);
      rstr(TP_DATA_ACCESS_ERROR);
      rstr(TP_DATA_ACCESS_MMU_MISS);
      rstr(TP_DATA_ACCESS_EXCEPTION);
      rstr(TP_TAG_OVERFLOW);
      rstr(TP_DIVISION_BY_ZERO);
      rstr(TP_UNIMPLEMENTED_INSTRUCTION);
#undef rstr
    }
    
    if ((type & 0x10) == 0x10) {
      std::string name = "TP_INTERRUPT_LEVEL";
      name += (type & 0xf) + '0';
      return name;
    }

    if ((type & 0x80) == 0x80) 
      return "TP_TRAP_INSTRUCTION";

    return "Unknown trap!!!";
  }
}}


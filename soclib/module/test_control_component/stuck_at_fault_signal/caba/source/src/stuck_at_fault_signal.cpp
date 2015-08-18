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
 *         Dimitri Refauvelet <dimitri.refauvelet@lip6.fr>, 2009
 *
 * Maintainers: dimitri.refauvelet@etu.upmc.fr
 */

#include "stuck_at_fault_signal.h"

namespace soclib{
  namespace caba{

#define tmpl(x) template<typename signal_type> x StuckAtFaultSignal<signal_type>

    tmpl(/**/)::StuckAtFaultSignal(sc_module_name insname,signal_type high, signal_type low)
	       :caba::BaseModule(insname),
	       p_in("in"),
	       p_out("out")
    {
      SC_METHOD(genMoore);
      dont_initialize();
      sensitive << p_in;
      
      highFault = high;
      lowFault = low;
    }
    
    tmpl(/**/)::~StuckAtFaultSignal()
    {
    }
    
    tmpl(void)::genMoore()
    {
      tmp=p_in.read();
      tmp=tmp|highFault;
      tmp=tmp&(~lowFault);
      p_out=tmp;
    }
  }
}

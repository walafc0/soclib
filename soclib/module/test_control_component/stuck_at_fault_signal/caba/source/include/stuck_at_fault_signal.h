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
 * Copyright (c) UPMC, Lip6
 *          Dimitri Refauvelet <dimitri.refauvelet@lip6.fr>, 2009
 *
 *Maintainers: dimitri.refauvelet@etu.upmc.fr
 */
#ifndef SOCLIB_CABA_STUCK_AT_FAULT_SIGNAL_H
#define SOCLIB_CABA_STUCK_AT_FAULT_SIGNAL_H

#include <systemc>
#include "caba_base_module.h"

namespace soclib {
  namespace caba {
    
    using namespace sc_core;
    
    template<typename signal_type>
    class StuckAtFaultSignal
      : public soclib::caba::BaseModule
    {      
    
    private:
      signal_type tmp;
      signal_type highFault, lowFault;
      
      void genMoore();
    
    protected:
      SC_HAS_PROCESS(StuckAtFaultSignal);
      
    public:
      sc_in<signal_type> p_in;
      sc_out<signal_type> p_out;
      
      StuckAtFaultSignal(sc_module_name insname,signal_type high, signal_type low);
      ~StuckAtFaultSignal();
    };
  }
}

#endif /* SOCLIB_CABA_STUCK_AT_FAULT_SIGNAL_H */

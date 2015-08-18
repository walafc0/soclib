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
 *     Dimitri Refauvelet <dimitri.refauvelet@lip6.fr>     
 *
 * Maintainers: dimitri.refauvelet@etu.upmc.fr
 */

#ifndef SOCLIB_CABA_BLACKHOLE_H
#define SOCLIB_CABA_BLACKHOLE_H

#include <systemc>
#include "caba_base_module.h"
#include "vci_target.h"

namespace soclib {
  namespace caba {
    
    using namespace sc_core;
    
    template<typename vci_param>
    class VciBlackhole
      : public soclib::caba::BaseModule
    {
      
    protected:
      SC_HAS_PROCESS(VciBlackhole);
      
    public:
      sc_in<bool> p_resetn;
      sc_in<bool> p_clk;
      soclib::caba::VciTarget<vci_param> p_vci;
      
      VciBlackhole(
		   sc_module_name insname);
      ~VciBlackhole();

    private:
      void genMoore();
    };
    
  }}

#endif /* SOCLIB_CABA_BLACKHOLE_H */

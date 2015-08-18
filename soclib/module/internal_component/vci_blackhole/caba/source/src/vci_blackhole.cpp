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

#include "vci_blackhole.h"

namespace soclib {
  namespace caba {

    using namespace soclib;

#define tmpl(x) template<typename vci_param> x VciBlackhole<vci_param>
    
    tmpl(/**/)::VciBlackhole(
			     sc_module_name insname
			     )
	       : caba::BaseModule(insname),
	       p_resetn("resetn"),
	       p_clk("clk"),
	       p_vci("vci")
    {
      SC_METHOD(genMoore);
      dont_initialize();
      sensitive << p_clk.neg();
      
    }
    
    tmpl(/**/)::~VciBlackhole()
    {
    }
    
    tmpl(void)::genMoore()
    {
      p_vci.setAck(true);
    }
    
  }}



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
 * WITHOUT ANY WARRANTY; without even the tlmtied warranty of
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
 * Copyright (c) TurboConcept -- 2008
 * Christophe Cunat <Christophe.Cunat@turboconcept.com>
 *
 */
 
#include <stdlib.h>
#include "base_module.h"
#include "register.h"
#include "../include/tc4200.h"


namespace soclib {
namespace tlmt {


#define tmpl(x) template <typename vci_param> x tc4200<vci_param>

  /* Constructor */
  tmpl()::tc4200(sc_module_name   insname) //Instance name
    : soclib::tlmt::BaseModule(insname), 
      m_segment(mt.getSegment(index)),
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci")
  {
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
    
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
    portRegister("clk", p_clk);
    portRegister("resetn", p_resetn);
    portRegister("vci", p_vci);

  }

  /* Transition function */
  tmpl(void)::transition()
  {
    return;
  }

  /* Moore fsm generation */
  tmpl(void)::genMoore()
  {
    return;
  }


}}


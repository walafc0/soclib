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

#ifndef SOCLIB_TLMT_TC4200_H
#define SOCLIB_TLMT_TC4200_H

#include <systemc.h>
#include "tlmt_base_module.h"


namespace soclib {
  namespace tlmt {
    
    using namespace sc_core;
    
template <typename vci_param> class tc4200
      : public soclib::tlmt::BaseModule 
    {
    public:
      sc_in<bool>                        p_clk;
      sc_in<bool>                        p_resetn;
      soclib::tlmt::VciTarget<vci_param> p_vci; 

    protected: 
      SC_HAS_PROCESS(tc4200);

    public:
      tc4200(sc_modul_name insname);

    private: 
      void transition();
      void genMoore();
};

  }}               




#endif /* SOCLIB_TLMT_TC4200_H */

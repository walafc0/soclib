/*
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
 * Copyright (c) TurboConcept, 2010
 *         Christophe Cunat <Christophe.Cunat@TurboConcept.com> 
 *
 * Maintainers: Cunat
 */

#ifndef SOCLIB_CABA_DEMODULATOR_H
#define SOCLIB_CABA_DEMODULATOR_H

#include <systemc.h>
#include "caba_base_module.h"
#include "mapping_table.h"
#include "vci_target.h"
#include "vci_xcache_wrapper.h"
#include "vci_target_fsm.h"
#include "mips32.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_vgmn.h"
#include "tc4200.h"

namespace soclib {
  namespace caba {  

    using namespace sc_core;
    using soclib::common::IntTab;
    using soclib::common::Segment;
    typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;
    typedef soclib::common::Mips32ElIss iss_t;
    
    class Demodulator 
      : public caba::BaseModule
    {
    public: 
      sc_in<bool>         p_clk; 
      sc_in<bool>         p_resetn;
      
    private:

      sc_signal<bool> m_signal_mips0_it0;
      sc_signal<bool> m_signal_mips0_it1;
      sc_signal<bool> m_signal_mips0_it2;
      sc_signal<bool> m_signal_mips0_it3;
      sc_signal<bool> m_signal_mips0_it4;
      sc_signal<bool> m_signal_mips0_it5;
      
      soclib::caba::VciSignals<vci_param> m_signal_vci_m0; 
      soclib::caba::VciSignals<vci_param> m_signal_vci_tty; 
      soclib::caba::VciSignals<vci_param> m_signal_vci_vcimultiram0; 
      soclib::caba::VciSignals<vci_param> m_signal_vci_vcimultiram1; 
      soclib::caba::VciSignals<vci_param> m_signal_vci_tc4200; 
      
      sc_signal<bool> m_signal_tty_irq0; 
      

      soclib::common::Loader m_loader;


      soclib::common::MappingTable *m_maptab;
      soclib::caba::VciXcacheWrapper<vci_param, iss_t > *m_mips0;

      soclib::caba::VciRam<vci_param> *m_ram0;
      soclib::caba::VciRam<vci_param> *m_ram1;
      soclib::caba::VciMultiTty<vci_param> *m_tty;
      
      soclib::caba::Tc4200<vci_param> *m_tc4200;

      soclib::caba::VciVgmn<vci_param> *m_vgmn;

    protected: 
      SC_HAS_PROCESS(Demodulator);

    public: 
      Demodulator(sc_module_name insname, 
                  const std::string &soft_filename); 

      ~Demodulator();

    };      
  }
}
      


#endif /* SOCLIB_CABA_DEMODULATOR_H */


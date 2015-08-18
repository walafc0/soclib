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

#include <iostream>
#include <cstdlib>


#include "base_module.h"
#include "demodulator.h"
#include "segmentation.h"

namespace soclib{
  namespace caba {

#define tmpl(x) x Demodulator

    using namespace sc_core; 
    using soclib::common::IntTab;

    typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;
    using namespace soclib; 
    using namespace caba; 

    /* Constructor */
    tmpl(/**/)::Demodulator(sc_module_name insname, //Instance name
                            const std::string &soft_filename)
      : soclib::caba::BaseModule(insname), 
        p_clk("clk"), 
        p_resetn("resetn"), 
        m_signal_mips0_it0("signal_mips0_it0"),
        m_signal_mips0_it1("signal_mips0_it1"),
        m_signal_mips0_it2("signal_mips0_it2"),
        m_signal_mips0_it3("signal_mips0_it3"),
        m_signal_mips0_it4("signal_mips0_it4"),
        m_signal_mips0_it5("signal_mips0_it5"),
        m_signal_vci_m0("signal_vci_m0"),
        m_signal_vci_tty("signal_vci_tty"),
        m_signal_vci_vcimultiram0("signal_vci_ram0"),
        m_signal_vci_vcimultiram1("signal_vci_ram1"),
        m_signal_vci_tc4200("signal_vci_tc4200"),
        m_signal_tty_irq0("signal_tty_irq0"),
        m_loader(soft_filename)
    {
      /*************************************/
      /* Instanciation for sub component   */
      /*************************************/
      m_maptab = new MappingTable(32, IntTab(8), IntTab(8), 0x00300000);
      m_maptab->add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
      m_maptab->add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
      m_maptab->add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  
      m_maptab->add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
  
      m_maptab->add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));

      // segment(name, base_address, size, target_index, cacheability, initiator=false)
      m_maptab->add(Segment("tc4200", TC4200_BASE, TC4200_SIZE, IntTab(3), false)); 
      
      
      m_mips0  = new (VciXcacheWrapper<vci_param, iss_t >)("mips0", 0, *m_maptab, IntTab(0), 1, 8, 4, 1, 8, 4);
      m_ram0   = new (VciRam<vci_param>)("vcimultiram0", IntTab(0), *m_maptab, m_loader);
      m_ram1   = new (VciRam<vci_param>)("vcimultiram1", IntTab(1), *m_maptab, m_loader);
      m_tty    = new (VciMultiTty<vci_param>)("vcitty", IntTab(2), *m_maptab, "vcitty0", NULL);
      m_tc4200 = new (Tc4200<vci_param>)("tc4200", IntTab(3), *m_maptab);
      m_vgmn   = new (VciVgmn<vci_param>)("vgmn", *m_maptab, 1, 4, 2, 8);
      

      

      /***********************************/
      /*      Net-list                   */
      /***********************************/
      m_mips0->p_clk(p_clk);  
      m_ram0->p_clk(p_clk);
      m_ram1->p_clk(p_clk);
      
      
      m_mips0->p_resetn(p_resetn);  
      m_ram0->p_resetn(p_resetn);
      m_ram1->p_resetn(p_resetn);

      /* Processor management */
      m_mips0->p_irq[0](m_signal_mips0_it0); 
      m_mips0->p_irq[1](m_signal_mips0_it1); 
      m_mips0->p_irq[2](m_signal_mips0_it2); 
      m_mips0->p_irq[3](m_signal_mips0_it3); 
      m_mips0->p_irq[4](m_signal_mips0_it4); 
      m_mips0->p_irq[5](m_signal_mips0_it5); 

      /* Processor cache */
      m_mips0->p_vci(m_signal_vci_m0);
      
      m_ram0->p_vci(m_signal_vci_vcimultiram0);
      
      m_ram1->p_vci(m_signal_vci_vcimultiram1);
      
      m_tty->p_clk(p_clk);
      m_tty->p_resetn(p_resetn);
      m_tty->p_vci(m_signal_vci_tty);
      m_tty->p_irq[0](m_signal_tty_irq0); 
      
      m_tc4200->p_clk(p_clk);
      m_tc4200->p_resetn(p_resetn);
      m_tc4200->p_vci(m_signal_vci_tc4200);
      
      m_vgmn->p_clk(p_clk);
      m_vgmn->p_resetn(p_resetn);
      
      m_vgmn->p_to_initiator[0](m_signal_vci_m0);
      
      m_vgmn->p_to_target[0](m_signal_vci_vcimultiram0);
      m_vgmn->p_to_target[1](m_signal_vci_vcimultiram1);
      m_vgmn->p_to_target[2](m_signal_vci_tty);
      m_vgmn->p_to_target[3](m_signal_vci_tc4200);
      


    }

    /* Destructor */
    tmpl(/**/)::~Demodulator()
    {
      delete m_vgmn; 
      delete m_tc4200;
      delete m_tty;
      delete m_ram1;
      delete m_ram0;
      delete m_mips0;
      delete m_maptab;
    }

  }
}


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
 * WITHOUT ANY WARRANTY; without even the cabaied warranty of
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
 *
 * Maintainer: C. Cunat
 *
 */

#ifndef SOCLIB_CABA_TC4200_H
#define SOCLIB_CABA_TC4200_H

#include <systemc.h>
#include "caba_base_module.h"
#include "vci_target.h"
#include "mapping_table.h"
#include "vci_target_fsm.h"


#include "tc_tc4200.h"


namespace soclib {
  namespace caba {
    
    using namespace sc_core;


    
template <typename vci_param> class Tc4200
      : public caba::BaseModule 
    {


    public:
      sc_in<bool>                p_clk;
      sc_in<bool>                p_resetn;
      caba::VciTarget<vci_param> p_vci; 

    private: 
      caba::VciTargetFsm<vci_param, true> m_vci_fsm;
      const soclib::common::Segment       m_segment;
      caba::Tc_Tc4200                     m_decoder_hw; 

      struct Config {
        unsigned char expansion_factor_index : 5;
        unsigned char                        : 0; //Force alignement
        unsigned char rate                   : 3;
        unsigned char itmax                  : 8;
        unsigned char itstop                 : 1; 
      } m_config; 
      
      struct Monitor{
        unsigned char idle                   : 1;
        unsigned char                        : 0;
        unsigned char syndok                 : 1;
        unsigned char                        : 0;
        unsigned char itdone                 : 8;
      } r_monitor;
      
      int m_nb_data_expected;
      int m_nb_data_received;
      int m_nb_data_send_expected;
      int m_nb_data_sent;
      int m_input_buffer[2304]; 
      
      bool r_pvci_cmdack; 
      bool r_qrdy;

      bool on_write(int seg, 
                    typename vci_param::addr_t addr,
                    typename vci_param::data_t data,
                    int be);

      bool on_read(int seg,
                   typename vci_param::addr_t addr, 
                   typename vci_param::data_t &data);

      void transition();
      void genMoore();

      sc_signal<bool>          m_itstop;
      sc_signal<sc_uint<8> >   m_itmax;
      sc_signal<sc_uint<3> >   m_rate;
      sc_signal<sc_uint<5> >   m_exp_fact_index;
                          
      sc_signal<bool>          m_qrdy;
                        
      sc_signal<bool>          m_dblk;
      sc_signal<bool>          m_den;
      sc_signal<bool>          m_dlast;
      sc_signal<sc_uint<20> >  m_d;
                          
      sc_signal<bool>         m_syndok;
      sc_signal<bool>         m_idle;
      sc_signal<sc_uint<8> >  m_itdone;
                       
      sc_signal<bool>         m_qblk;
      sc_signal<bool>         m_qen; 
      sc_signal<bool>         m_qlast; 
      sc_signal<sc_uint<16> > m_q;
      
      sc_signal<bool>         m_drdy;

      bool r_den;
      bool r_dblk;
      sc_uint<20> r_d;
      bool r_dlast;
      
      // Management for q latency. 
      sc_uint<16>  r_q[16];
      int          r_q_beg;
      int          r_q_end;

#ifdef SOCLIB_MODULE_DEBUG
      unsigned int r_cnt; 
#endif /* SOCLIB_MODULE_DEBUG */


    protected: 
      SC_HAS_PROCESS(Tc4200);

    public:
      Tc4200(sc_module_name insname, 
             const IntTab &index, 
             const MappingTable &mt);

      ~Tc4200();
};


    enum SoclibTc4200Registers {
      TC4200_CONFIG,
      TC4200_MONITOR, 
      TC4200_D_IN_FIRST,
      TC4200_D_IN, 
      TC4200_D_OUT
    };


  }}               




#endif /* SOCLIB_CABA_TC4200_H */

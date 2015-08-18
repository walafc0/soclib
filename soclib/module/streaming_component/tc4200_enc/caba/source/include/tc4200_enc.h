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

#ifndef SOCLIB_CABA_TC4200_ENC_H
#define SOCLIB_CABA_TC4200_ENC_H

#include <systemc.h>
#include "caba_base_module.h"

#include "vci_target.h"
#include "mapping_table.h"
#include "vci_target_fsm.h"


//#define SOCLIB_MODULE_DEBUG

#include "tc_tc4200_enc.h"

namespace soclib {
  namespace caba {
    
    using namespace sc_core;
    
    template <typename vci_param> class Tc4200_enc
      : public soclib::caba::BaseModule 
    {
    public:
      sc_in<bool>                        p_clk;
      sc_in<bool>                        p_resetn;
      soclib::caba::VciTarget<vci_param> p_vci; 
      
    private: 
      caba::VciTargetFsm<vci_param, true> m_vci_fsm;
      const soclib::common::Segment       m_segment;
      caba::Tc_Tc4200_enc                 m_encoder_hw; 
  

      struct Config {
        unsigned char expansion_factor_index : 5;
        unsigned char                        : 0;
        unsigned char rate                   : 3;
      } m_config;
      
      bool r_idle;


      /* Vci wrapper: data buffer management */
      unsigned char m_input_buffer[480]; // Max payload size
      unsigned char m_output_buffer[2][576]; // Max encoded block size
      
      int r_index_input_buf;
      int r_index_output_buf;


      // input registers
      bool r_den;
      bool r_dblk;
      sc_uint<4> r_d;
      bool r_dlast;
      

      bool r_pvci_cmdack;
      bool r_qrdy;

      int  r_nb_data_in_expected;
      int  r_nb_data_in;   
      bool r_input_buf_filled;
      int  r_nb_data_out_expected[2];
      int  r_nb_data_out[2];
 
      
      bool on_write(int seg, 
                    typename vci_param::addr_t addr,
                    typename vci_param::data_t data,
                    int be);

      bool on_read(int seg,
                   typename vci_param::addr_t addr, 
                   typename vci_param::data_t &data);


      sc_signal<bool>        m_idle;

      sc_signal<bool>        m_dblk;
      sc_signal<bool>        m_den;
      sc_signal<bool>        m_dlast;
      sc_signal<sc_uint<4> > m_d;
      sc_signal<bool>        m_drdy;

      sc_signal<bool>        m_qblk;
      sc_signal<bool>        m_qen;
      sc_signal<bool>        m_qlast;
      sc_signal<sc_uint<4> > m_q;
      sc_signal<bool>        m_qrdy;
      
      sc_signal<sc_uint<3> > m_rate;
      sc_signal<sc_uint<5> > m_exp_fact_index;


#ifdef SOCLIB_MODULE_DEBUG
      unsigned int r_cnt; 
#endif /* SOCLIB_MODULE_DEBUG */


    protected: 
      SC_HAS_PROCESS(Tc4200_enc);

    public:
      Tc4200_enc(sc_module_name insname, 
                 const IntTab &index, 
                 const MappingTable &mt);
      ~Tc4200_enc();


    private: 
      void transition();
      void genMoore();
      
};

    enum SoclibTc4200EncRegisters {
      TC4200_ENC_CONFIG,
      TC4200_ENC_MONITOR,
      TC4200_ENC_D_IN_FIRST,
      TC4200_ENC_D_IN,
      TC4200_ENC_D_OUT
    };


  }}               




#endif /* SOCLIB_CABA_TC4200_ENC_H */

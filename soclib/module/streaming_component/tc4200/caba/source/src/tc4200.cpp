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
 
#include <stdlib.h>
#include "base_module.h"
#include "../include/tc4200.h"




#ifdef SOCLIB_MODULE_DEBUG
#include <iostream>
#include <iomanip>
using namespace std;
#endif /* SOCLIB_MODULE_DEBUG */

namespace soclib {
namespace caba {

#define tmpl(x) template <typename vci_param> x Tc4200<vci_param>

  /* Constructor */
  tmpl(/**/)::Tc4200(sc_module_name   insname, //Instance name
                     const IntTab &index, 
                     const MappingTable &mt)
    : soclib::caba::BaseModule(insname), 
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci"), 
      m_vci_fsm(p_vci, mt.getSegmentList(index)),
      m_segment(mt.getSegment(index)),
      m_decoder_hw("tc4200_hw")
  {
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
    
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

    // Connect decoder_hw ports
    m_decoder_hw.ck(p_clk);
    m_decoder_hw.arst_n(p_resetn);
    m_decoder_hw.itstop(m_itstop);
    m_decoder_hw.itmax(m_itmax);
    m_decoder_hw.rate(m_rate);
    m_decoder_hw.exp_fact_index(m_exp_fact_index);
    m_decoder_hw.qrdy(m_qrdy);          
    m_decoder_hw.dblk(m_dblk);          
    m_decoder_hw.den(m_den);           
    m_decoder_hw.dlast(m_dlast);         
    m_decoder_hw.d(m_d);             
    m_decoder_hw.syndok(m_syndok);        
    m_decoder_hw.idle(m_idle);          
    m_decoder_hw.itdone(m_itdone);        
    m_decoder_hw.qblk(m_qblk);          
    m_decoder_hw.qen(m_qen);           
    m_decoder_hw.qlast(m_qlast);         
    m_decoder_hw.q(m_q);             
    m_decoder_hw.drdy(m_drdy);          


    // Define vci_target_fsm read and write functions
    m_vci_fsm.on_read_write(on_read, on_write);

    printf("Successfull Instanciation of tc4200: %s\n", (const char*)insname);

    m_nb_data_expected = -2;
    m_nb_data_received = -1;
    
  }

  /*****************/
  /* VCI interface */
  /* Writing itf   */
  /*****************/
  tmpl(bool)::on_write(int seg, 
                       typename vci_param::addr_t addr, 
                       typename vci_param::data_t data, 
                       int be)
  {
    int cell = (int)addr / vci_param::B;
    
    int expansion_factor = 24 + 4 * m_config.expansion_factor_index;
    int input_data;
    

    switch ((enum SoclibTc4200Registers)cell)
      {
        case TC4200_CONFIG: 
          m_config.expansion_factor_index = (unsigned char)data;
          m_config.rate                   = (unsigned char)(data >> 8);
          m_config.itmax                  = (unsigned char)(data >> 16);
          m_config.itstop                 = (unsigned char)(data >> 24);
#ifdef SOCLIB_MODULE_DEBUG
          printf("New config : %d %d %d %d\n", 
                 m_config.expansion_factor_index, 
                 m_config.rate,
                 m_config.itmax,
                 m_config.itstop);
#endif /* SOCLIB_MODULE_DEBUG */ 
          return true;

        case TC4200_D_IN_FIRST:
          /* First data of a new block */
          // 1 data holds 4 input data for the tc4200 hardware decoder 
          m_nb_data_expected      = 24 * expansion_factor / 4; 
          m_nb_data_send_expected = 24 * expansion_factor / 16; 
          m_nb_data_sent          = 0;
          input_data              = data;
          m_input_buffer[0]       = input_data;
          r_den  = true;
          r_dblk = true;
          
          r_d = ((((data >> 24) & 0x1f) << 15) +
                 (((data >> 16) & 0x1f) << 10) + 
                 (((data >> 8) & 0x1f) << 5) + 
                 (((data >> 0) & 0x1f) << 0));
          
          
          m_nb_data_received      = 1;

#ifdef SOCLIB_MODULE_DEBUG
          printf("New frame\n Current config : %d %d %d %d\n", 
                 m_config.expansion_factor_index, 
                 m_config.rate,
                 m_config.itmax,
                 m_config.itstop);
          printf("nb_data_expected %d\n", m_nb_data_expected );
//           printf("D_IN_FIRST written @%d cycles\n", r_cnt);
#endif /* SOCLIB_MODULE_DEBUG */ 
          return true; 

        case TC4200_D_IN: 
          input_data                         = data;
          m_input_buffer[m_nb_data_received] = input_data;

          r_d    = (((data >> 24) & 0x1f) << 15) +
            (((data >> 16) & 0x1f) << 10) + 
            (((data >> 8) & 0x1f) << 5) + 
            (((data >> 0) & 0x1f) << 0);
          
          m_nb_data_received ++;
          r_den  = true;
          if (m_nb_data_received == m_nb_data_expected)
            r_dlast = true;
#ifdef SOCLIB_MODULE_DEBUG
//           printf("D_IN written @%d cycles\n", r_cnt);
          if (m_nb_data_received == m_nb_data_expected)
            printf("input frame completed @%d cycles\n", r_cnt);
          if (m_nb_data_received > m_nb_data_expected)
            printf("too much received data\n");
#endif /* SOCLIB_MODULE_DEBUG */ 
          return true; 

        default: 
          printf("tc4200: Writing address erroneous\n");
          return false;
      }

  }


  /*****************/
  /* VCI interface */
  /* Reading itf   */
  /*****************/
  tmpl(bool)::on_read(int seg,
                      typename vci_param::addr_t addr, 
                      typename vci_param::data_t & data)
  {
    int cell = (int)addr / vci_param::B;
    int local_data;
   
    switch ((enum SoclibTc4200Registers) cell)
      {
        case TC4200_MONITOR: 
          local_data = 0;
          local_data = r_monitor.idle + 
            ((unsigned int)r_monitor.syndok << 8) +
            ((unsigned int)r_monitor.itdone << 16); 
          data = local_data;
#ifdef SOCLIB_MODULE_DEBUG
          printf("tc4200 monitoring requested. Status register :0x%08x\n", 
                 local_data);
#endif /* SOCLIB_MODULE_DEBUG */           
          return true;

        case TC4200_D_OUT: 
          /* Output data */
          data = r_q[r_q_end];
                    
#ifdef SOCLIB_MODULE_DEBUG
          cout << setfill('0') << hex;
          cout << "tc4200 d_out : sent : 0x"
               << setw(4) 
               << (int)r_q[r_q_end] 
               << "\n";
          cout << "       nb_data_sent : "
               << dec << m_nb_data_sent + 1
               << "\n";
          
          if (m_nb_data_sent == m_nb_data_send_expected)
            printf("output frame ok\n");
#endif /* SOCLIB_MODULE_DEBUG */ 

          m_nb_data_sent ++;
          r_q_end = (r_q_end + 1) % 16;
          return true;
          
        default: 
          printf("reading address incorrect\n");
          return false;
          
      }
  }

  /************************************/
  /* Transition function              */
  /* Evaluated on Positive Clock Edge */
  /* No output port reference         */
  /************************************/
  tmpl(void)::transition()
  {

    bool reached = false;    
    typename vci_param::addr_t addr;
    typename vci_param::addr_t offset;
    int cell = 0;    

    r_qrdy = false;

    if (p_resetn == false)
      {
        m_vci_fsm.reset();
    
        /* Monitoring register */
        r_monitor.idle   = 1;
        r_monitor.syndok = 0;
        r_monitor.itdone = 0;
        r_pvci_cmdack    = false;

        r_den   = false;
        r_dblk  = false;
        r_d     = 0;
        r_dlast = false;
        r_qrdy      = false;

        r_q_beg = 0;
        r_q_end = 0;
#ifdef SOCLIB_MODULE_DEBUG
        r_cnt = 0;
#endif /* SOCLIB_MODULE_DEBUG */
      
        /* End of reset */
        return;
      }
    
    
    /* Manage Monitoring register */
    r_monitor.idle   = m_idle;
    r_monitor.syndok = m_syndok;
    r_monitor.itdone = (int)m_itdone.read();

    r_den   = false;
    r_dblk  = false;
    r_dlast = false;
    m_vci_fsm.transition();
    

    /* Management for VCI interface */
    addr = p_vci.address;
    if (m_segment.contains(addr))
      {
        reached = true;
        offset = addr - m_segment.baseAddress();
        cell = (int)offset / vci_param::B;
      }

    r_pvci_cmdack = false;

    

    /* The core has more flexibility in term of I/O timing due to parallel
     * access to the Input, Output, Configuration and Monitoring Interface.
     * In order to show a correct VCI interface, limitations are imposed
     */
    if (reached && p_vci.cmdval == true)
      {
        
        /* Address points toward the LDPC decoder and the Command is Valid */
        switch ((enum SoclibTc4200Registers)cell)
          {
            case TC4200_CONFIG: 
            case TC4200_MONITOR: 
              /* Always ready for these two registers */
              if ( ! p_vci.iAccepted() )
                r_pvci_cmdack = true; 
              break;

            case TC4200_D_OUT: 
              /* Qrdy shall be set */
              if ( r_q_beg != r_q_end &&
                   !p_vci.iAccepted() )
                {
#ifdef SOCLIB_MODULE_DEBUG
                  cout << "cmd ack immediately\n";
#endif
                  r_pvci_cmdack = true;
                  /* Some data were already provided and saved into the output
                   * fifo. Command accept immediately.
                   */
                }
              else 
                {
                  /* No output in the r_q fifo. Shall set qrdy to HIGH.
                   */
                  if ( !p_vci.iAccepted() )
                    {
                      r_qrdy      = true;
#ifdef SOCLIB_MODULE_DEBUG
                      cout << "d_out set @" << r_cnt << "\n";
#endif
                    }
                  if ( m_qen.read() &&
                       !p_vci.iAccepted() )
                    {
                      r_pvci_cmdack  = true;
                      r_qrdy         = false;
                    }
                }
              break;

            case TC4200_D_IN_FIRST:
              if ( m_drdy.read() &&
                   ! p_vci.iAccepted() ) 
                {
                  r_pvci_cmdack = true;
#ifdef SOCLIB_MODULE_DEBUG
                  printf("D_IN_FIRST at cycle %d\n", r_cnt);
#endif
                }
              break;
            case TC4200_D_IN: 
              if ( m_decoder_hw.drdy.read() &&
                   ! p_vci.iAccepted() )
                {
                  r_pvci_cmdack = true;
                }
              break;
          }
      }
    
    /* Output data from Hardware Tc4200 decoder */
    if ( m_qen.read() )
      {
        r_q[r_q_beg] = m_q.read(); 
        r_q_beg = (r_q_beg + 1) % 16;
#ifdef SOCLIB_MODULE_DEBUG
        cout << "     data_read @" << r_cnt << "\n";
        if (m_qlast.read() )
          cout << "qlast received @" << r_cnt << "\n";
#endif
      }


#ifdef SOCLIB_MODULE_DEBUG
    r_cnt ++;
#endif /* SOCLIB_MODULE_DEBUG */


    
  }

  /* Moore fsm generation */
  /* Evaluated on Negative Clock Edge */
  /* No input port reference */
  tmpl(void)::genMoore()
  {
    
    /* Default assignation */
    m_dblk  = false;
    m_dlast = false;
    m_den   = false;
    m_d     = 0;
    m_qrdy  = false;


    m_vci_fsm.genMoore();
    /* overwrite cmdack value according to decoder I/O */
    p_vci.cmdack = r_pvci_cmdack;


    /* Manage configuration register */
    m_exp_fact_index = m_config.expansion_factor_index; 
    m_rate           = m_config.rate;
    m_itmax          = m_config.itmax;
    m_itstop         = m_config.itstop;


    m_qrdy  = r_qrdy;
    m_dblk  = r_dblk;
    m_dlast = r_dlast;
    m_den   = r_den;
    m_d     = r_d;

    return;
  }



  tmpl(/**/)::~Tc4200()
  {
  }

}}


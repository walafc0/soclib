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
#include "../include/tc4200_enc.h"


#ifdef SOCLIB_MODULE_DEBUG
#include <iostream>
#include <iomanip>
using namespace std;
#endif /* SOCLIB_MODULE_DEBUG */

namespace soclib {
namespace caba {


#define tmpl(x) template <typename vci_param> x Tc4200_enc<vci_param>

  /* Constructor */
  tmpl(/**/)::Tc4200_enc(sc_module_name   insname, //Instance name
                         const IntTab &index, 
                         const MappingTable &mt)
    : soclib::caba::BaseModule(insname), 
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci"),
      m_vci_fsm(p_vci, mt.getSegmentList(index)),
      m_segment(mt.getSegment(index)),
      m_encoder_hw("tc4200_enc_hw")
  {
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
    
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

    // Connect encoder_hw ports
    m_encoder_hw.ck(p_clk);
    m_encoder_hw.arst_n(p_resetn);
    // Config
    m_encoder_hw.rate(m_rate);
    m_encoder_hw.exp_fact_index(m_exp_fact_index);
    // Monitorinig
    m_encoder_hw.idle(m_idle);
    // Input interface
    m_encoder_hw.dblk(m_dblk);
    m_encoder_hw.den(m_den);
    m_encoder_hw.dlast(m_dlast);
    m_encoder_hw.d(m_d);
    m_encoder_hw.drdy(m_drdy);
    // Output interface
    m_encoder_hw.qblk(m_qblk);
    m_encoder_hw.qen(m_qen);
    m_encoder_hw.qlast(m_qlast);
    m_encoder_hw.q(m_q);
    m_encoder_hw.qrdy(m_qrdy);


    // Define vci_target_fsm read and write functions
    m_vci_fsm.on_read_write(on_read, on_write);

    printf("Successfull Instanciation of tc4200_enc: %s\n", (const char*)insname);


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
    unsigned char input_data;


    switch ((enum SoclibTc4200EncRegisters)cell)
      {
        case TC4200_ENC_CONFIG: 
          m_config.expansion_factor_index = (unsigned char)data;
          m_config.rate                   = (unsigned char)(data >> 8);
#ifdef SOCLIB_MODULE_DEBUG
          cout << "New config : \n expansion factor index: " 
               << (int)m_config.expansion_factor_index 
               <<              "\n rate                  : "
               << (int)m_config.rate << "\n";
#endif /* SOCLIB_MODULE_DEBUG */ 
          return true;
          
        case TC4200_ENC_D_IN_FIRST: 
          /* First data of a new blck */
          // 1 hardware input data holds 4 payload bits.
          // 1 hardware output data holds 4 encoded bits.
          r_nb_data_out_expected[r_index_input_buf] = 24 * expansion_factor / 4; 
          
          /* Deal with rate for input data */
          switch (m_config.rate)
            {
              case 0: /* r=1/2 */
                r_nb_data_in_expected = 12 * expansion_factor / 4;
                break;
              case 1: 
              case 2: /* r=2/3 */
                r_nb_data_in_expected = 16 * expansion_factor / 4;
                break;
              case 3: 
              case 4: /* r=3/4 */
                r_nb_data_in_expected = 18 * expansion_factor / 4;
                break;
              case 5: /* r=5/6 */
                r_nb_data_in_expected = 20 * expansion_factor / 4;
                break;
              default: 
#ifdef SOCLIB_MODULE_DEBUG
                cout << "Erroneous rate value"  << endl;
#endif /* SOCLIB_MODULE_DEBUG */ 
                return false;
            }

          r_nb_data_out[r_index_input_buf] = 0;
          
          r_nb_data_in = 0;
          // Data is assumed to be 32 bits large
          for (int i = 0; i < 8; i ++)
            {
              input_data = 0;
              input_data = (data >> (4 * (7 - i))) & 0x0f;
              m_input_buffer[r_nb_data_in ++] = input_data;
            }

#ifdef SOCLIB_MODULE_DEBUG
//           cout << "First value received: 0x"  
//                << setfill('0') << hex << setw(8) << (int) data 
//                << dec << endl;
//           cout << "First values in m_input_buffer =\n"
//                << setfill('0') << hex 
//                << setw(1) << (int)m_input_buffer[0] <<"\n"
//                << setw(1) << (int)m_input_buffer[1] <<"\n"
//                << setw(1) << (int)m_input_buffer[2] <<"\n"
//                << setw(1) << (int)m_input_buffer[3] <<"\n"
//                << setw(1) << (int)m_input_buffer[4] <<"\n"
//                << setw(1) << (int)m_input_buffer[5] <<"\n"
//                << setw(1) << (int)m_input_buffer[6] <<"\n"
//                << setw(1) << (int)m_input_buffer[7] <<"\n"
//                << dec << endl;
#endif /* SOCLIB_MODULE_DEBUG */ 

          return true;

        case TC4200_ENC_D_IN: 
          for (int i = 0; i < 8; i ++)
            {
              input_data = 0;
              input_data = (data >> (4 * (7 - i))) & 0x0f;
              m_input_buffer[r_nb_data_in ++] = input_data;
            }
          if (r_nb_data_in == r_nb_data_in_expected)
            {
              r_input_buf_filled = true;
            }
              
#ifdef SOCLIB_MODULE_DEBUG
          if (r_nb_data_in == r_nb_data_in_expected)
            cout << "Input frame completed @" << r_cnt << " cycles\n";
#endif /* SOCLIB_MODULE_DEBUG */
          return true;

        default: 
          cerr << "Tc4200_enc: Writing address erroneous" << endl;
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
   

    switch ((enum SoclibTc4200EncRegisters) cell)
      {
        case TC4200_ENC_MONITOR: 
          data = r_idle ? 1 : 0;
#ifdef SOCLIB_MODULE_DEBUG
          cout << "Tc4200_Enc monitoring requested. Status register :0x"
               << setfill('0') << hex << setw(4) << (int) data << dec
               << endl;
          cout << "Tc4200_Enc monitoring @" << r_cnt << endl;
#endif /* SOCLIB_MODULE_DEBUG */           
          return true;
          
        case TC4200_ENC_D_OUT: 
          
          local_data = 0;
          for (int i = 0; 
               i < 8 && (r_nb_data_out[1 - r_index_output_buf] < 
                         r_nb_data_out_expected[1 - r_index_output_buf]); 
               i ++)
            {
              local_data = local_data + 
                ((unsigned int)(m_output_buffer[1 - r_index_output_buf]
                                [r_nb_data_out[1 - r_index_output_buf] ++])
                 << (4 * (7 - i)));
            }
                 
          
          data = local_data;
         
#ifdef SOCLIB_MODULE_DEBUG
          cout << hex;
          cout << "tc4200 d_out : sent : 0x"
               << setw(1) 
               << (int)data 
               << "\n";
          cout << "       nb_data_sent : "
               << dec << r_nb_data_out[1 - r_index_output_buf]
               << endl;
          if (r_nb_data_out[1 - r_index_output_buf] == 
              r_nb_data_out_expected[1 - r_index_output_buf])
            cout << "output frame ok" << endl;
#endif /* SOCLIB_MODULE_DEBUG */ 
          return true;

        default: 
          cerr << "Tc4200_enc: Reading address erroneous" << endl;
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
        
        r_pvci_cmdack  = false;
        
        r_den   = false;
        r_dblk  = false;
        r_d     = 0;
        r_dlast = false;
        r_qrdy  = false;
        r_idle  = false;

        r_index_input_buf  = 0;
        r_index_output_buf = 0;

        r_input_buf_filled = false;

        r_nb_data_out_expected[0] = 1;
        r_nb_data_out_expected[1] = -1;
        r_nb_data_out[0]          = 0;
        r_nb_data_out[1]          = -1;

        r_nb_data_in_expected     = 0;
        r_nb_data_in              = 0;
        
#ifdef SOCLIB_MODULE_DEBUG
        cout << "tc4200_enc: reset @" << r_cnt << endl;
        r_cnt = 0;
#endif /* SOCLIB_MODULE_DEBUG */
      
        /* End of reset */
        return;

      }



    /* Qrdy is Ok when tc_tc4200_enc writing buffer is empty */
    r_qrdy = r_nb_data_out[r_index_output_buf] == 0;


    /* Manage Monitoring register */
    r_idle = m_idle;
    
    r_den   = false;
    r_dblk  = false;
    r_dlast = false;
    m_vci_fsm.transition(); /* calls on_read, on_write */

    /* Management for VCI interface */
    addr = p_vci.address;
    if (m_segment.contains(addr))
      {
        reached = true;
        offset = addr - m_segment.baseAddress();
        cell = (int)offset / vci_param::B;
      }

    r_pvci_cmdack = false;

    /* The core has different flexibility in term of I/O timing due to fully 
     * dataflow and parallel access to the Input, Output, Configuration and
     * Monitoring Interface. In order to show a correct VCI interface,
     * limitations are imposed;
     */
    if (reached && p_vci.cmdval == true)
      {
        /* Address points towards the LDPC encoder and the Command is Valid */
        switch ((enum SoclibTc4200EncRegisters)cell)
          {
            case TC4200_ENC_CONFIG:
            case TC4200_ENC_MONITOR: 
              /* Always ready for these two registers */
              if (! p_vci.iAccepted() )
                r_pvci_cmdack = true;
              break;
              
            case TC4200_ENC_D_OUT:
              /* output will be valid when 1- r_index_output_buf is not
               * completely sent */
              if (r_nb_data_out[1 - r_index_output_buf] < 
                  r_nb_data_out_expected[1 - r_index_output_buf] && 
                  !p_vci.iAccepted())
                {
                  r_pvci_cmdack = true;
                }
              break;
              
            case TC4200_ENC_D_IN_FIRST: 
              if (!r_input_buf_filled && // input_buffer is not full
                  m_drdy.read() && // hardware encoder is ready
                  !p_vci.iAccepted() ) // command not accepted yet
                {
                  r_pvci_cmdack = true;
                  // hardware encoder shall be ready since output buffer
                  // parameters are updated. 
#ifdef SOCLIB_MODULE_DEBUG
                  cout << "D_IN_FIRST at cycle " <<r_cnt << endl;
#endif
                }
              break;
              
            case TC4200_ENC_D_IN: 
              if (r_nb_data_in < r_nb_data_in_expected && // input buffer is not
                                                          // full
                  !p_vci.iAccepted() )
                {
                  r_pvci_cmdack = true;
                }
              break;
              
          }
      }

    ////////////////////////////////////////
    //
    // Management for input buffer
    //
    ////////////////////////////////////////
    if (r_nb_data_in < r_nb_data_in_expected &&
        r_input_buf_filled)
      {
        // input buffer is filled: providing data to encode. 
        r_den = true;
        r_d = (sc_uint<4>)m_input_buffer[r_nb_data_in ++];
        if (r_nb_data_in == r_nb_data_in_expected)
          {
            // Input buffer has been provided. 
            r_input_buf_filled = false;
            r_dlast            = true;
          }
      }
    if (r_nb_data_in == r_nb_data_in_expected && 
        r_input_buf_filled && 
        m_drdy)
      {
        // input buffer has just been filled and hardware encoder is available
        // for encoding.
        r_nb_data_in = 0;
        r_den  = true;
        r_dblk = true;
        r_d    = (sc_uint<4>)m_input_buffer[r_nb_data_in ++];
        r_index_input_buf = 1 - r_index_input_buf;
      }


    ////////////////////////////////////////
    //
    // Management for output buffer
    //
    ////////////////////////////////////////
    if ((r_nb_data_out[r_index_output_buf] ==
         r_nb_data_out_expected[r_index_output_buf]) &&
        (r_nb_data_out[1 - r_index_output_buf] == 
         r_nb_data_out_expected[1 - r_index_output_buf]))
      {
        // hardware writing buffer is full and
        //  VCI output read buffer is totally provided
        
        // => swap buffers 
        r_index_output_buf = 1 - r_index_output_buf;
        // => set hardware writing buffer to empty
        r_nb_data_out[r_index_output_buf] = 0;
        // => set VCI output read buffer at start value
        r_nb_data_out[1 - r_index_output_buf] = 0;
      }
 
    
    ////////////////////////////////////////
    //
    // Management for hardware written buffer
    //
    ////////////////////////////////////////
    if (m_qen.read())
      {
        m_output_buffer[r_index_output_buf]
          [r_nb_data_out[r_index_output_buf] ++] = (unsigned char)m_q.read();
#ifdef SOCLIB_MODULE_DEBUG
//         cout << "Tc4200_enc: output data: \n"
//              << "  r_index_output_buf = " << r_index_output_buf 
//              << "  r_nb_data_out = " << r_nb_data_out[r_index_output_buf] - 1
//              << "  data " << hex << (int)m_q.read() << dec
//              << "  @" << r_cnt << endl;

        if (r_nb_data_out[r_index_output_buf] == 
            r_nb_data_out_expected[r_index_output_buf])
          {
            if (m_qlast) 
              cout << "Tc4200_enc:  Qlast correctly set" <<endl;
            else
              cout << "Tc4200_enc:  Qlast pb" << endl;
          }

#endif /* SOCLIB_MODULE_DEBUG */
      }
    
#ifdef SOCLIB_MODULE_DEBUG
    r_cnt ++;
#endif /* SOCLIB_MODULE_DEBUG */


    return;
  }

  /* Moore fsm generation */
  tmpl(void)::genMoore()
  {
    /* Default assignation */
    m_dblk  = r_dblk;
    m_dlast = r_dlast;
    m_den   = r_den;
    m_d     = r_d;
    m_qrdy  = r_qrdy;


    m_exp_fact_index = m_config.expansion_factor_index;
    m_rate           = m_config.rate;


    m_vci_fsm.genMoore();
    /* overwrite cmdack value according to encoder I/O */
    p_vci.cmdack = r_pvci_cmdack;



    return;
  }


  /* Destructor */
  tmpl(/**/)::~ Tc4200_enc()
    {
    }
  

}}


/*
 *
 * Copyright (c) 2008, TurboConcept SAS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions 
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of TurboConcept nor the names of its contributors 
 *       may be used to endorse or promote products derived from this 
 *       software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TURBOCONCEPT AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TURBOCONCEPT AND CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 2008, TurboConcept SAS
 * Author: C. Cunat
 *
 */

#ifndef SOCLIB_CABA_TC_TC4200_H
#define SOCLIB_CABA_TC_TC4200_H


#include <systemc.h>
#include "caba_base_module.h" 

namespace soclib {
  namespace caba {
    
    using namespace sc_core;
    using namespace sc_dt;

    class Tc_Tc4200 
      : public soclib::caba::BaseModule
    {

    public: 
      sc_in<bool>          ck;
      sc_in<bool>          arst_n;
                          
      sc_in<bool>          itstop;
      sc_in<sc_uint<8> >   itmax;
      sc_in<sc_uint<3> >   rate;
      sc_in<sc_uint<5> >   exp_fact_index;
                                           
      sc_in<bool>          qrdy;          
                                           
      sc_in<bool>          dblk;          
      sc_in<bool>          den;           
      sc_in<bool>          dlast;         
      sc_in<sc_uint<20> >  d;             
                                           
      sc_out<bool>         syndok;        
      sc_out<bool>         idle;          
      sc_out<sc_uint<8> >  itdone;        
                                           
      sc_out<bool>         qblk;          
      sc_out<bool>         qen;           
      sc_out<bool>         qlast;         
      sc_out<sc_uint<16> > q;             
                                                               
      sc_out<bool>         drdy;          
      
    private:
      void * m_private_data;
      void transition();
      void genMoore();
            
    protected: 
      SC_HAS_PROCESS(Tc_Tc4200);
      
    public: 
      Tc_Tc4200(sc_module_name insname);
      ~Tc_Tc4200();

    };

  }
}

#endif /* SOCLIB_CABA_TC_TC4200_H */



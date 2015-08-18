/*
 *
 * Copyright (c) 2010, TurboConcept SAS
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
 * Copyright (c) 2010, TurboConcept SAS
 * Author: C. Cunat
 *
 */

#ifndef SOCLIB_TLMDT_TC_TC4200_H
#define SOCLIB_TLMDT_TC_TC4200_H


#include <tlmdt>


namespace soclib {
  namespace tlmdt {
    
    using namespace std; 
    using namespace sc_core; 
    using namespace tlm; 
    using namespace tlm_utils; 
    
    class Tc_Tc4200 
      : public sc_core::sc_module
    {
      
      
    private:
      sc_event *m_decoder_out; 
      sc_event *m_decoder_in; 
      void     *m_private_data;
      uint32_t  m_MWMR2core_fifo_depth;


    public: 
      Tc_Tc4200(sc_module_name insname, uint32_t MWMR2core_fifo_depth);
      ~Tc_Tc4200();
      
      void      set_out_event(sc_event *e);
      void      set_in_event(sc_event *e);
      
      int       get_input_status(void); 
      uint32_t* get_output_buffer(void); 
      uint32_t  get_out_buf_size(void);
      sc_time   get_out_time(void); 
      void      set_end_out(sc_time &time); 


      tlm_sync_enum readStatusReceived(uint32_t *m_status_register, sc_time &time);
      tlm_sync_enum writeConfigReceived(uint32_t * m_config_register, sc_time &time);
      
      sc_time set_input_buffer(tlm_generic_payload &payload,                 // payload
                               tlm_phase           &phase,                   // phase
                               sc_time             &time);
      
    private: 
      void update_pipe(sc_time &time); 
      
    };
  }
}


#endif /* SOCLIB_TLMDT_TC_TC4200_H */


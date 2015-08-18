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
 * Maintainers: alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2010
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include "../include/vci_audio_fifo.h"


#ifndef VCI_AUDIOFIFO_DEBUG
#define VCI_AUDIOFIFO_DEBUG 0
#endif

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param> x VciAudioFifo<vci_param>

tmpl(/**/)::VciAudioFifo
( sc_core::sc_module_name name,
  const soclib::common::IntTab &index,
  const soclib::common::MappingTable &mt
  )
	   : sc_module(name)
	  , m_index(index)
	  , m_mt(mt)
	  , m_segment(m_mt.getSegment(m_index))
	  , p_vci("socket")
{
  // bind target
  p_vci(*this);                     

  /* Init */
  frate_int[0] = 48000;
  frate_int[1] = 44100;
  frate_int[2] = 32000;
  header_done  = false;
  nsamples     = 0;
}

tmpl(/**/)::~VciAudioFifo()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if VCI SOCKET
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw
( tlm::tlm_generic_payload &payload,
  tlm::tlm_phase           &phase,  
  sc_core::sc_time         &time)   
{
  soclib_payload_extension *extension_pointer;
  payload.get_extension(extension_pointer);

  //this target does not treat the null message
  if(extension_pointer->is_null_message()){
    return tlm::TLM_COMPLETED;
  }

  size_t nwords = (size_t)(payload.get_data_length() / vci_param::nbytes);

  if ( m_segment.contains(payload.get_address())) {
    switch(extension_pointer->get_command()){
    case VCI_READ_COMMAND:
	     std::cout << "READ AUDIO_FIFO" << std::endl;
	     payload.set_response_status(tlm::TLM_OK_RESPONSE);
	     phase = tlm::BEGIN_RESP;
	     time = time + ((( 2 * nwords) - 1) * UNIT_TIME);
	     p_vci->nb_transport_bw(payload, phase, time);
	     return tlm::TLM_COMPLETED;
      break;
    case VCI_WRITE_COMMAND:
      typename vci_param::addr_t address;
      uint32_t mask;

      for (size_t i=0; i<nwords; i++)
      {
        address = ((payload.get_address() - m_segment.baseAddress()) / vci_param::nbytes);
        mask    = atou(payload.get_byte_enable_ptr(), (i * vci_param::nbytes));
        if (address == AUDIO_FIFO_NCHANNELS_REG ) 
        {
          nchannels_reg = (atou( payload.get_data_ptr(), (i * vci_param::nbytes) ) & mask);
          std::cout << "AUDIO FIFO - CHANNELS :  " << nchannels_reg << std::endl;   
        }
        if (address == AUDIO_FIFO_FRATE_REG ) 
        {
          frate_reg = (atou( payload.get_data_ptr(), (i * vci_param::nbytes) ) & mask);
          std::cout << "AUDIO FIFO - FRATE  :  " << frate_reg << std::endl;   
          if (frate_reg > 2 )
          { 
            std::cout << "AUDIO FIFO - WRONG FRATE !!!!" << frate_reg << std::endl;   
            frate_reg = 2; /* Security */
          }
        }
        if (address >= AUDIO_FIFO_SAMPLES_BASE)
        {
          int value = atou( payload.get_data_ptr(), (i * vci_param::nbytes) ) & mask;
          int idx = nsamples+(address & 0x01);
          //std::cout << "AF SAMP " << value  << " - " <<  baseaddr << std::endl;
          samples[idx] = (short int)value;
        }
        if (address == AUDIO_FIFO_VALIDSAMPLES_REG )
        {
          if ( !header_done )
            af_init_wave_file();
          nsamples+=2;
          if (nsamples == AUDIO_FIFO_SAMPLE_WRITE_TRIGGER*AUDIO_FIFO_DEFAULT_CHANNELS ) 
          {
            sf_write_short(sndfile, samples, nsamples);
            nsamples = 0;
          }
          //std::cout << "AUDIO FIFO - SAMPELS VALID " << std::endl;
        }
      }
	     payload.set_response_status(tlm::TLM_OK_RESPONSE);
	     phase = tlm::BEGIN_RESP;
	     time = time + ((( 2 * nwords) - 1) * UNIT_TIME);	
	     p_vci->nb_transport_bw(payload, phase, time);
	     return tlm::TLM_COMPLETED;
      break;
    default:
      assert("command does not exist in VciFrameBuffer");
      break;
    }
  }

  //send error message
  payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
  
  phase = tlm::BEGIN_RESP;
  time = time + (nwords * UNIT_TIME);
  
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[" << name() << "] Address " << std::hex << payload.get_address() << std::dec << " does not match any segment " << std::endl;
#endif
  p_vci->nb_transport_bw(payload, phase, time);
  return tlm::TLM_COMPLETED;
}

tmpl(void)::af_init_wave_file(void)
{
  sndinfo.samplerate = frate_int[frate_reg];
  sndinfo.channels   = nchannels_reg;
  sndinfo.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16 | SF_ENDIAN_FILE;
  sndfile = sf_open("output.wav", SFM_WRITE, &sndinfo );
  sf_command (sndfile, SFC_SET_UPDATE_HEADER_AUTO, NULL, SF_TRUE) ;

  header_done = true;

  return;
}

// Not implemented for this example but required by interface
tmpl(void)::b_transport
( tlm::tlm_generic_payload &payload,                // payload
  sc_core::sc_time         &_time)                  //time
{
  return;
}

// Not implemented for this example but required by interface
tmpl(bool)::get_direct_mem_ptr
( tlm::tlm_generic_payload &payload,                // address + extensions
  tlm::tlm_dmi             &dmi_data)               // DMI data
{ 
  return false;
}
    
// Not implemented for this example but required by interface
tmpl(unsigned int):: transport_dbg                            
( tlm::tlm_generic_payload &payload)                // debug payload
{
  return false;
}
 
}}


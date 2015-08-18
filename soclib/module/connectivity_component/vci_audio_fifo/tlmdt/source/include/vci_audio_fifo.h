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
#ifndef SOCLIB_TLMDT_VCI_FRAMEBUFFER_H
#define SOCLIB_TLMDT_VCI_FRAMEBUFFER_H

#include <tlmdt>
#include "mapping_table.h"
#include "soclib_endian.h"

#include <sndfile.h>

/** 32 bits register index values*/
#define AUDIO_FIFO_NCHANNELS_REG        0x00
#define AUDIO_FIFO_FRATE_REG            0x01
#define AUDIO_FIFO_VALIDSAMPLES_REG     0x02
#define AUDIO_FIFO_SAMPLES_BASE         0x04

/* Write samples each 128 writes */
#define AUDIO_FIFO_SAMPLE_WRITE_TRIGGER 128
/* Two channels only ... */
#define AUDIO_FIFO_DEFAULT_CHANNELS     2

namespace soclib { namespace tlmdt {

template<typename vci_param>
class VciAudioFifo 
  : public sc_core::sc_module                                             // inherit from SC module base class
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "forward interface"
{
private:
  /////////////////////////////////////////////////////////////////////////////////////
  // Member Variables
  /////////////////////////////////////////////////////////////////////////////////////
  soclib::common::IntTab         m_index;
  soclib::common::MappingTable   m_mt;
  soclib::common::Segment        m_segment;
  
  uint32_t   nchannels_reg;
  uint32_t   frate_reg;
  SF_INFO    sndinfo;
  SNDFILE    *sndfile;
  uint32_t   frate_int[3];
  uint32_t   nsamples;
  bool       header_done;
  short int  samples[AUDIO_FIFO_SAMPLE_WRITE_TRIGGER*AUDIO_FIFO_DEFAULT_CHANNELS];

  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_fw_transport_if (VCI TARGET SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_fw        // receive command from initiator
  ( tlm::tlm_generic_payload &payload,      // payload
    tlm::tlm_phase           &phase,        // phase
    sc_core::sc_time         &time);        // time

  // Not implemented for this example but required by interface
  void b_transport                          // b_transport() - Blocking Transport
  ( tlm::tlm_generic_payload &payload,      // payload
    sc_core::sc_time         &time);        // time
  
  // Not implemented for this example but required by interface
  bool get_direct_mem_ptr
  ( tlm::tlm_generic_payload &payload,      // payload
    tlm::tlm_dmi             &dmi_data);    // DMI data
  
  // Not implemented for this example but required by interface
  unsigned int transport_dbg                            
  ( tlm::tlm_generic_payload &payload);     // payload

  void af_init_wave_file(void);

protected:
  SC_HAS_PROCESS(VciAudioFifo);
public:
  tlm::tlm_target_socket<32,tlm::tlm_base_protocol_types> p_vci;   // VCI TARGET socket

  VciAudioFifo(sc_core::sc_module_name name,
		 const soclib::common::IntTab &index,
		 const soclib::common::MappingTable &mt );
  
  ~VciAudioFifo();

};

}}

#endif

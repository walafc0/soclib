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
 * Maintainers: fpecheux, alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include "vci_local_crossbar.h"

namespace soclib { namespace tlmdt {

#define DELAY 1

/////////////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////////////

VciLocalCrossbar::VciLocalCrossbar
(
 sc_core::sc_module_name name,                      // module name
 const soclib::common::MappingTable &mt,            // mapping table
 const soclib::common::IntTab &init_index,          // initiator index mapping table
 const soclib::common::IntTab &target_index,        // target index mapping table
 size_t nb_init,                                    // number of initiators
 size_t nb_target)                                  // number of targets
  : sc_module(name)
{
  std::ostringstream module_name;
  module_name << name  << "_interconnect";
  m_interconnect = new Interconnect(module_name.str().c_str(), mt.getRoutingTable(init_index), mt.getLocalityTable(init_index), mt.getIdMaskingTable(init_index.level()), mt.getIdLocalityTable(init_index), (nb_init + 1), (nb_target + 1), DELAY);
  init(nb_init,nb_target);
}

VciLocalCrossbar::VciLocalCrossbar
(
 sc_core::sc_module_name name,                      // module name
 const soclib::common::MappingTable &mt,            // mapping table
 const soclib::common::IntTab &index,               // index mapping table
 size_t nb_init,                                    // number of initiators
 size_t nb_target)                                  // number of targets
  : sc_module(name)
{
  std::ostringstream module_name;
  module_name << name  << "_interconnect";
  m_interconnect = new Interconnect(module_name.str().c_str(), mt.getRoutingTable(index), mt.getLocalityTable(index), mt.getIdMaskingTable(index.level()), mt.getIdLocalityTable(index), (nb_init + 1), (nb_target + 1), DELAY);
  init(nb_init,nb_target);
}

VciLocalCrossbar::VciLocalCrossbar
(
 sc_core::sc_module_name name,                      // module name
 const soclib::common::MappingTable &mt,            // mapping table
 const soclib::common::IntTab &index,               // global index
 int nb_init,                                       // number of initiators
 int nb_target,                                     // number of targets
 sc_core::sc_time delay )                           // interconnect delay
  : sc_module(name)
{
  std::ostringstream module_name;
  module_name << name  << "_interconnect";
  m_interconnect = new Interconnect(module_name.str().c_str(), mt.getRoutingTable(index), mt.getLocalityTable(index), mt.getIdMaskingTable(index.level()), mt.getIdLocalityTable(index), (nb_init + 1), (nb_target + 1), delay.value());
  init(nb_init,nb_target);
}
   
void VciLocalCrossbar::init
( size_t nb_init,
  size_t nb_target)
{
  // bind VCI TARGET SOCKETS
  for(size_t i=0;i<nb_init;i++){
    std::ostringstream target_name;
    target_name << "target" << i;
    p_to_initiator.push_back(new tlm_utils::simple_target_socket_tagged<VciLocalCrossbar,32,tlm::tlm_base_protocol_types>(target_name.str().c_str()));
    p_to_initiator[i]->register_nb_transport_fw(this, &VciLocalCrossbar::nb_transport_fw_down, i);

    std::ostringstream inits_name;
    inits_name << "inits" << i;
    p_vci_initiators.push_back(new tlm_utils::simple_initiator_socket_tagged<VciLocalCrossbar,32,tlm::tlm_base_protocol_types>(inits_name.str().c_str()));
    p_vci_initiators[i]->register_nb_transport_bw(this, &VciLocalCrossbar::nb_transport_bw_up, i);

    (*p_vci_initiators[i])(*m_interconnect->p_to_initiator[i]);
  }


  // bind VCI INITIATOR SOCKETS
  for(size_t i=0;i<nb_target;i++){
    std::ostringstream init_name;
    init_name << "init" << i;
    p_to_target.push_back(new tlm_utils::simple_initiator_socket_tagged<VciLocalCrossbar,32,tlm::tlm_base_protocol_types>(init_name.str().c_str()));
    p_to_target[i]->register_nb_transport_bw(this, &VciLocalCrossbar::nb_transport_bw_down, i);

    std::ostringstream targets_name;
    targets_name << "targets" << i;
    p_vci_targets.push_back(new tlm_utils::simple_target_socket_tagged<VciLocalCrossbar,32,tlm::tlm_base_protocol_types>(targets_name.str().c_str()));
    p_vci_targets[i]->register_nb_transport_fw(this, &VciLocalCrossbar::nb_transport_fw_up, i);

    (*p_vci_targets[i])(*m_interconnect->p_to_target[i]);
  }

  p_target_to_up.register_nb_transport_fw(this, &VciLocalCrossbar::nb_transport_fw_down);
  p_vci_target_to_down.register_nb_transport_fw(this, &VciLocalCrossbar::nb_transport_fw_up);

  p_initiator_to_up.register_nb_transport_bw(this, &VciLocalCrossbar::nb_transport_bw_down);
  p_vci_initiator_to_down.register_nb_transport_bw(this, &VciLocalCrossbar::nb_transport_bw_up);

  p_vci_initiator_to_down(*m_interconnect->p_to_initiator[nb_init]);
  p_vci_target_to_down(*m_interconnect->p_to_target[nb_target]);

}

void VciLocalCrossbar::print(){
  m_interconnect->print();
}
  
/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if (VCI TARGET SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tlm::tlm_sync_enum VciLocalCrossbar::nb_transport_fw_down   
( tlm::tlm_generic_payload    &payload,            // payload
  tlm::tlm_phase              &phase,              // phase
  sc_core::sc_time            &time)               // time
{
  p_vci_initiator_to_down->nb_transport_fw(payload, phase, time);
  return  tlm::TLM_COMPLETED;
} //end nb_transport_fw

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if (VCI TARGET SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tlm::tlm_sync_enum VciLocalCrossbar::nb_transport_fw_up
( tlm::tlm_generic_payload    &payload,            // payload
  tlm::tlm_phase              &phase,              // phase
  sc_core::sc_time            &time)               // time
{
  p_initiator_to_up->nb_transport_fw(payload, phase, time);
  return  tlm::TLM_COMPLETED;
} //end nb_transport_fw

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_bw_transport_if (VCI INITIATOR SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tlm::tlm_sync_enum VciLocalCrossbar::nb_transport_bw_down   
( tlm::tlm_generic_payload    &payload,            // payload
  tlm::tlm_phase              &phase,              // phase
  sc_core::sc_time            &time)               // time
{
  p_vci_target_to_down->nb_transport_bw(payload, phase, time);
  return  tlm::TLM_COMPLETED;
} //end nb_transport_fw

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_bw_transport_if (VCI INITIATOR SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tlm::tlm_sync_enum VciLocalCrossbar::nb_transport_bw_up
( tlm::tlm_generic_payload    &payload,            // payload
  tlm::tlm_phase              &phase,              // phase
  sc_core::sc_time            &time)               // time
{
  p_target_to_up->nb_transport_bw(payload, phase, time);
  return  tlm::TLM_COMPLETED;
} //end nb_transport_fw



/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if (VCI TARGET SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tlm::tlm_sync_enum VciLocalCrossbar::nb_transport_fw_up   
( int                          id,                 // register id
  tlm::tlm_generic_payload    &payload,            // payload
  tlm::tlm_phase              &phase,              // phase
  sc_core::sc_time            &time)               // time
{
  (*p_to_target[id])->nb_transport_fw(payload, phase, time);
  return  tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if (VCI TARGET SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tlm::tlm_sync_enum VciLocalCrossbar::nb_transport_fw_down   
( int                          id,                 // register id
  tlm::tlm_generic_payload    &payload,            // payload
  tlm::tlm_phase              &phase,              // phase
  sc_core::sc_time            &time)               // time
{
  (*p_vci_initiators[id])->nb_transport_fw(payload, phase, time);
  return  tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_bw_transport_if (VCI INITIATOR SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tlm::tlm_sync_enum VciLocalCrossbar::nb_transport_bw_up   
( int                          id,                 // register id
  tlm::tlm_generic_payload    &payload,            // payload
  tlm::tlm_phase              &phase,              // phase
  sc_core::sc_time            &time)               // time
{
  (*p_to_initiator[id])->nb_transport_bw(payload, phase, time);
  return  tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_bw_transport_if (VCI INITIATOR SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tlm::tlm_sync_enum VciLocalCrossbar::nb_transport_bw_down   
( int                          id,                 // register id
  tlm::tlm_generic_payload    &payload,            // payload
  tlm::tlm_phase              &phase,              // phase
  sc_core::sc_time            &time)               // time
{
  (*p_vci_targets[id])->nb_transport_bw(payload, phase, time);
  return  tlm::TLM_COMPLETED;
}

}}

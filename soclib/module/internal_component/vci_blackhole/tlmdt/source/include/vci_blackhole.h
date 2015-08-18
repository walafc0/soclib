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
 * Maintainers: fpecheux, alinev
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef SOCLIB_TLMDT_VCI_BLACKHOLE_H
#define SOCLIB_TLMDT_VCI_BLACKHOLE_H

#include <tlmdt>       			// TLM-DT headers
#include "vci_blackhole_base.h"

namespace soclib { namespace tlmdt {
    
template<typename socket_type>
class VciBlackhole
  : public soclib::tlmdt::VciBlackholeBase
{

protected:
  SC_HAS_PROCESS(VciBlackhole);
  
public:
  std::vector<tlm::tlm_initiator_socket<> *> p_socket;

  VciBlackhole
  ( sc_core::sc_module_name name,
    int n_sockets )
    : soclib::tlmdt::VciBlackholeBase(name) // init module name
  {
    for(int i=0; i<n_sockets; i++){
      std::ostringstream name;
      name << "socket" << i;
      p_socket.push_back(new tlm::tlm_initiator_socket<>(name.str().c_str()));
      // bind
      p_socket[i]->bind(*this);
    }
  };
};

template<>
class VciBlackhole<tlm_utils::simple_initiator_socket_tagged<VciBlackholeBase, 32, tlm::tlm_base_protocol_types> >
  : public soclib::tlmdt::VciBlackholeBase
{

protected:
  SC_HAS_PROCESS(VciBlackhole);
  
public:
  std::vector<tlm_utils::simple_initiator_socket_tagged<VciBlackholeBase, 32, tlm::tlm_base_protocol_types> *> p_socket;

  VciBlackhole
  ( sc_core::sc_module_name name,
    int n_sockets )
    : soclib::tlmdt::VciBlackholeBase(name) // init module name
  {
    for(int i=0; i<n_sockets; i++){
      std::ostringstream name;
      name << "socket" << i;
      p_socket.push_back(new tlm_utils::simple_initiator_socket_tagged<VciBlackholeBase, 32, tlm::tlm_base_protocol_types>(name.str().c_str()));
      // register
      p_socket[i]->register_nb_transport_bw(this, &VciBlackholeBase::nb_transport_bw, i);
    }
  };
};

template<>
class VciBlackhole<tlm_utils::simple_target_socket_tagged<VciBlackholeBase, 32, tlm::tlm_base_protocol_types> >
  : public soclib::tlmdt::VciBlackholeBase
{

protected:
  SC_HAS_PROCESS(VciBlackhole);
  
public:
  std::vector<tlm_utils::simple_target_socket_tagged<VciBlackholeBase, 32, tlm::tlm_base_protocol_types> *> p_socket;

  VciBlackhole
  ( sc_core::sc_module_name name,
    int n_sockets )
    : soclib::tlmdt::VciBlackholeBase(name) // init module name
  {
    for(int i=0; i<n_sockets; i++){
      std::ostringstream name;
      name << "socket" << i;
      p_socket.push_back(new tlm_utils::simple_target_socket_tagged<VciBlackholeBase, 32, tlm::tlm_base_protocol_types>(name.str().c_str()));
      // register
      p_socket[i]->register_nb_transport_fw(this, &VciBlackholeBase::nb_transport_fw, i);
    }
  };
};

}}

#endif

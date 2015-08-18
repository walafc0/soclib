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
 * Copyright (c) CEA
 *
 * Authors: Franck Vedrine <franck.vedrine@cea.fr>, 2008
 */

#ifndef SOCLIB_TLMT_PORTS_FILTERTEMPLATE
#define SOCLIB_TLMT_PORTS_FILTERTEMPLATE

#include "vci_filtered_ports.h"

namespace soclib { namespace tlmt {

#define tmpl(t) template<typename vci_param> t VciFilteredTarget<vci_param>

tmpl(void)::writeError(const char* szError, Direction dDirection) const {
   (m_log_file ? *m_log_file : (std::ostream&) std::cout)
      << "ERROR : Protocol Error \""<< szError <<"\""
      << " for the packet " << ((dDirection == DOut) ? m_nb_response_packets : m_nb_request_packets)
      << " issued from " << ((dDirection == DOut) ? ((const char*) "response") : ((const char*) "request")) << " !!!\n";
}


tmpl(void)::LockedAddress::add(uint32_t srcid,
      typename vci_param::addr_t aAddress) {
  AddressInterval aiInterval(srcid, aAddress);
   typename std::set<AddressInterval>::iterator iter = saiAddresses.find(aiInterval);
   if (iter == saiAddresses.end())
      saiAddresses.insert(iter, aiInterval);
   else {
      AddressInterval& aiCurrent = const_cast<AddressInterval&>(*iter);
      aiCurrent.merge(aiInterval);
      ++iter;
      if (iter != saiAddresses.end() && (aiCurrent == *iter))
         saiAddresses.erase(iter);
      iter = saiAddresses.find(aiInterval);
      if (&*iter == &aiCurrent) {
         if (iter != saiAddresses.begin()) {
            --iter;
            if (aiCurrent == *iter)
               saiAddresses.erase(iter);
         };
      }
      else {
         if (aiCurrent == *iter)
            saiAddresses.erase(iter);
      };   
   };
}

tmpl(void)::LockedAddress::remove(VciFilteredTarget<vci_param>& filter,
      uint32_t srcid, typename vci_param::addr_t aAddress) {
   AddressInterval aiInterval(srcid, aAddress);
   typename std::set<AddressInterval>::iterator iter = saiAddresses.find(aiInterval);
   if (iter != saiAddresses.end()) {
      AddressInterval* paiOther = NULL;
      bool fExtern = false;
      bool fDelete = const_cast<AddressInterval&>(*iter).remove(filter, aiInterval, paiOther, fExtern);
      if (fDelete)
         saiAddresses.erase(iter);
      else if (paiOther) {
         saiAddresses.insert(iter, *paiOther);
         delete paiOther;
      };
   };
}

tmpl(void)::check_command(vci_cmd_packet<vci_param>* pkt) {
   assume(!pkt->contig || ((pkt->nwords-1 & pkt->nwords) == 0),
          "Packets should be contigous and have a length within the form 2^n", DIn);
   m_pending_packets.add(*pkt);
   if (pkt->cmd == vci_param::CMD_LOCKED_READ)
      m_locked_addresses.add(pkt->srcid, pkt->address);
   ++m_nb_request_packets;
}

tmpl(void)::check_response(vci_rsp_packet<vci_param>* pkt) {
   assume(m_pending_packets.count() > 0,
         "The response packet does not correspond to any request", DOut);
   Packet* pPacket = m_pending_packets.remove(*pkt);
   assume(pPacket,
      "The response packet does not correspond to any request packet", DOut);
   assume(pPacket->nwords == pkt->nwords,
      "The response packet has not the expected length", DOut);
   if (pPacket->cmd == vci_param::CMD_WRITE) // Write
      m_locked_addresses.remove(*this, pkt->rsrcid, pkt->address);
   if (pPacket) delete pPacket;
   ++m_nb_response_packets;
}

#undef tmpl

#define tmpl(t) template<typename vci_param> t VciFilteredInitiator<vci_param>

tmpl(void)::writeError(const char* szError, Direction dDirection) const {
   (m_log_file ? *m_log_file : (std::ostream&) std::cout)
      << "ERROR : Protocol Error \""<< szError <<"\""
      << " for the packet " << ((dDirection == DOut) ? m_nb_response_packets : m_nb_request_packets)
      << " issued from " << ((dDirection == DOut) ? ((const char*) "response") : ((const char*) "request")) << " !!!\n";
}


tmpl(void)::LockedAddress::add(uint32_t srcid,
      typename vci_param::addr_t aAddress) {
  AddressInterval aiInterval(srcid, aAddress);
   typename std::set<AddressInterval>::iterator iter = saiAddresses.find(aiInterval);
   if (iter == saiAddresses.end())
      saiAddresses.insert(iter, aiInterval);
   else {
      AddressInterval& aiCurrent = const_cast<AddressInterval&>(*iter);
      aiCurrent.merge(aiInterval);
      ++iter;
      if (iter != saiAddresses.end() && (aiCurrent == *iter))
         saiAddresses.erase(iter);
      iter = saiAddresses.find(aiInterval);
      if (&*iter == &aiCurrent) {
         if (iter != saiAddresses.begin()) {
            --iter;
            if (aiCurrent == *iter)
               saiAddresses.erase(iter);
         };
      }
      else {
         if (aiCurrent == *iter)
            saiAddresses.erase(iter);
      };   
   };
}

tmpl(void)::LockedAddress::remove(VciFilteredInitiator<vci_param>& filter,
      uint32_t srcid, typename vci_param::addr_t aAddress) {
   AddressInterval aiInterval(srcid, aAddress);
   typename std::set<AddressInterval>::iterator iter = saiAddresses.find(aiInterval);
   if (iter != saiAddresses.end()) {
      AddressInterval* paiOther = NULL;
      bool fExtern = false;
      bool fDelete = const_cast<AddressInterval&>(*iter).remove(filter, aiInterval, paiOther, fExtern);
      if (fDelete)
         saiAddresses.erase(iter);
      else if (paiOther) {
         saiAddresses.insert(iter, *paiOther);
         delete paiOther;
      };
   };
}

tmpl(void)::check_command(vci_cmd_packet<vci_param>* pkt) {
   assume(!pkt->contig || ((pkt->nwords-1 & pkt->nwords) == 0),
          "Packets should be contigous and have a length within the form 2^n", DIn);
   m_pending_packets.add(*pkt);
   if (pkt->cmd == vci_param::CMD_LOCKED_READ)
      m_locked_addresses.add(pkt->srcid, pkt->address);
   ++m_nb_request_packets;
}

tmpl(void)::check_response(vci_rsp_packet<vci_param>* pkt) {
   assume(m_pending_packets.count() > 0,
         "The response packet does not correspond to any request", DOut);
   Packet* pPacket = m_pending_packets.remove(*pkt);
   assume(pPacket,
      "The response packet does not correspond to any request packet", DOut);
   assume(pPacket->nwords == pkt->nwords,
      "The response packet has not the expected length", DOut);
   if (pPacket->cmd == vci_param::CMD_WRITE) // Write
      m_locked_addresses.remove(*this, pkt->srcid, pPacket->address);
   if (pPacket) delete pPacket;
   ++m_nb_response_packets;
}

#undef tmpl

}} // end of namespace soclib::tlmt

#endif // SOCLIB_TLMT_PORTS_FILTERTEMPLATE


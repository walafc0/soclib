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

#ifndef SOCLIB_CABA_PVDC_ADVANCED_FILTERTEMPLATE
#define SOCLIB_CABA_PVDC_ADVANCED_FILTERTEMPLATE

#include "avci_filter.h"

namespace soclib { namespace caba {

#define tmpl(t) template<typename vci_param> t AdvancedVciFilter<vci_param>

tmpl(void)::writeError(const char* szError, Direction dDirection) const {
   (m_log_file ? *m_log_file : (std::ostream&) std::cout)
      << "ERROR : Protocol Error \""<< szError <<"\"on "<< name()
      << " for the packet " << ((dDirection == DOut) ? m_nb_response_packets : m_nb_request_packets)
      << ", the cell " << ((dDirection == DOut) ? m_response_cells : m_request_cells)
      << " issued from " << ((dDirection == DOut) ? ((const char*) "response") : ((const char*) "request")) << " !!!\n";
}

tmpl(void)::reset()
   {  setReset(m_default_reset); }

tmpl(void)::testHandshake() { // see p 41, 42, verified on the waveforms
   switch (m_request_state) {
      case SIdle:
         if (p_in.cmdval) {
            acquireRequestCell();
            if (p_out.cmdack)
               m_request_state = SSync;
            else
               m_request_state = SValid;
         }
         else if (p_out.cmdack)
            m_request_state = SDefault_Ack;
         break;
      case SValid:
         assume(p_in.cmdval, "The protocol does not accept \"bubble\" in a VCI command request packet", DIn);
         assume((p_in.address == m_address_previous) && (p_in.be == m_be_previous)
            && (p_in.cfixed == m_cfixed_previous) && (p_in.clen == m_clen_previous)
            && (p_in.cmd == m_cmd_previous) && (p_in.contig == m_contig_previous)
            && (p_in.wdata == m_wdata_previous) && (p_in.eop == m_eop_previous)
            && (p_in.cons == m_cons_previous) && (p_in.plen == m_plen_previous)
            && (p_in.wrap == m_wrap_previous),
               "The protocol does not accept \"change\" during request acknowledgement", DIn);
         if (p_out.cmdack)
            m_request_state = SSync;
         break;
      case SDefault_Ack:
         assume(p_out.cmdack, "The protocol requires default request acknowledgement", DOut);
         if (p_in.cmdval) {
            acquireRequestCell();
            m_request_state = SSync;
         };
         break;
      case SSync:
         if (!p_in.cmdval) {
            if (!p_out.cmdack)
               m_request_state = SIdle;
            else
               m_request_state = SDefault_Ack;
         }
         else {
            acquireRequestCell();
            if (!p_out.cmdack)
               m_request_state = SValid;
         };
         break;
   };
 
   switch (m_response_state) {
      case SIdle:
         if (p_out.rspval) {
            acquireResponseCell();
            if (p_in.rspack)
               m_response_state = SSync;
            else
               m_response_state = SValid;
         }
         else if (p_in.rspack)
            m_response_state = SDefault_Ack;
         break;
      case SValid:
         assume(p_out.rspval, "The protocol does not accept \"bubble\" in a VCI command response packet", DOut);
         assume((p_out.rdata == m_rdata_previous) && (p_out.reop == m_reop_previous)
            && (p_out.rerror == m_rerror_previous),
            "The protocol does not accept \"change\" during response acknowledgement", DOut);
         if (p_in.rspack)
            m_response_state = SSync;
         break;
      case SDefault_Ack:
         assume(p_in.rspack, "The protocol requires default response acknowledgement", DIn);
         if (p_out.rspval) {
            acquireResponseCell();
            m_response_state = SSync;
         };
         break;
      case SSync:
         if (!p_out.rspval) {
            if (!p_in.rspack)
               m_response_state = SIdle;
            else
               m_response_state = SDefault_Ack;
         }
         else {
            acquireResponseCell();
            if (!p_in.rspack)
               m_response_state = SValid;
         };
         break;
   };
}

tmpl(void)::LockedAddress::add(typename vci_param::srcid_t srcid,
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

tmpl(void)::LockedAddress::add(typename vci_param::srcid_t srcid,
      typename vci_param::addr_t aAddress, int plen) {
   AddressInterval aiInterval(srcid, aAddress, plen);
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

tmpl(void)::LockedAddress::remove(AdvancedVciFilter& filter,
      typename vci_param::srcid_t srcid, typename vci_param::addr_t aAddress) {
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

tmpl(void)::LockedAddress::remove(AdvancedVciFilter& filter,
      typename vci_param::srcid_t srcid, typename vci_param::addr_t aAddress, int plen) {
   AddressInterval aiInterval(srcid, aAddress, plen);
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

tmpl(void)::acquireRequestCell() {
   if (m_request_cells == 0) {
      if (m_packet_clen > 0) {
         ++m_packet_clen;
         assume(p_in.clen == m_packet_clen && m_packet_cmd == p_in.cmd && m_packet_cfixed == p_in.cfixed,
               "The length, command and cfixed should be constant during the reception of chain of packets", DIn);
         --m_packet_clen;
         if (m_packet_cfixed)
            assume(m_packet_contig == p_in.contig && m_packet_wrap == p_in.wrap
                  && m_packet_cons == p_in.cons && m_packet_plen == p_in.plen,
               "The fields contig, wrap, const and plen should be constant when cfixed during the reception of chain of packets", DIn);
      }
      else {
         m_packet_clen = p_in.clen;
         ++m_packet_clen;
         m_packet_cmd = p_in.cmd;
         m_packet_cfixed = p_in.cfixed;
      };
      --m_packet_clen;
      m_packet_address = p_in.address;
      m_packet_plen = p_in.plen;
      m_packet_contig = p_in.contig;
      m_packet_wrap = p_in.wrap;
      assume(!m_packet_wrap || (m_packet_contig && ((m_packet_plen-1 & m_packet_plen) == 0)),
            "Packets should be contigous and have a length within the form 2^n", DIn);
      m_packet_cons = p_in.cons;
      m_pending_packets.add(p_in);
      if (m_packet_cmd == 3) { // Locked read
         if (m_packet_contig)
            m_locked_addresses.add(p_in.srcid, m_packet_address, m_packet_plen);
         else
            m_locked_addresses.add(p_in.srcid, m_packet_address);
      };
   }
   else {
      assume(m_packet_clen == p_in.clen && m_packet_cmd == p_in.cmd && m_packet_plen == p_in.plen,
            "The length, command and clen should be constant during packet reception", DIn);
      if (m_packet_cons) {
         assume(m_packet_cfixed == p_in.cfixed && m_packet_address == p_in.address
            && m_packet_contig == p_in.contig && m_packet_wrap == p_in.wrap && m_packet_cons == p_in.cons,
            "The fields fixed, address, contig, wrap and cons should be constant when cons during the reception of a packet", DIn);
      }
      else if (m_packet_contig) {
         m_packet_address += vci_param::B; // cell_size()
         if (m_packet_wrap && (m_request_cells > m_packet_plen))
            m_packet_address -= m_packet_plen*vci_param::B;
         assume(m_packet_address == p_in.address,
            "The protocol accepts only increasing addresses in a given packet", DIn);
      };
   };
   ++m_request_cells;
   if (p_in.eop) {
      assume(!m_packet_plen || m_packet_plen == m_request_cells,
            "The request packet has not the expected length", DIn);
      m_request_cells = 0;
      m_packet_address = 0;
      ++m_nb_request_packets;
   };
}

tmpl(void)::acquireResponseCell() {
   if (m_response_cells == 0) {
      assume(m_pending_packets.count() > 0,
            "The response packet does not correspond to any request", DOut);
      {  Packet* pPacket = m_pending_packets.remove(p_out);
         assume(pPacket,
            "The response packet does not correspond to any request packet", DOut);
         if (pPacket->cmd == 2) { // Write
            if (pPacket->contig)
               m_locked_addresses.remove(*this, p_out.rsrcid, pPacket->address, pPacket->plen);
            else
               m_locked_addresses.remove(*this, p_out.rsrcid, pPacket->address);
         };
         if (pPacket) delete pPacket;
      };
   };
   ++m_response_cells;
   if (p_out.reop) {
      m_response_cells = 0;
      ++m_nb_response_packets;
   };
}

#undef tmpl

}} // end of namespace soclib::caba

#endif // SOCLIB_CABA_PVDC_ADVANCED_FILTERTEMPLATE


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

#ifndef SOCLIB_TLMT_PORTS_FILTERH
#define SOCLIB_TLMT_PORTS_FILTERH

#include "vci_ports.h"
#include <set>

namespace soclib { namespace tlmt {

template<typename vci_param>
class VciFilteredTarget : public VciTarget<vci_param>
{
   class verif_callback : public tlmt_core::tlmt_callback_base<vci_cmd_packet<vci_param>*>
   {
   private:
      VciFilteredTarget* target;
      tlmt_core::tlmt_callback_base<vci_cmd_packet<vci_param>*> *call_back;

   public:
      verif_callback(VciFilteredTarget* target_src, tlmt_core::tlmt_callback_base<vci_cmd_packet<vci_param>*> * call_back_src)
         : target(target_src), call_back(call_back_src) {}
      virtual void operator()(vci_cmd_packet<vci_param>* param, const tlmt_core::tlmt_time &time)
      {  target->check_command(param);
         /* tlmt_core::tlmt_return& result =*/ (*call_back)(param, time);
         /* return result; */
      }
   };
   
   std::ostream* m_log_file;
   verif_callback m_call_back;
   enum Direction { DIn, DOut };
   void writeError(const char* szError, Direction dDirection) const;
   void assume(bool fCondition, const char* szError, Direction dDirection) const
      {  if (!fCondition)
            writeError(szError, dDirection);
      }

public:
   class PacketsList;
   class Packet {
     public:
      typename vci_param::cmd_t cmd;
      typename vci_param::addr_t address;
      bool contig;
      size_t nwords;
      uint32_t srcid;
      uint32_t trdid;
      uint32_t pktid;
      friend class PacketsList;
      
      Packet(const vci_rsp_packet<vci_param>& p_out)
         :  /* cmd(p_out.cmd), */ srcid(p_out.srcid), trdid(p_out.trdid), pktid(p_out.pktid) {}
     public:
      Packet(const vci_cmd_packet<vci_param>& p_in)
         :  cmd(p_in.cmd), address(p_in.address), contig(p_in.contig), nwords(p_in.nwords),
            srcid(p_in.srcid), trdid(p_in.trdid), pktid(p_in.pktid) {}
      Packet(const Packet& source)
         :  cmd(source.cmd), address(source.address), contig(source.contig), nwords(source.nwords),
            srcid(source.srcid), trdid(source.trdid), pktid(source.pktid) {}

      struct Less : public std::less<Packet*> {
        public:
         bool operator()(Packet* ppFst, Packet* ppSnd) const
            {  return (ppFst->pktid < ppSnd->pktid)
                  || ((ppFst->pktid == ppSnd->pktid) && (ppFst->srcid < ppSnd->srcid));
            }
      };
   };
   class PacketsList {
     private:
      typedef std::set<Packet*, typename Packet::Less> Packets;
      Packets lpContent;

     public:
      PacketsList() {}
      PacketsList(const PacketsList& source)
         {  for (typename Packets::const_iterator iter = source.lpContent.begin();
                  iter != source.lpContent.end(); ++iter)
               if (*iter) lpContent.push_back(new Packet(**iter));
         }
      ~PacketsList()
         {  for (typename Packets::iterator iter = lpContent.begin(); iter != lpContent.end(); ++iter)
               if (*iter) delete *iter;
         }
      int count() const { return lpContent.size(); }
      void add(const vci_cmd_packet<vci_param>& p_in) { lpContent.insert(new Packet(p_in)); }
      Packet* remove(const vci_rsp_packet<vci_param>& p_out)
         {  Packet pLocate(p_out);
            typename std::set<Packet*, typename Packet::Less>::iterator iter = lpContent.find(&pLocate);
            Packet* pResult = NULL;
            if (iter != lpContent.end()) {
               pResult = *iter;
               lpContent.erase(iter);
            };
            return pResult;
         }
   };

   class AddressInterval {
     private:
      typename vci_param::addr_t aMin, aMax;
      uint32_t src_id;

     public:
      AddressInterval(uint32_t src_idSource)
         :  aMin(0), aMax(0), src_id(src_idSource) {}
      AddressInterval(uint32_t src_idSource,
            typename vci_param::addr_t aMinSource)
         :  aMin(aMinSource), aMax(aMinSource), src_id(src_idSource) {}
      AddressInterval(uint32_t src_idSource,
            typename vci_param::addr_t aMinSource, typename vci_param::addr_t aMaxSource)
         :  aMin(aMinSource), aMax(aMaxSource), src_id(src_idSource) {}

      AddressInterval& operator=(const AddressInterval& source)
         {  aMin = source.aMin; aMax = source.aMax; src_id = source.src_id; return *this; }
      bool operator<(const AddressInterval& source) const
         {  return aMax < source.aMin; }
      bool operator>(const AddressInterval& source) const
         {  return aMax > source.aMin; }
      bool operator==(const AddressInterval& source) const
         {  return aMax >= source.aMin && aMin <= source.aMax; }
      void merge(const AddressInterval& source)
         {  if (aMin > source.aMin)
               aMin = source.aMin;
            if (aMax < source.aMax)
               aMax = source.aMax;
         }
      bool isValid() const { return aMax > aMin; }
      bool remove(VciFilteredTarget<vci_param>& filter, const AddressInterval& source, AddressInterval*& paiOther, bool& fError)
         {  fError = source.aMin < aMin || source.aMax > aMax;
            bool fResult = (source.aMin <= aMin && source.aMax >= aMax);
            if (!fResult) {
               if (aMin < source.aMin) {
                  filter.assume(src_id == source.src_id,
                        (((std::string("Conflict access : a locked was set by the initiator ")
                        += src_id) += " and accessed by the initiator ") += source.src_id).c_str(), DIn);
                  if (aMax > source.aMax) {
                     paiOther = new AddressInterval(src_id, source.aMax, aMax);
                     aMax = source.aMin;
                  }
                  else // aMax == source.aMax
                     aMax = source.aMin;
               }
               else { // aMin == source.aMin
                  if (aMax > source.aMax) {
                     filter.assume(src_id == source.src_id,
                        (((std::string("Conflict access : a locked was set by the initiator ")
                        += src_id) += " and accessed by the initiator ") += source.src_id).c_str(), DIn);
                     aMin = source.aMax;
                  };
               };
            }
            else
               filter.assume(src_id == source.src_id,
                  (((std::string("Conflict access : a locked was set by the initiator ")
                  += src_id) += " and accessed by the initiator ") += source.src_id).c_str(), DIn);
            return fResult;
         }
   };
   class LockedAddress {
     private:
      std::set<AddressInterval> saiAddresses;

     public:
      LockedAddress() {}
      LockedAddress(const LockedAddress& source) : saiAddresses(source.saiAddresses) {}

      void add(uint32_t srcid, typename vci_param::addr_t aAddress);
      void remove(VciFilteredTarget<vci_param>& filter, uint32_t srcid, typename vci_param::addr_t aAddress);
      int count() const { return saiAddresses.size(); }
   };

   PacketsList m_pending_packets;
   LockedAddress m_locked_addresses;
   int m_nb_request_packets, m_nb_response_packets;

	VciFilteredTarget(const std::string &name, typename VciTarget<vci_param>::callback_t cb,
         tlmt_core::tlmt_thread_context *opt_ref = NULL)
		: VciTarget<vci_param>(name, &m_call_back, opt_ref), m_call_back(this, cb),
        m_nb_request_packets(0), m_nb_response_packets(0) {}

   void check_command(vci_cmd_packet<vci_param>* pkt);
   void check_response(vci_rsp_packet<vci_param>* pkt);
   bool isFinished() const { return m_pending_packets.count() == 0 && m_locked_addresses.count() == 0; }
   void setLogOut(std::ostream& logFile) { m_log_file = &logFile; }
	void send(vci_rsp_packet<vci_param> *pkt, const tlmt_core::tlmt_time &time)
	{	check_response(pkt);
      /* return */ VciTarget<vci_param>::send(pkt, time);
	}
};

template<typename vci_param>
class VciFilteredInitiator : public VciInitiator<vci_param>
{
   class verif_callback : public tlmt_core::tlmt_callback_base<vci_rsp_packet<vci_param>*>
   {
   private:
      VciFilteredInitiator* initiator;
      tlmt_core::tlmt_callback_base<vci_rsp_packet<vci_param>*> *call_back;

   public:
      verif_callback(VciFilteredInitiator* initiator_src, tlmt_core::tlmt_callback_base<vci_rsp_packet<vci_param>*> * call_back_src)
         : initiator(initiator_src), call_back(call_back_src) {}
      virtual void operator()(vci_rsp_packet<vci_param>* param, const tlmt_core::tlmt_time &time)
      {  initiator->check_response(param);
         /* tlmt_core::tlmt_return& result = */ (*call_back)(param, time);
         /* return result; */
      }
   };
   
   std::ostream* m_log_file;
   verif_callback m_call_back;
   enum Direction { DIn, DOut };
   void writeError(const char* szError, Direction dDirection) const;
   void assume(bool fCondition, const char* szError, Direction dDirection) const
      {  if (!fCondition)
            writeError(szError, dDirection);
      }

public:
   class PacketsList;
   class Packet {
     public:
      typename vci_param::cmd_t cmd;
      typename vci_param::addr_t address;
      bool contig;
      size_t nwords;
      uint32_t srcid;
      uint32_t trdid;
      uint32_t pktid;
      friend class PacketsList;
      
      Packet(const vci_rsp_packet<vci_param>& p_out)
         :  /* cmd(p_out.cmd), */ srcid(p_out.srcid), trdid(p_out.trdid), pktid(p_out.pktid) {}
     public:
      Packet(const vci_cmd_packet<vci_param>& p_in)
         :  cmd(p_in.cmd), address(p_in.address), contig(p_in.contig), nwords(p_in.nwords),
            srcid(p_in.srcid), trdid(p_in.trdid), pktid(p_in.pktid) {}
      Packet(const Packet& source)
         :  cmd(source.cmd), address(source.address), contig(source.contig), nwords(source.nwords),
            srcid(source.srcid), trdid(source.trdid), pktid(source.pktid) {}

      struct Less : public std::less<Packet*> {
        public:
         bool operator()(Packet* ppFst, Packet* ppSnd) const
            {  return (ppFst->pktid < ppSnd->pktid)
                  || ((ppFst->pktid == ppSnd->pktid) && (ppFst->srcid < ppSnd->srcid));
            }
      };
   };
   class PacketsList {
     private:
      typedef std::set<Packet*, typename Packet::Less> Packets;
      Packets lpContent;

     public:
      PacketsList() {}
      PacketsList(const PacketsList& source)
         {  for (typename Packets::const_iterator iter = source.lpContent.begin();
                  iter != source.lpContent.end(); ++iter)
               if (*iter) lpContent.push_back(new Packet(**iter));
         }
      ~PacketsList()
         {  for (typename Packets::iterator iter = lpContent.begin(); iter != lpContent.end(); ++iter)
               if (*iter) delete *iter;
         }
      int count() const { return lpContent.size(); }
      void add(const vci_cmd_packet<vci_param>& p_in) { lpContent.insert(new Packet(p_in)); }
      Packet* remove(const vci_rsp_packet<vci_param>& p_out)
         {  Packet pLocate(p_out);
            typename std::set<Packet*, typename Packet::Less>::iterator iter = lpContent.find(&pLocate);
            Packet* pResult = NULL;
            if (iter != lpContent.end()) {
               pResult = *iter;
               lpContent.erase(iter);
            };
            return pResult;
         }
   };
   
   class AddressInterval {
     private:
      typename vci_param::addr_t aMin, aMax;
      uint32_t src_id;

     public:
      AddressInterval(uint32_t src_idSource)
         :  aMin(0), aMax(0), src_id(src_idSource) {}
      AddressInterval(uint32_t src_idSource,
            typename vci_param::addr_t aMinSource)
         :  aMin(aMinSource), aMax(aMinSource), src_id(src_idSource) {}
      AddressInterval(uint32_t src_idSource,
            typename vci_param::addr_t aMinSource, typename vci_param::addr_t aMaxSource)
         :  aMin(aMinSource), aMax(aMaxSource), src_id(src_idSource) {}

      AddressInterval& operator=(const AddressInterval& source)
         {  aMin = source.aMin; aMax = source.aMax; src_id = source.src_id; return *this; }
      bool operator<(const AddressInterval& source) const
         {  return aMax < source.aMin; }
      bool operator>(const AddressInterval& source) const
         {  return aMax > source.aMin; }
      bool operator==(const AddressInterval& source) const
         {  return aMax >= source.aMin && aMin <= source.aMax; }
      void merge(const AddressInterval& source)
         {  if (aMin > source.aMin)
               aMin = source.aMin;
            if (aMax < source.aMax)
               aMax = source.aMax;
         }
      bool isValid() const { return aMax > aMin; }
      bool remove(VciFilteredInitiator<vci_param>& filter, const AddressInterval& source, AddressInterval*& paiOther, bool& fError)
         {  fError = source.aMin < aMin || source.aMax > aMax;
            bool fResult = (source.aMin <= aMin && source.aMax >= aMax);
            if (!fResult) {
               if (aMin < source.aMin) {
                  filter.assume(src_id == source.src_id,
                        (((std::string("Conflict access : a locked was set by the initiator ")
                        += src_id) += " and accessed by the initiator ") += source.src_id).c_str(), DIn);
                  if (aMax > source.aMax) {
                     paiOther = new AddressInterval(src_id, source.aMax, aMax);
                     aMax = source.aMin;
                  }
                  else // aMax == source.aMax
                     aMax = source.aMin;
               }
               else { // aMin == source.aMin
                  if (aMax > source.aMax) {
                     filter.assume(src_id == source.src_id,
                        (((std::string("Conflict access : a locked was set by the initiator ")
                        += src_id) += " and accessed by the initiator ") += source.src_id).c_str(), DIn);
                     aMin = source.aMax;
                  };
               };
            }
            else
               filter.assume(src_id == source.src_id,
                  (((std::string("Conflict access : a locked was set by the initiator ")
                  += src_id) += " and accessed by the initiator ") += source.src_id).c_str(), DIn);
            return fResult;
         }
   };
   class LockedAddress {
     private:
      std::set<AddressInterval> saiAddresses;

     public:
      LockedAddress() {}
      LockedAddress(const LockedAddress& source) : saiAddresses(source.saiAddresses) {}

      void add(uint32_t srcid, typename vci_param::addr_t aAddress);
      void remove(VciFilteredInitiator<vci_param>& filter, uint32_t srcid, typename vci_param::addr_t aAddress);
      int count() const { return saiAddresses.size(); }
   };

   PacketsList m_pending_packets;
   LockedAddress m_locked_addresses;
   int m_nb_request_packets, m_nb_response_packets;

	VciFilteredInitiator(const std::string &name, typename VciInitiator<vci_param>::callback_t cb, tlmt_core::tlmt_thread_context *opt_ref = NULL)
		: VciInitiator<vci_param>(name, &m_call_back, opt_ref), m_call_back(this, cb),
        m_nb_request_packets(0), m_nb_response_packets(0) {}

   void check_response(vci_rsp_packet<vci_param>* pkt);
   void check_command(vci_cmd_packet<vci_param>* pkt);
   bool isFinished() const { return m_pending_packets.count() == 0 && m_locked_addresses.count() == 0; }
   void setLogOut(std::ostream& logFile) { m_log_file = &logFile; }
	void send(vci_cmd_packet<vci_param> *pkt, const tlmt_core::tlmt_time &time)
	{	check_command(pkt);
      /* return */ VciInitiator<vci_param>::send(pkt, time);
	}
};

}} // end of namespace soclib::tlmt

#endif // SOCLIB_TLMT_PORTS_FILTERH


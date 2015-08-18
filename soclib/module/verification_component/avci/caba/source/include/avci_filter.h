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

#ifndef SOCLIB_CABA_PVDC_ADVANCED_FILTERH
#define SOCLIB_CABA_PVDC_ADVANCED_FILTERH

#include "caba/caba_base_module.h"
#include "vci_initiator.h"
#include "vci_target.h"

#include <set>

namespace soclib { namespace caba {

template <typename vci_param>
class AdvancedVciFilter : public soclib::caba::BaseModule {
  private:
   std::ostream* m_log_file;
   bool m_default_mode;

  public:
 	typedef VciTarget<vci_param> In;
 	typedef VciInitiator<vci_param> Out;

   sc_in<bool> p_clk;
   sc_in<bool> p_resetn;
   In p_in;
   Out p_out;

  private:
   enum Direction { DIn, DOut };
   void writeError(const char* szError, Direction dDirection) const;
   void assume(bool fCondition, const char* szError, Direction dDirection) const
      {  if (!fCondition)
            writeError(szError, dDirection);
      }

   enum State { SIdle, SValid, SDefault_Ack, SSync };
   State m_request_state, m_response_state;

   void testHandshake(); // see p 41, 42, verified on the waveforms

   int m_reset;
   void setReset(int uResetSource = 8)
      {  if (!p_out.rspack && !p_in.cmdval)
            m_reset = 0;
         else
            m_reset = uResetSource;
      }
   void testReset() // see p.31
      {  if (!p_in.cmdval && !p_out.cmdack && !p_out.rspval && !p_in.rspack)
            m_reset = 0;
         else {
            assume(m_reset > 1, "Violation in the protocol : the reset command had no effect", DIn);
            --m_reset;
         };
      }

   class Packet {
     public:
      typename vci_param::cmd_t cmd;
      typename vci_param::addr_t address;
      typename vci_param::contig_t contig;
      typename vci_param::plen_t plen;
      typename vci_param::srcid_t srcid;
      typename vci_param::trdid_t trdid;
      typename vci_param::pktid_t pktid;
      
      Packet(const Out& p_out)
         :  cmd(0), address(0), contig(0), plen(0),
            srcid(p_out.rsrcid), trdid(p_out.rtrdid), pktid(p_out.rpktid) {}
     public:
      Packet(const In& p_in)
         :  cmd(p_in.cmd), address(p_in.address), contig(p_in.contig), plen(p_in.plen),
            srcid(p_in.srcid), trdid(p_in.trdid), pktid(p_in.pktid) {}
      Packet(const Packet& source)
         :  cmd(source.cmd), address(source.address), contig(source.contig), plen(source.plen),
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
               if (*iter) lpContent.insert(new Packet(**iter));
         }
      ~PacketsList()
         {  for (typename Packets::iterator iter = lpContent.begin(); iter != lpContent.end(); ++iter)
               if (*iter) delete *iter;
         }
      int count() const { return lpContent.size(); }
      void add(const In& p_in) { lpContent.insert(new Packet(p_in)); }
      Packet* remove(const Out& p_out)
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
      typename vci_param::srcid_t src_id;

     public:
      AddressInterval(typename vci_param::srcid_t src_idSource)
         :  aMin(0), aMax(0), src_id(src_idSource) {}
      AddressInterval(typename vci_param::srcid_t src_idSource,
            typename vci_param::addr_t aMinSource)
         :  aMin(aMinSource), aMax(aMinSource + vci_param::B), src_id(src_idSource) {}
      AddressInterval(typename vci_param::srcid_t src_idSource,
            typename vci_param::addr_t aMinSource, int plen)
         :  aMin(aMinSource), aMax(aMinSource + plen * vci_param::B), src_id(src_idSource) {}

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
      bool remove(AdvancedVciFilter& filter, const AddressInterval& source, AddressInterval*& paiOther, bool& fError)
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

      void add(typename vci_param::srcid_t srcid, typename vci_param::addr_t aAddress);
      void add(typename vci_param::srcid_t srcid, typename vci_param::addr_t aAddress, int plen);
      void remove(AdvancedVciFilter& filter, typename vci_param::srcid_t srcid, typename vci_param::addr_t aAddress);
      void remove(AdvancedVciFilter& filter, typename vci_param::srcid_t srcid, typename vci_param::addr_t aAddress, int plen);
      int count() const { return saiAddresses.size(); }
   };

   PacketsList m_pending_packets;
   LockedAddress m_locked_addresses;

   typename vci_param::clen_t m_packet_clen;
   typename vci_param::cfixed_t m_packet_cfixed;
   typename vci_param::addr_t m_packet_address;
   typename vci_param::plen_t m_packet_plen;
   typename vci_param::cmd_t m_packet_cmd;
   typename vci_param::contig_t m_packet_contig;
   typename vci_param::wrap_t m_packet_wrap;
   typename vci_param::const_t m_packet_cons;
   
   int m_request_cells, m_response_cells;
   int m_nb_request_packets, m_nb_response_packets;
   void acquireRequestCell();
   void acquireResponseCell();

   typename vci_param::val_t     m_cmdval_previous;
   typename vci_param::addr_t    m_address_previous;
   typename vci_param::be_t      m_be_previous;
   typename vci_param::cfixed_t  m_cfixed_previous;
   typename vci_param::clen_t    m_clen_previous;
   typename vci_param::cmd_t     m_cmd_previous;
   typename vci_param::contig_t  m_contig_previous;
   typename vci_param::data_t    m_wdata_previous;
   typename vci_param::eop_t     m_eop_previous;
   typename vci_param::const_t   m_cons_previous;
   typename vci_param::plen_t    m_plen_previous;
   typename vci_param::wrap_t    m_wrap_previous;

   typename vci_param::val_t     m_rspval_previous;
   typename vci_param::data_t    m_rdata_previous;
   typename vci_param::eop_t     m_reop_previous;
   typename vci_param::rerror_t  m_rerror_previous;
   int m_default_reset;

  protected:
   SC_HAS_PROCESS(AdvancedVciFilter);

  public:
   AdvancedVciFilter(sc_module_name insname)
      :  soclib::caba::BaseModule(insname), m_log_file(NULL), m_default_mode(true),
         p_in("input"), p_out("output"),
         m_request_state(SIdle), m_response_state(SIdle), m_reset(0), m_packet_address(0),
         m_request_cells(0), m_response_cells(0), m_nb_request_packets(0), m_nb_response_packets(0),
         m_cmdval_previous(0), m_default_reset(8)
      {  SC_METHOD(copy);
         dont_initialize();
         sensitive << p_in.cmdval << p_in.address << p_in.be << p_in.cfixed << p_in.clen << p_in.cmd
            << p_in.contig << p_in.wdata << p_in.eop << p_in.cons << p_in.plen << p_in.wrap << p_in.rspack
            << p_out.cmdack << p_out.rspval << p_out.rdata << p_out.reop << p_out.rerror;
         SC_METHOD(reset);
         dont_initialize();
         sensitive << p_resetn.pos();
         
         SC_METHOD(transition);
         dont_initialize();
         sensitive << p_clk.pos();
      }
   void setDefaultReset(int uDefaultReset) { m_default_reset = uDefaultReset; }
   void reset();
   void copy()
      {  p_out.cmdval = p_in.cmdval;
         p_out.address = p_in.address;
         p_out.be = p_in.be;
         p_out.cfixed = p_in.cfixed;
         p_out.clen = p_in.clen;
         p_out.cmd = p_in.cmd;
         p_out.contig = p_in.contig;
         p_out.wdata = p_in.wdata;
         p_out.eop = p_in.eop;
         p_out.cons = p_in.cons;
         p_out.plen = p_in.plen;
         p_out.wrap = p_in.wrap;
         p_out.rspack = p_in.rspack;
         p_in.cmdack = p_out.cmdack;
         p_in.rspval = p_out.rspval;
         p_in.rdata = p_out.rdata;
         p_in.reop = p_out.reop;
         p_in.rerror = p_out.rerror;
      }
   void transition() 
      {  testHandshake();
         if (m_reset > 0) testReset();

         m_cmdval_previous = p_in.cmdval;
         m_address_previous = p_in.address;
         m_be_previous = p_in.be;
         m_cfixed_previous = p_in.cfixed;
         m_clen_previous = p_in.clen;
         m_cmd_previous = p_in.cmd;
         m_contig_previous = p_in.contig;
         m_wdata_previous = p_in.wdata;
         m_eop_previous = p_in.eop;
         m_cons_previous = p_in.cons;
         m_plen_previous = p_in.plen;
         m_wrap_previous = p_in.wrap;

         m_rspval_previous = p_out.rspval;
         m_rdata_previous = p_out.rdata;
         m_reop_previous = p_out.reop;
         m_rerror_previous = p_out.rerror;
      }
   bool isFinished() const { return m_pending_packets.count() == 0 && m_locked_addresses.count() == 0; }
   
   void setLogOut(std::ostream& osOut) { m_log_file = &osOut; }
   void setDefaultMode() { m_default_mode = true; }
   void setFreeMode() { m_default_mode = false; }
};

}} // end of namespace soclib::caba

/*
#include <systemc.h>
#include <fstream>

#include "../include/avci_filter.h"

// A component with a vci iniator
template<typename vci_param>
class VciFstComponent : public soclib::caba::BaseModule {
  public:
   sc_in<bool> p_clk;
   soclib::caba::VciInitiator<vci_param>   p_vci;
   VciFstComponent(sc_module_name insname);
};

// A component with a vci target
template<typename vci_param>
class VciSndComponent : public soclib::caba::BaseModule {
  public:
   sc_in<bool> p_clk;
   soclib::caba::VciTarget<vci_param>   p_vci;
   VciSndComponent(sc_module_name insname);
};

// Specific size for vci-protocol
typedef soclib::caba::VciParams<4,1,32,1,1,1,8,1,1,1> MyVciParams;

int sc_main(int ac, char *av[]) {
  VciFstComponent<MyVciParams> vciFst("VciFstComponent");
  VciSndComponent<MyVciParams> vciSnd("VciSndComponent");
  sc_clock clk("Clock", 1, 0.5, 0.0);
  soclib::caba::VciSignals<MyVciParams> vciSignals("VciSignals");

  std::ofstream log_file("verif.log");
  soclib::caba::VciSignals<MyVciParams> vciSignalsVerif("VciSignals_verif");
  soclib::caba::AdvancedVciFilter<MyVciParams> vciFilter("VciFilter_verif");
  vciFilter.p_clk(clk);
  vciFilter.setLogOut(log_file);
  // vciFilter.activateFilter();
  vciFilter.p_in(vciSignals);
  vciFilter.p_out(vciSignalsVerif);
  
  vciFst.p_clk(clk);
  vciFst.p_vci(vciSignals);
  vciSnd.p_clk(clk);
  vciSnd.p_vci(vciSignalsVerif);

  sc_start(clk, -1);
  return 0;
}

*/

#endif // SOCLIB_CABA_PVDC_ADVANCED_FILTERH


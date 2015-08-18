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

#ifndef SOCLIB_CABA_PVDC_FILTERH
#define SOCLIB_CABA_PVDC_FILTERH

#include "caba/caba_base_module.h"
#include "vci_initiator.h"
#include "vci_target.h"

namespace soclib { namespace caba {

template <typename vci_param>
class PVciFilter : public soclib::caba::BaseModule {
  private:
   std::ostream* m_log_file;
   bool m_default_mode;

  public:
   struct In {
      sc_in<typename vci_param::val_t>     val;
      sc_in<typename vci_param::eop_t>     eop;
      sc_in<typename vci_param::cmd_t>     cmd;
      sc_in<typename vci_param::addr_t>    address;
      sc_in<typename vci_param::be_t>      be;
      sc_in<typename vci_param::data_t>    wdata;

   	sc_out<typename vci_param::data_t>    rdata;
      sc_out<typename vci_param::ack_t>     cmdack;
   	sc_out<typename vci_param::rerror_t>  rerror;


#define __ren(x) x((name+"_in_" #x).c_str())
      In(const std::string &name)
         : __ren(val), __ren(eop), __ren(cmd), __ren(address), __ren(be), __ren(wdata),
#undef __ren
#define __ren(x) x((name+"_out_" #x).c_str())
           __ren(rdata), __ren(cmdack), __ren(rerror) {}
#undef __ren

      void operator()(VciSignals<vci_param> &sig)
         {  val     (sig.cmdval);
            eop     (sig.eop);
            cmd     (sig.cmd);
            address (sig.address);
            be      (sig.be);
            wdata   (sig.wdata);
            rdata   (sig.rdata);
            cmdack  (sig.cmdack);
            rerror  (sig.rerror);
         }

	   void operator()(VciInitiator<vci_param> &ports) // To see : create a VciSignals between the ports
	      {  val     (ports.cmdval);
            eop     (ports.eop);
            cmd     (ports.cmd);
            address (ports.address);
            be      (ports.be);
            wdata   (ports.wdata);
            rdata   (ports.rdata);
            cmdack  (ports.cmdack);
            rerror  (ports.rerror);
         }
   };

   struct Out {
   	sc_in<typename vci_param::data_t>    rdata;
      sc_in<typename vci_param::ack_t>     cmdack;
   	sc_in<typename vci_param::rerror_t>  rerror;

      sc_out<typename vci_param::val_t>    val;
      sc_out<typename vci_param::eop_t>    eop;
      sc_out<typename vci_param::cmd_t>    cmd;
      sc_out<typename vci_param::addr_t>   address;
      sc_out<typename vci_param::be_t>     be;
      sc_out<typename vci_param::data_t>   wdata;

      Out(const std::string &name)
#define __ren(x) x((name+"_in_" #x).c_str())
         : __ren(rdata), __ren(cmdack), __ren(rerror),
#undef __ren
#define __ren(x) x((name+"_out_" #x).c_str())
           __ren(val), __ren(eop), __ren(cmd), __ren(address), __ren(be), __ren(wdata) {}
#undef __ren

      void operator()(VciSignals<vci_param> &sig)
         {  rdata   (sig.rdata);
            cmdack  (sig.cmdack);
            rerror  (sig.rerror);
            
            val     (sig.cmdval);
            eop     (sig.eop);
            cmd     (sig.cmd);
            address (sig.address);
            be      (sig.be);
            wdata   (sig.wdata);
         }

      void operator()(VciTarget<vci_param> &ports)
         {  rdata   (ports.rdata);
            cmdack  (ports.cmdack);
            rerror  (ports.rerror);

            val     (ports.cmdval);
            eop     (ports.eop);
            cmd     (ports.cmd);
            address (ports.address);
            be      (ports.be);
            wdata   (ports.wdata);
         }
   };

   sc_in<bool> p_clk;
   sc_in<bool> p_resetn;
   In p_in;
   Out p_out;

  private:
   enum Direction { DIn, DOut };
   void writeError(const char* szError, Direction dDirection) const
      {  (m_log_file ? *m_log_file : (std::ostream&) std::cout)
            << "ERROR : Protocol Error \""<< szError <<"\"on "<< name()
            << " for the packet " << m_nb_packets << ", the cell " << m_cells
            << " issued from " << ((dDirection == DOut) ? ((const char*) "response") : ((const char*) "request")) << " !!!\n";
      }
   void assume(bool fCondition, const char* szError, Direction dDirection) const
      {  if (!fCondition)
            writeError(szError, dDirection);
      }
   enum PeripheralHandShakeState { PHSSNone, PHSSValTriggered, PHSSAckVal };
   PeripheralHandShakeState m_handshake_state;
   void testHandshake() // verified on the waveforms of p.18-20
      {  switch (m_handshake_state) {
            case PHSSNone:
               if (p_in.val) {
                  acquireCell();
                  if (p_out.cmdack)
                     m_handshake_state = PHSSAckVal;
                  else
                     m_handshake_state = PHSSValTriggered;
               };
               // else // rule R2 p.16
               //   assume(!p_out.cmdack);
               break;
            case PHSSValTriggered:
               assume(p_in.val, "The peripheral protocol does not accept \"bubble\" in a VCI command packet", DIn);
               assume((p_in.address == m_address_previous) && (p_in.be == m_be_previous) // rule R1 p.16
                  && (p_in.cmd == m_cmd_previous) && (p_in.wdata == m_wdata_previous)
                  && (p_in.eop == m_eop_previous),
                  "The peripheral protocol does not accept \"change\" during acknowledgement", DIn);
               if (p_out.cmdack)
                  m_handshake_state = PHSSAckVal;
               break;
            case PHSSAckVal:
               if (!p_in.val) {
                  if (!p_out.cmdack)
                     m_handshake_state = PHSSNone;
                  else
                     m_handshake_state = PHSSNone; // rule R2 p.16
               }
               else {
                  acquireCell();
                  if (!p_out.cmdack) // rule R3 p.16
                     // assume((p_out.rdata == m_rdata_previous) && (p_out.rerror == m_rerror_previous));
                     m_handshake_state = PHSSValTriggered;
               };
               break;
         };
      }

   int m_reset;
   void setReset(int uResetSource = 8)
      {  if (!p_out.cmdack && !p_in.val)
            m_reset = 0;
         else
            m_reset = uResetSource;
      }
   void testReset() // see p.11
      {  if (!p_out.cmdack && !p_in.val)
            m_reset = 0;
         else {
            assume(m_reset > 1, "Violation in the peripheral protocol : the reset command had no effect", DIn);
            --m_reset;
         };
      }

   typename vci_param::addr_t m_previous_address;
   int m_cells;
   int m_nb_packets;
   void acquireCell()
      {  if (m_default_mode) {
            unsigned int uBE = p_in.be.read(); // see p.21
            if (uBE >= 4) {
               if (uBE >> 2 <= 2)
                  assume(uBE & 3 == 0, "The peripheral protocol accepts only VCI BE corresponding to known formats", DIn);
               else // uBE >> 2 == 3
                  assume(uBE & 3 == 0 || uBE & 3 == 3, "The peripheral protocol accepts only VCI BE corresponding to known formats", DIn);
            };
         };
         if (m_cells == 0)
            m_previous_address = p_in.address;
         else { // see p.13
            m_previous_address += vci_param::B; // cell_size()
            assume(m_previous_address == p_in.address, "The peripheral protocol accepts only increasing addresses in a given packet", DIn);
         };
         ++m_cells;
         if (p_in.eop) {
            m_cells = 0;
            m_previous_address = 0;
            ++m_nb_packets;
         };
      }
   typename vci_param::val_t      m_val_previous;
   typename vci_param::eop_t      m_eop_previous;
   typename vci_param::cmd_t      m_cmd_previous; // = rd
   typename vci_param::addr_t     m_address_previous;
   typename vci_param::be_t       m_be_previous;
   typename vci_param::data_t     m_wdata_previous;
   
  	typename vci_param::data_t     m_rdata_previous;
  	typename vci_param::rerror_t   m_rerror_previous;

   int m_default_reset;

  protected:
   SC_HAS_PROCESS(PVciFilter);

  public:
   PVciFilter(sc_module_name insname)
      :  soclib::caba::BaseModule(insname), m_log_file(NULL), m_default_mode(true),
         p_in((const char*) insname), p_out((const char*) insname),
         m_handshake_state(PHSSNone), m_reset(0), m_previous_address(0), m_cells(0),
         m_nb_packets(0), m_val_previous(0), m_default_reset(8)
      {  SC_METHOD(copy);
         dont_initialize();
         sensitive << p_in.val << p_in.eop << p_in.cmd << p_in.address
            << p_in.be << p_in.wdata << p_out.rdata << p_out.cmdack << p_out.rerror;
         SC_METHOD(reset);
         dont_initialize();
         sensitive << p_resetn.pos();
         
         SC_METHOD(transition);
         dont_initialize();
         sensitive << p_clk.pos();
      }
   void setDefaultReset(int uDefaultReset) { m_default_reset = uDefaultReset; }
   void reset()
      {  setReset(m_default_reset); }
   void copy()
      {  p_out.val = p_in.val;
         p_out.eop = p_in.eop;
         p_out.cmd = p_in.cmd;
         p_out.address = p_in.address;
         p_out.be = p_in.be;
         p_out.wdata = p_in.wdata;
         p_in.rdata = p_out.rdata;
         p_in.cmdack = p_out.cmdack;
         p_in.rerror = p_out.rerror;
      }
   void transition() 
      {  testHandshake();
         if (m_reset > 0) testReset();

         m_val_previous = p_in.val;
         m_eop_previous = p_in.eop;
         m_cmd_previous = p_in.cmd;
         m_address_previous = p_in.address;
         m_be_previous = p_in.be;
         m_wdata_previous = p_in.wdata;

         m_rdata_previous = p_out.rdata;
         m_rerror_previous = p_out.rerror;
      }
   void setLogOut(std::ostream& osOut) { m_log_file = &osOut; }
   void setDefaultMode() { m_default_mode = true; }
   void setFreeMode() { m_default_mode = false; }
};

}} // end of namespace soclib::caba

/*
#include <systemc.h>
#include <fstream>

#include "../include/pvci_filter.h"

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
  soclib::caba::PVciFilter<MyVciParams> vciFilter("VciFilter_verif");
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

#endif // SOCLIB_CABA_PVDC_FILTERH


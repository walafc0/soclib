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

#ifndef SOCLIB_CABA_PVDC_ASSERTH
#define SOCLIB_CABA_PVDC_ASSERTH

#include "caba/caba_base_module.h"
#include "vci_initiator.h"
#include "vci_target.h"

namespace soclib { namespace caba {

template <typename vci_param>
class PVciAssert : public soclib::caba::BaseModule {
  private:
   std::ostream* m_log_file;
   bool m_default_mode;

  public:
   VciSignals<vci_param>& r_observed_signals;
   sc_in<bool> p_clk;
   sc_in<bool> p_resetn;
   
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
               if (r_observed_signals.cmdval) {
                  acquireCell();
                  if (r_observed_signals.cmdack)
                     m_handshake_state = PHSSAckVal;
                  else
                     m_handshake_state = PHSSValTriggered;
               };
               // else // rule R2 p.16
               //   assume(!r_observed_signals.cmdack);
               break;
            case PHSSValTriggered:
               assume(r_observed_signals.cmdval, "The peripheral protocol does not accept \"bubble\" in a VCI command packet", DIn);
               assume((r_observed_signals.address == m_address_previous) && (r_observed_signals.be == m_be_previous) // rule R1 p.16
                  && (r_observed_signals.cmd == m_cmd_previous) && (r_observed_signals.wdata == m_data_previous)
                  && (r_observed_signals.eop == m_eop_previous),
                  "The peripheral protocol does not accept \"change\" during acknowledgement", DIn);
               if (r_observed_signals.cmdack)
                  m_handshake_state = PHSSAckVal;
               break;
            case PHSSAckVal:
               if (!r_observed_signals.cmdval) {
                  if (!r_observed_signals.cmdack)
                     m_handshake_state = PHSSNone;
                  else
                     m_handshake_state = PHSSNone; // rule R2 p.16
               }
               else {
                  acquireCell();
                  if (!r_observed_signals.cmdack) // rule R3 p.16
                     // assume((r_observed_signals.rdata == rdataPrevious) && (r_observed_signals.rerror == rerrorPrevious));
                     m_handshake_state = PHSSValTriggered;
               };
               break;
         };
      }

   int m_reset;
   void setReset(int uResetSource = 8)
      {  if (!r_observed_signals.cmdack && !r_observed_signals.cmdval)
            m_reset = 0;
         else
            m_reset = uResetSource;
      }
   void testReset() // see p.11
      {  if (!r_observed_signals.cmdack && !r_observed_signals.cmdval)
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
            unsigned int uBE = r_observed_signals.be.read(); // see p.21
            if (uBE >= 4) {
               if (uBE >> 2 <= 2)
                  assume(uBE & 3 == 0, "The peripheral protocol accepts only VCI BE corresponding to known formats", DIn);
               else // uBE >> 2 == 3
                  assume(uBE & 3 == 0 || uBE & 3 == 3, "The peripheral protocol accepts only VCI BE corresponding to known formats", DIn);
            };
         };
         if (m_cells == 0)
            m_previous_address = r_observed_signals.address;
         else { // see p.13
            m_previous_address += vci_param::B; // cell_size()
            assume(m_previous_address == r_observed_signals.address, "The peripheral protocol accepts only increasing addresses in a given packet", DIn);
         };
         ++m_cells;
         if (r_observed_signals.eop) {
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
   typename vci_param::data_t     m_data_previous;
   int m_default_reset;

  protected:
   SC_HAS_PROCESS(PVciAssert);

  public:
   PVciAssert(VciSignals<vci_param>& observedSignalsReference, sc_module_name insname)
      :  soclib::caba::BaseModule(insname), m_log_file(NULL), m_default_mode(true),
         r_observed_signals(observedSignalsReference), m_handshake_state(PHSSNone), m_reset(0),
         m_previous_address(0), m_cells(0), m_nb_packets(0), m_val_previous(0), m_default_reset(8)
      {  SC_METHOD(reset);
         dont_initialize();
         sensitive << p_resetn.pos();

         SC_METHOD(transition);
         dont_initialize();
         sensitive << p_clk.pos();
      }
   void setDefaultReset(int uDefaultReset) { m_default_reset = uDefaultReset; }
   void reset()
      {  setReset(m_default_reset); }
   void transition() 
      {  testHandshake();
         if (m_reset > 0) testReset();

         m_val_previous = r_observed_signals.cmdval;
         m_eop_previous = r_observed_signals.eop;
         m_cmd_previous = r_observed_signals.cmd;
         m_address_previous = r_observed_signals.address;
         m_be_previous = r_observed_signals.be;
         m_data_previous = r_observed_signals.wdata;
      }
   void setLogOut(std::ostream& osOut) { m_log_file = &osOut; }
   void setDefaultMode() { m_default_mode = true; }
   void setFreeMode() { m_default_mode = false; }
};

}} // end of namespace soclib::caba

/*
#include <systemc.h>
#include <fstream>

#include "caba_base_module.h"
#include "vci_initiator.h"
#include "vci_target.h"
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
  soclib::caba::PVciAssert<MyVciParams> vciAssert(vciSignals, "VciAssert_verif");
  vciAssert.p_clk(clk);
  vciAssert.setLogOut(log_file);
  
  vciFst.p_clk(clk);
  vciFst.p_vci(vciSignals);
  vciSnd.p_clk(clk);
  vciSnd.p_vci(vciSignals);

  sc_start(clk, -1);
  return 0;
}

*/

#endif // SOCLIB_CABA_PVDC_ASSERTH


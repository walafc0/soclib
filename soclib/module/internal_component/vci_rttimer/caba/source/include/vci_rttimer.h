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
 * Copyright (c) Telecom ParisTech
 *         Alexandre Becoulet <alexandre.becoulet@telecom-paristech.fr>, 2011
 *
 */

#ifndef SOCLIB_VCI_RTTIMER_H
#define SOCLIB_VCI_RTTIMER_H

#include <systemc>

#include "vci_target_fsm.h"
#include "caba_base_module.h"
#include "mapping_table.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciRtTimer
	: public caba::BaseModule
{
private:
  soclib::caba::VciTargetFsm<vci_param, true> m_vci_fsm;

  bool on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be);
  bool on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data);
  void transition();
  void genMoore();

  size_t m_deadlines;
  bool m_separate_irqs;

  sc_signal<uint32_t>  r_sccnt;
  sc_signal<uint32_t>  r_scrld;
  sc_signal<uint32_t>  r_ctrl;
  sc_signal<uint64_t>  r_rtc;
  sc_signal<uint32_t>  r_pe;
  uint32_t  m_ie;
  uint32_t  m_ip;
  sc_signal<uint32_t>  r_rtc_tmp;

  sc_signal<uint64_t> *r_dln;
  sc_signal<uint32_t> *r_dlnp;

  // internal regs
  sc_signal<uint32_t>  r_data;
  sc_signal<int> *r_cmd;

  enum cmd_e {
    r_cmd_compare,
    r_cmd_dln_set,
    r_cmd_dln_add,
    r_cmd_dln_period,
  };

protected:
  SC_HAS_PROCESS(VciRtTimer);

public:
  sc_in<bool> p_clk;
  sc_in<bool> p_resetn;
  soclib::caba::VciTarget<vci_param> p_vci;
  sc_out<bool> *p_irq;

  VciRtTimer(sc_module_name name, const IntTab &index,
	     const MappingTable &mt, size_t deadlines, bool separate_irqs);

  ~VciRtTimer();
};

}}

#endif


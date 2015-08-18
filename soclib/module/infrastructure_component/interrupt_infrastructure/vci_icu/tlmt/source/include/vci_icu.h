/* -*- mode: c++; coding: utf-8 -*-
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
 * Maintainers: fpecheux, nipo, alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Nicolas Pouillon <nipo@ssji.net>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef SOCLIB_TLMT_VCI_ICU_H
#define SOCLIB_TLMT_VCI_ICU_H

#include <tlmt>
#include "vci_param.h"
#include "vci_ports.h"
#include "tlmt_base_module.h"
#include "mapping_table.h"
#include "icu.h"

struct irq_struct{
  bool val;
  tlmt_core::tlmt_time time;
};

namespace soclib { namespace tlmt {

template <typename vci_param>
class VciIcu
    : public soclib::tlmt::BaseModule
{
 private:

  irq_struct *irq;
  
  soclib::common::IntTab m_index;
  soclib::common::MappingTable m_mt;
  std::list<soclib::common::Segment> segList;

  vci_rsp_packet<vci_param> rsp;

  size_t m_nirq;
  unsigned int r_mask;
  unsigned int r_current;

protected:
  SC_HAS_PROCESS(VciIcu);
public:
  soclib::tlmt::VciTarget<vci_param> p_vci;
  tlmt_core::tlmt_out<bool> p_irqOut;
  std::vector<tlmt_core::tlmt_in<bool> *> p_irqIn;
    
  VciIcu(
	 sc_core::sc_module_name name,
	 const soclib::common::IntTab &index,
	 const soclib::common::MappingTable &mt,
	 size_t nirq);
  
  void callback(soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		const tlmt_core::tlmt_time &time,
		void *private_data);
  
  void callback_read(size_t segIndex,soclib::common::Segment &s,
		     soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		     const tlmt_core::tlmt_time &time,
		     void *private_data);
  
  void callback_write(size_t segIndex,soclib::common::Segment &s,
		      soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		      const tlmt_core::tlmt_time &time,
		      void *private_data);

  void irqReceived(bool val,
		   const tlmt_core::tlmt_time &time,
		   void *private_data);
  
  void behavior();
  
  int getInterruption();
  
  unsigned int getActiveInterruptions(const tlmt_core::tlmt_time time);
  
 };
}}

#endif /* SOCLIB_TLMT_VCI_ICU_H */

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

#ifndef SOCLIB_TLMT_VCI_LOCKS_H
#define SOCLIB_TLMT_VCI_LOCKS_H

#include <tlmt>
#include "vci_param.h"
#include "vci_ports.h"
#include "tlmt_base_module.h"
#include "mapping_table.h"
#include "soclib_endian.h"

namespace soclib { namespace tlmt{

template<typename vci_param>
class VciLocks
    : public soclib::tlmt::BaseModule
{
 private:
    soclib::common::IntTab m_index;
    soclib::common::MappingTable m_mt;

    std::list<soclib::common::Segment> segList;
    typedef bool ram_t;
    ram_t **m_contents;
    vci_rsp_packet<vci_param> rsp;

 protected:
	SC_HAS_PROCESS(VciLocks);

 public:
    soclib::tlmt::VciTarget<vci_param> p_vci;

    VciLocks(sc_core::sc_module_name name,
             const soclib::common::IntTab &index,
             const soclib::common::MappingTable &mt);

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

};

}}

#endif /* SOCLIB_TLMT_VCI_LOCKS_H */

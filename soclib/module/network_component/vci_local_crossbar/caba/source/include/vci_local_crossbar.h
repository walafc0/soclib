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
 * Copyright (c) UPMC, Lip6, Asim
 *         Alain Greiner <alain.greiner@lip6.fr>, 2005
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: alain
 */

#ifndef SOCLIB_CABA_VCI_LOCAL_CROSSBAR_H_
#define SOCLIB_CABA_VCI_LOCAL_CROSSBAR_H_

#include <systemc>
#include "caba_base_module.h"
#include "vci_initiator.h"
#include "vci_target.h"
#include "vci_buffers.h"
#include "mapping_table.h"
#include "address_decoding_table.h"

namespace soclib { namespace caba {

using namespace soclib::common;

template<typename pkt_t> class SimpleCrossbar;

////////////////////////////////////
template<typename vci_param>
class VciLocalCrossbar
////////////////////////////////////
    : public BaseModule
{
public:

    sc_in<bool>                               p_clk;
    sc_in<bool>                               p_resetn;

    VciInitiator<vci_param>                  *p_to_target;
    VciTarget<vci_param>                     *p_to_initiator;
    VciTarget<vci_param>                      p_target_to_up;
    VciInitiator<vci_param>                   p_initiator_to_up;

private:

    size_t                                    m_nb_attached_initiators;
    size_t                                    m_nb_attached_targets;

    AddressDecodingTable<uint64_t, size_t>    m_cmd_rt;   // command routing table
    AddressDecodingTable<uint64_t, bool>      m_cmd_lt;   // command locality table

    AddressDecodingTable<uint32_t, size_t>    m_rsp_rt;   // response routing table
    AddressDecodingTable<uint32_t, bool>      m_rsp_lt;   // response locality table

    VciInitiator<vci_param>                 **m_ports_to_target;
    VciTarget<vci_param>                    **m_ports_to_initiator;

    SimpleCrossbar<VciCmdBuffer<vci_param> > *m_cmd_crossbar;
    SimpleCrossbar<VciRspBuffer<vci_param> > *m_rsp_crossbar;

    void transition();
    void genMealy();

protected:
    SC_HAS_PROCESS(VciLocalCrossbar);

public:
    void print_trace();

    VciLocalCrossbar( sc_core::sc_module_name             name,
					  const soclib::common::MappingTable  &mt,
					  const size_t                        cluster_id,
					  const size_t                        nb_attached_initiators,
					  const size_t                        nb_attached_targets,
                      const size_t                        default_target_id );
    ~VciLocalCrossbar();
};

}}

#endif /* SOCLIB_CABA_VCI_LOCAL_CROSSBAR_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

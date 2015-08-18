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
 *         Alain Greiner <alain.greiner@lip6.fr> 2005
 *         Nicolas Pouillon <nipo@ssji.net> 2007
 *         Alain Greiner <alain.greiner@lip6.fr> 2013
 *
 * Maintainers: alain
 */

#ifndef SOCLIB_CABA_VCI_VGMN_H_
#define SOCLIB_CABA_VCI_VGMN_H_

#include <systemc>
#include "caba_base_module.h"
#include "vci_initiator.h"
#include "vci_target.h"
#include "vci_buffers.h"
#include "address_decoding_table.h"
#include "address_masking_table.h"
#include "mapping_table.h"
#include "generic_fifo.h"

namespace soclib { namespace caba {

using namespace sc_core;
using namespace soclib::caba;
using namespace soclib::common;

template<typename vci_flit_t, 
         typename vci_input_t,
         typename vci_output_t> class VgmnMicroNetwork;

///////////////////////////////////
template<typename vci_param>
class VciVgmn
    : public soclib::caba::BaseModule
{
public:
    sc_in<bool>                                 p_clk;
    sc_in<bool>                                 p_resetn;

    VciInitiator<vci_param>                     *p_to_target;
    VciTarget<vci_param>                        *p_to_initiator;

private:
    const size_t                                m_nb_initiators;
    const size_t                                m_nb_targets;

    AddressDecodingTable<uint64_t, size_t>      m_cmd_rt;
    AddressDecodingTable<uint32_t, size_t>      m_rsp_rt;
 
    VgmnMicroNetwork<VciCmdBuffer<vci_param>,
                     VciTarget<vci_param>,
                     VciInitiator<vci_param> >  *m_cmd_mn;

    VgmnMicroNetwork<VciRspBuffer<vci_param>,
                     VciInitiator<vci_param>,
                     VciTarget<vci_param> >     *m_rsp_mn;

    void transition();
    void genMoore();

protected:
    SC_HAS_PROCESS(VciVgmn);

public:
    void print_trace();

    VciVgmn( sc_module_name                      name,
              const soclib::common::MappingTable &mt,
              size_t                             nb_initiators,
              size_t                             nb_targets,
              size_t                             min_latency,
              size_t                             fifo_depth,
              const size_t                       default_index = 0 );

    ~VciVgmn();
};

}}

#endif /* SOCLIB_CABA_VCI_VGMN_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

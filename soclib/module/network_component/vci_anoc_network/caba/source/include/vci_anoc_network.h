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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors : Ivan MIRO PANADES
 * 
 * History :
 *
 * Comment :
 *
 */

#ifndef VCI_ANOC_NETWORK_H_
#define VCI_ANOC_NETWORK_H_

#include <systemc>
#include "caba_base_module.h"
#include "vci_target.h"
#include "vci_initiator.h"
#include "mapping_table.h"
#include "vci_anoc_wrapper.h"

#include "anoc_common.h"
#include "anoc_node.h"
//#include "anoc_debug.h"
#include "anoc_stopper.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template<typename vci_param, int anoc_fifo_size, int anoc_yx_size>
        class VciAnocNetwork
        : public soclib::caba::BaseModule
        {

            public:
                sc_in<bool>		p_clk;
                sc_in<bool>		p_resetn;

                soclib::caba::VciInitiator<vci_param> **p_to_target;
                soclib::caba::VciTarget<vci_param>    **p_to_initiator;

            protected:
                SC_HAS_PROCESS(VciAnocNetwork);

            private:
                anoc_node    ***anoc_router;
                anoc_stopper  **empty_north;
                anoc_stopper  **empty_south;
                anoc_stopper  **empty_east;
                anoc_stopper  **empty_west;

                VciAnocWrapper<vci_param, anoc_fifo_size, anoc_yx_size> ***vci_anoc_wrapper;

                int m_width_network;
                int m_height_network;

            public:
                VciAnocNetwork( sc_module_name name,
                                const soclib::common::MappingTable &mt,
                                size_t width_network,   //X
                                size_t height_network); //Y

                ~VciAnocNetwork();
        };
}}
//end

#endif //VCI_ANOC_NETWORK_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

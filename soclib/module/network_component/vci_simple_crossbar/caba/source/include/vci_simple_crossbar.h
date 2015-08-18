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
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Based on previous works by Alain Greiner, 2005
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_CABA_VCI_SIMPLE_CROSSBAR_H_
#define SOCLIB_CABA_VCI_SIMPLE_CROSSBAR_H_

#include <systemc>
#include "caba_base_module.h"
#include "vci_initiator.h"
#include "vci_target.h"
#include "vci_buffers.h"
#include "address_decoding_table.h"
#include "address_masking_table.h"
#include "mapping_table.h"

namespace soclib { namespace caba {

using namespace sc_core;

namespace _crossbar {

template<typename pkt_t> class Crossbar;

}

template<typename vci_param>
class VciSimpleCrossbar
    : public soclib::caba::BaseModule
{
public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;

    soclib::caba::VciInitiator<vci_param> *p_to_target;
    soclib::caba::VciTarget<vci_param> *p_to_initiator;

private:
    size_t m_nb_initiat;
    size_t m_nb_target;

    typedef _crossbar::Crossbar<VciCmdBuffer<vci_param> > cmd_crossbar_t;
    typedef _crossbar::Crossbar<VciRspBuffer<vci_param> > rsp_crossbar_t;

    void transition();
    void genMealy();

	cmd_crossbar_t *m_cmd_crossbar;
	rsp_crossbar_t *m_rsp_crossbar;

	static soclib::common::IntTab s_default_target;

protected:
    SC_HAS_PROCESS(VciSimpleCrossbar);

public:
    VciSimpleCrossbar( sc_module_name name,
					   const soclib::common::MappingTable &mt,
					   size_t nb_initiat,
					   size_t nb_target,
					   const soclib::common::IntTab &index = soclib::common::IntTab(),
					   const soclib::common::IntTab &default_target = s_default_target );
};

}}

#endif /* SOCLIB_CABA_VCI_SIMPLE_CROSSBAR_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

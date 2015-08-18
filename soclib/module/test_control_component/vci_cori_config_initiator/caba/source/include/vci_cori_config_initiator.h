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
 * Copyright (c) UPMC, Lip6, SoC
 *         Etienne Le Grand <etilegr@hotmail.com>, 2009
 */
 
#ifndef SOCLIB_CABA_VCI_CORI_CONFIG_INITIATOR_H
#define SOCLIB_CABA_VCI_CORI_CONFIG_INITIATOR_H

#include <inttypes.h>
#include <systemc>
#include "caba_base_module.h"
#include "vci_initiator.h"

namespace soclib {
namespace caba {

    using namespace sc_core;

template<typename    vci_param>
class VciCoriConfigInitiator
    : public soclib::caba::BaseModule
{
private:
    typedef uint32_t addr_t;
    typedef uint32_t data_t;

    enum initiator_fsm_state_e {
                INITIATOR_IDLE       = 0,
                INITIATOR_REQ_READ  = 1,
                INITIATOR_RSP_READ  = 2
    };

public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    soclib::caba::VciInitiator<vci_param>   p_vci;

private:

    // STRUCTURAL PARAMETERS

    // REGISTERS
    sc_signal<int>      r_initiator_fsm;
    sc_signal<int>      r_reg1;

	
protected:
    SC_HAS_PROCESS(VciCoriConfigInitiator);

public:
    VciCoriConfigInitiator(
        sc_module_name insname );

    ~VciCoriConfigInitiator();

private:
    void transition();
    void genMoore();
};

}}

#endif /* SOCLIB_CABA_VCI_CORI_CONFIG_INITIATOR_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


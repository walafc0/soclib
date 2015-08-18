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
 
#ifndef SOCLIB_CABA_HHT_TARGET_H
#define SOCLIB_CABA_HHT_TARGET_H

#include <systemc>
#include "caba_base_module.h"
#include "hht_target.h"
#include "lazy_fifo.h"

namespace soclib {
namespace caba {

    using namespace sc_core;

template<typename hht_param>
class HhtTestelgTarget
    : public soclib::caba::BaseModule
{
private:

    enum target_fsm_state_e {
                TARGET_IDLE     = 0,
                TARGET_SENDING  = 1
    };

public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    soclib::caba::HhtTarget<hht_param> p_hht;
    
private:

    // STRUCTURAL PARAMETERS

    // REGISTERS
	LazyFifo<HhtCmdFlit<hht_param> > f_ctrlPCO;
    LazyFifo<HhtCmdFlit<hht_param> > f_ctrlNPCO;
    LazyFifo<typename hht_param::data_t> f_dataPCO;
    LazyFifo<typename hht_param::data_t> f_dataNPCO;
	LazyFifo<HhtRspFlit<hht_param> > f_ctrlRI;
    LazyFifo<typename hht_param::data_t> f_dataRI;
    sc_signal<int>      r_target_fsm;
	sc_signal<int>      r_target_dec;
	
protected:
    SC_HAS_PROCESS(HhtTestelgTarget);

public:
    HhtTestelgTarget(
        sc_module_name insname );

    ~HhtTestelgTarget();

private:
    void transition();
    void genMoore();
};

}}

#endif /* SOCLIB_CABA_HHT_TARGET_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


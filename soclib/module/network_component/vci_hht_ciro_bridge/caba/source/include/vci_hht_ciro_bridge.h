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
 
#ifndef SOCLIB_CABA_VCI_HHT_CIRO_BRIDGE_H
#define SOCLIB_CABA_VCI_HHT_CIRO_BRIDGE_H

#include <inttypes.h>
#include <systemc>
#include "caba_base_module.h"
#include "vci_target.h"
#include "vci_initiator.h"
#include "hht_target.h"
#include "lazy_fifo.h"
#include "vci_flits.h"
namespace soclib {
namespace caba {

    using namespace sc_core;
template<typename vci_param, typename hht_param>
class VciHhtCiroBridge
    : public soclib::caba::BaseModule
{
private:
    enum hhtci_fsm_state_e {
		HHTCI_RESET		= 0,
		HHTCI_IDLE		= 1,
		HHTCI_WPOSTED	= 2,
		HHTCI_WNPOSTED	= 3,
		HHTCI_ATOMIC		= 4,
		HHTCI_CS			= 5
    };
    enum hhtro_fsm_state_e {
		HHTRO_RESET		= 0,
		HHTRO_IDLE		= 1,
		HHTRO_SENDING	= 2,
		HHTRO_FILLATID	= 3,
		HHTRO_FILLID		= 4,
		HHTRO_LL1		= 5,
		HHTRO_LL2		= 6,
		HHTRO_ATSUCCESS	= 7
		
    };
public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    soclib::caba::VciInitiator<vci_param>   p_vci_io;
	soclib::caba::VciTarget<vci_param>   	p_vci_config;
	soclib::caba::HhtTarget<hht_param> 		p_hht;
    
public:
	// STRUCTURAL PARAMETERS
	static const int nb_ids	= 8;
    static const int nb_atids	= 8;
    
    // REGISTERS
    LazyFifo<VciCmdFlit<vci_param> > f_vciCI;
    LazyFifo<VciRspFlit<vci_param> > f_vciRO;
	
    LazyFifo<VciCmdFlit<vci_param> > f_ROtoCI;
    LazyFifo<HhtRspFlit<hht_param> > f_CItoRO;
    LazyFifo<int > f_vciids;
    LazyFifo<int > f_atids;
    
	LazyFifo<HhtCmdFlit<hht_param> > f_ctrlPCI;
    LazyFifo<HhtCmdFlit<hht_param> > f_ctrlNPCI;
    LazyFifo<typename hht_param::data_t> f_dataPCI;
    LazyFifo<typename hht_param::data_t> f_dataNPCI;
	LazyFifo<HhtRspFlit<hht_param> > f_ctrlRO;
    LazyFifo<typename hht_param::data_t> f_dataRO;

private:	
	sc_signal<int>      r_hhtci_fsm;
    sc_signal<int>		r_hhtci_dec;
    sc_signal<typename hht_param::addr_t>		r_hhtci_addr;
    sc_signal<typename hht_param::data_t>		r_hhtci_mask;
    
	sc_signal<int>      r_hhtro_fsm;
	sc_signal<int>		r_reset_numid;
	
	
	sc_signal<typename hht_param::ctrl_t>		r_ctrl_table[nb_ids+nb_atids];
	sc_signal<typename hht_param::data_t>		r_atdata_table[nb_atids][6];
	sc_signal<bool>								r_sumoverflow;
	sc_signal<typename hht_param::data_t>		r_attmpdata;
protected:
    SC_HAS_PROCESS(VciHhtCiroBridge);

public:
    VciHhtCiroBridge(
        sc_module_name insname );

    ~VciHhtCiroBridge();
private:
    void transition();
    void genMoore();
};

}}

#endif /* SOCLIB_CABA_VCI_HHT_CIRO_BRIDGE_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


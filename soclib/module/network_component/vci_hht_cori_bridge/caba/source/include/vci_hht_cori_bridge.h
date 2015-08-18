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
 
#ifndef SOCLIB_CABA_VCI_HHT_CORI_BRIDGE_H
#define SOCLIB_CABA_VCI_HHT_CORI_BRIDGE_H

#include <inttypes.h>
#include <systemc>
#include "caba_base_module.h"
#include "vci_target.h"
#include "hht_initiator.h"
#include "lazy_fifo.h"
#include "vci_flits.h"
namespace soclib {
namespace caba {

    using namespace sc_core;
template <typename vci_param>
struct CoToRi{
	typename vci_param::rerror_t rerror;
	typename vci_param::srcid_t  rsrcid;
	typename vci_param::trdid_t  rtrdid;
	typename vci_param::pktid_t  rpktid;
	VciRspFlit<vci_param> get_vci_rsp_flit(){
		VciRspFlit<vci_param> res;
		res.rsrcid=rsrcid;
		res.rtrdid=rtrdid;
		res.rpktid=rpktid;
		res.rerror=rerror;
		res.reop=1;
		res.rdata=0;
		return res;
	}
};

template<typename vci_param, typename hht_param>
class VciHhtCoriBridge
    : public soclib::caba::BaseModule
{
private:
    enum vcico_fsm_state_e {
                VCICO_RESET		= 0,
                VCICO_IDLE		= 1,
                VCICO_SENDING	= 2
    };
    enum hhtco_fsm_state_e {
                HHTCO_RESET		= 0,
                HHTCO_IDLE		= 1,
                HHTCO_SENDING	= 2
    };
    enum vciri_fsm_state_e {
                VCIRI_RESET		= 0,
                VCIRI_IDLE		= 1,
                VCIRI_SENDING	= 2,
				VCIRI_FILLID	= 3
    };

public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    soclib::caba::VciTarget<vci_param>   p_vci_io;
	soclib::caba::VciTarget<vci_param>   p_vci_config;
	soclib::caba::HhtInitiator<hht_param> p_hht;
	
public:
	// STRUCTURAL PARAMETERS
	static const int nb_ids	= 8;
    
    // REGISTERS
	LazyFifo<HhtCmdFlit<hht_param> > f_ctrlPCO;
    LazyFifo<HhtCmdFlit<hht_param> > f_ctrlNPCO;
    LazyFifo<typename hht_param::data_t> f_dataPCO;
    LazyFifo<typename hht_param::data_t> f_dataNPCO;
	LazyFifo<HhtRspFlit<hht_param> > f_ctrlRI;
    LazyFifo<typename hht_param::data_t> f_dataRI;
	
    LazyFifo<VciCmdFlit<vci_param> > f_vciCO;
    LazyFifo<VciRspFlit<vci_param> > f_vciRI;
	
    LazyFifo<CoToRi<vci_param> > f_COtoRI;
    LazyFifo<int > f_hhtids;
	
    LazyFifo<HhtCmdFlit<hht_param> > f_ctrlCO;
	LazyFifo<typename hht_param::data_t> f_maskCO;
	LazyFifo<typename hht_param::data_t> f_dataCO;

private:	
	sc_signal<int>      r_vcico_fsm;
    sc_signal<int>		r_vcico_dec;
    sc_signal<typename vci_param::addr_t>	r_vcico_addr;
    sc_signal<int>		r_vcico_mask[7];
    sc_signal<bool>		r_vcico_masked;
	
	sc_signal<int>      r_hhtco_fsm;
    sc_signal<int>      r_hhtco_dec;
    sc_signal<typename hht_param::ctrl_t>    r_ctrlCO;
	
	sc_signal<int>      r_vciri_fsm;
	sc_signal<int>      r_vciri_dec;
	sc_signal<int>		r_reset_numid;
	
	sc_signal<int>      r_srcid_table[nb_ids];
	sc_signal<int>      r_trdid_table[nb_ids];
	sc_signal<int>      r_pktid_table[nb_ids];
	
protected:
    SC_HAS_PROCESS(VciHhtCoriBridge);

public:
    VciHhtCoriBridge(
        sc_module_name insname );

    ~VciHhtCoriBridge();
private:
    void transition();
    void genMoore();
};

}}

#endif /* SOCLIB_CABA_VCI_HHT_CORI_BRIDGE_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


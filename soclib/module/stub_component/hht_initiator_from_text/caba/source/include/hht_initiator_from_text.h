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
#ifndef SOCLIB_CABA_HHT_INITIATOR_FROM_TEXT_H
#define SOCLIB_CABA_HHT_INITIATOR_FROM_TEXT_H

#include <inttypes.h>
#include <stdio.h>
#include <systemc>
#include "caba_base_module.h"
#include "hht_initiator.h"
#include "lazy_fifo.h"
namespace soclib {
namespace caba {

    using namespace sc_core;

template<typename    hht_param>
class HhtInitiatorFromText
    : public soclib::caba::BaseModule
{
private:

    enum r_cmd_fsm_state_e {
                CMD_RESET	= 0,
                CMD_IDLE	= 1,
                CMD_SEND	= 2
    };
    enum r_rsp_fsm_state_e {
                RSP_RESET	= 0,
                RSP_IDLE	= 1,
		RSP_RECEIVE	= 2
    };

public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
	soclib::caba::HhtInitiator<hht_param> 	p_hht;
private:

    // NON-CABA
	FILE *m_file_cmd;
    char *m_sfilename;
	bool has_to_loop;
    bool init_sending[1024];
	bool waiting;
	bool inside_command;
	
	// REGISTERS
	LazyFifo<HhtCmdFlit<hht_param> > f_ctrlPCI;
    LazyFifo<HhtCmdFlit<hht_param> > f_ctrlNPCI;
    LazyFifo<typename hht_param::data_t> f_dataPCI;
    LazyFifo<typename hht_param::data_t> f_dataNPCI;
	LazyFifo<HhtRspFlit<hht_param> > f_ctrlRO;
    LazyFifo<typename hht_param::data_t> f_dataRO;
	
    sc_signal<int>      r_cmd_fsm;
    sc_signal<int>      r_rsp_fsm;
	HhtCmdFlit<hht_param>		m_hht_cmd;
	typename hht_param::data_t		m_hht_cmd_data;
	HhtRspFlit<hht_param>		m_hht_rsp;
	typename hht_param::data_t		m_hht_rsp_data;
	
	
protected:
    SC_HAS_PROCESS(HhtInitiatorFromText);

public:
    HhtInitiatorFromText(
        sc_module_name insname );

    ~HhtInitiatorFromText();
	bool start_send(char* filename,bool loop);
private:
    void transition();
    void genMoore();
};

}}

#endif /* SOCLIB_CABA_HHT_INITIATOR_FROM_TEXT_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


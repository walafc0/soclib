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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2006
 *
 * Maintainers: alain nipo
 */

#include "../include/pibus_bcu.h"
#include "alloc_elems.h"

#define Pibus soclib::caba::Pibus

namespace soclib { namespace caba {

///////////////////////////////////////
//	constructor
///////////////////////////////////////

PibusBcu::PibusBcu (	sc_module_name 				name,
                        const soclib::common::MappingTable 	&maptab,
                        size_t 					nb_master,
                        size_t 					nb_slave,
                        uint32_t 				time_out)
	: soclib::caba::BaseModule(name),
      m_target_table(maptab.getRoutingTable(soclib::common::IntTab())),
      m_nb_master(nb_master),
      m_nb_target(nb_slave),
      m_time_out(time_out),
      p_clk("clk"),
      p_resetn("resetn"),
      p_req(soclib::common::alloc_elems<sc_in<bool> >("req", m_nb_master)),
      p_gnt(soclib::common::alloc_elems<sc_out<bool> >("gnt", m_nb_master)),
      p_sel(soclib::common::alloc_elems<sc_out<bool> >("sel", m_nb_target)),
      //p_a("a"),
      //p_lock("lock"),
      //p_ack("ack"),
      //p_tout("tout"),
      p_pi("pi"),
      r_fsm_state("fsm_state"),
      r_current_master("current_master"),
      r_tout_counter("tout_counter"),
      r_req_counter(soclib::common::alloc_elems<sc_signal<size_t> >("req_counter", m_nb_master)),
      r_wait_counter(soclib::common::alloc_elems<sc_signal<uint32_t> >("wait_counter", m_nb_master))
{
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	sensitive << p_clk.neg();

	SC_METHOD(genMealy_sel);
	sensitive << p_clk.neg();
	sensitive << p_pi.a;

	SC_METHOD(genMealy_gnt);
	sensitive << p_clk.neg();
	for (size_t i = 0 ; i < m_nb_master; i++)
        sensitive << p_req[i];

	if (!m_target_table.isAllBelow( m_nb_target )) 
		throw soclib::exception::ValueError(
           "At least one target index is larger than the number of targets");

#ifdef SOCLIB_MODULE_DEBUG
	std::cout << this->name() << ": " << m_target_table << std::endl;
#endif
}

PibusBcu::~PibusBcu()
{
    soclib::common::dealloc_elems(p_req, m_nb_master);
    soclib::common::dealloc_elems(p_gnt, m_nb_master);
    soclib::common::dealloc_elems(p_sel, m_nb_target);
    soclib::common::dealloc_elems(r_req_counter, m_nb_master);
    soclib::common::dealloc_elems(r_wait_counter, m_nb_master);
}

/////////////////////////////////////
//	transition()
/////////////////////////////////////
void PibusBcu::transition()
{
    if (p_resetn == false) {
        r_fsm_state = FSM_IDLE;
        r_current_master = 0;
        for(size_t i = 0 ; i < m_nb_master ; i++) {
            r_req_counter[i] = 0;
            r_wait_counter[i] = 0;
        }
        return;
    } // end p_resetn

    for(size_t i = 0 ; i < m_nb_master ; i++) {
        if(p_req[i])
            r_wait_counter[i] = r_wait_counter[i] + 1;
	}
	
    switch(r_fsm_state) {
	case FSM_IDLE:
        r_tout_counter = m_time_out;
        for(size_t i = 0 ; i < m_nb_master ; i++) {
            int j = (i + 1 + r_current_master) % m_nb_master;
            if(p_req[j]) {
                r_current_master = j;
                r_req_counter[j] = r_req_counter[j] + 1;
                r_fsm_state = FSM_AD;
                break;
            }
        } // end for
        break;

	case FSM_AD:
        if(p_pi.lock)   r_fsm_state = FSM_DTAD;  
        else	     r_fsm_state = FSM_DT; 
        break;

	case FSM_DTAD:
        if(r_tout_counter == 0) {
            r_fsm_state = FSM_IDLE;
        } else if(p_pi.ack.read() == Pibus::ACK_RDY) {
            if(p_pi.lock == false) { r_fsm_state = FSM_DT; } 
        } else if((p_pi.ack.read() == Pibus::ACK_ERR) || (p_pi.ack.read() == Pibus::ACK_RTR)) {
            r_fsm_state = FSM_IDLE;
        } else { 
            r_tout_counter = r_tout_counter - 1;
        }
        break;

	case FSM_DT:
        if(r_tout_counter == 0) {
            r_fsm_state = FSM_IDLE;
        } else if(p_pi.ack.read() != Pibus::ACK_WAT) { // new allocation
            r_tout_counter = m_time_out;
            bool found = false;
            for(size_t i = 0 ; i < m_nb_master ; i++) {
                int j = (i + 1 + r_current_master) % m_nb_master;
                if((p_req[j] == true) && (found == false)) {
                    r_current_master = j;
                    r_req_counter[j] = r_req_counter[j] + 1;
                    found = true;
                    break;
                }
            } // end for
            if(found == true)
                r_fsm_state = FSM_AD; 
            else
                r_fsm_state = FSM_IDLE; 
        } else { 
            r_tout_counter = r_tout_counter - 1;
        }
        break;
    } // end switch FSM
}

//////////////////////////////////////////////////////:
//	genMealy_gnt()
//////////////////////////////////////////////////////:
void PibusBcu::genMealy_gnt()
{
    bool	found = false;
    if((r_fsm_state == FSM_IDLE) || 
       ((r_fsm_state == FSM_DT) && (p_pi.ack.read() != Pibus::ACK_WAT))) {
        for(size_t i = 0 ; i < m_nb_master ; i++) {
            int j = (i + 1 + r_current_master) % m_nb_master;
            if((p_req[j] == true) && (found == false)) {
                p_gnt[j] = true;
                found = true;
            } else {
                p_gnt[j] = false;
            }
        } // end for
    } else {
        for (size_t i = 0 ; i < m_nb_master ; i++) {
            p_gnt[i] = false;
        } // end for
    } // end if
}

//////////////////////////////////////////////////////:
//	genMealy_sel()
//////////////////////////////////////////////////////:
void PibusBcu::genMealy_sel()
{
    if((r_fsm_state == FSM_AD) || (r_fsm_state == FSM_DTAD)) {
        size_t index = m_target_table[p_pi.a.read()];
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << name() << ": index=" << std::dec << index
				<< " addr=" << std::hex << p_pi.a.read() << std::endl;
#endif
        for(size_t i = 0; i < m_nb_target ; i++) {
            if(i == index)  	p_sel[i] = true;
            else			p_sel[i] = false;
        } // end for
    } else {
        for(size_t i = 0 ; i < m_nb_target ; i++) {
            p_sel[i] = false;
        } // end for
    } // end if
} // end genMealy_sel()

//////////////////////////////////////////////////////:
//	genMoore
//////////////////////////////////////////////////////:
void PibusBcu::genMoore()
{
	p_pi.tout = (r_tout_counter == 0);
}  // end genMoore 

}} // end namespace


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

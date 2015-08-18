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
 * Copyright (c) Lab-STICC, UBS
 *         Caaliph Andriamisaina <andriami@univ-ubs.fr>, 2008
 *
*/

#include <math.h>

#include "../include/fir128.h"
#include "soclib_endian.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, int fifo_depth> x Fir128<vci_param, fifo_depth>

tmpl(void)::transition()
{
	if ( p_resetn.read() == false ) {
			m_state = FIFO_FIR_READ;
			m_cycles_left = fifo_depth;
			m_ptr = 0;
			return;
	}

	if ( m_cycles_left > 0 ) {
		switch (m_state) {
		case FIFO_FIR_WAIT:
			--m_cycles_left;
			break;
		case FIFO_FIR_READ:
			if ( p_from_ctrl.rok.read() ) {
				uint32_t buffer = le_to_machine((uint32_t)p_from_ctrl.data);
				((uint32_t*)&m_input_buffer[0])[m_ptr++] = buffer;
				--m_cycles_left;
			}
		default:
			break;
		}
	}
	if ( m_cycles_left == 0 ) {
		switch (m_state) {
		case FIFO_FIR_WAIT:
			m_state = FIFO_FIR_WRITE;
			m_cycles_left = sizeof(m_output_buffer)/sizeof(uint32_t);
			m_ptr = 0;
			break;
		case FIFO_FIR_WRITE:
			m_state = FIFO_FIR_READ;
			m_cycles_left = sizeof(m_input_buffer)/sizeof(uint32_t);
			m_ptr = 0;
			break;
		case FIFO_FIR_READ:
			m_state = FIFO_FIR_EXEC;
			m_cycles_left = 0;
			break;
		case FIFO_FIR_EXEC:
			do_fir128();
			m_state = FIFO_FIR_WAIT;
			m_cycles_left = m_work_latency;
			break;
		}
	}
	if ( m_cycles_left > 0 ) {
		switch (m_state) {
		case FIFO_FIR_WRITE:
			if ( p_to_ctrl.wok.read() ) {
				m_recv_buffer = ((uint32_t*)&m_output_buffer[0])[m_ptr++];
				--m_cycles_left;
			}
		default:
			break;
		}
	}
}

tmpl(void)::genMoore()
{
	p_from_ctrl.r = (m_state == FIFO_FIR_READ);
	p_to_ctrl.w = (m_state == FIFO_FIR_WRITE);
	if (m_state == FIFO_FIR_WRITE)
		p_to_ctrl.data = le_to_machine((uint32_t)m_recv_buffer);
}

tmpl(void)::do_fir128()
{
      /*double*/int sum, delay[128];
      /*double*/int coef[128] = {0,     -8,      7,    -15,     12,    -19,     14,    -19,     10,
      -12,     -2,      2,    -20,     20,    -40,     36,    -53,     40,
      -50,     26,    -27,    -10,     14,    -59,     63,   -106,     99,
     -131,    102,   -115,     59,    -50,    -30,     52,   -142,    160,
     -241,    232,   -281,    226,   -228,    119,    -73,    -81,    154,
     -323,    387,   -530,    538,   -605,    515,   -472,    258,    -91,
     -244,    515,   -936,   1261,  -1705,   2015,  -2407,   2628,  -2899,
     2972,  2972,  -2899,   2628,  -2407,   2015,  -1705,   1261,
     -936,    515,   -244,    -91,    258,   -472,    515,   -605,    538,
     -530,    387,   -323,    154,    -81,    -73,    119,   -228,    226,
     -281,    232,   -241,    160,   -142,     52,    -30,    -50,     59,
     -115,    102,   -131,     99,   -106,     63,    -59,     14,    -10,
      -27,     26,    -50,     40,    -53,     36,    -40,     20,    -20,
        2,     -2,    -12,     10,    -19,     14,    -19,     12,    -15,
        7,     -8,      0};
     
      for(int i=0; i < 128; i++){
         delay [i] = 0;
      }
      for (int j = 0; j < fifo_depth; j++) {
      	sum = 0;
	delay[0] = m_input_buffer[j];
        for(int i=0; i < 128; i++){
            sum += delay[i]*coef[i];                         
        }
        for(int i = 127; i>0; i--) {
            delay[i] = delay[i-1];
        } 
        ///sum = sum >> 15; 
        m_output_buffer[j] = sum;
     }
}
	
tmpl()::Fir128(
	sc_core::sc_module_name insname,
	int ncycles)
	   : soclib::caba::BaseModule(insname),
	   m_work_latency(ncycles)	   
{

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}
}}

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

#include "../include/demapping.h"
#include "soclib_endian.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, int fifo_depth> x Demapping<vci_param, fifo_depth>

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
			do_demapping();
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

tmpl(void)::do_demapping()
{	
	int i_symb, bits_index;	

	bits_index = 0;	
	for (i_symb=0 ; i_symb < fifo_depth/2 ; i_symb++)
	{
	      // Demap I first
	      if(m_input_buffer[bits_index] < 0)
		      m_output_buffer[bits_index] = 1;
	      else
		      m_output_buffer[bits_index] = 0;
	      bits_index++;
	      // Demap Q last
	      if(m_input_buffer[bits_index] < 0)
		      m_output_buffer[bits_index] = 1;
	      else
		      m_output_buffer[bits_index] = 0;
	      bits_index++;
	}
}
	
tmpl()::Demapping(
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

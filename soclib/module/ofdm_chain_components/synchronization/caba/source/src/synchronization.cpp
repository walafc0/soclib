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

#include "../include/synchronization.h"
#include "soclib_endian.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, int fifo_depth> x Synchronization<vci_param, fifo_depth>

tmpl(void)::transition()
{
	if ( p_resetn.read() == false ) {
			m_state = FIFO_SYNCHRONIZATION_READ;
			m_cycles_left = fifo_depth;
			m_ptr = 0;
			return;
	}

	if ( m_cycles_left > 0 ) {
		switch (m_state) {
		case FIFO_SYNCHRONIZATION_WAIT:
			--m_cycles_left;
			break;
		case FIFO_SYNCHRONIZATION_READ:
			if ( p_from_ctrl.rok.read() ) {
				word_t buffer = le_to_machine((word_t)p_from_ctrl.data);
				((word_t*)&m_input_buffer[0])[m_ptr++] = buffer;
				--m_cycles_left;
			}
		default:
			break;
		}
	}
	if ( m_cycles_left == 0 ) {
		switch (m_state) {
		case FIFO_SYNCHRONIZATION_WAIT:
			m_state = FIFO_SYNCHRONIZATION_WRITE;
			m_cycles_left = sizeof(m_output_buffer)/sizeof(word_t);
			m_ptr = 0;
			break;
		case FIFO_SYNCHRONIZATION_WRITE:
			m_state = FIFO_SYNCHRONIZATION_READ;
			m_cycles_left = sizeof(m_input_buffer)/sizeof(word_t);
			m_ptr = 0;
			break;
		case FIFO_SYNCHRONIZATION_READ:
			m_state = FIFO_SYNCHRONIZATION_EXEC;
			m_cycles_left = 0;
			break;
		case FIFO_SYNCHRONIZATION_EXEC:
			do_synchronization();
			m_state = FIFO_SYNCHRONIZATION_WAIT;
			m_cycles_left = m_work_latency;
			break;
		}
	}
	if ( m_cycles_left > 0 ) {
		switch (m_state) {
		case FIFO_SYNCHRONIZATION_WRITE:
			if ( p_to_ctrl.wok.read() ) {
				m_recv_buffer = ((word_t*)&m_output_buffer[0])[m_ptr++];
				--m_cycles_left;
			}
		default:
			break;
		}
	}
}

tmpl(void)::genMoore()
{
	p_from_ctrl.r = (m_state == FIFO_SYNCHRONIZATION_READ);
	p_to_ctrl.w = (m_state == FIFO_SYNCHRONIZATION_WRITE);
	if (m_state == FIFO_SYNCHRONIZATION_WRITE)
		p_to_ctrl.data = le_to_machine((word_t)m_recv_buffer);
}

tmpl(void)::do_synchronization()
{
	int LgFFT = 256;  // 128 256 512 ou 1024 ?
	int tailleSynch = LgFFT/2;  //1 2 ou 4 ?
	int surech = 4; // 1 ou 4
	int So = tailleSynch; // valeur dependant du rapport signal à bruit et longueur synchro
	int nS = 10; // nombre de symbole OFDM par trame
	int tailleIG = LgFFT/8;  // / 4 8 16
	double accu;
	int i, l, N ;

	N = surech*(LgFFT+tailleIG);
	double delay[N];
	// initialisation
	accu = 0;
	l = 0;
	for (i = 0; i <N; i++)
		delay[i] = 0;

	//for (l = 1; :length(xei)
	while(/*1*/l<fifo_depth)   {
	   // autocorrélation attention 
	   accu = accu - delay[0]*delay[N/2+1]; // multiplication complexe
	   // decalage registre ou buffer circulaire
	   for(i = N-1; i>0; i--) {
		    delay[i] = delay[i-1];
	   }
	   accu = accu + delay[N/2]*delay[N];

	   if (abs(accu) > So) {      
	      accu = 0;
	      // vider la memoire
	      for (int k=0; k<N; k++)
		 m_output_buffer[k] = delay[k];
	      // acquisition de la trame 
	      for (int l= l+1; l<l+nS*N; l++)
		 m_output_buffer[l] = m_input_buffer[l]; // mettre en sortie apres lecture d'une donnée
	   }
	   l++;
	}
}
	
tmpl()::Synchronization(
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

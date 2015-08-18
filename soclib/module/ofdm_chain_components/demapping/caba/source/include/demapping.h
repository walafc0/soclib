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
	
#ifndef SOCLIB_DEMAPPING_H_
#define SOCLIB_DEMAPPING_H_

#include <systemc>

#include "caba_base_module.h"
#include "fifo_ports.h"

namespace soclib { namespace caba {

template <typename vci_param, int fifo_depth>
class Demapping
        : public soclib::caba::BaseModule
{
	public:
    	   sc_core::sc_in<bool> p_clk;
	   sc_core::sc_in<bool> p_resetn;
	
           soclib::caba::FifoOutput<uint32_t> p_to_ctrl;
           soclib::caba::FifoInput<uint32_t> p_from_ctrl;
	
	private:
           int m_work_latency;
           int m_cycles_left;
	   
           uint32_t m_recv_buffer;

           enum {
                FIFO_FIR_READ,
                FIFO_FIR_EXEC,
                FIFO_FIR_WAIT,
                FIFO_FIR_WRITE
           } m_state;
           int32_t m_input_buffer[fifo_depth];
           int32_t m_output_buffer[fifo_depth];
           size_t m_ptr;

	protected:
	    SC_HAS_PROCESS(Demapping);
	
	public:
	    Demapping(sc_core::sc_module_name insname, int ncycles);	
	private:
	    void transition();
            void genMoore();
        void do_demapping();
	
};
	
}}
	
#endif /* SOCLIB_FIFO_FIR128_H_ */

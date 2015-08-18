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
	
#ifndef SOCLIB_FIFO_CHANNEL_ESTIMATION_H_
#define SOCLIB_FIFO_CHANNEL_ESTIMATION_H_

#include <systemc>

#include "caba_base_module.h"
#include "fifo_ports.h"

namespace soclib { namespace caba {

template <typename vci_param>
class Channel_estimation
        : public soclib::caba::BaseModule
{
	public:
    	   sc_core::sc_in<bool> p_clk;
	   sc_core::sc_in<bool> p_resetn;
	
           soclib::caba::FifoOutput<word_t> p_to_ctrl;
           soclib::caba::FifoInput<word_t> p_from_ctrl;
	
	private:
           int m_work_latency;
           int m_cycles_left;

           word_t m_recv_buffer;

        enum {
                FIFO_CHEST_READ,
                FIFO_CHEST_EXEC,
                FIFO_CHEST_WAIT,
                FIFO_CHEST_WRITE
        } m_state;

        int32_t m_input_buffer[64];
        int32_t m_output_buffer[64];
        size_t m_ptr;

	protected:
	    SC_HAS_PROCESS(Channel_estimation);
	
	public:
	    FifoFir128(sc_core::sc_module_name insname, int ncycles);
	
	private:
	    void transition();
            void genMoore();
        void do_chest();
	
};
	
}}
	
#endif /* SOCLIB_FIFO_CHANNEL_ESTIMATION_H_ */

/*
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
 * Copyright (c) CITI/INSA, 2009
 * 
 * Authors:
 * 	Ludovic L'Hours <ludovic.lhours@insa-lyon.fr>
 * 	Antoine Fraboulet <antoine.fraboulet@insa-lyon.fr>
 * 	Tanguy Risset <tanguy.risset@insa-lyon.fr>
 * 
 */

#ifndef FIFO_ADAPTOR_H
#define FIFO_ADAPTOR_H

#include <iostream>
#include <systemc.h>
#include <base_module.h>
#include <fifo_ports.h>

namespace soclib { namespace caba {

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
class FifoAdaptor : BaseModule {
   
public:
    sc_in  < bool > p_clk;
    sc_in  < bool > p_resetn;

	FifoInput<  INPUT_TYPE >  p_input;
	FifoOutput< OUTPUT_TYPE > p_output;

private:

	//sc_signal<bool> rok__w;
	//sc_signal<bool> r__wok;

	void transition ();
    void genMoore ();
	void genMealy();

protected: 
    SC_HAS_PROCESS (FifoAdaptor);
  
public:
    FifoAdaptor (sc_module_name insname);

	~FifoAdaptor();
};				

}} // end of soclib::caba

#endif


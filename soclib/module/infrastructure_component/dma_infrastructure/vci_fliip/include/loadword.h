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

#ifndef LOADWORD_H
#define LOADWORD_H

#include <iostream>
#include <systemc.h>
#include <base_module.h>
#include <fifo_ports.h>

namespace soclib { namespace caba {

template <unsigned int INPUT_BITWIDTH,
	unsigned int OUTPUT_BITWIDTH ,
	bool SWAP_IN = false, bool SWAP_OUT = false >
class LoadWord : BaseModule {
   
public:
    sc_in  < bool > p_clk;
    sc_in  < bool > p_resetn;

	FifoInput<  sc_uint< INPUT_BITWIDTH > >  p_input;
	FifoOutput< sc_uint< OUTPUT_BITWIDTH > > p_output;

private:

	void transition ();
    void genMoore ();
	void genMealy();

	struct State {
		unsigned int num;
		bool outputValid;
		bool outputValidKnown;
		bool doLoad;
		bool isLast;
		unsigned int loadPosition;
		State *next;
	
		State(int n) : num(n), outputValid(false), doLoad(false),
			isLast(false), next(NULL) { }
		void setDoLoad(unsigned int position) {
			doLoad = true;
			loadPosition = position;
		}
		void setOutputValid(bool valid) {
			outputValid = valid;
			outputValidKnown = true;
		}
		friend std::ostream& operator<<(std::ostream& stream, const State& state) {
			stream << state.num << " " << (state.outputValidKnown ?
				(state.outputValid ? "Valid " : "NotValid ") : "Unknown ")
				<< (state.doLoad ? (signed int)state.loadPosition : -1) << " "
				<< (state.next ? state.next->num : 0);
			return stream;
		}
	};

	State *start, *current;
	unsigned int shiftRegSize;
	unsigned long long data;

	void build();

protected: 
    SC_HAS_PROCESS (LoadWord);
  
public:
    LoadWord (sc_module_name insname);

	~LoadWord () {
		// delete states from start
	}
};				

}} // end of soclib::caba

#endif


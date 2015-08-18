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

#include "soclib_accelerator_fifo.h"

namespace soclib { namespace caba {

#define tmpl(x) \
template < unsigned int BITWIDTH, unsigned int SIZE > \
x AcceleratorFifo<BITWIDTH, SIZE>

tmpl(void)::transition () {
	unsigned int read  = 0;
	unsigned int write = 0;
	unsigned int data  = 0;

	if (p_resetn == false) {
		DPRINTF_FIFO("** Reset : empty fifo\n");
		while (fifo.rok())
			fifo.simple_get();
	}

	if (DATA_IN.WRITE == true && DATA_IN.WRITEOK == true) {
		write = 1;
		data  = DATA_IN.DATA.read();
	}

	if (DATA_OUT.WRITE == true && DATA_OUT.WRITEOK == true) {
		read  = 1;
	}

	if (read && !write) {
		unsigned int olddata = fifo.read();
		fifo.simple_get();
		DPRINTF_FIFO("simple_get 0x%08x (size was %d)\n",olddata,fifo.filled_status());
	} else if (!read && write) {
		fifo.simple_put(data);
		DPRINTF_FIFO("simple_put 0x%08x (size was %d)\n",data,fifo.filled_status());
	} else if (read && write) {
		unsigned int olddata = fifo.read();
		fifo.put_and_get(data);
		DPRINTF_FIFO("put 0x%08x and get 0x%08x (size was %d)\n",data,olddata,fifo.filled_status());
	}
}

tmpl(void)::genMoore () {
	if (fifo.wok()) {
		DATA_IN.WRITEOK = true;
	} else {
		DATA_IN.WRITEOK = false;
    }

	if (fifo.rok()) {
		DATA_OUT.WRITE = true;
		DATA_OUT.DATA  = fifo.read();
	} else {
		DATA_OUT.WRITE = false;
	}
}
  
tmpl(/**/)::AcceleratorFifo (sc_module_name insname)
	: BaseModule(insname), fifo(insname, SIZE) {
	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD (genMoore);
	dont_initialize();
	sensitive << p_clk.neg();

#ifdef DEBUG_FIFO
	fifo_out = fopen(insname, "w");
	if (fifo_out == NULL) 
	    std::cerr << insname << " cannot open fifo_out log" << endl;
#endif
}

tmpl(/**/)::~AcceleratorFifo() {
#ifdef DEBUG_FIFO
	if (fifo_out)
		fclose(fifo_out);
#endif
}

}} // end of namespace soclib::caba


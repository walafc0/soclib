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

#include <clockenable.h>

namespace soclib { namespace caba {

#define tmpl(x) \
template < unsigned int NTOIP_FIFO , unsigned int NFRIP_FIFO, \
unsigned int CTRL_DATA_SIZE, unsigned int CTRL_ADDR_SIZE > \
x ClockEnable<NTOIP_FIFO, NFRIP_FIFO, CTRL_DATA_SIZE, CTRL_ADDR_SIZE>

tmpl(void)::write_super(unsigned int n, unsigned int data) {
	r_super_memory[n].length = (data>>16) & 0xFFFF;
	r_super_memory[n].count  = data & 0xFFFF;
	if (n > r_super_max)
		r_super_max = n;
}

tmpl(void)::write_phase(unsigned int n, unsigned int data) {
	r_phase_memory[n].length = (data>>16) & 0xFFFF;
	r_phase_memory[n].count  = data & 0xFFFF;
	if (n > r_phase_max)
		r_phase_max = n;
}

tmpl(void)::write_pattern(unsigned int n, unsigned int data) {
	r_pattern_memory[n].input  = (data>>16) & 0xFFFF;
	r_pattern_memory[n].output = data & 0xFFFF;
	if (n > r_pattern_max)
		r_pattern_max = n;
}

tmpl(void)::start() {
	r_current_super   = 0;
	r_current_phase   = 0;
	r_current_pattern = 0;
	r_pattern_base    = 0;
	r_super_count     = 0;
	r_phase_count     = 0;
	r_pattern_count   = 0;
	r_pattern_offset  = 0;
}

tmpl(void)::print_tables() {
	std::cout << "=== Super Phase table ===" << std::endl;
	for (unsigned int i = 0; i <= r_super_max; ++i) {
		std::cout << std::hex << r_super_memory[i].length << " "
			<< std::hex << r_super_memory[i].count << std::endl;
	}
	std::cout << "=== Phase table ===" << std::endl;
	for (unsigned int i = 0; i <= r_phase_max; ++i) {
		std::cout << std::hex << r_phase_memory[i].length << " "
			<< std::hex << r_phase_memory[i].count << std::endl;
	}
	std::cout << "=== Pattern table ===" << std::endl;
	for (unsigned int i = 0; i <= r_pattern_max; ++i) {
		std::cout << std::hex << r_pattern_memory[i].input << " "
			<< std::hex << r_pattern_memory[i].output << std::endl;
	}
}

tmpl(void)::next_virtual_cycle() {

	++r_pattern_offset;
	if (r_pattern_offset < r_phase_memory[r_current_phase + r_phase_count].length) {
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "nvc: next pattern "
				<< (r_current_pattern + r_pattern_offset) << std::endl;
#endif
		return;
	}
		
	++r_pattern_count;
	r_pattern_offset = 0;
	if (r_pattern_count < r_phase_memory[r_current_phase + r_phase_count].count) {
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "nvc: repeat phase " << r_pattern_count << std::endl;
#endif
		return;
	}
	
	r_current_pattern += r_phase_memory[r_current_phase + r_phase_count].length;
	++r_phase_count;
	r_pattern_count = 0;
	if (r_phase_count < r_super_memory[r_current_super].length) {
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "nvc: next phase " << (r_current_phase + r_phase_count) << std::endl;
#endif
		return;
	}
			
	++r_super_count;
	r_phase_count = 0;
	if (r_super_count < r_super_memory[r_current_super].count) {
		r_current_pattern = r_pattern_base;
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "nvc: repeat super phase " << r_super_count << std::endl;
#endif
		return;
	}
	
	r_current_phase += r_super_memory[r_current_super].length;
	++r_current_super;
	r_pattern_base = r_current_pattern;
	r_super_count = 0;
	if (r_current_super <= r_super_max) {
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "nvc: next super phase " << r_current_super << std::endl;
#endif
		return;
	}

#ifdef SOCLIB_MODULE_DEBUG
	std::cout << "nvc: stop" << std::endl;
#endif
	running = false;
}

tmpl(void)::transition () {
	if (p_resetn == false) {
		running = false;
		r_super_max   = 0;
		r_phase_max   = 0;
		r_pattern_max = 0;
		return;
	}

	if (p_enbl.WRITE == true) {
		unsigned int addr = p_enbl.ADDR.read();
		unsigned int data = p_enbl.DATA.read();
		unsigned int mask = 3 << (CTRL_ADDR_SIZE-2);
		unsigned int type = addr>>(CTRL_ADDR_SIZE-2);
		if (type == 0) {
			write_super(addr & ~mask, data);
		} else if (type == 1) {
			write_phase(addr & ~mask, data);
		} else {
			write_pattern(addr & ~mask, data);
		}
	}

	if (p_start == true && running == false) {
		start();
#ifdef SOCLIB_MODULE_DEBUG
		print_tables();
#endif
		running = true;
	}

	if (running && clk_enable) {
		next_virtual_cycle();
	}
}

tmpl(void)::genMoore () {
	p_enbl.WRITEOK = true;
}

tmpl(void)::genMealy() {

	// TODO : flush pipeline -> toip_running == false && frip_running == true ?
	if (running == true) {
		bool all_ok;
		bool toip_vc = true;
		bool frip_vc = true;
		unsigned int toip_motif = r_pattern_memory[
				r_current_pattern + r_pattern_offset].input;
		unsigned int frip_motif = r_pattern_memory[
				r_current_pattern + r_pattern_offset].output;

		for (unsigned int i = 0; i < NTOIP_FIFO; ++i) {
			if (toip_motif & (1 << i)) {
				if (p_toip_write[i] == false) {
					toip_vc = false;
				}
			}
		}

		for (unsigned int i = 0; i < NFRIP_FIFO; ++i) {
			if (frip_motif & (1 << i)) {
				if ((bool)p_frip_writeok[i] == false) {
					frip_vc = false;
				}
			}
		}

		all_ok = toip_vc && frip_vc;
		for(unsigned int i = 0; i < NTOIP_FIFO; ++i) {
			p_toip_writeok[i] = all_ok && ((toip_motif & (1 << i)) != 0);
		}
		for(unsigned int i = 0; i < NFRIP_FIFO; ++i) {
			p_frip_write[i] = all_ok && ((frip_motif & (1 << i)) != 0);
		}
		clk_enable = all_ok;
	} else {
		for(unsigned int i = 0; i < NTOIP_FIFO; ++i) {
			p_toip_writeok[i] = false;
		}
		for(unsigned int i = 0; i < NFRIP_FIFO; ++i) {
			p_frip_write[i] = false;
		}
		clk_enable = false;
	}
	p_ce = clk_enable;
}

tmpl(/**/)::ClockEnable(sc_module_name insname)
		: soclib::caba::BaseModule(insname) {
	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();
    
	SC_METHOD (genMoore);
	dont_initialize();
	sensitive << p_clk.neg();

	SC_METHOD (genMealy);
	dont_initialize();
	sensitive << p_clk.neg();
	for(unsigned int i = 0; i < NTOIP_FIFO; ++i) {
		sensitive << p_toip_write[i];
	}
	for(unsigned int i = 0; i < NFRIP_FIFO; ++i) {
		sensitive << p_frip_writeok[i];
 	}

}

tmpl(/**/)::~ClockEnable () {
}

}} // end of soclib::caba


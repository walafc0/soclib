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

#include <loadword.h>


namespace soclib { namespace caba {

#define tmpl(x) \
template < unsigned int INPUT_BITWIDTH, unsigned int OUTPUT_BITWIDTH, \
bool SWAP_IN, bool SWAP_OUT > \
x LoadWord<INPUT_BITWIDTH, OUTPUT_BITWIDTH, SWAP_IN, SWAP_OUT>


inline unsigned long long pad(unsigned int size, unsigned int total, unsigned long long data) {
	return data << (total - size);
}

inline unsigned long long range(unsigned long long data, unsigned int from, unsigned int to) {
	return (data >> to) & ((1 << (from-to+1)) - 1);
}

inline unsigned long long concat(unsigned long long data, unsigned long long other, unsigned int size) {
	return data << size | other;
}

inline unsigned long long swap(bool doSwap, unsigned int size, unsigned long long value) {
	if (! doSwap || (size % 8) != 0)
		return value;
	unsigned long long newValue = 0;
	for (unsigned int i = 0; i < size; i+=8) {
		newValue |= ((value>>i) & 0xFF) << (size-i-8);
	}
	return newValue;
}

tmpl(void)::transition () {

	if (p_resetn == false) {
		current = start;
		return;
    }

	unsigned long long next_data = data;
	State * next_state = current;

	bool cond = p_output.wok;
	if (current->doLoad) {
		if (! current->outputValidKnown || current->outputValid) {
			cond = p_input.rok && p_output.wok;
#ifdef SOCLIB_MODULE_DEBUG
			//std::cout << "lw(" << name() << ") cond both " << cond << std::endl;
#endif
		} else {
			cond = p_input.rok;
#ifdef SOCLIB_MODULE_DEBUG
			//std::cout << "lw(" << name() << ") cond input " << cond << std::endl;
#endif
		}
	}
	if (cond) {
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "lw(" << name() << ") current=|" << (*current) << "|" << std::endl;
		std::cout << "lw(" << name() << ") input.r=" << p_input.r << std::endl;
#endif
		next_state = current->next;
		if (current->doLoad) {
			unsigned long long in_data = swap(SWAP_IN, INPUT_BITWIDTH, p_input.data.read());
#ifdef SOCLIB_MODULE_DEBUG
			std::cout << "lw(" << name() << ") lw_in=" << std::hex << in_data << std::endl;
#endif
			if (current->loadPosition > 0) {
				if (! current->outputValidKnown || current->outputValid) {
					// shift the internal register
					next_data = pad(current->loadPosition + INPUT_BITWIDTH, shiftRegSize,
						concat(
							range(data, shiftRegSize - OUTPUT_BITWIDTH - 1,
								shiftRegSize - OUTPUT_BITWIDTH - current->loadPosition),
							in_data, INPUT_BITWIDTH));
				} else {
					// don't shift the internal register, as data are not consumed
					next_data = pad(current->loadPosition + INPUT_BITWIDTH, shiftRegSize,
						concat(
							range(data, shiftRegSize - 1,
								shiftRegSize - current->loadPosition),
							in_data, INPUT_BITWIDTH));
				}
			} else {
				next_data = pad(INPUT_BITWIDTH, shiftRegSize, in_data);
			}
		} else {
			next_data = pad(shiftRegSize - OUTPUT_BITWIDTH, shiftRegSize,
				range(data, shiftRegSize - OUTPUT_BITWIDTH - 1, 0));
		}
	} else if (current->isLast && p_output.wok) {
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "lw(" << name() << ") doLast"  << std::endl;
#endif
		next_state = start;
	}
#ifdef SOCLIB_MODULE_DEBUG
	if (cond) {
		std::cout << "lw(" << name() << ") next_data="  << std::hex << next_data << std::endl;
		std::cout << "lw(" << name() << ") next_state=" << std::hex << next_state->num << std::endl;
	}
#endif
	data = next_data;
	current = next_state;
}

tmpl(void)::genMoore () {

	p_output.data  = swap(SWAP_OUT, OUTPUT_BITWIDTH, (data >> (shiftRegSize - OUTPUT_BITWIDTH))
			& ((1<<OUTPUT_BITWIDTH) - 1));

}

tmpl(void)::genMealy () {

	bool next_valid = p_input.rok.read();
	bool next_get = false;

	bool cond = p_output.wok;
	if (current->doLoad) {
		if (! current->outputValidKnown || current->outputValid) {
			cond = p_input.rok && p_output.wok;
		} else {
			cond = p_input.rok;
		}
	}
	if (cond) {
		next_get = current->doLoad;
	}

	if (current->outputValidKnown)
		next_valid = current->outputValid;

	//std::cout << "next_valid=" << next_valid << std::endl;
	//std::cout << "next_get=" << next_get << std::endl;

	p_output.w = next_valid;
	p_input.r = next_get;

}

tmpl(void)::build() {
	unsigned int num = 0;
	unsigned int n   = INPUT_BITWIDTH;
	unsigned int max = INPUT_BITWIDTH;
	State *cur, *prev = NULL;
	
	cur = new State(num++);
	start = cur;
	cur->setOutputValid(false);
	cur->setDoLoad(0);

	bool isLast = false;
	while (! isLast) {
		if (prev != NULL)
			prev->next = cur;
		prev = cur;
		cur = new State(num++);
		
		if (n >= OUTPUT_BITWIDTH) {
			if (n == OUTPUT_BITWIDTH)
				cur->setOutputValid(true);
			else
				cur->outputValidKnown = false;
			n -= OUTPUT_BITWIDTH;
		} else {
			cur->setOutputValid(false);
		}
		if (n < OUTPUT_BITWIDTH) {
			if (n == 0)
				isLast = true;
			cur->setDoLoad(n);
			n += INPUT_BITWIDTH;
			if (n > max)
				max = n;
		} else {
			cur->setOutputValid(true);
		}
	}
	if (prev != NULL)
		prev->next = cur;
	cur->next = start->next;
	cur->isLast = true;
	shiftRegSize = max;
}


tmpl(/**/)::LoadWord (sc_module_name insname)
			: BaseModule(insname) {
	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD (genMoore);
	dont_initialize();
	sensitive << p_clk.neg();

	SC_METHOD (genMealy);
	dont_initialize();
	sensitive << p_clk.neg()
		<< p_input.rok
		<< p_output.wok;

	build();
	current = start;

	assert (shiftRegSize <= sizeof(unsigned long long)*8);

#ifdef SOCLIB_MODULE_DEBUG
	// debug
	State * c = start;
	std::cout << "=== states === regSize: " << shiftRegSize << std::endl;
	while(1) {
		std::cout << (*c) << std::endl;
		if (c->num > 0 && c->next->num == 1) {
			break;
		}
		c = c->next;
	}
	std::cout << "=====================" << std::endl;
#endif
}

}} // end of soclib::caba

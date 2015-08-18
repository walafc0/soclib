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

#include <stdio.h>

struct {
	unsigned int length;
	unsigned int count;
} phase[] = {
	{ 1, 128 }, { 1, 1 },
	{ 1, 4 }, { 1,  124 }, { 1, 1}, { 1, 128 }, { 1, 1 }
};

struct {
	unsigned int length;
	unsigned int count;
} super[] = {
	{ 2, 6 },
	{ 5, 1 },
};

void p(unsigned int index, unsigned int iter) {

	printf("%5d -> p[%2d]\n", iter, index);
}


void clken1() {
	unsigned int max_super = sizeof(super)/sizeof(super[0]);

	unsigned int current_super   = 0;
	unsigned int current_phase   = 0;
	unsigned int current_pattern = 0;

	unsigned int iter = 0;

	do {
		unsigned int pattern_base = current_pattern;

		unsigned int super_count = 0;
		do {

			current_pattern = pattern_base;
			unsigned int phase_count = 0;
			do {
				unsigned int pattern_count = 0;
				do {
					unsigned int x = 0;
					do {
						p(current_pattern + x, iter++);
						++x;
					} while (x < phase[current_phase + phase_count].length);
		
					++pattern_count;
				} while (pattern_count < phase[current_phase + phase_count].count);
	
				current_pattern += phase[current_phase + phase_count].length;
				++phase_count;
			} while (phase_count < super[current_super].length);
			
			++super_count;
		} while (super_count < super[current_super].count);
	
		current_phase += super[current_super].length;
		++current_super;
	} while (current_super < max_super);
}

unsigned int max_super = sizeof(super)/sizeof(super[0]);

unsigned int current_super   = 0;
unsigned int current_phase   = 0;
unsigned int current_pattern = 0;
unsigned int pattern_base   = 0;
unsigned int super_count    = 0;
unsigned int phase_count    = 0;
unsigned int pattern_count  = 0;
unsigned int pattern_offset = 0;

unsigned int iter = 0;

int clken2() {
	p(current_pattern + pattern_offset, iter++);
	++pattern_offset;
	if (pattern_offset < phase[current_phase + phase_count].length)
		return 1;
		
	++pattern_count;
	pattern_offset = 0;
	if (pattern_count < phase[current_phase + phase_count].count)
		return 1;
	
	current_pattern += phase[current_phase + phase_count].length;
	++phase_count;
	pattern_count = 0;
	if (phase_count < super[current_super].length)
		return 1;
			
	++super_count;
	phase_count = 0;
	if (super_count < super[current_super].count) {
		current_pattern = pattern_base;
		return 1;
	}
	
	current_phase += super[current_super].length;
	++current_super;
	pattern_base = current_pattern;
	super_count = 0;
	if (current_super < max_super)
		return 1;

	return 0;
}

int main() {

	//clken1();
	while(clken2())
		;

	printf("stop\n");

	return 0;
}

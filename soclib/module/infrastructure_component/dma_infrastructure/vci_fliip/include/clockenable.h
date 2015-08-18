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

#ifndef CLOCKENABLE_H
#define CLOCKENABLE_H

#include <caba_base_module.h>
#include <soclib_accelerator_fifoports.h>

#ifdef DEBUG_CLOCKENABLE
#include <stdio.h>
#define DPRINTF_CE(x...) { fprintf(ce_out,x); }
#else
#define DPRINTF_CE(x...) {}
#endif

namespace soclib { namespace caba {

template < unsigned int NTOIP_FIFO , unsigned int NFRIP_FIFO,
	unsigned int CTRL_DATA_SIZE, unsigned int CTRL_ADDR_SIZE >
class ClockEnable : soclib::caba::BaseModule {

public:
    sc_in  < bool > p_clk;
    sc_in  < bool > p_resetn;
    sc_in  < bool > p_start;

    sc_in  < bool > p_toip_write  [NTOIP_FIFO];
    sc_out < bool > p_toip_writeok[NTOIP_FIFO];

    sc_in  < bool > p_frip_writeok[NFRIP_FIFO];
    sc_out < bool > p_frip_write  [NFRIP_FIFO];

    sc_out < bool > p_ce;          //clock enable

    ACCELERATOR_ENBL_SIGNALS_IN  < CTRL_DATA_SIZE, CTRL_ADDR_SIZE > p_enbl;

private:
	enum {
		MAX_PHASES   = 1<<(CTRL_ADDR_SIZE-1),
		MAX_PATTERNS = 1<<(CTRL_ADDR_SIZE-1),
		MAX_SUPERS   = 1<<(CTRL_ADDR_SIZE-1),
	};

	struct {
		unsigned int length;
		unsigned int count;
	} r_super_memory[MAX_SUPERS];

	struct {
		unsigned int length;
		unsigned int count;
	} r_phase_memory[MAX_PHASES];

	struct {
		unsigned int input;
		unsigned int output;
	} r_pattern_memory[MAX_PATTERNS];

    bool running;
	bool clk_enable;
	unsigned int r_super_max;
	unsigned int r_phase_max;
	unsigned int r_pattern_max;

	unsigned int r_current_super;
	unsigned int r_current_phase;
	unsigned int r_current_pattern;
	unsigned int r_pattern_base;
	unsigned int r_super_count;
	unsigned int r_phase_count;
	unsigned int r_pattern_count;
	unsigned int r_pattern_offset;

	void write_super(unsigned int n, unsigned int data);
	void write_phase(unsigned int n, unsigned int data);
	void write_pattern(unsigned int n, unsigned int data);
	void start();
	void print_tables();
	void next_virtual_cycle();

    void transition();
    void genMoore();
    void genMealy();
  
protected:
    SC_HAS_PROCESS (ClockEnable);
  
public:
    ClockEnable (sc_module_name insname);
    ~ClockEnable ();
};

}} // end of soclib::caba

#endif /* CLOCKENABLE_H */


/// Local Variables:
/// mode: hs-minor
/// c-basic-offset: 4
/// End:

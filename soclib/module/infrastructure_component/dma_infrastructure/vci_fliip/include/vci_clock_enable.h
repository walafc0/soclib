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

#ifndef VCI_CLOCK_ENABLE_H
#define VCI_CLOCK_ENABLE_H

#include <generic_fifo.h>
#include <caba_base_module.h>
#include <mapping_table.h>
#include <vci_target_fsm.h>
#include "soclib_accelerator_fifoports.h"
#include "dma_regs.h"

namespace soclib { namespace caba {

template < typename vci_param,
unsigned int NTOIP_FIFO,
unsigned int NFRIP_FIFO ,
unsigned int FIFO_BITWIDTH,
unsigned int CTRL_ADDR_SIZE,
unsigned int NUSER_REGS>
class VciClockEnable : BaseModule {

public:
    sc_in  < bool > p_clk;
    sc_in  < bool > p_resetn;

    sc_out < bool > p_start;

    // VCI ports
    soclib::caba::VciTarget<vci_param> p_vci_target;

    // Fifo enable ports
    ACCELERATOR_ENBL_SIGNALS_OUT  < vci_param::B*8, CTRL_ADDR_SIZE > p_enbl;

	sc_out< sc_dt::sc_uint<vci_param::B*8> > p_user_regs[NUSER_REGS];

private:

	// indirect controller memory access
	unsigned int r_ctrl_addr;

	sc_dt::sc_uint<vci_param::B*8> r_user_regs[NUSER_REGS];

    soclib::caba::VciTargetFsm<vci_param, true> m_vci_target_fsm;

    unsigned int get_fifo_number_from_address(unsigned int addr) {
		return (addr >> PORT_SHIFT) & ((1<<PORT_BITS)-1);
    }
    
    // /////////////////
    // variables used to know the end of the previous cycle (read on interface/IP ports) 
    // /////////////////

	typedef typename vci_param::data_t data_t;

    bool         enbl_bool;
    bool         enbl_frip;
    unsigned int enbl_data;
    unsigned int enbl_addr;

    void write_to_user(unsigned int nreg, unsigned int data);
    void write_to_enbl(unsigned int addr, unsigned int data);

	bool on_write(int seg, typename vci_param::addr_t addr,
		typename vci_param::data_t data, int be);
	bool on_read(int seg, typename vci_param::addr_t addr,
		typename vci_param::data_t &data);

	void transition ();
    void genMoore ();

protected:
    SC_HAS_PROCESS (VciClockEnable);
  
public:
    VciClockEnable (
			sc_module_name insname, const MappingTable& mt,
			const IntTab& tgtid);

	~VciClockEnable();

};				

}} // end of soclib::caba

#endif


/// Local Variables:
/// mode: hs-minor
/// c-basic-offset: 4
/// End:

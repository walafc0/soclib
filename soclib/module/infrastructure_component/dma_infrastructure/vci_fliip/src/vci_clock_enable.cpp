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

#include <vci_clock_enable.h>
#include <alloc_elems.h>

namespace soclib { namespace caba {

#define tmpl(x) \
template < typename vci_param, \
unsigned int NTOIP_FIFO, \
unsigned int NFRIP_FIFO, \
unsigned int FIFO_BITWIDTH, \
unsigned int CTRL_ADDR_SIZE, \
unsigned int NUSER_REGS> \
x VciClockEnable<vci_param, NTOIP_FIFO, NFRIP_FIFO, FIFO_BITWIDTH, CTRL_ADDR_SIZE, NUSER_REGS>

tmpl(void)::write_to_user(unsigned int nreg, unsigned int data) {
	r_user_regs[nreg] = data;
}

tmpl(void)::write_to_enbl(unsigned int addr, unsigned int data) {
	unsigned int mask = 3 << (CTRL_ADDR_SIZE-2);
	if ((addr & ((1<<CTRL_ADDR_SIZE)-1)) != (addr & ~0xc0000000)) {
		std::cout << "Clock enable address too big, increase CTRL_ADDR_SIZE" << std::endl;
	}
	enbl_bool = true;
	enbl_data = data;
	enbl_frip = false; // XXX not used anymore
	enbl_addr = (addr & ~mask) | ((addr >> 30) << (CTRL_ADDR_SIZE-2));
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << "write to FIFO ENABLE " << (enbl_frip ? "FRIP" : "TOIP")
		<< " control register (addr = "<< std:hex << addr ", data = 0x"
		<< data << ")" << std::endl;
#endif
}
  
tmpl(void)::transition () {

	enbl_bool = false;

	if (p_resetn == false) {
		m_vci_target_fsm.reset();
		
		p_start             = false; // XXX should be in moore
		return;
	}	

	// make a strobe on start pin
	if (p_start == true) {
		p_start = false;
	}

	m_vci_target_fsm.transition();

}

tmpl(void)::genMoore() {

	m_vci_target_fsm.genMoore();

	if (enbl_bool) {
		p_enbl.WRITE = true;
		p_enbl.FRIP  = enbl_frip;
		p_enbl.DATA  = enbl_data;
		p_enbl.ADDR  = enbl_addr;
	} else {
		p_enbl.WRITE = false;
	}

	for (unsigned int i = 0; i < NUSER_REGS; ++i) {
		p_user_regs[i] = r_user_regs[i];
	}
}

tmpl(bool)::on_write(int seg, typename vci_param::addr_t addr,
		typename vci_param::data_t data, int be) {

	unsigned int cell = addr / vci_param::B;

#ifdef SOCLIB_MODULE_DEBUG
	std::cout << "Target: write at " << std::hex << addr
			<< " data=" << data << " be=" << be << std::endl;
#endif

	if (cell <= FIFO_15_REG) {
		return false;
	}
	if (cell >= USER_0_REG) {
		write_to_user(cell - USER_0_REG, data);
		return true;
	}
	switch (cell) {

		case DMA_INFO_REG:
		case DMA_MEM_REG:
		case DMA_PHASE_REG:
		case DMA_LENGTH_REG:
		case DMA_READ_LOOP_REG:
		case DMA_WRITE_LOOP_REG:
		case DMA_RESET_REG:
		case DMA_START_REG:
			return false;

		case CTRL_ADDR_REG:
			r_ctrl_addr = data;
			return true;

		case CTRL_DATA_REG:
			write_to_enbl(r_ctrl_addr, data);
			return true;

		case CTRL_RESET_REG:
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "***** Reset of CTRL" << std::endl;
#endif
			return true;

		case CTRL_START_REG:
			p_start = true; // XXX should use an intermediate var
			return true;

		default:
#ifdef SOCLIB_MODULE_DEBUG
		std::cout << "***** Unknown address " << std::hex << addr << std::endl;
#endif
			return false;
	}
	return false;
}

tmpl(bool)::on_read(int seg, typename vci_param::addr_t addr,
		typename vci_param::data_t &data) {

	unsigned int cell = addr / vci_param::B;

#ifdef SOCLIB_MODULE_DEBUG
	std::cout << "Target: read at address " << std::hex << addr << std::endl;
#endif

	return false;
}

tmpl(/**/)::VciClockEnable(
		sc_module_name insname, const MappingTable& mt,
		const IntTab& tgtid)
	: BaseModule(insname),
	m_vci_target_fsm(p_vci_target, mt.getSegmentList(tgtid)) {

	m_vci_target_fsm.on_read_write(on_read, on_write);

	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD (genMoore);
	dont_initialize();
	sensitive << p_clk.neg();

}

tmpl(/**/)::~VciClockEnable() {
}

}} // end of soclib::caba


/// Local Variables:
/// mode: hs-minor
/// c-basic-offset: 4
/// End:

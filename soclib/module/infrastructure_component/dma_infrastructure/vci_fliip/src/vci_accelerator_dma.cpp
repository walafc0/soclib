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

#include <vci_accelerator_dma.h>
#include <read_to_fifo_request.h>
#include <write_from_fifo_request.h>
#include <alloc_elems.h>

namespace soclib { namespace caba {

#define tmpl(x) \
template < typename vci_param, \
unsigned int NTOIP_FIFO, \
unsigned int NFRIP_FIFO, \
unsigned int FIFO_BITWIDTH, \
unsigned int CTRL_ADDR_SIZE, \
unsigned int NUSER_REGS, \
unsigned int FIFO_SIZE > \
x VciAcceleratorDma<vci_param, NTOIP_FIFO, NFRIP_FIFO, FIFO_BITWIDTH, CTRL_ADDR_SIZE, NUSER_REGS, FIFO_SIZE>

// XXX data type should be vci_param::data_t
tmpl(void)::write_to_fifo(unsigned int nfifo, unsigned int data) {
	// called from transition(), either during a DMA read (from the external memory) 
	// or called from target_io when we want to write directly from the C program
	// we want to write to TOIP_FIFO[n]
	toip_write_bool[nfifo] = true;
	toip_write_data[nfifo] = data;
	DPRINTF_IDMA("   write_to_fifo[%d] = true\n", nfifo);
}

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
	DPRINTF_IDMA("    write to FIFO ENABLE %s control register (addr = 0x%04x, data = 0x%08x)\n",
			enbl_frip ? "FRIP" : "TOIP", addr, data);
}

tmpl(void)::add_read_stride() {
	for(unsigned int i = 0; i < r_read_max_conf; ++i) {
		if (r_read_confs[i].mem_mode != DMA_MEM_CONST)
			r_read_confs[i].mem += r_read_loop_stride;
		r_read_confs[i].current = r_read_confs[i].mem;
		r_read_confs[i].count = r_read_confs[i].nwords;
	}
}

tmpl(void)::add_write_stride() {
	for(unsigned int i = 0; i < r_write_max_conf; ++i) {
		if (r_write_confs[i].mem_mode != DMA_MEM_CONST)
			r_write_confs[i].mem += r_write_loop_stride;
		r_write_confs[i].current = r_write_confs[i].mem;
		r_write_confs[i].count = r_write_confs[i].nwords;
	}
}

tmpl(void)::next_request() {
	if (r_current_is_read) {
		if (r_read_dma_running == true) {
			next_read_request();
		}
		if (r_write_dma_running == true) {
			r_current_is_read = false;
		}
	} else {
		if (r_write_dma_running == true) {
			next_write_request();
		}
		if (r_read_dma_running == true) {
			r_current_is_read = true;
		}
	}
}

tmpl(void)::next_read_request() {
	if (r_read_cur_conf == r_read_max_conf) {
		if (r_read_loop_count > 1) {
#ifdef SOCLIB_DEBUG_MODULE
			std::cout << "End of DMA_READ --> Looping config " << r_read_loop_count << std::endl;
#endif
			r_read_loop_count = r_read_loop_count - 1;
			add_read_stride();
			r_read_cur_conf = 0;
			r_read_start_phase_conf = 0;
			r_read_current_phase = 0;
			r_read_all_finished = true;
	    } else {
			//std::cout << "DMA READ FINISHED" << std::endl;
			r_read_dma_running = false;
		}
	} else {
		unsigned int count = r_read_confs[r_read_cur_conf].count;
		if (count > 0) {
			unsigned int current_count = count < r_burst ? count : r_burst;
#ifdef SOCLIB_DEBUG_MODULE
			std::cout << "next_read_request " << r_read_cur_conf << " " << current_count << std::endl;
#endif
	    	GenericFifo < sc_uint<vci_param::B*8> > *fifo = &TOIP_FIFO[r_read_confs[r_read_cur_conf].fifo];
			if ((fifo->size() - fifo->filled_status()) >= current_count) {
				ReadToFifoRequest<vci_param> *req = new ReadToFifoRequest<vci_param>(
						r_read_confs[r_read_cur_conf].current,
						r_read_confs[r_read_cur_conf].mem_mode,
						current_count);
				req->setOnWrite(this, ON_WRITE_FIFO(on_write_fifo));
				req->setDone(this, ON_T(read_request_done));
				m_vci_init_fsm.doReq(req);
				r_is_transfer_active = true;
			} else {
				next_read_conf(false);
			}
		} else {
			next_read_conf(true);
		}
	}
}

tmpl(void)::next_read_conf(bool isFinished) {
	++r_read_cur_conf;
#ifdef SOCLIB_DEBUG_MODULE
	std::cout << "next in read group " << r_read_cur_conf << " " << isFinished << std::endl;
#endif
	if (! isFinished)
		r_read_all_finished = false;
	if (r_read_confs[r_read_cur_conf].phase != r_read_current_phase) {
		if (! r_read_all_finished) {
#ifdef SOCLIB_DEBUG_MODULE
			std::cout << "restart group " << r_read_start_phase_conf << std::endl;
#endif
			r_read_cur_conf = r_read_start_phase_conf;
			r_read_all_finished = true;
		} else {
#ifdef SOCLIB_DEBUG_MODULE
			std::cout << "group finished " << r_read_current_phase << std::endl;
#endif
			r_read_current_phase++;
			r_read_start_phase_conf = r_read_cur_conf;
		}
	}
}

tmpl(void)::read_request_done(VciInitiatorReq<vci_param> *req) {
	r_is_transfer_active = false;
	if ( !req->failed() /*&& !m_must_finish */) {
		// XXX Should access req->m_len
		unsigned int count = r_read_confs[r_read_cur_conf].count;
		if (count > r_burst)
			count = r_burst;
		//std::cout << "DMA request_done " << count << std::endl;
		r_read_confs[r_read_cur_conf].current += count * vci_param::B;
		r_read_confs[r_read_cur_conf].count -= count;
		next_request();
	} else {
		std::cout << name() << ": DMA read request failed" << std::endl;
	}
	delete req;
}

tmpl(bool)::on_write_fifo(data_t data) {
	if (TOIP_FIFO[r_read_confs[r_read_cur_conf].fifo].wok() == false)
		return false;
	//std::cout << "writing to fifo: " << DMA_READ_TOIP_FIFO << " data=" << data << std::endl;
	write_to_fifo(r_read_confs[r_read_cur_conf].fifo, data);
	return true;
}

tmpl(void)::next_write_request() {
	if (r_write_cur_conf == r_write_max_conf) {
		if (r_write_loop_count > 1) {
#ifdef SOCLIB_DEBUG_MODULE
			std::cout << "End of DMA_WRITE --> Looping config " << r_write_loop_count << std::endl;
#endif
			r_write_loop_count = r_write_loop_count - 1;
			add_write_stride();
			r_write_cur_conf = 0;
			r_write_start_phase_conf = 0;
			r_write_current_phase = 0;
			r_write_all_finished = true;
	    } else {
			//std::cout << "DMA WRITE FINISHED" << std::endl;
			r_write_dma_running = false;
		}
	} else {
		unsigned int count = r_write_confs[r_write_cur_conf].count;
		if (count > 0) {
			unsigned int current_count = count < r_burst ? count : r_burst;
	    	GenericFifo < sc_uint<vci_param::B*8> > *fifo = &FRIP_FIFO[r_write_confs[r_write_cur_conf].fifo];
#ifdef SOCLIB_DEBUG_MODULE
			std::cout << "next_write_request " << r_write_cur_conf << " " << fifo->filled_status() << std::endl;
#endif

			// XXX may be necessary to flush the write FIFO and avoid deadlocks
			//if (current_count > fifo->filled_status())
			//	current_count = fifo->filled_status();
			//if (current_count > 0) {
			if (fifo->filled_status() >= current_count) {
#if SOCLIB_MODULE_DEBUG
				std::cout << name() << ": DMA write request " << std::hex << current_count
	    			<< " fifo=" << r_write_confs[r_write_cur_conf].fifo
	    			<< " mem=" << r_write_confs[r_write_cur_conf].current << std::endl;
#endif
				WriteFromFifoRequest<vci_param> *req = new WriteFromFifoRequest<vci_param>(
						r_write_confs[r_write_cur_conf].current,
						r_write_confs[r_write_cur_conf].mem_mode,
						current_count);
				req->setOnRead(this, ON_READ_FIFO(on_read_fifo));
				req->setDone(this, ON_T(write_request_done));
				m_vci_init_fsm.doReq(req);
				r_is_transfer_active = true;
			} else {
				next_write_conf(false);
			}
		} else {
			next_write_conf(true);
		}
	}
}

tmpl(void)::next_write_conf(bool isFinished) {
	++r_write_cur_conf;
#if SOCLIB_MODULE_DEBUG
	std::cout << "next in write group " << r_write_cur_conf << " " << isFinished << std::endl;
#endif
	if (! isFinished)
		r_write_all_finished = false;
	if (r_write_confs[r_write_cur_conf].phase != r_write_current_phase) {
		if (! r_write_all_finished) {
#if SOCLIB_MODULE_DEBUG
			std::cout << "restart write group " << r_write_start_phase_conf << std::endl;
#endif
			r_write_cur_conf = r_write_start_phase_conf;
			r_write_all_finished = true;
		} else {
#if SOCLIB_MODULE_DEBUG
			std::cout << "write group finished " << r_write_current_phase << std::endl;
#endif
			r_write_current_phase++;
			r_write_start_phase_conf = r_write_cur_conf;
		}
	}
}

tmpl(void)::write_request_done(VciInitiatorReq<vci_param> *req) {
	r_is_transfer_active = false;
	if ( !req->failed() ) {
		// XXX Should access req->m_len
		unsigned int count = r_write_confs[r_write_cur_conf].count;
		if (count > r_burst)
			count = r_burst;
		//std::cout << "DMA request_done " << count << std::endl;
		r_write_confs[r_write_cur_conf].current += count * vci_param::B;
		r_write_confs[r_write_cur_conf].count -= count;
		next_request();
	} else {
		std::cout << name() << ": DMA write request failed" << std::endl;
	}
	delete req;
}

tmpl(bool)::on_read_fifo(data_t& data) {
	unsigned int nfifo = r_write_confs[r_write_cur_conf].fifo;
	if (FRIP_FIFO[nfifo].rok() == false)
		return false;
	data = FRIP_FIFO[nfifo].read();
	frip_read_bool[nfifo] = true;
	//std::cout << "reading from fifo: " << nfifo << " data=" << std::hex << data << std::endl;
	return true;
}
  
tmpl(void)::transition () {

	for(unsigned int i = 0; i < NTOIP_FIFO; ++i) {
		toip_read_bool[i]  = false;
		toip_write_bool[i] = false;
	}

	for(unsigned int i = 0; i < NFRIP_FIFO; ++i) {
		frip_read_bool[i]  = false;
		frip_write_bool[i] = false;
	}

	enbl_bool = false;

	if (p_resetn == false) {
		m_vci_target_fsm.reset();
		m_vci_init_fsm.reset();

		r_read_cur_conf      = 0;
		r_read_max_conf      = 0;
		r_read_loop_count    = 0;
		r_read_loop_stride   = 0;

		r_access_is_read     = true;
		r_is_transfer_active = false;
		r_current_is_read    = true;
		r_read_dma_running   = false;
		r_write_dma_running  = false;
		
		p_start             = false; // XXX should be in moore
		return;
	}	

	// /////////////////////////////////////////
	// /////////////////////////////////////////
	// Transition : End from previous cycle for in/out FIFO
	// /////////////////////////////////////////
	// /////////////////////////////////////////
	
	for(unsigned int i = 0; i < NTOIP_FIFO; ++i) {
		if (p_toip_data[i].wok == true && p_toip_data[i].w == true) {
			toip_read_bool[i] = true;
		}
	}

	for(unsigned int i = 0; i < NFRIP_FIFO; ++i) {
		if (p_frip_data[i].rok == true && p_frip_data[i].r == true) {
			frip_write_bool[i] = true;
			frip_write_data[i] = p_frip_data[i].data.read();
		}
	}

	// make a strobe on start pin
	if (p_start == true) {
		p_start = false;
	}

	m_vci_target_fsm.transition();
	m_vci_init_fsm.transition();

	// check for a next request to be started
	if ((r_read_dma_running || r_write_dma_running) && r_is_transfer_active == false) {
		next_request();
	}

	// /////////////////////////////////////////
	// /////////////////////////////////////////
	// Transition : FIFO OUT TO IP
	// /////////////////////////////////////////
	// /////////////////////////////////////////

	for(unsigned int i = 0; i < NTOIP_FIFO; ++i) {
		if (toip_read_bool[i] == true && toip_write_bool[i] == true) {
			DPRINTF_IDMA("   TOIP_FIFO[%d] : put 0x%08x and get 0x%08x\n",i,
					toip_write_data[i], (unsigned int)TOIP_FIFO[i].read());
			TOIP_FIFO[i].put_and_get(toip_write_data[i]);
 		} else if (toip_read_bool[i] == false && toip_write_bool[i] == true) {
			DPRINTF_IDMA("   TOIP_FIFO[%d] : simple_put 0x%8x\n", i, toip_write_data[i]);
			TOIP_FIFO[i].simple_put (toip_write_data[i]);
		} else if (toip_read_bool[i] == true && toip_write_bool[i] == false) {
			DPRINTF_IDMA("   TOIP_FIFO[%d] : simple_get value 0x%08x\n",i,
					(unsigned int)TOIP_FIFO[i].read());
			TOIP_FIFO[i].simple_get();
		}
	}

	// /////////////////////////////////////////
	// /////////////////////////////////////////
	// Transition : FIFO IN FROM IP
	// /////////////////////////////////////////
	// /////////////////////////////////////////

	for(unsigned int i = 0; i < NFRIP_FIFO; ++i) {
		if (frip_read_bool[i] == true && frip_write_bool[i] == true) {
#ifdef SOCLIB_DEBUG_MODULE
			std::cout << "   FRIP_FIFO[" << i << "] : put " << std::hex << frip_write_data[i]
					<< " and get " << FRIP_FIFO[i].read() << std::endl;
#endif
			FRIP_FIFO[i].put_and_get(frip_write_data[i]);
		} else if (frip_read_bool[i] == false && frip_write_bool[i] == true) {
#ifdef SOCLIB_DEBUG_MODULE
			std::cout << "   FRIP_FIFO[" << i << "] : simple_put " << std::hex
					<< frip_write_data[i] << std::endl;
#endif
			FRIP_FIFO[i].simple_put (frip_write_data[i]);
		} else if (frip_read_bool[i] == true && frip_write_bool[i] == false) {
#ifdef SOCLIB_DEBUG_MODULE
			std::cout << "   FRIP_FIFO[" << i << "] : simple_get value " << std::hex
					<< (unsigned int)FRIP_FIFO[i].read() << std::endl;
#endif
			FRIP_FIFO[i].simple_get();
		}
	}

}

tmpl(void)::genMoore() {

	m_vci_target_fsm.genMoore();
	m_vci_init_fsm.genMoore();

	// TOIP FIFO
	for(unsigned int i = 0; i < NTOIP_FIFO; ++i) {
		if (TOIP_FIFO[i].rok()) {
			// data
			p_toip_data[i].w = true;
			p_toip_data[i].data  = TOIP_FIFO[i].read();
		} else {
			p_toip_data[i].w = false;
		}
	}

	// FRIP FIFO and CTRL PORTS
	for(unsigned int i = 0; i < NFRIP_FIFO; ++i) {
		if (FRIP_FIFO[i].wok()) {
			// data
			p_frip_data[i].r = true;
		} else {
			p_frip_data[i].r = false;
		}
	}

	// CLOCKENABLE PORT
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

	DPRINTF_IDMA("Target: write at 0x%08x data=0x%08x be=%1x\n", (int)addr, (int)data, be);

	if (cell <= FIFO_15_REG) {
		write_to_fifo(cell, data);
		return true;
	}
	if (cell >= USER_0_REG) {
		write_to_user(cell - USER_0_REG, data);
		return true;
	}
	switch (cell) {

		case DMA_INFO_REG:
			DPRINTF_IDMA("   DMA fifo changed to 0x%08x\n", data & 0xFF);
			DPRINTF_IDMA("   DMA mem_mode changed to 0x%01x\n", (data>>8) & 0x3);
			DPRINTF_IDMA("   DMA dir changed to 0x%01x\n", (data>>10) & 0x1);
			r_access_is_read = ((data >> 10) & 0x1) == 0;
			if (r_access_is_read) {
				r_read_confs[r_read_max_conf].fifo = data & 0xFF;
				r_read_confs[r_read_max_conf].mem_mode = (data >> 8) & 0x3;
			} else {
				r_write_confs[r_write_max_conf].fifo = data & 0xFF;
				r_write_confs[r_write_max_conf].mem_mode = (data >> 8) & 0x3;
			}
			return true;

		case DMA_MEM_REG:
			DPRINTF_IDMA("   DMA memory address changed to 0x%08x\n", (unsigned int)data);
			if (r_access_is_read) {
				r_read_confs[r_read_max_conf].mem     = data;
				r_read_confs[r_read_max_conf].current = data;
			} else {
				r_write_confs[r_write_max_conf].mem     = data;
				r_write_confs[r_write_max_conf].current = data;
			}
			return true;

		case DMA_PHASE_REG:
			DPRINTF_IDMA("   DMA phase changed to 0x%08x\n", (unsigned int)data);
			// phase indicator is used to compare current and next configuration
			// so the first unused configuration is marked in order not to
			// go further than the last configuration
			if (r_access_is_read) {
				r_read_confs[r_read_max_conf].phase     = data;
				r_read_confs[r_read_max_conf+1].phase   = -1;
			} else {
				r_write_confs[r_write_max_conf].phase   = data;
				r_write_confs[r_write_max_conf+1].phase = -1;
			}
			return true;

		case DMA_LENGTH_REG:
			DPRINTF_IDMA("   DMA nwords  written %d\n", (unsigned int)data);
			if (r_access_is_read) {
				r_read_confs[r_read_max_conf].nwords = data;
				r_read_confs[r_read_max_conf].count  = data;
				r_read_max_conf = r_read_max_conf + 1;
			} else {
				r_write_confs[r_write_max_conf].nwords = data;
				r_write_confs[r_write_max_conf].count  = data;
				r_write_max_conf = r_write_max_conf + 1;
			}
			return true;

		case DMA_READ_LOOP_REG:
			DPRINTF_IDMA("   READ_LOOP_COUNT written %d\n", data &  0xffff);
			DPRINTF_IDMA("   READ_LOOP_STRIDE written %d\n",(data >> 16) &  0xffff);
			r_read_loop_count  = data &  0xffff;
			r_read_loop_stride = (data >> 16) &  0xffff;
			return true;

		case DMA_WRITE_LOOP_REG:
			DPRINTF_IDMA("   WRITE_LOOP_COUNT written %d\n", data &  0xffff);
			DPRINTF_IDMA("   WRITE_LOOP_STRIDE written %d\n",(data >> 16) &  0xffff);
			r_write_loop_count  = data &  0xffff;
			r_write_loop_stride = (data >> 16) &  0xffff;
			return true;

		case DMA_RESET_REG:
			r_read_max_conf      = 0;
			r_read_cur_conf      = 0;
			r_read_loop_count    = 0;
			r_read_loop_stride   = 0;
			r_write_max_conf      = 0;
			r_write_cur_conf      = 0;
			r_write_loop_count    = 0;
			r_write_loop_stride   = 0;
			r_read_confs[r_read_max_conf].phase     = -1;
			r_write_confs[r_write_max_conf].phase   = -1;
			return true;

		case DMA_START_REG:
			if (r_read_dma_running == false && r_write_dma_running == false) {
				r_read_dma_running = true;
				r_write_dma_running = true;
				r_read_cur_conf = 0;
				r_write_cur_conf = 0;
				r_read_current_phase = 0;
				r_write_current_phase = 0;
				r_read_start_phase_conf = 0;
				r_write_start_phase_conf = 0;
				r_read_all_finished = true;
				next_request();
			}
			return true;

		case CTRL_ADDR_REG:
			r_ctrl_addr = data;
			return true;

		case CTRL_DATA_REG:
			write_to_enbl(r_ctrl_addr, data);
			return true;

		case CTRL_RESET_REG:
			DPRINTF_IDMA("***** Reset of CTRL\n");
			return true;

		case CTRL_START_REG:
			p_start = true; // XXX should use an intermediate var
			return true;

		default:
			DPRINTF_IDMA("***** Unknown address %08x\n", (unsigned int)addr);
			return false;
	}
	return false;
}

tmpl(bool)::on_read(int seg, typename vci_param::addr_t addr,
		typename vci_param::data_t &data) {

	unsigned int cell = addr / vci_param::B;

	DPRINTF_IDMA("Target: read at address 0x%08x\n", (int)addr);

	if (cell <= FIFO_15_REG) {
		if (FRIP_FIFO[cell].rok()) {
			DPRINTF_IDMA("   read from TOIP_FIFO[%d]\n", cell);
			data = FRIP_FIFO[cell].read();
			frip_read_bool[cell] = true;
		}
		return true;
	}
	if (cell == DMA_START_REG) {
		data = (r_read_dma_running == false && r_write_dma_running == false) ? 0 : 1;
		return true;
	}
	return false;
}

tmpl(/**/)::VciAcceleratorDma(
		sc_module_name insname, const MappingTable& mt,
		const IntTab& srcid, const IntTab& tgtid, int burst_size)
	: BaseModule(insname),
	TOIP_FIFO(alloc_elems<AccGenericFifo<sc_uint<vci_param::B*8> > >("toip_fifo", NTOIP_FIFO)),
	FRIP_FIFO(alloc_elems<AccGenericFifo<sc_uint<vci_param::B*8> > >("frip_fifo", NFRIP_FIFO)),
	m_vci_target_fsm(p_vci_target, mt.getSegmentList(tgtid)),
	m_vci_init_fsm(p_vci_initiator, mt.indexForId(srcid)) {

	m_vci_target_fsm.on_read_write(on_read, on_write);

	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD (genMoore);
	dont_initialize();
	sensitive << p_clk.neg();

	r_burst = burst_size;

	/*if (CTRL_ADDR_SIZE > 7) {
		std::cerr << name() << ": CTRL address size is too big (7 bits max)" << endl;
	}*/
}

tmpl(/**/)::~VciAcceleratorDma() {
	dealloc_elems<AccGenericFifo<sc_uint<vci_param::B*8> > >(TOIP_FIFO, NTOIP_FIFO);
	dealloc_elems<AccGenericFifo<sc_uint<vci_param::B*8> > >(FRIP_FIFO, NFRIP_FIFO);
}

}} // end of soclib::caba


/// Local Variables:
/// mode: hs-minor
/// c-basic-offset: 4
/// End:

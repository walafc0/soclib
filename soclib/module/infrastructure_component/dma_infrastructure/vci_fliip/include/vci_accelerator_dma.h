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

#ifndef VCI_ACCELERATOR_DMA_H
#define VCI_ACCELERATOR_DMA_H

#include <generic_fifo.h>
#include <caba_base_module.h>
#include <mapping_table.h>
#include <vci_target_fsm.h>
#include <vci_initiator_fsm.h>
#include <fifo_ports.h>
#include "soclib_accelerator_fifoports.h"
#include "dma_regs.h"

//#define DEBUG_IDMA

#include <stdio.h>
#include <stdarg.h>
template<bool ENABLED = true>
class DebugPrinter {
public:
	DebugPrinter(int a) {} // must have a parameter
	void operator()(const char *format, ...) {
		va_list valist;
		if (ENABLED) {
			va_start(valist, format);
			vprintf(format, valist);
			va_end(valist);
		}
	}
};

#ifdef DEBUG_IDMA
#define DPRINTF_IDMA (DebugPrinter<true>(0))
#else
#define DPRINTF_IDMA (DebugPrinter<false>(0))
#endif

namespace soclib { namespace caba {

template < typename vci_param,
unsigned int NTOIP_FIFO,
unsigned int NFRIP_FIFO ,
unsigned int FIFO_BITWIDTH,
unsigned int CTRL_ADDR_SIZE,
unsigned int NUSER_REGS,
unsigned int FIFO_SIZE = 20 >
class VciAcceleratorDma : BaseModule {

public:
    sc_in  < bool > p_clk;
    sc_in  < bool > p_resetn;

    sc_out < bool > p_start;
    //sc_in  < bool > RELOAD; // ??

    // VCI ports
    soclib::caba::VciTarget<vci_param> p_vci_target;
    soclib::caba::VciInitiator<vci_param> p_vci_initiator;

    // IRQ port
    sc_out < bool > p_irq;

    // FIFO DATA ports 
    soclib::caba::FifoOutput< sc_uint<vci_param::B*8> > p_toip_data[NTOIP_FIFO];
    soclib::caba::FifoInput< sc_uint<vci_param::B*8> > p_frip_data[NFRIP_FIFO];

    // Fifo enable ports
    ACCELERATOR_ENBL_SIGNALS_OUT  < vci_param::B*8, CTRL_ADDR_SIZE > p_enbl;

	sc_out< sc_dt::sc_uint<vci_param::B*8> > p_user_regs[NUSER_REGS];

private:

	template< typename data_type >
	class AccGenericFifo : public GenericFifo<data_type> {
	public:
		AccGenericFifo(const char *name) : GenericFifo<data_type>(name, FIFO_SIZE) { }
	};
    
    // XXX to be splitted so that each FIFO can have a different size
    AccGenericFifo < sc_uint<vci_param::B*8> > *TOIP_FIFO;
    AccGenericFifo < sc_uint<vci_param::B*8> > *FRIP_FIFO;

    unsigned int r_burst;
	bool r_is_transfer_active;
	bool r_current_is_read;
	bool r_read_dma_running;
	bool r_write_dma_running;

	// indirect controller memory access
	unsigned int r_ctrl_addr;

    // MMR Memory Mapped Register for DMA configuration
    unsigned int  r_read_loop_stride;
    unsigned int  r_read_loop_count;
    unsigned int  r_read_cur_conf;
    unsigned int  r_read_max_conf;

    unsigned int  r_write_loop_stride;
    unsigned int  r_write_loop_count;
    unsigned int  r_write_cur_conf;
    unsigned int  r_write_max_conf;
	
    unsigned int  r_read_current_phase;
    unsigned int  r_write_current_phase;
    unsigned int  r_read_start_phase_conf;
    unsigned int  r_write_start_phase_conf;

	bool r_read_all_finished;
	bool r_write_all_finished;

#define MAX_CONF 1000
	struct DmaConfiguration {
    	unsigned int  mem;
    	unsigned int  current;
    	unsigned int  mem_mode;
    	unsigned int  fifo;
    	unsigned int  nwords;
    	unsigned int  phase;
    	unsigned int  count;
	};

	DmaConfiguration r_read_confs[MAX_CONF];
	DmaConfiguration r_write_confs[MAX_CONF];
	bool r_access_is_read;

	sc_dt::sc_uint<vci_param::B*8> r_user_regs[NUSER_REGS];
  

    soclib::caba::VciTargetFsm<vci_param, true> m_vci_target_fsm;
    soclib::caba::VciInitiatorFsm<vci_param> m_vci_init_fsm;

    unsigned int get_fifo_number_from_address(unsigned int addr) {
		return (addr >> PORT_SHIFT) & ((1<<PORT_BITS)-1);
    }
    
    // /////////////////
    // variables used to know the end of the previous cycle (read on interface/IP ports) 
    // /////////////////

	typedef typename vci_param::data_t data_t;

    bool         toip_write_bool[NTOIP_FIFO];
    unsigned int toip_write_data[NTOIP_FIFO];
    bool         toip_read_bool [NTOIP_FIFO];
    unsigned int toip_read_data [NTOIP_FIFO];

    bool         frip_write_bool[NFRIP_FIFO];
    unsigned int frip_write_data[NFRIP_FIFO];
    bool         frip_read_bool [NFRIP_FIFO];
    unsigned int frip_read_data [NFRIP_FIFO];
    
#if 0
    bool         toip_ctrl_bool [NTOIP_FIFO];
    unsigned int toip_ctrl_data [NTOIP_FIFO];
    unsigned int toip_ctrl_addr [NTOIP_FIFO];

    bool         frip_ctrl_bool [NFRIP_FIFO];
    unsigned int frip_ctrl_data [NFRIP_FIFO];
    unsigned int frip_ctrl_addr [NFRIP_FIFO];
#endif

    bool         enbl_bool;
    bool         enbl_frip;
    unsigned int enbl_data;
    unsigned int enbl_addr;

    void write_to_user(unsigned int nreg, unsigned int data);
    void write_to_fifo(unsigned int nfifo, unsigned int data);
    void read_from_fifo(unsigned int address);
    void write_to_enbl(unsigned int addr, unsigned int data);

    void add_read_stride();
    void add_write_stride();
	void next_request();
	void next_read_request();
	void next_read_conf(bool);
	void next_write_conf(bool);
	void next_write_request();
	void read_request_done(VciInitiatorReq<vci_param> *req);
	void write_request_done(VciInitiatorReq<vci_param> *req);
	bool on_write_fifo(data_t data);
	bool on_read_fifo(data_t& data);
	bool on_write(int seg, typename vci_param::addr_t addr,
		typename vci_param::data_t data, int be);
	bool on_read(int seg, typename vci_param::addr_t addr,
		typename vci_param::data_t &data);

	void transition ();
    void genMoore ();

protected:
    SC_HAS_PROCESS (VciAcceleratorDma);
  
public:
    VciAcceleratorDma (
			sc_module_name insname, const MappingTable& mt,
			const IntTab& srcid, const IntTab& tgtid, int burst_size);

	~VciAcceleratorDma ();

};				

}} // end of soclib::caba

#endif


/// Local Variables:
/// mode: hs-minor
/// c-basic-offset: 4
/// End:

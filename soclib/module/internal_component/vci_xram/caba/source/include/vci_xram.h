/* -*- c++ -*-
 * File         : vci_xram.h
 * Date         : 26/10/2008
 * Copyright    : UPMC / LIP6
 * Authors      : Alain Greiner 
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
 * Maintainers: alain
 */

    ///////////////////////////////////////////////////////////////////////////
    // This component accepts two types of request, that are both VCI write :
    // - cache line read  when VCI address == base address
    //   The VCI command contains one single cell with the cache line index
    //   in the WDATA field.
    //   The VCI response contains N cells, with the cache line words
    //   in the WDATA field.
    // - cache line write when VCI address == base address + 4
    //   The VCI command contains (1 + N) cells with the cache line index
    //   in the first cell, and the data in the following cells.
    //   The VCI response contains one single cell.
    ///////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_CABA_XRAM_H
#define SOCLIB_CABA_XRAM_H

#include <inttypes.h>
#include <systemc>
#include <cassert>
#include "arithmetics.h"
#include "alloc_elems.h"
#include "caba_base_module.h"
#include "vci_target.h"
#include "mapping_table.h"
#include "loader.h"

#define DEBUG_VCI_XRAM 0

namespace soclib {  namespace caba {
  using namespace sc_core;

///////////////////////////////////////////
template<typename vci_param> class VciXRam
///////////////////////////////////////////
  : public soclib::caba::BaseModule
  {
    typedef uint32_t addr_t;
    typedef uint32_t data_t;
    typedef uint32_t nline_t;

    enum tgt_fsm_state_e{
        XRAM_IDLE,
	XRAM_WDATA,
        XRAM_WAIT,
	XRAM_READ,
	XRAM_WRITE,
    };

 protected:
    SC_HAS_PROCESS(VciXRam);
    
 public:

    sc_in<bool> 				p_clk;
    sc_in<bool> 				p_resetn;
    soclib::caba::VciTarget<vci_param>   	p_vci_tgt;

    VciXRam(
	sc_module_name name,
	const soclib::common::MappingTable &mt,
        const soclib::common::IntTab &target_index,
	soclib::common::Loader &loader,
	const size_t mem_cache_words,
	const size_t ram_byte_size,
	const size_t read_latency
	);

    ~VciXRam();

    void transition();
    void genMoore();

 private:

    // Component attributes
    soclib::common::Loader 	       	*m_loader;
    const size_t			m_nwords;
    const soclib::common::Segment      	m_segment;
    const size_t      			m_xram_nlines;
    const size_t			m_read_latency;
    uint32_t            		m_ncycles;

    data_t			      	**m_ram_data;

    // Registers 
    sc_signal<int> 			r_xram_fsm;	
    sc_signal<data_t> 			r_xram_nline; 
    sc_signal<size_t> 			r_xram_srcid;
    sc_signal<size_t> 			r_xram_trdid;
    sc_signal<size_t> 			r_xram_pktid;
    sc_signal<data_t> 	       	       *r_xram_wdata;
    sc_signal<size_t>			r_xram_cpt;
    sc_signal<size_t>			r_xram_latency;
	
};
 
}}

#endif

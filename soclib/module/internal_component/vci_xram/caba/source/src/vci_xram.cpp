/* -*- c++ -*-
 * File         : vci_xram.cpp
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
    // This component contains one single FSM.
    // It accepts two types of request, that are both VCI write :
    // - cache line read  when VCI address == base address
    //   The VCI command contains one single cell with the cache line index
    //   in the WDATA field.
    //   The VCI response contains N cells, with the cache line words
    //   in the WDATA field.
    // - cache line write when VCI address == base address + 4
    //   The VCI command contains (1 + N) cells with the cache line index
    //   in the first cell, and the data in the following cells.
    //   The VCI response contains one single cell.
    //
    //   Attention : la valeur de l'offset est violemment cablée à 0x10000000
    ///////////////////////////////////////////////////////////////////////////

#include "../include/vci_xram.h"
#include "soclib_endian.h"
#include "loader.h"

namespace soclib { namespace caba {

#define OFFSET 0x10000000

#define DEBUG_VCI_XRAM 0

#ifdef DEBUG_VCI_XRAM
const char *xram_fsm_str[] = {
	"XRAM_IDLE",
	"XRAM_WDATA",
        "XRAM_WAIT",
	"XRAM_READ",
	"XRAM_WRITE",
	};
#endif

#define tmpl(x) template<typename vci_param> x VciXRam<vci_param>

  using soclib::common::uint32_log2;

  /////////////////////////////////////////////////////////////
  // 		Constructor
  /////////////////////////////////////////////////////////////
  tmpl(/**/)::VciXRam( 
	sc_module_name name,
	const soclib::common::MappingTable &mt,
	const soclib::common::IntTab &target_index,
	common::Loader &loader,
	const size_t line_cache_words,
	const size_t ram_byte_size,
	const size_t read_latency
	)
    : soclib::caba::BaseModule(name),

      p_clk("p_clk"),
      p_resetn("p_resetn"),
      p_vci_tgt("p_vci_tgt"),

      m_loader(&loader),
      m_nwords(line_cache_words),
      m_segment(mt.getSegment(target_index)),
      m_xram_nlines(ram_byte_size / (line_cache_words*4) ),
      m_read_latency(read_latency),
      m_ncycles(0),

      r_xram_fsm("r_xram_fsm"),
      r_xram_nline("r_xram_nline"),
      r_xram_srcid("r_xram_srcid"),
      r_xram_trdid("r_xram_trdid"),
      r_xram_pktid("r_xram_pktid")

  {
    assert(IS_POW_OF_2(line_cache_words));
    assert(line_cache_words);
    assert(line_cache_words <= 32);
    assert((ram_byte_size%line_cache_words) == 0);

    // Ram  & buffer allocation
    m_ram_data = new data_t*[ram_byte_size / (line_cache_words*4)];
    for(size_t i=0; i<(ram_byte_size / (line_cache_words*4)) ; i++) {
      m_ram_data[i] = new data_t[line_cache_words]; 
    }
    r_xram_wdata = new sc_signal<data_t>[line_cache_words];

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
  
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

  } // end constructor

  ////////////////////////////////////////////////////
  //    destructor
  ///////////////////////////////////////////////////
  tmpl(/**/)::~VciXRam()
  {
    for(size_t i=0; i<m_nwords ; i++) delete [] m_ram_data[i];
    delete [] m_ram_data;
    delete [] r_xram_wdata;
  }

  ///////////////////////////////////////////////////
  tmpl(void)::transition()
  ///////////////////////////////////////////////////
  {
    ////// reset ////// 
    if ( ! p_resetn ) {
      r_xram_fsm  = XRAM_IDLE;

      if ( m_loader ){
        for (size_t i=0 ; i<m_xram_nlines; i++ ) {
          m_loader->load(&m_ram_data[i][0], 
		(typename vci_param::addr_t) ((i* m_nwords * (vci_param::B)) + OFFSET),
                m_nwords*(vci_param::B));
 	  for (size_t j = 0 ; j < m_nwords ; ++j ) {
	          m_ram_data[i][j] = le_to_machine(m_ram_data[i][j]);
	  }
 	}
      } // end loader 

      return;
    } // end reset

#if DEBUG_VCI_XRAM
std::cout << "----------------------------------------------" << std::endl;
std::cout << "XRAM / Time = " << std::dec << m_ncycles << std::endl;
std::cout << " - XRAM_FSM       = " << xram_fsm_str[r_xram_fsm.read()] << std::endl;
#endif

    switch ( r_xram_fsm.read() ) {

    //////////////
    case XRAM_IDLE:	// decode the VCI write command :
    {
      if ( p_vci_tgt.cmdval ) {
        r_xram_nline    = p_vci_tgt.wdata.read();
        r_xram_srcid	= p_vci_tgt.srcid.read();
        r_xram_trdid	= p_vci_tgt.trdid.read();
        r_xram_pktid	= p_vci_tgt.pktid.read();
	r_xram_cpt	= 0;
	r_xram_latency  = m_read_latency;
      
        addr_t address	= p_vci_tgt.address.read();
        assert( m_segment.contains(address) 
          && "error in component XRAM : out of segment VCI command received");
        assert( (p_vci_tgt.cmd.read() == vci_param::CMD_WRITE) 
          && "error in component XRAM : the reveived VCI command is not a write");
        addr_t read_address = m_segment.baseAddress();
        if ( address == read_address ) {
	  assert( p_vci_tgt.eop 
                && "error in component XRAM : received VCI command read length > 1" );
          if ( m_read_latency )  r_xram_fsm = XRAM_WAIT;
          else 	                 r_xram_fsm = XRAM_READ;
        } else {
                                 r_xram_fsm = XRAM_WDATA;
        }
      }
      break;
    }
    ///////////////
    case XRAM_WDATA:	// store the cache line data in local buffer in case of write
    {
      if ( p_vci_tgt.cmdval ) {
        r_xram_wdata[r_xram_cpt.read()] = p_vci_tgt.wdata.read();
        r_xram_cpt = r_xram_cpt.read() + 1;
	if ( p_vci_tgt.eop.read() ) {
          assert( (r_xram_cpt.read() == (m_nwords - 1))
             && "error in component XRAM : bad write request length" );
          r_xram_fsm = XRAM_WRITE;
        }
      }
      break;
    }
    ///////////////
    case XRAM_WAIT:	// emulate the read latency
    {
      if ( r_xram_latency.read() == 0 )	r_xram_fsm = XRAM_READ;
      else				r_xram_latency = r_xram_latency.read() - 1;
      break;
    }
    ///////////////
    case XRAM_READ:	// send the cache line in the VCI response
    {
      if ( p_vci_tgt.rspack ) {
	if ( r_xram_cpt.read() == (m_nwords - 1) ) r_xram_fsm = XRAM_IDLE;
        else					   r_xram_cpt = r_xram_cpt.read() + 1;
      }
      break;
    }
    ///////////////
    case XRAM_WRITE:	// write the line in the XRAM & acknowledge the VCI command
    {
      if (  p_vci_tgt.rspack ) {
	for ( size_t i=0 ; i<m_nwords ; i++ ) {
	  m_ram_data[r_xram_nline.read() - (OFFSET/(m_nwords*4))][i] = r_xram_wdata[i].read();
	}
	r_xram_fsm  = XRAM_IDLE;
      }
      break;
    }
    } // end switch r_xram_fsm

    m_ncycles++;

  } // end transition()

  /////////////////////////////////
  tmpl(void)::genMoore()
  /////////////////////////////////
  {

    switch ( r_xram_fsm.read() ) {

    case XRAM_IDLE:
    case XRAM_WDATA:
      p_vci_tgt.cmdack  = true;
      p_vci_tgt.rspval  = false;
      p_vci_tgt.rdata   = 0;
      p_vci_tgt.rsrcid  = 0;
      p_vci_tgt.rtrdid  = 0;
      p_vci_tgt.rpktid  = 0;
      p_vci_tgt.rerror  = 0;
      p_vci_tgt.reop	= false;
      break;
    case XRAM_WAIT:
      p_vci_tgt.cmdack  = false;
      p_vci_tgt.rspval  = false;
      p_vci_tgt.rdata   = 0;
      p_vci_tgt.rsrcid  = 0;
      p_vci_tgt.rtrdid  = 0;
      p_vci_tgt.rpktid  = 0;
      p_vci_tgt.rerror  = 0;
      p_vci_tgt.reop	= false;
      break;
    case XRAM_READ:
      p_vci_tgt.cmdack  = false;
      p_vci_tgt.rspval  = true;
      p_vci_tgt.rdata   = m_ram_data[r_xram_nline.read() - (OFFSET/(m_nwords*4))][r_xram_cpt.read()];
      p_vci_tgt.rsrcid  = r_xram_srcid.read();
      p_vci_tgt.rtrdid  = r_xram_trdid.read();
      p_vci_tgt.rpktid  = r_xram_pktid.read();
      p_vci_tgt.rerror  = 0;
      p_vci_tgt.reop	= (r_xram_cpt.read() == (m_nwords - 1));
      break;
    case XRAM_WRITE:
      p_vci_tgt.cmdack  = false;
      p_vci_tgt.rspval  = true;
      p_vci_tgt.rdata   = 0;
      p_vci_tgt.rsrcid  = r_xram_srcid.read();
      p_vci_tgt.rtrdid  = r_xram_trdid.read();
      p_vci_tgt.rpktid  = r_xram_pktid.read();
      p_vci_tgt.rerror  = 0;
      p_vci_tgt.reop	= true;
      break;
    } // end switch r_xram_fsm

} // end genMoore()

}}

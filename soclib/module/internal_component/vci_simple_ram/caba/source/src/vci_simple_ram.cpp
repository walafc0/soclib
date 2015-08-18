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
 * Copyright (c) UPMC, Lip6, Asim
 *         Alain Greiner <alain.greiner@lip6.fr>, 2008
 *
 * Maintainers: alain
 */

/////////////////////////////////////////////////////////////////////////
//  This component is a multi-segments Ram controller.
//
//  It supports only the compact VCI packets defined 
//  in the VCI advanced specification.
//  The VCI DATA field must be 32 or 64 bits.
//  The VCI ADDRESS should be multiple of vci_param::B.
//  - A READ burst command packet (such a cache line request) 
//    contains one single flit. 
//    The response packet length is defined by the PLEN field.
//    The zero value for the PLEN field is not supported, 
//    but there is no other restrictions on the pakek length.
//    and unaligned packet bursts are supported: the number 
//    of response flits is computed from both BE and PLEN fields.
//    An error response packets contain one single flit.
//  - WRITE burst command packets at consecutive addresses are supported.
//    Write response packets contain always one single flit.
//  - Regarding LL/SC instructions, it supports both the classical
//    semantic (one flit SC instruction / registration of LL reservation
//    in a linked_buffer) and the Compare&Swap semantic for the SC 
//    (when the VCI SC command contains 2 flits : old value / new value).
//  The RAM latency is a parameter, that can have a zero value.
////////////////////////////////////////////////////////////////////////
//  Implementation note: 
//  Ther RAM itself is implemented as a set of uint32_t arrays
//  (one array per segment). 
//  This component is controlled by a single FSM.
//  The latency counter is decremented in the IDLE state.
//  The VCI command is analysed and checked in the CMD_GET state.
//  - For read or ll commands, the command is acknowledged in
//  the CMD_STATE. It is executed and the response is sent in the
//  RSP_READ, RSP_LL states. 
//  - For write or sc commands, the command is acknowledged in the 
//  CMD_STATE, or in the CMD_WRITE & CMD_ERROR states in case of bursts.
//  The command is executed in the CMD_WRITE state, or in the RSP_WRITE
//  state for the last flit of a burst. The response packet is sent
//  in the RSP_WRITE state.
/////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstring>
#include "arithmetics.h"
#include "vci_simple_ram.h"

namespace soclib {
namespace caba {

using namespace soclib;

#define tmpl(x) template<typename vci_param> x VciSimpleRam<vci_param>

//////////////////////////
tmpl(/**/)::VciSimpleRam(
    sc_module_name name,
    const soclib::common::IntTab index,
    const soclib::common::MappingTable &mt,
    const soclib::common::Loader &loader,
    const uint32_t latency)
	: caba::BaseModule(name),
      m_loader(loader),
      m_seglist(mt.getSegmentList(index)),
      m_latency(latency),

      r_llsc_buf((size_t)(1<<vci_param::S)),

      r_fsm_state("r_fsm_state"),
      r_flit_count("r_flit_count"),
      r_seg_index("r_seg_index"),
      r_address("r_address"),
      r_wdata("r_wdata"),
      r_be("r_be"),
      r_srcid("r_srcid"),
      r_trdid("r_trdid"),
      r_pktid("r_pktid"),
      r_contig("r_contig"),
      r_latency_count("r_latency_count"),

      p_resetn("p_resetn"),
      p_clk("p_clk"),
      p_vci("p_vci")
{
    std::cout << "  - Building VciSimpleRam : " << name << std::endl;

    size_t nsegs = 0;

    assert( (m_seglist.empty() == false) and
    "VCI_MULTI_RAM error : no segment allocated");

    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ )
    {
        std::cout << "    => segment " << seg->name()
                  << " / base = " << std::hex << seg->baseAddress()
                  << " / size = " << seg->size() << std::endl; 
        nsegs++;
    }

    m_nbseg = nsegs;

    assert( ((vci_param::B == 4) or (vci_param::B == 8)) and
    "VCI_SIMPLE_RAM ERROR : The VCI DATA field must be 32 or 64 bits");

    // actual memory allocation
    m_ram = new uint32_t*[m_nbseg];
    m_seg = new soclib::common::Segment*[m_nbseg];

    size_t i = 0;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ ) 
    { 
        m_ram[i] = new uint32_t[ (seg->size()+3)/4 ];
        m_seg[i] = &(*seg);
        i++;
    }

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
}

///////////////////////////
tmpl(/**/)::~VciSimpleRam()
{
    for (size_t i=0 ; i<m_nbseg ; ++i) delete [] m_ram[i];
    delete [] m_ram;
    delete [] m_seg;
}

/////////////////////
tmpl(void)::reload()
{
    for ( size_t i=0 ; i<m_nbseg ; ++i ) 
    {
        m_loader.load(&m_ram[i][0], m_seg[i]->baseAddress(), m_seg[i]->size());
        for ( size_t addr = 0 ; addr < m_seg[i]->size()/vci_param::B ; ++addr )
            m_ram[i][addr] = le_to_machine(m_ram[i][addr]);
    }
}

////////////////////
tmpl(void)::reset()
{
    for ( size_t i=0 ; i<m_nbseg ; ++i ) std::memset(&m_ram[i][0], 0, m_seg[i]->size()); 
    m_cpt_read = 0;
    m_cpt_write = 0;
    m_monitor_ok = false;
    if (m_latency) 
    {
 	    r_fsm_state = FSM_IDLE;
        r_latency_count = m_latency - 1;
    } 
    else 
    {
	    r_fsm_state = FSM_CMD_GET;
        r_latency_count = 0;
    }
    r_llsc_buf.clearAll();
}

///////////////////////////////////
tmpl(bool)::write(size_t      seg, 
                  vci_addr_t  addr, 
                  vci_data_t  wdata, 
                  vci_be_t    be)
{
    if ( m_monitor_ok )
    {
        if ( (addr >= m_monitor_base) and 
             (addr < m_monitor_base + m_monitor_length) )
        {
            std::cout << "Monitor RAM  " << name()
                      << " at cycle " << std::dec << m_cpt_cycles
                      << " : address = " << std::hex << addr
                      << " / data = " << wdata
                      << " / be = " << be << std::endl;
        }
    }

    if ( m_seg[seg]->contains(addr) ) 
    {
        uint32_t  input;
        uint32_t  current;
        uint32_t  mask;
        size_t index = (size_t)((addr - m_seg[seg]->baseAddress()) / 4);

        if ( vci_param::B == 4 )        // VCI DATA == 32 bits 
        {
            mask    = (uint32_t)vci_param::be2mask(be);
            current = m_ram[seg][index];
            input   = (uint32_t)wdata;
            m_ram[seg][index] = (current & ~mask) | (input & mask);
        }
        else                            // VCI DATA == 64 bits
        {
            // first 32 bits word
            mask    = (uint32_t)vci_param::be2mask(be & 0x0F);
            current = m_ram[seg][index];
            input   = (uint32_t)(wdata);
            m_ram[seg][index] = (current & ~mask) | (input & mask);

            // second 32 bits word
            mask    = (uint32_t)vci_param::be2mask(be >> 4);
            current = m_ram[seg][index+1];
            input   = (uint32_t)(wdata>>32);
            m_ram[seg][index+1] = (current & ~mask) | (input & mask);
        }
        m_cpt_write++;
        return true;
    } 
    return false;
}

//////////////////////////////////
tmpl(bool)::read(size_t       seg, 
                 vci_addr_t   addr, 
                 vci_data_t   &rdata )
{
    if ( m_seg[seg]->contains(addr) ) 
    {
        size_t index = (size_t)((addr - m_seg[seg]->baseAddress()) / 4);

        if ( vci_param::B == 4 )        // VCI DATA == 32 bits
        {
            rdata = (vci_data_t)m_ram[seg][index];
        }
        else                            // VCI DATA == 64 bits
        {
            rdata = (vci_data_t)m_ram[seg][index] | ((vci_data_t)m_ram[seg][index+1] << 32);
        }
        m_cpt_read++;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////
tmpl(void)::start_monitor(vci_addr_t base, vci_addr_t length)
{
    m_monitor_ok        = true;
    m_monitor_base      = base;
    m_monitor_length    = length;
}

//////////////////////////
tmpl(void)::stop_monitor()
{
    m_monitor_ok        = false;
}

//////////////////////////
tmpl(void)::print_trace()
{
    const char* state_str[] = { "IDLE", 
                                "CMD_GET",
                                "CMD_WRITE",
                                "CMD_ERROR",
                                "CMD_CAS",
                                "RSP_READ",
                                "RSP_WRITE",
                                "RSP_LL",
                                "RSP_SC",
                                "RSP_ERROR", 
                                "RSP_CAS" };
    std::cout << "SIMPLE_RAM " << name() 
              << " : state = " << state_str[r_fsm_state] 
              << " / latency_count = " << r_latency_count 
              << " / flit_count = " << r_flit_count << std::endl;
}

/////////////////////////
tmpl(void)::transition()
{
    if (!p_resetn) 
    {
        reset();
        reload();
        m_cpt_cycles = 0;
        return;
    }

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "Simple_ram : " << name() << std::endl;
std::cout << " fsm_state = " << r_fsm_state 
          << " latency_count = " << r_latency_count << std::endl;
#endif

    m_cpt_cycles++;

    switch ( r_fsm_state ) {
    /////////////
    case FSM_IDLE: 	// unreachable state if m_latency == 0 
    {
        if ( p_vci.cmdval.read() ) 
        {
            if (r_latency_count.read() == 0) 
            {
                r_fsm_state = FSM_CMD_GET;
                r_latency_count = m_latency - 1;
            } 
            else 
            {
                r_latency_count = r_latency_count.read() - 1;
            }
	}				   
	break;
    }
    //////////////// 
    case FSM_CMD_GET:   // decode the VCI command
    {
        if ( !p_vci.cmdval.read() ) break;

        vci_addr_t   address = p_vci.address.read();
        bool         error = true;

        assert( ((address & 0x3) == 0) and
                 "VCI_SIMPLE_RAM ERROR : The VCI ADDRESS must be multiple of 4");

        assert( (p_vci.plen.read() != 0) and
                 "VCI_SIMPLE_RAM ERROR : The VCI PLLEN should be != 0");

        for ( size_t index = 0 ; index<m_nbseg  && error ; ++index) 
        {
            if ( (m_seg[index]->contains(address)) &&
                 (m_seg[index]->contains(address + p_vci.plen.read() - 1)) ) 
            {
                error = false;
                r_seg_index = index;
            }
        } 

        r_address    = address;
        r_be         = p_vci.be.read();
        r_wdata      = p_vci.wdata.read();
        r_srcid      = p_vci.srcid.read();
        r_trdid      = p_vci.trdid.read();
        r_pktid      = p_vci.pktid.read();

        if ( error ) 
        {
            if( p_vci.eop.read() )  r_fsm_state = FSM_RSP_ERROR;
            else                    r_fsm_state = FSM_CMD_ERROR;
        }
        else
        {
            if ( p_vci.cmd.read() == vci_param::CMD_WRITE ) 
            {
                // we don't use the PLEN field : response is always one flit
                r_contig     = p_vci.contig.read();
                if( p_vci.eop.read() )  r_fsm_state = FSM_RSP_WRITE;
                else 			        r_fsm_state = FSM_CMD_WRITE;
            }
            else if ( p_vci.cmd.read() == vci_param::CMD_READ )
            {
                assert( p_vci.eop.read() and
                        "VCI_SIMPLE_RAM ERROR : read command packets should be one flit");

                // The number of response flits depends on PLEN and BE fields
                // (ctz returns the number of trailing 0 in the BE field)
                r_flit_count = ( p_vci.plen.read() +
                                 soclib::common::ctz(p_vci.be.read()) + 
                                 vci_param::B-1) / vci_param::B;
                r_contig = p_vci.contig.read();
                r_fsm_state = FSM_RSP_READ;
            }
            else if ( p_vci.cmd.read() == vci_param::CMD_STORE_COND )
            {
                if ( p_vci.eop.read() )     // One flit command => classical SC
                {
                    r_fsm_state = FSM_RSP_SC;
                }
                else                        // Two flits command => Compare & Swap
                {
                    r_fsm_state = FSM_CMD_CAS;
                }
            }
            else if ( p_vci.cmd.read() == vci_param::CMD_LOCKED_READ )
            {
                r_fsm_state = FSM_RSP_LL;
                assert( p_vci.eop.read() && 
                        "a VCI ll command packets should be one flit");
            }
        }
        break;
    }
    ///////////////////
    case FSM_CMD_WRITE:     // write data but no response (in case of write burst)
    {
        assert( write (r_seg_index, r_address , r_wdata, r_be ) && 
                "VCI_SIMPLE_RAM ERROR : out of bounds access in a write burst" );

        if ( p_vci.cmdval.read() ) 
        {
            vci_addr_t next_address = r_address.read() + (vci_addr_t)vci_param::B;
            assert( ((r_contig && (next_address == p_vci.address.read())) ||
                     (!r_contig && (r_address.read() == p_vci.address.read()))) &&
                        "addresses must be contiguous or constant in a VCI write burst" );
            r_address   = p_vci.address.read();
            r_be        = p_vci.be.read();
            r_wdata     = p_vci.wdata.read();
            if ( p_vci.eop.read() ) 	 r_fsm_state = FSM_RSP_WRITE;
        }
        break;
    }
    ///////////////////
    case FSM_RSP_WRITE: // send response for a write (after receiving last flit) 
    {
        if( p_vci.rspack.read() )
        { 
            assert( write (r_seg_index, r_address , r_wdata, r_be ) && 
                    "out of bounds access in a write burst" );
            if( m_latency )	r_fsm_state = FSM_IDLE;
            else           	r_fsm_state = FSM_CMD_GET;
        }
        break;
    }
    //////////////////
    case FSM_RSP_READ:  // send one response word in a read burst
    {
        if ( p_vci.rspack.read() )
        {
            r_flit_count = r_flit_count - 1;
            if ( r_contig ) 	r_address  = r_address.read() + vci_param::B;
            if ( r_flit_count == 1) 	// last flit 
            {
                if( m_latency )	r_fsm_state = FSM_IDLE;
                else           	r_fsm_state = FSM_CMD_GET;
            }
        }
        break;
    }
    ///////////////////
    case FSM_CMD_ERROR: // waits lat flit of a VCI CMD erroneous packet 
    {
        if ( p_vci.cmdval.read() && p_vci.eop.read() )
        {
            r_fsm_state = FSM_RSP_ERROR;
        }
        break;
    }
    //////////////////
    case FSM_RSP_ERROR: // send a response error
    {
        if ( p_vci.rspack.read() ) 
        {
            if( m_latency )	r_fsm_state = FSM_IDLE;
            else           	r_fsm_state = FSM_CMD_GET;
        }
        break;
    }
    ////////////////
    case FSM_RSP_LL:    // register the LL, and send the response
    {
        if ( p_vci.rspack.read() ) 
        {   
            r_llsc_buf.doLoadLinked(r_address.read(), r_srcid.read());
            if( m_latency )	r_fsm_state = FSM_IDLE;
            else           	r_fsm_state = FSM_CMD_GET;
        }
        break;
    }
    ////////////////
    case FSM_RSP_SC:    // write if SC success, and send the response
    {
        if ( p_vci.rspack.read() ) 
        {    
            if ( r_llsc_buf.isAtomic(r_address.read(), r_srcid.read()) ) 
            {
                r_llsc_buf.accessDone(r_address.read());
                write (r_seg_index, r_address , r_wdata, r_be);
            }
            if( m_latency )	r_fsm_state = FSM_IDLE;
            else           	r_fsm_state = FSM_CMD_GET;
        }
        break;
    }
    /////////////////
    case FSM_CMD_CAS:   // consume the second VCI CMD flit, and compare old/new
    {
        if ( p_vci.cmdval.read() )
        {
            assert( p_vci.eop.read() && 
                    "a VCI SC (CAS) command cannot contain more than two flits" );
            vci_data_t   rdata;
            assert( read(r_seg_index, r_address, rdata) && 
                    "out of bounds access in a read burst" );
            r_cmp_success = ( rdata == r_wdata.read() );
            r_wdata       = p_vci.wdata.read();
            r_be          = 0xF;
            r_fsm_state = FSM_RSP_CAS;
        }
        break;
    }
    /////////////////
    case FSM_RSP_CAS:   // Write if success, and send the response to the CAS
    {
        if ( p_vci.rspack.read() ) 
        {    
            if ( r_cmp_success.read() )
            {
                //In the case where we use both CAS and SC
                r_llsc_buf.accessDone(r_address.read());
                write (r_seg_index, r_address , r_wdata, r_be);
            }
            if( m_latency )	r_fsm_state = FSM_IDLE;
            else           	r_fsm_state = FSM_CMD_GET;
        }
        break;
    }
    } // end switch fsm_state

} // end transition()

///////////////////////
tmpl(void)::genMoore()
{
    switch ( r_fsm_state ) {
    case FSM_IDLE:
    {
        p_vci.cmdack  = false;
        p_vci.rspval  = false;
        p_vci.rdata   = 0;
        p_vci.rsrcid  = 0;
        p_vci.rtrdid  = 0;
        p_vci.rpktid  = 0;
        p_vci.rerror  = 0;
        p_vci.reop    = false;
        break;
    }
    case FSM_CMD_GET:
    case FSM_CMD_WRITE:
    case FSM_CMD_ERROR:
    case FSM_CMD_CAS:
    {
        p_vci.cmdack  = true;
        p_vci.rspval  = false;
        p_vci.rdata   = 0;
        p_vci.rsrcid  = 0;
        p_vci.rtrdid  = 0;
        p_vci.rpktid  = 0;
        p_vci.rerror  = 0;
        p_vci.reop    = false;
        break;
    }
    case FSM_RSP_WRITE:
    {
        p_vci.cmdack  = false;
        p_vci.rspval  = true;
        p_vci.rdata   = 0;
        p_vci.rsrcid  = r_srcid.read();
        p_vci.rtrdid  = r_trdid.read();
        p_vci.rpktid  = r_pktid.read();
        p_vci.rerror  = vci_param::ERR_NORMAL;
        p_vci.reop    = true;
        break;
    }
    case FSM_RSP_READ:
    {
        vci_data_t   rdata;
        assert( read(r_seg_index, r_address, rdata) && 
                "out of bounds access in a read burst" );
        p_vci.cmdack = false;
        p_vci.rspval = true;
        p_vci.rdata  = rdata;
        p_vci.rsrcid = r_srcid.read();
        p_vci.rtrdid = r_trdid.read();
        p_vci.rpktid = r_pktid.read();
        p_vci.rerror = vci_param::ERR_NORMAL;
        p_vci.reop   = (r_flit_count.read() == 1);
        break;
    }
    case FSM_RSP_LL:
    {
        vci_data_t   rdata;
        assert( read(r_seg_index, r_address, rdata) && "out of bounds access in a ll access" );
        p_vci.cmdack = false;
        p_vci.rspval = true;
        p_vci.rdata  = rdata;
        p_vci.rsrcid = r_srcid.read();
        p_vci.rtrdid = r_trdid.read();
        p_vci.rpktid = r_pktid.read();
        p_vci.rerror = vci_param::ERR_NORMAL;
        p_vci.reop   = true;
        break;
    }
    case FSM_RSP_SC:
    {
        p_vci.cmdack = false;
        p_vci.rspval = true;
        if ( r_llsc_buf.isAtomic(r_address.read(), r_srcid.read()) )
            p_vci.rdata = vci_param::STORE_COND_ATOMIC;
        else
            p_vci.rdata = vci_param::STORE_COND_NOT_ATOMIC;
        p_vci.rsrcid = r_srcid.read();
        p_vci.rtrdid = r_trdid.read();
        p_vci.rpktid = r_pktid.read();
        p_vci.rerror = vci_param::ERR_NORMAL;
        p_vci.reop   = true;
        break;
    }
    case FSM_RSP_CAS:
    {
        p_vci.cmdack  = false;
        p_vci.rspval  = true;
        if ( r_cmp_success.read() ) p_vci.rdata = vci_param::STORE_COND_ATOMIC;
        else                        p_vci.rdata = vci_param::STORE_COND_NOT_ATOMIC;
        p_vci.rsrcid  = r_srcid.read();
        p_vci.rtrdid  = r_trdid.read();
        p_vci.rpktid  = r_pktid.read();
        p_vci.rerror  = vci_param::ERR_NORMAL;
        p_vci.reop    = true;
        break;
    }
    case FSM_RSP_ERROR:
    {
        p_vci.cmdack  = false;
        p_vci.rspval  = true;
        p_vci.rdata   = 0;
        p_vci.rsrcid  = r_srcid.read();
        p_vci.rtrdid  = r_trdid.read();
        p_vci.rpktid  = r_pktid.read();
        p_vci.rerror  = vci_param::ERR_GENERAL_DATA_ERROR;
        p_vci.reop    = true;
        break;
    }
    } // end switch fsm_state
} // end genMoore()

}} 

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


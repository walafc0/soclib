/*
 *
 * Authors  : alain.greiner@lip6.fr 
 * Date     : march  2013
 * Copyright: UPMC - LIP6
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
 */

/////////////////////////////////////////////////////////////////////////
//  This component is an over-simplified VCI target that can be used
//  for networks performance analysis. 
//  This component does nor register data, but cheks that the command
//  has been routed to the proper target.
//  It accept only READ and WRITE VCI commands, and the DATA field is
//  not used: In case of READ, the returned RDATA field is always 0.
//  It supports only compact VCI packets:
//  - A READ burst command packet (such a cache line request) 
//    contains one single flit. 
//    The response packet length is defined by the PLEN field.
//  - WRITE burst command packets at consecutive addresses are supported.
//    The zero value for the PLEN field is not supported.
//    Write response packets contain always one single flit.
////////////////////////////////////////////////////////////////////////
//  Implementation note: This component is controlled by a single FSM.
//  The VCI command is acknowledged, and checked in the IDLE state.
//  - For READ, the multi-flit response is sent in the RSP_READ state. 
//  - For WRITE, the single flit response is sent in the RSP_WRITE state.
/////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstring>
#include "vci_synthetic_target.h"

namespace soclib {
namespace caba {

using namespace soclib;

#define tmpl(x) template<typename vci_param> x VciSyntheticTarget<vci_param>

///////////////////////////////
tmpl(/**/)::VciSyntheticTarget(
    sc_module_name name,
    const size_t   tgtid,			// target identifier (x,y,l)
    const size_t   x_width,			// x field width
    const size_t   y_width,			// y field width
    const size_t   l_width )        // l field width
	: caba::BaseModule(name),

      r_fsm_state("r_fsm_state"),
      r_flit_count("r_flit_count"),
      r_srcid("r_srcid"),
      r_trdid("r_trdid"),
      r_pktid("r_pktid"),

      m_x( tgtid >> (y_width + l_width) ),
      m_x_mask( ((1<<x_width) - 1) << (vci_param::N - x_width) ),
      m_x_shift( vci_param::N - x_width ),

      m_y( (tgtid >> l_width) & ((1<<y_width) - 1) ),
      m_y_mask( ((1<<y_width) - 1) << (vci_param::N - x_width - y_width) ),
      m_y_shift( vci_param::N - x_width - y_width ),

      m_l ( tgtid & ((1<<l_width) - 1) ),
      m_l_mask( ((1<<l_width) - 1) << (vci_param::N - x_width - y_width - l_width) ),
      m_l_shift( vci_param::N - x_width - y_width - l_width),

      p_resetn("p_resetn"),
      p_clk("p_clk"),
      p_vci("p_vci")
{
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
}

//////////////////////////
tmpl(void)::print_trace()
{
    const char* state_str[] = { "IDLE", "RSP_READ", "RSP_WRITE" };
    std::cout << "VCI_SYNTHETIC_TARGET " << name() 
              << " : state = " << state_str[r_fsm_state] 
              << " / flit_count = " << r_flit_count << std::endl;
}

/////////////////////////
tmpl(void)::transition()
{
    if (!p_resetn) 
    {
        r_fsm_state = FSM_IDLE;
    }

    switch ( r_fsm_state ) 
    {
        case FSM_IDLE: 
            if ( p_vci.cmdval.read() ) 
            {
                vci_addr_t   address = p_vci.address.read();
                size_t x = (size_t)(address & m_x_mask) >> m_x_shift;
                size_t y = (size_t)(address & m_y_mask) >> m_y_shift;
                size_t l = (size_t)(address & m_l_mask) >> m_l_shift;

                if( not ( (x == m_x) and (y == m_y) and (l == m_l) ) )
                {
                    std::cout << "ERROR in VCI_SYNTHETIC_TARGET " << name()
                         << " : address = " << std::hex << address << std::endl
                         << " x = " << x << " / m_x = " << m_x << std::endl
                         << " y = " << y << " / m_y = " << m_y << std::endl
                         << " l = " << l << " / m_l = " << m_l << std::endl;
                    exit(0);
                }

                r_srcid      = p_vci.srcid.read();
                r_trdid      = p_vci.trdid.read();
                r_pktid      = p_vci.pktid.read();

                assert( (p_vci.plen.read() != 0) and
                "ERROR in VCI_SYNTHETIC_TARGET: plen == 0");

                if ( p_vci.cmd.read() == vci_param::CMD_WRITE ) 
                {
                    if( p_vci.eop.read() )  r_fsm_state = FSM_RSP_WRITE;
                }
                else if ( p_vci.cmd.read() == vci_param::CMD_READ )
                {
                    r_flit_count = p_vci.plen.read()/vci_param::B;
                    r_fsm_state = FSM_RSP_READ;

                    assert( p_vci.eop.read() and
                    "ERROR in VCI_SYNTHETIC_TARGET: read command is not one flit");
            }
            else    
            {
                assert( false and
                "ERROR in VCI_SYNTHETIC_TARGET: only READ & WRITE commands");
            }
        }
        break;
        case FSM_RSP_WRITE:
            if( p_vci.rspack.read() )
            { 
                r_fsm_state = FSM_IDLE;
            }
        break;
        case FSM_RSP_READ:
            if ( p_vci.rspack.read() )
            {
                r_flit_count = r_flit_count - 1;
                if ( r_flit_count == 1) 	// last flit 
                {
                	r_fsm_state = FSM_IDLE;
                }
            }
        break;
    } // end switch fsm_state

} // end transition()

///////////////////////
tmpl(void)::genMoore()
{
    switch ( r_fsm_state.read() ) 
    {
        case FSM_IDLE:
        p_vci.cmdack  = true;
        p_vci.rspval  = false;
        p_vci.rdata   = 0;
        p_vci.rsrcid  = 0;
        p_vci.rtrdid  = 0;
        p_vci.rpktid  = 0;
        p_vci.rerror  = 0;
        p_vci.reop    = false;
        break;
        case FSM_RSP_WRITE:
        p_vci.cmdack  = false;
        p_vci.rspval  = true;
        p_vci.rdata   = 0;
        p_vci.rsrcid  = r_srcid.read();
        p_vci.rtrdid  = r_trdid.read();
        p_vci.rpktid  = r_pktid.read();
        p_vci.rerror  = vci_param::ERR_NORMAL;
        p_vci.reop    = true;
        break;
        case FSM_RSP_READ:
        p_vci.cmdack = false;
        p_vci.rspval = true;
        p_vci.rdata  = 0;
        p_vci.rsrcid = r_srcid.read();
        p_vci.rtrdid = r_trdid.read();
        p_vci.rpktid = r_pktid.read();
        p_vci.rerror = vci_param::ERR_NORMAL;
        p_vci.reop   = (r_flit_count.read() == 1);
        break;
    } // end switch fsm_state
} // end genMoore()

}} 



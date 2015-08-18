/* -*- c++ -*-
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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: alain
 */

#include "../include/vci_multi_tty.h"
#include "tty.h"

#include <stdarg.h>

namespace soclib {
namespace caba {

#define tmpl(typ) template<typename vci_param> typ VciMultiTty<vci_param>

////////////////////////
tmpl(void)::transition()
{
	if (!p_resetn) 
    {
		r_fsm_state  = IDLE;
        r_cpt_read   = 0;
        r_cpt_write  = 0;
        r_lock       = false;
		return;
	}

    switch ( r_fsm_state.read() )
    {
        //////////
        case IDLE:   // waiting a VCI command
        {
            if ( not p_vci.cmdval.read() ) break;

            vci_addr_t address = p_vci.address.read();
            size_t     cell      = (size_t)((address & 0xFFF)>>2);
            size_t     reg       = cell % TTY_SPAN;
            size_t     channel   = cell / TTY_SPAN;
            bool       seg_error = true;

            r_srcid = p_vci.srcid.read();
            r_trdid = p_vci.trdid.read();
            r_pktid = p_vci.pktid.read();

            std::list<soclib::common::Segment>::iterator seg;
            for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ )
            {
                if ( seg->contains( address ) and p_vci.eop.read() )
                {
                    seg_error   = false;
                    break;
                }
            }

            /////////////////////////////////////////////////////////
            if      ( not seg_error and            // write char
                      (p_vci.cmd.read() == vci_param::CMD_WRITE) and
                      (reg == TTY_WRITE ) and 
                      channel < m_term.size() )
            {
                m_term[channel]->putc( p_vci.wdata.read() );
                r_cpt_write = r_cpt_write.read() + 1; 
                r_fsm_state = RSP_WRITE;
            }     
            /////////////////////////////////////////////////////////
            else if ( not seg_error and            // release lock
                      (p_vci.cmd.read() == vci_param::CMD_WRITE) and
                      (reg == TTY_CONFIG) and 
                      channel < m_term.size() )
            {
                r_lock      = false;
                r_fsm_state = RSP_WRITE;
            }
            /////////////////////////////////////////////////////////
            else if ( not seg_error and            // try to get lock
                      (p_vci.cmd.read() == vci_param::CMD_READ) and
                      (reg == TTY_CONFIG) and 
                      channel < m_term.size() )
            {
                r_rdata     = (vci_data_t)r_lock.read();
                r_lock      = true;
                r_fsm_state = RSP_READ;
            }
            /////////////////////////////////////////////////////////
            else if ( not seg_error and            // read char 
                      (p_vci.cmd.read() == vci_param::CMD_READ) and
                      (reg == TTY_READ) and 
                      channel < m_term.size() )
            {
                vci_data_t  byte = m_term[channel]->getc();
                if ( byte == 0x0d ) r_rdata = m_term[channel]->getc();
                else                r_rdata = byte;
                r_cpt_read  = r_cpt_read.read() + 1; 
                r_fsm_state = RSP_READ;
            }
            /////////////////////////////////////////////////////////
            else if ( not seg_error and            // read status
                      (p_vci.cmd.read() == vci_param::CMD_READ) and
                      (reg == TTY_STATUS) and 
                      channel < m_term.size() )
            {
                r_rdata     = m_term[channel]->hasData();
                r_fsm_state = RSP_READ;
            }
            /////////////////////////////////////////////////////////
            else if ( p_vci.eop.read() )
            {
                r_fsm_state = RSP_ERROR;
            }
            break;
        }
        ///////////////
        case RSP_ERROR:   // return an error response
        case RSP_READ:    // return a valid read response
        case RSP_WRITE:   // return a valid write response
        {
            if ( p_vci.rspack.read() ) r_fsm_state = IDLE;
            break;
        }
    }  // end switch r_fsm_state
}

//////////////////////
tmpl(void)::genMoore()
{
    if ( r_fsm_state.read() == IDLE )
    {
        p_vci.cmdack  = true;
        p_vci.rspval  = false;
        p_vci.rdata   = 0;
        p_vci.rsrcid  = 0;
        p_vci.rtrdid  = 0;
        p_vci.rpktid  = 0;
        p_vci.rerror  = vci_param::ERR_NORMAL;
        p_vci.reop    = true;
    }
    else if ( r_fsm_state.read() == RSP_WRITE )
    {
        p_vci.cmdack  = false;
        p_vci.rspval  = true;
        p_vci.rdata   = 0;
        p_vci.rsrcid  = r_srcid.read();
        p_vci.rtrdid  = r_trdid.read();
        p_vci.rpktid  = r_pktid.read();
        p_vci.rerror  = vci_param::ERR_NORMAL;
        p_vci.reop    = true;
    }
    else if ( r_fsm_state.read() == RSP_READ )
    {
        p_vci.cmdack  = false;
        p_vci.rspval  = true;
        p_vci.rdata   = r_rdata.read();
        p_vci.rsrcid  = r_srcid.read();
        p_vci.rtrdid  = r_trdid.read();
        p_vci.rpktid  = r_pktid.read();
        p_vci.rerror  = vci_param::ERR_NORMAL;
        p_vci.reop    = true;
    }
    else if ( r_fsm_state.read() == RSP_ERROR )
    {
        p_vci.cmdack  = false;
        p_vci.rspval  = true;
        p_vci.rdata   = 0;
        p_vci.rsrcid  = r_srcid.read();
        p_vci.rtrdid  = r_trdid.read();
        p_vci.rpktid  = r_pktid.read();
        p_vci.rerror  = vci_param::ERR_GENERAL_DATA_ERROR;
        p_vci.reop    = true;
    }
        
    for ( size_t i=0; i<m_term.size(); i++ )
        p_irq[i] = m_term[i]->hasData();
}

///////////////////////////////////////////////////////
tmpl(void)::init(const std::vector<std::string> &names)
{
    for ( std::vector<std::string>::const_iterator i = names.begin();
          i != names.end();
          ++i )
		m_term.push_back(soclib::common::allocateTty(*i));

	p_irq = new sc_out<bool>[m_term.size()];
    
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

//////////////////////////////////////////////////
tmpl(/**/)::VciMultiTty( sc_module_name     name,
                         const IntTab       &index,
                         const MappingTable &mt,
                         const char *first_name, ...)
    : soclib::caba::BaseModule(name),
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci"),
      m_seglist(mt.getSegmentList(index))
{
    std::cout << "  - Building VciMultiTTy " << name << std::endl;

    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ )
    {
        std::cout << "    => segment " << seg->name()
                  << " / base = " << std::hex << seg->baseAddress()
                  << " / size = " << seg->size() << std::endl; 
    }

	va_list va_tty;

	va_start (va_tty, first_name);
    std::vector<std::string> args;
	const char *cur_tty = first_name;
	while (cur_tty) 
    {
        args.push_back(cur_tty);

		cur_tty = va_arg( va_tty, char * );
	}
	va_end( va_tty );

    init(args);
}

//////////////////////////////////////////////////////////////
tmpl(/**/)::VciMultiTty( sc_module_name                  name,
                         const IntTab                    &index,
                         const MappingTable              &mt,
                         const std::vector<std::string>  &names )
    : soclib::caba::BaseModule(name),
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci"),
      m_seglist(mt.getSegmentList(index))
{
    std::cout << "  - Building VciMultiTTy " << name << std::endl;

    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ )
    {
        std::cout << "    => segment " << seg->name()
                  << " / base = " << std::hex << seg->baseAddress()
                  << " / size = " << seg->size() << std::endl; 
    }

    init(names);
}

//////////////////////////
tmpl(/**/)::~VciMultiTty()
{
	for (unsigned int i=0; i<m_term.size(); i++ )
        delete m_term[i];

	delete[] p_irq;
}

/////////////////////////
tmpl(void)::print_stats()
{
    std::cout << name() << std::endl;
    std::cout << "- READ    = " << r_cpt_read.read() << std::endl;
    std::cout << "- WRITE   = " << r_cpt_write.read() << std::endl;
}

/////////////////////////
tmpl(void)::print_trace()
{
    const char* state_str[] = { "IDLE",
                                "RSP_WRITE",
                                "RSP_READ",
                                "RSP_ERROR", };

    std::cout << "MULTI_TTY " << name()
              << " : state = " << state_str[r_fsm_state.read()] << std::endl; 
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


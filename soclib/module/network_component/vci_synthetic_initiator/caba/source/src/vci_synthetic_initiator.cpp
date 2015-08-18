
/* -*- c++ -*-
 * File 	: vci_synthetic_initiator.cpp
 * Date 	: march/2013
 * Copyright: UPMC / LIP6
 * Authors 	: Alain Greiner 
 * Version	: 1.0
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
 * Maintainers: alain.greiner@lip6.fr
 */

#include "../include/vci_synthetic_initiator.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param> x VciSyntheticInitiator<vci_param>

//////////////////////////////////
tmpl(/**/)::VciSyntheticInitiator( 
   sc_module_name name,
   const uint32_t srcid,          // VCI SRCID
   const uint32_t length,         // Packet length (flit numbers)
   const uint32_t load,           // Offered load * 1000
   const uint32_t fifo_depth )    // Fifo depth

    : soclib::caba::BaseModule(name),

    p_clk("clk"),
    p_resetn("resetn"),
    p_vci("vci_ini"),

    m_srcid( srcid ),
    m_length( length ),
    m_load( load ),
    m_pending_size( 1<<vci_param::T ),

    r_date_fifo( "r_date_fifo", fifo_depth ),
    r_vci_fsm( "r_vci_fsm" ),
    r_vci_address( "r_vci_address" ),  		
    r_vci_trdid( "r_vci_trdid" ),	
    r_vci_count("r_vci_count"),  	
    r_cycles( "r_cycles" ),  		
    r_nb_read( "r_nb_read" ),  		
    r_nb_write( "r_nb_write" ),  		
    r_latency_read( "r_latency_read" ),	
    r_latency_write( "r_latency_write" )	
{
    r_pending_fsm = new sc_signal<int>[1<<vci_param::T];
    r_pending_date = new sc_signal<uint64_t>[1<<vci_param::T];

    assert( (vci_param::N >= 32) and
    "ERROR in VCI_SYNTHETIC_INITIATOR: The VCI ADDRESS width must be larger than 32");

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
} // end constructor

/////////////////////////
tmpl(void)::print_trace()
{
  	const char* state_cmd_str[] = { "IDLE", "READ", "WRITE"};

    size_t load = (r_nb_posted.read() * m_length * 1000) / (r_cycles.read() + 1);

	std::cout << "VCI_SYNTHETIC_INITIATOR " << name() << std::dec
		  << " : cmd_fsm = " << state_cmd_str[r_vci_fsm.read()] 
          << " / fifo content = " << r_date_fifo.filled_status() << std::endl;
}

/////////////////////////
tmpl(void)::print_stats()
{
    size_t load = ((r_nb_read.read()+r_nb_write.read()) * m_length * 1000) / (r_cycles.read() + 1);

	std::cout << "VCI_SYNTHETIC_INITIATOR " << name() << std::dec << std::endl
          << " - cycles        = " << r_cycles.read() << std::endl
          << " - nb_read       = " << r_nb_read.read() << std::endl
          << " - latency_read  = " << r_latency_read.read() / (r_nb_read.read()+1) << std::endl
          << " - nb_write      = " << r_nb_write.read() << std::endl
          << " - latency_write = " << r_latency_write.read() / (r_nb_write.read()+1) << std::endl
          << " - offered load  = " << m_load << std::endl
          << " - accepted load = " << load << std::endl;
}

////////////////////////
tmpl(void)::transition()
{
    //  RESET          
    if ( not p_resetn.read() ) 
    {
        srandom(m_srcid);
        r_vci_fsm = VCI_IDLE;
        for(size_t i=0 ; i<m_pending_size ; i++) r_pending_fsm[i] = PENDING_IDLE;
        r_date_fifo.init();
        r_latency_write = 0;
        r_latency_read  = 0;
        r_nb_write      = 0;
        r_nb_read       = 0;
        r_nb_posted     = 0;
        r_cycles        = 0;
        return;
    }

    // default values
    bool    fifo_put = false;
    bool    fifo_get = false;

    uint32_t alea = random();

    // VCI CMD FSM 
    switch ( r_vci_fsm.read() ) 
    {
        case VCI_IDLE:
	        if (r_date_fifo.rok())
            {
                // looking for a free slot in pending table
	            size_t id = m_pending_size;
                for(size_t i = 0; i < m_pending_size; i++)
                {  	
	                if( r_pending_fsm[i].read() == PENDING_IDLE )
                    {
		                id = i;
		                break;
		            }
	            }
                // start a VCI transaction if slot found
	            if( id < m_pending_size )
                {
                    // 75% write / 25% READ
                    if ( alea & 0x03 ) r_vci_fsm = VCI_WRITE;
                    else               r_vci_fsm = VCI_READ;
	                r_vci_count   = 0;
                    r_vci_trdid   = id;
	                r_vci_address = (uint64_t)((alea & 0x0000FFFF) << (vci_param::N - 16));
	            }
            }
        break;
        case VCI_READ:
	        if ( p_vci.cmdack.read() )
            {
                r_vci_fsm = VCI_IDLE;
	            r_pending_date[r_vci_trdid.read()] = r_date_fifo.read();
                r_pending_fsm[r_vci_trdid.read()]  = PENDING_READ;
                fifo_get = true;
            }
        break;
        case VCI_WRITE:
	        if ( p_vci.cmdack.read() )
            {
	            r_vci_count = r_vci_count.read() + 1;
	            if (r_vci_count.read() == m_length-1) 
                {
                    r_vci_fsm = VCI_IDLE;
	                r_pending_date[r_vci_trdid.read()] = r_date_fifo.read();
                    r_pending_fsm[r_vci_trdid.read()]  = PENDING_WRITE;
                    fifo_get = true;
                }
	        }
        break;
    } // end switch vci_fsm

    // pending FSMs
    if( p_vci.rspval.read() and p_vci.reop.read() )  // last flit received
    {
        size_t index = (size_t)p_vci.rtrdid.read();
        if ( r_pending_fsm[index] == PENDING_WRITE )
        {
            r_pending_fsm[index] = PENDING_IDLE;
            r_nb_write      = r_nb_write.read() + 1;
	        r_latency_write = r_latency_write.read() + 
               (r_cycles.read() - r_pending_date[index].read());
        }
        else if ( r_pending_fsm[index] == PENDING_READ )
        {
            r_pending_fsm[index] = PENDING_IDLE;
            r_nb_read       = r_nb_read.read() + 1;
	        r_latency_read = r_latency_read.read() + 
               (r_cycles.read() - r_pending_date[index].read());
        }
        else
        {
            std::cout << "ERROR in VCI_SYNTHETIC_INITIATOR " << name()
                      << " : illegal VCI response "
                      << " / r_pending_fsm[" << index 
                      << "] = " << r_pending_fsm[index] << std::endl;
            exit(0);
        }
    }

    //  traffic generator
    size_t accepted_load = (r_nb_posted.read() * m_length * 1000) / (r_cycles.read() + 1);
    if( (accepted_load + (alea>>16 & 0xF)) < (m_load) )
    {
       fifo_put = true ;
       if( r_date_fifo.wok() ) r_nb_posted = r_nb_posted.read() + 1;
    }

    //  update fifos
    r_date_fifo.update( fifo_get, fifo_put, r_cycles.read() );

    //  increment local time
    r_cycles = r_cycles.read() + 1;

} // end transition()

/////////////////////////////
tmpl(void)::genMoore()
/////////////////////////////
{
    p_vci.be         = 0xF;                             
    p_vci.cons       = false;        
    p_vci.wrap       = false;       
    p_vci.contig     = true;       
    p_vci.clen       = 0;         
    p_vci.cfixed     = false;           
    p_vci.rspack     = true;
	p_vci.plen       = m_length*4;                                         
	p_vci.wdata      = 0;                                       
    p_vci.srcid      = m_srcid;   
	p_vci.trdid      = r_vci_trdid.read();                  
	p_vci.pktid      = 0;      

    switch ( r_vci_fsm.read() ) 
    {
        case VCI_IDLE:
	        p_vci.cmdval  = false;                 
        break;
        case VCI_WRITE:
	        p_vci.cmdval  = true;                 
            p_vci.cmd     = vci_param::CMD_WRITE;   
	        p_vci.address = (addr_t)(r_vci_address.read() + (r_vci_count.read()*4)); 
	        if (r_vci_count.read() == m_length - 1 ) p_vci.eop = true; 
	        else                                     p_vci.eop = false; 
        break;
        case VCI_READ:
       	    p_vci.cmdval  = true;                 
            p_vci.cmd     = vci_param::CMD_READ;   
	        p_vci.address = (addr_t) r_vci_address.read(); 
	        p_vci.eop     = true;                                   
        break;
    } // end switch vci_cmd_fsm

} // end genMoore()

}} // end name space

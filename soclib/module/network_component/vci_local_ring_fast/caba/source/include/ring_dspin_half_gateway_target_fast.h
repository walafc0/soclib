/* -*- c++ -*-
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
 * Author   : Abdelmalek SI MERABET
 * Date     : Februrary 2011
 * Copyright: UPMC - LIP6
 */

#ifndef RING_DSPIN_HALF_GATEWAY_TARGET_FAST_H
#define RING_DSPIN_HALF_GATEWAY_TARGET_FAST_H

#include "vci_initiator.h"
#include "generic_fifo.h"
#include "mapping_table.h"
#include "ring_signals_fast.h"
#include "dspin_interface.h"

// #define HT_DEBUG

namespace soclib { namespace caba {

using soclib::common::IntTab;

namespace {
const char *ring_rsp_fsm_state_str_ht[] = {
                "RSP_IDLE",
                "DEFAULT",
                "SENDING",
                "PREEMPT",
        };

#ifdef HT_DEBUG

const char *ring_cmd_fsm_state_str_ht[] = {
        "CMD_IDLE",
        "ALLOC",
        "NALLOC",
        };
#endif

} // end namespace

template<typename vci_param, int ring_cmd_data_size, int ring_rsp_data_size>
class RingDspinHalfGatewayTargetFast
{

typedef typename vci_param::fast_addr_t vci_addr_t;
typedef LocalRingSignals ring_signal_t; 
typedef DspinOutput<ring_cmd_data_size >  cmd_out_t;
typedef DspinInput<ring_rsp_data_size >   rsp_in_t;

private:
        
        enum ring_cmd_fsm_state_e {
        	CMD_IDLE,	 // waiting for first flit of a command packet
        	ALLOC,  	// next flit of a local cmd packet
        	NALLOC,         // next flit of a ring cmd packet
        };
        
        // cmd token allocation fsm
        enum ring_rsp_fsm_state_e {
        	RSP_IDLE,	    
        	DEFAULT,  	
        	SENDING, 
                PREEMPT,	            
        };
        
        // structural parameters
 	std::string   m_name;
        bool          m_alloc_target;
 
        // internal fifos 
        GenericFifo<uint64_t > m_cmd_fifo;     
        GenericFifo<uint64_t > m_rsp_fifo;     
        
        // locality table 
	soclib::common::AddressDecodingTable<vci_addr_t, bool> m_lt_addr;
	soclib::common::AddressDecodingTable<uint32_t, bool> m_lt_src;
        //soclib::common::IntTab m_ringid;

        bool          m_local;

        // internal registers
        sc_core::sc_signal<int>	        r_ring_cmd_fsm;	    // ring command packet FSM 
        sc_core::sc_signal<int>		r_ring_rsp_fsm;	    // ring response packet FSM

#ifdef HT_DEBUG
        uint32_t            m_cpt;
	uint32_t            m_cyc;
#endif

public :

#define __renRegGateTgt(x) x((((std::string) name)+"_" #x).c_str())

RingDspinHalfGatewayTargetFast(
	const char     *name,
        bool            alloc_target,
        const int       &wrapper_fifo_depth,
        const soclib::common::MappingTable &mt,
        const soclib::common::IntTab &ringid,
        bool  local) 
    :   m_name(name), 
        m_alloc_target(alloc_target),
        m_cmd_fifo("m_cmd_fifo", wrapper_fifo_depth),
        m_rsp_fifo("m_rsp_fifo", wrapper_fifo_depth),
        m_lt_addr(mt.getLocalityTable<typename vci_param::fast_addr_t>(ringid)),
        m_lt_src(mt.getIdLocalityTable(ringid)),
        m_local(local),
        __renRegGateTgt(r_ring_cmd_fsm),
        __renRegGateTgt(r_ring_rsp_fsm)
{
} //  end constructor

void reset()
{
        if(m_alloc_target)
        	r_ring_rsp_fsm = DEFAULT;
        else
        	r_ring_rsp_fsm = RSP_IDLE;
        
        r_ring_cmd_fsm = CMD_IDLE;
        m_cmd_fifo.init();
        m_rsp_fifo.init(); 

#ifdef HT_DEBUG 
	m_cpt = 0;
	m_cyc = (uint32_t) atoi(getenv("CYCLES"));   
#endif
}
////////////////////////////////
//	transition 
////////////////////////////////
void transition(const cmd_out_t &p_gate_cmd_out, const rsp_in_t &p_gate_rsp_in, const ring_signal_t p_ring_in, bool &tgt_cmd_val, rsp_str &tgt_rsp, const bool iga)
{

//	bool      cmd_fifo_get = false;
	bool      cmd_fifo_put = false;
	uint64_t  cmd_fifo_data = 0;
	
	bool      rsp_fifo_get = false;
	bool      rsp_fifo_put = false;
	uint64_t  rsp_fifo_data = 0;

	
//////////// VCI CMD FSM /////////////////////////

	if (p_gate_rsp_in.write) {
		rsp_fifo_data = (uint64_t) p_gate_rsp_in.data.read();
		rsp_fifo_put =  m_rsp_fifo.wok();
	}

	bool cmd_fifo_get = p_gate_cmd_out.read;
   
//////////// NET RSP FSM (distributed) /////////////////////////
        
	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE: 
                { 
                         bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);
#ifdef HT_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name
          << " - ring_rsp_fsm : " << ring_rsp_fsm_state_str_ht[r_ring_rsp_fsm]
          << " - fifo rok : " <<  m_rsp_fifo.rok()
          << " - fifo data : " <<  std::hex << m_rsp_fifo.read()
          << " - in grant : " << p_ring_in.rsp_grant
          << " - in iga : " << iga
          << " - in wok : " << p_ring_in.rsp_r
          << " - in data : " << p_ring_in.rsp_data
          << std::endl;
#endif

                        if(m_rsp_fifo.rok() && iga)
                        {
                                rsp_fifo_get = p_ring_in.rsp_r;
                                //if(p_ring_in.rsp_r && eop)

                                if(eop)
                                        r_ring_rsp_fsm = RSP_IDLE;
                                else 
                                        r_ring_rsp_fsm = PREEMPT;
                                break;
                        }
			if (m_rsp_fifo.rok() && p_ring_in.rsp_grant) 
				r_ring_rsp_fsm = SENDING;           
                }
		break;

		case DEFAULT: 
                {

#ifdef HT_DEBUG
 if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
                         << " - ring_rsp_fsm  = " << ring_rsp_fsm_state_str_ht[r_ring_rsp_fsm] 
                         << " - fifo ROK : " << m_rsp_fifo.rok()
                         << " - in grant : " << p_ring_in.rsp_grant
                         << " - in wok : " << p_ring_in.rsp_r
                         << " - fifo data : " << std::hex << m_rsp_fifo.read()
                         << std::endl;
#endif
		
                        bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);

                        if ( m_rsp_fifo.rok() && eop && p_ring_in.rsp_r )  
			{
				rsp_fifo_get = true;
                                if ( !p_ring_in.rsp_grant )
				        r_ring_rsp_fsm = RSP_IDLE;
                                else
				        r_ring_rsp_fsm = DEFAULT;
			}   
                        if ( m_rsp_fifo.rok() && (!eop || !p_ring_in.rsp_r)) 
			{
				rsp_fifo_get = p_ring_in.rsp_r;
				r_ring_rsp_fsm = SENDING;
			}

                        if ( !m_rsp_fifo.rok() && !p_ring_in.rsp_grant )
				r_ring_rsp_fsm = RSP_IDLE; 

                        if ( !m_rsp_fifo.rok() && p_ring_in.rsp_grant )
				r_ring_rsp_fsm = DEFAULT;
                }  
		break;

		case SENDING:   
#ifdef HT_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
                         << " - ring_rsp_fsm  = " << ring_rsp_fsm_state_str_ht[r_ring_rsp_fsm] 
                         << " - fifo ROK : " << m_rsp_fifo.rok()
                         << " - in grant : " << p_ring_in.rsp_grant
                         << " - in wok : " << p_ring_in.rsp_r
                         << " - fifo data : " << std::hex << m_rsp_fifo.read()
                         << std::endl;
#endif            
			if(m_rsp_fifo.rok() && p_ring_in.rsp_r) 
			{

                                bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);

				rsp_fifo_get = true;             
 
				if (eop)  
				{             
					if ( p_ring_in.rsp_grant )
						r_ring_rsp_fsm = DEFAULT;  
					else   
						r_ring_rsp_fsm = RSP_IDLE;                
				} 
			}           
		break;

		case PREEMPT:   
#ifdef HT_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
                         << " - ring_rsp_fsm  = " << ring_rsp_fsm_state_str_ht[r_ring_rsp_fsm] 
                         << " - fifo ROK : " << m_rsp_fifo.rok()
                         << " - in grant : " << p_ring_in.rsp_grant
                         << " - in iga : " << iga
                         << " - in wok : " << p_ring_in.rsp_r
                         << " - fifo data : " << std::hex << m_rsp_fifo.read()
                         << std::endl;
#endif                 
			if(m_rsp_fifo.rok() && p_ring_in.rsp_r ) 
			{
				rsp_fifo_get = true; 
                                bool eop = ((int) (m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1;
				if (eop) 
				{  
					r_ring_rsp_fsm = RSP_IDLE; 
				}        
			}      
		break;
	} // end switch ring rsp fsm

/////////// NET CMD FSM ////////////////////////
	switch( r_ring_cmd_fsm ) 
	{

		case CMD_IDLE:
                // condition de broadcast pour local target : brdcst.!c1.c2
                // brdcst : flit(0) = 1
                // c1 : coord = 0 
                // c2 : srcid local (broadcast comes from local initiator, to avoid looping on brdcst inifinitly)   
		{
                        vci_addr_t rtgtid = (vci_addr_t) ((p_ring_in.cmd_data >> (ring_cmd_data_size-vci_param::N+1)) << 2);
                        uint32_t   rsrcid =  (uint32_t) ((sc_dt::sc_uint<vci_param::S>) (p_ring_in.cmd_data >> 5)); 
                        bool       brdcst = (p_ring_in.cmd_data & 0x1) == 0X1;                   
                        bool       c1     = (p_ring_in.cmd_data >> 19) == 0;
                        bool       c2     = m_lt_src[rsrcid];
                        bool       loc    = !m_lt_addr[rtgtid] && !m_local;
                        bool       eop    = ((int)((p_ring_in.cmd_data >> (ring_cmd_data_size - 1) ) & 0x1) == 1);
 
#ifdef HT_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
              << " - ring_cmd_fsm = " << ring_cmd_fsm_state_str_ht[r_ring_cmd_fsm]
              << " - in preempt : " << p_ring_in.cmd_preempt
              << " - in rok : " << p_ring_in.cmd_w
	      << " - addr : " << std::hex << rtgtid
              << " - brdcst : " << brdcst
              << " - c1 (coord = 0) : " << c1
              << " - c2 (srcid local) : " << c2  
              << " - isloc : " << loc 
              << " - in wok : " << p_ring_in.cmd_r
              << " - fifo wok : " << m_cmd_fifo.wok()
              << std::endl;
#endif

                        if(p_ring_in.cmd_w && !eop)  
                        {
                                if(brdcst && (c1 || !c2)) 
                                        r_ring_cmd_fsm = NALLOC;
                                if(brdcst && !c1 && c2) 
                                {
                                        r_ring_cmd_fsm = ALLOC; 
                                        cmd_fifo_put   = m_cmd_fifo.wok();
                                        cmd_fifo_data  = p_ring_in.cmd_data;
                                }
                                if(!brdcst && loc) 
                                {
                                        r_ring_cmd_fsm = ALLOC; 
                                        cmd_fifo_put   = m_cmd_fifo.wok();
                                        cmd_fifo_data  = p_ring_in.cmd_data;  

                                } 
        
                                if (!brdcst && !loc) {
                                        r_ring_cmd_fsm = NALLOC;
                                }  

                        } // end if rok.!eop 
                }
		break;

		case ALLOC:  
                // in this state, ring can be preempted by Init gate
                {
                        bool eop = ( (int) ((p_ring_in.cmd_data >> (ring_cmd_data_size - 1) ) & 0x1) == 1);
#ifdef HT_DEBUG
if(m_cpt > m_cyc)

   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
              << " - ring_cmd_fsm = " << ring_cmd_fsm_state_str_ht[r_ring_cmd_fsm] 
              << " - in preempt : " << p_ring_in.cmd_preempt
              << " - in rok : " << p_ring_in.cmd_w
              << " - in wok : " << p_ring_in.cmd_r 
              << " - in data : " << std::hex << p_ring_in.cmd_data
              << " - fifo wok : " << m_cmd_fifo.wok()	
	      << " - eop : " << eop
              << std::endl;
#endif

                        if(p_ring_in.cmd_preempt) break;

                 	if ( p_ring_in.cmd_w && m_cmd_fifo.wok() && eop )
                        { 

				cmd_fifo_put  = true;
				cmd_fifo_data = p_ring_in.cmd_data;
				if(p_ring_in.cmd_palloc)
			     		r_ring_cmd_fsm = NALLOC;
				else
			     		r_ring_cmd_fsm = CMD_IDLE;		
                        }
                        
                 	else // !p_ring_in.cmd_w || !m_cmd_fifo.wok() || !eop 
                        { 

				cmd_fifo_put  = p_ring_in.cmd_w && m_cmd_fifo.wok();
				cmd_fifo_data = p_ring_in.cmd_data;
			     	r_ring_cmd_fsm = ALLOC;
			     		
                        }                        
                } 
		break;

		case NALLOC:   
                { 
 
			bool eop = ( (int) ((p_ring_in.cmd_data >> (ring_cmd_data_size - 1) ) & 0x1) == 1);
#ifdef HT_DEBUG
if(m_cpt > m_cyc)

   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
              << " - ring_cmd_fsm = " << ring_cmd_fsm_state_str_ht[r_ring_cmd_fsm]
              << " - in rok : " << p_ring_in.cmd_w
              << " - in data : " << std::hex << p_ring_in.cmd_data
              << " - in wok : " << p_ring_in.cmd_r
	      << " - eop : " << eop
              << std::endl;
#endif 

			if (p_ring_in.cmd_w && eop && p_ring_in.cmd_r) {        
        			r_ring_cmd_fsm = CMD_IDLE;
                        }
                        else {
 				r_ring_cmd_fsm = NALLOC;
                        }
                }
		break;
	} // end switch cmd fsm

#ifdef HT_DEBUG
 	m_cpt+=1;
#endif

    ////////////////////////
    //  fifos update      //
   ////////////////////////
//-- keep trace on ring traffic
        tgt_rsp.rspval  = rsp_fifo_get;
        tgt_rsp.flit    = m_rsp_fifo.read();
        tgt_rsp.state   = ring_rsp_fsm_state_str_ht[r_ring_rsp_fsm];

	tgt_cmd_val = cmd_fifo_put;
	//tgt_rsp_val = rsp_fifo_get;
// local cmd fifo update
	if ( cmd_fifo_put && cmd_fifo_get ) m_cmd_fifo.put_and_get(cmd_fifo_data);
	else if (  cmd_fifo_put && !cmd_fifo_get ) m_cmd_fifo.simple_put(cmd_fifo_data);
	else if ( !cmd_fifo_put && cmd_fifo_get ) m_cmd_fifo.simple_get();
// local rsp fifo update
	if (  rsp_fifo_put &&  rsp_fifo_get ) m_rsp_fifo.put_and_get(rsp_fifo_data);
	else if (  rsp_fifo_put && !rsp_fifo_get ) m_rsp_fifo.simple_put(rsp_fifo_data);
	else if ( !rsp_fifo_put &&  rsp_fifo_get ) m_rsp_fifo.simple_get();
 
}  // end Transition()
  
///////////////////////////////////////////////////////////////////
void genMoore(cmd_out_t &p_gate_cmd_out, rsp_in_t &p_gate_rsp_in)
///////////////////////////////////////////////////////////////////
{
	p_gate_cmd_out.write = m_cmd_fifo.rok();
	p_gate_cmd_out.data  = (sc_dt::sc_uint<ring_cmd_data_size>) m_cmd_fifo.read();

	p_gate_rsp_in.read = m_rsp_fifo.wok();

} // end genMoore

/////////////////////////////////////////////////////////////////////////////
void update_ring_signals(ring_signal_t p_ring_in, ring_signal_t &p_ring_out, bool &tga, bool iga)
////////////////////////////////////////////////////////////////////////////
{

	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE:
               {

			p_ring_out.rsp_grant = !m_rsp_fifo.rok() && p_ring_in.rsp_grant;

                        p_ring_out.rsp_preempt   = m_rsp_fifo.rok() && iga; 
                        p_ring_out.rsp_palloc    = m_rsp_fifo.rok() && iga; 
                        p_ring_out.rsp_header    = m_rsp_fifo.rok();
 
                        if (m_rsp_fifo.rok() && iga) 
                        {
                                p_ring_out.rsp_w     =  m_rsp_fifo.rok();
		                p_ring_out.rsp_data  =  m_rsp_fifo.read();
                        }
                        else
                        {
        		        p_ring_out.rsp_w     = p_ring_in.rsp_w;
	        	        p_ring_out.rsp_data  = p_ring_in.rsp_data;
                        }

                }
		break;

		case DEFAULT:
                { 
                        bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);
			p_ring_out.rsp_grant = (!m_rsp_fifo.rok() || (eop && p_ring_in.rsp_r)) ;
 
                        p_ring_out.rsp_preempt = 0;
                        p_ring_out.rsp_header  = 0;
                        p_ring_out.rsp_palloc  = 0;

        		p_ring_out.rsp_w    =  m_rsp_fifo.rok();
	        	p_ring_out.rsp_data =  m_rsp_fifo.read(); 

                }
		break;

		case SENDING: 
                { 
                        bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);
			p_ring_out.rsp_grant = m_rsp_fifo.rok() && p_ring_in.rsp_r && eop;

                        p_ring_out.rsp_preempt = 0;
                        p_ring_out.rsp_header  = 0;
                        p_ring_out.rsp_palloc  = 0;

        		p_ring_out.rsp_w    =  m_rsp_fifo.rok();
	        	p_ring_out.rsp_data =  m_rsp_fifo.read();

                }

		break; 

		case PREEMPT:  
			p_ring_out.rsp_grant = p_ring_in.rsp_grant;
                        p_ring_out.rsp_palloc    = 1+(iga ? 1:0);
                        p_ring_out.rsp_preempt   = m_rsp_fifo.rok(); //&& iga; 
                        p_ring_out.rsp_header    = 0;

                        if ( m_rsp_fifo.rok() )
                        {
                                p_ring_out.rsp_w     = 1; //m_rsp_fifo.rok();
		                p_ring_out.rsp_data  = m_rsp_fifo.read();
                        }
                        else
                        {
				// if tgt  local has finished, iga = 0
				// tgt  gate remains the only target, then w = 0
        		        p_ring_out.rsp_w     = p_ring_in.rsp_w && iga; 
	        	        p_ring_out.rsp_data  = p_ring_in.rsp_data;
                        }
		break;
	} // end switch
        p_ring_out.rsp_r       = p_ring_in.rsp_r;

	p_ring_out.cmd_w       = p_ring_in.cmd_w;
	p_ring_out.cmd_data    = p_ring_in.cmd_data;

	p_ring_out.cmd_grant   = p_ring_in.cmd_grant;

	p_ring_out.cmd_palloc  = p_ring_in.cmd_palloc;
        p_ring_out.cmd_preempt = p_ring_in.cmd_preempt;
        p_ring_out.cmd_header  = p_ring_in.cmd_header;

	switch( r_ring_cmd_fsm ) 
	{
		case CMD_IDLE:
		{
                        vci_addr_t rtgtid = (vci_addr_t) ((p_ring_in.cmd_data >> (ring_cmd_data_size-vci_param::N+1)) << 2);
                        uint32_t   rsrcid =  (uint32_t) ((sc_dt::sc_uint<vci_param::S>) (p_ring_in.cmd_data >> 5)); 
                        bool       brdcst = (p_ring_in.cmd_data & 0x1) == 0X1;                   
                        bool       c1     = (p_ring_in.cmd_data >> 19) == 0;
                        bool       c2     = m_lt_src[rsrcid];
                        bool       loc    = !m_lt_addr[rtgtid] && !m_local;
                        bool       eop    = ((int)((p_ring_in.cmd_data >> (ring_cmd_data_size - 1) ) & 0x1) == 1);

                        tga = false;

                        if(p_ring_in.cmd_w && !eop) 
                        {
                                if(brdcst && (c1 || !c2)) 
                                        p_ring_out.cmd_r =  p_ring_in.cmd_r;
                                if(brdcst && !c1 && c2) 
                                {
                                        p_ring_out.cmd_r =  m_cmd_fifo.wok();
                                }
                                if(!brdcst && loc) 
                                {
                                        p_ring_out.cmd_r =  m_cmd_fifo.wok();  

                                } 
        
                                if (!brdcst && !loc) {
                                        p_ring_out.cmd_r =  p_ring_in.cmd_r;
                                }  

                        } // end if rok.!eop
                        else
                                p_ring_out.cmd_r =  p_ring_in.cmd_r;
                }
		break;

		case ALLOC:
                        tga = true;
                        if (!p_ring_in.cmd_preempt)
                                p_ring_out.cmd_r =  m_cmd_fifo.wok(); 	
                        else
        			p_ring_out.cmd_r = p_ring_in.cmd_r;                        
		break;

		case NALLOC:
                        tga = false;
			p_ring_out.cmd_r = p_ring_in.cmd_r;
		break;

	} // end switch

} // end update_ring_signals 
  
};

}} // end namespace

#endif // RING_DSPIN_HALF_GATEWAY_TARGET_FAST_H

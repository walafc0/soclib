
 /* SOCLIB_LGPL_HEADER_BEGIN
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
#ifndef RING_DSPIN_HALF_GATEWAY_INITIATOR_FAST_H
#define RING_DSPIN_HALF_GATEWAY_INITIATOR_FAST_H

#include "caba_base_module.h"
#include "generic_fifo.h"
#include "mapping_table.h"
#include "ring_signals_fast.h"
#include "dspin_interface.h"

// #define  HI_DEBUG
#define HI_STATS

namespace soclib { namespace caba {

namespace {

const char *ring_cmd_fsm_state_str_hi[] = {
        "CMD_IDLE",
        "DEFAULT",
        "SENDING",
        "PREEMPT",
};

#ifdef HI_DEBUG

const char *ring_rsp_fsm_state_str_hi[] = {
        "RSP_IDLE",
        "ALLOC",
        "NALLOC",
};
#endif

}
template<typename vci_param, int ring_cmd_data_size, int ring_rsp_data_size>
class RingDspinHalfGatewayInitiatorFast
{

typedef LocalRingSignals ring_signal_t;
typedef DspinInput<ring_cmd_data_size>   cmd_in_t;
typedef DspinOutput<ring_rsp_data_size>  rsp_out_t;

private:
        
        enum ring_rsp_fsm_state_e {
            	RSP_IDLE,    // waiting for first flit of a response packet
            	ALLOC,      // next flit of a local rsp packet
            	NALLOC,     // next flit of a ring rsp packet
            };
        
        // cmd token allocation fsm
        enum ring_cmd_fsm_state_e {
            	CMD_IDLE,	    
             	DEFAULT,  	
            	SENDING,
                PREEMPT,          	    
            };
        
        // structural parameters
 	std::string         m_name;
        bool                m_alloc_init;

        
        // internal fifos 
        GenericFifo<uint64_t > m_cmd_fifo;     // fifo for the local command packet
        GenericFifo<uint64_t > m_rsp_fifo;     // fifo for the local response packet
        
        // routing table
        soclib::common::AddressDecodingTable<uint32_t, bool> m_lt;
        bool                m_local;

#if defined(HI_DEBUG) or defined(HI_STATS)
        uint32_t            m_cpt;
#endif
#ifdef HI_DEBUG
	uint32_t            m_cyc;
#endif

        // internal registers
        sc_core::sc_signal<int>	    r_ring_cmd_fsm;    // ring command packet FSM (distributed)
        sc_core::sc_signal<int>	    r_ring_rsp_fsm;    // ring response packet FSM

#ifdef HI_STATS
        uint32_t tok_wait;
        uint32_t fifo_full;
        uint32_t flits_sent;
        uint32_t preempt;
        uint32_t wait_rok;
#endif

public :

#define __renRegGateInit(x) x((((std::string) name)+"_" #x).c_str())

RingDspinHalfGatewayInitiatorFast(
	const char     *name,
        bool            alloc_init,
        const int       &wrapper_fifo_depth,
        const soclib::common::MappingTable &mt,
        const soclib::common::IntTab &ringid,
        bool local)
      : m_name(name),
        m_alloc_init(alloc_init),
        m_cmd_fifo(((std::string) name)+"m_cmd_fifo", wrapper_fifo_depth),
        m_rsp_fifo(((std::string) name)+"m_rsp_fifo", wrapper_fifo_depth),
        m_lt(mt.getIdLocalityTable(ringid)),
        m_local(local),
        __renRegGateInit(r_ring_cmd_fsm),
        __renRegGateInit(r_ring_rsp_fsm)
 { } //  end constructor


void reset()
{
	if(m_alloc_init)
		r_ring_cmd_fsm = DEFAULT;
	else
		r_ring_cmd_fsm = CMD_IDLE;

	r_ring_rsp_fsm = RSP_IDLE;
	m_cmd_fifo.init();
	m_rsp_fifo.init();

#if defined(HI_DEBUG) or defined(HI_STATS)
	m_cpt = 0;
#endif
#ifdef HI_DEBUG
	m_cyc = (uint32_t) atoi(getenv("CYCLES"));
#endif

#ifdef HI_STATS
        tok_wait   = 0;
        fifo_full  = 0;
        flits_sent = 0;
        preempt    = 0;
        wait_rok   = 0;
#endif
}

void transition(const cmd_in_t &p_gate_cmd_in, const rsp_out_t &p_gate_rsp_out, const ring_signal_t p_ring_in, cmd_str &init_cmd, bool &init_rsp_val, const bool tga)      
{

	bool      cmd_fifo_get = false;
	bool      cmd_fifo_put = false;
	uint64_t  cmd_fifo_data = 0;

	bool      rsp_fifo_put = false;
	uint64_t  rsp_fifo_data = 0;


//////////// VCI CMD FSM /////////////////////////

	if (p_gate_cmd_in.write.read()) {
		cmd_fifo_data = (uint64_t) p_gate_cmd_in.data.read();
		cmd_fifo_put =  m_cmd_fifo.wok();
	}

	bool rsp_fifo_get = p_gate_rsp_out.read.read();

//////////// RING CMD FSM /////////////////////////
	switch( r_ring_cmd_fsm ) 
	{
		case CMD_IDLE:  
                { 
                 
#ifdef HI_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
                         << " - ring_cmd_fsm  = " << ring_cmd_fsm_state_str_hi[r_ring_cmd_fsm] 
                         << " - fifo ROK : " << m_cmd_fifo.rok()
                         << " - fifo _data : " << std::hex << m_cmd_fifo.read()
                         << " - in grant : " << p_ring_in.cmd_grant
                         << " - in tga : " << tga
                         << " - in wok : " << p_ring_in.cmd_r
                         << " - in data : " << p_ring_in.cmd_data

                         << std::endl;
#endif

#ifdef HI_STATS
        if ( m_cmd_fifo.rok() && tga )
        {
                preempt +=1;
                if(!p_ring_in.cmd_r)
                        fifo_full +=1;
                else
                        flits_sent +=1;
        }
        else if ( m_cmd_fifo.rok() && !p_ring_in.cmd_grant )
                tok_wait +=1;
#endif
                        // tga : target gate allocated
                        if(m_cmd_fifo.rok() && tga)  
                        {
                                cmd_fifo_get = p_ring_in.cmd_r;
                	        r_ring_cmd_fsm = PREEMPT;
                                break;
                        }

                        if(m_cmd_fifo.rok() && p_ring_in.cmd_grant) 
                        {
                                r_ring_cmd_fsm = SENDING;       
                        }
                }
		break;

		case DEFAULT:

#ifdef HI_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
                         << " - ring_cmd_fsm  = " << ring_cmd_fsm_state_str_hi[r_ring_cmd_fsm] 
                         << " - fifo ROK : " << m_cmd_fifo.rok()
                         << " - in grant : " << p_ring_in.cmd_grant
                         << " - in wok : " << p_ring_in.cmd_r
                         << " - fifo data : " << std::hex << m_cmd_fifo.read()
                         << std::endl;
#endif       

#ifdef HI_STATS
        if( m_cmd_fifo.rok() & p_ring_in.cmd_r)
                flits_sent += 1;
        if( m_cmd_fifo.rok() & !p_ring_in.cmd_r)
                fifo_full  += 1;
#endif

			if ( m_cmd_fifo.rok() ) 
			{
				cmd_fifo_get = p_ring_in.cmd_r;  
				r_ring_cmd_fsm = SENDING;             
			}   
			else if ( !p_ring_in.cmd_grant )
				r_ring_cmd_fsm = CMD_IDLE; 
		break;

		case SENDING:   
#ifdef HI_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
                         << " - ring_cmd_fsm  = " << ring_cmd_fsm_state_str_hi[r_ring_cmd_fsm] 
                         << " - fifo ROK : " << m_cmd_fifo.rok()
                         << " - in grant : " << p_ring_in.cmd_grant
                         << " - in wok : " << p_ring_in.cmd_r
                         << " - fifo data : " << std::hex << m_cmd_fifo.read()
                         << std::endl;
#endif                 

#ifdef HI_STATS
if (m_cmd_fifo.rok() && p_ring_in.cmd_r)
     flits_sent +=1;
else if  (m_cmd_fifo.rok() && !p_ring_in.cmd_r) 
         fifo_full  +=1;
#endif


			if(m_cmd_fifo.rok() && p_ring_in.cmd_r ) 
			{
				cmd_fifo_get = true; 
                                bool eop = ((int) (m_cmd_fifo.read() >> (ring_cmd_data_size - 1) ) & 0x1) == 1;
				if (eop) 
				{  
					if ( p_ring_in.cmd_grant )
						r_ring_cmd_fsm = DEFAULT;  
					else   
						r_ring_cmd_fsm = CMD_IDLE; 
				}        
			}      
		break;

		case PREEMPT:   
#ifdef HI_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
                         << " - ring_cmd_fsm  = " << ring_cmd_fsm_state_str_hi[r_ring_cmd_fsm] 
                         << " - fifo ROK : " << m_cmd_fifo.rok()
                         << " - in grant : " << p_ring_in.cmd_grant
                         << " - in tga : " << tga
                         << " - in wok : " << p_ring_in.cmd_r
                         << " - fifo data : " << std::hex << m_cmd_fifo.read()
                         << std::endl;
#endif                 

#ifdef HI_STATS
if (m_cmd_fifo.rok())
{
        preempt +=1; 
        if(p_ring_in.cmd_r)
                flits_sent +=1;
        else  
                fifo_full  +=1;
}
else
        wait_rok +=1;
#endif

			if(m_cmd_fifo.rok() && p_ring_in.cmd_r ) 
			{
				cmd_fifo_get = true; 
                                bool eop = ((int) (m_cmd_fifo.read() >> (ring_cmd_data_size - 1) ) & 0x1) == 1;
				if (eop) 
				{ 
                                        // One Init Local has finished, it's in WAIT_PALLOC_END and has the token
					r_ring_cmd_fsm = CMD_IDLE;  
				}        
			}      
		break;

	} // end switch ring cmd fsm
 
/////////// RING RSP FSM ////////////////////////
    
	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE:  

		{
			uint32_t rsrcid  = (uint32_t)  ( (sc_dt::sc_uint<vci_param::S>) (p_ring_in.rsp_data >> ring_rsp_data_size-vci_param::S-1));
			bool islocal     = (m_lt[rsrcid] && m_local) || (!m_lt[rsrcid] && !m_local);
			bool reop        = ((p_ring_in.rsp_data >> (ring_rsp_data_size - 1)) & 0x1) == 1;
#ifdef HI_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
              << " - ring_rsp_fsm = " << ring_rsp_fsm_state_str_hi[r_ring_rsp_fsm]
              << " - in preempt : " << p_ring_in.rsp_preempt
              << " - in rok : " << p_ring_in.rsp_w
              << " - in data : " << std::hex << p_ring_in.rsp_data
              << " - rsrcid : " << rsrcid
              << " - isloc : " << islocal 
              << " - in wok : " << p_ring_in.rsp_r
              << " - fifo wok : " << m_rsp_fifo.wok()
              << " - reop : " << reop
              << std::endl;
#endif


			if (p_ring_in.rsp_w)
                        {
                                if(islocal) 
			        {   
				        rsp_fifo_put  = m_rsp_fifo.wok();
				        rsp_fifo_data = p_ring_in.rsp_data;

                                        if (reop && m_rsp_fifo.wok())
                                                r_ring_rsp_fsm = RSP_IDLE;
                                        else
                                                r_ring_rsp_fsm = ALLOC;
			        }

			        else  // !islocal 
			        {

                                        if (reop && p_ring_in.rsp_r)
                                                r_ring_rsp_fsm = RSP_IDLE;
                                        else
                                                r_ring_rsp_fsm = NALLOC;
			        }
                        }
			else // !p_ring_in.rsp_w 
		        	r_ring_rsp_fsm = RSP_IDLE;
		}
		break;

		case ALLOC:
		{
			bool reop     = ((p_ring_in.rsp_data >> (ring_rsp_data_size - 1)) & 0x1) == 1;
#ifdef HI_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name 
              << " - ring_rsp_fsm = " << ring_rsp_fsm_state_str_hi[r_ring_rsp_fsm] 
              << " - in preempt : " << p_ring_in.rsp_preempt
              << " - in rok : " << p_ring_in.rsp_w
              << " - in wok : " << p_ring_in.rsp_r 
              << " - in data : " << std::hex << p_ring_in.rsp_data
              << " - fifo wok : " << m_rsp_fifo.wok()	
	      << " - reop : " << reop
              << std::endl;
#endif


                        if(p_ring_in.rsp_preempt) break;

			if (p_ring_in.rsp_w && m_rsp_fifo.wok() && reop)         
			{

				rsp_fifo_put  = true;
				rsp_fifo_data = p_ring_in.rsp_data;

				if(p_ring_in.rsp_palloc)
			     		r_ring_rsp_fsm = NALLOC;
				else
			     		r_ring_rsp_fsm = RSP_IDLE;             
			}
			else //  !p_ring_in.rsp_w || !m_rsp_fifo.wok() || !reop
			{

				rsp_fifo_put  = p_ring_in.rsp_w && m_rsp_fifo.wok();
				rsp_fifo_data = p_ring_in.rsp_data;
				r_ring_rsp_fsm = ALLOC;             
			}
		} 
		break;

		case NALLOC:     
		{

#ifdef HI_DEBUG
if(m_cpt > m_cyc)
   std::cout << std::dec << sc_time_stamp() << " - " << m_name
                         << " - ring_rsp_fsm  = " << ring_rsp_fsm_state_str_hi[r_ring_rsp_fsm] 
                         << " - in rok : " << p_ring_in.rsp_w
                         << " - fifo wok : " <<  m_rsp_fifo.wok()   
                         << " - in wok : " << p_ring_in.rsp_r 
                         << " - in data : " << std::hex << p_ring_in.rsp_data
                         << std::endl;
#endif

			bool reop  = ((p_ring_in.rsp_data >> (ring_rsp_data_size - 1)) & 0x1) == 1;


			if (p_ring_in.rsp_w && reop && p_ring_in.rsp_r)
			{
				r_ring_rsp_fsm = RSP_IDLE; 
			}
			else
			{
				r_ring_rsp_fsm = NALLOC;
			}
		}
		break;

	} // end switch rsp fsm

#if defined(HI_DEBUG) or defined(HI_STATS)
	m_cpt+=1;
#endif

    ////////////////////////
    //  fifos update      //
   ////////////////////////

//-- keep trace on ring traffic
        init_cmd.cmdval  = cmd_fifo_get;
        init_cmd.flit    = m_cmd_fifo.read();
        init_cmd.state   = ring_cmd_fsm_state_str_hi[r_ring_cmd_fsm];

	//init_cmd_val = cmd_fifo_get;
	init_rsp_val = rsp_fifo_put;
// local cmd fifo update
	if (  cmd_fifo_put &&  cmd_fifo_get ) m_cmd_fifo.put_and_get(cmd_fifo_data);
	else if (  cmd_fifo_put && !cmd_fifo_get ) m_cmd_fifo.simple_put(cmd_fifo_data);
	else if ( !cmd_fifo_put &&  cmd_fifo_get ) m_cmd_fifo.simple_get();
	
// local rsp fifo update
	if (  rsp_fifo_put &&  rsp_fifo_get ) m_rsp_fifo.put_and_get(rsp_fifo_data);
	else if (  rsp_fifo_put && !rsp_fifo_get ) m_rsp_fifo.simple_put(rsp_fifo_data);
	else if ( !rsp_fifo_put &&  rsp_fifo_get ) m_rsp_fifo.simple_get();
     
}  // end Transition()

///////////////////////////////////////////////////////////////////
void genMoore(cmd_in_t &p_gate_cmd_in, rsp_out_t &p_gate_rsp_out)
///////////////////////////////////////////////////////////////////
{
	p_gate_rsp_out.write = m_rsp_fifo.rok();
	p_gate_rsp_out.data  = (sc_dt::sc_uint<ring_rsp_data_size>) m_rsp_fifo.read();

	p_gate_cmd_in.read= m_cmd_fifo.wok();

} // end genMoore

///////////////////////////////////////////////////////////////////
void update_ring_signals(ring_signal_t p_ring_in, ring_signal_t &p_ring_out, bool tga, bool &iga)
///////////////////////////////////////////////////////////////////
{    
	switch( r_ring_cmd_fsm ) 
	{
		case CMD_IDLE:
                {

			p_ring_out.cmd_grant = !m_cmd_fifo.rok() && p_ring_in.cmd_grant;

                        p_ring_out.cmd_preempt   = m_cmd_fifo.rok() && tga; 
                        p_ring_out.cmd_palloc    = m_cmd_fifo.rok() && tga; 
                        p_ring_out.cmd_header    = m_cmd_fifo.rok();
 
                        if (m_cmd_fifo.rok() && tga) 
                        {
                                p_ring_out.cmd_w     = m_cmd_fifo.rok();
		                p_ring_out.cmd_data  = m_cmd_fifo.read();
                        }
                        else
                        {
        		        p_ring_out.cmd_w     = p_ring_in.cmd_w;
	        	        p_ring_out.cmd_data  = p_ring_in.cmd_data;
                        }

                }
		break;
	
		case DEFAULT:        
			p_ring_out.cmd_grant = !m_cmd_fifo.rok();  

                        p_ring_out.cmd_preempt = 0;
                        p_ring_out.cmd_header  = 0;
                        p_ring_out.cmd_palloc  = 0;
	        	p_ring_out.cmd_w       = m_cmd_fifo.rok();
		        p_ring_out.cmd_data    = m_cmd_fifo.read();
		break;
	
		case SENDING: 
                { 
			bool eop = ((int) (m_cmd_fifo.read() >> (ring_cmd_data_size - 1) ) & 0x1) == 1;
			p_ring_out.cmd_grant = m_cmd_fifo.rok() && p_ring_in.cmd_r && eop;

                        // si on est dans cet état, c'est qu'on n'utilise pas le cmd_preempt mais le gnt.
                        p_ring_out.cmd_preempt= 0;
                        p_ring_out.cmd_header = 0;
                        p_ring_out.cmd_palloc = 0;
        		p_ring_out.cmd_w      = m_cmd_fifo.rok();
	        	p_ring_out.cmd_data   = m_cmd_fifo.read();
                }
                break;
	
		case PREEMPT:
                { 

			p_ring_out.cmd_grant   = p_ring_in.cmd_grant;

                        // palloc may have 2 values :
                        // palloc = 2 : means Target Gate still allocated
                        // global state for targets :  TG  TL0  TL1   TL2  ... TLn
                        //                              A   N    N     P2       N
                        //                              A   N    N     N        N
                        // palloc = 1 : means Target Gate free (not allocated)
                        // global state for targets :  TG  TL0  TL1   TL2  ... TLn
                        //                              A   N    N     P2       N
                        //                              N   N    N     P1       N                       
                        //                              I   I    I     I        I                                                                
                        // TLi not allocated, in case of preempt and last flit, needs to test value of palloc
                        // if palloc=1      => next state : IDLE (TG not allocated)
                        // else (palloc2=2) => next state : NALLOC (TG still Allocated) 
                        p_ring_out.cmd_palloc  = 1+(tga ? 1:0);

                        p_ring_out.cmd_preempt = m_cmd_fifo.rok(); //&& tga; 
                        p_ring_out.cmd_header  = 0;

                        if ( m_cmd_fifo.rok() )
                        {
                                p_ring_out.cmd_w     = 1; //m_cmd_fifo.rok();
		                p_ring_out.cmd_data  = m_cmd_fifo.read();
                        }
                        else
                        {
				// if init local has finished, tga = 0
				// init gate remains the only initiator, then w = 0
         		        p_ring_out.cmd_w     = p_ring_in.cmd_w && tga; 
	        	        p_ring_out.cmd_data  = p_ring_in.cmd_data;
                        }
                }
		break;

	} // end switch

	p_ring_out.cmd_r       = p_ring_in.cmd_r;

	p_ring_out.rsp_w       = p_ring_in.rsp_w;
	p_ring_out.rsp_data    = p_ring_in.rsp_data;

	p_ring_out.rsp_grant   = p_ring_in.rsp_grant;

	p_ring_out.rsp_palloc  = p_ring_in.rsp_palloc;
        p_ring_out.rsp_preempt = p_ring_in.rsp_preempt;
        p_ring_out.rsp_header  = p_ring_in.rsp_header;


	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE:	
		{
			uint32_t rsrcid  = (uint32_t)  ( (sc_dt::sc_uint<vci_param::S>) (p_ring_in.rsp_data >> ring_rsp_data_size-vci_param::S-1));
			bool islocal     = (m_lt[rsrcid] && m_local) || (!m_lt[rsrcid] && !m_local);

                        iga = false;

			if(p_ring_in.rsp_w && islocal) 
				p_ring_out.rsp_r = m_rsp_fifo.wok();
                        else
				p_ring_out.rsp_r = p_ring_in.rsp_r;
		}
		break;
	
		case ALLOC:
                        iga = true;

			if (!p_ring_in.rsp_preempt)
                                p_ring_out.rsp_r =  m_rsp_fifo.wok(); 	
                        else
        			p_ring_out.rsp_r = p_ring_in.rsp_r;
		break;
	
		case NALLOC:
                        iga = false;
			p_ring_out.rsp_r = p_ring_in.rsp_r;
		break;    
	} // end switch


} // end update_ring_signals

#ifdef HI_STATS
void print_stats() {

std::cout << m_name << " , " << m_cpt << " , " << flits_sent << " , " << tok_wait << " , " << fifo_full << " , " << preempt << " , " << wait_rok << std::endl;
 
}
#endif
};

}} // end namespace

#endif // RING_DSPIN_HALF_GATEWAY_INITIATOR_FAST_H


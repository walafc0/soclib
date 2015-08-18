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
 * Date     : March 2010
 *
 * Copyright: UPMC - LIP6
 */
#ifndef SOCLIB_CABA_VCI_SIMPLE_RING_TARGET_FAST_H
#define SOCLIB_CABA_VCI_SIMPLE_RING_TARGET_FAST_H

#include "vci_initiator.h"
#include "generic_fifo.h"
#include "mapping_table.h"
#include "ring_signals_fast.h"

//#define T_DEBUG

namespace soclib { namespace caba {

namespace {

const char *ring_rsp_fsm_state_str_st[] = {
                "RSP_IDLE",
                "DEFAULT",
                "KEEP",
        };

#ifdef T_DEBUG

const char *vci_cmd_fsm_state_str_st[] = {
        "CMD_FIRST_HEADER",
        "CMD_SECOND_HEADER",
        "WDATA",
};
const char *vci_rsp_fsm_state_str_st[] = {
        "RSP_HEADER",
        "RSP_SINGLE_DATA",
        "RSP_MULTI_DATA",
};

const char *ring_cmd_fsm_state_str_st[] = {
        "CMD_IDLE",
        "LOCAL",
        "RING",
};
#endif
} // end namespace

template<typename vci_param, int ring_cmd_data_size, int ring_rsp_data_size>
class VciSimpleRingTargetFast
{

typedef typename vci_param::fast_addr_t vci_addr_t;
typedef SimpleRingSignals ring_signal_t; 
typedef VciInitiator<vci_param> vci_initiator_t;

private:
        enum vci_cmd_fsm_state_e {
        	CMD_FIRST_HEADER,     // first flit for a ring cmd packet (read or write)
                CMD_SECOND_HEADER,   //  second flit for a ring cmd packet
        	WDATA,               //  data flit for a ring cmd write packet
            };
        
        enum vci_rsp_fsm_state_e {
        	RSP_HEADER,     
                RSP_SINGLE_DATA, 
                RSP_MULTI_DATA,      
            };
        
        enum ring_cmd_fsm_state_e {
        	CMD_IDLE,	 // waiting for first flit of a command packet
        	LOCAL,  	// next flit of a local cmd packet
        	RING,  	       // next flit of a ring cmd packet
        };
        
        // cmd token allocation fsm
        enum ring_rsp_fsm_state_e {
        	RSP_IDLE,	    
        	DEFAULT,  	
        	KEEP,  	            
        };
        
        // structural parameters
	std::string   m_name;
        bool          m_alloc_target;

        // internal fifos 
        GenericFifo<uint64_t > m_cmd_fifo;     // fifo for the local command paquet
        GenericFifo<uint64_t > m_rsp_fifo;     // fifo for the local response paquet

        // routing table 
        soclib::common::AddressDecodingTable<vci_addr_t, int> m_rt;
        // locality table
        soclib::common::AddressDecodingTable<vci_addr_t, bool> m_lt;

        int           m_tgtid;
        uint32_t      m_shift;
 
        // internal registers
        sc_core::sc_signal<int>	        r_ring_cmd_fsm;	    // ring command packet FSM 
        sc_core::sc_signal<int>		r_ring_rsp_fsm;	    // ring response packet FSM
        sc_core::sc_signal<int>		r_vci_cmd_fsm;	    // vci command packet FSM
        sc_core::sc_signal<int>		r_vci_rsp_fsm;	    // vci response packet FSM
        
        sc_core::sc_signal<sc_dt::sc_uint<vci_param::S> >      r_srcid;
        sc_core::sc_signal<sc_dt::sc_uint<2> >                 r_cmd;
        sc_core::sc_signal<sc_dt::sc_uint<vci_param::T> >      r_trdid;
        sc_core::sc_signal<sc_dt::sc_uint<vci_param::P> >      r_pktid;
        sc_core::sc_signal<sc_dt::sc_uint<vci_param::K> >      r_plen;
        sc_core::sc_signal<sc_dt::sc_uint<1> >                 r_contig;
        sc_core::sc_signal<sc_dt::sc_uint<1> >                 r_const;
        sc_core::sc_signal<sc_dt::sc_uint<vci_param::N> >      r_addr;
            
public :

#define __renRegTgtS(x) x((((std::string) name)+"_" #x).c_str())

VciSimpleRingTargetFast(
	const char     *name,
        bool            alloc_target,
        const int       &wrapper_fifo_depth,
        const soclib::common::MappingTable &mt,
        const soclib::common::IntTab &ringid,
        const int &tgtid)
     :  m_name(name),
        m_alloc_target(alloc_target),
        m_cmd_fifo(((std::string) name)+"m_cmd_fifo", wrapper_fifo_depth),
        m_rsp_fifo(((std::string) name)+"m_rsp_fifo", wrapper_fifo_depth),
        m_rt(mt.getRoutingTable<typename vci_param::fast_addr_t>(ringid)),
        m_lt(mt.getLocalityTable<typename vci_param::fast_addr_t>(ringid)),
        m_tgtid(tgtid),
        m_shift(ring_cmd_data_size-vci_param::N+1),
        __renRegTgtS(r_ring_cmd_fsm),
        __renRegTgtS(r_ring_rsp_fsm),
        __renRegTgtS(r_vci_cmd_fsm),
        __renRegTgtS(r_vci_rsp_fsm),
        __renRegTgtS(r_srcid),
        __renRegTgtS(r_cmd),
        __renRegTgtS(r_trdid),
        __renRegTgtS(r_pktid),
        __renRegTgtS(r_plen),
        __renRegTgtS(r_contig),
        __renRegTgtS(r_const),
        __renRegTgtS(r_addr)

{} //  end constructor

void reset()
{
        if(m_alloc_target)
        	r_ring_rsp_fsm = DEFAULT;
        else
        	r_ring_rsp_fsm = RSP_IDLE;
        
        r_vci_cmd_fsm = CMD_FIRST_HEADER;
        r_vci_rsp_fsm = RSP_HEADER;
        r_ring_cmd_fsm = CMD_IDLE;
        m_cmd_fifo.init();
        m_rsp_fifo.init();    
}
void transition(const vci_initiator_t &p_vci, const ring_signal_t p_ring_in, bool &tgt_cmd_val, rsp_str &tgt_rsp)       
{

	bool      cmd_fifo_get = false;
	bool      cmd_fifo_put = false;
	uint64_t  cmd_fifo_data = 0;
	
	bool      rsp_fifo_get = false;
	bool      rsp_fifo_put = false;
	uint64_t  rsp_fifo_data = 0;


//////////// VCI CMD FSM /////////////////////////
	switch ( r_vci_cmd_fsm ) 
	{

		case CMD_FIRST_HEADER:
                        if (m_cmd_fifo.rok() == true)
                        {
#ifdef T_DEBUG
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_vci_cmd_fsm = " << vci_cmd_fsm_state_str_st[r_vci_cmd_fsm]
                          << " -- fifo_rok : " << m_cmd_fifo.rok()
                          << " -- fifo_data : " << std::hex << m_cmd_fifo.read()
                          << " -- vci cmdack : " << p_vci.cmdack.read()
                          << std::endl;
#endif

                                cmd_fifo_get = true; 
				
                                if (m_cmd_fifo.read() & 0x1 == 0x1) // broadcast
                                {
                                        r_addr   = (sc_dt::sc_uint<vci_param::N>) 0x3;     
                                        r_srcid  = (sc_dt::sc_uint<vci_param::S>) (m_cmd_fifo.read() >> 5); 
 					r_cmd    = (sc_dt::sc_uint<2>)  0x2; 
 					r_contig = (sc_dt::sc_uint<1>)  0x1; 
 					r_const  = (sc_dt::sc_uint<1>)  0x0; 
 					r_plen   = (sc_dt::sc_uint<vci_param::K>) 0x04; 
 					r_pktid  = (sc_dt::sc_uint<vci_param::P>) 0x0; 
 					r_trdid  = (sc_dt::sc_uint<vci_param::T>) (m_cmd_fifo.read() >> 1); 

                                        r_vci_cmd_fsm = WDATA; 
                                }
                                else
                                {
                                        r_addr = (sc_dt::sc_uint<vci_param::N>) ((m_cmd_fifo.read() >> m_shift )<< 2);
                                        r_vci_cmd_fsm = CMD_SECOND_HEADER; 
                                }

         
			}  // end if rok
		break;

		case CMD_SECOND_HEADER:        
			if ( m_cmd_fifo.rok() ) 
			{
#ifdef T_DEBUG
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_vci_cmd_fsm = " << vci_cmd_fsm_state_str_st[r_vci_cmd_fsm]
                          << " -- fifo_rok : " << m_cmd_fifo.rok()
                          << " -- fifo_data : " << std::hex << m_cmd_fifo.read()
                          << " -- r_addr : " << r_addr
                          << " -- vci cmdack : " << p_vci.cmdack.read()
                          << std::endl;
#endif

				if(((int) (m_cmd_fifo.read() >> (ring_cmd_data_size - 1) ) & 0x1) == 1)  // read command
				{

					if (p_vci.cmdack.read())
					{

						cmd_fifo_get = true;
						r_vci_cmd_fsm = CMD_FIRST_HEADER;
					} 
				}
				else  // write command
				{

 					cmd_fifo_get =  true;
 					r_srcid  = (sc_dt::sc_uint<vci_param::S>)  (m_cmd_fifo.read() >> (ring_cmd_data_size-vci_param::S-1)) ; 
 					r_cmd    = (sc_dt::sc_uint<2>)  ((m_cmd_fifo.read() >> 23) & 0x3); 
 					r_contig = (sc_dt::sc_uint<1>)  ((m_cmd_fifo.read() >> 22) & 0x1); 
 					r_const =  (sc_dt::sc_uint<1>)  ((m_cmd_fifo.read() >> 21) & 0x1); 
 					r_plen  =  (sc_dt::sc_uint<vci_param::K>) ((m_cmd_fifo.read() >> 13) & 0xFF); 
 					r_pktid  = (sc_dt::sc_uint<vci_param::P>) ((m_cmd_fifo.read() >> 9) & 0xF); 
 					r_trdid  = (sc_dt::sc_uint<vci_param::T>) ((m_cmd_fifo.read() >> 5) & 0xF); 
 					r_vci_cmd_fsm = WDATA;
				}                                          
			} 
		break;

		case WDATA:
#ifdef T_DEBUG
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_vci_cmd_fsm = " << vci_cmd_fsm_state_str_st[r_vci_cmd_fsm]
                          << " -- fifo_rok : " << m_cmd_fifo.rok()
                          << " -- fifo_data : " << std::hex << m_cmd_fifo.read()
                          << " -- r_addr : " << r_addr
                          << " -- vci cmdack : " << p_vci.cmdack.read()
                          << std::endl;
#endif
			if ( p_vci.cmdack.read() && m_cmd_fifo.rok() ) 
			{

				cmd_fifo_get = true; 
				sc_dt::sc_uint<1> contig = r_contig;
				if(contig == 0x1)    
					r_addr = r_addr.read() + vci_param::B ;                        
				if(( (m_cmd_fifo.read() >> (ring_cmd_data_size - 1) ) & 0x1) == 1)
					r_vci_cmd_fsm = CMD_FIRST_HEADER;   
				else 
					r_vci_cmd_fsm = WDATA;                                   
			} // end if cmdack
		break;
        
	} // end switch r_vci_cmd_fsm

/////////// VCI RSP FSM /////////////////////////
	switch ( r_vci_rsp_fsm ) 
	{
		case RSP_HEADER:
                {
/* #ifdef T_DEBUG
if(p_vci.rspval.read())
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_vci_rsp_fsm = " << vci_rsp_fsm_state_str_st[r_vci_rsp_fsm]
                          << " -- vci_rspval : " << p_vci.rspval.read()
                          << " -- fifo_rsp_wok : " << m_rsp_fifo.wok() 
                          << " -- rsrcid : " << std::hex << p_vci.rsrcid.read()
                          << " -- rdata : " << p_vci.rdata.read()
                          << " -- reop : " << p_vci.reop.read()
                          << " -- rerror : " << p_vci.rerror.read()
                          << std::endl;
#endif */
			if(p_vci.rspval.read() && m_rsp_fifo.wok())
			{
                                rsp_fifo_put = true;

				rsp_fifo_data = (((uint64_t) p_vci.rsrcid.read()) << (ring_rsp_data_size-vci_param::S-1)) |
                                                (((uint64_t) p_vci.rerror.read() & 0x1) << 16) | 
                                                (((uint64_t) p_vci.rpktid.read() & 0xF) << 12) | 
                                                (((uint64_t) p_vci.rtrdid.read() & 0xF) << 8);
                               
                                // one flit for write response                                 
                                if((p_vci.rdata.read() == 0) && p_vci.reop.read()) 
                                {
                                        
                                        rsp_fifo_data = rsp_fifo_data |  (((uint64_t) 0x1) << (ring_rsp_data_size-1)) ;
				        r_vci_rsp_fsm = RSP_SINGLE_DATA;
                                }
                                else
        				r_vci_rsp_fsm = RSP_MULTI_DATA;                      
			}
                }
		break;

		case RSP_SINGLE_DATA: // to avoid Mealy dependency (testing vci_rdata == 0 && vci_reop == 1 in genMoore/RSP_HEADER)
/* #ifdef T_DEBUG
if(p_vci.rspval.read())
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_vci_rsp_fsm = " << vci_rsp_fsm_state_str_st[r_vci_rsp_fsm]
                          << " -- vci_rspval : " << p_vci.rspval.read()
                          << " -- fifo_rsp_wok : " << m_rsp_fifo.wok() 
                          << " -- rsrcid : " << std::hex << p_vci.rsrcid.read()
                          << " -- rdata : " << p_vci.rdata.read()
                          << " -- reop : " << p_vci.reop.read()
                          << " -- rerror : " << p_vci.rerror.read()
                          << std::endl;
#endif   */         
			r_vci_rsp_fsm = RSP_HEADER;
		break;

		case RSP_MULTI_DATA:
/* #ifdef T_DEBUG
if(p_vci.rspval.read())
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_vci_rsp_fsm = " << vci_rsp_fsm_state_str_st[r_vci_rsp_fsm]
                          << " -- vci_rspval : " << p_vci.rspval.read()
                          << " -- fifo_rsp_wok : " << m_rsp_fifo.wok() 
                          << " -- rsrcid : " << std::hex << p_vci.rsrcid.read()
                          << " -- rdata : " << p_vci.rdata.read()
                          << " -- reop : " << p_vci.reop.read()
                          << " -- rerror : " << p_vci.rerror.read()
                          << std::endl;
#endif     */       
			if(p_vci.rspval.read() && m_rsp_fifo.wok())
			{


				rsp_fifo_put = true;
				rsp_fifo_data = (uint64_t) p_vci.rdata.read();  
         
				if (p_vci.reop.read()) 
				{ 
					rsp_fifo_data = rsp_fifo_data |  (((uint64_t) 0x1) << (ring_rsp_data_size-1)) ;
					r_vci_rsp_fsm = RSP_HEADER;
				}           		    
			}  
		break;

	} // end switch r_vci_rsp_fsm
   
//////////// RING RSP FSM  /////////////////////////
        
	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE:   
/* #ifdef T_DEBUG 
if(m_rsp_fifo.rok())
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_ring_rsp_fsm = " << ring_rsp_fsm_state_str_st[r_ring_rsp_fsm]
                          << " -- in rsp grant : " << p_ring_in.rsp_grant
                          << " -- in wok : " <<  p_ring_in.rsp_r
                          << " -- fifo rok : " <<  m_rsp_fifo.rok()
                          << " -- fifo data  : " <<  std::hex << m_rsp_fifo.read()
                          << std::endl;  
#endif   */
			if ( p_ring_in.rsp_grant && m_rsp_fifo.rok() ) 

				r_ring_rsp_fsm = KEEP;           
                
		break;

		case DEFAULT: 
                { 
/* #ifdef T_DEBUG 
if(m_rsp_fifo.rok())
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_ring_rsp_fsm = " << ring_rsp_fsm_state_str_st[r_ring_rsp_fsm]
                          << " -- in rsp grant : " << p_ring_in.rsp_grant
                          << " -- in wok : " <<  p_ring_in.rsp_r
                          << " -- fifo rok : " <<  m_rsp_fifo.rok()
                          << " -- fifo data  : " <<  std::hex << m_rsp_fifo.read()
                          << std::endl;  
#endif */
                        bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);

                        if ( m_rsp_fifo.rok() && eop && p_ring_in.rsp_r )  
			{
				rsp_fifo_get = true;
                                if ( p_ring_in.rsp_grant )
				        r_ring_rsp_fsm = DEFAULT;
                                else
				        r_ring_rsp_fsm = RSP_IDLE;
			}  
 
                        if ( m_rsp_fifo.rok() && (!eop || !p_ring_in.rsp_r)) 
			{
				rsp_fifo_get = p_ring_in.rsp_r;
				r_ring_rsp_fsm = KEEP;
			}

                        if ( !m_rsp_fifo.rok() && !p_ring_in.rsp_grant )
				r_ring_rsp_fsm = RSP_IDLE; 

                        if ( !m_rsp_fifo.rok() && p_ring_in.rsp_grant )
				r_ring_rsp_fsm = DEFAULT;
                }
		break;

		case KEEP:   
/* #ifdef T_DEBUG 
if(m_rsp_fifo.rok())
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_ring_rsp_fsm = " << ring_rsp_fsm_state_str_st[r_ring_rsp_fsm]
                          << " -- in rsp grant : " << p_ring_in.rsp_grant
                          << " -- in wok : " <<  p_ring_in.rsp_r
                          << " -- fifo rok : " <<  m_rsp_fifo.rok()
                          << " -- fifo data  : " <<  std::hex << m_rsp_fifo.read()
                          << std::endl;  
#endif     */        
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

	} // end switch ring cmd fsm

/////////// RING CMD FSM ////////////////////////
	switch( r_ring_cmd_fsm ) 
	{

		case CMD_IDLE:  
		{ // for variable scope
#ifdef T_DEBUG
if(p_ring_in.cmd_w)
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_ring_cmd_fsm = " << ring_cmd_fsm_state_str_st[r_ring_cmd_fsm]
                          << " - in gnt : " << p_ring_in.cmd_grant
                          << " - in rok : " << p_ring_in.cmd_w
                          << " - in data : " << std::hex << p_ring_in.cmd_data
                          << " - in wok : " << p_ring_in.cmd_r
                          << " - fifo wok : " << m_cmd_fifo.wok()
                          << std::endl;
#endif
			vci_addr_t rtgtid = (vci_addr_t) ((p_ring_in.cmd_data >> m_shift) << 2);
			bool islocal = m_lt[rtgtid]  && (m_rt[rtgtid] == m_tgtid);
                        bool eop     = ( (int) ((p_ring_in.cmd_data >> (ring_cmd_data_size - 1) ) & 0x1) == 1); 

			if(p_ring_in.cmd_w && !eop)
			{
				if (islocal)
				{
					r_ring_cmd_fsm = LOCAL; 
                              		cmd_fifo_put   = m_cmd_fifo.wok();
                              		cmd_fifo_data  = p_ring_in.cmd_data;
				}
				else
					r_ring_cmd_fsm = RING;
			}
			else
				r_ring_cmd_fsm = CMD_IDLE;
                }

		break;

		case LOCAL:   
                {
#ifdef T_DEBUG
if(p_ring_in.cmd_w)
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_ring_cmd_fsm = " << ring_cmd_fsm_state_str_st[r_ring_cmd_fsm]
                          << " - in rok : " << p_ring_in.cmd_w
                          << " - in gnt : " << p_ring_in.cmd_grant
                          << " - in data : " << std::hex << p_ring_in.cmd_data
                          << " - in wok : " << p_ring_in.cmd_r
                          << " - fifo wok : " << m_cmd_fifo.wok()
                          << std::endl;
#endif
                        bool eop = ( (int) ((p_ring_in.cmd_data >> (ring_cmd_data_size - 1) ) & 0x1) == 1);


                 	if ( p_ring_in.cmd_w && m_cmd_fifo.wok() && eop )
                        { 

				cmd_fifo_put  = true;
				cmd_fifo_data = p_ring_in.cmd_data;
			     	r_ring_cmd_fsm = CMD_IDLE;
			     		
                        }
                        
                 	if ( !p_ring_in.cmd_w || !m_cmd_fifo.wok() || !eop )
                        { 

				cmd_fifo_put  = p_ring_in.cmd_w && m_cmd_fifo.wok();
				cmd_fifo_data = p_ring_in.cmd_data;
			     	r_ring_cmd_fsm = LOCAL;
			     		
                        }                        
                } 
		break;

		case RING:   
                { 
 #ifdef T_DEBUG
if(p_ring_in.cmd_w)
    std::cout << std::dec << sc_time_stamp() << " - " << m_name
                          << " - r_ring_cmd_fsm = " << ring_cmd_fsm_state_str_st[r_ring_cmd_fsm]
                          << " - in rok : " << p_ring_in.cmd_w
                          << " - in gnt : " << p_ring_in.cmd_grant
                          << " - in data : " << std::hex << p_ring_in.cmd_data
                          << " - in wok : " << p_ring_in.cmd_r
                          << " - fifo wok : " << m_cmd_fifo.wok()
                          << std::endl;
#endif 
			bool eop = ( (int) ((p_ring_in.cmd_data >> (ring_cmd_data_size - 1) ) & 0x1) == 1);

			if ( p_ring_in.cmd_w && eop  && p_ring_in.cmd_r) {        
        			r_ring_cmd_fsm = CMD_IDLE;
                        }
                        else {
 				r_ring_cmd_fsm = RING;
                        }
                }
		break;

	} // end switch cmd fsm 

    ////////////////////////
    //  fifos update      //
   ////////////////////////
//-- to keep trace on ring traffic : a valid command is received by the target
        tgt_rsp.rspval  = rsp_fifo_get;
        tgt_rsp.flit    = m_rsp_fifo.read();
        tgt_rsp.state   = ring_rsp_fsm_state_str_st[r_ring_rsp_fsm];

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
void genMoore(vci_initiator_t &p_vci)
///////////////////////////////////////////////////////////////////
{
	if(r_vci_rsp_fsm == RSP_HEADER)
        { 
	        p_vci.rspack = false;
        }
	else if (r_vci_rsp_fsm == RSP_SINGLE_DATA)
             {   
	        p_vci.rspack = true;
             }
             else // multi_data
             {   
	        p_vci.rspack = m_rsp_fifo.wok();
             }

	switch ( r_vci_cmd_fsm ) 
	{
		case CMD_FIRST_HEADER:                        			  
		        p_vci.cmdval = false;

		break;

		case CMD_SECOND_HEADER:         
			if(((int) (m_cmd_fifo.read() >> (ring_cmd_data_size - 1) ) & 0x1) == 1) // eop
			{
				p_vci.cmdval  = m_cmd_fifo.rok(); 
				p_vci.address = (sc_dt::sc_uint<vci_param::N>) r_addr.read();
				p_vci.cmd     = (sc_dt::sc_uint<2>)  ((m_cmd_fifo.read() >> 23) & 0x3);
                                p_vci.be      = (sc_dt::sc_uint<4>)  ((m_cmd_fifo.read() >> 1) & 0xF);
				p_vci.wdata   = 0;
				p_vci.pktid   = (sc_dt::sc_uint<vci_param::P>) ((m_cmd_fifo.read() >> 9) & 0xF);
				p_vci.srcid   = (sc_dt::sc_uint<vci_param::S>)  (m_cmd_fifo.read() >> (ring_cmd_data_size-vci_param::S-1));
				p_vci.trdid   = (sc_dt::sc_uint<vci_param::T>) ((m_cmd_fifo.read() >> 5) & 0xF);
				p_vci.plen    =  (sc_dt::sc_uint<vci_param::K>)  ((m_cmd_fifo.read() >> 13) & 0xFF);
				p_vci.eop     = true;         
				sc_dt::sc_uint<1> cons = (sc_dt::sc_uint<1>)  ((m_cmd_fifo.read() >> 21) & 0x1) ; 
				if (cons == 0x1)
					p_vci.cons = true;
				else
					p_vci.cons = false;        
				sc_dt::sc_uint<1> contig = (sc_dt::sc_uint<1>)  ((m_cmd_fifo.read() >> 22) & 0x1);
				if(contig == 0x1) 
					p_vci.contig = true;
				else
					p_vci.contig = false;          	    
			} 
			else 
				p_vci.cmdval = false;         
		break;
    
		case WDATA:
		{   // for variable scope

			p_vci.cmdval = m_cmd_fifo.rok();
			p_vci.address = (sc_dt::sc_uint<vci_param::N>) r_addr.read();
			p_vci.be = (sc_dt::sc_uint<vci_param::B>)((m_cmd_fifo.read()  >> 32) & 0xF);
			p_vci.cmd = r_cmd;
			p_vci.wdata = (sc_dt::sc_uint<32>)(m_cmd_fifo.read()); 
			p_vci.pktid = r_pktid;
			p_vci.srcid = r_srcid;
			p_vci.trdid = r_trdid;
			p_vci.plen  = r_plen;        
			sc_dt::sc_uint<1> cons = r_const;         
			if (cons == 0x1)
				p_vci.cons = true;
			else
				p_vci.cons = false;        
			sc_dt::sc_uint<1> contig = r_contig;
			if(contig == 0x1)                     
				p_vci.contig = true;           
			else
				p_vci.contig = false;
                        if(((int) (m_cmd_fifo.read() >> (ring_cmd_data_size - 1) ) & 0x1) == 1)
				p_vci.eop = true;
			else    
				p_vci.eop = false; 
		}
		break;
            
	} // end switch fsm
} // end genMoore

///////////////////////////////////////////////////////////////////
void update_ring_signals(ring_signal_t p_ring_in, ring_signal_t &p_ring_out)
///////////////////////////////////////////////////////////////////
{

	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE:
			p_ring_out.rsp_grant = p_ring_in.rsp_grant && !m_rsp_fifo.rok();

	        	p_ring_out.rsp_w    = p_ring_in.rsp_w;
		        p_ring_out.rsp_data = p_ring_in.rsp_data;

		break;

		case DEFAULT:
                {
                        bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);
			p_ring_out.rsp_grant = (!m_rsp_fifo.rok() || (eop && p_ring_in.rsp_r)) ; 

        		p_ring_out.rsp_w    =  m_rsp_fifo.rok();
	        	p_ring_out.rsp_data =  m_rsp_fifo.read(); 

                }
		break;

		case KEEP: 
                { 
                        bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);
			p_ring_out.rsp_grant = m_rsp_fifo.rok() && p_ring_in.rsp_r && eop;

        		p_ring_out.rsp_w    =  m_rsp_fifo.rok();
	        	p_ring_out.rsp_data =  m_rsp_fifo.read();

                }
		break; 

	} // end switch
	p_ring_out.rsp_r     = p_ring_in.rsp_r;

	p_ring_out.cmd_w     = p_ring_in.cmd_w;
	p_ring_out.cmd_data  = p_ring_in.cmd_data;
	p_ring_out.cmd_grant = p_ring_in.cmd_grant;

	switch( r_ring_cmd_fsm ) 
	{
		case CMD_IDLE:
		{
			vci_addr_t rtgtid = (vci_addr_t) ((p_ring_in.cmd_data >> m_shift) << 2);
		        bool islocal = m_lt[rtgtid]  && (m_rt[rtgtid] == m_tgtid); 
                        bool eop     = ( (int) ((p_ring_in.cmd_data >> (ring_cmd_data_size - 1) ) & 0x1) == 1); 

			if (p_ring_in.cmd_w && !eop && islocal) 
			{
                               	p_ring_out.cmd_r =  m_cmd_fifo.wok();
                       	} 
       			else
                       	{
                               	p_ring_out.cmd_r =  p_ring_in.cmd_r; 
                       	}
		}
		break;

		case LOCAL:

                        p_ring_out.cmd_r =  m_cmd_fifo.wok(); 	
		break;

		case RING:

			p_ring_out.cmd_r = p_ring_in.cmd_r;
		break; 
	} // end switch

} // end update_ring_signals 
  
};

}} // end namespace
#endif // SOCLIB_CABA_VCI_SIMPLE_RING_TARGET_FAST_H


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
 * Date     : March 2010
 * Copyright: UPMC - LIP6
 */
///////////////////////////////////////////////////////////////////////////////////
//   Ring : Read Command Packet Format : 2 flits                                 //
//---------------------------------------------------------------------------------
//  1st flit    | eop |                address                                  |0|
//     (40)       (1)                      (38)                                 (1)  
//---------------------------------------------------------------------------------
//  2nd flit    | eop | srcid | cmd | contig |const | plen | pktid | trdid | be |0|
//     (40)       (1)   (14)     (2)    (1)     (1)    (8)    (4)      (4)   (4)(1)
///////////////////////////////////////////////////////////////////////////////////
//   Ring : Write Command Packet Format : 2 + N flits                            //
//---------------------------------------------------------------------------------
//  1st flit    | eop |                   address                               |0|
//     (40)       (1)                      (38)                                 (1)
//---------------------------------------------------------------------------------
//  2nd flit    | eop | srcid  | cmd | contig |const | plen | pktid | trdid | res |
//     (40)       (1)   (14)     (2)    (1)     (1)    (8)    (4)      (4)    (5)
//---------------------------------------------------------------------------------
//  next flits  | eop |res| be |              wdata                               |
//    (40)        (1)  (3)  (4)                (32)                                  
///////////////////////////////////////////////////////////////////////////////////
//   Ring : Read & write Response Packet Format : 1 + N flits     //
//-----------------------------------------------------------------
//  1st flit    | eop | rsrcid  | rerror | rpktid | rtrdid | res|BC|
//     (33)       (1)    (14)      (2)      (4)      (4)     (7) (1)
//-----------------------------------------------------------------
//  next flits  | eop |                   data                    |
//     (33)       (1)                     (32)                 
/////////////////////////////////////////////////////////////////////////////////////
//   Ring : Broadcast : 2 flits                                                    //
//-----------------------------------------------------------------------------------
//  1st flit    | eop | tgtid (proc)        | res   |  srcid  (memc)      | trdid |1|
//     (40)       (1)   (14)                  (6)      (14)                  (4)  (1)
//-----------------------------------------------------------------------------------
//  next flit   | eop | res |     nline (be + wdata)                                |
//     (40)       (1)   (5)       (34)    4    32                                       
/////////////////////////////////////////////////////////////////////////////////////
#ifndef SOCLIB_CABA_VCI_SIMPLE_RING_INITIATOR_FAST_H
#define SOCLIB_CABA_VCI_SIMPLE_RING_INITIATOR_FAST_H

#include "vci_target.h"
#include "generic_fifo.h"
#include "mapping_table.h"
#include "ring_signals_fast.h"

//#define I_DEBUG

namespace soclib { namespace caba {

namespace {

const char *ring_cmd_fsm_state_str_si[] = {
                "CMD_IDLE",
                "DEFAULT",
                "BDC_FIRST",
                "BDC_SECOND",
                "KEEP",
};

#ifdef I_DEBUG

const char *vci_cmd_fsm_state_str_si[] = {
        "CMD_FIRST_HEADER",
        "CMD_SECOND_HEADER",
        "WDATA",
};
const char *vci_rsp_fsm_state_str_si[] = {
        "RSP_HEADER",
        "RSP_DATA",
};

const char *ring_rsp_fsm_state_str_si[] = {
        "RSP_IDLE",
        "LOCAL",
        "RING",
};
#endif
} // end namespace

template<typename vci_param, int ring_cmd_data_size, int ring_rsp_data_size>
class VciSimpleRingInitiatorFast
{

typedef VciTarget<vci_param> vci_target_t;
typedef SimpleRingSignals ring_signal_t;

private:
        enum vci_cmd_fsm_state_e {
            CMD_FIRST_HEADER,     // first  flit for a ring cmd packet (read or write)
            CMD_SECOND_HEADER,   //  second flit for a ring cmd packet 
            WDATA,              //   data flit for a ring cmd write packet
        };
        
        enum vci_rsp_fsm_state_e {
            RSP_HEADER,     // first flit for a ring rsp packet (read or write)
            RSP_DATA,       // next  flit for a ring rsp packet
            
        };
        
        enum ring_rsp_fsm_state_e {
            	RSP_IDLE,    // waiting for first flit of a response packet
            	LOCAL,      // next flit of a local rsp packet
            	RING,  	    // next flit of a ring rsp packet
            };
        
        // cmd token allocation fsm
        enum ring_cmd_fsm_state_e {
            	CMD_IDLE,	    
             	DEFAULT,  
                BDC_FIRST,
                BDC_SECOND,	
            	KEEP,          	    
            };
        
        // structural parameters
        std::string         m_name;
        bool                m_alloc_init;

        sc_core::sc_signal<bool>     r_read_ack;       // vci ack  if vci cmd read 
         
        sc_core::sc_signal<sc_dt::sc_uint<vci_param::S> > r_srcid_save;
        sc_core::sc_signal<sc_dt::sc_uint<vci_param::T> > r_trdid_save;
        sc_core::sc_signal<sc_dt::sc_uint<vci_param::P> > r_pktid_save;
        sc_core::sc_signal<sc_dt::sc_uint<vci_param::E> > r_error_save;
        sc_core::sc_signal<uint32_t>                     r_cpt_tgt;
        // internal fifos 
        GenericFifo<uint64_t > m_cmd_fifo;     // fifo for the local command packet
        GenericFifo<uint64_t > m_rsp_fifo;     // fifo for the local response packet
        
        // routing table
        soclib::common::AddressMaskingTable<uint32_t> m_rt;
        soclib::common::AddressDecodingTable<uint32_t, bool> m_lt;

	//uint32_t          r_cpt_tgt;    
	uint64_t          r_brdcst_save; 
        uint32_t          m_srcid;
        uint32_t          m_nb_target;
        uint32_t          m_shift;
        
        // internal registers
        sc_core::sc_signal<int>	    r_ring_cmd_fsm;    // ring command packet FSM (distributed)
        sc_core::sc_signal<int>	    r_ring_rsp_fsm;    // ring response packet FSM
        sc_core::sc_signal<int>	    r_vci_cmd_fsm;    // vci command packet FSM
        sc_core::sc_signal<int>	    r_vci_rsp_fsm;    // vci response packet FSM


public :

#define __renRegInitS(x) x((((std::string) name)+"_" #x).c_str())

VciSimpleRingInitiatorFast(
	const char     *name,
        bool            alloc_init,
        const int       &wrapper_fifo_depth,
        const soclib::common::MappingTable &mt,
        const soclib::common::IntTab &ringid,
        const uint32_t &srcid,
	const uint32_t &nb_target)
      : m_name(name),
        m_alloc_init(alloc_init),
        m_cmd_fifo(((std::string) name)+"m_cmd_fifo", wrapper_fifo_depth),
        m_rsp_fifo(((std::string) name)+"m_rsp_fifo", wrapper_fifo_depth),
        m_rt(mt.getIdMaskingTable(ringid.level())),
        m_lt(mt.getIdLocalityTable(ringid)),
        m_srcid(srcid),
	m_nb_target(nb_target),
        m_shift(ring_cmd_data_size-vci_param::N+1),
        __renRegInitS(r_cpt_tgt),
        __renRegInitS(r_ring_cmd_fsm),
        __renRegInitS(r_ring_rsp_fsm),
	__renRegInitS(r_vci_cmd_fsm),
	__renRegInitS(r_vci_rsp_fsm)
  
 {} //  end constructor

 void reset()
{
	if(m_alloc_init)
		r_ring_cmd_fsm = DEFAULT;
	else
		r_ring_cmd_fsm = CMD_IDLE;

	r_vci_cmd_fsm  = CMD_FIRST_HEADER;
	r_vci_rsp_fsm  = RSP_HEADER;
	r_ring_rsp_fsm = RSP_IDLE;
	m_cmd_fifo.init();
	m_rsp_fifo.init();
 }

void transition(const vci_target_t &p_vci, const ring_signal_t p_ring_in, cmd_str &init_cmd, bool &init_rsp_val)
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
			if ( p_vci.cmdval.read() ) 
			{ 
#ifdef I_DEBUG
        std::cout << std::dec << sc_time_stamp() << " - " << m_name
                  << " - r_vci_cmd_fsm = " << vci_cmd_fsm_state_str_si[r_vci_cmd_fsm]
                  << " - vci_cmdval : " << p_vci.cmdval.read()
                  << " - vci_cmd : " << std::hex << p_vci.cmd.read()
                  << " - vci_plen : " << p_vci.plen.read()
                  << " - vci_address : " << p_vci.address.read()
                  << " - vci_srcid : " << p_vci.srcid.read()
                  << " - vci_wdata : " << p_vci.wdata.read()
                  << " - vci_be : " << p_vci.be.read() 
                  << " - vci_eop : " << p_vci.eop.read()
                  << " - fifo_wok : " << m_cmd_fifo.wok()                  
                  << std::endl;
#endif
                                cmd_fifo_data = (uint64_t) (((uint64_t) (p_vci.address.read() >> 2)) << m_shift); 
                                r_read_ack = p_vci.eop.read() 
                                                && ((p_vci.cmd.read() == vci_param::CMD_READ) 
                                                ||  (p_vci.cmd.read() == vci_param::CMD_LOCKED_READ)); 

                                if(m_cmd_fifo.wok())
                                {       
                                        cmd_fifo_put  = true;

                                        // test sur broadcast
                                        if ((p_vci.address.read() & 0x3) == 0x3)     
                                        {

                                              cmd_fifo_data = cmd_fifo_data | ((uint64_t) 0x1) |
                                                                              (((uint64_t) p_vci.srcid.read()) << 5) | 
                                                                              (((uint64_t) p_vci.trdid.read()) << 1) ;

                                              r_vci_cmd_fsm = WDATA;
   
                                        }
                                        else
                                        {
					        r_vci_cmd_fsm = CMD_SECOND_HEADER;
     
                                        }                                   
                                } // end fifo wok


			} // end if cmdval             
		break;
   
		case CMD_SECOND_HEADER:
#ifdef I_DEBUG
if (p_vci.cmdval.read())
        std::cout << std::dec << sc_time_stamp() << " - " << m_name
                  << " - r_vci_cmd_fsm = " << vci_cmd_fsm_state_str_si[r_vci_cmd_fsm]
                  << " - vci_cmdval : " << p_vci.cmdval.read()
                  << " - vci_cmd : " << std::hex << p_vci.cmd.read()
                  << " - vci_plen : " << p_vci.plen.read()
                  << " - vci_address : " << p_vci.address.read()
                  << " - vci_srcid : " << p_vci.srcid.read()
                  << " - vci_wdata : " << p_vci.wdata.read()
                  << " - vci_be : " << p_vci.be.read() 
                  << " - vci_eop : " << p_vci.eop.read()
                  << " - fifo_wok : " << m_cmd_fifo.wok()                  
                  << std::endl;
#endif
			if ( p_vci.cmdval.read() && m_cmd_fifo.wok() ) 
			{

				cmd_fifo_put  = true;
				cmd_fifo_data =  (((uint64_t) p_vci.srcid.read()) << (ring_cmd_data_size-vci_param::S-1))| 
                                                 (((uint64_t) (p_vci.cmd.read()   & 0x3))  << 23) |
                                                 (((uint64_t) (p_vci.plen.read()  & 0xFF)) << 13) | 
                                                 (((uint64_t) (p_vci.pktid.read() & 0xF))  << 9) | 
                                                 (((uint64_t) (p_vci.trdid.read() & 0xF))  << 5);   
				if (p_vci.contig == true)
					cmd_fifo_data = cmd_fifo_data | ((uint64_t) 0x1) << 22; 
				if (p_vci.cons == true)
					cmd_fifo_data = cmd_fifo_data | ((uint64_t) 0x1) << 21;
                                if(r_read_ack)
				{
					cmd_fifo_data =  cmd_fifo_data | 
                                                         (((uint64_t) (p_vci.be.read() & 0xF))  << 1) |
                                                         (((uint64_t) 0x1) << (ring_cmd_data_size-1));  
					r_vci_cmd_fsm = CMD_FIRST_HEADER;
				} 
				else     // write command
				{
					r_vci_cmd_fsm = WDATA;          
				}                                      
			} // endif cmdval
		break;    

		case WDATA:  
#ifdef I_DEBUG
if (p_vci.cmdval.read())
        std::cout << std::dec << sc_time_stamp() << " - " << m_name
                  << " - r_vci_cmd_fsm = " << vci_cmd_fsm_state_str_si[r_vci_cmd_fsm]
                  << " - vci_cmdval : " << p_vci.cmdval.read()
                  << " - vci_cmd : " << std::hex << p_vci.cmd.read()
                  << " - vci_plen : " << p_vci.plen.read()
                  << " - vci_address : " << p_vci.address.read()
                  << " - vci_srcid : " << p_vci.srcid.read()
                  << " - vci_wdata : " << p_vci.wdata.read()
                  << " - vci_be : " << p_vci.be.read() 
                  << " - vci_eop : " << p_vci.eop.read()
                  << " - fifo_wok : " << m_cmd_fifo.wok()                  
                  << std::endl;
#endif
			if ( p_vci.cmdval.read() && m_cmd_fifo.wok() ) 
			{



				cmd_fifo_put  = true;
				cmd_fifo_data =  ((uint64_t) p_vci.wdata.read()) | (((uint64_t) p_vci.be.read()) << 32);
				if ( p_vci.eop.read() == true ) 
				{
					r_vci_cmd_fsm = CMD_FIRST_HEADER;
					cmd_fifo_data = cmd_fifo_data | (((uint64_t) 0x1) << (ring_cmd_data_size-1)); //39 
				}
				else 
					r_vci_cmd_fsm = WDATA;
			} // end if cmdval
		break;
        
	} // end switch r_vci_cmd_fsm  

/////////// VCI RSP FSM /////////////////////////
	switch ( r_vci_rsp_fsm ) 
	{
		case RSP_HEADER:
 
			if ( m_rsp_fifo.rok() ) 
			{
/* #ifdef I_DEBUG
        std::cout << std::dec << sc_time_stamp() << " - " << m_name
                  << " - r_vci_rsp_fsm = " << vci_rsp_fsm_state_str_si[r_vci_rsp_fsm]
                  << " - rspack : " <<  p_vci.rspack.read()
                  << " - rdata : " <<  m_rsp_fifo.read()
                  << " - fifo_rsp_rok : " << m_rsp_fifo.rok()
                  << std::endl;
#endif */

                                bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);
                                if (eop) 
                                {
                                	rsp_fifo_get  = p_vci.rspack.read();
				        r_vci_rsp_fsm = RSP_HEADER;
                                }
                                else
                                {
		        		rsp_fifo_get = true;
			                r_srcid_save = ( sc_dt::sc_uint<vci_param::S>)  (m_rsp_fifo.read() >> (ring_rsp_data_size-vci_param::S-1));
			                r_trdid_save = ((sc_dt::sc_uint<vci_param::T>) (m_rsp_fifo.read() >> 8)) & 0xF; 
			                r_pktid_save = ((sc_dt::sc_uint<vci_param::P>) (m_rsp_fifo.read() >> 12)) & 0xF;            
			                r_error_save = ((sc_dt::sc_uint<vci_param::E>) (m_rsp_fifo.read() >> 16)) & 0x1;
				        r_vci_rsp_fsm = RSP_DATA;
                                }
			}       
		break;

		case RSP_DATA:
/* #ifdef I_DEBUG
if(m_rsp_fifo.rok())
        std::cout << std::dec << sc_time_stamp() << " - " << m_name
                  << " - r_vci_rsp_fsm = " << vci_rsp_fsm_state_str_si[r_vci_rsp_fsm]
                  << " - rspack : " <<  p_vci.rspack.read()
                  << " - rsrcid : " << std::hex << r_srcid_save.read()
                  << " - rdata : " <<  m_rsp_fifo.read()
                  << " - rerror : " << r_error_save.read()
                  << " - fifo_rsp_rok : " << m_rsp_fifo.rok()
                  << std::endl;
#endif */
			if ( p_vci.rspack.read() && m_rsp_fifo.rok() ) 
			{

				rsp_fifo_get = true;            
				if(((m_rsp_fifo.read()  >> 32) & 0x1) == 0x1)  
					r_vci_rsp_fsm = RSP_HEADER;                   
			} // endif rspack && rok
		break;

	} // end switch r_vci_rsp_fsm

//////////// RING CMD FSM /////////////////////////
	switch( r_ring_cmd_fsm ) 
	{
		case CMD_IDLE:    
			if ( p_ring_in.cmd_grant && m_cmd_fifo.rok() )  
                        {
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << m_name << " -- r_ring_cmd_fsm : CMD_IDLE " 
          << " -- in grant : " << p_ring_in.cmd_grant
          << " -- fifo rok : " << m_cmd_fifo.rok()
          << " -- fifo data : " << std::hex << m_cmd_fifo.read()          
          << std::endl;
#endif

                                if (m_cmd_fifo.read() & 0x1 == 0x1) // broadcast
                                {
                                        r_cpt_tgt      = 0;                   
                                        r_brdcst_save  = m_cmd_fifo.read(); // save first flit of brdcst
                                        r_ring_cmd_fsm = BDC_FIRST;                                  

                                } else {
                        		r_ring_cmd_fsm = KEEP; 
                                }
                        }
		break;

		case DEFAULT: 
			if ( m_cmd_fifo.rok()) // && p_ring_in.cmd_wok.read() ) 
			{
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << m_name 
          << " -- r_ring_cmd_fsm : DEFAULT "
          << " -- in grant   : " << p_ring_in.cmd_grant
          << " -- fifo rok : " << m_cmd_fifo.rok()
          << " -- fifo_cmd_data : " << std::hex << m_cmd_fifo.read()
          << std::endl;
#endif 
                                if (m_cmd_fifo.read() & 0x1 == 0x1) // broadcast
                                {
                                        r_cpt_tgt      = 0;                   
                                        r_brdcst_save  = m_cmd_fifo.read(); // save first flit of brdcst
                                        r_ring_cmd_fsm = BDC_FIRST;                                        
                                } 
				else 
				{
                                            
                                        cmd_fifo_get   = p_ring_in.cmd_r;
        		                r_ring_cmd_fsm = KEEP; 
                                }             
			}   
			else if ( !p_ring_in.cmd_grant)
				r_ring_cmd_fsm = CMD_IDLE; 
		break;

                case BDC_FIRST:
                       
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << m_name << " -- r_ring_cmd_fsm : BDC_FIRST "
          << " -- in grant   : " << p_ring_in.cmd_grant
          << " -- cpt_tgt : " << r_cpt_tgt
          << " -- fifo rok : " << m_cmd_fifo.rok()
          << " -- in wok : " << p_ring_in.cmd_r
          << " -- fifo_cmd_data : " << std::hex << m_cmd_fifo.read()
          << std::endl;
#endif

                        if (p_ring_in.cmd_r)                                 
			{
                        	if (r_cpt_tgt.read() == 0)
				{
                                	cmd_fifo_get = true;
				}
                                //r_cpt_tgt = r_cpt_tgt.read() + 1;
                                r_ring_cmd_fsm = BDC_SECOND;
			}
                break;

                case BDC_SECOND:
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << m_name << " -- r_ring_cmd_fsm : BDC_SECOND "
          << " -- in grant   : " << p_ring_in.cmd_grant
          << " -- fifo rok : " << m_cmd_fifo.rok()
          << " -- in wok : " << p_ring_in.cmd_r
          << " -- fifo_cmd_data : " << std::hex << m_cmd_fifo.read()
          << std::endl;
#endif
          		if(m_cmd_fifo.rok() && p_ring_in.cmd_r)
			{ 
				if(r_cpt_tgt.read() == m_nb_target - 2)
				{
					cmd_fifo_get = true;  

					if ( p_ring_in.cmd_grant )
						r_ring_cmd_fsm = DEFAULT;  
					else   
						r_ring_cmd_fsm = CMD_IDLE;
				}
				else
				{
					r_cpt_tgt      = r_cpt_tgt + 1;
                                	r_ring_cmd_fsm = BDC_FIRST;
				}

			}
                break;

		case KEEP:   
#ifdef I_DEBUG
std::cout << std::dec << sc_time_stamp() << " - " << m_name
                  << " - r_ring_cmd_fsm = " << ring_cmd_fsm_state_str_si[r_ring_cmd_fsm] 
                  << " -- in grant : " << p_ring_in.cmd_grant  
                  << " -- fifo ROK : " << m_cmd_fifo.rok()
                  << " -- in wok : " << p_ring_in.cmd_r
                  << " -- fifo data : " << std::hex << m_cmd_fifo.read()
                  << std::endl;
#endif                        
			if(m_cmd_fifo.rok() && p_ring_in.cmd_r ) 
			{
				cmd_fifo_get = true;  
				if (((int) (m_cmd_fifo.read() >> (ring_cmd_data_size - 1) ) & 0x1) == 1)  // 39
				{  
					if ( p_ring_in.cmd_grant )
						r_ring_cmd_fsm = DEFAULT;  
					else   
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
/* #ifdef I_DEBUG
if(p_ring_in.rsp_w)
        std::cout << std::dec << sc_time_stamp() << " - " << m_name
                  << " - r_ring_rsp_fsm = " << ring_rsp_fsm_state_str_si[r_ring_rsp_fsm]
                  << " -- in rok : " << p_ring_in.rsp_w
                  << " -- in wok : " << p_ring_in.rsp_r
                  << " -- fifo wok : " <<  m_rsp_fifo.wok() 
                  << " -- in data : " << std::hex << p_ring_in.rsp_data
                  << std::endl;
#endif */
			uint32_t  rsrcid  = (uint32_t)  ((sc_dt::sc_uint<vci_param::S>) (p_ring_in.rsp_data >> ring_rsp_data_size-vci_param::S-1));
			bool islocal      = m_lt[rsrcid] && (m_rt[rsrcid] == m_srcid);
			bool reop         = ((p_ring_in.rsp_data >> (ring_rsp_data_size - 1)) & 0x1) == 1; 

			if (p_ring_in.rsp_w  &&  islocal) 
			{   
				rsp_fifo_put  = m_rsp_fifo.wok();
				rsp_fifo_data = p_ring_in.rsp_data;

                                if (reop && m_rsp_fifo.wok())
                                        r_ring_rsp_fsm = RSP_IDLE;
                                else
                                        r_ring_rsp_fsm = LOCAL;

			}

			else    if (p_ring_in.rsp_w  &&  !islocal) 
			        {

                                        if (reop && p_ring_in.rsp_r)
                                                r_ring_rsp_fsm = RSP_IDLE;
                                        else
                                                r_ring_rsp_fsm = RING;
			        }

			        else // !p_ring_in.rsp_w 
			        {			
			        	r_ring_rsp_fsm = RSP_IDLE;
		        	}
		}
		break;

		case LOCAL:
		{
/* #ifdef I_DEBUG
if(p_ring_in.rsp_w)
        std::cout << std::dec << sc_time_stamp() << " - " << m_name
                  << " - r_ring_rsp_fsm = " << ring_rsp_fsm_state_str_si[r_ring_rsp_fsm]
                  << " -- in rok : " << p_ring_in.rsp_w
                  << " -- in wok : " << p_ring_in.rsp_r
                  << " -- fifo wok : " <<  m_rsp_fifo.wok() 
                  << " -- in data : " << std::hex << p_ring_in.rsp_data
                  << std::endl;
#endif */

			bool reop     = ((p_ring_in.rsp_data >> (ring_rsp_data_size - 1)) & 0x1) == 1;

			if (p_ring_in.rsp_w && m_rsp_fifo.wok() && reop)         
			{

				rsp_fifo_put  = true;
				rsp_fifo_data = p_ring_in.rsp_data;
				r_ring_rsp_fsm = RSP_IDLE;             
			}
			if (!p_ring_in.rsp_w || !m_rsp_fifo.wok() || !reop)         
			{

				rsp_fifo_put  = p_ring_in.rsp_w && m_rsp_fifo.wok();
				rsp_fifo_data = p_ring_in.rsp_data;
				r_ring_rsp_fsm = LOCAL;             
			}
		} 
		break;

		case RING:    
		{
/* #ifdef I_DEBUG
if(p_ring_in.rsp_w)
        std::cout << std::dec << sc_time_stamp() << " - " << m_name
                  << " - r_ring_rsp_fsm = " << ring_rsp_fsm_state_str_si[r_ring_rsp_fsm]
                  << " -- in rok : " << p_ring_in.rsp_w
                  << " -- in wok : " << p_ring_in.rsp_r
                  << " -- fifo wok : " <<  m_rsp_fifo.wok() 
                  << " -- in data : " << std::hex << p_ring_in.rsp_data
                  << std::endl;
#endif */
			bool reop     = ((p_ring_in.rsp_data >> (ring_rsp_data_size - 1)) & 0x1) == 1;

			if (p_ring_in.rsp_w && reop && p_ring_in.rsp_r)
			{
				r_ring_rsp_fsm = RSP_IDLE; 
			}
			else
			{
				r_ring_rsp_fsm = RING;
			}
		}	
		break;

	} // end switch rsp fsm

    ////////////////////////
    //  fifos update      //
   ////////////////////////
//-- to keep trace on ring traffic : a valid initiator command is being sent
        init_cmd.cmdval  = cmd_fifo_get;
        init_cmd.flit    = m_cmd_fifo.read();
        init_cmd.state   = ring_cmd_fsm_state_str_si[r_ring_cmd_fsm]; 
 
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
void genMoore(vci_target_t &p_vci)
///////////////////////////////////////////////////////////////////
{

	switch ( r_vci_cmd_fsm ) 
	{
		case CMD_FIRST_HEADER:
			p_vci.cmdack = false;
		break;

		case CMD_SECOND_HEADER:
			p_vci.cmdack = m_cmd_fifo.wok() && r_read_ack ;
		break;

		case WDATA:
			p_vci.cmdack = m_cmd_fifo.wok();
		break;    

	} // end switch fsm

	switch ( r_vci_rsp_fsm ) 
	{
		case RSP_HEADER:
                {
                        bool eop = ( (int) ((m_rsp_fifo.read() >> (ring_rsp_data_size - 1) ) & 0x1) == 1);
                        p_vci.rspval = m_rsp_fifo.rok() && eop;
                        p_vci.rsrcid = ( sc_dt::sc_uint<vci_param::S>)  (m_rsp_fifo.read() >> (ring_rsp_data_size-vci_param::S-1));
                        p_vci.rtrdid = ((sc_dt::sc_uint<vci_param::T>) (m_rsp_fifo.read() >> 8)) & 0xF; 
                        p_vci.rpktid = ((sc_dt::sc_uint<vci_param::P>) (m_rsp_fifo.read() >> 12)) & 0xF;            
                        p_vci.rerror = ((sc_dt::sc_uint<vci_param::E>) (m_rsp_fifo.read() >> 16)) & 0x1;
                        p_vci.rdata  = 0;
                        p_vci.reop   = true;

                }
		break;
	
		case RSP_DATA:
			p_vci.rspval = m_rsp_fifo.rok();
			p_vci.rsrcid = r_srcid_save;
			p_vci.rtrdid = r_trdid_save;
			p_vci.rpktid = r_pktid_save;
			p_vci.rerror = r_error_save;
			p_vci.rdata  = (sc_dt::sc_uint<32>) (m_rsp_fifo.read());       
			if (((m_rsp_fifo.read() >> 32) & 0x1) == 0x1)
				p_vci.reop   = true;
			else 
				p_vci.reop   = false;
		break;
	
	} // end switch fsm
} // end genMoore

///////////////////////////////////////////////////////////////////
void update_ring_signals(ring_signal_t p_ring_in, ring_signal_t &p_ring_out)
///////////////////////////////////////////////////////////////////
{    
	switch( r_ring_cmd_fsm ) 
	{
		case CMD_IDLE:
			p_ring_out.cmd_grant = p_ring_in.cmd_grant && !m_cmd_fifo.rok();

        		p_ring_out.cmd_w     = p_ring_in.cmd_w;
	        	p_ring_out.cmd_data  = p_ring_in.cmd_data;
		break;
	
		case DEFAULT:       
		{ 
                        bool brdcst = (m_cmd_fifo.read() & 0x1 == 0x1);
			p_ring_out.cmd_grant = !( m_cmd_fifo.rok());  

	        	p_ring_out.cmd_w    =  m_cmd_fifo.rok() && !brdcst;
		        p_ring_out.cmd_data =  m_cmd_fifo.read();
		}
		break;

	       	case BDC_FIRST:
			p_ring_out.cmd_grant = false;
			p_ring_out.cmd_w     = true;
			p_ring_out.cmd_data  =  r_brdcst_save | (((uint64_t) r_cpt_tgt.read()) << (ring_cmd_data_size-vci_param::S-1));     
                break;
		
		case BDC_SECOND:			
			p_ring_out.cmd_grant = m_cmd_fifo.rok() && p_ring_in.cmd_r && (r_cpt_tgt.read() == m_nb_target - 2);
       			p_ring_out.cmd_w    =  m_cmd_fifo.rok();
	        	p_ring_out.cmd_data =  m_cmd_fifo.read();

		break;

		case KEEP: 
		{ 
			int cmd_fifo_eop = (int) ((m_cmd_fifo.read() >> (ring_cmd_data_size - 1)) & 0x1) ; //39
			p_ring_out.cmd_grant = m_cmd_fifo.rok() && p_ring_in.cmd_r && (cmd_fifo_eop == 1);

        		p_ring_out.cmd_w    =  m_cmd_fifo.rok();
	        	p_ring_out.cmd_data =  m_cmd_fifo.read();
		}
		break;
	
	} // end switch

	p_ring_out.cmd_r       = p_ring_in.cmd_r;

	p_ring_out.rsp_grant   = p_ring_in.rsp_grant;
	p_ring_out.rsp_w       = p_ring_in.rsp_w;
	p_ring_out.rsp_data    = p_ring_in.rsp_data;

	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE:	
		{
			uint32_t  rsrcid  = (uint32_t)  ((sc_dt::sc_uint<vci_param::S>) (p_ring_in.rsp_data >> ring_rsp_data_size-vci_param::S-1));
			bool islocal      = m_lt[rsrcid] && (m_rt[rsrcid] == m_srcid);
			//bool reop     = ((p_ring_in.rsp_data >> (ring_rsp_data_size - 1)) & 0x1) == 1;

			if(p_ring_in.rsp_w && islocal) 
				p_ring_out.rsp_r = m_rsp_fifo.wok();
                        else
				p_ring_out.rsp_r = p_ring_in.rsp_r;
		}
		break;
	
		case LOCAL:
			p_ring_out.rsp_r = m_rsp_fifo.wok();
		break;
	
		case RING:
			p_ring_out.rsp_r = p_ring_in.rsp_r;
		break;    
	} // end switch


} // end update_ring_signals

};

}} // end namespace
#endif // SOCLIB_CABA_VCI_SIMPLE_RING_INITIATOR_FAST_H



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
 * Authors  : Franck WAJSBÜRT, Abdelmalek SI MERABET 
 * Date     : september 2008
 * Copyright: UPMC - LIP6
 */

///////////////////////////////////////////////////////////////////////////////////
//   Ring : Read Command Packet Format : 2 flits                                 //
//---------------------------------------------------------------------------------
//  1st flit    | eop |                   address                                 |
//     (37)       (1)                      (36)
//---------------------------------------------------------------------------------
//  2nd flit    | eop | srcid  | cmd | contig |const | plen | pktid | trdid | res |
//    (37)        (1)    (12)    (2)    (1)     (1)    (8)    (4)      (4)    (4)
///////////////////////////////////////////////////////////////////////////////////
//   Ring : Write Command Packet Format : 2 + N flits                            //
//---------------------------------------------------------------------------------
//  1st flit    | eop |                   address                                 |
//     (37)       (1)                      (36)
//---------------------------------------------------------------------------------
//  2nd flit    | eop | srcid  | cmd | contig |const | plen | pktid | trdid | res |
//    (37)        (1)    (12)    (2)    (1)     (1)    (8)    (4)      (4)    (4)
//---------------------------------------------------------------------------------
//  next flits  | eop | be |              data                                    |
//   (37)         (1)   (4)               (32)  
///////////////////////////////////////////////////////////////////////////////////
//   Ring : Broadcast Packet Format : 2 flits                                    //
//---------------------------------------------------------------------------------
//  1st flit    | eop | srcid  | cmd | contig |const | plen | pktid | trdid |res|11|
//    (37)        (1)    (12)    (2)    (1)     (1)    (8)    (4)      (4)   (2) (2)
//---------------------------------------------------------------------------------
//  next flits  | eop |\\\\|              nline                                    |
//   (37)         (1)   (4)               (32)  
////////////////////////////////////////////////////////////////////////////////////
//   Ring : Read Response Packet Format : 1 + N flits             //
//-----------------------------------------------------------------
//  1st flit    | eop |  rsrcid  | res | rerror | rpktid | rtrdid |
//     (33)       (1)     (12)     (8)    (4)      (4)      (4)
//-----------------------------------------------------------------
//  next flits  | eop |                   data                    |
//                (1)                     (32)                 
///////////////////////////////////////////////////////////////////
//   Ring : Write Response Packet Format : 2 flits               //
//-----------------------------------------------------------------
//  1st flit    | eop |  rsrcid  | res | rerror | rpktid | rtrdid |
//     (33)       (1)     (12)     (8)    (4)      (4)      (4)
///////////////////////////////////////////////////////////////////
//  next flit  | eop |                   data                     |
//      (33)     (1)                     (32)     
///////////////////////////////////////////////////////////////////


#include "../include/vci_ring_initiator_wrapper.h"

namespace soclib { namespace caba {

//#define I_DEBUG 

#define tmpl(x) template<typename vci_param> x VciRingInitiatorWrapper<vci_param>

///////////////////////////////////////////////
//	constructor
///////////////////////////////////////////////

tmpl(/**/)::VciRingInitiatorWrapper(sc_module_name	insname,
                            bool            alloc_init,
                            const int       &wrapper_fifo_depth,
                            const soclib::common::MappingTable &mt,
                            const soclib::common::IntTab &ringid,
                            const uint32_t &srcid)
    : soclib::caba::BaseModule(insname),
        m_alloc_init(alloc_init),
        m_srcid(srcid),
        r_ring_cmd_fsm("r_ring_cmd_fsm"),
        r_ring_rsp_fsm("r_ring_rsp_fsm"),
	r_vci_cmd_fsm("r_vci_cmd_fsm"),
	r_vci_rsp_fsm("r_vci_rsp_fsm"),
        m_cmd_fifo("m_cmd_fifo", wrapper_fifo_depth),
        m_rsp_fifo("m_rsp_fifo", wrapper_fifo_depth),
        m_rt(mt.getIdMaskingTable(ringid.level())),
        m_lt(mt.getIdLocalityTable(ringid)) 
 {

SC_METHOD (transition);
dont_initialize();
sensitive << p_clk.pos();

SC_METHOD (genMoore);
dont_initialize();
sensitive << p_clk.neg();

SC_METHOD(genMealy_vci_cmd);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_vci.eop;
sensitive << p_vci.cmd;

SC_METHOD(genMealy_rsp_out);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.rsp_rok;
sensitive << p_ring_in.rsp_data;

SC_METHOD(genMealy_rsp_in);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.rsp_rok;
sensitive << p_ring_in.rsp_wok;
sensitive << p_ring_in.rsp_data;

SC_METHOD(genMealy_cmd_out);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.cmd_rok;
sensitive << p_ring_in.cmd_data;

SC_METHOD(genMealy_cmd_in);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.cmd_wok;

SC_METHOD(genMealy_cmd_grant);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.cmd_grant;
sensitive << p_ring_in.cmd_wok;

SC_METHOD(genMealy_rsp_grant);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.rsp_grant;

} //  end constructor



/////////////////////////////////
//	transition             //
////////////////////////////////
tmpl(void)::transition()       
{
	if ( p_resetn == false ) 
	{ 
		if(m_alloc_init)
			r_ring_cmd_fsm = DEFAULT;
	else
		r_ring_cmd_fsm = CMD_IDLE;

	r_vci_cmd_fsm = CMD_FIRST_HEADER;
	r_vci_rsp_fsm = RSP_HEADER;
	r_ring_rsp_fsm = RSP_IDLE;
	m_cmd_fifo.init();
	m_rsp_fifo.init();

	   return;

        } // end if reset

	bool                       cmd_fifo_get = false;
	bool                       cmd_fifo_put = false;
	sc_uint<37>                cmd_fifo_data;

	bool                       rsp_fifo_get = false;
	bool                       rsp_fifo_put = false;
	sc_uint<33>                rsp_fifo_data;


//////////// VCI CMD FSM /////////////////////////
	switch ( r_vci_cmd_fsm ) 
	{
		case CMD_FIRST_HEADER:       
			if ( p_vci.cmdval.read() ) 
			{  
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << name()
          << " -- r_vci_cmd_fsm -- CMD_FIRST_HEADER "
          << " -- vci_cmdval : " << p_vci.cmdval.read()
          << " -- vci_address : " << std::hex << p_vci.address.read()
          << " -- vci_srcid : " << p_vci.srcid.read()
          << " -- fifo_wok : " << m_cmd_fifo.wok()
          << std::endl;
#endif
                                cmd_fifo_data = (sc_uint<37>) p_vci.address.read(); 

                                if(m_cmd_fifo.wok())
                                {       
                                        cmd_fifo_put  = true;

                                        // test sur broadcast
                                        if ((p_vci.address.read() & 0x3) == 0x3)     
                                        {
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << name() << " broadcast " << std::endl;
#endif
                                              cmd_fifo_data = cmd_fifo_data | (sc_uint<37>) ((p_vci.srcid.read() & 0xFF) << 24) | 
                                                                              (sc_uint<37>) ((p_vci.trdid.read() & 0xF)  << 4);

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


			if ( p_vci.cmdval.read() && m_cmd_fifo.wok() ) 
			{
#ifdef I_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- r_vci_cmd_fsm -- SECOND_HEADER "
              << " -- vci_cmdval : " << p_vci.cmdval.read()
              << " -- fifo_wok : " << m_cmd_fifo.wok()
              << " -- vci_cmd : " << std::hex << p_vci.cmd.read()
              << " -- vci_plen : " << p_vci.plen.read()   
              << " -- vci_trdid : " << p_vci.trdid.read()             
              << std::endl;
#endif
				cmd_fifo_put  = true;
				cmd_fifo_data =  (sc_uint<37>) ((p_vci.srcid.read() & 0xFF) << 24)| 
                                                 (sc_uint<37>) ((p_vci.cmd.read()   & 0x3)  << 22) |
                                                 (sc_uint<37>) ((p_vci.plen.read()  & 0xFF) << 12) | 
                                                 (sc_uint<37>) ((p_vci.pktid.read() & 0xF)  << 8) | 
                                                 (sc_uint<37>) ((p_vci.trdid.read() & 0xF)  << 4) ;  
				if (p_vci.contig == true)
					cmd_fifo_data = cmd_fifo_data | (sc_uint<37>) (0x1 << 21); 
				if (p_vci.cons == true)
					cmd_fifo_data = cmd_fifo_data | (sc_uint<37>) (0x1 << 20);
				if (p_vci.eop.read() && ((p_vci.cmd.read() == vci_param::CMD_READ) || (p_vci.cmd.read() == vci_param::CMD_LOCKED_READ))) // read command
				{
					sc_uint<1> eop = 1;
					cmd_fifo_data =  cmd_fifo_data | (sc_uint<37>) (eop << 36); 
					r_vci_cmd_fsm = CMD_FIRST_HEADER;
				} 
				else     // write command
				{
					r_vci_cmd_fsm = WDATA;          
				}                                      
			} // endif cmdval
		break;    

		case WDATA:  

  
			if ( p_vci.cmdval.read() && m_cmd_fifo.wok() ) 
			{
#ifdef I_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- r_vci_cmd_fsm -- WDATA "
              << " -- vci_cmdval : " << p_vci.cmdval.read()
              << " -- fifo_wok : " << m_cmd_fifo.wok()
              << " -- vci_wdata : " << std::hex << p_vci.wdata.read()
              << " -- vci_be : " << p_vci.be.read() 
              << " -- vci_eop : " << p_vci.eop.read()             
              << std::endl;
#endif


				cmd_fifo_put  = true;
				cmd_fifo_data =  (sc_uint<37>) (p_vci.wdata.read()) | (sc_uint<37>) (p_vci.be.read() << 32);
				if ( p_vci.eop.read() == true ) 
				{
					sc_uint<1> eop = 1;
					r_vci_cmd_fsm = CMD_FIRST_HEADER;
					cmd_fifo_data = cmd_fifo_data | (sc_uint<37>) (eop << 36); 
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

#ifdef I_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- r_vci_rsp_fsm -- RSP_HEADER "
              << " -- fifo_rsp_rok : " << m_rsp_fifo.rok()
              << std::endl;
#endif
				rsp_fifo_get = true;
				r_srcid_save = (sc_uint<vci_param::S>) ((m_rsp_fifo.read() >> 20 ) & 0xFF);
				r_trdid_save = (sc_uint<vci_param::T>) (m_rsp_fifo.read() & 0xF); 
				r_pktid_save = (sc_uint<vci_param::P>) ((m_rsp_fifo.read() >> 4) & 0xF);            
				r_error_save = (sc_uint<vci_param::E>)((m_rsp_fifo.read() >> 8) & 0x1);
				r_vci_rsp_fsm = RSP_DATA;
			}       
		break;

		case RSP_DATA:
			if ( p_vci.rspack.read() && m_rsp_fifo.rok() ) 
			{
#ifdef I_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- r_vci_rsp_fsm -- RSP_DATA "
              << " -- rsrcid : " << std::hex << r_srcid_save.read()
              << " -- rdata : " <<  m_rsp_fifo.read()
              << " -- rerror : " << r_error_save.read()
              << std::endl;
#endif
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
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << name() << " -- r_ring_cmd_fsm : CMD_IDLE " 
          << " -- in grant : " << p_ring_in.cmd_grant.read()
          << " -- fifo_ROK : " << m_cmd_fifo.rok() 
          << " -- fifo_data : " << std::hex << m_cmd_fifo.read()
          << std::endl;
#endif

			if ( p_ring_in.cmd_grant.read() && m_cmd_fifo.rok() )  
                        {

                		r_ring_cmd_fsm = KEEP; 
                        }
		break;

		case DEFAULT:          
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << name() 
           << " -- r_ring_cmd_fsm : DEFAULT "
           << " -- fifo_ROK : " << m_cmd_fifo.rok() 
           << " -- in wok : " << p_ring_in.cmd_wok.read()
           << " -- fifo_data : " << std::hex << m_cmd_fifo.read()
           << std::endl;
#endif

			if ( m_cmd_fifo.rok()) 
			{
				cmd_fifo_get = p_ring_in.cmd_wok.read();  
				r_ring_cmd_fsm = KEEP;             
			}   
			else if ( !p_ring_in.cmd_grant.read() )
				r_ring_cmd_fsm = CMD_IDLE; 
		break;

		case KEEP:  
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << name() << " -- r_ring_cmd_fsm : KEEP "
          << " -- fifo_rok : " << m_cmd_fifo.rok()
          << " -- ring_in_wok : " << p_ring_in.cmd_wok.read()
          << " -- fifo_out_data : " << std::hex << m_cmd_fifo.read()
          << std::endl;
#endif
                            
			if(m_cmd_fifo.rok() && p_ring_in.cmd_wok.read() ) 
			{
				cmd_fifo_get = true;  
				if (((int) (m_cmd_fifo.read() >> 36 ) & 0x1) == 1) 
				{  
					if ( p_ring_in.cmd_grant.read() )
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
			int rsrcid = (int)  ((p_ring_in.rsp_data.read() >> 20 ) & 0xFF);
			bool islocal = m_lt[rsrcid] && (m_rt[rsrcid] == m_srcid);

			if ( p_ring_in.rsp_rok.read() ) 
			{ 
#ifdef I_DEBUG 
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- ring_rsp_fsm -- RSP_IDLE "
              << " -- islocal : " << islocal
              << " -- rsrcid : " << std::hex << rsrcid
              << " -- ring_in.rsp_rok : " << p_ring_in.rsp_rok.read()
              << " -- m_rsp_fifo.wok : " <<  m_rsp_fifo.wok()         
              << std::endl;
#endif

 				if ( islocal && m_rsp_fifo.wok()) // response packet to local initiator
				{   
					r_ring_rsp_fsm = LOCAL;
					rsp_fifo_put  = true;
					rsp_fifo_data = p_ring_in.rsp_data.read();
				}
				else  
				{  
					if(!islocal && p_ring_in.rsp_wok.read())        
						r_ring_rsp_fsm = RING;  
				}
			} 
		}
		break;

		case LOCAL:

			if ( p_ring_in.rsp_rok.read() && m_rsp_fifo.wok() )         
			{
#ifdef I_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- ring_rsp_fsm -- LOCAL "
              << " -- ring_in.rsp_rok : " << p_ring_in.rsp_rok.read()
              << " -- m_rsp_fifo.wok : " <<  m_rsp_fifo.wok()   
              << " -- ring_in.rsp_data : " << std::hex << p_ring_in.rsp_data.read()
              << std::endl;
#endif
				rsp_fifo_put  = true;
				rsp_fifo_data = p_ring_in.rsp_data.read();
				int reop = (int) ((p_ring_in.rsp_data.read() >> 32 ) & 0x1) ;
				if ( reop == 1 )
                                {
					r_ring_rsp_fsm = RSP_IDLE;             
                                }
			} 
		break;

		case RING:     
			if ( p_ring_in.rsp_rok.read() && p_ring_in.rsp_wok.read())
			{
				int reop = (int) ((p_ring_in.rsp_data.read() >> 32 ) & 0x1) ;
				if ( reop == 1 ) 
					r_ring_rsp_fsm = RSP_IDLE; 
			}
		break;

	} // end switch rsp fsm
      
    ////////////////////////
    //  fifos update      //
   ////////////////////////

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
tmpl(void)::genMoore()
///////////////////////////////////////////////////////////////////
{
	switch ( r_vci_rsp_fsm ) 
	{
		case RSP_HEADER:
			p_vci.rspval = false;
		break;
	
		case RSP_DATA:
			p_vci.rspval = m_rsp_fifo.rok();
			p_vci.rsrcid = r_srcid_save;
			p_vci.rtrdid = r_trdid_save;
			p_vci.rpktid = r_pktid_save;
			p_vci.rerror = r_error_save;
			p_vci.rdata  = (sc_uint<32>) (m_rsp_fifo.read());       
			if (((m_rsp_fifo.read() >> 32) & 0x1) == 0x1)
				p_vci.reop   = true;
			else 
				p_vci.reop   = false;
		break;
	
	} // end switch fsm
} // end genMoore

///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_vci_cmd()
///////////////////////////////////////////////////////////////////
{
	switch ( r_vci_cmd_fsm ) 
	{
		case CMD_FIRST_HEADER:
			p_vci.cmdack = false;
		break;

		case CMD_SECOND_HEADER:
			if (p_vci.eop.read() && ((p_vci.cmd.read() == vci_param::CMD_READ) || (p_vci.cmd.read() == vci_param::CMD_LOCKED_READ)))    
				p_vci.cmdack = m_cmd_fifo.wok();
			else
				p_vci.cmdack = false;
		break;

		case WDATA:
			p_vci.cmdack = m_cmd_fifo.wok();
		break;    

	} // end switch fsm    
}  

///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_cmd_grant()
///////////////////////////////////////////////////////////////////
{    
	switch( r_ring_cmd_fsm ) 
	{
		case CMD_IDLE:
			p_ring_out.cmd_grant = p_ring_in.cmd_grant.read() && !m_cmd_fifo.rok();
		break;
	
		case DEFAULT:        
			p_ring_out.cmd_grant = !m_cmd_fifo.rok();  
		break;
	
		case KEEP:  
			int cmd_fifo_eop = (int) ((m_cmd_fifo.read() >> 36) & 0x1) ;
			p_ring_out.cmd_grant = m_cmd_fifo.rok() && p_ring_in.cmd_wok.read() && (cmd_fifo_eop == 1);	
		break;
	
	} // end switch
} // end genMealy_cmd_grant

///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_rsp_grant()
///////////////////////////////////////////////////////////////////

{
	p_ring_out.rsp_grant = p_ring_in.rsp_grant.read();
} // end genMealy_rsp_grant
   
///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_rsp_in()
///////////////////////////////////////////////////////////////////
{
	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE:	
		{
			int rsrcid = (int)  ((p_ring_in.rsp_data.read() >> 20 ) & 0xFF);
			bool islocal = m_lt[rsrcid] && (m_rt[rsrcid] == m_srcid); 
	
			p_ring_out.rsp_r = p_ring_in.rsp_rok.read() && ((islocal && m_rsp_fifo.wok()) || (!islocal && p_ring_in.rsp_wok.read()));	
		}
		break;
	
		case LOCAL:
			p_ring_out.rsp_r = m_rsp_fifo.wok();
		break;
	
		case RING:
			p_ring_out.rsp_r = p_ring_in.rsp_wok.read();
		break;    
	} // end switch
} // end genMealy_rsp_in_r

///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_rsp_out()
///////////////////////////////////////////////////////////////////
{
	p_ring_out.rsp_w    = p_ring_in.rsp_rok.read();
	p_ring_out.rsp_data = p_ring_in.rsp_data.read();
} // end genMealy_rsp_out_w

///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_cmd_in()
///////////////////////////////////////////////////////////////////
{

	if(r_ring_cmd_fsm==CMD_IDLE) 
             	p_ring_out.cmd_r = p_ring_in.cmd_wok.read();
	else 
            	p_ring_out.cmd_r = 1;
} // end genMealy_cmd_in
    
///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_cmd_out()
///////////////////////////////////////////////////////////////////
{

	if(r_ring_cmd_fsm==CMD_IDLE)
	{
		p_ring_out.cmd_w    = p_ring_in.cmd_rok.read();
		p_ring_out.cmd_data = p_ring_in.cmd_data.read();
	}
	else
	{
		p_ring_out.cmd_w    =  m_cmd_fifo.rok();
		p_ring_out.cmd_data =  m_cmd_fifo.read(); 
	}
} // end genMealy_cmd_out

}} // end namespace



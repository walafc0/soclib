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
 *
 * Copyright: UPMC - LIP6
 */
#include "../include/vci_ring_target_wrapper.h"
#include <cassert>

namespace soclib { namespace caba {

//#define T_DEBUG 

#define tmpl(x) template<typename vci_param> x VciRingTargetWrapper<vci_param>

///////////////////////////////////////////////
//	constructor
///////////////////////////////////////////////
tmpl(/**/)::VciRingTargetWrapper(sc_module_name	insname,
                            bool            alloc_target,
                            const int       &wrapper_fifo_depth,
                            const soclib::common::MappingTable &mt,
                            const soclib::common::IntTab &ringid,
                            const int &tgtid)
    : soclib::caba::BaseModule(insname),
        m_alloc_target(alloc_target),
        m_tgtid(tgtid),
        r_ring_cmd_fsm("r_ring_cmd_fsm"),
	r_ring_rsp_fsm("r_ring_rsp_fsm"),
	r_vci_cmd_fsm("r_vci_cmd_fsm"),
	r_vci_rsp_fsm("r_vci_rsp_fsm"),
        r_srcid("r_srcid"),
        r_cmd("r_cmd"),
        r_trdid("r_trdid"),
        r_pktid("r_pktid"),
        r_plen("r_plen"),
        r_contig("r_contig"),
        r_const("r_const"),
        r_addr("r_addr"),
        //r_brdcst("brdcst"),
        m_cmd_fifo("m_cmd_fifo", wrapper_fifo_depth),
        m_rsp_fifo("m_rsp_fifo", wrapper_fifo_depth),
        m_rt(mt.getRoutingTable(ringid)),
        m_lt(mt.getLocalityTable(ringid))

{

SC_METHOD (transition);
dont_initialize();
sensitive << p_clk.pos();

SC_METHOD (genMoore);
dont_initialize();
sensitive << p_clk.neg();

SC_METHOD(genMealy_cmd_out);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.cmd_rok;
sensitive << p_ring_in.cmd_data;

SC_METHOD(genMealy_cmd_in);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.cmd_rok;
sensitive << p_ring_in.cmd_wok;
sensitive << p_ring_in.cmd_data;

SC_METHOD(genMealy_rsp_out);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.rsp_rok;
sensitive << p_ring_in.rsp_data;

SC_METHOD(genMealy_rsp_in);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.rsp_wok;
sensitive << p_ring_in.rsp_data;

SC_METHOD(genMealy_rsp_grant);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.rsp_grant;
sensitive << p_ring_in.rsp_wok;


SC_METHOD(genMealy_cmd_grant);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_ring_in.cmd_grant;

} //  end constructor
///////////////////////////////
tmpl(void)::print_state()   //
//////////////////////////////
{
        std::cout << " -- " << name() << " -- ring_cmd_fsm transactions nulles " << std::endl;

        for (int i = 0; i < sizeof(nb_trans)/sizeof(typeof(*nb_trans)); i++) {
                if( nb_trans[i] == 0)
                        std::cout << " -- n° transaction : " << i << std::endl;

        }
} 
////////////////////////////////
//	transition 
////////////////////////////////
tmpl(void)::transition()       
{

	bool                       cmd_fifo_get = false;
	bool                       cmd_fifo_put = false;
	sc_uint<37>                cmd_fifo_data;
	
	bool                       rsp_fifo_get = false;
	bool                       rsp_fifo_put = false;
	sc_uint<33>                rsp_fifo_data;
	
	if ( p_resetn == false ) 
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
 
                for (int i = 0; i < sizeof(nb_trans)/sizeof(typeof(*nb_trans)); i++)
                     nb_trans[i] = 0;
 
		return;
	} 
    
//////////// VCI CMD FSM /////////////////////////
	switch ( r_vci_cmd_fsm ) 
	{

		case CMD_FIRST_HEADER:
                        if (m_cmd_fifo.rok() == true)
                        {
#ifdef T_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- r_vci_cmd_fsm -- CMD_FIRST_HEADER "
              << " -- fifo_rok : " << m_cmd_fifo.rok()
              << " -- fifo_data : " << std::hex << m_cmd_fifo.read()
              << std::endl;
#endif

                                cmd_fifo_get = true; 
				
                                if (m_cmd_fifo.read() & 0x3 == 0x3) // broadcast
                                {
#ifdef I_DEBUG
std::cout << sc_time_stamp() << " -- " << name() << " broadcast " << std::endl;
#endif
                                        r_addr   = (sc_uint<vci_param::N>) 0x3;     
                                        r_srcid  = (sc_uint<vci_param::S>) ((m_cmd_fifo.read() >> 24) & 0xFF); 
 					r_cmd    = (sc_uint<2>)  0x2; 
 					r_contig = (sc_uint<1>)  0x1; 
 					r_const  = (sc_uint<1>)  0x0; 
 					r_plen   = (sc_uint<vci_param::K>) 0x04; 
 					r_pktid  = (sc_uint<vci_param::P>) 0x0; 
 					r_trdid  = (sc_uint<vci_param::T>) ((m_cmd_fifo.read() >> 4) & 0xF); 

                                        r_vci_cmd_fsm = WDATA; 
                                }
                                else
                                {
                                        r_addr = (sc_uint<vci_param::N>) m_cmd_fifo.read();
                                        r_vci_cmd_fsm = CMD_SECOND_HEADER; 
                                }

         
			}  // end if rok
		break;

		case CMD_SECOND_HEADER:        
			if ( m_cmd_fifo.rok() ) 
			{

#ifdef T_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- r_vci_cmd_fsm -- CMD_SECOND_HEADER "
              << " -- fifo_rok : " << m_cmd_fifo.rok()
              << " -- fifo_data : " << std::hex << m_cmd_fifo.read()
              << " -- vci cmdack : " << p_vci.cmdack.read()
              << std::endl;
#endif
				if(((int) (m_cmd_fifo.read() >> 36 ) & 0x1) == 1)  // read command
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
 					r_srcid  = (sc_uint<vci_param::S>)  ((m_cmd_fifo.read() >> 24) & 0xFF); 
 					r_cmd    = (sc_uint<2>)  ((m_cmd_fifo.read() >> 22) & 0x3); 
 					r_contig = (sc_uint<1>)  ((m_cmd_fifo.read() >> 21) & 0x1); 
 					r_const =  (sc_uint<1>)  ((m_cmd_fifo.read() >> 20) & 0x1); 
 					r_plen  =  (sc_uint<vci_param::K>) ((m_cmd_fifo.read() >> 12) & 0xFF); 
 					r_pktid  = (sc_uint<vci_param::P>) ((m_cmd_fifo.read() >> 8) & 0xF); 
 					r_trdid  = (sc_uint<vci_param::T>) ((m_cmd_fifo.read() >> 4) & 0xF); 
 					r_vci_cmd_fsm = WDATA;
				}                                          
			} 
		break;

		case WDATA:

			if ( p_vci.cmdack.read() && m_cmd_fifo.rok() ) 
			{
#ifdef T_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- r_vci_cmd_fsm -- WDATA "
              << " -- vci_cmdack : " << p_vci.cmdack.read()
              << " -- fifo_rok : " << m_cmd_fifo.rok()
              << " -- fifo_data : " << std::hex << m_cmd_fifo.read()
              << " -- r_plen : " << r_plen.read()
              << std::endl;
#endif	

                		cmd_fifo_get = true; 
				sc_uint<1> contig = r_contig;
				if(contig == 0x1)    
					r_addr = r_addr.read() + vci_param::B ;                        
				if(((int) (m_cmd_fifo.read() >> 36 ) & 0x1) == 1)
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
			if((p_vci.rspval.read() == true) && (m_rsp_fifo.wok()))
			{
#ifdef T_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- r_vci_rsp_fsm -- RSP_HEADER "
              << " -- fifo_rsp_wok : " << m_rsp_fifo.wok()
              << " -- vci_rspval : " << p_vci.rspval.read()
              << " -- rsrcid : " << std::hex << p_vci.rsrcid.read()
              << " --  rerror : " << p_vci.rerror.read()
              << std::endl;
#endif
				rsp_fifo_data = (((sc_uint<33>) p_vci.rsrcid.read() & 0xFF) << 20) |
                                                (((sc_uint<33>) p_vci.rerror.read() & 0x1) << 8) | 
                                                (((sc_uint<33>) p_vci.rpktid.read() & 0xF) << 4) | 
                                                 ((sc_uint<33>) p_vci.rtrdid.read() & 0xF); 
				rsp_fifo_put = true; 
				r_vci_rsp_fsm = DATA;
			}
		break;

		case DATA:              
			if((p_vci.rspval.read() == true) && (m_rsp_fifo.wok())) 
			{
#ifdef T_DEBUG
         std::cout << sc_time_stamp() << " -- " << name()
              << " -- r_vci_rsp_fsm -- RSP_DATA "
              << " -- fifo_rsp_wok : " << m_rsp_fifo.wok()
              << " -- vci_rspval : " << p_vci.rspval.read()
              << " -- rdata : " << std::hex << p_vci.rdata.read()
              << " -- reop : " << p_vci.reop.read()
              << std::endl;
#endif

 
				rsp_fifo_put = true;
				rsp_fifo_data = (sc_uint<33>) p_vci.rdata.read();           
				if (p_vci.reop.read() == true) 
				{ 
					sc_uint<1> eop = 1;
					rsp_fifo_data = rsp_fifo_data |
					(sc_uint<33>) (eop << 32) ;
					r_vci_rsp_fsm = RSP_HEADER;
				}           		    
			}  
		break;

	} // end switch r_vci_rsp_fsm
   
//////////// RING RSP FSM (distributed) /////////////////////////
        
	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE:        

		if ( p_ring_in.rsp_grant.read() && m_rsp_fifo.rok() ) 
                {

//#ifdef T_DEBUG
//std::cout << sc_time_stamp() << " -- " << name() << " -- r_ring_rsp_fsm : RSP_IDLE "
//         << " -- rsp_fifo rok : "     <<  m_rsp_fifo.rok()
//         << " -- ring_rsp_grant : "   <<  p_ring_in.rsp_grant.read()
//         << " -- fifo data : " << std::hex << m_rsp_fifo.read()
//         << std::endl;
//#endif

			r_ring_rsp_fsm = KEEP;           
                }

		break;

		case DEFAULT:               

			if ( m_rsp_fifo.rok())   // && p_ring_in.rsp_wok.read() ) 
			{
//#ifdef T_DEBUG
//std::cout << sc_time_stamp() << " -- " << name() 
//         << " -- r_ring_rsp_fsm : DEFAULT "   
//         << " -- rsp_fifo rok : "     <<  m_rsp_fifo.rok()
//         << " -- ring_rsp_grant : "   <<  p_ring_in.rsp_grant.read()
//         << " -- ring_in wok : " << p_ring_in.rsp_wok.read() 
//         << " -- fifo_rsp_data : " << std::hex << m_rsp_fifo.read()
//         << std::endl;
//#endif
				rsp_fifo_get = p_ring_in.rsp_wok.read();
				r_ring_rsp_fsm = KEEP;
			}   
			else if ( !p_ring_in.rsp_grant.read() )
				r_ring_rsp_fsm = RSP_IDLE;  
		break;

		case KEEP:                
			if(m_rsp_fifo.rok() && p_ring_in.rsp_wok.read()) 
			{
//#ifdef T_DEBUG
//std::cout << sc_time_stamp() << " -- " << name() << " -- r_ring_rsp_fsm : KEEP "
//         << " -- fifo_rok : " << m_rsp_fifo.rok()
//         << " -- ring_in_wok : " << p_ring_in.rsp_wok.read()
//         << " -- fifo_out_data : " << std::hex << m_rsp_fifo.read()
//         << std::endl;
//#endif
				rsp_fifo_get = true;              
				if ((int) ((m_rsp_fifo.read() >> 32 ) & 0x1) == 1)  
				{             
					if ( p_ring_in.rsp_grant.read() )
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
			uint32_t rtgtid = (uint32_t) p_ring_in.cmd_data.read();
			bool islocal = m_lt[rtgtid]  && (m_rt[rtgtid] == m_tgtid); 
                        bool brdcst  = (rtgtid & 0x3) == 0X3; 
                        bool eop     = ( (int) ((p_ring_in.cmd_data.read() >> 36 ) & 0x1) == 1); 
                        uint32_t nb_next_states = 0;

//#ifdef T_DEBUG
//        std::cout << sc_time_stamp() << " -- " << name()
//             << " -- ring_cmd_fsm -- CMD_IDLE "
//             << " -- in rok : " << p_ring_in.cmd_rok.read()
//             << " -- isloc : " << islocal
//             << " -- addr : " << std::hex << rtgtid
//             << " -- brdcst : " << brdcst
//             << " -- in wok : " << p_ring_in.cmd_wok.read()
//             << " -- fifo wok : " << m_cmd_fifo.wok()
//             << " -- eop : " << eop
//             << std::endl;
//#endif   

                      if(p_ring_in.cmd_rok.read() && !eop && brdcst && !m_cmd_fifo.wok()) {
                              r_ring_cmd_fsm = BROADCAST_0; 
                              nb_next_states++;
                              nb_trans[0]++;
                      }  
        
                      if(p_ring_in.cmd_rok.read() && !eop && brdcst && m_cmd_fifo.wok()) {
                              r_ring_cmd_fsm = BROADCAST_1;
                              cmd_fifo_put  = true;
                      	      cmd_fifo_data = p_ring_in.cmd_data.read();
                              nb_trans[1]++;
                              nb_next_states++;

                      }  
        
                      if (p_ring_in.cmd_rok.read() && !eop && !brdcst && islocal) {
                              r_ring_cmd_fsm = LOCAL; 
                              cmd_fifo_put   = m_cmd_fifo.wok();
                              cmd_fifo_data  = p_ring_in.cmd_data.read();  
                              nb_next_states++;
                              nb_trans[2]++;

                      } 
        
                      if (p_ring_in.cmd_rok.read() && !eop && !brdcst && !islocal) {
                              r_ring_cmd_fsm = RING;
                              nb_next_states++;
                              nb_trans[3]++;
                      }  

                      if (!p_ring_in.cmd_rok.read() || eop) {
                              r_ring_cmd_fsm = CMD_IDLE;
                              nb_next_states++;
                              nb_trans[4]++;
                      }
              
                assert(  nb_next_states && "r_ring_cmd_fsm IDLE : NO NEXT STATE !!!" );
                assert( (nb_next_states == 1) &&  "r_ring_cmd_fsm IDLE :  TOO MANY NEXT STATES !!!");

                }
                break;                                

                case BROADCAST_0:

//#ifdef T_DEBUG
//        std::cout << sc_time_stamp() << " -- " << name()
//             << " -- ring_cmd_fsm -- BROADCAST_0 "
//             << " -- ringin cmd rok : " << p_ring_in.cmd_rok.read()
//             << " -- ringin cmd wok : " << p_ring_in.cmd_wok.read() 
//             << " -- ringin data : " << std::hex << p_ring_in.cmd_data.read()
//             << " -- fifo cmd wok : " << m_cmd_fifo.wok()
//             << std::endl;
//#endif
                	if ( m_cmd_fifo.wok() )
                        { 
				cmd_fifo_data = p_ring_in.cmd_data.read();
				r_ring_cmd_fsm = BROADCAST_1;
                                nb_trans[5]++;

                        } else {
				r_ring_cmd_fsm = BROADCAST_0;
                                nb_trans[6]++;
                        }

		break;

                case BROADCAST_1:
                {
//#ifdef T_DEBUG
//        std::cout << sc_time_stamp() << " -- " << name()
//             << " -- ring_cmd_fsm -- BROADCAST_1 "
//             << " -- ringin cmd rok : " << p_ring_in.cmd_rok.read()
//             << " -- ringin cmd wok : " << p_ring_in.cmd_wok.read() 
//             << " -- ringin data : " << std::hex << p_ring_in.cmd_data.read()
//             << " -- fifo cmd wok : " << m_cmd_fifo.wok()
//             << std::endl;
//#endif

                        bool eop = ( (int) ((p_ring_in.cmd_data.read() >> 36 ) & 0x1) == 1);

                	if ( p_ring_in.cmd_rok.read() && m_cmd_fifo.wok() && eop )
                        { 
				cmd_fifo_data = p_ring_in.cmd_data.read();
                                bool eop      = ( (int) ((p_ring_in.cmd_data.read() >> 36 ) & 0x1) == 1);
				cmd_fifo_put  = 1;
				r_ring_cmd_fsm = CMD_IDLE;
                                nb_trans[7]++;

                        }
                        else {        		 
                                nb_trans[8]++;
                		r_ring_cmd_fsm = BROADCAST_1;
                        }
                        
                } 
 		break;

		case LOCAL:  
                {
//#ifdef T_DEBUG
//        std::cout << sc_time_stamp() << " -- " << name()
//             << " -- ring_cmd_fsm -- LOCAL "
//             << " -- ringin cmd rok : " << p_ring_in.cmd_rok.read()
//             << " -- ringin cmd wok : " << p_ring_in.cmd_wok.read() 
//             << " -- ringin data : " << std::hex << p_ring_in.cmd_data.read()
//             << " -- fifo cmd wok : " << m_cmd_fifo.wok()
//             << std::endl;
//#endif

                        bool eop = ( (int) ((p_ring_in.cmd_data.read() >> 36 ) & 0x1) == 1);

                 	if ( p_ring_in.cmd_rok.read() && m_cmd_fifo.wok() && eop )
                        { 

				cmd_fifo_put  = true;
				cmd_fifo_data = p_ring_in.cmd_data.read();
			     	r_ring_cmd_fsm = CMD_IDLE;
                                nb_trans[9]++;
			     		
                        }
                        
                 	if ( !p_ring_in.cmd_rok.read() || !m_cmd_fifo.wok() || !eop )
                        { 

				cmd_fifo_put  = p_ring_in.cmd_rok.read() && m_cmd_fifo.wok();
				cmd_fifo_data = p_ring_in.cmd_data.read();
			     	r_ring_cmd_fsm = LOCAL;
                                nb_trans[10]++;
			     		
                        }                        
                } 
		break;

		case RING:  
                { 
//#ifdef T_DEBUG
//        std::cout << sc_time_stamp() << " -- " << name()
//             << " -- ring_cmd_fsm -- RING "
//             << " -- ringin cmd rok : " << p_ring_in.cmd_rok.read()
//             << " -- ringin data : " << std::hex << p_ring_in.cmd_data.read()
//             << " -- ring cmd wok : " << p_ring_in.cmd_wok.read()
//             << std::endl;
//#endif  
			bool eop = ( (int) ((p_ring_in.cmd_data.read() >> 36 ) & 0x1) == 1);
			if ( p_ring_in.cmd_rok.read() && eop ) {        
        			r_ring_cmd_fsm = CMD_IDLE;
                                nb_trans[11]++;
                        }
                        else {
 				r_ring_cmd_fsm = RING;
                                nb_trans[12]++;
                        }
                }
		break;

	} // end switch cmd fsm 

    ////////////////////////
    //  fifos update      //
   ////////////////////////

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
tmpl(void)::genMoore()
///////////////////////////////////////////////////////////////////
{
	if( r_vci_rsp_fsm == RSP_HEADER ) 
		p_vci.rspack = false;
	else
		p_vci.rspack = m_rsp_fifo.wok();

	switch ( r_vci_cmd_fsm ) 
	{
		case CMD_FIRST_HEADER:                        			  
		        p_vci.cmdval = false;

		break;

		case CMD_SECOND_HEADER:         
			if(((int) (m_cmd_fifo.read() >> 36 ) & 0x1) == 1) // eop
			{
				p_vci.cmdval = m_cmd_fifo.rok(); 
				p_vci.address = r_addr;
				p_vci.cmd = (sc_uint<2>)  ((m_cmd_fifo.read() >> 22) & 0x3);
				p_vci.wdata = 0;
                                p_vci.be    = 0xF;
				p_vci.pktid = (sc_uint<vci_param::P>) ((m_cmd_fifo.read() >> 8) & 0xF);
				p_vci.srcid = (sc_uint<vci_param::S>)  ((m_cmd_fifo.read() >> 24) & 0xFF);
				p_vci.trdid = (sc_uint<vci_param::T>)  ((m_cmd_fifo.read() >> 4)  & 0xF);
				p_vci.plen =  (sc_uint<vci_param::K>)  ((m_cmd_fifo.read() >> 12) & 0xFF);
				p_vci.eop = true;         
				sc_uint<1> cons = (sc_uint<1>)  ((m_cmd_fifo.read() >> 20) & 0x1) ; 
				if (cons == 0x1)
					p_vci.cons = true;
				else
					p_vci.cons = false;        
				sc_uint<1> contig = (sc_uint<1>)  ((m_cmd_fifo.read() >> 21) & 0x1);
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
			p_vci.address = r_addr;
			p_vci.be = (sc_uint<vci_param::B>)((m_cmd_fifo.read()  >> 32) & 0xF);
			p_vci.cmd = r_cmd;
			p_vci.wdata = (sc_uint<32>)(m_cmd_fifo.read()); 
			p_vci.pktid = r_pktid;
			p_vci.srcid = r_srcid;
			p_vci.trdid = r_trdid;
			p_vci.plen  = r_plen;        
			sc_uint<1> cons = r_const;         
			if (cons == 0x1)
				p_vci.cons = true;
			else
				p_vci.cons = false;        
			sc_uint<1> contig = r_contig;
			if(contig == 0x1)                     
				p_vci.contig = true;           
			else
				p_vci.contig = false;
                        if(((int) (m_cmd_fifo.read() >> 36 ) & 0x1) == 1)
				p_vci.eop = true;
			else    
				p_vci.eop = false; 
		}
		break;
	} // end switch fsm
} // end genMoore

///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_rsp_grant()
///////////////////////////////////////////////////////////////////
{
	switch( r_ring_rsp_fsm ) 
	{
		case RSP_IDLE:
			p_ring_out.rsp_grant = p_ring_in.rsp_grant.read() && !m_rsp_fifo.rok();
		break;

		case DEFAULT:
			p_ring_out.rsp_grant = !m_rsp_fifo.rok();  
		break;

		case KEEP:  
			int rsp_fifo_eop = (int) ((m_rsp_fifo.read() >> 32) & 0x1);
			p_ring_out.rsp_grant = m_rsp_fifo.rok() && p_ring_in.rsp_wok.read() && (rsp_fifo_eop == 1);

		break; 

	} // end switch          
} // end genMealy_rsp_grant

///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_cmd_grant()
///////////////////////////////////////////////////////////////////
{
	p_ring_out.cmd_grant = p_ring_in.cmd_grant.read();
} // end genMealy_cmd_grant
    
///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_cmd_in()
///////////////////////////////////////////////////////////////////
{ 
        //bool brdcst  = false;
        
	switch( r_ring_cmd_fsm ) 
	{
		case CMD_IDLE:
		{
			uint32_t rtgtid = (uint32_t) p_ring_in.cmd_data.read();
			bool islocal = m_lt[rtgtid]  && (m_rt[rtgtid] == m_tgtid); 
                        bool brdcst  = (rtgtid & 0x3) == 0X3; 
                        bool eop     = ( (int) ((p_ring_in.cmd_data.read() >> 36 ) & 0x1) == 1); 

//                       p_ring_out.cmd_r = p_ring_in.cmd_rok.read() && !eop &&
//                                           ((brdcst && m_cmd_fifo.wok() && p_ring_in.cmd_wok.read()) || 
//                                           (islocal &&  m_cmd_fifo.wok()) || (!islocal && p_ring_in.cmd_wok.read()));
//
//----

                      if(p_ring_in.cmd_rok.read() && !eop && brdcst && !m_cmd_fifo.wok()) {
                              p_ring_out.cmd_r = m_cmd_fifo.wok() && p_ring_in.cmd_wok.read();
                              //r_ring_cmd_fsm = BROADCAST_0; 
                      }  
        
                      if(p_ring_in.cmd_rok.read() && !eop && brdcst && m_cmd_fifo.wok()) {
                               p_ring_out.cmd_r = m_cmd_fifo.wok() && p_ring_in.cmd_wok.read();
//                             r_ring_cmd_fsm = BROADCAST_1;
//                             cmd_fifo_put  = true;
//                     	      cmd_fifo_data = p_ring_in.cmd_data.read();

                      }  
        
                      if (p_ring_in.cmd_rok.read() && !eop && !brdcst && islocal) {
                               p_ring_out.cmd_r =  m_cmd_fifo.wok();
//                             r_ring_cmd_fsm = LOCAL; 
//                             cmd_fifo_put   = m_cmd_fifo.wok();
//                             cmd_fifo_data  = p_ring_in.cmd_data.read();  

                      } 
        
                      if (p_ring_in.cmd_rok.read() && !eop && !brdcst && !islocal) {
                                p_ring_out.cmd_r =  p_ring_in.cmd_wok.read(); 
//                              r_ring_cmd_fsm = RING;
                      }  

                      if (!p_ring_in.cmd_rok.read() || eop) {
                           p_ring_out.cmd_r =  p_ring_in.cmd_wok.read(); 
//                              r_ring_cmd_fsm = CMD_IDLE;
                      }

//---- 


		}
		break;

                case BROADCAST_0:
                        p_ring_out.cmd_r =  m_cmd_fifo.wok() && p_ring_in.cmd_wok.read(); 
                break;

                case BROADCAST_1:
                        p_ring_out.cmd_r =  m_cmd_fifo.wok() && p_ring_in.cmd_wok.read(); 
                break;

		case LOCAL:

                        p_ring_out.cmd_r =  m_cmd_fifo.wok(); 	
		break;

		case RING:

			p_ring_out.cmd_r = p_ring_in.cmd_wok.read();
		break;    
	} // end switch

} // end genMealy_cmd_in_r
  
///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_cmd_out()
///////////////////////////////////////////////////////////////////
{
	p_ring_out.cmd_w    = p_ring_in.cmd_rok.read();
	p_ring_out.cmd_data = p_ring_in.cmd_data.read();
} // end genMealy_cmd_out_w

   
///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_rsp_in()
//////////////////////////////////////////////////////////////////
{
       	p_ring_out.rsp_r = p_ring_in.rsp_wok.read();
} // end genMealy_rsp_in
  
///////////////////////////////////////////////////////////////////
tmpl(void)::genMealy_rsp_out()
///////////////////////////////////////////////////////////////////
{
	if(r_ring_rsp_fsm==RSP_IDLE)
	{
		p_ring_out.rsp_w    = p_ring_in.rsp_rok.read();
		p_ring_out.rsp_data = p_ring_in.rsp_data.read();
	}
	else
	{
		p_ring_out.rsp_w    =  m_rsp_fifo.rok();
		p_ring_out.rsp_data =  m_rsp_fifo.read(); 
	}
} // end genMealy_rsp_out
}} // end namespace



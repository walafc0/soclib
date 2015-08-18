/* -*- c++ -*-
  *
  * File : vci_dspin_initiator_wrapper.cpp
  * Copyright (c) UPMC, Lip6
  * Authors : Alain Greiner,
  * Date    : 03/06/20213
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
  */

////////////////////////////////////////////////////////////i//////////////////////
// This component can be used to connect a VCI initiator to a DSPIN interconnect.
// The VCI ADDRESS width can have up to 40 bits.
// Both 32 bits and 64 bits are supported for VCI DATA width.
// This is a lightweight implementation, as this component contains
// no intermediate FIFOs. 
///////////////////////////////////////////////////////////////////////////////////
// For VCI 32 bits, DSPIN command flit width must be 39 bits (plus EOP), 
// and response packet must be 32 bits (plus EOP)
// All VCI fields are transmited through the DSPIN network.
// All VCI commands (including LL/SC/CAS) are supported.
///////////////////////////////////////////////////////////////////////////////////   
// For VCI 64 bits, DSPIN command flit width must be 64 bits (plus EOP),
// and response packet must be 64 bits (plus EOP)
// Not all VCI fiels are transmited (no BE, no PKTID, no CONS/CONTIG).
///////////////////////////////////////////////////////////////////////////////////

#include "../include/vci_dspin_initiator_wrapper.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, size_t dspin_cmd_width, size_t dspin_rsp_width> x VciDspinInitiatorWrapper<vci_param, dspin_cmd_width, dspin_rsp_width>

//////////////////////////////////////////////////////////
tmpl(/**/)::VciDspinInitiatorWrapper( sc_module_name name,
                                      const size_t  srcid_width)
       : soclib::caba::BaseModule(name),
     
        p_clk( "p_clk" ),
        p_resetn( "p_resetn" ),
        p_dspin_cmd( "p_dspin_cmd" ),
        p_dspin_rsp( "p_dspin_rsp" ),
        p_vci( "p_vci" ),

        r_cmd_fsm( "r_cmd_fsm" ),
        r_rsp_fsm( "r_rsp_fsm" ),
        r_rsp_buf( "r_rsp_buf" ),

        m_srcid_width( srcid_width )
{
    std::cout << "  - Building VciDspinInitiatorWrapper : " << name << std::endl;

	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();

    SC_METHOD (genMealy_vci_cmd);
	dont_initialize();
    sensitive << p_clk.neg();
    sensitive << p_dspin_cmd.read;

    SC_METHOD (genMealy_vci_rsp);
	dont_initialize();
    sensitive << p_clk.neg();
    sensitive << p_dspin_rsp.data;
    sensitive << p_dspin_rsp.write;
    sensitive << p_dspin_rsp.eop;

    SC_METHOD (genMealy_dspin_cmd);
	dont_initialize();
    sensitive << p_clk.neg();
    sensitive << p_vci.cmdval;
    sensitive << p_vci.address;
    sensitive << p_vci.wdata;
    sensitive << p_vci.srcid;
    sensitive << p_vci.trdid;
    sensitive << p_vci.pktid;
    sensitive << p_vci.plen;
    sensitive << p_vci.be;
    sensitive << p_vci.cmd;
    sensitive << p_vci.cons;
    sensitive << p_vci.contig;
    sensitive << p_vci.eop;

    SC_METHOD (genMealy_dspin_rsp);
	dont_initialize();
    sensitive << p_clk.neg();
    sensitive << p_vci.rspack;

    if ( vci_param::B == 4 )    // 32 bits
    {
        assert( (dspin_cmd_width == 39) and "DSPIN CMD flit width must have 39 bits");
        assert( (dspin_rsp_width == 32) and "DSPIN RSP flit width must have 32 bits");
        assert( (vci_param::N    <= 40) and "VCI ADDRESS cannot have more than 40 bits");
        assert( (vci_param::K    <= 8 ) and "VCI PLEN cannot have more than 8 bits");
        assert( (vci_param::S    <= 14) and "VCI SRCID cannot have more than 14 bits");
        assert( (vci_param::T    <= 4 ) and "VCI TRDID cannot have more than 4 bits");
        assert( (vci_param::P    <= 4 ) and "VCI PKTID cannot have more than 4 bits");
        assert( (vci_param::E    <= 2 ) and "VCI RERROR cannot have more than 2 bits");
    }
    else if ( vci_param::B == 8 )   // 64 bits
    {
        assert( (dspin_cmd_width == 64) and "DSPIN CMD flit width must have 64 bits");
        assert( (dspin_rsp_width == 64) and "DSPIN RSP flit width must have 64 bits");
        assert( (vci_param::N    <= 40) and "VCI ADDRESS cannot have more than 40 bits");
        assert( (vci_param::K    <= 8 ) and "VCI PLEN cannot have more than 8 bits");
        assert( (vci_param::S    <= 14) and "VCI SRCID cannot have more than 14 bits");
        assert( (vci_param::T    <= 4 ) and "VCI TRDID cannot have more than 4 bits");
        assert( (vci_param::P    <= 4 ) and "VCI PKTID cannot have more than 4 bits");
        assert( (vci_param::E    <= 2 ) and "VCI RERROR cannot have more than 2 bits");
    }
    else
    {
        assert( false and "VCI DATA should be 32 or 64 bits");
    }

}; //  end constructor

////////////////////////
tmpl(void)::transition()
{
	if ( p_resetn == false ) 
    {
	    r_cmd_fsm = CMD_IDLE;
	    r_rsp_fsm = RSP_IDLE;
	    return;
	} 

    /////////////////////////////////////////////////////////////
    // VCI command packet to DSPIN command packet
    /////////////////////////////////////////////////////////////
    // When VCI DATA width is 32 bits:
    // - A N flits VCI write command packet is translated
    //   to a N+2 flits DSPIN command.
    // - A single flit VCI read command packet is translated
    //   to a 2 flits DSPIN command.
    // The FSM has four states:
    // A DSPIN flit is written on the DSPIN port in all states
    // but a VCI flit is consumed only in the CMD_READ and
    // CMD_WDATA states.
    /////////////////////////////////////////////////////////////
    // When VCI DATA width is 64 bits:
    // - A N flits VCI write command packet is translated
    //   to a N+1 flits DSPIN command.
    // - A single flit VCI read command packet is translated
    //   to a singles DSPIN command.
    // The FSM has only three states: IDLE, READ and WDATA.
    //////////////////////////////////////////////////////////////

    if ( vci_param::B == 4 ) ////////////////  VCI DATA = 32 bits //////////
    {
	    switch( r_cmd_fsm.read() )
        {
	    case CMD_IDLE:        // transmit first DSPIN CMD flit 
		    if ( p_vci.cmdval.read() and p_dspin_cmd.read.read() )
            {
                if ( (p_vci.cmd.read() == vci_param::CMD_READ) or
                     (p_vci.cmd.read() == vci_param::CMD_LOCKED_READ) ) 
                {
                    r_cmd_fsm = CMD_READ;
                }
                else
                {
                    r_cmd_fsm = CMD_WRITE;
                }
            } 
        break;
        case CMD_READ:        // transmit second DSPIN CMD flit
		    if ( p_vci.cmdval.read() and p_dspin_cmd.read.read() )
            {
                r_cmd_fsm = CMD_IDLE;
            }
        break;
        case CMD_WRITE:       // transmit second DSPIN CMD flit
		    if ( p_vci.cmdval.read() and p_dspin_cmd.read.read() )
            {
                r_cmd_fsm = CMD_WDATA;
            }
        break;
        case CMD_WDATA:     // transmit DSPIN DATA flits for a WRITE 
		    if ( p_vci.cmdval.read() and 
                 p_dspin_cmd.read.read() and
                 p_vci.eop.read() )
            {
                r_cmd_fsm = CMD_IDLE;
            } 
        break;
        }       // end switch 32 bits
    }
    else ////////////////////////////////////// VCI DATA = 64 bits //////////
    {
        switch( r_cmd_fsm.read() )
        {
	    case CMD_IDLE:        // transmit first DSPIN CMD flit 
		    if ( p_vci.cmdval.read() and p_dspin_cmd.read.read() )
            {
                assert( ((p_vci.cmd.read() != vci_param::CMD_LOCKED_READ) and
                         (p_vci.cmd.read() != vci_param::CMD_STORE_COND)) and
                "ERROR in VCI/DSPIN wrapper 64 bit:  LL and SC not supported");

                if ( p_vci.cmd.read() == vci_param::CMD_WRITE )
                {
                    r_cmd_fsm = CMD_WDATA;
                }
                else // cmd = vci_param::CMD_READ
                {
                    r_cmd_fsm = CMD_READ;
                }

            }
        break;
        case CMD_READ:        // send CMDACK on VCI port in case of READ
		    if ( p_dspin_cmd.read.read() )
            {
                r_cmd_fsm = CMD_IDLE;
            }
        break;
        case CMD_WDATA:      // transmit DSPIN DATA flits for a WRITE
		    if ( p_vci.cmdval.read() and 
                 p_dspin_cmd.read.read() and
                 p_vci.eop.read() )
            {
                r_cmd_fsm = CMD_IDLE;
            } 
        break;
        }      // end switch 64 bits
    }  

    /////////////////////////////////////////////////////////////////
    // DSPIN response packet to VCI response packet
    /////////////////////////////////////////////////////////////////
    // This FSM has the same structure for VCI DATA 32 and 64 bits.
    // - A N+1 flits DSPIN response packet is translated
    //   to a N flits VCI response.
    // - A single flit DSPIN response packet is translated
    //   to a single flit VCI response with RDATA = 0.
    // A valid DSPIN flit is always consumed in the CMD_IDLE 
    // state, but no VCI flit is transmitted.
    // The VCI flits are sent in the RSP_READ & RSP_WRITE states.
    /////////////////////////////////////////////////////////////////

    switch( r_rsp_fsm.read() )
    {
    case RSP_IDLE:     // try to transmit VCI flit if  WRITE
        if ( p_dspin_rsp.write.read() )  
        {
            r_rsp_buf = p_dspin_rsp.data.read();
            if ( not p_dspin_rsp.eop.read() )    r_rsp_fsm = RSP_READ; 
            else if ( not p_vci.rspack.read() )  r_rsp_fsm = RSP_WRITE;
        }
    break;
    case RSP_READ:    // try to transmit a flit VCI for a READ
        if ( p_vci.rspack.read() and 
             p_dspin_rsp.write.read() and
             p_dspin_rsp.eop.read() )       r_rsp_fsm = RSP_IDLE;
    break;
    case RSP_WRITE:    // try to transmit a VCI flit for a WRITE
        if ( p_vci.rspack.read() ) r_rsp_fsm = RSP_IDLE;
    break;
    }         // end switch

}  // end transition

//////////////////////////////
tmpl(void)::genMealy_vci_cmd()
{
    if ( (r_cmd_fsm.read() == CMD_IDLE) or
         (r_cmd_fsm.read() == CMD_WRITE) ) p_vci.cmdack = false;
    else                                   p_vci.cmdack = p_dspin_cmd.read.read();
}
////////////////////////////////
tmpl(void)::genMealy_dspin_cmd()
{
    if ( vci_param::B == 4 ) //////////////// dspin flit = 39 bits
    {
        sc_uint<39> dspin_data;

        if ( r_cmd_fsm.read() == CMD_IDLE )     // first header flit
        {
            dspin_data = (sc_uint<39>)((p_vci.address.read()>>2)<<(41-vci_param::N));

            p_dspin_cmd.write = p_vci.cmdval.read();
            p_dspin_cmd.data  = dspin_data;
            p_dspin_cmd.eop   = false;
        }
        else if ( (r_cmd_fsm.read() == CMD_READ) or
                  (r_cmd_fsm.read() == CMD_WRITE) )    // second header flit
        {
            sc_uint<39> be      = ((sc_uint<39>)p_vci.be.read())<<1;
            sc_uint<39> pktid   = ((sc_uint<39>)p_vci.pktid.read())<<5;
            sc_uint<39> trdid   = ((sc_uint<39>)p_vci.trdid.read())<<9;
            sc_uint<39> plen    = ((sc_uint<39>)p_vci.plen.read())<<13;
            sc_uint<39> cons    = ((sc_uint<39>)p_vci.cons.read())<<21;
            sc_uint<39> contig  = ((sc_uint<39>)p_vci.contig.read())<<22;
            sc_uint<39> cmd     = ((sc_uint<39>)p_vci.cmd.read())<<23;
            sc_uint<39> srcid   = ((sc_uint<39>)p_vci.srcid.read())<<(39-m_srcid_width);

            dspin_data = (be     & 0x000000001ELL) |
                         (pktid  & 0x00000001E0LL) |
                         (trdid  & 0x0000001E00LL) |
                         (plen   & 0x00001FE000LL) |
                         (cons   & 0x0000200000LL) |
                         (contig & 0x0000400000LL) |
                         (cmd    & 0x0001800000LL) |
                         (srcid  & 0x7FFE000000LL) ;  // SRCID left aligned

            p_dspin_cmd.write = p_vci.cmdval.read();
            p_dspin_cmd.data  = dspin_data;
            p_dspin_cmd.eop   = ( r_cmd_fsm.read() == CMD_READ );
        }
        else    // r_cmd_fsm == CMD_WDATA  =>  data flit
        {
            sc_uint<39> wdata = (sc_uint<39>)p_vci.wdata.read();
            sc_uint<39> be    = (sc_uint<39>)p_vci.be.read();
            dspin_data =  (wdata      & 0x00FFFFFFFFLL) |
                          ((be << 32) & 0x0F00000000LL) ;

            p_dspin_cmd.write = p_vci.cmdval.read();
            p_dspin_cmd.data  = dspin_data;
            p_dspin_cmd.eop   = p_vci.eop.read();
        }
    }
    else     /////////////////////// dspin flit = 64 bits
    {
        sc_uint<64> dspin_data;

        if ( r_cmd_fsm.read() == CMD_IDLE )     // header flit
        {
            sc_uint<64> address = ((sc_uint<64>)p_vci.address.read())<<(64-vci_param::N);
            sc_uint<64> wlen    = ((((sc_uint<64>)p_vci.plen.read())>>2)-1)<<20;
            sc_uint<64> cmd     = ((sc_uint<64>)p_vci.cmd.read())<<18;
            sc_uint<64> srcid   = ((sc_uint<64>)p_vci.srcid.read())<<4;
            sc_uint<64> trdid   = ((sc_uint<64>)p_vci.trdid.read());

            dspin_data = (trdid   & 0x000000000000000FLL) |
                         (srcid   & 0x000000000003FFF0LL) |
                         (cmd     & 0x00000000000C0000LL) |
                         (wlen    & 0x0000000000F00000LL) |
                         (address & 0xFFFFFFFFFC000000LL) ;

            p_dspin_cmd.write = p_vci.cmdval.read();
            p_dspin_cmd.data  = (sc_uint<64>)dspin_data;
            p_dspin_cmd.eop   = ( p_vci.cmd.read() == vci_param::CMD_READ );
        }
        else if ( r_cmd_fsm.read() == CMD_WDATA )     //  data flit
        {
            p_dspin_cmd.write = p_vci.cmdval.read();
            p_dspin_cmd.data  = (sc_uint<64>)p_vci.wdata.read();
            p_dspin_cmd.eop   = p_vci.eop.read();
        }
        else    // CMD_READ => no flit transmited
        {
            p_dspin_cmd.write = false;
            p_dspin_cmd.data  = 0;
            p_dspin_cmd.eop   = false;
        }
    }
}

////////////////////////////////
tmpl(void)::genMealy_dspin_rsp()
{
    if      ( r_rsp_fsm.read() == RSP_IDLE )   p_dspin_rsp.read = true;
    else if ( r_rsp_fsm.read() == RSP_READ )   p_dspin_rsp.read = p_vci.rspack.read();
    else                                       p_dspin_rsp.read = false;
}
//////////////////////////////
tmpl(void)::genMealy_vci_rsp()
{
    if ( vci_param::B == 4 ) //////////////// dspin flit = 32 bits
    {
        if ( r_rsp_fsm.read() == RSP_IDLE )  
        {
            p_vci.rspval = p_dspin_rsp.write.read() and p_dspin_rsp.eop.read();
            p_vci.rdata  = 0;
            p_vci.rsrcid = (sc_uint<vci_param::S>)((p_dspin_rsp.data.read() & 0xFFFC0000) >> (32-m_srcid_width));
            p_vci.rpktid = (sc_uint<vci_param::T>)((p_dspin_rsp.data.read() & 0x00000F00) >> 8);
            p_vci.rtrdid = (sc_uint<vci_param::P>)((p_dspin_rsp.data.read() & 0x0000F000) >> 12);
            p_vci.rerror = (sc_uint<vci_param::E>)((p_dspin_rsp.data.read() & 0x00030000) >> 16);
            p_vci.reop   = true;
        }
        else if ( r_rsp_fsm == RSP_READ )
        {
            p_vci.rspval = p_dspin_rsp.write.read();
            p_vci.rdata  = (sc_uint<32>)(p_dspin_rsp.data.read());
            p_vci.rsrcid = (sc_uint<vci_param::S>)((r_rsp_buf.read() & 0xFFFC0000) >> (32-m_srcid_width));
            p_vci.rpktid = (sc_uint<vci_param::T>)((r_rsp_buf.read() & 0x00000F00) >> 8);
            p_vci.rtrdid = (sc_uint<vci_param::P>)((r_rsp_buf.read() & 0x0000F000) >> 12);
            p_vci.rerror = (sc_uint<vci_param::E>)((r_rsp_buf.read() & 0x00030000) >> 16);
            p_vci.reop   = p_dspin_rsp.eop.read();
        }
        else //  r_rsp_fsm == RSP_WRITE
        {
            p_vci.rspval = true;
            p_vci.rdata  = 0;
            p_vci.rsrcid = (sc_uint<vci_param::S>)((r_rsp_buf.read() & 0xFFFC0000) >> (32-m_srcid_width));
            p_vci.rpktid = (sc_uint<vci_param::T>)((r_rsp_buf.read() & 0x00000F00) >> 8);
            p_vci.rtrdid = (sc_uint<vci_param::P>)((r_rsp_buf.read() & 0x0000F000) >> 12);
            p_vci.rerror = (sc_uint<vci_param::E>)((r_rsp_buf.read() & 0x00030000) >> 16);
            p_vci.reop   = true;
        }
    }
    else     ////////////////////////////// dspin flit = 64 bits
    {
        if ( r_rsp_fsm.read() == RSP_IDLE )  
        {
            p_vci.rspval = p_dspin_rsp.write.read() and p_dspin_rsp.eop.read();
            p_vci.rdata  = 0;
            p_vci.rsrcid = (sc_uint<vci_param::S>)((p_dspin_rsp.data.read() & 0xFFFC000000000000LL) >> (64-m_srcid_width));
            p_vci.rtrdid = (sc_uint<vci_param::P>)((p_dspin_rsp.data.read() & 0x000000000000000FLL));
            p_vci.rerror = (sc_uint<vci_param::E>)((p_dspin_rsp.data.read() & 0x0000000000000030LL) >> 4);
            p_vci.reop   = true;
        }
        else if ( r_rsp_fsm == RSP_READ )
        {
            p_vci.rspval = p_dspin_rsp.write.read();
            p_vci.rdata  = (sc_uint<64>)(p_dspin_rsp.data.read());
            p_vci.rsrcid = (sc_uint<vci_param::S>)((r_rsp_buf.read() & 0xFFFC000000000000LL) >> (64-m_srcid_width));
            p_vci.rtrdid = (sc_uint<vci_param::P>)((r_rsp_buf.read() & 0x000000000000000FLL));
            p_vci.rerror = (sc_uint<vci_param::E>)((r_rsp_buf.read() & 0x0000000000000030LL) >> 4);
            p_vci.reop   = p_dspin_rsp.eop.read();
        }
        else //  r_rsp_fsm == RSP_WRITE
        {
            p_vci.rspval = true;
            p_vci.rdata  = 0;
            p_vci.rsrcid = (sc_uint<vci_param::S>)((r_rsp_buf.read() & 0xFFFC000000000000LL) >> (64-m_srcid_width));
            p_vci.rtrdid = (sc_uint<vci_param::P>)((r_rsp_buf.read() & 0x000000000000000FLL));
            p_vci.rerror = (sc_uint<vci_param::E>)((r_rsp_buf.read() & 0x0000000000000030LL) >> 4);
            p_vci.reop   = true;
        }
    }
}
/////////////////////////
tmpl(void)::print_trace()
{
    const char* cmd_str[] = { "IDLE", "READ", "WRITE", "WDATA" };
    const char* rsp_str[] = { "IDLE", "READ", "WRITE" };
    std::cout << "VCI_DSPIN_INI_WRAPPER " << name()
              << " : cmd_fsm = " << cmd_str[r_cmd_fsm.read()]
              << " / rsp_fsm = " << rsp_str[r_rsp_fsm.read()] << std::endl; 
}

}} // end namespace

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

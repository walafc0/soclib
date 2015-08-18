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
 * Copyright (C) IRISA/INRIA, 2007-2008
 *         Francois Charot <charot@irisa.fr>
 * 	   Charles Wagner <wagner@irisa.fr>
 * 
 * Maintainer: wagner
 * 
 * File : avalon_switch_master.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_SWITCH_MASTER_H_
#define SOCLIB_CABA_AVALON_SWITCH_MASTER_H_

#include <systemc>
#include "avalonbus_master.h"
#include "avalon_switch_slave.h"
#include "avalon_mux_signals.h"
#include "avalon_param.h"


namespace soclib { namespace caba {

    using namespace sc_core;

    template< typename avalon_param >

    class AvalonSwitchMaster {   

    public:                                                 //  FUNDAMENTALS SIGNALS 

      //   0, 1 or multiple fixed wait-states
      sc_in<bool>                                            clk;                //
      sc_in<sc_dt::sc_uint<avalon_param::address_width> >    address;            //  1-32
      sc_in<bool>                                            read;               //  if used readdata or data must be used
      sc_out<sc_dt::sc_uint<avalon_param::data_width> >      readdata;           //  if used data cannot be used
      sc_in<bool>                                            write;              //  if used writedata or data must also be used and writebyteenable cannot be used
      sc_in<sc_dt::sc_uint<avalon_param::data_width> >       writedata;          //  if used write or writebyteenable must also be used and datra cannot be used
      sc_in<sc_dt::sc_uint<avalon_param::data_width/8 > >    byteenable;         //  if used writedata and writebyteenable cannot be used

      //  PIPELINE SIGNALS
      //       sc_in<bool>                                            flush;              //  
      sc_out<bool>                                           waitrequest;        //  variable wait-states
      sc_out<bool>                                           readdatavalid;      // pipelined read with VARIABLE latency

      //  BURST SIGNALS
      sc_in<sc_dt::sc_uint<avalon_param::burstcount_width> > burstcount;         //  2-32 

      //  FLOW CONTROL SIGNALS
      //       sc_out<bool>                                           endofpacket;        //

      //  TRISTATE SIGNALS
      //sc_inout<sc_dt::sc_uint<Data_Width> >  data;               //  1-128; if used readdata nd write data cannot be used
      //sc_in<sc_dt::sc_uint<Data_Width> >     data;               //  1-128; if used readdata nd write data cannot be used

      //  OTHER SIGNALS
      // si irq sur 1 bits irqnum sur 6 bits    
      // si irq sur 1-32  bits pas de irqnum                                                                  
      //       sc_out<bool>                             irq;                // 1 ou 32 bit
      //       sc_out<sc_dt::sc_uint<6> >               irqnumber;          //  avalon master only
      //       sc_out<bool>                             reset;              //
      //       sc_in<bool>                              resetrequest;       //      

#define ren(x) x(((std::string)(name_ + "_"#x)).c_str())

      AvalonSwitchMaster(std::string name_ = (std::string)sc_gen_unique_name("avalon_switch_master"))
	: ren(clk),
	  ren(address), 
	  ren(read),  
	  ren(readdata),
	  ren(write),
	  ren(writedata),
	  ren(byteenable),
	  // 	  ren(flush),
	  ren(waitrequest),
	  ren(readdatavalid),
	  ren(burstcount)
	  // 	  ren(endofpacket),    
	  //        ren(data),   
	  // 	  ren(irq),
	  // 	  ren(irqnumber), 
	  // 	  ren(reset),   
	  // 	  ren(resetrequest)  
      {

      }
#undef ren


      void operator () (AvalonSignals< avalon_param> &sig) {
	clk (sig.clk);
	address (sig.address);
	read (sig.read);
	readdata (sig.readdata);
	write (sig.write);
	writedata (sig.writedata);
	byteenable (sig.byteenable);
	// 	flush (sig.flush);
	waitrequest (sig.waitrequest);
	readdatavalid (sig.readdatavalid);
	burstcount (sig.burstcount);
	// 	endofpacket (sig.endofpacket);
	// 	data (sig.data);
	// 	irq (sig.irq);
	// 	irqnumber (sig.irqnumber);
	// 	reset (sig.reset);
	// 	resetrequest (sig.resetrequest); 
	
      }

      void operator () (AvalonSwitchSlave< avalon_param> &sig) {
	sig.readdata(readdata);
	sig.waitrequest(waitrequest);
	sig.readdatavalid(readdatavalid);   
      }

      void operator () (AvalonMuxSignals< avalon_param> &sig) {
	readdata (sig.readdata);
	waitrequest (sig.waitrequest);
	readdatavalid (sig.readdatavalid);
      }
    };
  } // end caba
} // end namespace 
#endif /*AVALON_SWITCH_MASTER_H_*/

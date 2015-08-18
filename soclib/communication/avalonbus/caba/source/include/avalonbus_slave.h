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
 * File : avalonbus_slave.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALONBUS_SLAVE_H_
#define SOCLIB_CABA_AVALONBUS_SLAVE_H_

#include <systemc>
#include "avalon_signals.h"
#include "avalon_param.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template< typename avalon_param >


    class AvalonSlave {   

    public:
      //  FUNDAMENTALS SIGNALS
      sc_in<bool>                                            clk;                //
      sc_in<bool>                                            chipselect;         //  avalon slave only
      sc_in<sc_dt::sc_uint<avalon_param::address_width> >    address;            //  1-32
      sc_in<bool>                                            read;               //  if used readdata or data must be used
      sc_out<sc_dt::sc_uint<avalon_param::data_width> >      readdata;           //  if used data cannot be used
      sc_in<bool>                                            write;              //  if used writedata or data must also be used and writebyteenable cannot be used
      sc_in<sc_dt::sc_uint<avalon_param::data_width> >       writedata;          //  if used write or writebyteenable must also be used and datra cannot be used
      sc_in<sc_dt::sc_uint<avalon_param::data_width/8> >     byteenable;         //  if used writedata and writebyteenable cannot be used
//       sc_in<sc_dt::sc_uint<avalon_param::Data_Width/8> >     writebyteenable;    //  avalon slave only; if used writedata must also be used. write and byteenable cannot be used
//       sc_in<bool>                                            begintransfer;      //  avalon slave only

      //  WAIT-STATE SIGNALS
      sc_out<bool>                                           waitrequest;        //

      //  PIPELINE SIGNALS
      sc_out<bool>                                           readdatavalid;      //

      //  BUIRST SIGNALS
      sc_in<sc_dt::sc_uint<avalon_param::burstcount_width> > burstcount;          //  2-32
//       sc_in<bool>                                            beginbursttransfer;  //  avalon slave only

      //  FLOW CONTROL SIGNALS
//       sc_out<bool>                                           readyfordata;        // 
//       sc_out<bool>                                           dataavailable;       // 
//       sc_out<bool>                                           endofpacket;         //

      //  TRISTATE SIGNALS
//       sc_inout<sc_dt::sc_uint<avalon_param::Data_Width> >    data;                //  1-128; if used readdata nd write data cannot be used
//       sc_in<bool>                                            outputenable;        // 

//       //  OTHER SIGNALS
//       sc_out<bool>                                           irq;                 //
//       sc_in<bool>                                            reset;               //
//       sc_out<bool>                                           resetrequest;        //             

#define ren(x) x(((std::string)(name_ + "_"#x)).c_str())

      AvalonSlave(std::string name_ = (std::string)sc_gen_unique_name("avalon_slave"))
	: ren(clk),
	  ren(chipselect),
	  ren(address), 
	  ren(read),  
	  ren(readdata),
	  ren(write),
	  ren(writedata),
	  ren(byteenable),
// 	  ren(writebyteenable),
// 	  ren(begintransfer),
	  ren(waitrequest),
	  ren(readdatavalid),    
	  ren(burstcount)
// 	  ren(beginbursttransfer),
// 	  ren(readyfordata), 
// 	  ren(dataavailable),   
// 	  ren(endofpacket),  
// 	  ren(data),  
// 	  ren(outputenable),
// 	  ren(irq),
// 	  ren(reset), 
// 	  ren(resetrequest) 
      {
      }
#undef ren

      void operator () (AvalonSlave< avalon_param> &ports) {
	clk (ports.clk);
	chipselect (ports.chipselect);
	address (ports.address);
	read (ports.read);
	readdata (ports.readdata);
	write (ports.write);
	writedata (ports.writedata);
	byteenable (ports.byteenable);
// 	writebyteenable (ports.writebyteenable);
// 	begintransfer (ports.begintransfer);
	waitrequest (ports.waitrequest);
	readdatavalid (ports.readdatavalid);
	burstcount (ports.burstcount);
// 	beginbursttransfer (ports.beginbursttransfer);
// 	readyfordata (ports.readyfordata);
// 	dataavailable (ports.dataavailable);
// 	endofpacket (ports.endofpacket);
// 	data (ports.data);
// 	outputenable (ports.outputenable);
// 	irq (ports.irq);
// 	reset (ports.reset);
// 	resetrequest (ports.resetrequest);     
      }


      void operator () (AvalonSignals< avalon_param> &sig) {
	clk (sig.clk);
	chipselect (sig.chipselect);
	address (sig.address);
	read (sig.read);
	readdata (sig.readdata);
	write (sig.write);
	writedata (sig.writedata);
	byteenable (sig.byteenable);
// 	writebyteenable (sig.writebyteenable);
// 	begintransfer (sig.begintransfer);
	waitrequest (sig.waitrequest);
	readdatavalid (sig.readdatavalid);
	burstcount (sig.burstcount);
// 	beginbursttransfer (sig.beginbursttransfer);
// 	readyfordata (sig.readyfordata);
// 	dataavailable (sig.dataavailable);
// 	endofpacket (sig.endofpacket);
// 	data (sig.data);
// 	outputenable (sig.outputenable);
// 	irq (sig.irq);
// 	reset (sig.reset);
// 	resetrequest (sig.resetrequest);    
	
      }
    };

  }
}
#endif /*AVALONBUS_SLAVE_H_*/

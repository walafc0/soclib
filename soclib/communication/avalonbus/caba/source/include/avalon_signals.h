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
 * File : avalon_signals.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_SIGNALS_H_
#define SOCLIB_CABA_AVALON_SIGNALS_H_

#include <systemc>
#include "avalon_param.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template< typename avalon_param >

    class AvalonSignals {   

    public:
      // signals 
      //#define      ByteEnable_Width  (Data_Width / 4);
      //  FUNDAMENTALS SIGNALS
      sc_signal<bool>                                            clk;                //
      sc_signal<bool>                                            chipselect;         //  avalon slave only
      sc_signal<sc_dt::sc_uint<avalon_param::address_width> >    address;            //  1-32
      sc_signal<bool>                                            read;               //  if used readdata or data must be used
      sc_signal<sc_dt::sc_uint<avalon_param::data_width> > 	   readdata;           //  1-1024if used data cannot be used
      sc_signal<bool>                                            write;              //  if used writedata or data must also be used and writebyteenable cannot be used
      sc_signal<sc_dt::sc_uint<avalon_param::data_width> >       writedata;          //  1-1024if used write or writebyteenable must also be used and datra cannot be used
      sc_signal<sc_dt::sc_uint<avalon_param::data_width / 8> >   byteenable;         //  if used writedata and writebyteenable cannot be used
      //       sc_signal<sc_dt::sc_uint<avalon_param::Data_Width / 8> >   writebyteenable;    //  avalon slave only; if u
      //       sed writedata must also be used. write and byteenable cannot be used
      //       sc_signal<bool>                                            begintransfer;      //  avalon slave only
      //  WAIT-STATE SIGNALS
      //       sc_signal<bool>                                            flush;              //  avalon master only
      sc_signal<bool>                                            waitrequest;        //                                                               //  PIPELINE SIGNALS
      sc_signal<bool>                                            readdatavalid;      //

      sc_signal<sc_dt::sc_uint<avalon_param::burstcount_width> > burstcount;         //  2-32 

      //  BURST SIGNALS
      //       sc_signal<bool>                                            beginbursttransfer; //  avalon slave only 
      //       sc_signal<bool>                                            readyfordata;       //  avalon slave only
      //       sc_signal<bool>                                            dataavailable;       //  avalon slave only
      //       sc_signal<bool>                                            endofpacket;     
      //
      //  FLOW CONTROL SIGNALS

      //  TRISTATE SIGNALS
      //       sc_signal<sc_dt::sc_uint<avalon_param::Data_Width> >       data;               //  1-1024; if used readdata and write data cannot be used
      //       sc_signal<bool>  							               outputenable;       //  avalon slave only

      // si irq sur 1 bits irqnum sur 6 bits    
      // si irq sur 1-32  bits pas de irqnum                                                                //  OTHER SIGNALS
      //       sc_signal<bool>                                            irq;                //
      //       sc_signal<sc_dt::sc_uint<6> >                              irqnumber;          //  avalon master only
      //       sc_signal<bool>                                            reset;              //
      //       sc_signal<bool>                                            resetrequest;       //             

#define ren(x) x(((std::string)(name_ + "_"#x)).c_str())

      AvalonSignals(std::string name_ = (std::string)sc_gen_unique_name("avalon"))
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
	  // 	  ren(flush),
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
	  // 	  ren(irqnumber),  
	  // 	  ren(reset), 
	  // 	  ren(resetrequest) 
      {
      }
#undef ren

    }; //end AvalonBus
  }
}// end namespace

#endif


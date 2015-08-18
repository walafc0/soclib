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
 * File : avalon_adl_out.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_ADL_OUT_H_
#define SOCLIB_CABA_AVALON_ADL_OUT_H_

#include <systemc>
#include "avalon_adl_signals.h"
#include "avalon_switch_slave.h"
#include "avalon_param.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template< typename avalon_param >

    class AvalonAdlOut {   

    public:
      //  FUNDAMENTALS SIGNALS
      sc_out<bool>                                            chipselect;         //  avalon slave only
      sc_out<sc_dt::sc_uint<avalon_param::address_width> >    address;            //  1-32
      sc_out<bool>                                            read;               //  if used readdata or data must be used
      sc_out<bool>                                            write;              //  if used writedata or data must also be used and writebyteenable cannot be used
      sc_out<sc_dt::sc_uint<avalon_param::data_width> >       writedata;          //  1-1024if used write or writebyteenable must also be used and datra cannot be used
      sc_out<sc_dt::sc_uint<avalon_param::data_width / 8> >   byteenable;         //  if used writedata and writebyteenable cannot be used
      //       sc_out<bool>                                            flush;              // 
      sc_out<sc_dt::sc_uint<avalon_param::burstcount_width> > burstcount;         //  2-32  
            

#define ren(x) x(((std::string)(name_ + "_"#x)).c_str())

      AvalonAdlOut(std::string name_ = (std::string)sc_gen_unique_name("avalon_adl"))
	: ren(chipselect),
	  ren(address), 
	  ren(read),  
	  ren(write),
	  ren(writedata),
	  ren(byteenable),
	  // 	  ren(flush),
	  ren(burstcount)
      {
      }
#undef ren

      void operator () (AvalonSwitchSlave< avalon_param> &sig) {
	chipselect (sig.chipselect);
	address (sig.address);
	read (sig.read);
	write (sig.write);
	writedata (sig.writedata);
	byteenable (sig.byteenable);
	burstcount (sig.burstcount);   
      }

      void operator () (AvalonAdlSignals< avalon_param> &sig) {
	chipselect (sig.chipselect);
	address (sig.address);
	read (sig.read);
	write (sig.write);
	writedata (sig.writedata);
	byteenable (sig.byteenable);
	// 	flush (sig.flush);
	burstcount (sig.burstcount); 
      }

    }; //end AvalonAdlOut
  }
}// end namespace
#endif


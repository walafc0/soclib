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
 * File : avalon_mux_master.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_MUX_MASTER_H_
#define SOCLIB_CABA_AVALON_MUX_MASTER_H_

#include <systemc>
#include "avalon_param.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template< typename avalon_param >

    class AvalonMuxMaster {   

    public: 
      sc_out<sc_dt::sc_uint<avalon_param::data_width> >       readdata;           //  1-1024if used data cannot be used
      sc_out<bool>                                            waitrequest;        //                                                               
      //  PIPELINE SIGNALS
      sc_out<bool>                                            readdatavalid;      // 

#define ren(x) x(((std::string)(name_ + "_"#x)).c_str())

      AvalonMuxMaster(std::string name_ = (std::string)sc_gen_unique_name("avalon_mux_master"))

	: ren(readdata), 
	  ren(waitrequest),  
	  ren(readdatavalid)
      {
      }

#undef ren

    };
  } // end caba
} // end namespace 
#endif /*AVALON_MUX_MASTER_H_*/

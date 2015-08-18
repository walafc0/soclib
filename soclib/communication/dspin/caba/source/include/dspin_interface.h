/* -*- c++ -*-
  * File : dspin_interface.h
  * Copyright (c) UPMC, Lip6
  * Authors : Alain Greiner, Abbas Sheibanyrad, Ivan Miro, Zhen Zhang
  *
  * SOCLIB_LGPL_HEADER_BEGIN
  * SoCLib is free software; you can redistribute it and/or modify it
  * under the terms of the GNU Lesser General Public License as published
  * by the Free Software Foundation; version 2.1 of the License.
  * SoCLib is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  * You should have received a copy of the GNU Lesser General Public
  * License along with SoCLib; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  * 02110-1301 USA
  * SOCLIB_LGPL_HEADER_END
  */

#ifndef SOCLIB_DSPIN_INTERFACE_H_
#define SOCLIB_DSPIN_INTERFACE_H_

#include <systemc>
#include "static_assert.h"

namespace soclib { namespace caba {

using namespace sc_core;
using namespace sc_dt;


/***  DSPIN SIGNALS  ***/
template<int dspin_data_size>
class DspinSignals 
{
    public:
        sc_signal<sc_uint<dspin_data_size> >    data;    // data
        sc_signal<bool>                 		eop;     // end of packet
        sc_signal<bool>                 		write;   // write command 
        sc_signal<bool>                 		read;    // read command

#define __ren(x) x((insname+"_" #x).c_str())
        DspinSignals(std::string insname = sc_gen_unique_name("dspin_signals"))
            : __ren(data),
              __ren(eop),
              __ren(write),
              __ren(read)
        {}
#undef __ren

        /////////////////////////////////////////////////////////////////
        void trace( sc_core::sc_trace_file* tf, const std::string &name )
        {
#define __trace(x) sc_core::sc_trace(tf, x, name+"_"+#x)
            __trace(data);
            __trace(eop); 
            __trace(write);
            __trace(read); 
#undef __trace
        }

        //////////////////////////////////
        void print_trace(std::string name)
        {
            if ( write )
            std::cout << name << " DSPIN"
                      << " : data = " << std::hex << data 
                      << " | eop = " << eop
                      << " | ack = " << read << std::endl;
        }
};


/***  DSPIN OUT Ports ***/
template<int dspin_data_size>
struct DspinOutput 
{
    sc_out<sc_uint<dspin_data_size> >   data;    // data
    sc_out<bool>                     	eop;     // end of packet
    sc_out<bool>                    	write;   // valid data
    sc_in<bool>                     	read;    // data accepted

    void operator () (DspinSignals<dspin_data_size> &sig) 
    {
	    data         (sig.data);
        eop          (sig.eop);
	    write        (sig.write);
	    read         (sig.read);
    };

    void operator () (DspinOutput<dspin_data_size> &port) 
    {
	    data         (port.data);
        eop          (port.eop);
	    write        (port.write);
	    read         (port.read);
    };

#define __ren(x) x((name+"_" #x).c_str())
    DspinOutput(const std::string &name = sc_gen_unique_name("dspin_output"))
	: __ren(data),
	  __ren(eop),
	  __ren(write),
	  __ren(read)
    {}
#undef __ren

}; 


/*** DSPIN IN Ports ***/
template<int dspin_data_size>
struct DspinInput 
{
    sc_in<sc_uint<dspin_data_size> >    data;     // data
    sc_in<bool>                     	eop;      // end of packet
    sc_in<bool>                    		write;    // valid data
    sc_out<bool>                   		read;     // data accepted

    void operator () (DspinSignals<dspin_data_size> &sig) 
    {
	    data         (sig.data);
        eop          (sig.eop);
	    write        (sig.write);
	    read         (sig.read);
    };

    void operator () (DspinInput<dspin_data_size> &port) 
    {
	    data         (port.data);
        eop          (port.eop);
	    write        (port.write);
	    read         (port.read);
    };

#define __ren(x) x((name+"_" #x).c_str())
    DspinInput(const std::string &name = sc_gen_unique_name("dspin_input"))
	: __ren(data),
	  __ren(eop),
	  __ren(write),
	  __ren(read)
    {}
#undef __ren
};

}}

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

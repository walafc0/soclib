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
 * Copyright (c) Telecom ParisTech
 *         Tarik Graba <tarik.graba@telecom-paristech.fr>, 2009
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 */

#ifndef SOCLIB_CABA_WB_SIGNAL_H_
#define SOCLIB_CABA_WB_SIGNAL_H_

#include <systemc>
#include "wb_param.h"

namespace soclib { namespace caba {

    /*** wishbone signal ***/

    template <typename wb_param>
        class WbSignal {
            public:
                sc_core::sc_signal<typename wb_param::wb_data_t>      MWDAT;    // masters output data
                sc_core::sc_signal<typename wb_param::wb_data_t>      MRDAT;    // masters input data
                sc_core::sc_signal<typename wb_param::wb_add_t>       ADR;      // master output address 
                sc_core::sc_signal<bool>                              ACK;      // Acknowledge from slave
                sc_core::sc_signal<bool>                              CYC;      // cycle valid
                sc_core::sc_signal<bool>                              ERR;      // error from slave
                sc_core::sc_signal<bool>                              LOCK;     // lock request
                sc_core::sc_signal<bool>                              RTY;      // retry from slave
                sc_core::sc_signal<typename wb_param::wb_sel_t>       SEL;      // BE
                sc_core::sc_signal<bool>                              STB;      // commande valid
                sc_core::sc_signal<bool>                              WE;       // write enable


#define __ren(x) x((name+"_" #x).c_str())
                WbSignal ( const std::string &name = sc_gen_unique_name("wb_signal"))
                    : __ren(MWDAT),
                    __ren(MRDAT),
                    __ren(ADR),
                    __ren(ACK),
                    __ren(CYC),
                    __ren(ERR),
                    __ren(LOCK),
                    __ren(RTY),
                    __ren(SEL),
                    __ren(STB),
                    __ren(WE)
            {
            }
#undef  __ren

                void trace( sc_core::sc_trace_file* tf, const std::string &name ) const
                {
#define __trace(x) sc_core::sc_trace(tf, x, name+"_"+#x)
                    __trace(MWDAT);
                    __trace(MRDAT);
                    __trace(ADR);
                    __trace(ACK);
                    __trace(CYC);
                    __trace(ERR);
                    __trace(LOCK);
                    __trace(RTY);
                    __trace(SEL);
                    __trace(STB);
                    __trace(WE);
#undef __trace
                }

                friend std::ostream &operator << (std::ostream &o, const WbSignal &_signal)
                {
                    _signal.print(o);
                    return o;
                }

                void print( std::ostream &o ) const
                {
                    o   << "WbSignal"<< std::hex << std::endl
                        << " MWDAT " << MWDAT << std::endl 
                        << " MRDAT " << MRDAT << std::endl 
                        << " ADR   " << ADR   << std::endl 
                        << " ACK   " << ACK   << std::endl 
                        << " CYC   " << CYC   << std::endl 
                        << " ERR   " << ERR   << std::endl 
                        << " LOCK  " << LOCK  << std::endl
                        << " RTY   " << RTY   << std::endl 
                        << " SEL   " << SEL   << std::endl 
                        << " STB   " << STB   << std::endl 
                        << " WE    " << WE    << std::endl;      
                }

        };

}}

namespace sc_core {
    // sc_trace function
    template <typename wb_param>
    void sc_trace( sc_core::sc_trace_file* tf, const soclib::caba::WbSignal<wb_param>& ws, const std::string& name )
    {
        ws.trace(tf, name);
    }
}

#endif //SOCLIB_CABA_WB_SIGNAL_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

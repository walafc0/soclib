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

#ifndef SOCLIB_CABA_WB_SLAVE_H_
#define SOCLIB_CABA_WB_SLAVE_H_

#include <systemc>
#include "wb_param.h"
#include "wb_signal.h"

namespace soclib { namespace caba {

    /*** wishbone slave port ***/

    template <typename wb_param>
        class WbSlave  {
            public:
                sc_core::sc_in <typename wb_param::wb_data_t>       DAT_I;      
                sc_core::sc_out<typename wb_param::wb_data_t>       DAT_O;      
                sc_core::sc_in <typename wb_param::wb_add_t>        ADR_I;      
                sc_core::sc_out<bool>                               ACK_O;      
                sc_core::sc_in <bool>                               CYC_I;      
                sc_core::sc_out<bool>                               ERR_O;      
                sc_core::sc_in <bool>                               LOCK_I;     
                sc_core::sc_out<bool>                               RTY_O;      
                sc_core::sc_in <typename wb_param::wb_sel_t>        SEL_I;      
                sc_core::sc_in <bool>                               STB_I;      
                sc_core::sc_in <bool>                               WE_I;       

#define __ren(x) x((name+"_" #x).c_str())
                WbSlave  ( const std::string &name = sc_gen_unique_name("wb_slave"))
                    : __ren(DAT_I),
                    __ren(DAT_O),
                    __ren(ADR_I),
                    __ren(ACK_O),
                    __ren(CYC_I),
                    __ren(ERR_O),
                    __ren(LOCK_I),
                    __ren(RTY_O),
                    __ren(SEL_I),
                    __ren(STB_I),
                    __ren(WE_I)
            {
            }
#undef  __ren


                // binds port to signal using () operator
                void operator () (WbSignal<wb_param> &_signal) {
                    DAT_I   (_signal.MWDAT) ; //write data as input 
                    DAT_O   (_signal.MRDAT) ; //read data as output
                    ADR_I   (_signal.ADR  ) ; 
                    ACK_O   (_signal.ACK  ) ; 
                    CYC_I   (_signal.CYC  ) ; 
                    ERR_O   (_signal.ERR  ) ; 
                    LOCK_I  (_signal.LOCK ) ;
                    RTY_O   (_signal.RTY  ) ; 
                    SEL_I   (_signal.SEL  ) ; 
                    STB_I   (_signal.STB  ) ; 
                    WE_I    (_signal.WE   ) ;  
                };// end of operator

                // binds port to portusing () operator
                void operator () (WbSlave<wb_param> &_sport) {
                    DAT_I   (_sport.DAT_I  ) ; 
                    DAT_O   (_sport.DAT_O  ) ;
                    ADR_I   (_sport.ADR_I  ) ; 
                    ACK_O   (_sport.ACK_O  ) ; 
                    CYC_I   (_sport.CYC_I  ) ; 
                    ERR_O   (_sport.ERR_O  ) ; 
                    LOCK_I  (_sport.LOCK_I ) ;
                    RTY_O   (_sport.RTY_O  ) ; 
                    SEL_I   (_sport.SEL_I  ) ; 
                    STB_I   (_sport.STB_I  ) ; 
                    WE_I    (_sport.WE_I   ) ;  
                };// end of operator


                friend sc_core::sc_sensitive &operator <<(
                        sc_core::sc_sensitive &ss,
                        WbSlave<wb_param> &sig )
                {
                    ss  << sig.DAT_I
                        << sig.ADR_I
                        << sig.CYC_I
                        << sig.LOCK_I
                        << sig.SEL_I
                        << sig.STB_I
                        << sig.WE_I;
                    return ss;
                }

                friend std::ostream &operator << (std::ostream &o, const WbSlave &_sport)
                {
                    _sport.print(o);
                    return o;
                }

                void print( std::ostream &o ) const
                {
                    o   << "WbSlave " << std::hex << std::endl
                        << " DAT_I  " << DAT_I  << std::endl 
                        << " DAT_O  " << DAT_O  << std::endl 
                        << " ADR_I  " << ADR_I  << std::endl 
                        << " ACK_O  " << ACK_O  << std::endl 
                        << " CYC_I  " << CYC_I  << std::endl 
                        << " ERR_O  " << ERR_O  << std::endl 
                        << " LOCK_I " << LOCK_I << std::endl
                        << " RTY_O  " << RTY_O  << std::endl 
                        << " SEL_I  " << SEL_I  << std::endl 
                        << " STB_I  " << STB_I  << std::endl 
                        << " WE_I   " << WE_I   << std::endl;      
                }

        }; 

}}
#endif //SOCLIB_CABA_WB_SLAVE_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

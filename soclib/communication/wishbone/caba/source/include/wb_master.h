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

#ifndef SOCLIB_CABA_WB_MASTER_H_
#define SOCLIB_CABA_WB_MASTER_H_

#include <systemc>
#include "wb_param.h"
#include "wb_signal.h"

namespace soclib { namespace caba {

    /*** wishbone master port ***/

    template <typename wb_param>
        class WbMaster {
            public:
                sc_core::sc_out<typename wb_param::wb_data_t>        DAT_O;      
                sc_core::sc_in <typename wb_param::wb_data_t>        DAT_I;      
                sc_core::sc_out<typename wb_param::wb_add_t>         ADR_O;      
                sc_core::sc_in <bool>                                ACK_I;      
                sc_core::sc_out<bool>                                CYC_O;      
                sc_core::sc_in <bool>                                ERR_I;      
                sc_core::sc_out<bool>                                LOCK_O;     
                sc_core::sc_in <bool>                                RTY_I;      
                sc_core::sc_out<typename wb_param::wb_sel_t>         SEL_O;      
                sc_core::sc_out<bool>                                STB_O;      
                sc_core::sc_out<bool>                                WE_O;       


#define __ren(x) x((name+"_" #x).c_str())
                WbMaster ( const std::string &name = sc_gen_unique_name("wb_master"))
                    : __ren(DAT_O),
                    __ren(DAT_I),
                    __ren(ADR_O),
                    __ren(ACK_I),
                    __ren(CYC_O),
                    __ren(ERR_I),
                    __ren(LOCK_O),
                    __ren(RTY_I),
                    __ren(SEL_O),
                    __ren(STB_O),
                    __ren(WE_O)
            {
            }
#undef  __ren

                // binds port to signal using () operator
                void operator () (WbSignal<wb_param> &_signal) {
                    DAT_O   (_signal.MWDAT) ; //write as output
                    DAT_I   (_signal.MRDAT) ; //read as input
                    ADR_O   (_signal.ADR  ) ; 
                    ACK_I   (_signal.ACK  ) ; 
                    CYC_O   (_signal.CYC  ) ; 
                    ERR_I   (_signal.ERR  ) ; 
                    LOCK_O  (_signal.LOCK ) ;
                    RTY_I   (_signal.RTY  ) ; 
                    SEL_O   (_signal.SEL  ) ; 
                    STB_O   (_signal.STB  ) ; 
                    WE_O    (_signal.WE   ) ;  
                }; 

                // binds port to port using () operator
                void operator () (WbMaster<wb_param>&_mport) {
                    DAT_O   (_mport.DAT_O ) ; 
                    DAT_I   (_mport.DAT_I ) ; 
                    ADR_O   (_mport.ADR_O ) ; 
                    ACK_I   (_mport.ACK_I ) ; 
                    CYC_O   (_mport.CYC_O ) ; 
                    ERR_I   (_mport.ERR_I ) ; 
                    LOCK_O  (_mport.LOCK_O) ;
                    RTY_I   (_mport.RTY_I ) ; 
                    SEL_O   (_mport.SEL_O ) ; 
                    STB_O   (_mport.STB_O ) ; 
                    WE_O    (_mport.WE_O  ) ;  
                }; 

                friend sc_core::sc_sensitive &operator <<(
                        sc_core::sc_sensitive &ss,
                        WbMaster<wb_param> &sig )
                {
                    ss  << sig.DAT_I
                        << sig.ACK_I
                        << sig.ERR_I
                        << sig.RTY_I;
                    return ss;
                }

                friend std::ostream &operator << (std::ostream &o, const WbMaster &_sport)
                {
                    _sport.print(o);
                    return o;
                }

                void print( std::ostream &o ) const
                {
                    o   << "WbMaster" << std::hex << std::endl
                        << " DAT_O  " << DAT_O  << std::endl 
                        << " DAT_I  " << DAT_I  << std::endl 
                        << " ADR_O  " << ADR_O  << std::endl 
                        << " ACK_I  " << ACK_I  << std::endl 
                        << " CYC_O  " << CYC_O  << std::endl 
                        << " ERR_I  " << ERR_I  << std::endl 
                        << " LOCK_O " << LOCK_O << std::endl
                        << " RTY_I  " << RTY_I  << std::endl 
                        << " SEL_O  " << SEL_O  << std::endl 
                        << " STB_O  " << STB_O  << std::endl 
                        << " WE_O   " << WE_O   << std::endl;      
                }


        }; 

}}
#endif //SOCLIB_CABA_WB_MASTER_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

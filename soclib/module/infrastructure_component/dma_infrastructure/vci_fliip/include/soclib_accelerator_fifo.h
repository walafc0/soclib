/*
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
 * Copyright (c) CITI/INSA, 2009
 * 
 * Authors:
 * 	Ludovic L'Hours <ludovic.lhours@insa-lyon.fr>
 * 	Antoine Fraboulet <antoine.fraboulet@insa-lyon.fr>
 * 	Tanguy Risset <tanguy.risset@insa-lyon.fr>
 * 
 */

#ifndef SOCLIB_ACCELERATOR_FIFO_H
#define SOCLIB_ACCELERATOR_FIFO_H

#include <caba_base_module.h>
#include <generic_fifo.h>
#include "soclib_accelerator_fifoports.h"

#ifdef DEBUG_FIFO
#include <stdio.h>
#define DPRINTF_FIFO(x...) { fprintf(fifo_out,x); }
#else
#define DPRINTF_FIFO(x...) {}
#endif

namespace soclib { namespace caba {

template < unsigned int BITWIDTH, unsigned int SIZE >
class AcceleratorFifo : soclib::caba::BaseModule {

public:
    sc_in< bool > p_clk;
    sc_in< bool > p_resetn;
    ACCELERATOR_FIFO_SIGNALS_IN  < BITWIDTH >  DATA_IN;
    ACCELERATOR_FIFO_SIGNALS_OUT < BITWIDTH >  DATA_OUT;

protected:
    SC_HAS_PROCESS (AcceleratorFifo);
  
public:
    AcceleratorFifo(sc_module_name insname);
    ~AcceleratorFifo ();

private:
    void transition ();
    void genMoore ();

    GenericFifo < sc_uint<BITWIDTH> > fifo;
#ifdef DEBUG_FIFO
    //FILE* fifo_out;
#endif
};

}} // end of namespace soclib::caba

#endif

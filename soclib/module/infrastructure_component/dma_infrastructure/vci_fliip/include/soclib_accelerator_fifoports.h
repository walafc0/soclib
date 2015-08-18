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

#ifndef SOCLIB_ACCELERATOR_FIFOPORTS_H
#define SOCLIB_ACCELERATOR_FIFOPORTS_H

#include <systemc.h>

// //////////////////////////////////////////////////
// input channel data types
// //////////////////////////////////////////////////


template < unsigned short BITWIDTH > 
struct ACCELERATOR_SIGNALS 
{
    sc_signal < bool > WRITE;
    sc_signal < bool > WRITEOK;
    sc_signal < sc_uint < BITWIDTH > >DATA;

};



template < unsigned short BITWIDTH > 
struct ACCELERATOR_SIGNALS_IN
{
    sc_in < bool > WRITE;
    sc_out < bool > WRITEOK;
    sc_in < sc_uint < BITWIDTH > >DATA;
    void operator  () (ACCELERATOR_SIGNALS < BITWIDTH > &signals)
    {
	WRITE (signals.WRITE);
	WRITEOK (signals.WRITEOK);
	DATA (signals.DATA);
    }
};



template < unsigned short BITWIDTH > 
struct ACCELERATOR_SIGNALS_OUT
{
    sc_out < bool > WRITE;
    sc_in < bool > WRITEOK;
    sc_out < sc_uint < BITWIDTH > > DATA;
    void operator  () (ACCELERATOR_SIGNALS < BITWIDTH > &signals)
    {
	WRITE (signals.WRITE);
	WRITEOK (signals.WRITEOK);
	DATA (signals.DATA);
    }
};


// //////////////////////////////////////////////////
//  data types used between MMAlpha input controler and MMAlpha FIFO 
// //////////////////////////////////////////////////



template < unsigned short BITWIDTH > 
struct ACCELERATOR_FIFO_SIGNALS 
{
    sc_signal < bool > WRITE;
    sc_signal < bool > WRITEOK;
    sc_signal < sc_uint < BITWIDTH > >DATA;

	ACCELERATOR_FIFO_SIGNALS(
			std::string name_ = sc_core::sc_gen_unique_name("acc_fifo_sig")) {}
};



template < unsigned short BITWIDTH > 
struct ACCELERATOR_FIFO_SIGNALS_IN 
{
    sc_in  < bool > WRITE;
    sc_out < bool > WRITEOK;
    sc_in  < sc_uint < BITWIDTH > > DATA;

    void operator  () (ACCELERATOR_FIFO_SIGNALS < BITWIDTH > &signals)
    {
	WRITE   (signals.WRITE);
	WRITEOK (signals.WRITEOK);
	DATA    (signals.DATA);
    }
};

template < unsigned short BITWIDTH > 
struct ACCELERATOR_FIFO_SIGNALS_OUT {
    sc_out < bool > WRITE;
    sc_in  < bool > WRITEOK;
    sc_out < sc_uint < BITWIDTH > > DATA;

    void operator  () (ACCELERATOR_FIFO_SIGNALS < BITWIDTH > &signals) {
		WRITE   (signals.WRITE);
		WRITEOK (signals.WRITEOK);
		DATA    (signals.DATA);
    }
};

// //////////////////////////////////////////////////
//  data types used between MMAlpha FIFO  and MMAlpha output controler 
// //////////////////////////////////////////////////

template < unsigned short DATA_BITWIDTH, unsigned int ADDR_BITWIDTH > 
struct ACCELERATOR_CTRL_SIGNALS {
    sc_signal < bool > WRITE;
    sc_signal < bool > WRITEOK;
    sc_signal < sc_uint < DATA_BITWIDTH > > DATA;
    sc_signal < sc_uint < ADDR_BITWIDTH > > ADDR;

	ACCELERATOR_CTRL_SIGNALS(
			std::string name_ = sc_core::sc_gen_unique_name("acc_ctrl_sig")) {}
};

template < unsigned short DATA_BITWIDTH, unsigned int ADDR_BITWIDTH > 
struct ACCELERATOR_CTRL_SIGNALS_IN
{
    sc_in  < bool > WRITE;
    sc_out < bool > WRITEOK;
    sc_in  < sc_uint < DATA_BITWIDTH > > DATA;
    sc_in  < sc_uint < ADDR_BITWIDTH > > ADDR;

    void operator  () (ACCELERATOR_CTRL_SIGNALS < DATA_BITWIDTH, ADDR_BITWIDTH > &signals)
    {
	WRITE   (signals.WRITE);
	WRITEOK (signals.WRITEOK);
	DATA    (signals.DATA);
	ADDR    (signals.ADDR);
    }
};



template < unsigned short DATA_BITWIDTH, unsigned short ADDR_BITWIDTH > 
struct ACCELERATOR_CTRL_SIGNALS_OUT {
    sc_out < bool > WRITE;
    sc_in  < bool > WRITEOK;
    sc_out < sc_uint < DATA_BITWIDTH > > DATA;
    sc_out < sc_uint < ADDR_BITWIDTH > > ADDR;

    void operator() (ACCELERATOR_CTRL_SIGNALS < DATA_BITWIDTH, ADDR_BITWIDTH > &signals) {
		WRITE   (signals.WRITE);
		WRITEOK (signals.WRITEOK);
		DATA    (signals.DATA);
		ADDR    (signals.ADDR);
    }
};


// //////////////////////////////////////////////////
//  data types used between VCI interface and CLOCKENABLE 
// //////////////////////////////////////////////////



template < unsigned short DATA_BITWIDTH, unsigned int ADDR_BITWIDTH > 
struct ACCELERATOR_ENBL_SIGNALS 
{
    sc_signal < bool > WRITE;
    sc_signal < bool > WRITEOK;
    sc_signal < bool > FRIP;
    sc_signal < sc_uint < DATA_BITWIDTH > > DATA;
    sc_signal < sc_uint < ADDR_BITWIDTH > > ADDR;

	ACCELERATOR_ENBL_SIGNALS(
			std::string name_ = sc_core::sc_gen_unique_name("acc_enbl_sig")) {}
};



template < unsigned short DATA_BITWIDTH, unsigned int ADDR_BITWIDTH > 
struct ACCELERATOR_ENBL_SIGNALS_IN
{
    sc_in  < bool > WRITE;
    sc_out < bool > WRITEOK;
    sc_in  < bool > FRIP;
    sc_in  < sc_uint < DATA_BITWIDTH > > DATA;
    sc_in  < sc_uint < ADDR_BITWIDTH > > ADDR;

    void operator  () (ACCELERATOR_ENBL_SIGNALS < DATA_BITWIDTH, ADDR_BITWIDTH > &signals)
    {
	WRITE   (signals.WRITE);
	WRITEOK (signals.WRITEOK);
	FRIP    (signals.FRIP);
	DATA    (signals.DATA);
	ADDR    (signals.ADDR);
    }
};



template < unsigned short DATA_BITWIDTH, unsigned short ADDR_BITWIDTH > 
struct ACCELERATOR_ENBL_SIGNALS_OUT {
    sc_out < bool > WRITE;
    sc_in  < bool > WRITEOK;
    sc_out < bool > FRIP;
    sc_out < sc_uint < DATA_BITWIDTH > > DATA;
    sc_out < sc_uint < ADDR_BITWIDTH > > ADDR;

    void operator  () (ACCELERATOR_ENBL_SIGNALS < DATA_BITWIDTH, ADDR_BITWIDTH > &signals) {
		WRITE   (signals.WRITE);
		WRITEOK (signals.WRITEOK);
		FRIP    (signals.FRIP);
		DATA    (signals.DATA);
		ADDR    (signals.ADDR);
    }
};


#endif


/// Local Variables:
/// mode: hs-minor
/// c-basic-offset: 4
/// End:

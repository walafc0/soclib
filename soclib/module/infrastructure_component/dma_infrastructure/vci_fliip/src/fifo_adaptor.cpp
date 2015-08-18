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

#include <fifo_adaptor.h>


namespace soclib { namespace caba {

#define tmpl(x) \
template < typename INPUT_TYPE, typename OUTPUT_TYPE > \
x FifoAdaptor<INPUT_TYPE, OUTPUT_TYPE>

tmpl(void)::genMealy () {

	p_output.data = (OUTPUT_TYPE)p_input.data.read();
	p_output.w = p_input.rok;
	p_input.r = p_output.wok;

}

tmpl(/**/)::FifoAdaptor (sc_module_name insname)
			: BaseModule(insname),
		p_input("input")
		,p_output("output")
		//,rok__w("rok__w")
		//,r__wok("r__wok")
{

	SC_METHOD (genMealy);
	dont_initialize();
	sensitive << p_clk.neg()
		<< p_input.data
		<< p_input.rok
		<< p_output.wok;

#if 0
	p_input.r(r__wok);
	p_input.rok(rok__w);
	p_output.w(rok__w);
	p_output.wok(r__wok);
#endif

}

tmpl(/**/)::~FifoAdaptor() {

}

}} // end of soclib::caba

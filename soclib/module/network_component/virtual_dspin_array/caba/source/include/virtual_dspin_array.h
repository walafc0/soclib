/* -*- c++ -*-
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
 * Authors  : Noé Girand noe.girand@polytechnique.org
 * Date     : june 2009
 * Copyright: UPMC - LIP6
 */

#ifndef VIRTUAL_DSPIN_ARRAY_H_
#define VIRTUAL_DSPIN_ARRAY_H_

#include <systemc>
#include "caba_base_module.h"
#include "virtual_dspin_router.h"

namespace soclib 
{ 
	namespace caba 
	{
		using namespace sc_core;

		template<	int io_mask_size, 		// Size of IO checking
				int io_number_size, 		// Size of IO index
				int x_addressing_size, 		// Size of first coordinate addressing
				int y_addressing_size, 		// Size of second coordinate addressing

				int data_size, 			// Size of command flits
				int io_mask_offset, 		// Emplacement of IO checking in paquets
				int io_number_offset, 		// Emplacement of IO index in IO table in paquets
				int x_addressing_offset, 	// Emplacement of target x in first flit in paquets
				int y_addressing_offset, 	// Emplacement of target y in first flit in paquets
				int eop_offset, 		// Emplacement of eop checking in paquets
				int broadcast_offset,		// Emplacement of broadcast checking in paquets

				int in_fifo_size, 
				int out_fifo_size,
				int x_min_offset,
				int x_max_offset,
				int y_min_offset,
				int y_max_offset
		>
		class VirtualDspinArray: public soclib::caba::BaseModule
		{
			

		protected:
			SC_HAS_PROCESS(VirtualDspinArray);

		public:

			// ports
			sc_in<bool>	p_clk;
			sc_in<bool>	p_resetn;

			DspinOutput<data_size>	***p_out;
			DspinInput<data_size>	***p_in;

			// constructor 
			VirtualDspinArray(sc_module_name  insname, int size_x, int size_y, bool broadcast0, bool broadcast1, bool io0, bool io1, clusterCoordinates<x_addressing_size, y_addressing_size> * aIO_table);

			// destructor 
			~VirtualDspinArray();

		private:

			VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
						x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
						x_min_offset, x_max_offset, y_min_offset, y_max_offset> ** VirtualDspinRouters;

			DspinSignals<data_size> *** vWires;
			DspinSignals<data_size> *** hWires;

			int m_size_x;
			int m_size_y;

		};
	}
} // end namespace

#endif // VIRTUAL_DSPIN_ARRAY_H_

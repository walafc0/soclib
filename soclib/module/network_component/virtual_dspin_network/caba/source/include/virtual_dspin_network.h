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

#ifndef VIRTUAL_DSPIN_NETWORK_H_
#define VIRTUAL_DSPIN_NETWORK_H_

#include <systemc>
#include "caba_base_module.h"
#include "virtual_dspin_array.h"

namespace soclib 
{ 
	namespace caba 
	{
		using namespace sc_core;

		template<	int io_mask_size, 		// Size of IO checking
				int io_number_size, 		// Size of IO index
				int x_addressing_size, 		// Size of first coordinate addressing
				int y_addressing_size, 		// Size of second coordinate addressing

				int cmd_data_size, 		// Size of command flits
				int cmd_io_mask_offset, 	// Emplacement of IO checking in command paquets
				int cmd_io_number_offset, 	// Emplacement of IO index in IO table in command paquets
				int cmd_x_addressing_offset, 	// Emplacement of target x in first flit in command paquets
				int cmd_y_addressing_offset, 	// Emplacement of target y in first flit in command paquets
				int cmd_eop_offset, 		// Emplacement of eop checking in command paquets
				int cmd_broadcast_offset,	// Emplacement of broadcast checking in command paquets

				int rsp_data_size, 		// Size of response flits
				int rsp_io_mask_offset, 	// Emplacement of IO checking in response paquets
				int rsp_io_number_offset, 	// Emplacement of IO index in IO table in response paquets
				int rsp_x_addressing_offset, 	// Emplacement of target x in first flit in response paquets
				int rsp_y_addressing_offset,  	// Emplacement of target y in first flit in response paquets
				int rsp_eop_offset,  		// Emplacement of eop checking in response paquets

				int in_fifo_size, 
				int out_fifo_size,
				int x_min_offset,
				int x_max_offset,
				int y_min_offset,
				int y_max_offset
		>
		class VirtualDspinNetwork: public soclib::caba::BaseModule
		{
			

		protected:
			SC_HAS_PROCESS(VirtualDspinNetwork);

		public:

			// ports
			sc_in<bool>	p_clk;
			sc_in<bool>	p_resetn;

			DspinOutput<cmd_data_size> *** p_out_cmd;
			DspinInput<cmd_data_size> *** p_in_cmd;
			DspinOutput<rsp_data_size> *** p_out_rsp;
			DspinInput<rsp_data_size> *** p_in_rsp;

			// constructor 
			VirtualDspinNetwork(sc_module_name  insname, int size_x, int size_y, clusterCoordinates<x_addressing_size, y_addressing_size> * aIO_table);

			// destructor 
			~VirtualDspinNetwork();

		private:

			VirtualDspinArray<io_mask_size, io_number_size, x_addressing_size, y_addressing_size, 
						cmd_data_size, cmd_io_mask_offset, cmd_io_number_offset, cmd_x_addressing_offset, cmd_y_addressing_offset, cmd_eop_offset, cmd_broadcast_offset,
						in_fifo_size, out_fifo_size, x_min_offset, x_max_offset, y_min_offset, y_max_offset> * cmdVirtualDspinArray;

			VirtualDspinArray<io_mask_size, io_number_size, x_addressing_size, y_addressing_size, 
						rsp_data_size, rsp_io_mask_offset, rsp_io_number_offset, rsp_x_addressing_offset, rsp_y_addressing_offset, rsp_eop_offset, 0,
						in_fifo_size, out_fifo_size, 0, 0, 0, 0> * rspVirtualDspinArray;

		};
	}
} // end namespace

#endif // VIRTUAL_DSPIN_NETWORK_H_

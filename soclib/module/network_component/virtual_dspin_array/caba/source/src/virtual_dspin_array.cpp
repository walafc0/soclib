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

#include "virtual_dspin_array.h"
#include <cstdlib>
#include <cassert>
#include <sstream>
#include "alloc_elems.h"
#include <new>

namespace soclib 
{ 
	namespace caba 
	{
		#define tmpl(x) template<int io_mask_size, int io_number_size, int x_addressing_size, int y_addressing_size, int data_size, int io_mask_offset,  int io_number_offset, int x_addressing_offset, int y_addressing_offset,int eop_offset, int broadcast_offset, int in_fifo_size, int out_fifo_size,	int x_min_offset, int x_max_offset, int y_min_offset, int y_max_offset> x VirtualDspinArray<io_mask_size, io_number_size, x_addressing_size, y_addressing_size, data_size, io_mask_offset,  io_number_offset, x_addressing_offset, y_addressing_offset,eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, x_min_offset, x_max_offset, y_min_offset, y_max_offset>
		
		////////////////////////////////
		//      constructor
		////////////////////////////////
		tmpl(/**/)::VirtualDspinArray(sc_module_name insname, int size_x, int size_y, bool broadcast0, bool broadcast1, bool io0, bool io1, clusterCoordinates<x_addressing_size, y_addressing_size> * aIO_table)	: soclib::caba::BaseModule(insname), p_clk("clk"), p_resetn("resetn")
		{

			assert((size_x > 1)&&(size_y > 1) && "Virtual dspin array needs at least 2 columns and 2 rows");
	
			m_size_x = size_x;
			m_size_y = size_y;

			std::ostringstream o;

			o.str("");
			o << insname << "_vWires" ;
			vWires = soclib::common::alloc_elems<soclib::caba::DspinSignals<data_size> >(o.str().c_str(),  4, size_x, size_y-1);
			o.str("");
			o << insname << "_hWires" ;
			hWires = soclib::common::alloc_elems<soclib::caba::DspinSignals<data_size> >(o.str().c_str(),  4, size_x-1, size_y);

			VirtualDspinRouters = (soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
					x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
					x_min_offset, x_max_offset, y_min_offset, y_max_offset> **) 
				malloc(sizeof(soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
						x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
						x_min_offset, x_max_offset, y_min_offset, y_max_offset> *)*size_x);

			for(int i=0; i<size_x; i++){
				VirtualDspinRouters[i] = (soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
						x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size,
						x_min_offset, x_max_offset, y_min_offset, y_max_offset> *)
					malloc(sizeof(soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
							x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size,
							x_min_offset, x_max_offset, y_min_offset, y_max_offset> )*size_y);
			}			

			// The four corners
			o.str("");
			o << insname << "_VirtualDspinRouter[" << 0 		<< "][" << 0 		<< "]";
			new(&VirtualDspinRouters[0][0]) 
				soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
						x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
						x_min_offset, x_max_offset, y_min_offset, y_max_offset>(
					o.str().c_str(), 0		, 0		, true, false, true, false, broadcast0, broadcast1, io0, io1, aIO_table);
			o.str("");
			o << insname << "_VirtualDspinRouter[" << 0 		<< "][" << (size_y-1) 	<< "]";
			new(&VirtualDspinRouters[0][size_y-1]) 
				soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
						x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
						x_min_offset, x_max_offset, y_min_offset, y_max_offset>(
					o.str().c_str(), 0		, (size_y-1)	, false, true, true, false, broadcast0, broadcast1, io0, io1, aIO_table);
			o.str("");
			o << insname << "_VirtualDspinRouter[" << size_x-1 	<< "][" << 0 		<< "]";
			new(&VirtualDspinRouters[size_x-1][0]) 
				soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
						x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
						x_min_offset, x_max_offset, y_min_offset, y_max_offset>(
					o.str().c_str(), (size_x-1)	, 0		, true, false, false, true, broadcast0, broadcast1, io0, io1, aIO_table);
			o.str("");
			o << insname << "_VirtualDspinRouter[" << size_x-1 	<< "][" << size_y-1 	<< "]";
			new(&VirtualDspinRouters[size_x-1][size_y-1]) 
				soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
						x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
						x_min_offset, x_max_offset, y_min_offset, y_max_offset>(
					o.str().c_str(), (size_x-1)	, (size_y-1)	, false, true, false, true, broadcast0, broadcast1, io0, io1, aIO_table);

			// South rown 			
			for(int i=1; i<size_x-1; i++){
				o.str("");
				o << insname << "_VirtualDspinRouter[" << i 		<< "][" << 0 		<< "]";
				new(&VirtualDspinRouters[i][0]) 
					soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
							x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
							x_min_offset, x_max_offset, y_min_offset, y_max_offset>(
						o.str().c_str(), i	, 0		, true, false, true, true, broadcast0, broadcast1, io0, io1, aIO_table);
			}

			// North row 
			for(int i=1; i<size_x-1; i++){
				o.str("");
				o << insname << "_VirtualDspinRouter[" << i 		<< "][" << size_y-1 	<< "]";
				new(&VirtualDspinRouters[i][size_y-1]) 
					soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
							x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
							x_min_offset, x_max_offset, y_min_offset, y_max_offset>(
						o.str().c_str(), i	, size_y-1	, false, true, true, true, broadcast0, broadcast1, io0, io1, aIO_table);
			}
			
			// West column
			for(int j=1; j<size_y-1; j++){
				o.str("");
				o << insname <<  "_VirtualDspinRouter[" << 0 	<< "][" << j 		<< "]";
				new(&VirtualDspinRouters[0][j]) 
					soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
							x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
							x_min_offset, x_max_offset, y_min_offset, y_max_offset>(
						o.str().c_str(), 0	, j		, true, true, true, false, broadcast0, broadcast1, io0, io1, aIO_table);
			}

			// East column
			for(int j=1; j<size_y-1; j++){
				o.str("");
				o << insname << "_VirtualDspinRouter[" << size_x-1 	<< "][" << j 		<< "]";
				new(&VirtualDspinRouters[size_x-1][j]) 
					soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size, 
							x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
							x_min_offset, x_max_offset, y_min_offset, y_max_offset>(
						o.str().c_str(), size_x-1, j		, true, true, false, true, broadcast0, broadcast1, io0, io1, aIO_table);
			}

			// The middle cells
			for(int i=1; i<size_x-1; i++)
				for(int j=1; j<size_y-1; j++){
					o.str("");
					o << insname << "_VirtualDspinRouter[" << i << "][" << j << "]";
					new(&VirtualDspinRouters[i][j]) 
						soclib::caba::VirtualDspinRouter<data_size, io_mask_offset, io_mask_size, io_number_offset, io_number_size,
								x_addressing_offset, x_addressing_size, y_addressing_offset, y_addressing_size, eop_offset, broadcast_offset, in_fifo_size, out_fifo_size, 
								x_min_offset, x_max_offset, y_min_offset, y_max_offset>(
							o.str().c_str(), i, j, true, true, true, true, broadcast0, broadcast1, io0, io1, aIO_table);
			}


			for(int i=0; i<size_x; i++){	// Connecting regular signals
				for(int j=0; j<size_y; j++){
					VirtualDspinRouters[i][j].p_clk(p_clk);
					VirtualDspinRouters[i][j].p_resetn(p_resetn);
				}
			}				

			for(int l=0; l<2; l++){		// Vertical connexions
				for(int i=0; i<size_x; i++){
					for(int j=0; j<size_y-1; j++){
						VirtualDspinRouters[i][j  ].p_out[l][NORTH](vWires[l	][i][j]);
						VirtualDspinRouters[i][j+1].p_in[l][SOUTH](vWires[l	][i][j]);
						VirtualDspinRouters[i][j+1].p_out[l][SOUTH](vWires[l+2	][i][j]);
						VirtualDspinRouters[i][j  ].p_in[l][NORTH](vWires[l+2	][i][j]);
					}
				}
			}

			for(int l=0; l<2; l++){		// Horizontal connexions
				for(int i=0; i<size_x-1; i++){
					for(int j=0; j<size_y; j++){
						VirtualDspinRouters[i  ][j].p_out[l][EAST](hWires[l	][i][j]);
						VirtualDspinRouters[i+1][j].p_in[l][WEST](hWires[l	][i][j]);
						VirtualDspinRouters[i+1][j].p_out[l][WEST](hWires[l+2	][i][j]);
						VirtualDspinRouters[i  ][j].p_in[l][EAST](hWires[l+2	][i][j]);
					}
				}
			}

			// Connecting ports to Virtual Dspin Routers
			o.str("");
			o << insname << "_p_out";
			p_out = soclib::common::alloc_elems<soclib::caba::DspinOutput<data_size> >(o.str().c_str(),  2, size_x, size_y);

			o.str("");
			o << insname << "_p_in";
			p_in = soclib::common::alloc_elems<soclib::caba::DspinInput<data_size> >(o.str().c_str(),  2, size_x, size_y);

			for(int i=0; i<size_x; i++){
				for(int j=0; j<size_y; j++){
					for(int k=0; k<2; k++){
						VirtualDspinRouters[i][j].p_out[k][LOCAL].data(p_out[k][i][j].data);
						VirtualDspinRouters[i][j].p_out[k][LOCAL].read(p_out[k][i][j].read);
						VirtualDspinRouters[i][j].p_out[k][LOCAL].write(p_out[k][i][j].write);

						VirtualDspinRouters[i][j].p_in[k][LOCAL].data(p_in[k][i][j].data);
						VirtualDspinRouters[i][j].p_in[k][LOCAL].read(p_in[k][i][j].read);
						VirtualDspinRouters[i][j].p_in[k][LOCAL].write(p_in[k][i][j].write);
					}
				}
			}
		
		} //  end constructor

		tmpl(/**/)::~VirtualDspinArray()
		{
			soclib::common::dealloc_elems(p_out,	2, 	m_size_x, 	m_size_y	);
			soclib::common::dealloc_elems(p_in,	2, 	m_size_x, 	m_size_y	);
			soclib::common::dealloc_elems(vWires,	4, 	m_size_x, 	m_size_y-1	);
			soclib::common::dealloc_elems(hWires,	4, 	m_size_x-1, 	m_size_y	);
			soclib::common::dealloc_elems(VirtualDspinRouters, m_size_x, 	m_size_y	);
		} // end destructor
	} // end namespace caba
} // end namespace soclib

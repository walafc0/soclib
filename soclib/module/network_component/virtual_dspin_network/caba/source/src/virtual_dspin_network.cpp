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

#include "virtual_dspin_network.h"
#include <cstdlib>
#include <cassert>
#include <sstream>
#include "alloc_elems.h"
#include <new>

namespace soclib 
{ 
	namespace caba 
	{
		#define tmpl(x) template<int io_mask_size, int io_number_size, int x_addressing_size, int y_addressing_size, int cmd_data_size, int cmd_io_mask_offset,  int cmd_io_number_offset, int cmd_x_addressing_offset, int cmd_y_addressing_offset,int cmd_eop_offset, int cmd_broadcast_offset, int rsp_data_size, int rsp_io_mask_offset, int rsp_io_number_offset, int rsp_x_addressing_offset, int rsp_y_addressing_offset, int rsp_eop_offset, int in_fifo_size, int out_fifo_size, int x_min_offset, int x_max_offset, int y_min_offset, int y_max_offset> x VirtualDspinNetwork<io_mask_size, io_number_size, x_addressing_size, y_addressing_size, cmd_data_size, cmd_io_mask_offset,  cmd_io_number_offset, cmd_x_addressing_offset, cmd_y_addressing_offset,cmd_eop_offset, cmd_broadcast_offset, rsp_data_size, rsp_io_mask_offset, rsp_io_number_offset, rsp_x_addressing_offset, rsp_y_addressing_offset, rsp_eop_offset, in_fifo_size, out_fifo_size, x_min_offset, x_max_offset, y_min_offset, y_max_offset>
		
		////////////////////////////////
		//      constructor
		////////////////////////////////
		tmpl(/**/)::VirtualDspinNetwork(sc_module_name insname, int size_x, int size_y, clusterCoordinates<x_addressing_size, y_addressing_size> * aIO_table): soclib::caba::BaseModule(insname), p_clk("clk"), p_resetn("resetn")
		{

			assert((size_x > 1)&&(size_y > 1) && "Virtual dspin network needs at least 2 columns and 2 rows");

			std::ostringstream o;

			o.str("");
			o << insname << "_cmdVirtualDspinArray" ;
			cmdVirtualDspinArray = new VirtualDspinArray<io_mask_size, io_number_size, x_addressing_size, y_addressing_size, 
						cmd_data_size, cmd_io_mask_offset, cmd_io_number_offset, cmd_x_addressing_offset, cmd_y_addressing_offset, cmd_eop_offset, cmd_broadcast_offset,
						in_fifo_size, out_fifo_size, x_min_offset, x_max_offset, y_min_offset, y_max_offset>(o.str().c_str(),  size_x, size_y, false, true, true, false, aIO_table);

			o.str("");
			o << insname << "_rspVirtualDspinArray" ;
			rspVirtualDspinArray = new VirtualDspinArray<io_mask_size, io_number_size, x_addressing_size, y_addressing_size, 
						rsp_data_size, rsp_io_mask_offset, rsp_io_number_offset, rsp_x_addressing_offset, rsp_y_addressing_offset, rsp_eop_offset, 0,
						in_fifo_size, out_fifo_size, 0, 0, 0, 0>(o.str().c_str(),  size_x, size_y, false, false, true, false, aIO_table);

			cmdVirtualDspinArray->p_clk(p_clk);
			rspVirtualDspinArray->p_clk(p_clk);
			cmdVirtualDspinArray->p_resetn(p_resetn);
			rspVirtualDspinArray->p_resetn(p_resetn);

			// Connecting ports to Virtual Dspin Arrays
			o.str("");
			o << insname << "_p_out_cmd";
			p_out_cmd = soclib::common::alloc_elems<soclib::caba::DspinOutput<cmd_data_size> >(o.str().c_str(),  2, size_x, size_y);

			o.str("");
			o << insname << "_p_in_cmd";
			p_in_cmd = soclib::common::alloc_elems<soclib::caba::DspinInput<cmd_data_size> >(o.str().c_str(),  2, size_x, size_y);

			o.str("");
			o << insname << "_p_out_rsp";
			p_out_rsp = soclib::common::alloc_elems<soclib::caba::DspinOutput<rsp_data_size> >(o.str().c_str(),  2, size_x, size_y);

			o.str("");
			o << insname << "_p_in_rsp";
			p_in_rsp = soclib::common::alloc_elems<soclib::caba::DspinInput<rsp_data_size> >(o.str().c_str(),  2, size_x, size_y);

			for(int i=0; i<size_x; i++){
				for(int j=0; j<size_y; j++){
					for(int k=0; k<2; k++){
						cmdVirtualDspinArray->p_in[k][i][j].data(p_in_cmd[k][i][j].data);
						cmdVirtualDspinArray->p_in[k][i][j].read(p_in_cmd[k][i][j].read);
						cmdVirtualDspinArray->p_in[k][i][j].write(p_in_cmd[k][i][j].write);

						rspVirtualDspinArray->p_out[k][i][j].data(p_out_rsp[k][i][j].data);
						rspVirtualDspinArray->p_out[k][i][j].read(p_out_rsp[k][i][j].read);
						rspVirtualDspinArray->p_out[k][i][j].write(p_out_rsp[k][i][j].write);

						cmdVirtualDspinArray->p_out[k][i][j].data(p_out_cmd[k][i][j].data);
						cmdVirtualDspinArray->p_out[k][i][j].read(p_out_cmd[k][i][j].read);
						cmdVirtualDspinArray->p_out[k][i][j].write(p_out_cmd[k][i][j].write);

						rspVirtualDspinArray->p_in[k][i][j].data(p_in_rsp[k][i][j].data);
						rspVirtualDspinArray->p_in[k][i][j].read(p_in_rsp[k][i][j].read);
						rspVirtualDspinArray->p_in[k][i][j].write(p_in_rsp[k][i][j].write);
					}
				}
			}
		} //  end constructor

		tmpl(/**/)::~VirtualDspinNetwork()
		{
			free(cmdVirtualDspinArray);
			free(rspVirtualDspinArray);
		} // end destructor
	} // end namespace caba
} // end namespace soclib

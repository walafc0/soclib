/* -*- c++ -*-
  * File : vci_dspinplus_network.h
  * Copyright (c) UPMC, Lip6
  * Authors : Alain Greiner, Abbas Sheibanyrad, Ivan Miro, Zhen Zhang
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
  */

#ifndef VCI_DSPINPLUS_NETWORK_H_
#define VCI_DSPINPLUS_NETWORK_H_

#include <systemc>
#include "caba_base_module.h"
#include "vci_target.h"
#include "vci_initiator.h"
#include "mapping_table.h"

#include "dspinplus_router.h"
#include "vci_dspinplus_target_wrapper.h"
#include "vci_dspinplus_initiator_wrapper.h"
#include "dspin_interface.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template<typename vci_param, int dspin_fifo_size, int dspin_yx_size>
	class VciDspinPlusNetwork
	: public soclib::caba::BaseModule
	{
           enum{
                NORTH   = 0,
                SOUTH   = 1,
                EAST    = 2,
                WEST    = 3,
                LOCAL   = 4
            };

	    public:
		sc_in<bool>		p_clk;
		sc_in<bool>		p_resetn;

		soclib::caba::VciInitiator<vci_param>** p_to_target;
		soclib::caba::VciTarget<vci_param>** p_to_initiator;

	    protected:
	    SC_HAS_PROCESS(VciDspinPlusNetwork);

	    private:
	    	size_t m_width_network;
		size_t m_height_network;

		//
		// signal between routers
		//
	    	soclib::caba::DspinSignals<37>** s_req_NS;
	    	soclib::caba::DspinSignals<37>** s_req_EW;
	    	soclib::caba::DspinSignals<37>** s_req_SN;
	    	soclib::caba::DspinSignals<37>** s_req_WE;

	    	soclib::caba::DspinSignals<33>** s_rsp_NS;
	    	soclib::caba::DspinSignals<33>** s_rsp_EW;
	    	soclib::caba::DspinSignals<33>** s_rsp_SN;
	    	soclib::caba::DspinSignals<33>** s_rsp_WE;

		//
		// signal between router and wrapper
		//
	    	soclib::caba::DspinSignals<37>** s_req_RW;
	    	soclib::caba::DspinSignals<37>** s_req_WR;
		
	    	soclib::caba::DspinSignals<33>** s_rsp_RW;
	    	soclib::caba::DspinSignals<33>** s_rsp_WR;

	    	//dspin
		soclib::caba::VciDspinPlusInitiatorWrapper<vci_param, dspin_fifo_size, dspin_yx_size>*** t_initiator_wrapper;
		soclib::caba::VciDspinPlusTargetWrapper<vci_param, dspin_fifo_size, dspin_yx_size>*** t_target_wrapper;
		soclib::caba::DspinPlusRouter<37, dspin_fifo_size, dspin_yx_size>*** t_req_router;
		soclib::caba::DspinPlusRouter<33, dspin_fifo_size, dspin_yx_size>*** t_rsp_router;

		//checker
		soclib_static_assert(dspin_fifo_size <= 256 && dspin_fifo_size >= 1);
		soclib_static_assert(dspin_yx_size <= 6 && dspin_yx_size >= 1);

	    public:
		VciDspinPlusNetwork( sc_module_name name,
						 const soclib::common::MappingTable &mt,
						 size_t width_network,   //X
						 size_t height_network); //Y

		~VciDspinPlusNetwork();

        public:
        void trace(sc_core::sc_trace_file*);
	};
}}
//end

#endif //VCI_DSPINPLUS_NETWORK_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

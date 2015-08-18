#ifndef SOCLIB_VIRTUAL_DSPIN_NETWORK_INTERFACE_H_
#define SOCLIB_VIRTUAL_DSPIN_NETWORK_INTERFACE_H_

#include <systemc>
//#include "static_assert.h"

namespace soclib { namespace caba {

using namespace sc_core;
using namespace sc_dt;

template<int cmd_data_size, int rsp_data_size>
class VirtualDspinNetworkPort{

	public:
		sc_in<sc_uint<cmd_data_size> >	in_cmd_data;
		sc_out<bool>			in_cmd_read;
 		sc_in<bool>			in_cmd_write;

        	sc_out<sc_uint<rsp_data_size> >	out_rsp_data;
		sc_in<bool>			out_rsp_read;
		sc_out<bool> 			out_rsp_write;

		sc_out<sc_uint<cmd_data_size> >	out_cmd_data;
		sc_in<bool>			out_cmd_read;
		sc_out<bool> 			out_cmd_write;

		sc_in<sc_uint<rsp_data_size> >	in_rsp_data;
		sc_out<bool> 			in_rsp_read;
		sc_in<bool>			in_rsp_write;

	#define __ren(x) x((name+"_" #x).c_str())

		VirtualDspinNetworkPort(const std::string &name = sc_gen_unique_name("virtual_dspin_network_port"))
		:__ren(in_cmd_data),
		__ren(in_cmd_write),
		__ren(in_cmd_read),

		__ren(out_rsp_data),
		__ren(out_rsp_write),
		__ren(out_rsp_read),

		__ren(out_cmd_data),
		__ren(out_cmd_write),
		__ren(out_cmd_read),

		__ren(in_rsp_data),
		__ren(in_rsp_write),
		__ren(in_rsp_read)

    		{}
	#undef __ren

};

}}

#endif

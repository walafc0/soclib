// -*- c++ -*-
#ifndef SOCLIB_TLMT_VCI_FRAMEBUFFER_H
#define SOCLIB_TLMT_VCI_FRAMEBUFFER_H

#include <tlmt>
#include "vci_ports.h"
#include "tlmt_base_module.h"
#include "mapping_table.h"
#include "soclib_endian.h"
#include "fb_controller.h"

namespace soclib { namespace tlmt {

template<typename vci_param>
class VciFrameBuffer 
  : public soclib::tlmt::BaseModule
{
private:
	soclib::common::IntTab m_index;
	soclib::common::MappingTable m_mt;
	soclib::common::Segment m_segment;

	vci_rsp_packet<vci_param> m_rsp;
	soclib::common::FbController m_framebuffer;
	typename vci_param::data_t *m_surface;

protected:
	SC_HAS_PROCESS(VciFrameBuffer);
public:
	soclib::tlmt::VciTarget<vci_param> p_vci;

	VciFrameBuffer(sc_core::sc_module_name name,
				   const soclib::common::IntTab &index,
				   const soclib::common::MappingTable &mt,
				   size_t width,
				   size_t height,
				   size_t subsampling);
  
	~VciFrameBuffer();

	void callback(
		soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		const tlmt_core::tlmt_time &time,
		void *private_data);
  
	void callback_read(
		soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		const tlmt_core::tlmt_time &time,
		void *private_data);
	
	void callback_write(
		soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		const tlmt_core::tlmt_time &time,
		void *private_data);
};

}}

#endif

#include "vci_param.h"
#include "../include/vci_framebuffer.h"

#ifndef VCI_FRAMEBUFFER_DEBUG
#define VCI_FRAMEBUFFER_DEBUG 0
#endif

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciFrameBuffer<vci_param>

template<typename T>
T be2mask(T be)
{
	const T m = (1<<sizeof(T));
	T r = 0;

	for ( size_t i=0; i<sizeof(T); ++i ) {
		r <<= 8;
		be <<= 1;
		if ( be & m )
			r |= 0xff;
	}
	return r;
}

tmpl(void)::callback(
	soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
	const tlmt_core::tlmt_time &time,
	void *private_data)
{
	if ( m_segment.contains(pkt->address) ) {
		switch(pkt->cmd){
		case vci_param::CMD_READ :
			return callback_read(pkt,time,private_data);
			break;
		case vci_param::CMD_WRITE :
			return callback_write(pkt,time,private_data);
			break;
		default:
			break;
		}
	}

    //send error message
    tlmt_core::tlmt_time delay = 1; 

    m_rsp.error  = true;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.pktid  = pkt->pktid;
    m_rsp.trdid  = pkt->trdid;
    p_vci.send(&m_rsp, time + delay);
}

tmpl(void)::callback_read(
	soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
	const tlmt_core::tlmt_time &time,
	void *private_data)
{
    typename vci_param::addr_t word = 
		(pkt->address - m_segment.baseAddress()) / sizeof(typename vci_param::data_t);

    for ( size_t i = 0; i < pkt->nwords; i++ ) {
		pkt->buf[i] = m_framebuffer.w<typename vci_param::data_t>(word);

		if ( ! pkt->contig )
			++word;
    }	
    tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords); 

    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.pktid  = pkt->pktid;
    m_rsp.trdid  = pkt->trdid;

    p_vci.send(&m_rsp, time + delay);
}

tmpl(void)::callback_write(
	soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
	const tlmt_core::tlmt_time &time,
	void *private_data)
{
    typename vci_param::addr_t word = 
		(pkt->address - m_segment.baseAddress()) / sizeof(typename vci_param::data_t);

    for ( size_t i = 0; i < pkt->nwords; i++ ) {
		typename vci_param::data_t cur = m_surface[word];
		uint32_t mask    = be2mask<typename vci_param::data_t>(pkt->be);
      
//		std::cout << name() << " write to " << word << " be: " << std::hex << mask << '(' << pkt->be << ')' << " data: " << pkt->buf[i] << std::endl;
		if ( word < m_framebuffer.m_surface_size )
			m_framebuffer.w<typename vci_param::data_t>(word)
				= (cur & ~mask) | (pkt->buf[i] & mask);
		else
			m_framebuffer.update();

		if ( ! pkt->contig )
			++word;
    }	

	if ( pkt->address ==  m_segment.baseAddress() + m_framebuffer.m_width*m_framebuffer.m_height-4 )
		m_framebuffer.update();

    tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords);

    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.pktid  = pkt->pktid;
    m_rsp.trdid  = pkt->trdid;

    p_vci.send(&m_rsp,  time + delay);
}

tmpl(/**/)::VciFrameBuffer(
	sc_core::sc_module_name name,
	const soclib::common::IntTab &index,
	const soclib::common::MappingTable &mt,
	size_t width,
	size_t height,
	size_t subsampling
	)
		   : soclib::tlmt::BaseModule(name),
		   m_index(index),
		   m_mt(mt),
		   m_segment(m_mt.getSegment(m_index)),
		   m_framebuffer((const char*)name, width, height, subsampling),
		   m_surface((typename vci_param::data_t*)m_framebuffer.surface()),
		   p_vci("vci", new tlmt_core::tlmt_callback<VciFrameBuffer,soclib::tlmt::vci_cmd_packet<vci_param> *>(this, &VciFrameBuffer<vci_param>::callback))
{
	assert( m_segment.size() >= m_framebuffer.m_surface_size &&
			"Framebuffer segment too short." );
}

tmpl(/**/)::~VciFrameBuffer()
{
}
 
}}


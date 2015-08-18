#ifndef SOCLIB_TLMT_SIMPLE_INITIATOR_H
#define SOCLIB_TLMT_SIMPLE_INITIATOR_H

#include <tlmt>
#include "tlmt_base_module.h"
#include "vci_ports.h"

namespace soclib { namespace tlmt {

template<typename vci_param>
  class VciSimpleInitiator
  : public soclib::tlmt::BaseModule
  {
    tlmt_core::tlmt_thread_context c0;
    sc_core::sc_event m_vci_event;

    uint32_t m_srcid;
    uint32_t m_pktid;
    uint32_t m_deltaTime;
    tlmt_core::tlmt_time m_simulationTime;

    soclib::tlmt::vci_cmd_packet<vci_param> m_cmd;
    
  protected:
    SC_HAS_PROCESS(VciSimpleInitiator);
    
  public:
    soclib::tlmt::VciInitiator<vci_param> p_vci;

    VciSimpleInitiator( sc_core::sc_module_name name, uint32_t index, uint32_t dt, tlmt_core::tlmt_time st);

    void rspReceived(soclib::tlmt::vci_rsp_packet<vci_param> *pkt,
		     const tlmt_core::tlmt_time &time,
		     void *private_data);

    void behavior();

    void configureMwmr(typename vci_param::addr_t address, int n_read_channels, int n_write_channels, int read_depth, int write_depth);

    void configureChannelStatus(int n_channels);
  };
 
}}

#endif

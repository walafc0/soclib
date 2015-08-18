#ifndef SOCLIB_TLMT_COPROCESSOR_H
#define SOCLIB_TLMT_COPROCESSOR_H

#include <tlmt>
#include "tlmt_base_module.h"
#include "fifo_ports.h"
#include "vci_ports.h"

namespace soclib { namespace tlmt {

template<typename vci_param>
class Coprocessor
  : public soclib::tlmt::BaseModule
{
 private:
  uint32_t m_id;
  uint32_t m_read_fifo_depth;
  uint32_t m_write_fifo_depth;
  uint32_t m_read_channels;
  uint32_t m_write_channels;
  uint32_t m_config_registers;
  uint32_t m_status_registers;
  
  uint32_t *m_config_register;
  uint32_t *m_status_register;

  tlmt_core::tlmt_thread_context c0;
  fifo_cmd_packet<vci_param> m_cmd;

  sc_core::sc_event m_rsp_write;
  sc_core::sc_event m_rsp_read;
  sc_core::sc_event m_active_event;
 
 protected:
  SC_HAS_PROCESS(Coprocessor);
 public:

  std::vector<tlmt_core::tlmt_in<typename vci_param::data_t> *>  p_config;
  std::vector<tlmt_core::tlmt_in<typename vci_param::data_t*> *> p_status;

  std::vector<soclib::tlmt::FifoInitiator<vci_param> *> p_read_fifo;
  std::vector<soclib::tlmt::FifoInitiator<vci_param> *> p_write_fifo;

  Coprocessor(sc_core::sc_module_name name,
	      uint32_t id,
	      uint32_t read_fifo_depth,
	      uint32_t write_fifo_depth,
	      uint32_t n_read_channels,
	      uint32_t n_write_channels,
	      uint32_t n_config,
	      uint32_t n_status);
  
  void writeConfigReceived(typename vci_param::data_t data,
			   const tlmt_core::tlmt_time &time,
			   void *private_data);

  void readStatusReceived(typename vci_param::data_t *data,
			  const tlmt_core::tlmt_time &time,
			  void *private_data);
  
  void readReponseReceived(int data,
			   const tlmt_core::tlmt_time &time,
			   void *private_data);

  void writeReponseReceived(int data,
			    const tlmt_core::tlmt_time &time,
			    void *private_data);

  void execLoop();
};

}}

#endif

#include "vci_param.h"
#include "../include/coprocessor.h"

#ifndef COPROCESSOR_DEBUG
#define COPROCESSOR_DEBUG 1
#endif

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x Coprocessor<vci_param>

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // RECEIVE REPONSE OF A READ REQUEST 
  //////////////////////////////////////////////////////////////////////////////////////////////// ///////
  tmpl(void)::readReponseReceived(int data,
				  const tlmt_core::tlmt_time &time,
				  void *private_data)
  {
    //update time
    c0.update_time(time);
    m_rsp_read.notify(sc_core::SC_ZERO_TIME);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // RECEIVE REPONSE OF A WRITE REQUEST 
  //////////////////////////////////////////////////////////////////////////////////////////////// ///////
  tmpl(void)::writeReponseReceived(int data,
				   const tlmt_core::tlmt_time &time,
				   void *private_data)
  {
    //update time
    c0.update_time(time);
    m_rsp_write.notify(sc_core::SC_ZERO_TIME);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // RECEIVE A REQUEST TO WRITE TO A CONFIGURATION REGISTER 
  //////////////////////////////////////////////////////////////////////////////////////////////// ///////
  tmpl(void)::writeConfigReceived(typename vci_param::data_t data,
				  const tlmt_core::tlmt_time &time,
				  void *private_data)
  {
    int idx = (int)private_data;
    m_config_register[idx] = data;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // RECEIVE A REQUEST TO READ FROM A STATUS REGISTER 
  //////////////////////////////////////////////////////////////////////////////////////////////// ///////
  tmpl(void)::readStatusReceived(typename vci_param::data_t *data,
				 const tlmt_core::tlmt_time &time,
				 void *private_data)
  {
    int idx = (int)private_data;
    *data = m_status_register[idx];
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // LOOP: WRITE TO A FIFO AND AFTER  RECEIVE A REQUEST TO WRITE TO A CONFIGURATION REGISTER 
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::execLoop()
  {
    typename vci_param::data_t write_buffer[m_write_fifo_depth];
    typename vci_param::data_t read_buffer[m_read_fifo_depth];
    uint32_t counter = 0;


    int count = 0;
    //while(true){
    while(count<10){
      count++;
      
      //write
      for(uint32_t i=0; i < m_write_fifo_depth; i++){
	write_buffer[i] = counter;
	counter += 1;
      }
      
      for(uint32_t j = 0; j < m_write_channels; j++){
	m_cmd.nwords  = m_write_fifo_depth;
	m_cmd.buf = write_buffer;
	
#if COPROCESSOR_DEBUG
	std::cout << "[COPROCESSOR " << m_id << "] Write FIFO " << j << " with time " << c0.time() << " number of executions "<< count << std::endl;
	for(uint32_t i=0; i<m_write_fifo_depth;i++)
	  std::cout << std::hex << "[ " << i <<" ] =  " << write_buffer[i] << std::dec << std::endl;
#endif
	
	p_write_fifo[j]->send(&m_cmd, c0.time());
	wait(m_rsp_write);
	c0.add_time(m_write_fifo_depth);

#if COPROCESSOR_DEBUG
	std::cout << "[COPROCESSOR " << m_id << "] Awnser Write FIFO " << j << " with time " << c0.time() << " number of executions "<< count << std::endl;
#endif
      }

      //calcule time
      c0.add_time(500);

      // read
      for(uint32_t j = 0; j < m_read_channels; j++){
	m_cmd.nwords  = m_read_fifo_depth;
	m_cmd.buf = read_buffer;
	
#if COPROCESSOR_DEBUG
	std::cout << "[COPROCESSOR " << m_id << "] Read FIFO " << j << " with time " << c0.time() << " number of executions "<< count << std::endl;
#endif

	p_read_fifo[j]->send(&m_cmd, c0.time());
	wait(m_rsp_read);
	c0.add_time(m_read_fifo_depth);

#if COPROCESSOR_DEBUG
	std::cout << "[COPROCESSOR " << m_id << "] Awnser Read FIFO " << j << " with time " << c0.time() << " number of executions "<< count << std::endl;
	for(uint32_t i=0; i<m_read_fifo_depth;i++)
	  std::cout << std::hex << "[ " << i <<" ] =  " << read_buffer[i] << std::dec << std::endl;
#endif
      }
    }
    c0.disable();
  }

  tmpl(/**/)::Coprocessor(sc_core::sc_module_name name,
			  uint32_t id,
			  uint32_t read_fifo_depth,
			  uint32_t write_fifo_depth,
			  uint32_t n_read_channels,
			  uint32_t n_write_channels,
			  uint32_t n_config,
			  uint32_t n_status)
    : soclib::tlmt::BaseModule(name),
      m_id(id),
      m_read_fifo_depth(read_fifo_depth),
      m_write_fifo_depth(write_fifo_depth),
      m_read_channels(n_read_channels),
      m_write_channels(n_write_channels),
      m_config_registers(n_config),
      m_status_registers(n_status)
  {
    //READ FIFO PORTS
    for(uint32_t i=0;i<m_read_channels;i++){
      std::ostringstream tmpName;
      tmpName << "read_fifo" << i;
      p_read_fifo.push_back(new soclib::tlmt::FifoInitiator<vci_param>(tmpName.str().c_str(), new tlmt_core::tlmt_callback<Coprocessor,int>(this,&Coprocessor<vci_param>::readReponseReceived,(int*)i)));

    }

    //WRITE FIFO PORTS
    for(uint32_t i=0;i<m_write_channels;i++){
      std::ostringstream tmpName;
      tmpName << "write_fifo" << i;
      p_write_fifo.push_back(new soclib::tlmt::FifoInitiator<vci_param>(tmpName.str().c_str(), new tlmt_core::tlmt_callback<Coprocessor,int>(this,&Coprocessor<vci_param>::writeReponseReceived,(int*)i)));
    }

    //CONFIG PORTS
    m_config_register = new uint32_t[m_config_registers];
    for(uint32_t i=0;i<m_config_registers;i++){
      m_config_register[i] = 0;
      std::ostringstream tmpName;
      tmpName << "config" << i;
      p_config.push_back(new tlmt_core::tlmt_in<typename vci_param::data_t>(tmpName.str().c_str(), new tlmt_core::tlmt_callback<Coprocessor,typename vci_param::data_t>(this,&Coprocessor<vci_param>::writeConfigReceived,(int*)i)));
    }

    //STATUS PORTS
    m_status_register = new uint32_t[m_status_registers];
    for(uint32_t i=0;i<m_status_registers;i++){
      m_status_register[i] = 0;
      std::ostringstream tmpName;
      tmpName << "status" << i;
      p_status.push_back(new tlmt_core::tlmt_in<typename vci_param::data_t*>(tmpName.str().c_str(), new tlmt_core::tlmt_callback<Coprocessor,typename vci_param::data_t*>(this,&Coprocessor<vci_param>::readStatusReceived,(int*)i)));
    }

    SC_THREAD(execLoop);

  }
  
}}


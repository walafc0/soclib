#include "vci_param.h"
#include "mwmr_controller.h"
#include "vci_mwmr_controller.h"

#ifndef MWMR_CONTROLLER_DEBUG
#define MWMR_CONTROLLER_DEBUG 0
#endif

#ifndef MY_MWMR_CONTROLLER_DEBUG
#define MY_MWMR_CONTROLLER_DEBUG 0
#endif

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciMwmrController<vci_param>

  /////////////////////////////////////////////////////////////////////////////  
  // CALLBACK FUNCTION FOR A COMMAND SENT (INITIATOR VCI)
  /////////////////////////////////////////////////////////////////////////////  
  tmpl(void)::vciRspReceived(soclib::tlmt::vci_rsp_packet<vci_param> *pkt,
			     const tlmt_core::tlmt_time &time,
			     void *private_data)
  {
    //Update the time local
    c0.update_time(time);
    m_vci_event.notify(sc_core::SC_ZERO_TIME);
  }

  /////////////////////////////////////////////////////////////////////////////  
  // FUNCTION TO RECEIVE COMMANDS FROM NETWORK (TARGET VCI)
  // Packets to target vci DO NOT UPDATE the local time 
  /////////////////////////////////////////////////////////////////////////////  
  tmpl(void)::vciCmdReceived(soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
			     const tlmt_core::tlmt_time &time,
			     void *private_data)
  {
    std::list<soclib::common::Segment>::iterator seg;	
    size_t segIndex;

    for (segIndex=0,seg = m_target_segments.begin(); seg != m_target_segments.end(); ++segIndex, ++seg ) {
      soclib::common::Segment &s = *seg;
      if (!s.contains(pkt->address))
	continue;

      switch(pkt->cmd){
      case vci_param::CMD_READ:
	vciCmdReceived_read(segIndex,s,pkt,time,private_data);
	break;
      case vci_param::CMD_WRITE:
	vciCmdReceived_write(segIndex,s,pkt,time,private_data);
	break;
      default:
	break;
      }
      return;
    }
    //send error message
    m_rsp.error  = true;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.trdid  = pkt->trdid;
    m_rsp.pktid  = pkt->pktid;

#if MWMR_CONTROLLER_DEBUG
    std::cout << "[MWMR Target " << m_destid << "] Address " << pkt->address << " does not match any segment " << std::endl;
    std::cout << "[MWMR Target " << m_destid << "] Send to source "<< pkt->srcid << " a error packet with time = "  << time << std::endl;
#endif
    
    typename tlmt_core::tlmt_time delay = 1;
    p_vci_target.send(&m_rsp, (time + delay));
  }

  /////////////////////////////////////////////////////////////////////////////  
  // FUNCTION TO RECEIVE READ COMMAND FROM NETWORK (TARGET VCI)
  // Packets to target vci DO NOT UPDATE the local time 
  /////////////////////////////////////////////////////////////////////////////  
  tmpl(void)::vciCmdReceived_read(size_t segIndex,
				  soclib::common::Segment &s,
				  soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
				  const tlmt_core::tlmt_time &time,
				  void *private_data)
  {
    int reg;
    typename tlmt_core::tlmt_time delay = pkt->nwords;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Target %d] Receive from source %d a read packet %d with time = %d\n", m_destid, pkt->srcid, pkt->pktid, (int)time);
    std::cout << "[MWMR Target " << m_destid << "] Receive from source " << pkt->srcid << " a read packet " << pkt->pktid << " with time = " << (int)time << std::endl;
#endif

    //Update the time local
    c0.update_time(time);

    for(unsigned int i=0; i<pkt->nwords;i++){
      reg = (int)((pkt->address + (i*vci_param::nbytes)) - s.baseAddress()) / vci_param::nbytes;
      if ( reg < MWMR_IOREG_MAX ) { // coprocessor IO register access
	//add the reading time (reading time equals to number of words, in this case 1)
	c0.add_time(1);
	p_status[reg]->send(&pkt->buf[i], c0.time());
      }
      else {                      // MWMR channel configuration access (or Reset)
	switch (reg) {
	case MWMR_CONFIG_FIFO_NO :
	  pkt->buf[i] = m_channel_index;
	  break;
	case MWMR_CONFIG_FIFO_WAY :
	  pkt->buf[i] = m_channel_read;
	  break;
	case MWMR_CONFIG_STATUS_ADDR :
	  if(m_channel_read)
	    pkt->buf[i] = m_read_channel[m_channel_index].status_address;
	  else
	    pkt->buf[i] = m_write_channel[m_channel_index].status_address;
	  break;
	case MWMR_CONFIG_DEPTH :
	  if(m_channel_read)
	    pkt->buf[i] = m_read_channel[m_channel_index].depth;
	  else
	    pkt->buf[i] = m_write_channel[m_channel_index].depth;
	  break;
	case MWMR_CONFIG_BUFFER_ADDR :
	  if(m_channel_read)
	    pkt->buf[i] = m_read_channel[m_channel_index].base_address;
	  else
	    pkt->buf[i] = m_write_channel[m_channel_index].base_address;
	  break;
	case MWMR_CONFIG_RUNNING :
	  if(m_channel_read)
	    pkt->buf[i] = m_read_channel[m_channel_index].running;
	  else
	    pkt->buf[i] = m_write_channel[m_channel_index].running;
	  break;
	}
      }
    }
    
    //send anwser
    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.trdid  = pkt->trdid;
    m_rsp.pktid  = pkt->pktid;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Target %d ] Send answer packet %d with time = %d\n", m_destid , pkt->pktid, (int)(time + delay));
    std::cout << "[MWMR Target " << m_destid << " ] Send answer packet " << pkt->pktid << " with time = " << (int)(time + delay) << std::endl;
#endif
    
    p_vci_target.send(&m_rsp, (time + delay)) ;
  }

  /////////////////////////////////////////////////////////////////////////////  
  // FUNCTION TO RECEIVE WRITE COMMAND FROM NETWORK (TARGET VCI)
  // Packets to target vci DO NOT UPDATE the local time 
  /////////////////////////////////////////////////////////////////////////////  
  tmpl(void)::vciCmdReceived_write(size_t segIndex,
				   soclib::common::Segment &s,
				   soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
				   const tlmt_core::tlmt_time &time,
				   void *private_data)
  {
    int reg;
    typename tlmt_core::tlmt_time delay = pkt->nwords;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Target %d ] Receive from source %d a write packet %d with time = %d\n",m_destid, pkt->srcid, pkt->pktid, (int)time);
    std::cout << "[MWMR Target " <<  m_destid << "] Receive from source " << pkt->srcid << " a write packet " << pkt->pktid << " with time = " << (int)time << std::endl;
#endif
    
    //Update the time local
    c0.update_time(time);

    for(unsigned int i=0; i<pkt->nwords;i++){
      reg = (int)((pkt->address + (i*vci_param::nbytes)) - s.baseAddress()) / vci_param::nbytes;
      if ( reg < MWMR_IOREG_MAX ) { // coprocessor IO register access
	p_config[reg]->send(pkt->buf[i], c0.time());
      }
      else {                      // MWMR channel configuration access (or Reset)
	switch (reg) {
	case MWMR_RESET :
	  m_reset_request = true;
	  break;
	case MWMR_CONFIG_FIFO_NO :
	  m_channel_index = pkt->buf[i];
	  break;
	case MWMR_CONFIG_FIFO_WAY :
	  m_channel_read = !(pkt->buf[i]);
	  break;
	case MWMR_CONFIG_STATUS_ADDR :
	  if(m_channel_read)
	    m_read_channel[m_channel_index].status_address = pkt->buf[i];
	  else
	    m_write_channel[m_channel_index].status_address = pkt->buf[i];
	  break;
	case MWMR_CONFIG_DEPTH :
	  if(m_channel_read)
	    m_read_channel[m_channel_index].depth = pkt->buf[i];
	  else
	    m_write_channel[m_channel_index].depth = pkt->buf[i];
	  break;
	case MWMR_CONFIG_BUFFER_ADDR :
	  if(m_channel_read)
	    m_read_channel[m_channel_index].base_address = pkt->buf[i];
	    else
	    m_write_channel[m_channel_index].base_address = pkt->buf[i];
	  break;
	case MWMR_CONFIG_RUNNING :
	  if(m_channel_read){
	    m_read_channel[m_channel_index].running = pkt->buf[i];
	  }
	  else{
	    m_write_channel[m_channel_index].running = pkt->buf[i];
	  }

	  m_active_event.notify();
	  //m_fifo_event.notify();
	  break;
	} // end switch cell
      }
    }

    //send anwser
    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.trdid  = pkt->trdid;
    m_rsp.pktid  = pkt->pktid;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Target %d ] Send answer packet %d with time = %d\n", m_destid , pkt->pktid, (int)(c0.time() + delay));
    std::cout << "[MWMR Target " <<  m_destid << "] Send answer packet " <<  pkt->pktid << " with time = " <<  (int)(c0.time() + delay) << std::endl;
#endif

    p_vci_target.send(&m_rsp, (c0.time() + delay));
  }

  /////////////////////////////////////////////////////////////////////////////  
  // CALLBACK FUNCTION FOR THE READ REQUEST FROM COPROCESSOR
  /////////////////////////////////////////////////////////////////////////////  
  tmpl(void)::readRequestReceived(soclib::tlmt::fifo_cmd_packet<vci_param> *req,
				  const tlmt_core::tlmt_time &time,
				  void *private_data)
  {
	  int index = (int)(long)private_data;

    //Update the time local
    if(m_read_fifo[index].time < time)
      m_read_fifo[index].time = time;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d ] Receive read request fifo = %d nword = %d with time = %d has data = %d\n", m_srcid, index, req->nwords, (int)m_read_fifo[index].time, (m_read_fifo[index].n_elements >= req->nwords));
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive read request fifo = " << index << " nword = " << req->nwords << " with time = " << (int)m_read_fifo[index].time << " has data = " << (m_read_fifo[index].n_elements >= req->nwords) << std::endl;
#endif

    //add the reading time (reading time equals to number of words)
    m_read_fifo[index].time += req->nwords;

    if ( m_read_fifo[index].n_elements >= req->nwords){ //have element to read 
      for (uint32_t k = 0, j = ((m_read_fifo_depth/vci_param::nbytes) - m_read_fifo[index].n_elements); k < req->nwords; k++, j++)
	req->buf[k] = m_read_fifo[index].data[j];

      m_read_fifo[index].n_elements -= req->nwords;

      if(m_read_fifo[index].n_elements == 0){
	m_read_fifo[index].empty = true;
	m_fifo_event.notify();
      }

      //send awnser
#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send read response to coprocessor\n",m_srcid);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read response to coprocessor" << std::endl;
#endif
      p_read_fifo[index]->send(index,m_read_fifo[index].time);
    } 
    else{
      m_read_request[index].pending = true;
      m_read_request[index].data = req->buf;
      m_read_request[index].n_elements = req->nwords;
      m_read_request[index].time = m_read_fifo[index].time;
    } 
  }

  /////////////////////////////////////////////////////////////////////////////  
  // CALLBACK FUNCTION FOR THE WRITE REQUEST FROM COPROCESSOR
  /////////////////////////////////////////////////////////////////////////////  
  tmpl(void)::writeRequestReceived(soclib::tlmt::fifo_cmd_packet<vci_param> *req,
				   const tlmt_core::tlmt_time &time,
				   void *private_data)
  {
	  int index = (int)(long)private_data;

    //Update the time local
    if(m_write_fifo[index].time < time)
      m_write_fifo[index].time = time;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Receive write request fifo = %d nword = %d with time = %d has space = %d n_elements = %d full = %d\n", m_srcid, index, req->nwords, (int)m_write_fifo[index].time, (((m_write_fifo_depth/vci_param::nbytes) - m_write_fifo[index].n_elements) >= req->nwords), m_write_fifo[index].n_elements,  m_write_fifo[index].full );
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive write request fifo = " <<  index << " nwords = " << req->nwords << " with time = " << (int)m_write_fifo[index].time << " has space = " << (((m_write_fifo_depth/vci_param::nbytes) - m_write_fifo[index].n_elements) >= req->nwords) << " n_elements = " << m_write_fifo[index].n_elements << " full = " << m_write_fifo[index].full << std::endl;
#endif

    //add the writing time (writing time equals to number of words)
    m_write_fifo[index].time += req->nwords;

    if (((m_write_fifo_depth/vci_param::nbytes) - m_write_fifo[index].n_elements) >= req->nwords){ //have space to write

      for (uint32_t k = 0, j = m_write_fifo[index].n_elements; k < req->nwords; k++, j++) 
	m_write_fifo[index].data[j] = req->buf[k];

      m_write_fifo[index].n_elements += req->nwords;

      if(m_write_fifo[index].n_elements == (m_write_fifo_depth/vci_param::nbytes)){
	m_write_fifo[index].full = true;
	m_fifo_event.notify();
      }

      //send awnser
#if MWMR_CONTROLLER_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Send write response to coprocessor\n",m_srcid);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write response to coprocessor" << std::endl;
#endif
      p_write_fifo[index]->send(index,m_write_fifo[index].time);
    }
    else{
      m_write_request[index].pending = true;
      for(uint32_t j=0;j< req->nwords; j++)
	m_write_request[index].data[j] = req->buf[j];
      m_write_request[index].n_elements = req->nwords;
      m_write_request[index].time = m_write_fifo[index].time;
    }
  }

  /////////////////////////////////////////////////////////////////////////////  
  // RESET THE MWMR
  /////////////////////////////////////////////////////////////////////////////  
  tmpl(void)::reset() 
  {
    for ( uint32_t  i = 0 ; i < m_read_channels ; i++ ) {
      m_read_fifo[i].empty       = true;
      m_read_fifo[i].full        = false;
      m_read_request[i].pending  = false;
      m_read_channel[i].running  = false;
    }
    for ( uint32_t  i = 0 ; i < m_write_channels ; i++ ) {
      m_write_fifo[i].empty      = true;
      m_write_fifo[i].full       = false;
      m_write_request[i].pending = false;
      m_write_channel[i].running = false;
    }

   
    //send the anwser to all read fifo 
    for ( uint32_t i = 0; i < m_read_channels; i++)
      p_read_fifo[i]->send(i,c0.time());
    //send the anwser to all write fifo
    for ( uint32_t i = 0; i < m_write_channels; i++)
      p_write_fifo[i]->send(i,c0.time());

    m_reset_request = false;
  }

  /////////////////////////////////////////////////////////////////////////////  
  // GET LOCK  EXECUTE THE LOCKED_READ AND STORE CONDITIONAL OPERATIONS 
  /////////////////////////////////////////////////////////////////////////////  
  tmpl(void)::getLock(typename vci_param::addr_t status_address, uint32_t *status) 
  {
    do{
#if MWMR_CONTROLLER_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] GET LOCK\n",m_srcid);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] GET LOCK" << std::endl;
#endif
      
      m_cmd.cmd     = vci_param::CMD_LOCKED_READ;
      m_cmd.nwords  = 1;
      m_cmd.address = status_address + 12;
      m_cmd.buf     = &status[3]; 
      m_cmd.srcid   = m_srcid; 
      m_cmd.trdid   = 0;
      m_cmd.pktid   = m_pktid;
      m_cmd.be      = 0xF;
      m_cmd.contig  = true;
      
      do{
#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Send locked read packet with time = %d\n",m_srcid,(int)c0.time());
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Send locked read packet with time = " << (int)c0.time() << std::endl;
#endif
	p_vci_initiator.send(&m_cmd, c0.time());
	wait(m_vci_event);
#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d lock = %d\n",m_srcid,(int)c0.time(),status[3]);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)c0.time() << " lock = " << status[3] << std::endl;
#endif
	m_pktid++;

	c0.add_time(1);

      }while(status[3]!=0);

      status[3]     = 1;
      m_cmd.cmd     = vci_param::CMD_STORE_COND;
      m_cmd.nwords  = 1;
      m_cmd.address = status_address + 12;
      m_cmd.buf     = &status[3]; 
      m_cmd.srcid   = m_srcid; 
      m_cmd.trdid   = 0;
      m_cmd.pktid   = m_pktid;
      m_cmd.be      = 0xF;
      m_cmd.contig  = true;
      
#if MWMR_CONTROLLER_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Send store conditional packet with time = %d lock = %d\n",m_srcid, (int)c0.time(),status[3]);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Send store conditional packet with time = " << (int)c0.time() << " lock = " << status[3] << std::endl;
#endif

      p_vci_initiator.send(&m_cmd, c0.time());
      wait(m_vci_event);

#if MWMR_CONTROLLER_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Receive awnser store conditional packet with time = %d lock = %d\n",m_srcid, (int)c0.time(),status[3]);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser store conditional packet with time = " << (int)c0.time() << " lock = " << status[3] << std::endl;
#endif
      m_pktid++;
      
      c0.add_time(1);

    }while(status[3]!=0);

#if MWMR_CONTROLLER_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] END GET LOCK\n",m_srcid);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] END GET LOCK" << std::endl;
#endif
  }

  tmpl(void)::releaseLock(typename vci_param::addr_t status_address, uint32_t *status) 
  {
#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] RELEASE THE LOCK\n",m_srcid);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] RELEASE THE LOCK" << std::endl;
#endif
    status[3]     = 0; //release the lock
    m_cmd.cmd     = vci_param::CMD_WRITE;
    m_cmd.nwords  = 1;
    m_cmd.address = status_address + 12;
    m_cmd.buf     = &status[3]; 
    m_cmd.be      = 0xF;
    m_cmd.srcid   = m_srcid; 
    m_cmd.trdid   = 0;
    m_cmd.pktid   = m_pktid;
    m_cmd.contig  = true;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send write packet with time = %d\n",m_srcid, (int)c0.time());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write packet with time = " << (int)c0.time() << std::endl;
#endif
    p_vci_initiator.send(&m_cmd, c0.time());
    wait(m_vci_event);
    m_pktid++;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Receive awnser write packet with time = %d\n",m_srcid, (int)c0.time());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser write packet with time = " << (int)c0.time() << std::endl;
#endif

    c0.add_time(1);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // READ STATUS OF A DETERMINED CHANNEL
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::readStatus(typename vci_param::addr_t status_address, uint32_t *status) 
  {
    // STATUS[0] = index_read;
    // STATUS[1] = index_write;
    // STATUS[2] = content (capacity)
    // STATUS[3] = lock

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] READ STATUS\n",m_srcid);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] READ STATUS" << std::endl;
#endif

    m_cmd.cmd     = vci_param::CMD_READ;
    m_cmd.nwords  = 3;
    m_cmd.address = status_address;
    m_cmd.buf     = status; 
    m_cmd.be      = 0xF;
    m_cmd.srcid   = m_srcid; 
    m_cmd.trdid   = 0;
    m_cmd.pktid   = m_pktid;
    m_cmd.contig  = true;
    
#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d\n",m_srcid, (int)c0.time());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)c0.time() << std::endl;
#endif

    p_vci_initiator.send(&m_cmd, c0.time());
    wait(m_vci_event);
    m_pktid++;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)c0.time());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)c0.time() << std::endl;
    for(unsigned int j=0; j<m_cmd.nwords; j++){
      fprintf(pFile, "[%.8x] = %.8x\n",(m_cmd.address+(j*vci_param::nbytes)),status[j]);
      std::cout << std::hex << "[" << (m_cmd.address+(j*vci_param::nbytes)) << "] = " << status[j] << std::dec << std::endl;
    }
#endif

    c0.add_time(1);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // UPDATE STATUS OF A DETERMINED CHANNEL
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::updateStatus(typename vci_param::addr_t status_address, uint32_t *status) 
  {
#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] UPDATE STATUS\n",m_srcid);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] UPDATE STATUS" << std::endl;
#endif
    status[3]     = 0; // release the lock 
    m_cmd.cmd     = vci_param::CMD_WRITE;
    m_cmd.nwords  = 4;
    m_cmd.address = status_address;
    m_cmd.buf     = status; 
    m_cmd.be      = 0xF;
    m_cmd.srcid   = m_srcid; 
    m_cmd.trdid   = 0;
    m_cmd.pktid   = m_pktid;
    m_cmd.contig  = true;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d\n",m_srcid, (int)c0.time());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)c0.time() << std::endl;
#endif

    p_vci_initiator.send(&m_cmd, c0.time());
    wait(m_vci_event);
    m_pktid++;

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)c0.time());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)c0.time() << std::endl;
#endif

  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // READ DATA FROM A DETERMINED CHANNEL TO A FIFO
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::readFromChannel(uint32_t fifo_index, uint32_t *status) 
  {

    // STATUS[0] = index_read;
    // STATUS[1] = index_write;
    // STATUS[2] = content (number of elements in the channel)
    // STATUS[3] = lock


#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] BUSY_POSITIONS = %d BASE ADDRESS = %.8x DEPTH = %.8x\n",m_srcid, status[2], m_read_channel[fifo_index].base_address, m_read_channel[fifo_index].depth);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] BUSY_POSITIONS = " << status[2] << std::hex << " BASE ADDRESS = " << m_read_channel[fifo_index].base_address << " DEPTH = " << m_read_channel[fifo_index].depth << std::dec << std::endl;
#endif

    c0.add_time(1);

    ///////// read transfer OK //////////
    if(status[2] >= (m_read_fifo_depth/vci_param::nbytes)){  

      if((status[0]+m_read_fifo_depth)<=m_read_channel[fifo_index].depth){ // send only 1 message

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] READ FIFO %d SEND MESSAGE 1 OF 1\n",m_srcid,fifo_index);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO " << fifo_index << " SEND MESSAGE 1 OF 1" << std::endl;
#endif
	  
	m_cmd.cmd     = vci_param::CMD_READ;
	m_cmd.nwords  = (m_read_fifo_depth/vci_param::nbytes);
	m_cmd.address = (m_read_channel[fifo_index].base_address + status[0]);
	m_cmd.buf     = m_read_fifo[fifo_index].data; 
	m_cmd.srcid   = m_srcid; 
	m_cmd.trdid   = 0;
	m_cmd.pktid   = m_pktid;
	m_cmd.contig  = true; //address contiguous

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d address = %d\n",m_srcid, (int)c0.time(),(m_read_channel[fifo_index].base_address + status[0]));
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)c0.time() << " address = " << (m_read_channel[fifo_index].base_address + status[0]) << std::endl;
#endif

	p_vci_initiator.send(&m_cmd, c0.time());
	wait(m_vci_event);
	m_pktid++;

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)c0.time());
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)c0.time() << std::endl;
	for(unsigned int j=0; j<m_cmd.nwords; j++){
	  fprintf(pFile, "[%.8x] = %.8x\n",(m_cmd.address+(j*vci_param::nbytes)),m_cmd.buf[j]);
	  std::cout << "[" << std::hex << (m_cmd.address+(j*vci_param::nbytes)) << "] = " << m_cmd.buf[j] << std::dec << std::endl;
	}
#endif

	c0.add_time(2);
      }
      else{ // send 2 message
	typename vci_param::data_t data_1[(m_read_fifo_depth/vci_param::nbytes)], data_2[(m_read_fifo_depth/vci_param::nbytes)];
	typename vci_param::addr_t address;
	uint32_t nwords_1, nwords_2;
	uint32_t count = 0;
	for(nwords_1 = 0, address = status[0]; address < m_read_channel[fifo_index].depth; nwords_1++, count++, address+=vci_param::nbytes );
	nwords_2 = (m_read_fifo_depth/vci_param::nbytes) - nwords_1;

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] READ FIFO %d SEND MESSAGE 1 OF 2\n",m_srcid,fifo_index);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO " << fifo_index << " SEND MESSAGE 1 OF 2" << std::endl;
#endif

	m_cmd.cmd     = vci_param::CMD_READ;
	m_cmd.nwords  = nwords_1;
	m_cmd.address = (m_read_channel[fifo_index].base_address + status[0]);
	m_cmd.buf     = data_1; 
	m_cmd.be      = 0xF;
	m_cmd.srcid   = m_srcid; 
	m_cmd.trdid   = 0;
	m_cmd.pktid   = m_pktid;
	m_cmd.contig  = true; 

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d address = %d\n",m_srcid, (int)c0.time(),(m_read_channel[fifo_index].base_address + status[0]));
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)c0.time() << " address = " << (m_read_channel[fifo_index].base_address + status[0]) << std::endl;
#endif
	p_vci_initiator.send(&m_cmd, c0.time());
	wait(m_vci_event);
	m_pktid++;

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)c0.time());
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)c0.time() << std::endl;
	for(unsigned int j=0; j<m_cmd.nwords; j++){
	  fprintf(pFile, "[%.8x] = %.8x\n",(m_cmd.address+(j*vci_param::nbytes)),m_cmd.buf[j]);
	  std::cout << "[" << std::hex << (m_cmd.address+(j*vci_param::nbytes)) << "] = " << m_cmd.buf[j] << std::dec << std::endl;
	}
#endif

	c0.add_time(2);

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] READ FIFO %d SEND MESSAGE 2 OF 2\n",m_srcid,fifo_index);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO " << fifo_index << " SEND MESSAGE 2 OF 2" << std::endl;
#endif
	m_cmd.cmd     = vci_param::CMD_READ;
	m_cmd.nwords  = nwords_2;
	m_cmd.address = m_read_channel[fifo_index].base_address;
	m_cmd.buf     = data_2; 
	m_cmd.srcid   = m_srcid; 
	m_cmd.trdid   = 0;
	m_cmd.pktid   = m_pktid;
	m_cmd.be      = 0xF;
	m_cmd.contig  = true; //address contiguous

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d address = %d\n",m_srcid, (int)c0.time(),m_read_channel[fifo_index].base_address);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)c0.time() << " address =" << m_read_channel[fifo_index].base_address << std::endl;
#endif
	p_vci_initiator.send(&m_cmd, c0.time());
	wait(m_vci_event);
	m_pktid++;

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)c0.time());
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)c0.time() << std::endl;
	for(unsigned int j=0; j<m_cmd.nwords; j++){
	  fprintf(pFile, "[%.8x] = %.8x\n",(m_cmd.address+(j*vci_param::nbytes)),m_cmd.buf[j]);
	  std::cout << "[" << std::hex << (m_cmd.address+(j*vci_param::nbytes)) << "] = " << m_cmd.buf[j] << std::dec << std::endl;
	}
#endif

	c0.add_time(2);

	count = 0;
	for(uint32_t j=0; count<nwords_1; count++, j++)
	  m_read_fifo[fifo_index].data[count] = data_1[j];
	for(uint32_t j=0; count<(m_read_fifo_depth/vci_param::nbytes); count++, j++)
	  m_read_fifo[fifo_index].data[count] = data_2[j];

      }

      // update the fifo state
      m_read_fifo[fifo_index].empty = false;
      m_read_fifo[fifo_index].full = true;
      m_read_fifo[fifo_index].n_elements = (m_read_fifo_depth/vci_param::nbytes);
      m_read_fifo[fifo_index].time = c0.time();

      // update the read pointer
      status[0] = status[0] + m_read_fifo_depth;
      if(status[0] >= m_read_channel[fifo_index].depth){
	status[0] = status[0] - m_read_channel[fifo_index].depth;
      }

      // update the number of elements in the channel
      status[2] -= m_read_fifo_depth;
      
      // release pending fifo
      releasePendingReadFifo(fifo_index);
      
      // update the status descriptor
      updateStatus(m_read_channel[fifo_index].status_address, status);

      c0.add_time(1);

      //update the fifo time
      m_read_fifo[fifo_index].time = c0.time() + tlmt_core::tlmt_time(m_waiting_time);
    }
    else{
#if MWMR_CONTROLLER_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] READ FIFO %d NOT OK\n",m_srcid,fifo_index);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO " << fifo_index << " NOT OK" << std::endl;
#endif

      ///////////// release lock ///////////////////////
      releaseLock(m_read_channel[fifo_index].status_address, status);

      ///////////// update the time ///////////////////
      m_read_fifo[fifo_index].time = c0.time() + tlmt_core::tlmt_time(m_waiting_time);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // WRITE DATA FROM A FIFO TO A DETERMINED CHANNEL
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::writeToChannel(uint32_t fifo_index, uint32_t *status) 
  {
    // STATUS[0] = index_read;
    // STATUS[1] = index_write;
    // STATUS[2] = content (number of elements in the channel)
    // STATUS[3] = lock
    
    c0.add_time(1);
    
    ///////// write transfer OK //////////
    if((m_write_channel[fifo_index].depth - status[2]) >= m_write_fifo_depth){
      do{

#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] BUSY_POSITIONS = %d FREE_POSITIONS = %d BASE ADDRESS = %.8x\n", m_srcid, status[2], (m_write_channel[fifo_index].depth - status[2]), m_write_channel[fifo_index].base_address);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] BUSY_POSITIONS = " << status[2] << " FREE_POSITIONS = " << (m_write_channel[fifo_index].depth - status[2]) << " BASE ADDRESS = " << std::hex << m_write_channel[fifo_index].base_address << std::dec << std::endl;
#endif

	if((status[1]+m_write_fifo_depth)<=m_write_channel[fifo_index].depth){ // send only 1 message

#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO %d SEND MESSAGE 1 OF 1\n",m_srcid,fifo_index);
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO " << fifo_index << " SEND MESSAGE 1 OF 1" << std::endl;
#endif
	  
	  m_cmd.cmd     = vci_param::CMD_WRITE;
	  m_cmd.nwords  = (m_write_fifo_depth/vci_param::nbytes);
	  m_cmd.address = (m_write_channel[fifo_index].base_address + status[1]);
	  m_cmd.buf     = m_write_fifo[fifo_index].data; 
	  m_cmd.srcid   = m_srcid; 
	  m_cmd.trdid   = 0;
	  m_cmd.pktid   = m_pktid;
	  m_cmd.be      = 0xF;
	  m_cmd.contig  = true; //address contiguous
	  
#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] Send write packet with time = %d address = %.8x nwords = %d\n",m_srcid, (int)c0.time(),(m_write_channel[fifo_index].base_address + status[1]),(m_write_fifo_depth/vci_param::nbytes));
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write packet with time = " << (int)c0.time() << std::hex << " address = " << (m_write_channel[fifo_index].base_address + status[1]) << std::dec << " nwords = " << (m_write_fifo_depth/vci_param::nbytes) << std::endl;
#endif
	  
	  p_vci_initiator.send(&m_cmd, c0.time());
	  wait(m_vci_event);
	  m_pktid++;
	  
	  c0.add_time(2);
	  
#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] Receive awnser write packet with time = %d\n",m_srcid, (int)c0.time());
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser write packet with time = " << (int)c0.time() << std::endl;
#endif
	}
	else{ // send 2 message

	  typename vci_param::addr_t address;
	  typename vci_param::data_t data_1[(m_write_fifo_depth/vci_param::nbytes)], data_2[(m_write_fifo_depth/vci_param::nbytes)];
	  uint32_t nwords_1, nwords_2;
	  uint32_t count = 0;
	  for(nwords_1 = 0, address = status[1]; address < m_write_channel[fifo_index].depth; nwords_1++, count++, address+=vci_param::nbytes)
	    data_1[nwords_1] = m_write_fifo[fifo_index].data[count];
	  for(nwords_2=0; count <(m_write_fifo_depth/vci_param::nbytes); nwords_2++,count++)
	    data_2[nwords_2] = m_write_fifo[fifo_index].data[count];
	  
#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO %d SEND MESSAGE 1 OF 2\n",m_srcid,fifo_index);
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO " << fifo_index << " SEND MESSAGE 1 OF 2" << std::endl;
#endif
	  
	  m_cmd.cmd     = vci_param::CMD_WRITE;
	  m_cmd.nwords  = nwords_1;
	  m_cmd.address = (m_write_channel[fifo_index].base_address + status[1]);
	  m_cmd.buf     = data_1; 
	  m_cmd.srcid   = m_srcid; 
	  m_cmd.trdid   = 0;
	  m_cmd.pktid   = m_pktid;
	  m_cmd.be      = 0xF;
	  m_cmd.contig  = true;
	
#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] Send write packet with time = %d address = %d nwords = %d\n",m_srcid, (int)c0.time(),(m_write_channel[fifo_index].base_address + status[1]),nwords_1);
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write packet with time = " << (int)c0.time() << std::hex << " address = " << (m_write_channel[fifo_index].base_address + status[1]) << std::dec << " nwords = " << nwords_1 << std::endl;
#endif
	  p_vci_initiator.send(&m_cmd, c0.time());
	  wait(m_vci_event);
	  m_pktid++;
	  
#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] Receive awnser write packet with time = %d\n",m_srcid, (int)c0.time());
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser write packet with time = " << (int)c0.time() << std::endl;
#endif
	  
	  c0.add_time(2);


#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO %d SEND 2 MESSAGE OF 2\n",m_srcid,fifo_index);
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO " << fifo_index << " SEND 2 MESSAGE OF 2" << std::endl;
#endif
	  
	  m_cmd.cmd     = vci_param::CMD_WRITE;
	  m_cmd.nwords  = nwords_2;
	  m_cmd.address = m_write_channel[fifo_index].base_address;
	  m_cmd.buf     = data_2; 
	  m_cmd.be      = 0xF;
	  m_cmd.srcid   = m_srcid; 
	  m_cmd.trdid   = 0;
	  m_cmd.pktid   = m_pktid;
	  m_cmd.contig  = true; //address contiguous
	  
#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] Send write packet with time = %d nwords = %d\n",m_srcid, (int)c0.time(), nwords_2);
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write packet with time = " << (int)c0.time() << " nwords = " << nwords_2 << std::endl;
#endif
	  p_vci_initiator.send(&m_cmd, c0.time());
	  wait(m_vci_event);
	  m_pktid++;
	  
#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] Receive awnser write packet with time = %d address = %d\n",m_srcid, (int)c0.time(), m_write_channel[fifo_index].base_address);
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser write packet with time = " << (int)c0.time() << std::hex << " address = " << m_write_channel[fifo_index].base_address << std::dec <<std::endl;
#endif
	  
	  c0.add_time(2);
	}
		
	// update the fifo state
	m_write_fifo[fifo_index].empty = true;
	m_write_fifo[fifo_index].full  = false;
	m_write_fifo[fifo_index].n_elements = 0;
	m_write_fifo[fifo_index].time = c0.time();
	
	// update the write pointer
	status[1] = status[1] + m_write_fifo_depth;
	if(status[1] >= m_write_channel[fifo_index].depth)
	  status[1] = status[1] - m_write_channel[fifo_index].depth;
      
	// update the number of elements in the channel
	status[2] += m_write_fifo_depth;

	//// release pending fifo ////
	releasePendingWriteFifo(fifo_index);
      }while(m_write_fifo[fifo_index].full &&  (m_write_fifo[fifo_index].time<=c0.time()) && ((m_write_channel[fifo_index].depth - status[2]) >= m_write_fifo_depth));

      //// update the status descriptor ////
      updateStatus(m_write_channel[fifo_index].status_address, status);

      c0.add_time(1);

      //update the fifo time
      m_write_fifo[fifo_index].time = c0.time() + tlmt_core::tlmt_time(m_waiting_time);
    }
    else{
#if MWMR_CONTROLLER_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO %d NOT OK\n",m_srcid,fifo_index);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO " << fifo_index << " NOT OK" << std::endl;
#endif

      ///////////// release lock ///////////////////////
      releaseLock(m_write_channel[fifo_index].status_address, status);

      ///////////// update the time ///////////////////
      m_write_fifo[fifo_index].time = c0.time() + tlmt_core::tlmt_time(m_waiting_time);
    } 
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // RELEASE THE PENDING READ FIFO
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::releasePendingReadFifo(uint32_t fifo_index)
  {
#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] RELEASE PENDING FIFO\n",m_srcid);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] RELEASE PENDING FIFO" << std::endl;
#endif
  
    if ( m_read_request[fifo_index].pending){ //there is request 
      //copy the data
      for ( uint32_t n = 0 ; n < m_read_request[fifo_index].n_elements; n++ )
	m_read_request[fifo_index].data[n] = m_read_fifo[fifo_index].data[n];
      
      // update the time
      //m_read_fifo[fifo_index].time += m_read_request[fifo_index].n_elements;
      m_read_fifo[fifo_index].n_elements -= m_read_request[fifo_index].n_elements;
      m_read_fifo[fifo_index].full = false;
      if(m_read_fifo[fifo_index].n_elements == 0){
	m_read_fifo[fifo_index].empty = true;
      }
      m_read_request[fifo_index].pending = false;

      // update the time
      if(m_read_fifo[fifo_index].time < c0.time())
	m_read_fifo[fifo_index].time = c0.time();
      
#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send answer to coprocessor time = %d\n",m_srcid,(int)m_read_fifo[fifo_index].time);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send answer to coprocessor time = " << (int)m_read_fifo[fifo_index].time << std::endl;
#endif

      //send awnser to coprocessor
      p_read_fifo[fifo_index]->send(fifo_index, m_read_fifo[fifo_index].time);
      
      // update the time
      //m_read_fifo[fifo_index].time += tlmt_core::tlmt_time(65);
      //m_read_fifo[fifo_index].time = c0.time() + tlmt_core::tlmt_time(64);
    } 
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // RELEASE THE PENDING FIFO
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::releasePendingWriteFifo(uint32_t fifo_index)
  {
#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] RELEASE PENDING FIFO\n",m_srcid);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] RELEASE PENDING FIFO" << std::endl;
#endif
  
    if (m_write_request[fifo_index].pending){
      //copy the data
      for ( uint32_t n = 0 ; n < m_write_request[fifo_index].n_elements; n++)
	m_write_fifo[fifo_index].data[n] = m_write_request[fifo_index].data[n];
      
      m_write_fifo[fifo_index].n_elements += m_write_request[fifo_index].n_elements;
      m_write_fifo[fifo_index].empty = false;
      if(m_write_fifo[fifo_index].n_elements == (m_write_fifo_depth/vci_param::nbytes)){
	m_write_fifo[fifo_index].full = true;
      }

      m_write_request[fifo_index].pending = false;
      
      // update the time
      if(m_write_fifo[fifo_index].time < c0.time())
	m_write_fifo[fifo_index].time = c0.time();

#if MWMR_CONTROLLER_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send answer to coprocessor time = %d\n",m_srcid,(int)m_write_fifo[fifo_index].time);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send answer to coprocessor time = " << (int)m_write_fifo[fifo_index].time << std::endl;
#endif
      
      //send awnser to coprocessor
      p_write_fifo[fifo_index]->send(fifo_index,m_write_fifo[fifo_index].time);
      
    } 
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // EXECUTE THE LOOP
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::execLoop() 
  {
    bool                       fifo_serviceable = false;
    tlmt_core::tlmt_time       fifo_time;
    uint32_t                   fifo_index = 0;
    bool                       fifo_read = false;
    uint32_t                   status[4];
    typename vci_param::addr_t status_address;

    // wait the running register to be set
    c0.disable();
    wait(m_active_event);
    c0.enable();

    while(true) {

      if(m_reset_request){
	reset();
	// wait the running register to be set
	c0.disable();
	wait(m_active_event);
	c0.enable();
      }
      
      //// select the first serviceable FIFO
      //// taking the request time into account 
      //// write FIFOs have the highest priority
      fifo_serviceable = false ; 
      fifo_time = std::numeric_limits<uint32_t>::max();
    
      for (uint32_t  i = 0; i < m_read_channels; i++) {
#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] READ CHANNEL %d EMPTY = %d RUNNING = %d max fifo_time = %d fifo time = %d\n", m_srcid, i, m_read_fifo[i].empty,m_read_channel[i].running, (int)fifo_time, (int)m_read_fifo[i].time);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] READ CHANNEL " << i << " EMPTY = " << m_read_fifo[i].empty << " RUNNING = " << m_read_channel[i].running << " max fifo_time = " << (uint32_t)fifo_time << " fifo time = " << (uint32_t)m_read_fifo[i].time << " current time = " << (uint32_t)c0.time() << std::endl;
#endif
	if ( m_read_fifo[i].empty && m_read_channel[i].running){
	  if (fifo_time >= m_read_fifo[i].time) {
	    fifo_serviceable = true;
	    fifo_time = m_read_fifo[i].time;
	    fifo_index = i;
	    fifo_read  = true;
	  } // end if date
	} // end if valid
      } // end for read fifo

      for (uint32_t  i = 0; i < m_write_channels; i++) {
#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] WRITE CHANNEL %d FULL = %d RUNNING = %d max fifo_time = %d fifo time = %d\n", m_srcid, i, m_write_fifo[i].empty, m_write_channel[i].running, (uint32_t)fifo_time, (uint32_t)m_write_fifo[i].time);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE CHANNEL " << i << " FULL = " << m_write_fifo[i].full << " RUNNING = " << m_write_channel[i].running << " max fifo_time = " << (uint32_t)fifo_time << " fifo time = " << (uint32_t)m_write_fifo[i].time << " current time = " << (uint32_t)c0.time() << std::endl;
#endif
	if ( m_write_fifo[i].full && m_write_channel[i].running) {
	  if (fifo_time >= m_write_fifo[i].time) {
	    fifo_serviceable = true;
	    fifo_time = m_write_fifo[i].time;
	    fifo_index = i;
	    fifo_read  = false;
	  } // end if date
	} // end if valid
      } // end for write fifo
               
      if ( !fifo_serviceable ){
#if MWMR_CONTROLLER_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] FIFO NO SERVICEABLE\n",m_srcid);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] FIFO NO SERVICEABLE" << std::endl;
#endif
	wait(m_fifo_event);
      }
      else{
	//Update Time
	c0.update_time(fifo_time);

	// get the status address
	if (fifo_read) {
#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] READ FIFO SELECTED %d\n", m_srcid, fifo_index);
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO SELECTED " << fifo_index << std::endl;
#endif
	  status_address = m_read_channel[fifo_index].status_address;
	} 
	else {    /////////////////////////////////////////////////////
#if MWMR_CONTROLLER_DEBUG
	  fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO SELECTED %d\n", m_srcid, fifo_index);
	  std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO SELECTED " << fifo_index << std::endl;
#endif
	  status_address = m_write_channel[fifo_index].status_address;
	}

	c0.add_time(2);

	//// get the lock ////
	getLock(status_address,status);
      
	//// read the status ////
	readStatus(status_address,status);

	if (fifo_read){
	  ////// read from channel //////
	  readFromChannel(fifo_index,status);
	}
	else{
	  ////// write to channel //////
	  writeToChannel(fifo_index,status);
	}
      }  
    }
  } // end loopExec        

  tmpl(/**/)::VciMwmrController(sc_core::sc_module_name name,
				const soclib::common::MappingTable &mt,
				const soclib::common::IntTab &initiator_index,
				const soclib::common::IntTab &target_index,
				uint32_t read_fifo_depth,  //in bytes
				uint32_t write_fifo_depth, //in bytes
				uint32_t n_read_channels,
				uint32_t n_write_channels,
				uint32_t n_config,
				uint32_t n_status)
    : soclib::tlmt::BaseModule(name),
      m_mt(mt),
      m_read_fifo_depth(read_fifo_depth),
      m_write_fifo_depth(write_fifo_depth),
      m_read_channels(n_read_channels),
      m_write_channels(n_write_channels),
      m_config_registers(n_config),
      m_status_registers(n_status),
      p_vci_initiator("vci_initiator", new tlmt_core::tlmt_callback<VciMwmrController,soclib::tlmt::vci_rsp_packet<vci_param> *>(this, &VciMwmrController<vci_param>::vciRspReceived),&c0),
      p_vci_target("vci_target", new tlmt_core::tlmt_callback<VciMwmrController,soclib::tlmt::vci_cmd_packet<vci_param> *>(this, &VciMwmrController<vci_param>::vciCmdReceived))
  {
    m_srcid = m_mt.indexForId(initiator_index);
    m_destid = m_mt.indexForId(target_index);
    m_initiator_segments = m_mt.getSegmentList(initiator_index);
    m_target_segments = m_mt.getSegmentList(target_index);
    
#if MWMR_CONTROLLER_DEBUG
    char fileName[50];
    sprintf (fileName, "mwmr%d.txt", m_srcid);
    pFile = fopen(fileName,"w");
#endif

    m_waiting_time = 64;
    m_channel_index = 0;
    m_channel_read = false;
    m_reset_request = false;
    m_pktid = 1;

    m_read_channel = new channel_struct<vci_param>[m_read_channels];
    m_read_request = new request_struct<vci_param>[m_read_channels];
    m_read_fifo    = new fifos_struct<vci_param>[m_read_channels];
    for(uint32_t i=0;i<m_read_channels;i++){
      //Channel
      m_read_channel[i].running   = false ;
      //Requests
      m_read_request[i].data      = new typename vci_param::data_t[(m_read_fifo_depth/vci_param::nbytes)];
      m_read_request[i].pending   = false ;
      m_read_request[i].n_elements = 0;
      //Fifos
      m_read_fifo[i].data         = new typename vci_param::data_t[(m_read_fifo_depth/vci_param::nbytes)];
      m_read_fifo[i].empty        = true;
      m_read_fifo[i].full         = false ;
      m_read_fifo[i].n_elements   = 0;

      std::ostringstream tmpName;
      tmpName << "read_fifo" << i;
      p_read_fifo.push_back(new soclib::tlmt::FifoTarget<vci_param>(tmpName.str().c_str(), new tlmt_core::tlmt_callback<VciMwmrController,soclib::tlmt::fifo_cmd_packet<vci_param> *>(this,&VciMwmrController<vci_param>::readRequestReceived,(int*)i)));
    }

    m_write_channel = new channel_struct<vci_param>[m_write_channels];
    m_write_request = new request_struct<vci_param>[m_write_channels];
    m_write_fifo    = new fifos_struct<vci_param>[m_write_channels];
    for(uint32_t i=0;i<m_write_channels;i++){
      //Channel
      m_write_channel[i].running  = false;
      //Request
      m_write_request[i].data     = new typename vci_param::data_t[(m_write_fifo_depth/vci_param::nbytes)];
      m_write_request[i].pending  = false;
      m_write_request[i].n_elements = 0;
      //Fifos
      m_write_fifo[i].data        = new typename vci_param::data_t[(m_write_fifo_depth/vci_param::nbytes)];
      m_write_fifo[i].empty       = true;
      m_write_fifo[i].full        = false;
      m_write_fifo[i].n_elements  = 0;

      std::ostringstream tmpName;
      tmpName << "write_fifo" << i;
      p_write_fifo.push_back(new soclib::tlmt::FifoTarget<vci_param>(tmpName.str().c_str(), new tlmt_core::tlmt_callback<VciMwmrController,soclib::tlmt::fifo_cmd_packet<vci_param> *>(this,&VciMwmrController<vci_param>::writeRequestReceived,(int*)i)));
    }

    //CONFIG PORTS
    for(uint32_t i=0;i<m_config_registers;i++){
      std::ostringstream tmpName;
      tmpName << "config" << i;
      p_config.push_back(new tlmt_core::tlmt_out<typename vci_param::data_t>(tmpName.str().c_str(),NULL));
    }

    //STATUS PORTS
    for(uint32_t i=0;i<m_status_registers;i++){
      std::ostringstream tmpName;
      tmpName << "status" << i;
      p_status.push_back(new tlmt_core::tlmt_out<typename vci_param::data_t*>(tmpName.str().c_str(),NULL));
    }

    SC_THREAD(execLoop);
    
  }
}}


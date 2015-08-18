#include "vci_param.h"
#include "../include/vci_ram.h"

#ifndef VCI_RAM_DEBUG
#define VCI_RAM_DEBUG 0
#endif

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciRam<vci_param>

  tmpl(void)::callback(soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		       const tlmt_core::tlmt_time &time,
		       void *private_data)
  {
    // First, find the right segment using the first address of the packet
    std::list<soclib::common::Segment>::iterator seg;	
    size_t segIndex;

    for (segIndex=0,seg = m_segments.begin(); seg != m_segments.end(); ++segIndex, ++seg ) {
      soclib::common::Segment &s = *seg;
      if (!s.contains(pkt->address))
	continue;
      
      switch(pkt->cmd){
      case vci_param::CMD_READ :
	return callback_read(segIndex,s,pkt,time,private_data);
	break;
      case vci_param::CMD_WRITE :
	return callback_write(segIndex,s,pkt,time,private_data);
	break;
      case vci_param::CMD_LOCKED_READ :
	return callback_locked_read(segIndex,s,pkt,time,private_data);
	break;
      case vci_param::CMD_STORE_COND :
	return callback_store_cond(segIndex,s,pkt,time,private_data);
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

#if VCI_RAM_DEBUG
    std::cout << "[RAM " << m_id << "] Address " << pkt->address << " does not match any segment " << std::endl;
    std::cout << "[RAM " << m_id << "] Send to source "<< pkt->srcid << " a error packet with time = "  << time + delay << std::endl;
#endif
    p_vci.send(&m_rsp, time + delay);
  }

  tmpl(void)::callback_read(size_t segIndex,
			    soclib::common::Segment &s,
			    soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
			    const tlmt_core::tlmt_time &time,
			    void *private_data)
  {
#if VCI_RAM_DEBUG
    std::cout << "[RAM " << m_id <<"] Receive from source " << pkt->srcid <<" a Read packet " << pkt->pktid << " Time = "  << time << std::endl;
#endif

    typename vci_param::addr_t address;
    for (size_t i=0;i<pkt->nwords;i++){
      if (pkt->contig) 
	address = (pkt->address+(i*vci_param::nbytes)) - s.baseAddress();
      else
	address = pkt->address - s.baseAddress(); //always the same address

      pkt->buf[i]= m_contents[segIndex][address / vci_param::nbytes];
    }

    tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords); 
    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.pktid  = pkt->pktid;
    m_rsp.trdid  = pkt->trdid;

#if VCI_RAM_DEBUG
    std::cout << "[RAM " << m_id << "] Send to source "<< pkt->srcid << " a anwser packet " << pkt->pktid << " Time = "  << time + delay << std::endl;
#endif

    m_cpt_cycle = time + delay;
    m_cpt_read_packet++;
    m_cpt_read += pkt->nwords;
    if(pkt->nwords == 1)
      m_cpt_read_1++;

    p_vci.send(&m_rsp, time + delay);
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  // CALLBACK FUNCTION TO LOCKED READ COMMAND
  //////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::callback_locked_read(size_t segIndex,
				   soclib::common::Segment &s,
				   soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
				   const tlmt_core::tlmt_time &time,
				   void *private_data)
  {
#if VCI_RAM_DEBUG
    std::cout << "[RAM " << m_id <<"] Receive from source " << pkt->srcid <<" a Locked Read packet " << pkt->pktid << " Time = "  << time << std::endl;
#endif

    typename vci_param::addr_t address;
    for (size_t i=0; i<pkt->nwords; i++){
      if (pkt->contig)
	address = (pkt->address+(i*vci_param::nbytes)) - s.baseAddress();
      else
	address = pkt->address - s.baseAddress(); //always the same address

      pkt->buf[i]= m_contents[segIndex][address / vci_param::nbytes];
      m_atomic.doLoadLinked(address, pkt->srcid);
    }

    tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords); 
    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.pktid  = pkt->pktid;
    m_rsp.trdid  = pkt->trdid;

#if VCI_RAM_DEBUG
    std::cout << "[RAM " << m_id << "] Send to source "<< pkt->srcid << " a anwser packet " << pkt->pktid << " Time = "  << time + delay << std::endl;
#endif

    m_cpt_cycle = time + delay;
    m_cpt_read_packet++;
    m_cpt_read += pkt->nwords;
    if(pkt->nwords == 1)
      m_cpt_read_1++;

    p_vci.send(&m_rsp, time + delay);
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  // CALLBACK FUNCTION TO WRITE COMMAND
  //////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::callback_write(size_t segIndex,
			     soclib::common::Segment &s,
			     soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
			     const tlmt_core::tlmt_time &time,
			     void *private_data)
  {
#if VCI_RAM_DEBUG
    std::cout << "[RAM " << m_id << "] Receive from source " << pkt->srcid <<" a Write packet "<< pkt->pktid << " Time = "  << time << std::endl;
    for(uint32_t j=0;j< pkt->nwords; j++){
      if(pkt->contig)
	std::cout << std::hex << "[" << (pkt->address + (j*vci_param::nbytes)) << "] = " << pkt->buf[j] << std::dec << std::endl;
      else
	std::cout << std::hex << "[" << pkt->address << "] = " << pkt->buf[j] << std::dec << std::endl;
    }     
#endif

    typename vci_param::addr_t address;
    for (size_t i=0; i<pkt->nwords; i++){
      if(pkt->contig)
	address = (pkt->address+(i*vci_param::nbytes)) - s.baseAddress();
      else
	address = pkt->address - s.baseAddress();

      m_atomic.accessDone(address);

      uint32_t index   = address / vci_param::nbytes;
      ram_t *tab       = m_contents[segIndex];
      unsigned int cur = tab[index];
      uint32_t mask    = 0;
      unsigned int be  = pkt->be;

      if ( be & 1 )
	mask |= 0x000000ff;
      if ( be & 2 )
	mask |= 0x0000ff00;
      if ( be & 4 )
	mask |= 0x00ff0000;
      if ( be & 8 )
	mask |= 0xff000000;
      
      tab[index] = (cur & ~mask) | (pkt->buf[i] & mask);
    }

    tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords);
    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.pktid  = pkt->pktid;
    m_rsp.trdid  = pkt->trdid;

    m_cpt_cycle = time + delay;
    m_cpt_write_packet++;
    m_cpt_write += pkt->nwords;
    if(pkt->nwords == 1)
      m_cpt_write_1++;
    
#if VCI_RAM_DEBUG
    std::cout << "[RAM " << m_id << "] Send to source "<< pkt->srcid << " a anwser packet " << pkt->pktid << " Time = "  << time + delay  << std::endl;
#endif

    p_vci.send(&m_rsp,  time + delay);
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  // CALLBACK FUNCTION TO STORE CONDITIONNEL COMMAND
  //////////////////////////////////////////////////////////////////////////////////////////
  tmpl(void)::callback_store_cond(size_t segIndex,
				  soclib::common::Segment &s,
				  soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
				  const tlmt_core::tlmt_time &time,
				  void *private_data)
  {
    typename vci_param::addr_t address;
    
    tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords);

#if VCI_RAM_DEBUG
    std::cout << "[RAM " << m_id << "] Receive from source " << pkt->srcid <<" a Store Conditionnel packet "<< pkt->pktid << " Time = "  << time << std::endl;
    for(uint32_t j=0;j< pkt->nwords; j++){
      if(pkt->contig)
	std::cout << std::hex << "[" << (pkt->address + (j*vci_param::nbytes)) << "] = " << pkt->buf[j] << std::dec << std::endl;
      else
	std::cout << std::hex << "[" << pkt->address << "] = " << pkt->buf[j] << std::dec << std::endl;
    }         
#endif

    for (size_t i=0; i<pkt->nwords; i++){
      if(pkt->contig)
	address = (pkt->address + (i*vci_param::nbytes)) - s.baseAddress();
      else
	address = pkt->address - s.baseAddress();
	
      if(m_atomic.isAtomic(address,pkt->srcid)){
	m_atomic.accessDone(address);
      
	uint32_t index   = address / vci_param::nbytes;
	ram_t *tab       = m_contents[segIndex];
	unsigned int cur = tab[index];
	uint32_t mask    = 0;
	unsigned int be  = pkt->be;
	
	if ( be & 1 )
	  mask |= 0x000000ff;
	if ( be & 2 )
	  mask |= 0x0000ff00;
	if ( be & 4 )
	  mask |= 0x00ff0000;
	if ( be & 8 )
	  mask |= 0xff000000;
      
	tab[index] = (cur & ~mask) | (pkt->buf[i] & mask);
	pkt->buf[i] = 0;
      }
      else{
	pkt->buf[i] = 1;
      }
    }

    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.pktid  = pkt->pktid;
    m_rsp.trdid  = pkt->trdid;
    
#if VCI_RAM_DEBUG
    std::cout << "[RAM " << m_id << "] Send to source "<< pkt->srcid << " a anwser packet " << pkt->pktid << " Time = "  << time + delay  << std::endl;
#endif

    m_cpt_cycle = time + delay;
    m_cpt_write_packet++;
    m_cpt_write += pkt->nwords;
    if(pkt->nwords == 1)
      m_cpt_write_1++;

    p_vci.send(&m_rsp,  time + delay);
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  // CONSTRUCTOR
  //////////////////////////////////////////////////////////////////////////////////////////
  tmpl(/**/)::VciRam(
		     sc_core::sc_module_name name,
		     uint32_t id,
		     const soclib::common::IntTab &index,
		     const soclib::common::MappingTable &mt,
		     common::Loader &loader)
    : soclib::tlmt::BaseModule(name),
      m_index(index),
      m_mt(mt),
      m_loader(new common::Loader(loader)),
      m_atomic(256),// 256 equals to maximal number of initiator
      p_vci("vci", new tlmt_core::tlmt_callback<VciRam,soclib::tlmt::vci_cmd_packet<vci_param> *>(this, &VciRam<vci_param>::callback))
  {
    m_id = id;
    m_segments=m_mt.getSegmentList(m_index);
    m_contents = new ram_t*[m_segments.size()];
    size_t word_size = sizeof(typename vci_param::data_t);

    std::list<soclib::common::Segment>::iterator seg;
    size_t i;
    for (i=0, seg = m_segments.begin(); seg != m_segments.end(); ++i, ++seg ) {
      soclib::common::Segment &s = *seg;
      m_contents[i] = new ram_t[(s.size()+word_size-1)/word_size];
    }
    
    if ( m_loader ){
      for (i=0, seg = m_segments.begin(); seg != m_segments.end(); ++i, ++seg ) {
	soclib::common::Segment &s = *seg;
	m_loader->load(&m_contents[i][0], s.baseAddress(), s.size());
	for (size_t addr = 0; addr < s.size()/word_size; ++addr )
	  m_contents[i][addr] = le_to_machine(m_contents[i][addr]);
      }
    }

    //initialize the control table LL/SC
    m_atomic.clearAll();

    m_cpt_cycle        = 0;
    m_cpt_read         = 0;
    m_cpt_read_1       = 0;
    m_cpt_read_packet  = 0;
    m_cpt_write        = 0;
    m_cpt_write_1      = 0;
    m_cpt_write_packet = 0;
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  // CONSTRUCTOR
  //////////////////////////////////////////////////////////////////////////////////////////
  tmpl(/**/)::VciRam(
		     sc_core::sc_module_name name,
		     const soclib::common::IntTab &index,
		     const soclib::common::MappingTable &mt,
		     common::Loader &loader)
    : soclib::tlmt::BaseModule(name),
      m_index(index),
      m_mt(mt),
      m_loader(new common::Loader(loader)),
      m_atomic(256),// 256 equals to maximal number of initiator
      p_vci("vci", new tlmt_core::tlmt_callback<VciRam,soclib::tlmt::vci_cmd_packet<vci_param> *>(this, &VciRam<vci_param>::callback))
  {
    m_id = m_mt.indexForId(index);
    m_segments=m_mt.getSegmentList(m_index);
    m_contents = new ram_t*[m_segments.size()];
    size_t word_size = sizeof(typename vci_param::data_t);

    std::list<soclib::common::Segment>::iterator seg;
    size_t i;
    for (i=0, seg = m_segments.begin(); seg != m_segments.end(); ++i, ++seg ) {
      soclib::common::Segment &s = *seg;
      m_contents[i] = new ram_t[(s.size()+word_size-1)/word_size];
    }
    
    if ( m_loader ){
      for (i=0, seg = m_segments.begin(); seg != m_segments.end(); ++i, ++seg ) {
	soclib::common::Segment &s = *seg;
	m_loader->load(&m_contents[i][0], s.baseAddress(), s.size());
	for (size_t addr = 0; addr < s.size()/word_size; ++addr )
	  m_contents[i][addr] = le_to_machine(m_contents[i][addr]);
      }
    }

    //initialize the control table LL/SC
    m_atomic.clearAll();

    m_cpt_cycle        = 0;
    m_cpt_read         = 0;
    m_cpt_read_1       = 0;
    m_cpt_read_packet  = 0;
    m_cpt_write        = 0;
    m_cpt_write_1      = 0;
    m_cpt_write_packet = 0;
  }

  tmpl(/**/)::~VciRam(){}

  tmpl(int)::getTotalCycles()
  {
    return m_cpt_cycle;
  }
  
  tmpl(int)::getActiveCycles()
  {
    return (m_cpt_read + m_cpt_write);
  }
  
  tmpl(int)::getIdleCycles()
  {
    return (m_cpt_cycle - (m_cpt_read + m_cpt_write));
  }
  
  tmpl(int)::getNReadPacket()
  {
    return m_cpt_read_packet;
  }
  
  tmpl(int)::getNWritePacket()
  {
    return m_cpt_write_packet;
  }
  
  tmpl(int)::getNReadPacket_1word()
  {
    return m_cpt_read_1;
  }
  
  tmpl(int)::getNWritePacket_1word()
  {
    return m_cpt_write_1;
  }
  
  tmpl(int)::getNReadPacket_Nwords()
  {
    return (m_cpt_read_packet - m_cpt_read_1);
  }
  
  tmpl(int)::getNWritePacket_Nwords()
  {
    return (m_cpt_write_packet - m_cpt_write_1);
  }
 
}}


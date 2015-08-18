#include "vci_param.h"
#include "vci_multi_tty.h"
#include "tty.h"
#include <stdarg.h>

#ifndef MULTI_TTY_DEBUG
#define MULTI_TTY_DEBUG 0
#endif

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciMultiTty<vci_param>

  tmpl(void)::callback(soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		       const tlmt_core::tlmt_time &time,
		       void *private_data)
  {
    std::list<soclib::common::Segment>::iterator seg;
    size_t segIndex;
    for (segIndex=0,seg = segList.begin();seg != segList.end(); ++segIndex, ++seg ){
      soclib::common::Segment &s = *seg;

      if (!s.contains(pkt->address))
	continue;
      switch(pkt->cmd){
      case vci_param::CMD_READ:
	callback_read(segIndex,s,pkt,time,private_data);
	break;
      case vci_param::CMD_WRITE:
	callback_write(segIndex,s,pkt,time,private_data);
	break;
      default:
	break;
      }
      return;
    }
    std::cout << "Address does not match any segment" << std::endl;
  }
    
  tmpl(void)::callback_read(size_t segIndex,
			    soclib::common::Segment &s,
			    soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
			    const tlmt_core::tlmt_time &time,
			    void *private_data){
      
#if MULTI_TTY_DEBUG
    std::cout << "[TTY] Receive a read packet with time = "  << time << std::endl;
#endif
      
    uint32_t localbuf[200];
    int cell, reg, term_no;
    tlmt_core::tlmt_time delay = pkt->nwords + (pkt->nwords-1);

    m_cpt_idle = m_cpt_idle + ((int)time - m_cpt_cycle);
    m_cpt_cycle = (int)(time + delay);
    m_cpt_read++;
      
    for(unsigned int i=0; i<pkt->nwords;i++){

      if (pkt->contig) {
	cell = (int)(((pkt->address+(i*vci_param::nbytes)) - s.baseAddress()) / vci_param::nbytes);
      }
      else{
	cell = (int)((pkt->address - s.baseAddress()) / vci_param::nbytes); // always write in the same address
      }
      
      reg = cell % TTY_SPAN;
      term_no = cell / TTY_SPAN;

#if MULTI_TTY_DEBUG
      std::cout << "[TTY] term_no=" << term_no << " reg=" << reg << std::endl;
#endif
	
      if (term_no>=(int)m_term.size()){

#if MULTI_TTY_DEBUG
	std::cout << "term_no (" << term_no <<") greater than the maximum (" << m_term.size() << ")" << std::endl;
#endif

	// remplir paquet d'erreur
	m_rsp.error = true;
	return;
      }
	
      switch (reg) {
      case TTY_STATUS:
	localbuf[i] = m_term[term_no]->hasData();
	m_rsp.error = false;
	break;
	  
      case TTY_READ:
	if (m_term[term_no]->hasData()) {
	  char tmp = m_term[term_no]->getc();
	  localbuf[i] = tmp;
	}
	m_rsp.error = false;
	break;
	
      default:
	//error message
	m_rsp.error = true;
	break;
      }
    }

    pkt->buf = localbuf;
      
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.trdid  = pkt->trdid;
    m_rsp.pktid  = pkt->pktid;

#if MULTI_TTY_DEBUG
    std::cout << "[TTY] Send answer with time = " << time + tlmt_core::tlmt_time(50) << std::endl;
#endif

    p_vci.send(&m_rsp, time + delay) ;
  }
    
  tmpl(void)::callback_write(size_t segIndex,
			     soclib::common::Segment &s,
			     soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
			     const tlmt_core::tlmt_time &time,
			     void *private_data) {

#if MULTI_TTY_DEBUG
    std::cout << "[TTY] Receive a write packet with time = "  << time << std::endl;
#endif
      
    int cell, reg, term_no;
    char data;
    tlmt_core::tlmt_time delay = pkt->nwords + (pkt->nwords-1);

    m_cpt_idle = m_cpt_idle + ((int)time - m_cpt_cycle);
    m_cpt_cycle = (int)(time + delay);
    m_cpt_write++;
    for(unsigned int i=0; i<pkt->nwords;i++){
      if (pkt->contig) {
	cell = (int)(((pkt->address+(i*vci_param::nbytes)) - s.baseAddress()) / vci_param::nbytes);
      }
      else{
	cell = (int)((pkt->address - s.baseAddress()) / vci_param::nbytes); // always write in the same address
      }

      reg = cell % TTY_SPAN;
      term_no = cell / TTY_SPAN;
      data = pkt->buf[i];

#if MULTI_TTY_DEBUG
      std::cout << "[TTY] term_no=" << term_no << " reg=" << reg << " data=" << data << std::endl;
#endif

      if (term_no>=(int)m_term.size()){
#if MULTI_TTY_DEBUG
	std::cout << "term_no (" << term_no <<") greater than the maximum (" << m_term.size() << ")" << std::endl;
#endif

	// remplir paquet d'erreur
	m_rsp.error= true;
	return;  
      }
	
      switch (reg) {
      case TTY_WRITE:
	if ( data == '\a' ) {
	  char tmp[32];
	  size_t ret = snprintf(tmp, sizeof(tmp), "[%d] ", (int)time);

	  for ( size_t i=0; i<ret; ++i )
	    m_term[term_no]->putc( tmp[i] );
	} 
	else
	  m_term[term_no]->putc( data );

	m_rsp.error = false;
	break;

      default:
	//error message
	m_rsp.error= true;
	break;
      }
	
    }
      
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.pktid  = pkt->pktid;
    m_rsp.trdid  = pkt->trdid;
      
#if MULTI_TTY_DEBUG
    std::cout << "[TTY] Send answer with time = " << time + tlmt_core::tlmt_time(50) << std::endl;
#endif

    p_vci.send(&m_rsp, time + delay);
  }

  tmpl(void)::init(const std::vector<std::string> &names){
    segList=m_mt.getSegmentList(m_index);
    int j=0;

	p_irq.resize(names.size());

    for(std::vector<std::string>::const_iterator i = names.begin();i != names.end();++i){
      m_term.push_back(soclib::common::allocateTty(*i));

      std::ostringstream tmpName;
      tmpName << "irq" << j;
	  p_irq.set(j, new tlmt_core::tlmt_out<bool>(tmpName.str().c_str(),NULL));
      j++;
    }
    m_cpt_cycle = 0;
    m_cpt_idle = 0;
    m_cpt_read = 0;
    m_cpt_write = 0;
  }

  tmpl(/**/)::VciMultiTty(sc_core::sc_module_name name,
			  const soclib::common::IntTab &index,
			  const soclib::common::MappingTable &mt,
			  const char *first_name,
			  ...)
    : tlmt_core::tlmt_module(name),
      m_index(index),
      m_mt(mt),
      p_vci("vci", new tlmt_core::tlmt_callback<VciMultiTty,soclib::tlmt::vci_cmd_packet<vci_param> *>(this, &VciMultiTty<vci_param>::callback))
  {
    va_list va_tty;
    va_start (va_tty, first_name);
    std::vector<std::string> names;
    const char *cur_tty = first_name;
    while (cur_tty) {
      names.push_back(cur_tty);
      cur_tty = va_arg( va_tty, char * );
    }
    va_end( va_tty );
    init(names);
  }

  tmpl(/**/)::VciMultiTty(sc_core::sc_module_name name,
			  const soclib::common::IntTab &index,
			  const soclib::common::MappingTable &mt,
			  const std::vector<std::string> &names)
    : tlmt_core::tlmt_module(name),
      m_index(index),
      m_mt(mt),
      p_vci("vci", new tlmt_core::tlmt_callback<VciMultiTty,soclib::tlmt::vci_cmd_packet<vci_param> *>(this, &VciMultiTty<vci_param>::callback))
  {
    init(names);
  }

  tmpl(size_t)::getTotalCycles(){
    return m_cpt_cycle;
  }

  tmpl(size_t)::getActiveCycles(){
    return (m_cpt_cycle - m_cpt_idle);
  }

  tmpl(size_t)::getIdleCycles(){
    return m_cpt_idle;
  }

  tmpl(size_t)::getNRead(){
    return m_cpt_read;
  }

  tmpl(size_t)::getNWrite(){
    return m_cpt_write;
  }

}
}

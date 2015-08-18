/* -*- mode: c++; coding: utf-8 -*-
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
 *
 * Maintainers: fpecheux, nipo, alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Nicolas Pouillon <nipo@ssji.net>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include "vci_timer.h"

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciTimer<vci_param>

  tmpl(void)::callback(soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		       const tlmt_core::tlmt_time &time,
		       void *private_data)
  {
    std::list<soclib::common::Segment>::iterator seg;
    size_t segIndex;
    for (segIndex=0,seg = m_segList.begin();seg != m_segList.end(); ++segIndex, ++seg ){
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
    
#if SOCLIB_MODULE_DEBUG
    std::cout << "[TIMER] Receive a read packet with time = "  << time << std::endl;
#endif
    
    uint32_t localbuf[32];
    int cell, reg, t;
    
    for(unsigned int i=0; i<pkt->nwords;i++){
      if(pkt->contig)
	cell = (int)(pkt->address + (i*vci_param::nbytes) - s.baseAddress()) / vci_param::nbytes;
      else
	cell = (int)(pkt->address - s.baseAddress()) / vci_param::nbytes; //always the same address

      reg = cell % TIMER_SPAN;
      t = cell / TIMER_SPAN;
      
      
      if (t>=(int)m_ntimer){
	// remplir paquet d'erreur
	m_rsp.error  = true;
	m_rsp.nwords = pkt->nwords;
	m_rsp.srcid  = pkt->srcid;
	m_rsp.trdid  = pkt->trdid;
	m_rsp.pktid  = pkt->pktid;
	p_vci.send(&m_rsp, time + tlmt_core::tlmt_time(50)) ;
	return;
      }
      
      switch (reg) {
      case TIMER_VALUE:
	localbuf[i] = m_timer[t].value;
	break;
	
      case TIMER_PERIOD:
	localbuf[i] = m_timer[t].period;
	break;
	
      case TIMER_MODE:
	localbuf[i] = m_timer[t].mode;
	break;
	
	case TIMER_RESETIRQ:
	  if(time >= (tlmt_core::tlmt_time)m_timer[t].value)
	    localbuf[i] = 1;
	  else
	    localbuf[i] = 0;
	  break;
      }
    }

    pkt->buf = localbuf;
    
    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.trdid  = pkt->trdid;
    m_rsp.pktid  = pkt->pktid;

#if SOCLIB_MODULE_DEBUG
    std::cout << "[TIMER] Send answer with time = " << time + tlmt_core::tlmt_time(50) << std::endl;
#endif

    p_vci.send(&m_rsp, time + tlmt_core::tlmt_time(50)) ;
  }
    
    tmpl(void)::callback_write(size_t segIndex,
			       soclib::common::Segment &s,
			       soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
			       const tlmt_core::tlmt_time &time,
			       void *private_data) {
    
#if SOCLIB_MODULE_DEBUG
    std::cout << "[TIMER] Receive a write packet with time = "  << time << std::endl;
#endif
    
    int cell, reg, t;
    
    for(unsigned int i=0; i<pkt->nwords;i++){
      if(pkt->contig)
	cell = (int)(pkt->address + (i*vci_param::nbytes) - s.baseAddress()) / vci_param::nbytes;
      else
	cell = (int)(pkt->address - s.baseAddress()) / vci_param::nbytes; //always the same address

      reg = cell % TIMER_SPAN;
      t = cell / TIMER_SPAN;
      
      if (t>=(int)m_ntimer){
	// remplir paquet d'erreur
#if SOCLIB_MODULE_DEBUG
	std::cout << " t value bigger than the acceptable  t = " << t << std::endl;
#endif
	m_rsp.error  = true;
	m_rsp.nwords = pkt->nwords;
	m_rsp.srcid  = pkt->srcid;
	m_rsp.trdid  = pkt->trdid;
	m_rsp.pktid  = pkt->pktid;
	p_vci.send(&m_rsp, time + tlmt_core::tlmt_time(50)) ;
	return;
      }
      
      switch (reg) {
      case TIMER_VALUE:
	//the writing in this register occurs only when:
	//1) the reset is actived
	//2) it occurs a reading in "period register"
	break;
	
      case TIMER_RESETIRQ:
	//disable an interruption
	//irq[t]->send(false,time);
	//generate a new interruption
	m_timer[t].value = m_timer[t].value + m_timer[t].period;

#if SOCLIB_MODULE_DEBUG
	std::cout << "[TIMER] Send Interruption " << t <<" with time = " << m_timer[t].value << std::endl;
#endif

	p_irq[t]->send(true,tlmt_core::tlmt_time(m_timer[t].value));
	break;
	
      case TIMER_MODE:
	m_timer[t].mode = (int)pkt->buf[i] & 0x3;
	break;
	
      case TIMER_PERIOD:
	m_timer[t].period = pkt->buf[i];
	m_timer[t].value = time;
	break;
      }
    }
    
    m_rsp.error  = false;
    m_rsp.nwords = pkt->nwords;
    m_rsp.srcid  = pkt->srcid;
    m_rsp.trdid  = pkt->trdid;
    m_rsp.pktid  = pkt->pktid;
    
#if SOCLIB_MODULE_DEBUG
    std::cout << "[TIMER] Send answer with time = " << time + tlmt_core::tlmt_time(50) << std::endl;
#endif

    p_vci.send(&m_rsp, time + tlmt_core::tlmt_time(50));
  }
  
  tmpl(/**/)::VciTimer(
			 sc_core::sc_module_name name,
			 const soclib::common::IntTab &index,
			 const soclib::common::MappingTable &mt,
			 size_t ntimer)
    : soclib::tlmt::BaseModule(name),
      m_index(index),
      m_mt(mt),
      m_ntimer(ntimer),
      p_vci("vci", new tlmt_core::tlmt_callback<VciTimer,soclib::tlmt::vci_cmd_packet<vci_param> *>(this, &VciTimer<vci_param>::callback))
  {
    m_segList=m_mt.getSegmentList(m_index);
    
    m_timer = new timer_struct<typename vci_param::data_t>[m_ntimer];
    
    for(unsigned int i=0;i<m_ntimer;i++){
      m_timer[i].period = 0;
      m_timer[i].value = 0;
      m_timer[i].mode = 0;
      
      std::ostringstream tmpName;
      tmpName << "irq" << i;
      p_irq.push_back(new tlmt_core::tlmt_out<bool>(tmpName.str().c_str(),NULL));
      
    }
  }
  
}}

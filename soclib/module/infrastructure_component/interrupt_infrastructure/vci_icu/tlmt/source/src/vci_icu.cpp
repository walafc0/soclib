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

#include "vci_icu.h"

//ICU_INT = 0 read-only
//ICU_MASK = 1 read-only
//ICU_MASK_SET = 2 write-only
//ICU_MASK_CLEAR = 3 write-only
//ICU_IT_VECTOR = 4 read-only

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciIcu<vci_param>

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
      
#if SOCLIB_MODULE_DEBUG
    std::cout << "[ICU] Receive a read packet with time = "  << time << std::endl;
#endif
      
    uint32_t localbuf[32];
    int reg;

    for(unsigned int i=0; i<pkt->nwords;i++){
      if(pkt->contig)
	reg = (int)((pkt->address + (i*vci_param::nbytes)) - s.baseAddress()) / vci_param::nbytes;
      else
	reg = (int)(pkt->address-s.baseAddress()) / vci_param::nbytes; //always the same address
	
      switch (reg) {
      case ICU_INT:
	localbuf[i] = getActiveInterruptions(time);
	rsp.error = false;
	break;
	  
      case ICU_MASK:
	localbuf[i] = r_mask;
	rsp.error = false;
	break;
	  
      case ICU_IT_VECTOR:
	// give the highest priority interrupt
	localbuf[i] = r_current;
	rsp.error = false;
	break;
	  
      default:
	//send error message
	rsp.error = true;
	break;
      }
    }
      
    pkt->buf = localbuf;
      
    tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords); 

    rsp.nwords = pkt->nwords;
    rsp.srcid  = pkt->srcid;
    rsp.trdid  = pkt->trdid;
    rsp.pktid  = pkt->pktid;

#if SOCLIB_MODULE_DEBUG
    std::cout << "[ICU] Send Answer Time = " << time + delay << std::endl;
#endif

    p_vci.send(&rsp, time + delay) ;
  }

  tmpl(void)::callback_write(size_t segIndex,
			     soclib::common::Segment &s,
			     soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
			     const tlmt_core::tlmt_time &time,
			     void *private_data) {
    
#if SOCLIB_MODULE_DEBUG
    std::cout << "[ICU] Receive a write packet with time = "  << time << std::endl;
#endif
    
    int reg;
    
    for(unsigned int i=0; i<pkt->nwords;i++){
      if(pkt->contig)
	reg = (int)(pkt->address + (i * vci_param::nbytes) - s.baseAddress()) / vci_param::nbytes;                                                  
      else
	reg = (int)(pkt->address-s.baseAddress()) / vci_param::nbytes;

      switch (reg) {
      case ICU_MASK_SET:
	r_mask = r_mask | pkt->buf[i];
	rsp.error = false;
	break;
	  
      case ICU_MASK_CLEAR:
	r_mask = r_mask & ~(pkt->buf[i]);
	rsp.error = false;
	break;
	  
      default:
	//send error message
	rsp.error = true;
	break;
      }
    }

    tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords); 
      
    rsp.nwords = pkt->nwords;
    rsp.srcid  = pkt->srcid;
    rsp.pktid  = pkt->pktid;
    rsp.trdid  = pkt->trdid;
      
#if SOCLIB_MODULE_DEBUG
    std::cout << "[ICU] Send answer with time = " << time + delay << std::endl;
#endif
    p_vci.send(&rsp, time + delay);
  }

  tmpl(void)::irqReceived(bool val,
			  const tlmt_core::tlmt_time &time,
			  void *private_data)
  {
    int idx = (int)private_data;
    irq[idx].val=val;
    irq[idx].time=time;
    if(val){
      behavior();
    }
  }

  tmpl(void)::behavior()
  {
    int idx = getInterruption();
    switch (idx) {
    case -1:        // no interruption available
      break;
    default:        // idx contains the index of higher interruption
#if SOCLIB_MODULE_DEBUG
      std::cout << "[ICU] Send Interruption " << idx << " with time = " << irq[idx].time << std::endl;
#endif
      r_current = idx;
      p_irqOut.send(irq[idx].val,irq[idx].time);
      break;
    }
  }

  tmpl(int)::getInterruption(){
    tlmt_core::tlmt_time min_time=std::numeric_limits<uint32_t>::max();
    int min_index=-1;
    unsigned int mask;

    // starting with interruption with higher priority
    for (unsigned int j=0;j<m_nirq;j++) {
      // If the interruption is active
      if (irq[j].val){
	//verify if the interruption is r_mask=true
	mask = 0;
	mask = (1 << j);
	if((r_mask & mask) == mask){
	  //verify if the interruption has the minor timer
	  if(irq[j].time < min_time) {
	    min_time=irq[j].time;
	    min_index=j;
	  }
	}
      }
      else{
	//All masked interruption must have a time, if there is one desactive interruption then it waits
	return -1;
      }
    }
    return min_index;
  }

  tmpl(unsigned int)::getActiveInterruptions(const tlmt_core::tlmt_time time){
    unsigned int r_interrupt = 0x00000000;

    // starting with interruption with higher priority
    for (unsigned int j=0;j<m_nirq;j++) {
      // If the interruption is active and time is greater or equals to m_fifos_time[j]
      if (irq[j].val && time >= irq[j].time){
	r_interrupt = r_interrupt | (1 << j);
      }
    }
    return r_interrupt;
  }

  tmpl(/**/)::VciIcu(
		     sc_core::sc_module_name name,
		     const soclib::common::IntTab &index,
		     const soclib::common::MappingTable &mt,
		     size_t nirq)
    : soclib::tlmt::BaseModule(name),
      m_index(index),
      m_mt(mt),
      m_nirq(nirq),
      p_vci("vci", new tlmt_core::tlmt_callback<VciIcu,soclib::tlmt::vci_cmd_packet<vci_param> *>(this, &VciIcu<vci_param>::callback)),
      p_irqOut("irqOut",NULL)
  {
    //maximum number of interruption equal 32
    if (m_nirq >= 32)
      m_nirq = 32;
      
    segList=m_mt.getSegmentList(m_index);
      
    r_mask = 0x00000000;
    r_current = 0x00000000;
      
    irq = new irq_struct[m_nirq];
      
    for(unsigned int i=0;i<m_nirq;i++){
      irq[i].val=false;
      irq[i].time=0;

      std::ostringstream tmpName;
      tmpName << "irqIn" << i;
      p_irqIn.push_back(new tlmt_core::tlmt_in<bool>(tmpName.str().c_str(), new tlmt_core::tlmt_callback<VciIcu,bool>(this,&VciIcu<vci_param>::irqReceived,(int*)i)));
    }
  }
}}



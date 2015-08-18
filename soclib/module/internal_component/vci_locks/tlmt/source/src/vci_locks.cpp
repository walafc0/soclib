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

#include "../include/vci_locks.h"

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciLocks<vci_param>

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
	  return callback_read(segIndex,s,pkt,time,private_data);
	  break;
	case vci_param::CMD_WRITE:
	  return callback_write(segIndex,s,pkt,time,private_data);
	  break;
	default:
	  std::cout << "Command does not implemmented" << std::endl;
	  break;
	}
      return;
      }
      //send error message
      std::cout << "Address does not match any segment" << std::endl;
    }
    
    tmpl(void)::callback_read(size_t segIndex,
			    soclib::common::Segment &s,
			    soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
			    const tlmt_core::tlmt_time &time,
			    void *private_data){
      
      typename vci_param::addr_t address;
      uint32_t index;
      
      for (size_t i=0;i<pkt->nwords;i++){
	if (pkt->contig)
	  address = pkt->address + (i* vci_param::nbytes);
	else
	  address = pkt->address;
	index = (address - s.baseAddress()) / vci_param::nbytes;
	pkt->buf[i]= m_contents[segIndex][index];
	m_contents[segIndex][index] = true;
      }

      tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords); 
     
      rsp.nwords = pkt->nwords;
      rsp.srcid  = pkt->srcid;
      rsp.pktid  = pkt->pktid;
      rsp.trdid  = pkt->trdid;
      
      p_vci.send(&rsp, time + delay);
    }
    
    tmpl(void)::callback_write(size_t segIndex,
			       soclib::common::Segment &s,
			       soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
			       const tlmt_core::tlmt_time &time,
			       void *private_data) {
    
      typename vci_param::addr_t address;
      uint32_t index;
      
      for (size_t i=0;i<pkt->nwords;i++){
	if (pkt->contig)
	  address = pkt->address + (i* vci_param::nbytes);
	else
	  address = pkt->address;
	index = (address - s.baseAddress()) / vci_param::nbytes;
	ram_t *tab = m_contents[segIndex];
	tab[index]=false;
      }
      
      tlmt_core::tlmt_time delay = tlmt_core::tlmt_time(pkt->nwords); 

      rsp.nwords = pkt->nwords;
      rsp.srcid  = pkt->srcid;
      rsp.pktid  = pkt->pktid;
      rsp.trdid  = pkt->trdid;
      
      p_vci.send(&rsp, time + delay);
    }
    
    tmpl(/**/)::VciLocks(sc_core::sc_module_name name,
			 const soclib::common::IntTab &index,
			 const soclib::common::MappingTable &mt)
	       : soclib::tlmt::BaseModule(name),
	       m_index(index),
	       m_mt(mt),
	       p_vci("vci", new tlmt_core::tlmt_callback<VciLocks,soclib::tlmt::vci_cmd_packet<vci_param> *>(this, &VciLocks<vci_param>::callback)){
      
      segList=m_mt.getSegmentList(m_index);
      size_t nbSeg=segList.size();
      std::list<soclib::common::Segment>::iterator seg;
      
      m_contents = new ram_t*[nbSeg];
      size_t word_size = 4;
      size_t i=0;
      
      for (i=0, seg = segList.begin(); seg != segList.end(); ++i, ++seg ) {
	soclib::common::Segment &s = *seg;
	m_contents[i] = new ram_t[(s.size()+word_size-1)/word_size];
      }
      
      for (i=0, seg = segList.begin(); seg != segList.end(); ++i, ++seg ) {
	soclib::common::Segment &s = *seg;
	for ( size_t addr = 0; addr < s.size()/4; ++addr )
	  m_contents[i][addr] = false;
      }
    }
    
}}

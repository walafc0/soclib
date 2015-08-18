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
 * Maintainers: fpecheux, nipo, alinev
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     Francois Pecheux <francois.pecheux@lip6.fr>
 *     Nicolas Pouillon <nipo@ssji.net>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
*/

#include <limits>
#include "../include/vci_rsp_arb_cmd_rout.h"
#include "../include/vci_cmd_arb_rsp_rout.h"

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciCmdArbRspRout<vci_param>

  tmpl(void)::callback(soclib::tlmt::vci_rsp_packet<vci_param> *pkt,
		       const tlmt_core::tlmt_time &time,
		       void *private_data)
  {
    c0.update_time(time+tlmt_core::tlmt_time(1));
    m_RspArbCmdRout[pkt->srcid]->stop_sending();
    m_RspArbCmdRout[pkt->srcid]->p_vci.send(pkt,time+m_delay);
  }

  tmpl(tlmt_core::tlmt_time)::getTime(){
    return c0.time();
  }
  
  tmpl(void)::behavior()
  {
    while (1) {
      int decision=through_fifo();

      switch (decision) {
      case -1:	// no packet available
	sc_core::wait(e0);
	break;
      case -2:	// the only packet available is false 
	sc_core::wait(sc_core::SC_ZERO_TIME);
	break;
      default:	// decision contains the index of the destination target
	{
	  //std::cout << "[CMD_ARB_RSP_ROUT " << fifos[decision].pkt->trdid << "] send packet from source " << decision << std::endl;
	  p_vci.send(fifos[decision].pkt,fifos[decision].time);
	  fifos[decision].event=false;
	  //sc_core::wait(e0); // should be optimized here
	}
	break;
      }
    }
  }

  tmpl(void)::setRspArbCmdRout(std::vector<typename soclib::tlmt::VciRspArbCmdRout<vci_param> *> &RspArbCmdRout)
  {
    m_RspArbCmdRout=RspArbCmdRout;
  }

  tmpl(void)::put(soclib::tlmt::vci_cmd_packet<vci_param> *pkt,uint32_t idx,const tlmt_core::tlmt_time &time)
  {
    //std::cout <<"put" << std::endl;
    fifos[idx].event=true;
    fifos[idx].pkt=pkt;
    fifos[idx].time=time;
    e0.notify(sc_core::SC_ZERO_TIME);
  }
  
  tmpl(int)::through_fifo()
  {
    tlmt_core::tlmt_time min_time=std::numeric_limits<uint32_t>::max();
    int min_index = -1;
    int available_packets = 0;
    
    for (unsigned int i = m_selected_port + 1; i != m_selected_port+m_nbinit+1; ++i ) {
      int k = i%m_nbinit;
      
      // Do not take inactive initiators into account
      // when looking for the fifo with smallest timestamp
      if (!m_RspArbCmdRout[k]->p_vci.peer_active())
	continue;
      
      // If the initiator send a packet to this target, get its time directly,
      // If not, get the original time of the initiator and add the interconnect
      // delay to compare comparable times
      if (fifos[k].event) {
	available_packets++;
	//If fifo time is lesser than vgmn time, then the time is updating
	if(c0.time() > fifos[k].time)
	  fifos[k].time = c0.time();
      }
      else {
	fifos[k].time = m_RspArbCmdRout[k]->p_vci.peer_time()+m_delay;
	//if the initiator is sending a packet to other target, 
	//verify if the cmd target has a time greater than time initiator plus the delay interconnect
	//in affirmative case, it updates the fifo time with the cmd target time
	//because it is not possible that this target receive a packet with a inferior time
	if(m_RspArbCmdRout[k]->is_sending()){
	  if(m_RspArbCmdRout[k]->getCmdTime() > fifos[k].time)
	    fifos[k].time = m_RspArbCmdRout[k]->getCmdTime();
	}
      }
      
      // If the fifo k contains a packet with the smallest timestamp, modify the min variables. 
      // min_index indicates the initiator with the smallest timestamp.
      if(fifos[k].time < min_time) {
	 min_time = fifos[k].time;
	 min_index = k;
      }
      else if(fifos[k].time == min_time && !fifos[min_index].event) {
	//if the min_index priority initiator has not packet for that target,
	//verify if it is sending a packet to other target. In affirmative case, 
	//min_index receive the index of current initiator. This action prevents deadlock.
	if (m_RspArbCmdRout[min_index]->is_sending()) {
	  min_index = k;
	}
      }
    }

    if (available_packets==0) {
      return -1;
    } else {
      if (fifos[min_index].event) {
	m_selected_port = min_index;
	return min_index;
      }
      else {
	return -2;
      }
    }
    return -1;
  }
  
  tmpl(/***/)::VciCmdArbRspRout( sc_core::sc_module_name name,
				 uint32_t idx,
				 uint32_t nb_init,
				 tlmt_core::tlmt_time dl )
    : soclib::tlmt::BaseModule(name),
      m_index(idx),
      m_nbinit(nb_init),
      m_delay(dl),
      p_vci("vci", new tlmt_core::tlmt_callback<VciCmdArbRspRout,soclib::tlmt::vci_rsp_packet<vci_param> *>(this, &VciCmdArbRspRout<vci_param>::callback), &c0)
  {

    m_selected_port=0;

    fifos = new fifo_struct<vci_param>[m_nbinit];
    
    for (size_t i=0;i<nb_init;i++) {
      fifos[i].event = false;
      fifos[i].pkt = NULL;
      fifos[i].time = 0;
    }

    SC_THREAD(behavior);
  }
  
}}

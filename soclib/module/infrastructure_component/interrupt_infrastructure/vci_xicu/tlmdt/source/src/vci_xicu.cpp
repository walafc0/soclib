/* -*- c++ -*-
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
 * Copyright (c) UPMC, Lip6, 2010
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include <strings.h>

#include "xicu.h"
#include "register.h"
#include "arithmetics.h"
#include "alloc_elems.h"
#include "../include/vci_xicu.h"

namespace soclib {
namespace tlmdt {

using namespace soclib;

#define tmpl(t) template<typename vci_param> t VciXicu<vci_param>

#ifdef SOCLIB_MODULE_DEBUG
#define CHECK_BOUNDS(x)                                                 \
    if ( idx >= (m_##x##_count) ) {                                     \
        std::cout << name() << " error: " #x " index " << idx           \
                  << " out of bounds ("                                 \
                  << m_##x##_count << ")"                               \
                  << std::endl;                                         \
        payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);   \
        break;                                                          \
    }                                                                   
#else
#define CHECK_BOUNDS(x)                                                 \
    if ( idx >= (m_##x##_count) ) {                                     \
        payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE); break; \
    }
#endif



tmpl(void)::behavior()
{
    while(true){
        if ( ! m_pending_irqs.empty() ) {
            std::map<sc_core::sc_time, std::pair<int, bool> >::iterator i = m_pending_irqs.begin();
            send_interruption(i->second.first, i->second.second, i->first);
            m_pending_irqs.erase(i);
        }
        wait(sc_core::SC_ZERO_TIME);
    }
}

tmpl(void)::send_interruption(int idx, bool val, sc_core::sc_time time)
{
#if SOCLIB_MODULE_DEBUG
    //std::cout << "p_irq[" << idx << "] = " << val << std::endl;
#endif
      
  // set the values in irq tlm payload
  data_t nwords= 1;
  data_t nbytes= nwords * vci_param::nbytes;
  data_t byte_enable = vci_param::be2mask(0xF);
  unsigned char data_ptr[nbytes];
  unsigned char byte_enable_ptr[nbytes];
  
  utoa(byte_enable, byte_enable_ptr, 0);
  utoa(val, data_ptr, 0);
  
  // set the values in irq tlm payload
  m_irq_payload.set_byte_enable_ptr(byte_enable_ptr);
  m_irq_payload.set_byte_enable_length(nbytes);
  m_irq_payload.set_data_ptr(data_ptr);
  m_irq_payload.set_data_length(nbytes);
  
  // set the tlm phase
  m_irq_phase = tlm::BEGIN_REQ;
  // set the local time to transaction time
  m_irq_time = time;
  // send the transaction
  (*p_irq[idx])->nb_transport_fw(m_irq_payload, m_irq_phase, m_irq_time);
}

tmpl(void)::verify_period_interruption(int idx, uint32_t old_msk, uint32_t msk)
{
    if(old_msk!=msk && m_pti_per[idx]!=0){
        if(m_msk_pti[idx]){
            //send interruption
            m_pending_irqs[(m_pti_val[idx] * UNIT_TIME)] = std::pair<int, bool>(idx, true);
        }
        else{
            //disable interruption
            m_pending_irqs[(m_pti_val[idx] * UNIT_TIME)] = std::pair<int, bool>(idx, false);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if VCI TARGET SOCKET
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw
( tlm::tlm_generic_payload &payload,
  tlm::tlm_phase           &phase,  
  sc_core::sc_time         &time)   
{
  
    soclib_payload_extension *extension_pointer;
    payload.get_extension(extension_pointer);

    //this target does not treat the null message
    if(extension_pointer->is_null_message()){
        return tlm::TLM_COMPLETED;
    }
    
    // First, find the right segment using the first address of the packet
    std::list<soclib::common::Segment>::iterator seg;	
    size_t segIndex;
    
    data_t nwords = (data_t)(payload.get_data_length() / vci_param::nbytes);
    uint32_t old_msk;

    for (segIndex=0,seg = m_segments.begin(); seg != m_segments.end(); ++segIndex, ++seg ) {
        soclib::common::Segment &s = *seg;
        if (!s.contains(payload.get_address()))
            continue;
    
        for (data_t i=0;i<nwords;i++){

        switch(extension_pointer->get_command()){
        case VCI_READ_COMMAND:
        {
            size_t cell = (size_t)((payload.get_address()+(i*vci_param::nbytes)) - s.baseAddress())/ vci_param::nbytes;
            size_t idx = cell & 0x1f;
            size_t func = (cell >> 5) & 0x1f;
            data_t data;

            switch (func) {
            case XICU_WTI_REG:
                CHECK_BOUNDS(wti);
                data = m_wti_reg[idx];
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
                m_wti_pending &= ~(1<<idx);
                for(size_t n=0, count=0; n< m_irq_count; n++){
                    if(m_msk_wti[n]){
                        if(count==idx){
                            m_pending_irqs[(time+(n*UNIT_TIME))] = std::pair<int, bool>(n, false);
                        }
                        count++;
                    }
                }
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_WTI_REG[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_PTI_PER:
                CHECK_BOUNDS(pti);
                data = m_pti_per[idx];
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_PTI_PER[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_PTI_VAL:
                CHECK_BOUNDS(pti);
                data = m_pti_val[idx];
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_PTI_VAL[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_PTI_ACK:
                CHECK_BOUNDS(pti);
                m_pti_pending &= ~(1<<idx);
                if(m_msk_pti[idx]){
                    m_pending_irqs[(m_pti_val[idx] + 1) * UNIT_TIME] = std::pair<int, bool>(idx, false);
                    m_pti_val[idx] = m_pti_val[idx] + m_pti_per[idx];
                    m_pending_irqs[m_pti_val[idx] * UNIT_TIME] = std::pair<int, bool>(idx, true);
                }
                data = 0;
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_PTI_ACK[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_MSK_PTI:
                CHECK_BOUNDS(irq);
                data = m_msk_pti[idx];
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_MSK_PTI[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_PTI_ACTIVE:
                CHECK_BOUNDS(irq);
                data = m_msk_pti[idx] & m_pti_pending;
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_PTI_ACTIVE[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_MSK_HWI:
                CHECK_BOUNDS(irq);
                data = m_msk_hwi[idx];
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_MSK_HWI[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_HWI_ACTIVE:
                CHECK_BOUNDS(irq);
                data = m_msk_hwi[idx] & m_hwi_pending;
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_HWI_ACTIVE[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_MSK_WTI:
                CHECK_BOUNDS(irq);
                data = m_msk_wti[idx];
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_MSK_WTI[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_WTI_ACTIVE:
                CHECK_BOUNDS(irq);
                data = m_msk_wti[idx] & m_wti_pending;
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_WTI_ACTIVE[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_PRIO:
                CHECK_BOUNDS(irq);
                data = 
                    (((m_msk_pti[idx] & m_pti_pending) ? 1 : 0) << 0) |
                    (((m_msk_hwi[idx] & m_hwi_pending) ? 1 : 0) << 1) |
                    (((m_msk_wti[idx] & m_wti_pending) ? 1 : 0) << 2) |
                    ((soclib::common::ctz<uint32_t>(m_msk_pti[idx] & m_pti_pending) & 0x1f) <<  8) |
                    ((soclib::common::ctz<uint32_t>(m_msk_hwi[idx] & m_hwi_pending) & 0x1f) << 16) |
                    ((soclib::common::ctz<uint32_t>(m_msk_wti[idx] & m_wti_pending) & 0x1f) << 24);
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_PRIO[" << std::dec << idx << "] = " << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_CONFIG:
                data = (m_irq_count << 24) | (m_wti_count << 16) | (m_hwi_count << 8) | m_pti_count;
                utoa(data, payload.get_data_ptr(),(i * vci_param::nbytes));
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Read XICU_CONFIG = " << std::hex << (int)data << std::dec << " time = " << m_clock_cycles << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

             default:
                //send error message
                payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
                break;
             }//end case FUNC
        }//end case READ
        break;
        case VCI_WRITE_COMMAND:
        {
            size_t cell = (size_t)((payload.get_address()+(i*vci_param::nbytes)) - s.baseAddress())/ vci_param::nbytes;
            size_t idx = cell & 0x1f;
            size_t func = (cell >> 5) & 0x1f;
            data_t data = atou(payload.get_data_ptr(), (i * vci_param::nbytes));
            data_t be   = atou(payload.get_byte_enable_ptr(), (i * vci_param::nbytes));
  
            if ( be != vci_param::be2mask(0xf) ){
                payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
                break;
            }

            switch (func) {
            case XICU_WTI_REG:
                CHECK_BOUNDS(wti);
                m_wti_reg[idx] = data;
                m_wti_pending |= 1<<idx;
                for(size_t n=0, count=0; n< m_irq_count; n++){
                    if(m_msk_wti[n]){
                        if(count==idx){
                            m_pending_irqs[(time+(n*UNIT_TIME))] = std::pair<int, bool>(n, true);
                        }
                        count++;
                    }
                }
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write WTI_REG[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_PTI_PER:
                CHECK_BOUNDS(pti);
                m_pti_per[idx] = (int)data;
                if ( !data ) {
                    m_pti_pending &= ~(1<<idx);
                    m_pti_val[idx] = 0;
                    if(m_msk_pti[idx]){
                        m_pending_irqs[time] = std::pair<int, bool>(idx, false);
                    }
                } else {
                    m_pti_pending |= 1<<idx;
                    m_pti_val[idx] = time.value() + (int)data;
                    if(m_msk_pti[idx]){
                       m_pending_irqs[(m_pti_val[idx] * UNIT_TIME)] = std::pair<int, bool>(idx, true);
                    }
                }
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write PTI_PER[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;
                
            case XICU_PTI_VAL:
                CHECK_BOUNDS(pti);
                m_pti_val[idx] = data;
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write PTI_VAL[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;
        
            case XICU_MSK_PTI:
                CHECK_BOUNDS(irq);
                old_msk = m_msk_pti[idx];
                m_msk_pti[idx] = data;
                verify_period_interruption(idx, old_msk, m_msk_pti[idx]);

#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write MASK_PTI[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_MSK_PTI_ENABLE:
                CHECK_BOUNDS(irq);
                old_msk = m_msk_pti[idx];
                m_msk_pti[idx] |= data;
                verify_period_interruption(idx, old_msk, m_msk_pti[idx]);

#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write PTI_ENABLE[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;
                
            case XICU_MSK_PTI_DISABLE:
                CHECK_BOUNDS(irq);
                old_msk = m_msk_pti[idx];
                m_msk_pti[idx] &= ~data;
                verify_period_interruption(idx, old_msk, m_msk_pti[idx]);

#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write PTI_DISABLE[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;
                
            case XICU_MSK_HWI:
                CHECK_BOUNDS(irq);
                m_msk_hwi[idx] = data;
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write MSK_HWI[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;
        
            case XICU_MSK_HWI_ENABLE:
                CHECK_BOUNDS(irq);
                m_msk_hwi[idx] |= data;
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write HWI_ENABLE[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;

            case XICU_MSK_HWI_DISABLE:
                CHECK_BOUNDS(irq);
                m_msk_hwi[idx] &= ~data;
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write HWI_DISABLE[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;
                
            case XICU_MSK_WTI:
                CHECK_BOUNDS(irq);
                m_msk_wti[idx] = data;
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write MSK_WTI[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;
                
            case XICU_MSK_WTI_ENABLE:
                CHECK_BOUNDS(irq);
                m_msk_wti[idx] |= data;
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write WTI_ENABLE[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;
                
            case XICU_MSK_WTI_DISABLE:
                CHECK_BOUNDS(irq);
                m_msk_wti[idx] &= ~data;
#if SOCLIB_MODULE_DEBUG
                std::cout << "[" << name() << "] Write WTI_DISABLE[" << std::dec << idx << "] = "  << std::hex << (int)data << std::dec << " time = " << time.value() << std::endl;
#endif
                payload.set_response_status(tlm::TLM_OK_RESPONSE);
                break;
                
            default:
                //send error message
                payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
                break;
        
            }//end switch FUNC
        }// end case WRITE
        break;
        default:
            //send error message
            payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
#if SOCLIB_MODULE_DEBUG
            std::cout << "[" << name() << "] Address " << std::hex << payload.get_address() << std::dec << " does not match any segment " << std::endl;
            std::cout << "[" << name() << "] Send a error packet with time = "  << time.value() << std::endl;
#endif
             break;
        }//end switch COMMAND
        }//end for nwords
    }//end for segments

	phase = tlm::BEGIN_RESP;
	time = time + (nwords * UNIT_TIME);
 	
	p_vci->nb_transport_bw(payload, phase, time);
	return tlm::TLM_COMPLETED;

}

// Not implemented for this example but required by interface
tmpl(void)::b_transport
( tlm::tlm_generic_payload &payload,                // payload
  sc_core::sc_time         &_time)                  //time
{
  return;
}

// Not implemented for this example but required by interface
tmpl(bool)::get_direct_mem_ptr
( tlm::tlm_generic_payload &payload,                // address + extensions
  tlm::tlm_dmi             &dmi_data)               // DMI data
{ 
  return false;
}

// Not implemented for this example but required by interface
tmpl(unsigned int):: transport_dbg                            
( tlm::tlm_generic_payload &payload)                // debug payload
{
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if (IRQ TARGET SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::hwi_nb_transport_fw
( int                      id,         // interruption id
  tlm::tlm_generic_payload &payload,   // payload
  tlm::tlm_phase           &phase,     // phase
  sc_core::sc_time         &time)      // time
{
  bool val = (bool) atou(payload.get_data_ptr(), 0);
  m_pending_irqs[time] = std::pair<int, bool>(id, val);

#if SOCLIB_MODULE_DEBUG
  std::cout << "[" << name() << "] Receive Interruption " << id << " val = " << val << " with time = " << time.value() << std::endl;
#endif

  return tlm::TLM_COMPLETED;
}


tmpl(/**/)::VciXicu(
    sc_core::sc_module_name name,
    const soclib::common::MappingTable &mt,
    const soclib::common::IntTab &index,
    size_t pti_count,
    size_t hwi_count,
    size_t wti_count,
    size_t irq_count)
           : sc_module(name),                  // module name
           m_pti_count(pti_count),
           m_hwi_count(hwi_count),
           m_wti_count(wti_count),
           m_irq_count(irq_count),
           m_msk_pti(new uint32_t[irq_count]),
           m_msk_wti(new uint32_t[irq_count]),
           m_msk_hwi(new uint32_t[irq_count]),
           m_pti_pending(0),
           m_wti_pending(0),
           m_hwi_pending(0),
           m_pti_per(new uint32_t[pti_count]),
           m_pti_val(new uint32_t[pti_count]),
           m_wti_reg(new uint32_t[wti_count]),
           m_pending_irqs(),
           p_vci("vci_target")  // vci target socket name
{
    p_vci(*this);

    //PDES local time
    m_pdes_local_time = new pdes_local_time();

    for(size_t i=0; i<hwi_count; i++){
        std::ostringstream hwi_name;
        hwi_name << "hwi" << i;
        p_hwi.push_back(new tlm_utils::simple_target_socket_tagged<VciXicu,32,tlm::tlm_base_protocol_types>(hwi_name.str().c_str()));
    
        p_hwi[i]->register_nb_transport_fw(this, &VciXicu::hwi_nb_transport_fw, i);
        
    }

    for(size_t i=0; i<irq_count; i++){
        std::ostringstream irq_name;
        irq_name << "irq" << i;
        p_irq.push_back(new tlm_utils::simple_initiator_socket_tagged<VciXicu,32,tlm::tlm_base_protocol_types>(irq_name.str().c_str()));
    }

    m_segments = mt.getSegmentList(index);

    for ( size_t i = 0; i<m_pti_count; ++i ) {
        m_pti_per[i] = 0;
        m_pti_val[i] = 0;
    }
    for ( size_t i = 0; i<m_wti_count; ++i )
        m_wti_reg[i] = 0;
    for ( size_t i = 0; i<m_irq_count; ++i ) {
        m_msk_pti[i] = 0;
        m_msk_wti[i] = 0;
        m_msk_hwi[i] = 0;
    }
    m_pti_pending = 0;
    m_wti_pending = 0;
    m_hwi_pending = 0;

    SC_THREAD(behavior);
}

tmpl(/**/)::~VciXicu()
{
    delete m_msk_pti;
    delete m_msk_wti;
    delete m_msk_hwi;
    delete m_pti_per;
    delete m_pti_val;
    delete m_wti_reg;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


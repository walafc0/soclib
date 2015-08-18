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
 * Maintainers: fpecheux, alinevieiramello@hotmail.com, alain
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 *     Alain Greiner <alain.greiner@lip6.fr>  2014
 */

#include "vci_ram.h"

#define SOCLIB_MODULE_DEBUG  1

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param> x VciRam<vci_param>

//////////////////////////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciRam
//////////////////////////////////////////////////////////////////////////////////////////
(
    sc_core::sc_module_name             name,
    const soclib::common::IntTab        &index,
    const soclib::common::MappingTable  &mt,
    const soclib::common::Loader        &loader,
    const uint32_t                      latency )
  : sc_module(name),
    m_mt(mt),
    m_loader(new soclib::common::Loader(loader)),
    m_atomic(4096), 	  //  maximal number of initiator
    m_latency(latency),   // intrinsic latency
    p_vci("p_vci")
{
    // bind target
    p_vci(*this);                     

    //PDES local time
    m_pdes_local_time = new pdes_local_time(sc_core::SC_ZERO_TIME);

    // segments
    m_segments = m_mt.getSegmentList(index);
    m_contents = new ram_t*[m_segments.size()];
    size_t word_size = sizeof(typename vci_param::data_t);
    std::list<soclib::common::Segment>::iterator seg;
    size_t i;
    for ( i=0, seg = m_segments.begin(); seg != m_segments.end(); ++i, ++seg ) 
    {
        soclib::common::Segment &s = *seg;
        m_contents[i] = new ram_t[(s.size()+word_size-1)/word_size];
        memset(m_contents[i], 0, s.size());
    }
  
    if ( m_loader )
    {
        for ( i=0, seg = m_segments.begin(); seg != m_segments.end(); ++i, ++seg ) 
        {
            soclib::common::Segment &s = *seg;
            m_loader->load(&m_contents[i][0], s.baseAddress(), s.size());
            for (size_t addr = 0; addr < s.size()/word_size; ++addr )
            {
	            m_contents[i][addr] = le_to_machine(m_contents[i][addr]);
            }
        }
    }
  
    // initialize the LL/SC registration table
    m_atomic.clearAll();

    // counters
    m_cpt_read = 0;
    m_cpt_write = 0;
}

tmpl(/**/)::~VciRam(){}

//////////////////////////
tmpl(void)::print_stats()
{
    std::cout << name() << std::endl;
    std::cout << "- READ               = " << m_cpt_read << std::endl;
    std::cout << "- WRITE              = " << m_cpt_write << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////
//  Fuction executed when receiving a VCI command
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw ( tlm::tlm_generic_payload &payload,
                                            tlm::tlm_phase           &phase,  
                                            sc_core::sc_time         &time )   
{
    soclib_payload_extension *extension_pointer;
    payload.get_extension(extension_pointer);

    // this target does not use the NULL messages
    if(extension_pointer->is_null_message()) 
    {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "]" << " receive NULL message / time = " << std::dec
          << time.value() << std::endl;
#endif

        return tlm::TLM_COMPLETED;
    }

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "]" << " receive VCI COMMAND / time = " << std::dec
          << time.value();
#endif

    // update the VCI message time
    // required to model the contention on the RAM
    if ( time < m_pdes_local_time->get() ) time = m_pdes_local_time->get();

    // select segment from address
    std::list<soclib::common::Segment>::iterator seg;	
    size_t segIndex;
    uint32_t nwords = (uint32_t)(payload.get_data_length() / vci_param::nbytes);
    uint32_t srcid  = extension_pointer->get_src_id();
	typename vci_param::addr_t address = payload.get_address();

    for ( segIndex=0, seg = m_segments.begin(); seg != m_segments.end(); ++segIndex, ++seg ) 
    {
        soclib::common::Segment &s = *seg;
        if ( not s.contains(payload.get_address()) ) continue;
    
	    size_t index = ( address - s.baseAddress() ) / vci_param::nbytes; 

        if ( extension_pointer->get_command() == VCI_READ_COMMAND )
        {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << " / READ / address = " << std::hex << address
          << " / srcid = " << srcid << std::endl;
#endif
	        for ( size_t i=0 ; i<nwords ; i++ )
            {
	            utoa( m_contents[segIndex][index + i], 
                      payload.get_data_ptr(),
                      (i * vci_param::nbytes) );
                m_cpt_read++;
	        }
	        payload.set_response_status(tlm::TLM_OK_RESPONSE);
        }
        else if ( extension_pointer->get_command() == VCI_WRITE_COMMAND )
        {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << " / WRITE / address = " << std::hex << address
          << " / srcid = " << srcid << std::endl;
#endif
	        for ( size_t i=0 ; i<nwords ; i++ )
            {
	            m_atomic.accessDone( address );
	  
	            uint32_t cur   = m_contents[segIndex][index + i];
	            uint32_t mask  = atou( payload.get_byte_enable_ptr(), 
                                       (i * vci_param::nbytes) );
	            uint32_t rdata = atou( payload.get_data_ptr(), 
                                       (i * vci_param::nbytes) );
	            m_contents[segIndex][index+i] = (cur & ~mask) | (rdata & mask);
                m_cpt_write++;
	        }
	        payload.set_response_status(tlm::TLM_OK_RESPONSE);
        }
        else if ( extension_pointer->get_command() == VCI_LINKED_READ_COMMAND )
        {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << " LL / address = " << std::hex << address
          << " / srcid = " << srcid << std::endl;
#endif
            assert( (nwords == 1) and
            "ERROR in RAM : LL transaction must be one flit");

            utoa( m_contents[segIndex][index], 
                  payload.get_data_ptr(), 0);
            m_atomic.doLoadLinked(address, srcid);
            m_cpt_read++;
            payload.set_response_status(tlm::TLM_OK_RESPONSE);
        }
        else if ( extension_pointer->get_command() == VCI_STORE_COND_COMMAND )
        {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << " SC / address = " << std::hex << address
          << " / srcid = " << srcid << std::endl;
#endif
            assert( (nwords == 1) and
            "ERROR in RAM : SC transaction must be one flit");

            if(m_atomic.isAtomic( address, srcid ))
            {
                m_atomic.accessDone(address);
                m_contents[segIndex][index] = atou( payload.get_data_ptr(), 0);
	            utoa(0, payload.get_data_ptr(), 0);
            }
            else
            {
                utoa(1, payload.get_data_ptr(), 0);
            }
	        m_cpt_write++;
            payload.set_response_status(tlm::TLM_OK_RESPONSE);
        }
        else          //send error message
        {
            payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
      
#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "] send ERROR RESPONSE : illegal VCI Command" << std::endl;
#endif

        } 

        time = time + m_latency*UNIT_TIME;
        m_pdes_local_time->set( time );
  
        phase = tlm::BEGIN_RESP;

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "]" << " send VCI RESPONSE / time = " << std::dec
          << time.value() << std::endl;
#endif
        p_vci->nb_transport_bw( payload, 
                                phase, 
                                time );

        return tlm::TLM_COMPLETED;


    } // end loop on segments
  
    //send error message if not found
    payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
    time = time + m_latency*UNIT_TIME;
    m_pdes_local_time->set( time );
    phase = tlm::BEGIN_RESP;
    p_vci->nb_transport_bw( payload, 
                            phase, 
                            time);
      
#ifdef SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] send ERROR RESPONSE : segment not found" << std::endl;
#endif

    return tlm::TLM_COMPLETED;
}  // end nb_transport_fw()

////////////////////////////////////////////////////////////////////
// Not used but required by interface
////////////////////////////////////////////////////////////////////
tmpl(void)::b_transport( tlm::tlm_generic_payload &payload, 
                         sc_core::sc_time         &_time )  
{
  return;
}

/////////////////////////////////////////////////////////////////////
tmpl(bool)::get_direct_mem_ptr( tlm::tlm_generic_payload &payload,  
                                tlm::tlm_dmi             &dmi_data )
{ 
    return false;
}
    
/////////////////////////////////////////////////////////////////////
tmpl(unsigned int):: transport_dbg( tlm::tlm_generic_payload &payload ) 
{
    return false;
}

}}

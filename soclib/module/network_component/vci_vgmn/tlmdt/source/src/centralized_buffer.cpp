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
 * Maintainers: fpecheux, alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include <systemc>
#include "centralized_buffer.h"

namespace soclib { namespace tlmdt {

////////////////////////////////////////////////////////////////////////////
//   constructor / destructor for central buffer
////////////////////////////////////////////////////////////////////////////
centralized_buffer::centralized_buffer ( sc_core::sc_module_name   name,
                                         size_t                    nslots )
  : sc_module(name)
  , m_slots(nslots)
  , m_port_array(new init_port_descriptor[nslots])
  , m_previous( nslots - 1 )
{
    for(unsigned int i=0; i<nslots; i++)
    {
        std::ostringstream buf_name;
        buf_name << "slot_" << i;
        m_port_array[i].buffer.set_name(buf_name.str());
    }
}

centralized_buffer::~centralized_buffer()
{
  delete [] m_port_array;
}

///////////////////////////////////////////////////////////////
// This function push a transaction (payload, phase, time)
// in the circular buffer associated to initiator (from)
///////////////////////////////////////////////////////////////
bool centralized_buffer::push ( size_t                    from,
                                tlm::tlm_generic_payload  &payload,
                                tlm::tlm_phase            &phase,
                                sc_core::sc_time          &time)
{

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] PUSH [" << from <<"] " << std::endl;
#endif

    assert(!(time < m_port_array[from].port_time) and
    "PUSH transaction in a slot with a time smaller than precedent");

    return m_port_array[from].buffer.push(payload, phase, time);
}

///////////////////////////////////////////////////////////////////////////////
// This function implements the PDES time filtering algorithm:
// All active initiators are scanned, to select the earliest date.
// - if there is no transaction for this initiator, (false) is returned,
//   and no transaction is consumed in the central buffer.
// - if there is a transaction, (true) is returned. The selected 
//   initiator index is returned in (from). The transaction parameters 
//   are returned in (payload, phase, time), the transaction is 
//   removed from the central buffer, and the selected port time is updated.
///////////////////////////////////////////////////////////////////////////////
bool centralized_buffer::pop ( size_t                    &from,
                               tlm::tlm_generic_payload* &payload,
                               tlm::tlm_phase*           &phase,
                               sc_core::sc_time*         &time )
{
    uint64_t min_time = MAX_TIME;
    size_t sel_id = 0;              // selected port
    uint64_t time_value;            // date of the port
  
    // searching the earliest (smaller time) active port
    // we implement a round-robin priority because,
    // in case of equal times, the first found is selected
    for( size_t k=0 ; k<m_slots ; k++ )
    {
        size_t i = (m_previous + k + 1) % m_slots;

        if(m_port_array[i].active)   // only active ports are competing
        {
            // get time
            if(m_port_array[i].buffer.is_empty())   // no transaction available
            {
	            time_value = m_port_array[i].port_time.value();

std::cout << "@@@ port " << i << " / empty / time = " << std::dec << time_value << std::endl;

            }
            else                                    // front transaction is earliest
            {
	            m_port_array[i].buffer.get_front(payload, phase, time);
	            time_value = (*time).value();

std::cout << "@@@ port " << i << " /  ok   / time = " << std::dec << time_value << std::endl;

            }

            // test if it is the earliest
            if(time_value < min_time)
            {
	            min_time = time_value;
                sel_id   = i;
	        }
        }
    }

    if( not m_port_array[sel_id].buffer.is_empty() )    // success
    {
        m_port_array[sel_id].buffer.pop( payload,
                                         phase,
                                         time);
        from = sel_id;      
        m_previous = sel_id;
        set_port_time( sel_id, *time );
        return true;
    }
    else                                                 // no eligible command
    {
        return false;
    }
} // end pop()
   
////////////////////////////////////////////////////////////
circular_buffer centralized_buffer::get_buffer(size_t index)
{
    return m_port_array[index].buffer;
}

/////////////////////////////////////////////
const size_t centralized_buffer::get_nslots()
{
    return m_slots;
}

////////////////////////////////////////////////////////////////
sc_core::sc_time centralized_buffer::get_port_time(size_t index)
{
    return m_port_array[index].port_time;
}

////////////////////////////////////////////////////////////////////////
void centralized_buffer::set_port_time(size_t index, sc_core::sc_time t)
{
    m_port_array[index].port_time = t;

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] DELTA_TIME[" << index <<"] = " << t.value() << std::endl;
#endif

}

///////////////////////////////////////////////////////////
void centralized_buffer::set_activity(size_t index, bool b)
{
    m_port_array[index].active = b;

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] ACTIVE[" << index <<"] = " << b << std::endl;
#endif

}

}}

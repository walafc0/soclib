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
 * Maintainers: alinevieiramello@hotmail.com, alain
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 *     Alain Greiner <alain.greiner@lip6.fr>
 */

#include "vci_vgmn.h"       

#define SOCLIB_MODULE_DEBUG 1
                    
namespace soclib { namespace tlmdt {

#define tmpl(x) x VciVgmn

////////////////////////////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciVgmn ( sc_core::sc_module_name             name,           // module name
                      const soclib::common::MappingTable  &mt,            // mapping table
                      const size_t                        n_inits,        // number of initiators
                      const size_t                        n_targets,      // number of targets
                      const size_t                        min_latency,    // minimal latency
                      const size_t                        fifo_depth,     // not used in TLMDT
                      const size_t                        default_tgtid ) // default target index
  : sc_module( name )
  , m_inits( n_inits )
  , m_targets( n_targets )
  , m_latency( min_latency )
  , m_central_buffer( "central_buffer", n_inits )
  , m_cmd_routing_table( mt.getGlobalIndexFromAddress( default_tgtid ) )
  , m_rsp_routing_table( mt.getGlobalIndexFromSrcid() )
  , m_push_vci_count( 0 )
  , m_pop_vci_count( 0 )
  , m_push_null_count( 0 )
  , m_pop_null_count( 0 )
  , m_push_activity_count( 0 )
  , m_pop_activity_count( 0 )
  , m_null_sent_count( 0 )
{
    // allocate & bind p_to_initiator VCI ports
    for( size_t i=0 ; i<m_inits ; i++ )
    {
        std::ostringstream name;
        name << "p_to_initiator_" << i;
        p_to_initiator.push_back(new tlm_utils::simple_target_socket_tagged
           <VciVgmn,32,tlm::tlm_base_protocol_types>(name.str().c_str()));

        p_to_initiator[i]->register_nb_transport_fw( this, 
                                                     &VciVgmn::nb_transport_fw, 
                                                     i );
    }

    // allocate & bind p_to_target VCI ports
    for( size_t i=0 ; i<m_targets ; i++ )
    {
        std::ostringstream name;
        name << "p_to_target_" << i;
        p_to_target.push_back(new tlm_utils::simple_initiator_socket_tagged
           <VciVgmn,32,tlm::tlm_base_protocol_types>(name.str().c_str()));

        p_to_target[i]->register_nb_transport_bw( this, 
                                                  &VciVgmn::nb_transport_bw, 
                                                  i );
    }

    // PDES local time
    m_pdes_local_time = new pdes_local_time(100*UNIT_TIME);

    // initialises payload, phase and extension for null messages
    m_null_payload.set_extension(&m_null_extension);
    m_null_extension.set_null_message();
    m_null_phase = tlm::BEGIN_REQ;

    // register thread process
    SC_THREAD(execLoop);                  
} // end init()


//////////////////////////////////////////////////////////////////////////////////////
// This function analyses the type of transaction poped from the the central buffer,
// and execute the required action.
//////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::route( size_t                     from,       // initiator index
                   tlm::tlm_generic_payload   &payload, 
                   tlm::tlm_phase             &phase,  
                   sc_core::sc_time           &time)  
{
    // get payload extension
    soclib_payload_extension *extension_ptr;
    payload.get_extension(extension_ptr);

    // if message is activation/deactivation, the initiator port is actived/desactived, 
    // but no message is transmit, and no response is returned
    if( extension_ptr->is_active() || extension_ptr->is_inactive() )
    {

#ifdef SOCLIB_MODULE_DEBUG
m_pop_activity_count++;
printf("    [%s] handle ACTIVITY from port %d / time = %d \n", 
       name(), (int)from, (int)time.value() );
#endif
        // initiator port activation/deactivation 
        m_central_buffer.set_activity(from, extension_ptr->is_active());
    }

    // if transaction command is a NULL message, a response is sent to the initiator
    // but this NULL message is not transmit.
    else if( extension_ptr->is_null_message() )
    {

#ifdef SOCLIB_MODULE_DEBUG
m_pop_null_count++;
printf("    [%s] handle NULL from port %d / time = %d \n", 
       name(), (int)from, (int)time.value() );
#endif
        // send the response
        (*p_to_initiator[from])->nb_transport_bw( payload, 
                                                  phase, 
                                                  time);
    }

    // if transaction is a VCI command, it must be sent to the appropriate target,
    // and no response is sent to the initiator.
    else
    {
        size_t dest = m_cmd_routing_table[payload.get_address()];

        assert( ( dest < m_targets ) and
        "ERROR in VGMN: illegal target index" );
	
        time = time + (m_latency*UNIT_TIME);

#ifdef SOCLIB_MODULE_DEBUG
m_pop_vci_count++;
printf("    [%s] transfer VCI command from init %d to target %d / time = %d\n", 
       name(), (int)from, (int)dest, (int)time.value() );
#endif
        // transfer VCI command
        (*p_to_target[dest])->nb_transport_fw( payload, 
                                               phase, 
                                               time );
    }
}  // end route()
  
///////////////////////////////////////////////////////////////////////////////////////
//      PDES process 
// ALL command from initiators (VCI, NULL, or ACTIVITY) are registered in the central
// buffer and are handled in a strict increasing time (time filtering).
// The local time is updated each time a new transaction is poped from central buffer.
// Non blocking NULL messages are sent to all targets when time quantum elapsed.
// The thread deschedule if there is no more eligible command in central buffer,
// and wake up when a new command is received.
///////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::execLoop()  
{
    size_t                    	from;
    tlm::tlm_generic_payload* 	payload_ptr;
    tlm::tlm_phase*           	phase_ptr;
    sc_core::sc_time*         	time_ptr;

    while (true)
    {

#ifdef SOCLIB_MODULE_DEBUG
printf("######    [%s] wake up / time = %d\n",
       name(), (int)m_pdes_local_time->get().value() );
#endif

        // pop the earliest transaction from central buffer
        // while eligible command is found in central buffer
        while( m_central_buffer.pop( from, 
                                     payload_ptr, 
                                     phase_ptr, 
                                     time_ptr) )
        {
            assert( not (*time_ptr < m_pdes_local_time->get()) 
            && "ERROR in VGMN: Transaction time smaller than local time");
 
            // update local time
            m_pdes_local_time->set(*time_ptr);

            // process the transaction
            route( from, 
                   *payload_ptr, 
                   *phase_ptr, 
                   *time_ptr );

            // send NULL messages to all targets if time_quantum elapsed
            if ( m_pdes_local_time->need_sync() )
            {
                m_pdes_local_time->reset_sync();
                m_null_time = m_pdes_local_time->get();
                m_null_sent_count += m_targets;

                for ( size_t i=0 ; i<(m_targets-1) ; i++ )
                {
                    (*p_to_target[i])->nb_transport_fw(m_null_payload, 
                                                       m_null_phase, 
                                                       m_null_time);
                }
            }
        } // end while buffer not empty
    
        
#ifdef SOCLIB_MODULE_DEBUG
printf("######    [%s] no eligible transaction => deschedule / time = %d\n",
       name(), (int)m_pdes_local_time->get().value() );
#endif
        ////////////////////////////////
        sc_core::wait( m_cmd_received );
        ////////////////////////////////

    } // end infinite while
} // end execLoop()
    
/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a command from VCI initiator[id].
// It registers the command in the central buffer.
// The initiator thread is desceduled if the buffer is full.
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw( int                         id,          
                                           tlm::tlm_generic_payload    &payload,    
                                           tlm::tlm_phase              &phase,     
                                           sc_core::sc_time            &time)     
{

#ifdef SOCLIB_MODULE_DEBUG
printf( "    [%s] receive COMMAND from init %d / time = %d \n", 
       name(), id, (int)time.value() );
#endif

    bool push = false;
    do
    {
        // try to push a transaction in the central buffer
        push = m_central_buffer.push( id, 
                                      payload, 
                                      phase, 
                                      time );
        if( not push )
        {

#ifdef SOCLIB_MODULE_DEBUG
printf("######    [init %d] cannot push into VGMN buffer => deschedule \n", id);
#endif
            ///////////////////////////////////////
            sc_core::wait( sc_core::SC_ZERO_TIME );
            ///////////////////////////////////////

#ifdef SOCLIB_MODULE_DEBUG
printf("######    [init %d] wake up \n", id);
#endif

        }
        else
        {

#ifdef SOCLIB_MODULE_DEBUG
soclib_payload_extension *extension_ptr;
payload.get_extension(extension_ptr);
if( extension_ptr->is_active() || extension_ptr->is_inactive() )
{
    m_push_activity_count++;
    printf( "    [%s] push ACTIVITY command into buffer\n", name() );
}
else if( extension_ptr->is_null_message() )
{
    m_push_null_count++;
    printf( "    [%s] push NULL command into buffer\n", name() );
}
else
{
    m_push_vci_count++;
    printf( "    [%s] push VCI command into buffer\n", name() );
}
#endif
            // notify to wake up the thread
            m_cmd_received.notify( sc_core::SC_ZERO_TIME );
        }
    } while ( not push );

    return  tlm::TLM_COMPLETED;

} //end nb_transport_fw

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response from VCI target[id].
// It directly routes the response to the proper VCI initiator (no time filtering).
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_bw( int                        id,   
                                           tlm::tlm_generic_payload   &payload,  
                                           tlm::tlm_phase             &phase,   
                                           sc_core::sc_time           &time)  
{
    // get message SRCID
    soclib_payload_extension *resp_extension_ptr;
    payload.get_extension(resp_extension_ptr);

    unsigned int	srcid = resp_extension_ptr->get_src_id();
  
#ifdef SOCLIB_MODULE_DEBUG
printf("    [%s] receive VCI RESPONSE from target %d for init %d / time = %d\n", 
        name(), id, srcid, (int)time.value() );
#endif

    // get destination
//    unsigned int dest = m_rsp_routing_table[srcid];
    unsigned int dest = srcid;

    // update the transaction time
    time = time + (m_latency*UNIT_TIME);
  
#ifdef SOCLIB_MODULE_DEBUG
printf("    [%s] send VCI RESPONSE on port %d / time = %d\n", 
        name(), dest, (int)time.value() );
#endif

    (*p_to_initiator[dest])->nb_transport_bw( payload, 
                                              phase, 
                                              time);
    return tlm::TLM_COMPLETED;
} // end nb_transport_bw 

}}

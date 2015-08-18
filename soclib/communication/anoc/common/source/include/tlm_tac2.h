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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors :
 * 
 * History :
 *
 * Comment :
 *
 */

#ifndef _TLM_TAC2_H_
#define _TLM_TAC2_H_

#define MAGALI /* The 'MAGALI' define identifies all the changes due to the integration of the ST KIT with Magali */

#define TLM_MESSAGE_VERBOSE
#define TLM_TRANS_COUNTER
#define USE_CONFIG_PARAM_MANAGEMENT

#include "tac_error_reason.h"
#include "tac_if.h"
#include "tac_initiator_port.h"
#include "tac_memory.h"
#include "tac_metadata.h"
//#include "tac_multiport_router.h"
//#include "tac_protocol.h"
#include "tac_router.h"
#include "tac_simple_memory.h"
//#include "tac_simple_timer.h"
//#include "tac_simple_timer_registermap.h"
#include "tac_slave_base.h"
#include "tac_status.h"
#include "tac_target_port.h"
#include "tac_transrecord.h"

#include "tlm_address_map.h"
#include "tlm_begin_end_transrecord_if.h"
#include "tlm_begin_end_transrecord_port.h"
#include "tlm_command_line.h"
#include "tlm_event.h"
#include "tlm_event_transrecord_if.h"
#include "tlm_event_transrecord_port.h"
#include "tlm_generic_begin_end_recorder.h"
#include "tlm_generic_event_recorder.h"
#include "tlm_host_def.h"
#include "tlm_initiator_port.h"
#include "tlm_mapfile_parser.h"
#include "tlm_message.h"
#include "tlm_message_default.h"
#include "tlm_message_if.h"
#include "tlm_module.h"
#include "tlm_object_vector.h"
#include "tlm_pvt_base.h"
#include "tlm_random_time_generator.h"
#include "tlm_router_base.h"
#include "tlm_target_port.h"
#include "tlm_time_unit.h"
#include "tlm_transaction_counter.h"
#include "tlm_transrecord.h"
//#include "tlm_transrecord_database.h"
#include "tlm_transrecord_if.h"
#include "tlm_transrecord_module.h"
#include "tlm_transrecord_port.h"
#include "tlm_utilities.h"

using namespace prt_tlm_tac;

#endif /* _TLM_TAC2_H_ */

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
 * Copyright (c) UPMC, Lip6, Asim
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: nipo, mohamed
 *
 * Based on previous works by Etienne Faure & Alain Greiner, 2005
 *
 * E. Faure: Communications matérielles-logicielles dans les systèmes
 * sur puce orientés télécommunications.  PhD thesis, UPMC, 2007
 */

#include "register.h"
#include "../include/vci_mwmr_controller_cas.h"
#include "mwmr_controller_cas.h"
#include "generic_fifo.h"
#include "arithmetics.h"
#include "alloc_elems.h"
#include <cstring>
#include <algorithm>

#ifndef SOCLIB_MODULE_DEBUG
#define SOCLIB_MODULE_DEBUG 0
#endif

#if SOCLIB_MODULE_DEBUG
#define DEBUG_BEGIN do { do{} while(0)
#define DEBUG_END } while(0)
#else
#define DEBUG_BEGIN do { if (0) { do{} while(0)
#define DEBUG_END } } while(0)
#endif

namespace soclib { namespace caba {

using soclib::common::alloc_elems;
using soclib::common::dealloc_elems;

namespace Mwmr {
struct fifo_state_s {
	uint32_t status_address;
	uint32_t width;//word size
	uint32_t depth;//word size
	uint32_t buffer_address;
	uint32_t lock_address;
	bool running;
	enum SoclibMwmrWay way;
	uint32_t timer;
	uint32_t waiting;
    GenericFifo<uint32_t> *fifo;
    uint32_t in_words;
    uint32_t out_words;
};
}

#define tmpl(t) template<typename vci_param> t VciMwmrControllerCas<vci_param>

#define check_fifo() do { if ( ! m_config_fifo ) { std::cout << "VciMwmrControllerCas: No such fifo" << std::endl; return false; } }while(0)

typedef enum {
	INIT_IDLE,
	INIT_LOCK_TAKE_CAS_I,
	INIT_LOCK_TAKE_CAS_II,
    INIT_LOCK_TAKE_CAS_W,
	INIT_STATUS_READ_RPTR,
    INIT_DECIDE,
    INIT_DATA_WRITE,
    INIT_DATA_READ,
    INIT_STATUS_WRITE_RPTR,
    INIT_STATUS_WRITE_WPTR,
    INIT_STATUS_WRITE_USAGE,
    INIT_STATUS_WRITE_LOCK,
    INIT_STATUS_WRITE_LOCK_NOT_STATUS,
    INIT_DONE,
} InitFsmState;

typedef enum {
    RSP_IDLE,
    RSP_STATUS_READ_RPTR,
    RSP_STATUS_READ_WPTR,
    RSP_STATUS_READ_USAGE,
    RSP_DATA_READ_W,
    RSP_DATA_WRITE_W,
    RSP_STATUS_WAIT,
} RspFsmState;

#if SOCLIB_MODULE_DEBUG
static const char *init_states[] = {
	"INIT_IDLE",
	"INIT_LOCK_TAKE_CAS_I",
	"INIT_LOCK_TAKE_CAS_II",
    "INIT_LOCK_TAKE_CAS_W",
	"INIT_STATUS_READ_RPTR",
	"INIT_DECIDE",
	"INIT_DATA_WRITE",
	"INIT_DATA_READ",
	"INIT_STATUS_WRITE_RPTR",
	"INIT_STATUS_WRITE_WPTR",
	"INIT_STATUS_WRITE_USAGE",
	"INIT_STATUS_WRITE_LOCK",
	"INIT_STATUS_WRITE_LOCK_NOT_STATUS",
	"INIT_DONE",
};

static const char *rsp_states[] = {
    "RSP_IDLE",
    "RSP_STATUS_READ_RPTR",
    "RSP_STATUS_READ_WPTR",
    "RSP_STATUS_READ_USAGE",
    "RSP_DATA_READ_W",
    "RSP_DATA_WRITE_W",
    "RSP_STATUS_WAIT",
};
#endif

tmpl(void)::rehashConfigFifo()
{
	fifo_state_t *base =
		(m_config_way == MWMR_TO_COPROC)
		? m_to_coproc_state
		: m_from_coproc_state;
	size_t max_no =
		(m_config_way == MWMR_TO_COPROC)
		? m_n_to_coproc
		: m_n_from_coproc;
	if ( m_config_no < max_no )
		m_config_fifo = &base[m_config_no];
    else
		m_config_fifo = NULL;
}

tmpl(void)::elect()
{
DEBUG_BEGIN;
    //std::cout << name() << " elect, last one: " << m_last_elected << " ";
DEBUG_END;
	for ( size_t _i=1; _i<=m_n_all; ++_i ) {
		size_t i = (m_last_elected+_i)%m_n_all;
		fifo_state_t *st = &m_all_state[i];

DEBUG_BEGIN;
        //std::cout << i;
DEBUG_END;

		if ( st->timer != 0 ) {
DEBUG_BEGIN;
            //std::cout << "!T";
DEBUG_END;
            continue;
        }

		if ( ! st->running ) {
DEBUG_BEGIN;
            //std::cout << "!R";
DEBUG_END;
            continue;
        }

		if ( st->way == MWMR_TO_COPROC ) {
            if ( !(st->fifo->empty()) ) {
DEBUG_BEGIN;
                //std::cout << "!D";
DEBUG_END;
                st->waiting = 0;
                continue;
            }
        } else {
            if ( st->fifo->filled_status() < st->width ) {
DEBUG_BEGIN;
                //std::cout << "!D";
DEBUG_END;
                continue;
            } else {
                if ( !st->fifo->full() && st->waiting != 0 ) {
                    st->waiting--;
DEBUG_BEGIN;
                    //std::cout << "!W";
DEBUG_END;
                    continue;
                }
            }
        }

        m_current = st;
        m_last_elected = i;
        break;
	}
DEBUG_BEGIN;
    if ( m_current ) {
        //std::cout << " new one: " << m_last_elected << //std::endl;
    } else {
        //std::cout << " none" << //std::endl;
    }
DEBUG_END;
}

tmpl(bool)::on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be)
{
    uint32_t cell = (int)addr / vci_param::B;

	if ( cell < m_n_config ) {
		p_config[cell] = data;
		return true;
	}

DEBUG_BEGIN;
    std::cout << name() << " on write cell " << cell << " data: " << std::hex << data << std::endl;
DEBUG_END;

	switch ((enum SoclibMwmrRegisters)cell) {
	case MWMR_RESET:
        r_pending_reset = true;
		return true;
	case MWMR_CONFIG_FIFO_WAY:
		m_config_way = data == MWMR_FROM_COPROC ? MWMR_FROM_COPROC : MWMR_TO_COPROC;
		rehashConfigFifo();
		return true;
	case MWMR_CONFIG_FIFO_NO:
		m_config_no = data;
		rehashConfigFifo();
		return true;
    case MWMR_CONFIG_STATUS_ADDR:
		check_fifo();
		m_config_fifo->status_address = data;
		return true;
    case MWMR_CONFIG_WIDTH:
		check_fifo();
		m_config_fifo->width = data;
        assert( ((size_t)data%vci_param::B == 0) &&
               "You must configure a word-aligned width");
DEBUG_BEGIN;
        std::cout <<  name() << ": receiveid width is "<< 
                m_config_fifo->width << ", fifo size: " 
                << m_config_fifo->fifo->size() << std::endl;
DEBUG_END;
        assert( m_config_fifo->width <= m_config_fifo->fifo->size() 
                && "Hardware fifo is shorter than width, no transfer will be possible");
		return true;
    case MWMR_CONFIG_DEPTH:
		check_fifo();
		m_config_fifo->depth = data;
		return true;
    case MWMR_CONFIG_BUFFER_ADDR:
		check_fifo();
		m_config_fifo->buffer_address = data;
		return true;
    case MWMR_CONFIG_RUNNING:
		check_fifo();
        assert( m_config_fifo->width
                && "You need to first configure the channel (width, way...).");
        assert( (m_config_fifo->depth % m_config_fifo->width == 0)
                && "Fifo depth must be a multiple of width");
		m_config_fifo->running = !!data;
		return true;
    case MWMR_FIFO_FILL_STATUS:
        std::cerr << name()
                  << "MWMR_FIFO_FILL_STATUS is a Read-only address"
                  << std::endl;
		return false;
	}
	return false;
}

tmpl(bool)::on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data)
{
    uint32_t cell = (int)addr / vci_param::B;

	if ( cell < m_n_status ) {
		data = p_status[cell].read();
		return true;
	}

DEBUG_BEGIN;
    std::cout << name() << " on read cell " << cell << std::endl;
DEBUG_END;

	switch ((enum SoclibMwmrRegisters)cell) {
	case MWMR_RESET:
		return false;
	case MWMR_CONFIG_FIFO_WAY:
		data = m_config_way;
		return true;
	case MWMR_CONFIG_FIFO_NO:
		data = m_config_no;
		return true;
    case MWMR_CONFIG_STATUS_ADDR:
		check_fifo();
		data = m_config_fifo->status_address;
		return true;
    case MWMR_CONFIG_DEPTH:
		check_fifo();
		data = m_config_fifo->depth;
		return true;
    case MWMR_CONFIG_WIDTH:
		check_fifo();
		data = m_config_fifo->width;
		return true;
    case MWMR_CONFIG_BUFFER_ADDR:
		check_fifo();
		data = m_config_fifo->buffer_address;
		return true;
    case MWMR_CONFIG_RUNNING:
		check_fifo();
		data = m_config_fifo->running;
		return true;
    case MWMR_FIFO_FILL_STATUS:
		check_fifo();
		data = m_config_fifo->fifo->filled_status();
		return true;
	}
	return false;
}

tmpl(void)::reset()
{
	m_current = NULL;
	m_config_way = MWMR_TO_COPROC;
	m_config_no = 0;
	m_config_fifo = NULL;
    m_last_elected = 0;

	for ( size_t i=0; i<m_n_all; ++i ) {
		m_all_state[i].running = false;
		m_all_state[i].timer = 0;
		m_all_state[i].fifo->init();
		m_all_state[i].waiting = 64;
	}
}

tmpl(void)::transition()
{
	if (!p_resetn) {
        r_pending_reset = false;
		m_vci_target_fsm.reset();
        r_init_fsm = INIT_IDLE;
        r_rsp_fsm = RSP_IDLE;
		reset();
		return;
	}

    bool current_fifo_get = false;
    bool current_fifo_put = false;

	m_vci_target_fsm.transition();

	for ( size_t i=0; i<m_n_all; ++i )
		if ( m_all_state[i].timer )
			m_all_state[i].timer--;

#if SOCLIB_MODULE_DEBUG
    if ((InitFsmState)r_init_fsm.read() != INIT_IDLE || r_rsp_fsm.read() != RSP_IDLE) {
        std::cout << name() << " init: " << init_states[r_init_fsm.read()] << " rsp: " << rsp_states[r_rsp_fsm.read()] << std::endl;
    }
#endif

	switch ((InitFsmState)r_init_fsm.read()) {
	case INIT_IDLE:
		if ( m_current )
			r_init_fsm = INIT_LOCK_TAKE_CAS_I;
		else {
            if ( r_pending_reset ) {
                reset();
                break;
            }
			elect();
            if ( m_current )
                m_n_elect++;
        }
		break;

	case INIT_LOCK_TAKE_CAS_I:
        m_n_lock_spin++;
	case INIT_STATUS_WRITE_RPTR:
	case INIT_STATUS_READ_RPTR:
	case INIT_LOCK_TAKE_CAS_II:
	case INIT_STATUS_WRITE_WPTR:
		if ( p_vci_initiator.cmdack.read() )
			// Select next state...
			r_init_fsm = r_init_fsm+1;
		break;

	case INIT_STATUS_WRITE_USAGE:
        if ( p_vci_initiator.cmdack.read() ) {
            r_init_fsm = INIT_STATUS_WRITE_LOCK;
        }
        break;


	case INIT_STATUS_WRITE_LOCK:
	case INIT_STATUS_WRITE_LOCK_NOT_STATUS:
		if ( p_vci_initiator.cmdack.read() )
			r_init_fsm = INIT_DONE;
		break;

	case INIT_LOCK_TAKE_CAS_W:
		if ( !p_vci_initiator.rspval.read() )
			break;
        r_status_modified = false;
		r_init_fsm = ( p_vci_initiator.rdata.read() == vci_param::STORE_COND_ATOMIC )
			? INIT_STATUS_READ_RPTR
			: INIT_LOCK_TAKE_CAS_I;//should insert a dela before retrying the CAS
		break;

	case INIT_DECIDE:
        if (r_rsp_fsm.read() != RSP_IDLE)
            break;
DEBUG_BEGIN;
        std::cout << name() << " deciding for " << m_last_elected
                  << ", status: " << r_current_usage.read()
                  << ", rptr: " << r_current_rptr.read()
                  << ", wptr: " << r_current_wptr.read()
                  << ", fifo: " << m_current->fifo->filled_status()
                  << "/" << m_current->fifo->size()
                  << ": ";
DEBUG_END;

		if ( m_current->way == MWMR_FROM_COPROC ) {
//FIXME
            // If RAM FIFO have at least a full FIFO data space AND the
            // MWMR FIFO is FULL OR the RAM FIFO have at least one atomic
            // data space AND the MWMR hace at least one atomic data AND
            // timer is finished THEN send the data
            if ( m_current->depth - r_current_usage >= m_current->width ) {

                //Compute the number of words availabile on the mwmr channel (ram) ( min( max_to_depth, empty_spaces) )
                size_t word_to_send = std::min<size_t>(m_current->depth - r_current_wptr, m_current->depth - r_current_usage);

                // Dont transfer more than possible (max of the hardware fifo)
                word_to_send = std::min<size_t>(
                    word_to_send,//m_current->depth - r_current_usage,
                    m_current->fifo->filled_status());

                //Normalize to an item size
                word_to_send -= word_to_send % m_current->width;


                if ( word_to_send ) {

                    //Align the first burst
                    size_t transfer_addr = (m_current->buffer_address >> 2) + r_current_wptr ;
                    size_t offset = transfer_addr % m_max_burst;
                    size_t max_burst = std::min<size_t>(word_to_send, m_max_burst - offset);

                    //Assign the necessary fields
                    r_plen = max_burst;
                    r_part_count = max_burst;
                    r_cmd_count = word_to_send;
                    r_init_fsm = INIT_DATA_WRITE;
                    r_status_modified = true;

                    m_n_xfers++;

    
                    //Compute the expected number of response packet 
                    size_t first_slice = offset ? m_max_burst - offset : 0 ;
                    size_t last_slice = (offset + word_to_send) % m_max_burst; 
                    r_rsp_count = ((word_to_send - first_slice - last_slice) / m_max_burst )// number of aligned packet
                                    + (first_slice ? 1 : 0)                                 // + 1 if we got a first slice 
                                    + (last_slice ? 1 : 0) ;                                // + 1 if we got a last slice 

DEBUG_BEGIN;
                    std::cout << "going to write to the mwmr channel (ram) " << word_to_send << " words" << std::endl;
                    std::cout << "transfert address" << transfer_addr << " offset " << offset << " max_burst " << max_burst << std::endl;
DEBUG_END;
                    break;
                }
			}
		} else {
            // If the MWMR FIFO is empty AND ((the RAM FIFO has a full
            // FIFO data) OR (the RAM FIFO has some data and the timer
            // is 0)) THEN get data from the RAM FIFO
			if ( r_current_usage >= m_current->width ) {
                //to assure contiguous address
                size_t word_to_get = std::min<size_t>(m_current->depth - r_current_rptr, r_current_usage);
                // Dont transfer more than possible
                word_to_get = std::min<size_t>(
                    word_to_get,//r_current_usage,
                    m_fifo_to_coproc_depth-m_current->fifo->filled_status()  );
                //Normalize to an item size
                word_to_get -= word_to_get%m_current->width;
                
                //size_t max_burst = std::min<size_t>(word_to_get, m_max_burst);
                

                if ( word_to_get ) {

                    //align the burst!
                    size_t transfer_addr = (m_current->buffer_address + r_current_wptr * vci_param::B) >> 2 ;
                    size_t offset = transfer_addr % m_max_burst;
                    size_t max_burst = std::min<size_t>(word_to_get, m_max_burst - offset);

                    r_plen = max_burst;
                    r_part_count = max_burst;
                    r_cmd_count = word_to_get;
                    r_rsp_count = word_to_get;
                    r_init_fsm = INIT_DATA_READ;
                    r_status_modified = true;

                    m_n_xfers++;
DEBUG_BEGIN;
                    std::cout << "going to put " << word_to_get << " words to coproc" << std::endl;
                    std::cout << "read transfert address" << transfer_addr << " offset " << offset << " max_burst " << max_burst << std::endl;
DEBUG_END;
                    break;
                }
            }
        }
DEBUG_BEGIN;
        std::cout << "going to bail out: no room for transfer" << std::endl;
DEBUG_END;
        if ( r_status_modified.read() ) {
            m_current->waiting = 64;
            r_init_fsm = INIT_STATUS_WRITE_RPTR;
        } else {
            if ( m_current->waiting )
                m_current->waiting--;

            m_n_bailout++;
            r_init_fsm =  INIT_STATUS_WRITE_LOCK_NOT_STATUS;
        }
		break;
	case INIT_DATA_WRITE:
		if ( p_vci_initiator.cmdack.read() ) {

            if ( r_cmd_count == 1 )
                r_init_fsm = INIT_DECIDE;
        
            size_t next_wptr = (r_current_wptr  + 1) % m_current->depth;
            size_t next_cmd_count = r_cmd_count - 1;
            
            if(r_part_count == 1)
            {
                size_t transfer_addr = (m_current->buffer_address + next_wptr * vci_param::B) >> 2 ;
                size_t offset = transfer_addr % m_max_burst;
                size_t max_burst = std::min<size_t>(next_cmd_count, m_max_burst - offset);

                //size_t max_burst = std::min<size_t>(r_cmd_count - r_plen, m_max_burst);

DEBUG_BEGIN;
                std::cout << name() << " --> refill part @ " <<  std::hex << 
                            transfer_addr << ", cmd_count :" << r_cmd_count
                            << ", offset " << offset << ", going to send nb: " << max_burst << " on VCI" << std::endl;
DEBUG_END;
                r_part_count = max_burst;
                r_plen = max_burst;
            }else
            {
                r_part_count = r_part_count -1;
            }

            r_cmd_count = next_cmd_count;
            r_current_usage = r_current_usage + 1;
            r_current_wptr = next_wptr;
            current_fifo_get = true;
		}
		break;
	case INIT_DATA_READ:

		if ( p_vci_initiator.cmdack.read() ) {
            if ( r_cmd_count <= m_max_burst )
                r_init_fsm = INIT_DECIDE;

            size_t next_rptr = (r_current_rptr + r_plen) % m_current->depth;
			size_t next_cmd_count  = r_cmd_count - r_plen.read();
            //size_t max_to_get = std::min<size_t>(r_cmd_count - r_plen, m_max_burst);
            
            size_t transfer_addr = (m_current->buffer_address + next_rptr * vci_param::B) >> 2 ;
            size_t offset = transfer_addr % m_max_burst;
            size_t max_burst = std::min<size_t>(next_cmd_count, m_max_burst - offset);
DEBUG_BEGIN;
            std::cout << name() << "--> send read cmd data @" << std::hex << (m_current->buffer_address + r_current_rptr *vci_param::B) << ", nb: " << r_plen.read() << std::endl;
DEBUG_END;
			r_cmd_count  = next_cmd_count;
            r_current_rptr = next_rptr;
            r_current_usage = r_current_usage - r_plen;

			r_part_count = max_burst;
            r_plen = max_burst;
		}
		break;

    case INIT_DONE:
        if ( r_rsp_fsm.read() != RSP_IDLE )
            break;
        m_current->timer = m_plaps;
        m_current = NULL;
        r_init_fsm = INIT_IDLE;
	}

    switch ((RspFsmState)r_rsp_fsm.read()) {
    case RSP_IDLE:
        switch ((InitFsmState)r_init_fsm.read()) {
        case INIT_STATUS_READ_RPTR:
            r_rsp_count = 3;
            r_rsp_fsm = RSP_STATUS_READ_RPTR;
            break;
        case INIT_DATA_WRITE:
            r_rsp_fsm = RSP_DATA_WRITE_W;
            break;
        case INIT_DATA_READ:
            r_rsp_fsm = RSP_DATA_READ_W;
            break;
        case INIT_STATUS_WRITE_RPTR:
            r_rsp_count = 4;
            r_rsp_fsm = RSP_STATUS_WAIT;
            break;
        case INIT_STATUS_WRITE_LOCK_NOT_STATUS:
            r_rsp_count = 1;
            r_rsp_fsm = RSP_STATUS_WAIT;
            break;
        default:
            break;
        }
        break;

	case RSP_DATA_WRITE_W:
DEBUG_BEGIN;
        std::cout << name() << " waiting for write " << r_rsp_count.read() << " words to go" << std::endl;
DEBUG_END;
		if ( p_vci_initiator.rspval.read() ) {
			if ( r_rsp_count == 1 )//|| p_vci_initiator.reop.read() )
				r_rsp_fsm = RSP_IDLE;
			r_rsp_count = r_rsp_count - 1;
		}
		break;
	case RSP_DATA_READ_W:
DEBUG_BEGIN;
        std::cout << name() << " waiting for read " << r_rsp_count.read() << " words to go" << std::endl;
DEBUG_END;
		if ( p_vci_initiator.rspval.read() ) {
            current_fifo_put = true;
			if ( r_rsp_count == 1 )
				r_rsp_fsm = RSP_IDLE;
			r_rsp_count = r_rsp_count-1;
		}
		break;
    case RSP_STATUS_READ_RPTR:
DEBUG_BEGIN;
        std::cout << name() << " waiting for rptr" << std::endl;
DEBUG_END;
        r_current_rptr = p_vci_initiator.rdata.read();
		if ( p_vci_initiator.rspval.read() )
            r_rsp_fsm = RSP_STATUS_READ_WPTR;
        break;
    case RSP_STATUS_READ_WPTR:
DEBUG_BEGIN;
        std::cout << name() << " waiting for wptr" << std::endl;
DEBUG_END;
        r_current_wptr = p_vci_initiator.rdata.read();
		if ( p_vci_initiator.rspval.read() )
            r_rsp_fsm = RSP_STATUS_READ_USAGE;
        break;
    case RSP_STATUS_READ_USAGE:
DEBUG_BEGIN;
        std::cout << name() << " waiting for usage" << std::endl;
DEBUG_END;
        r_current_usage = p_vci_initiator.rdata.read();
		if ( p_vci_initiator.rspval.read() )
            r_rsp_fsm = RSP_IDLE;
        break;
    case RSP_STATUS_WAIT:
DEBUG_BEGIN;
        std::cout << name() << " waiting for status write " << r_rsp_count.read() << " words to go" << std::endl;
DEBUG_END;
		if ( p_vci_initiator.rspval.read() ) {
			if ( r_rsp_count == 1 || p_vci_initiator.reop.read() )
				r_rsp_fsm = RSP_IDLE;
			r_rsp_count = r_rsp_count-1;

		}
		break;
    }

    for ( size_t i = 0; i<m_n_from_coproc; ++i ) {
        fifo_state_t *st = &m_from_coproc_state[i];
        bool coproc_sent_data = p_from_coproc[i].r.read() && p_from_coproc[i].rok.read();
        bool vci_took_data = false;
        if ( st == m_current )
            vci_took_data = current_fifo_get;
        if ( coproc_sent_data ) {
DEBUG_BEGIN;
            std::cout << name() << " getting " << std::hex << p_from_coproc[i].data.read() << " from coproc" << std::endl;
DEBUG_END;
            if ( vci_took_data )
                st->fifo->put_and_get(p_from_coproc[i].data.read());
            else
                st->fifo->simple_put(p_from_coproc[i].data.read());
        } else {
            if ( vci_took_data )
                st->fifo->simple_get();
        }

        if ( vci_took_data )
            st->out_words++;
        if ( coproc_sent_data )
            st->in_words++;
    }
    for ( size_t i = 0; i<m_n_to_coproc; ++i ) {
        fifo_state_t *st = &m_to_coproc_state[i];
        bool coproc_took_data = p_to_coproc[i].w.read() && p_to_coproc[i].wok.read();
DEBUG_BEGIN;
        if (coproc_took_data)
            std::cout << name() << " put " << std::hex << p_to_coproc[i].data.read() << " to fifo" << std::endl;
DEBUG_END;
        bool vci_gave_data = false;
        if ( st == m_current )
            vci_gave_data = current_fifo_put;
        if ( vci_gave_data ) {
DEBUG_BEGIN;
            std::cout << name() << " getting " << std::hex << p_vci_initiator.rdata.read() << " from vci" << std::endl;
DEBUG_END;
            if ( coproc_took_data )
                st->fifo->put_and_get(p_vci_initiator.rdata.read());
            else
                st->fifo->simple_put(p_vci_initiator.rdata.read());
        } else {
            if ( coproc_took_data )
                st->fifo->simple_get();
        }

        if ( vci_gave_data )
            st->in_words++;
        if ( coproc_took_data )
            st->out_words++;
    }
}

tmpl(void)::genMoore()
{
	m_vci_target_fsm.genMoore();

	p_vci_initiator.rspack = true;

    size_t plen = 0;
	switch ((InitFsmState)r_init_fsm.read()) {
	case INIT_LOCK_TAKE_CAS_W:
	case INIT_IDLE:
	case INIT_DECIDE:
    case INIT_DONE:
		p_vci_initiator.cmdval = false;
		break;
	case INIT_LOCK_TAKE_CAS_I:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.wdata = 0;
		p_vci_initiator.address = m_current->status_address+3*vci_param::B;
		p_vci_initiator.cmd = vci_param::CMD_STORE_COND;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = false;
        plen = 2;
		break;
	case INIT_LOCK_TAKE_CAS_II:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.wdata = 1;
		p_vci_initiator.address = m_current->status_address+3*vci_param::B;
		p_vci_initiator.cmd = vci_param::CMD_STORE_COND;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        plen = 2;
		break;
	case INIT_STATUS_READ_RPTR:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address;
		p_vci_initiator.cmd = vci_param::CMD_READ;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        plen = 3;
		break;
	case INIT_DATA_WRITE:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->buffer_address + r_current_wptr*vci_param::B;
		p_vci_initiator.wdata = m_current->fifo->read();
DEBUG_BEGIN;
        std::cout << name() << " putting @" <<  std::hex << (m_current->buffer_address + r_current_wptr*vci_param::B) << ": " << m_current->fifo->read() << ", nb: " << r_plen.read() << " eop: " << (r_part_count==1) << " on VCI" << std::endl;
DEBUG_END;
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = (r_part_count == 1);
        plen =  r_plen;
		break;
	case INIT_DATA_READ:
		p_vci_initiator.cmdval = (r_plen == r_part_count);
		p_vci_initiator.address = m_current->buffer_address + r_current_rptr*vci_param::B;
		p_vci_initiator.cmd = vci_param::CMD_READ;
DEBUG_BEGIN;
        std::cout << name() << " reading data @" << std::hex << (m_current->buffer_address + r_current_rptr*vci_param::B) << ", nb: " << r_plen.read() << std::endl;
DEBUG_END;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = 1;
        plen =  r_plen;
		break;
	case INIT_STATUS_WRITE_RPTR:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address;
		p_vci_initiator.wdata = r_current_rptr.read();
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = false;
        plen = 4;
		break;
	case INIT_STATUS_WRITE_WPTR:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address+vci_param::B;
		p_vci_initiator.wdata = r_current_wptr.read();
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = false;
        plen = 4 ;
		break;
	case INIT_STATUS_WRITE_USAGE:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address+vci_param::B*2;
		p_vci_initiator.wdata = r_current_usage.read();
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = false;
        plen = 4 ;
		break;
	case INIT_STATUS_WRITE_LOCK:
        plen = 4;
	case INIT_STATUS_WRITE_LOCK_NOT_STATUS:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address+vci_param::B*3;
		p_vci_initiator.wdata = 0;
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        plen = plen ? plen : 1;
		break;
    default:
       assert("impossible case"); 
	}

	p_vci_initiator.contig = true;
	p_vci_initiator.cons = 1;
	p_vci_initiator.plen = plen*vci_param::B;
	p_vci_initiator.wrap = 0;
	p_vci_initiator.cfixed = true;
	p_vci_initiator.clen = 0;
	p_vci_initiator.srcid = m_ident;
	p_vci_initiator.trdid = 0;
	p_vci_initiator.pktid = 0;

DEBUG_BEGIN;
    if(p_vci_initiator.cmdval.read())
        ;//std::cout << name() << " sending cmd " << p_vci_initiator.address.read() << ", size(one cycle back): "<< p_vci_initiator.plen.read() << ", seted plen " << plen*vci_param::B << std::endl;
DEBUG_END;

    for ( size_t i = 0; i<m_n_from_coproc; ++i )
        p_from_coproc[i].r = m_from_coproc_state[i].fifo->wok();
    for ( size_t i = 0; i<m_n_to_coproc; ++i ) {
        p_to_coproc[i].w = m_to_coproc_state[i].fifo->rok();
        p_to_coproc[i].data = m_to_coproc_state[i].fifo->read();
    }
}

tmpl(/**/)::VciMwmrControllerCas(
    sc_module_name name,
    const MappingTable &mt,
    const IntTab &srcid,
    const IntTab &tgtid,
	const size_t plaps,
	const size_t fifo_to_coproc_depth,
	const size_t fifo_from_coproc_depth,
	const size_t n_to_coproc,
	const size_t n_from_coproc,
	const size_t n_config,
	const size_t n_status,
    const size_t max_burst_size)//burst_align
		   : caba::BaseModule(name),
		   m_vci_target_fsm(p_vci_target, mt.getSegmentList(tgtid)),
           m_ident(mt.indexForId(srcid)),
           m_fifo_to_coproc_depth(fifo_to_coproc_depth),
           m_fifo_from_coproc_depth(fifo_from_coproc_depth),
           m_plaps(plaps),
           m_n_to_coproc(n_to_coproc),
           m_n_from_coproc(n_from_coproc),
           m_n_all(n_to_coproc+n_from_coproc),
           m_n_config(n_config),
           m_n_status(n_status),
           m_all_state(new fifo_state_t[m_n_all]),
           m_to_coproc_state(m_all_state),
           m_from_coproc_state(&m_all_state[m_n_to_coproc]),
           r_config(alloc_elems<sc_signal<uint32_t> >("r_config", n_config)),
           r_init_fsm("init_fsm"),
           r_rsp_fsm("rsp_fsm"),
           r_cmd_count("cmd_count"),
           r_rsp_count("rsp_count"),
           r_current_rptr("current_rptr"),
           r_current_wptr("current_wptr"),
           r_current_usage("current_usage"),
           m_n_elect(0),
           m_n_lock_spin(0),
           m_n_bailout(0),
           m_n_xfers(0),
           m_config_way(MWMR_TO_COPROC),
           m_config_no(0),
           m_config_fifo(NULL),
           m_current(NULL),
           m_last_elected(0),
           m_max_burst(std::min<size_t>(((1<< vci_param::K)-1)/vci_param::B, max_burst_size)),//min of (plen_max, max_burst_size)
		   p_clk("clk"),
		   p_resetn("resetn"),
		   p_vci_target("vci_target"),
		   p_vci_initiator("vci_initiator"),
		   p_from_coproc(alloc_elems<FifoInput<uint32_t> >("from_coproc", n_from_coproc)),
		   p_to_coproc(alloc_elems<FifoOutput<uint32_t> >("to_coproc", n_to_coproc)),
		   p_config(alloc_elems<sc_out<uint32_t> >("config", n_config)),
		   p_status(alloc_elems<sc_in<uint32_t> >("status", n_status))
{
	m_vci_target_fsm.on_read_write(on_read, on_write);

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();

    std::memset(m_all_state, 0, sizeof(*m_all_state)*m_n_all);

    // If this fail, please implement word width adaptation in access
    // to VCI data.
    soclib_static_assert(vci_param::B == sizeof(uint32_t));

    assert(vci_param::K >= 2 && "The vci plen field is to small");

    for ( size_t i = 0; i<m_n_from_coproc; ++i ) {
        std::ostringstream o;
        o << "fifo_from_coproc[" << i << "]";
        m_from_coproc_state[i].way = MWMR_FROM_COPROC;
        m_from_coproc_state[i].fifo = new GenericFifo<uint32_t>(o.str(), m_fifo_from_coproc_depth);
    }
    for ( size_t i = 0; i<m_n_to_coproc; ++i ) {
        std::ostringstream o;
        o << "fifo_to_coproc[" << i << "]";
        m_to_coproc_state[i].way = MWMR_TO_COPROC;
        m_to_coproc_state[i].fifo = new GenericFifo<uint32_t>(o.str(), m_fifo_to_coproc_depth);
    }
}

tmpl(/**/)::~VciMwmrControllerCas()
{
    std::cout << std::dec;

    for ( size_t i = 0; i<m_n_from_coproc; ++i ) {
        fifo_state_t *st = &m_from_coproc_state[i];
        std::cout << name()
                  << " from coproc " << i
                  << ": in_words: " << st->in_words
                  << ", out_words: " << st->out_words
                  << ", fifo: " << st->fifo->filled_status()
                  << "/" << st->fifo->size()
                  << std::endl;
            
    }
    for ( size_t i = 0; i<m_n_to_coproc; ++i ) {
        fifo_state_t *st = &m_to_coproc_state[i];
        std::cout << name()
                  << " to coproc " << i
                  << ": in_words: " << st->in_words
                  << ", out_words: " << st->out_words
                  << ", fifo: " << st->fifo->filled_status()
                  << "/" << st->fifo->size()
                  << std::endl;
            
    }

    std::cout << name()
              << " elects: " <<  m_n_elect
              << ", spins: " <<  m_n_lock_spin
              << ", bailouts: " <<  m_n_bailout
              << ", xfers: " <<  m_n_xfers
              << std::endl;

    for ( size_t i = 0; i<m_n_all; ++i )
        delete m_all_state[i].fifo;
    dealloc_elems(p_from_coproc, m_n_from_coproc); 
    dealloc_elems(p_to_coproc, m_n_to_coproc); 
    dealloc_elems(p_config, m_n_config);
    dealloc_elems(p_status, m_n_status);
    dealloc_elems(r_config, m_n_config);
    delete [] m_all_state;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


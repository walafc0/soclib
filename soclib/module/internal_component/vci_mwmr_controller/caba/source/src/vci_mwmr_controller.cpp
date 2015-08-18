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
 * Maintainers: nipo
 *
 * Based on previous works by Etienne Faure & Alain Greiner, 2005
 *
 * E. Faure: Communications matérielles-logicielles dans les systèmes
 * sur puce orientés télécommunications.  PhD thesis, UPMC, 2007
 */

#include "register.h"
#include "../include/vci_mwmr_controller.h"
#include "mwmr_controller.h"
#include "generic_fifo.h"
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
	uint32_t width;
	uint32_t depth;
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

#define tmpl(t) template<typename vci_param> t VciMwmrController<vci_param>

#define check_fifo() if ( ! m_config_fifo ) return false

typedef enum {
	INIT_IDLE,
	INIT_LOCK_TAKE_RAMLOCK,
	INIT_LOCK_TAKE_RAMLOCK_W,
	INIT_LOCK_TAKE_LL,
    INIT_LOCK_TAKE_LL_W,
	INIT_LOCK_TAKE_SC,
    INIT_LOCK_TAKE_SC_W,
	INIT_STATUS_READ_RPTR,
	INIT_DECIDE,
	INIT_DATA_WRITE,
	INIT_DATA_READ,
	INIT_STATUS_WRITE_RPTR,
	INIT_STATUS_WRITE_WPTR,
	INIT_STATUS_WRITE_USAGE,
	INIT_STATUS_WAIT,
	INIT_STATUS_WRITE_LOCK,
	INIT_STATUS_WRITE_LOCK_NOT_STATUS,
	INIT_STATUS_WRITE_RAMLOCK,
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
	"INIT_LOCK_TAKE_RAMLOCK",
	"INIT_LOCK_TAKE_RAMLOCK_W",
	"INIT_LOCK_TAKE_LL",
    "INIT_LOCK_TAKE_LL_W",
	"INIT_LOCK_TAKE_SC",
    "INIT_LOCK_TAKE_SC_W",
	"INIT_STATUS_READ_RPTR",
	"INIT_DECIDE",
	"INIT_DATA_WRITE",
	"INIT_DATA_READ",
	"INIT_STATUS_WRITE_RPTR",
	"INIT_STATUS_WRITE_WPTR",
	"INIT_STATUS_WRITE_USAGE",
	"INIT_STATUS_WAIT",
	"INIT_STATUS_WRITE_LOCK",
	"INIT_STATUS_WRITE_LOCK_NOT_STATUS",
	"INIT_STATUS_WRITE_RAMLOCK",
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
    std::cout << name() << " elect, last one: " << m_last_elected << " ";
DEBUG_END;
	for ( size_t _i=1; _i<=m_n_all; ++_i ) {
		size_t i = (m_last_elected+_i)%m_n_all;
		fifo_state_t *st = &m_all_state[i];

DEBUG_BEGIN;
        std::cout << i;
DEBUG_END;

		if ( st->timer != 0 ) {
DEBUG_BEGIN;
            std::cout << "!T";
DEBUG_END;
            continue;
        }

		if ( ! st->running ) {
DEBUG_BEGIN;
            std::cout << "!R";
DEBUG_END;
            continue;
        }

		if ( st->way == MWMR_TO_COPROC ) {
            if ( !(st->fifo->empty()) ) {
DEBUG_BEGIN;
                std::cout << "!D";
DEBUG_END;
                st->waiting = 0;
                continue;
            }
        } else {
            if ( st->fifo->filled_status()*sizeof(uint32_t) < st->width ) {
DEBUG_BEGIN;
                std::cout << "!D";
DEBUG_END;
                continue;
            } else {
                if ( !st->fifo->full() && st->waiting != 0 ) {
                    st->waiting--;
DEBUG_BEGIN;
                    std::cout << "!W";
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
        std::cout << " new one: " << m_last_elected << std::endl;
    } else {
        std::cout << " none" << std::endl;
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
               "You must configure word-aligned widths");
        assert( m_config_fifo->width <= m_config_fifo->fifo->size() * sizeof(uint32_t)
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
    case MWMR_CONFIG_LOCK_ADDR:
        assert( !m_use_llsc
                && "You must not configure lock address with a"
                " LL-SC protocol, looks like a protocol mismatch");
		check_fifo();
		m_config_fifo->lock_address = data;
		return true;
    case MWMR_CONFIG_RUNNING:
		check_fifo();
        assert( (!m_use_llsc || m_config_fifo->lock_address==0)
                && "You must not configure lock address with a"
                " LL-SC protocol, looks like a protocol mismatch");
        assert( m_config_fifo->width
                && "You API for this module is not updated."
                " Please update the API and recompile the software");
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
    case MWMR_CONFIG_LOCK_ADDR:
        if ( m_use_llsc )
            assert(!"You must not read lock address with a LL-SC protocol, looks like a protocol mismatch");
		check_fifo();
		data = m_config_fifo->lock_address;
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
			r_init_fsm = m_use_llsc ? INIT_LOCK_TAKE_LL : INIT_LOCK_TAKE_RAMLOCK;
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

	case INIT_LOCK_TAKE_LL:
        m_n_lock_spin++;
	case INIT_STATUS_WRITE_RPTR:
	case INIT_STATUS_READ_RPTR:
	case INIT_LOCK_TAKE_RAMLOCK:
	case INIT_LOCK_TAKE_SC:
	case INIT_STATUS_WRITE_WPTR:
		if ( p_vci_initiator.cmdack.read() )
			// Select next state...
			r_init_fsm = r_init_fsm+1;
		break;

	case INIT_STATUS_WRITE_USAGE:
        if ( p_vci_initiator.cmdack.read() ) {
            if ( m_use_llsc )
                r_init_fsm = INIT_STATUS_WRITE_LOCK;
            else
                r_init_fsm = INIT_STATUS_WAIT;
        }
        break;

	case INIT_STATUS_WAIT:
		if ( r_rsp_fsm.read() == RSP_IDLE )
			r_init_fsm = INIT_STATUS_WRITE_RAMLOCK;
		break;

	case INIT_STATUS_WRITE_LOCK:
	case INIT_STATUS_WRITE_LOCK_NOT_STATUS:
	case INIT_STATUS_WRITE_RAMLOCK:
		if ( p_vci_initiator.cmdack.read() )
			r_init_fsm = INIT_DONE;
		break;

	case INIT_LOCK_TAKE_LL_W:
		if ( !p_vci_initiator.rspval.read() )
			break;
		r_init_fsm = ( p_vci_initiator.rdata.read() == 0 )
			? INIT_LOCK_TAKE_SC
			: INIT_LOCK_TAKE_LL;
		break;
	case INIT_LOCK_TAKE_SC_W:
		if ( !p_vci_initiator.rspval.read() )
			break;
        r_status_modified = false;
		r_init_fsm = ( p_vci_initiator.rdata.read() == 0 )
			? INIT_STATUS_READ_RPTR
			: INIT_LOCK_TAKE_LL;
		break;
	case INIT_LOCK_TAKE_RAMLOCK_W:
		if ( !p_vci_initiator.rspval.read() )
			break;
        r_status_modified = false;
		r_init_fsm = ( p_vci_initiator.rdata.read() == 0 )
			? INIT_STATUS_READ_RPTR
			: INIT_LOCK_TAKE_RAMLOCK;
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
            // If RAM FIFO have at least a full FIFO data space AND the
            // MWMR FIFO is FULL OR the RAM FIFO have at least one atomic
            // data space AND the MWMR hace at least one atomic data AND
            // timer is finished THEN send the data
            if ( m_current->depth - r_current_usage >= m_current->width ) {
                // Dont transfer more than possible
                size_t bytes_to_send = std::min<size_t>(
                    m_current->depth - r_current_usage,
                    m_current->fifo->filled_status() * sizeof(uint32_t) );
                //Normalize to an item size
                bytes_to_send -= bytes_to_send % m_current->width;

                if ( bytes_to_send ) {
                    r_cmd_count = bytes_to_send/vci_param::B;
                    r_rsp_count = bytes_to_send/vci_param::B;
                    r_init_fsm = INIT_DATA_WRITE;
                    r_status_modified = true;

                    m_n_xfers++;
DEBUG_BEGIN;
                    std::cout << "going to read from coproc " << bytes_to_send/vci_param::B << " words" << std::endl;
DEBUG_END;
                    break;
                }
			}
		} else {
            // If the MWMR FIFO is empty AND ((the RAM FIFO has a full
            // FIFO data) OR (the RAM FIFO has some data and the timer
            // is 0)) THEN get data from the RAM FIFO
			if ( r_current_usage >= m_current->width ) {
                // Dont transfer more than possible
                size_t bytes_to_get = std::min<size_t>(
                    r_current_usage,
                    m_fifo_to_coproc_depth-m_current->fifo->filled_status() * sizeof(uint32_t) );
                //Normalize to an item size
                bytes_to_get -= bytes_to_get%m_current->width;
                
                if ( bytes_to_get ) {
                    r_cmd_count = bytes_to_get/vci_param::B;
                    r_rsp_count = bytes_to_get/vci_param::B;
                    r_init_fsm = INIT_DATA_READ;
                    r_status_modified = true;

                    m_n_xfers++;
DEBUG_BEGIN;
                    std::cout << "going to put " << bytes_to_get/vci_param::B << " words to coproc" << std::endl;
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
            r_init_fsm = m_use_llsc
                ? INIT_STATUS_WRITE_LOCK_NOT_STATUS
                : INIT_STATUS_WRITE_RAMLOCK;
        }
		break;
	case INIT_DATA_WRITE:
		if ( p_vci_initiator.cmdack.read() ) {
			if ( r_cmd_count == 1 )
				r_init_fsm = INIT_DECIDE;
			r_cmd_count = r_cmd_count-1;
            r_current_usage = r_current_usage+vci_param::B;
            r_current_wptr = (r_current_wptr + vci_param::B) % m_current->depth;
            current_fifo_get = true;
		}
		break;
	case INIT_DATA_READ:
		if ( p_vci_initiator.cmdack.read() ) {
			if ( r_cmd_count == 1 )
				r_init_fsm = INIT_DECIDE;
			r_cmd_count = r_cmd_count-1;
            r_current_usage = r_current_usage-vci_param::B;
            r_current_rptr = (r_current_rptr + vci_param::B) % m_current->depth;
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
            r_rsp_count = m_use_llsc ? 4 : 3;
            r_rsp_fsm = RSP_STATUS_WAIT;
            break;
        case INIT_STATUS_WRITE_RAMLOCK:
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
			if ( r_rsp_count == 1 || p_vci_initiator.reop.read() )
				r_rsp_fsm = RSP_IDLE;
			r_rsp_count = r_rsp_count-1;
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
	case INIT_LOCK_TAKE_RAMLOCK_W:
	case INIT_LOCK_TAKE_LL_W:
	case INIT_STATUS_WAIT:
	case INIT_LOCK_TAKE_SC_W:
	case INIT_IDLE:
	case INIT_DECIDE:
    case INIT_DONE:
		p_vci_initiator.cmdval = false;
		break;
	case INIT_LOCK_TAKE_RAMLOCK:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->lock_address;
		p_vci_initiator.cmd = vci_param::CMD_READ;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        plen = 1;
		break;
	case INIT_LOCK_TAKE_LL:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address+3*vci_param::B;
		p_vci_initiator.cmd = vci_param::CMD_LOCKED_READ;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        plen = 1;
		break;
	case INIT_LOCK_TAKE_SC:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.wdata = 1;
		p_vci_initiator.address = m_current->status_address+3*vci_param::B;
		p_vci_initiator.cmd = vci_param::CMD_STORE_COND;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        plen = 1;
		break;
	case INIT_STATUS_READ_RPTR:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address;
		p_vci_initiator.cmd = vci_param::CMD_READ;
		p_vci_initiator.be = 0xf;
        plen = 3;
		p_vci_initiator.eop = true;
		break;
	case INIT_DATA_WRITE:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->buffer_address + r_current_wptr;
		p_vci_initiator.wdata = m_current->fifo->read();
DEBUG_BEGIN;
        std::cout << name() << " putting @" << (m_current->buffer_address + r_current_wptr) << ": " << m_current->fifo->read() << " on VCI" << std::endl;
DEBUG_END;
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = (r_cmd_count==1);
		break;
	case INIT_DATA_READ:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->buffer_address + r_current_rptr;
		p_vci_initiator.cmd = vci_param::CMD_READ;
DEBUG_BEGIN;
        std::cout << name() << " reading data @" << (m_current->buffer_address + r_current_rptr) << std::endl;
DEBUG_END;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = (r_cmd_count==1);
		break;
	case INIT_STATUS_WRITE_RPTR:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address;
		p_vci_initiator.wdata = r_current_rptr.read();
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = false;
        plen = m_use_llsc ? 4 : 3;
		break;
	case INIT_STATUS_WRITE_WPTR:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address+vci_param::B;
		p_vci_initiator.wdata = r_current_wptr.read();
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = false;
        plen = m_use_llsc ? 4 : 3;
		break;
	case INIT_STATUS_WRITE_USAGE:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->status_address+vci_param::B*2;
		p_vci_initiator.wdata = r_current_usage.read();
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = ! m_use_llsc;
        plen = m_use_llsc ? 4 : 3;
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
		break;
	case INIT_STATUS_WRITE_RAMLOCK:
		p_vci_initiator.cmdval = true;
		p_vci_initiator.address = m_current->lock_address;
		p_vci_initiator.wdata = 0;
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        plen = 1;
		break;
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

    for ( size_t i = 0; i<m_n_from_coproc; ++i )
        p_from_coproc[i].r = m_from_coproc_state[i].fifo->wok();
    for ( size_t i = 0; i<m_n_to_coproc; ++i ) {
        p_to_coproc[i].w = m_to_coproc_state[i].fifo->rok();
        p_to_coproc[i].data = m_to_coproc_state[i].fifo->read();
    }
}

tmpl(/**/)::VciMwmrController(
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
    const bool use_llsc )
		   : caba::BaseModule(name),
		   m_vci_target_fsm(p_vci_target, mt.getSegmentList(tgtid)),
           m_ident(mt.indexForId(srcid)),
           m_fifo_to_coproc_depth(fifo_to_coproc_depth*sizeof(uint32_t)),
           m_fifo_from_coproc_depth(fifo_from_coproc_depth*sizeof(uint32_t)),
           m_plaps(plaps),
           m_n_to_coproc(n_to_coproc),
           m_n_from_coproc(n_from_coproc),
           m_n_all(n_to_coproc+n_from_coproc),
           m_n_config(n_config),
           m_n_status(n_status),
           m_use_llsc(use_llsc),
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

    for ( size_t i = 0; i<m_n_from_coproc; ++i ) {
        std::ostringstream o;
        o << "fifo_from_coproc[" << i << "]";
        m_from_coproc_state[i].way = MWMR_FROM_COPROC;
        m_from_coproc_state[i].fifo = new GenericFifo<uint32_t>(o.str(), m_fifo_from_coproc_depth/sizeof(uint32_t));
    }
    for ( size_t i = 0; i<m_n_to_coproc; ++i ) {
        std::ostringstream o;
        o << "fifo_to_coproc[" << i << "]";
        m_to_coproc_state[i].way = MWMR_TO_COPROC;
        m_to_coproc_state[i].fifo = new GenericFifo<uint32_t>(o.str(), m_fifo_to_coproc_depth/sizeof(uint32_t));
    }
}

tmpl(/**/)::~VciMwmrController()
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


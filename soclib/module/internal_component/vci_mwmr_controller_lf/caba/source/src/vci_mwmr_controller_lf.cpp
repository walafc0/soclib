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
#include "../include/vci_mwmr_controller_lf.h"
#include "mwmr_controller_lf.h"
#include "generic_fifo.h"
#include "alloc_elems.h"
#include "soclib_endian.h"
#include <algorithm>
#include <stddef.h>

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
	uint32_t width; // bytes
	uint32_t depth; // bytes
	uint32_t buffer_address;
	bool running;
	enum SoclibMwmrWay way;
	uint32_t timer;
    GenericFifo<uint32_t> *fifo;
    uint32_t in_words;
    uint32_t out_words;
    bool must_swap;

    void print( std::ostream &o ) const
    {
        o << "<data @" << std::hex << std::showbase << buffer_address
          << " status @" << status_address
          << " " << width << "x" << depth/width
          << ">";
    }

    friend std::ostream &operator <<(std::ostream &o, const struct fifo_state_s &s)
    {
        s.print(o);
        return o;
    }
};
}

#define tmpl(t) template<typename vci_param> t VciMwmrControllerLf<vci_param>

#define check_fifo() if ( ! m_config_fifo ) return false

typedef enum {
	INIT_IDLE,

    INIT_LL_SIZE_BEFORE,
    INIT_LL_SIZE_BEFORE_W,
    INIT_SC_SIZE_BEFORE,
    INIT_SC_SIZE_BEFORE_W,

    INIT_LL_TAIL,
    INIT_LL_TAIL_W,
    INIT_SC_TAIL,
    INIT_SC_TAIL_W,

    INIT_DATA_READ_1,
    INIT_DATA_READ_1_W,
    INIT_DATA_READ_2,
    INIT_DATA_READ_2_W,

    INIT_DATA_WRITE_1,
    INIT_DATA_WRITE_1_W,
    INIT_DATA_WRITE_2,
    INIT_DATA_WRITE_2_W,

    INIT_LL_HEAD,
    INIT_LL_HEAD_W,
    INIT_SC_HEAD,
    INIT_SC_HEAD_W,

    INIT_LL_AFTER_SIZE,
    INIT_LL_AFTER_SIZE_W,
    INIT_SC_AFTER_SIZE,
    INIT_SC_AFTER_SIZE_W,

	INIT_DONE,
} InitFsmState;

#if SOCLIB_MODULE_DEBUG
static const char *init_states[] = {
	"INIT_IDLE",

    "INIT_LL_SIZE_BEFORE",
    "INIT_LL_SIZE_BEFORE_W",
    "INIT_SC_SIZE_BEFORE",
    "INIT_SC_SIZE_BEFORE_W",

    "INIT_LL_TAIL",
    "INIT_LL_TAIL_W",
    "INIT_SC_TAIL",
    "INIT_SC_TAIL_W",

    "INIT_DATA_READ_1",
    "INIT_DATA_READ_1_W",
    "INIT_DATA_READ_2",
    "INIT_DATA_READ_2_W",

    "INIT_DATA_WRITE_1",
    "INIT_DATA_WRITE_1_W",
    "INIT_DATA_WRITE_2",
    "INIT_DATA_WRITE_2_W",

    "INIT_LL_HEAD",
    "INIT_LL_HEAD_W",
    "INIT_SC_HEAD",
    "INIT_SC_HEAD_W",

    "INIT_LL_AFTER_SIZE",
    "INIT_LL_AFTER_SIZE_W",
    "INIT_SC_AFTER_SIZE",
    "INIT_SC_AFTER_SIZE_W",

	"INIT_DONE",
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

tmpl(uint32_t)::swap_data( uint32_t data ) const
{
    if ( m_current && m_current->must_swap )
        return soclib::endian::uint32_swap(data);
    return data;
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
                continue;
            }
        } else {
            if ( st->fifo->filled_status()*sizeof(uint32_t) < st->width ) {
DEBUG_BEGIN;
                std::cout << "!D";
DEBUG_END;
                continue;
            }
        }

        m_current = st;
        m_last_elected = i;
        break;
	}
DEBUG_BEGIN;
    if ( m_current ) {
        std::cout << " new one: " << m_last_elected << " " << *m_current << std::endl;
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
    case MWMR_CONFIG_RUNNING:
		check_fifo();
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
    case MWMR_CONFIG_ENDIANNESS:
		check_fifo();
        assert( data == 0x11223344 || data == 0x44332211 );
        m_config_fifo->must_swap = (data == 0x44332211);
		return true;
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
    case MWMR_CONFIG_ENDIANNESS:
		check_fifo();
        data = m_config_fifo->must_swap ? 0x44332211 : 0x11223344;
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
	}
}

tmpl(void)::transition()
{
	if (!p_resetn) {
        r_pending_reset = false;
		m_vci_target_fsm.reset();
        r_init_fsm = INIT_IDLE;
		reset();
		return;
	}

	m_vci_target_fsm.transition();

	for ( size_t i=0; i<m_n_all; ++i )
		if ( m_all_state[i].timer )
			m_all_state[i].timer--;

#if SOCLIB_MODULE_DEBUG
    if ((InitFsmState)r_init_fsm.read() != INIT_IDLE) {
        std::cout << name() << " init: " << init_states[r_init_fsm.read()] << std::endl;
    }
#endif

	switch ((InitFsmState)r_init_fsm.read()) {
	case INIT_IDLE:
		if ( m_current )
			r_init_fsm = INIT_LL_SIZE_BEFORE;
		else {
            if ( r_pending_reset ) {
                reset();
                break;
            }
			elect();
            if ( m_current )
                m_n_elect++;
        }

    case INIT_LL_SIZE_BEFORE:
    case INIT_SC_SIZE_BEFORE:
    case INIT_LL_TAIL:
    case INIT_SC_TAIL:
    case INIT_DATA_READ_1:
    case INIT_DATA_READ_2:
    case INIT_LL_HEAD:
    case INIT_SC_HEAD:
    case INIT_LL_AFTER_SIZE:
    case INIT_SC_AFTER_SIZE:
        if ( p_vci_initiator.toPeerEnd() )
            r_init_fsm = r_init_fsm.read() + 1;
        break;

    case INIT_DATA_WRITE_1_W:
    case INIT_DATA_WRITE_2_W:
        if ( p_vci_initiator.iAccepted() && p_vci_initiator.reop.read() )
            r_init_fsm = r_init_fsm.read() + 1;
        break;

    case INIT_LL_SIZE_BEFORE_W:
    {
        if ( ! p_vci_initiator.iAccepted() )
            break;

            uint32_t size_before = swap_data(p_vci_initiator.rdata.read());
            uint32_t data_xfer_bytes = m_current->width * std::min<size_t>(
                (m_current->way == MWMR_TO_COPROC
                 ? (m_current->fifo->size() - m_current->fifo->filled_status())
                 : (m_current->fifo->filled_status()))
                / (m_current->width / vci_param::B),
                (size_before / m_current->width) );
#if SOCLIB_MODULE_DEBUG
        std::cout << name()
                  << " size before: " << size_before
                  << " xfer_bytes: " << data_xfer_bytes
                  << std::endl;
#endif
            if ( data_xfer_bytes == 0 ) {
                r_init_fsm = INIT_DONE;
                break;
            }
            r_size_before = size_before;
            r_data_xfer_bytes = data_xfer_bytes;
            r_init_fsm = r_init_fsm.read() + 1;
        break;
    }
    case INIT_SC_SIZE_BEFORE_W:
        if ( ! p_vci_initiator.iAccepted() )
            break;
        if ( p_vci_initiator.rdata.read() == vci_param::STORE_COND_ATOMIC ) {
            r_init_fsm = r_init_fsm.read() + 1;
        } else {
            r_init_fsm = INIT_LL_SIZE_BEFORE;
        }
        break;

    case INIT_LL_TAIL_W:
        if ( ! p_vci_initiator.iAccepted() )
            break;
        r_tail = swap_data(p_vci_initiator.rdata.read());
        r_init_fsm = r_init_fsm.read() + 1;
        break;

    case INIT_SC_TAIL_W:
        if ( ! p_vci_initiator.iAccepted() )
            break;
        if ( p_vci_initiator.rdata.read() == vci_param::STORE_COND_ATOMIC ) {
            // Now space between r_tail and r_tail+r_data_xfer_bytes is ours.
            // It may wrap and be half at end, half at start of buffer...
            if ( m_current->way == MWMR_FROM_COPROC ) {
                if ( r_tail + r_data_xfer_bytes > m_current->depth )
                    r_init_fsm = INIT_DATA_WRITE_1;
                else
                    r_init_fsm = INIT_DATA_WRITE_2;
            } else {
                if ( r_tail + r_data_xfer_bytes > m_current->depth )
                    r_init_fsm = INIT_DATA_READ_1;
                else
                    r_init_fsm = INIT_DATA_READ_2;
            }
            r_data_xfer_bytes_done = 0;
        } else {
            r_init_fsm = INIT_LL_TAIL;
        }
        break;

    case INIT_DATA_READ_1_W:
    {
        if ( ! p_vci_initiator.iAccepted() )
            break;
        uint32_t data_xfer_bytes_done = r_data_xfer_bytes_done + vci_param::B;
        assert( r_tail + data_xfer_bytes_done <= m_current->depth );
        r_data_xfer_bytes_done = data_xfer_bytes_done;
        if ( p_vci_initiator.reop.read() ) {
            assert( r_tail + data_xfer_bytes_done == m_current->depth );
            r_init_fsm = INIT_DATA_READ_2;
        }
        break;
    }

    case INIT_DATA_READ_2_W:
    {
        if ( ! p_vci_initiator.iAccepted() )
            break;
        uint32_t data_xfer_bytes_done = r_data_xfer_bytes_done + vci_param::B;
        assert( data_xfer_bytes_done <= r_data_xfer_bytes );
        r_data_xfer_bytes_done = data_xfer_bytes_done;
        if ( p_vci_initiator.reop.read() ) {
            assert( data_xfer_bytes_done == r_data_xfer_bytes );
            r_init_fsm = INIT_LL_HEAD;
        }
        break;
    }

    case INIT_DATA_WRITE_1:
    {
        if ( ! p_vci_initiator.peerAccepted() )
            break;
        uint32_t data_xfer_bytes_done = r_data_xfer_bytes_done + vci_param::B;
        assert( r_tail + data_xfer_bytes_done <= m_current->depth );
        r_data_xfer_bytes_done = data_xfer_bytes_done;
        if ( p_vci_initiator.toPeerEnd() )
            r_init_fsm = INIT_DATA_WRITE_1_W;
        break;
    }

    case INIT_DATA_WRITE_2:
    {
        if ( ! p_vci_initiator.peerAccepted() )
            break;
        uint32_t data_xfer_bytes_done = r_data_xfer_bytes_done + vci_param::B;
        assert( data_xfer_bytes_done <= r_data_xfer_bytes );
        r_data_xfer_bytes_done = data_xfer_bytes_done;
        if ( p_vci_initiator.toPeerEnd() )
            r_init_fsm = INIT_DATA_WRITE_2_W;
        break;
    }

    case INIT_LL_HEAD_W:
    {
        if ( ! p_vci_initiator.iAccepted() )
            break;
        uint32_t head = swap_data(p_vci_initiator.rdata.read());
        if ( head != r_tail )
            r_init_fsm = INIT_LL_HEAD;
        else
            r_init_fsm = INIT_SC_HEAD;
        break;
    }
    case INIT_SC_HEAD_W:
        if ( ! p_vci_initiator.iAccepted() )
            break;
        if ( p_vci_initiator.rdata.read() == vci_param::STORE_COND_ATOMIC ) {
            r_init_fsm = INIT_LL_AFTER_SIZE;
        } else {
            r_init_fsm = INIT_LL_HEAD;
        }
        break;

    case INIT_LL_AFTER_SIZE_W:
        if ( ! p_vci_initiator.iAccepted() )
            break;
        r_size_after = swap_data(p_vci_initiator.rdata.read());
        r_init_fsm = r_init_fsm.read() + 1;
        break;

    case INIT_SC_AFTER_SIZE_W:
        if ( ! p_vci_initiator.iAccepted() )
            break;
        if ( p_vci_initiator.rdata.read() == vci_param::STORE_COND_ATOMIC ) {
            r_init_fsm = INIT_DONE;
        } else {
            r_init_fsm = INIT_LL_AFTER_SIZE;
        }
        break;

	case INIT_DONE:
        r_init_fsm = INIT_IDLE;
        m_current->timer = m_plaps;
        m_current = NULL;
		break;
    }

    for ( size_t i = 0; i<m_n_from_coproc; ++i ) {
        fifo_state_t *st = &m_from_coproc_state[i];
        bool coproc_sent_data = p_from_coproc[i].r.read() && p_from_coproc[i].rok.read();
        bool vci_took_data = false;
        if ( st == m_current && (
                 r_init_fsm == INIT_DATA_WRITE_2 || r_init_fsm == INIT_DATA_WRITE_1 ) )
            vci_took_data = p_vci_initiator.peerAccepted();
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
        if ( st == m_current && (
                 r_init_fsm == INIT_DATA_READ_2_W || r_init_fsm == INIT_DATA_READ_1_W ) )
            vci_gave_data = p_vci_initiator.iAccepted();
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

#define status_offset(x) offsetof(soclib_mwmr_status_s, x)

    size_t plen = 4;
	switch ((InitFsmState)r_init_fsm.read()) {
	case INIT_IDLE:
    case INIT_LL_SIZE_BEFORE_W:
    case INIT_SC_SIZE_BEFORE_W:
    case INIT_LL_TAIL_W:
    case INIT_SC_TAIL_W:
    case INIT_DATA_READ_1_W:
    case INIT_DATA_READ_2_W:
    case INIT_DATA_WRITE_1_W:
    case INIT_DATA_WRITE_2_W:
    case INIT_LL_HEAD_W:
    case INIT_SC_HEAD_W:
    case INIT_SC_AFTER_SIZE_W:
    case INIT_LL_AFTER_SIZE_W:
	case INIT_DONE:
        p_vci_initiator.setVal(false);
        break;

    case INIT_LL_SIZE_BEFORE:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->status_address +
            ( m_current->way == MWMR_FROM_COPROC
              ? status_offset(free_size)
              : status_offset(data_size) );
		p_vci_initiator.wdata = 0;
		p_vci_initiator.cmd = vci_param::CMD_LOCKED_READ;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        break;

    case INIT_SC_SIZE_BEFORE:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->status_address +
            ( m_current->way == MWMR_FROM_COPROC
              ? status_offset(free_size)
              : status_offset(data_size) );
		p_vci_initiator.wdata = swap_data(r_size_before - r_data_xfer_bytes);
		p_vci_initiator.cmd = vci_param::CMD_STORE_COND;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        break;

    case INIT_LL_TAIL:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->status_address +
            ( m_current->way == MWMR_FROM_COPROC
              ? status_offset(free_tail)
              : status_offset(data_tail) );
		p_vci_initiator.wdata = 0;
		p_vci_initiator.cmd = vci_param::CMD_LOCKED_READ;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        break;

    case INIT_SC_TAIL:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->status_address +
            ( m_current->way == MWMR_FROM_COPROC
              ? status_offset(free_tail)
              : status_offset(data_tail) );
		p_vci_initiator.wdata = swap_data(( r_tail + r_data_xfer_bytes ) % m_current->depth);
		p_vci_initiator.cmd = vci_param::CMD_STORE_COND;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        break;

    case INIT_DATA_READ_1:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->buffer_address + r_tail;
		p_vci_initiator.wdata = 0;
		p_vci_initiator.cmd = vci_param::CMD_READ;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        plen = m_current->depth - r_tail;;
        break;
        
    case INIT_DATA_READ_2:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->buffer_address +
            ( ( ( r_tail + r_data_xfer_bytes ) > m_current->depth )
              ? 0
              : r_tail.read() );
		p_vci_initiator.wdata = 0;
		p_vci_initiator.cmd = vci_param::CMD_READ;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        plen = ( ( ( r_tail + r_data_xfer_bytes ) > m_current->depth )
                 ? r_data_xfer_bytes - (m_current->depth - r_tail)
                 : r_data_xfer_bytes
            );
        break;

    case INIT_DATA_WRITE_1:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->buffer_address +
            r_tail + r_data_xfer_bytes_done;
		p_vci_initiator.wdata = m_current->fifo->read();
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = (
            (r_tail + r_data_xfer_bytes_done + vci_param::B)
            == m_current->depth);
        plen = m_current->depth - r_tail;;
        break;
        
    case INIT_DATA_WRITE_2:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->buffer_address +
            ( ( r_tail + r_data_xfer_bytes_done ) % m_current->depth );
		p_vci_initiator.wdata = m_current->fifo->read();
		p_vci_initiator.cmd = vci_param::CMD_WRITE;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = (
            r_data_xfer_bytes_done.read()+vci_param::B
            == r_data_xfer_bytes);
        plen = ( ( ( r_tail + r_data_xfer_bytes ) > m_current->depth )
                 ? r_data_xfer_bytes - (m_current->depth - r_tail)
                 : r_data_xfer_bytes
            );
        break;

    case INIT_LL_HEAD:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->status_address +
            ( m_current->way == MWMR_FROM_COPROC
              ? status_offset(data_head)
              : status_offset(free_head) );
		p_vci_initiator.wdata = 0;
		p_vci_initiator.cmd = vci_param::CMD_LOCKED_READ;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        break;

    case INIT_SC_HEAD:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->status_address +
            ( m_current->way == MWMR_FROM_COPROC
              ? status_offset(data_head)
              : status_offset(free_head) );
		p_vci_initiator.wdata = swap_data(( r_tail + r_data_xfer_bytes ) % m_current->depth);
		p_vci_initiator.cmd = vci_param::CMD_STORE_COND;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        break;

    case INIT_LL_AFTER_SIZE:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->status_address +
            ( m_current->way == MWMR_FROM_COPROC
              ? status_offset(data_size)
              : status_offset(free_size) );
		p_vci_initiator.wdata = 0;
		p_vci_initiator.cmd = vci_param::CMD_LOCKED_READ;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        break;

    case INIT_SC_AFTER_SIZE:
        p_vci_initiator.cmdval = true;
        p_vci_initiator.address = m_current->status_address +
            ( m_current->way == MWMR_FROM_COPROC
              ? status_offset(data_size)
              : status_offset(free_size) );
		p_vci_initiator.wdata = swap_data(r_size_after + r_data_xfer_bytes);
		p_vci_initiator.cmd = vci_param::CMD_STORE_COND;
		p_vci_initiator.be = 0xf;
		p_vci_initiator.eop = true;
        break;
	}
	p_vci_initiator.contig = true;
	p_vci_initiator.cons = 1;
	p_vci_initiator.plen = plen;
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

tmpl(/**/)::VciMwmrControllerLf(
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
           m_all_state(new fifo_state_t[m_n_all]),
           m_to_coproc_state(m_all_state),
           m_from_coproc_state(&m_all_state[m_n_to_coproc]),
           r_config(alloc_elems<sc_signal<uint32_t> >("r_config", n_config)),
           r_pending_reset("pending_reset"),
           r_init_fsm("init_fsm"),
           r_size_before("size_before"),
           r_size_after("size_after"),
           r_tail("tail"),
           r_data_xfer_bytes("bytes"),
           r_data_xfer_bytes_done("bytes_done"),
           m_n_elect(0),
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

    memset(m_all_state, 0, sizeof(*m_all_state)*m_n_all);

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

tmpl(/**/)::~VciMwmrControllerLf()
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


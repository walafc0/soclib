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
 * Copyright (c) Telecom ParisTech
 *         Alexandre Becoulet <alexandre.becoulet@enst.fr>, 2012
 *
 * Maintainers: becoulet
 */

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <iostream>

#include "register.h"
#include "../include/vci_ethernet.h"
#include "ethernet.h"

#define CHUNCK_SIZE (1<<(vci_param::K-1))

//#define SOCLIB_MODULE_DEBUG

namespace soclib { namespace caba {

#define tmpl(t) template<typename vci_param> t VciEthernet<vci_param>

tmpl(bool)::on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be)
{
    int cell = (int)addr / vci_param::B;

#ifdef SOCLIB_MODULE_DEBUG
    std::cout 
        << name()
        << ": write config register "
        << cell
        << " with data 0x"
        << std::hex << data
        << std::endl;
#endif

    fifo_entry_t *f;

	switch ((enum SoclibEthernetRegisters)cell)
        {
        case ETHERNET_TX_SIZE:
            _tx_size = std::min((int)data, VCI_ETHERNET_MAX_PKT_SIZE);
            return true;

        case ETHERNET_TX_FIFO:
            if (_tx_count >= VCI_ETHERNET_FIFO_SIZE) {
                std::cout << name() << ": TX fifo full, can not push more packets!" << std::endl;
                return false;
            }

            f = _tx_fifo + (_tx_start + _tx_count++) % VCI_ETHERNET_FIFO_SIZE;
            _tx_waiting++;

            f->addr = data;
            f->size = _tx_size;
            f->status = 0;
            f->data = (uint8_t*)malloc(_tx_size);
            return true;

        case ETHERNET_RX_SIZE:
            _rx_size = std::min((int)data, VCI_ETHERNET_MAX_PKT_SIZE);
            return true;

        case ETHERNET_RX_FIFO:
            if (_rx_count >= VCI_ETHERNET_FIFO_SIZE) {
                std::cout << name() << ": RX fifo full, can not push more packet buffers!" << std::endl;
                return false;
            }

            f = _rx_fifo + (_rx_start + _rx_count++) % VCI_ETHERNET_FIFO_SIZE;
            _rx_free++;

            f->addr = data;
            f->size = _rx_size;
            f->status = 0;
            f->data = (uint8_t*)malloc(_rx_size);
            return true;

        case ETHERNET_CTRL:
            if (data & ETHERNET_CTRL_RESET)
                _soft_reset = true;
            if (data & ETHERNET_CTRL_TX_IRQ)
                _tx_irq_en = true;
            if (data & ETHERNET_CTRL_RX_IRQ)
                _rx_irq_en = true;
            if (data & ETHERNET_CTRL_LINK_IRQ)
                _link_irq_en = true;
            return true;

        default:
            return false;
	}
}

tmpl(bool)::on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data)
{
    int cell = (int)addr / vci_param::B;

#ifdef SOCLIB_MODULE_DEBUG
    std::cout 
        << name()
        << ": read config register "
        << cell
        << std::endl;
#endif

    fifo_entry_t *f;

	switch ((enum SoclibEthernetRegisters)cell) {
    case ETHERNET_TX_FIFO:
        if (_tx_done == 0) {
            data = 0;
        } else {
            f = _tx_fifo + _tx_start++ % VCI_ETHERNET_FIFO_SIZE;
            _tx_count--;
            _tx_done--;
            data = f->status;
            free(f->data);
        }
        return true;

    case ETHERNET_RX_SIZE:
        if (_rx_done == 0) {
            data = 0;
        } else {
            f = _rx_fifo + _rx_start % VCI_ETHERNET_FIFO_SIZE;
            data = f->size;
        }
        return true;

    case ETHERNET_RX_FIFO:
        if (_rx_done == 0) {
            data = 0;
        } else {
            f = _rx_fifo + _rx_start++ % VCI_ETHERNET_FIFO_SIZE;
            _rx_count--;
            _rx_done--;
            free(f->data);
            data = f->status;
        }
        return true;

    case ETHERNET_STATUS:
        data = 0;
        _link_changed = false;
        if (_link_up)
            data |= ETHERNET_ST_LINK_UP;
        if (_rx_done > 0)
            data |= ETHERNET_ST_RX_DONE;
        if (_tx_done > 0)
            data |= ETHERNET_ST_TX_DONE;
        return true;

    case ETHERNET_FIFO_SIZE:
        data = VCI_ETHERNET_FIFO_SIZE;
        return true;

    case ETHERNET_MAC_LOW:
        data = _mac[0] | (_mac[1] << 8) | (_mac[2] << 16) | (_mac[3] << 24);
        return true;

    case ETHERNET_MAC_HIGH:
        data = _mac[4] | (_mac[5] << 8);
        return true;

    default:
        return false;
	}
}

tmpl(void)::write_finish( req_t *req )
{
    fifo_entry_t *f = _rx_fifo + (_rx_start + _rx_count - _rx_free - 1) % VCI_ETHERNET_FIFO_SIZE;

    assert(req == f->rd_req);
    delete req;

    if (req->failed()) {
        std::cout << name() << ": RX dma write error for packet @" << f->addr << std::endl;
        f->status = ETHERNET_RX_DMA_ERR;
        _rx_done++;
        _dma_busy = false;

    } else if ((unsigned)f->dma_offset >= f->size) {
#ifdef SOCLIB_MODULE_DEBUG
        std::cout << name() << ": RX dma done for packet @" << f->addr << std::endl;
#endif
        f->status = ETHERNET_RX_DONE;
        _rx_done++;
        _dma_busy = false;

    } else {
        int chunck_size = std::min((unsigned)CHUNCK_SIZE, f->size - f->dma_offset);
        f->wr_req = new VciInitSimpleWriteReq<vci_param>(f->addr + f->dma_offset, f->data + f->dma_offset, chunck_size);
        f->dma_offset += chunck_size;
        f->wr_req->setDone(this, ON_T(write_finish));
        m_vci_init_fsm.doReq(f->wr_req);
    }
}

tmpl(void)::read_finish( req_t *req )
{
    fifo_entry_t *f = _tx_fifo + (_tx_start + _tx_count - _tx_waiting - 1) % VCI_ETHERNET_FIFO_SIZE;

    assert(req == f->rd_req);
    delete req;

    if (req->failed()) {
        std::cout << name() << ": TX dma read error for packet @" << f->addr << std::endl;
        f->status = ETHERNET_TX_DMA_ERR;
        _tx_done++;
        _dma_busy = false;

    } else if ((unsigned)f->dma_offset >= f->size) {
#ifdef SOCLIB_MODULE_DEBUG
        std::cout << name() << ": TX dma done for packet @" << f->addr << std::endl;
#endif
        f->status = ETHERNET_TX_DONE;

        if (!_link_up || ::write(_fd, f->data, f->size) != f->size) {
            std::cout << name() << ": TX tap write error for packet @" << f->addr << std::endl;
            f->status = ETHERNET_TX_PHY_ERR;
            _link_check_counter = 1;
        }

        _tx_done++;
        _dma_busy = false;

    } else {
        int chunck_size = std::min((unsigned)CHUNCK_SIZE, f->size - f->dma_offset);
        f->rd_req = new VciInitSimpleReadReq<vci_param>(f->data + f->dma_offset, f->addr + f->dma_offset, chunck_size);
        f->dma_offset += chunck_size;
        f->rd_req->setDone(this, ON_T(read_finish));
        m_vci_init_fsm.doReq(f->rd_req);
    }
}

tmpl(void)::cleanup_fifos()
{
    for (int i = 0; i < _rx_count; i++) {
        fifo_entry_t *f = _rx_fifo + (_rx_start + i) % VCI_ETHERNET_FIFO_SIZE;
        free(f->data);
    }

    for (int i = 0; i < _tx_count; i++) {
        fifo_entry_t *f = _tx_fifo + (_tx_start + i) % VCI_ETHERNET_FIFO_SIZE;
        free(f->data);
    }
}

tmpl(void)::transition()
{
	if (!p_resetn || _soft_reset) {
		m_vci_target_fsm.reset();
		m_vci_init_fsm.reset();
        _soft_reset = false;

        cleanup_fifos();

        _rx_start = _rx_free = _rx_done = _rx_count = 0;
        _tx_start = _tx_waiting = _tx_done = _tx_count = 0;
        _rx_irq_en = _tx_irq_en = _link_irq_en = false;        
        _link_check_counter = 1;
        _link_changed = _link_up = false;
        _dma_busy = false;
		return;
	}

    if (_fd >= 0 && --_link_check_counter <= 0) {
        _link_check_counter = 100000;

        int sock = socket(AF_INET, SOCK_DGRAM, 0);

        if (sock >= 0) {
            if (ioctl(sock, SIOCGIFFLAGS, &_tap_ifr) < 0) {
                std::cout << name() << ": link status check error: " << _tap_ifr.ifr_flags << std::endl;
            } else if (_link_up != !!(_tap_ifr.ifr_flags & IFF_UP)) {
                _link_up = !_link_up;
                _link_changed = true;
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << name() << ": link status changed to: " << (_link_up ? "up" : "down") << std::endl;
#endif
            }
            close(sock);
        }
    }

    if (!_dma_busy) {

        if (_tx_waiting > 0) {

            fifo_entry_t *f = _tx_fifo + (_tx_start + _tx_count - _tx_waiting--) % VCI_ETHERNET_FIFO_SIZE;

            // start DMA read
            int chunck_size = std::min((unsigned)CHUNCK_SIZE, f->size);
            f->rd_req = new VciInitSimpleReadReq<vci_param>(f->data, f->addr, chunck_size);
            f->dma_offset = chunck_size;
            f->rd_req->setDone(this, ON_T(read_finish));
            m_vci_init_fsm.doReq(f->rd_req);
            _dma_busy = true;

#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << ": started TX dma read @" << f->addr << ", " << f->size << " bytes " << std::endl;
#endif
        } else if (_rx_free > 0 && _link_up) {

            fifo_entry_t *f = _rx_fifo + (_rx_start + _rx_count - _rx_free) % VCI_ETHERNET_FIFO_SIZE;
            int rd = ::read(_fd, f->data, f->size);

            if (rd < 0 && errno != EAGAIN) {
                f->status = ETHERNET_RX_PHY_ERR;
                _rx_done++;
                std::cout << name() << ": tap read error on RX" << std::endl;
                _link_check_counter = 1;

            } else if (rd > 0) {
                f->size = rd;
                _rx_free--;

                // start DMA write
                int chunck_size = std::min((unsigned)CHUNCK_SIZE, f->size);
                f->wr_req = new VciInitSimpleWriteReq<vci_param>(f->addr, f->data, chunck_size);
                f->dma_offset = chunck_size;
                f->wr_req->setDone(this, ON_T(write_finish));
                m_vci_init_fsm.doReq(f->wr_req);
                _dma_busy = true;

#ifdef SOCLIB_MODULE_DEBUG
                std::cout << name() << ": started RX dma write @" << f->addr << ", " << f->size << " bytes " << std::endl;
#endif
            }
        }
    }

	m_vci_target_fsm.transition();
	m_vci_init_fsm.transition();
}

tmpl(void)::genMoore()
{
	m_vci_target_fsm.genMoore();
	m_vci_init_fsm.genMoore();

	p_irq = (_rx_irq_en && _rx_done > 0) || (_tx_irq_en && _tx_done > 0) || (_link_changed && _link_irq_en);
}

tmpl(/**/)::VciEthernet(sc_module_name name, const MappingTable &mt,
                        const IntTab &srcid, const IntTab &tgtid,
                        const std::string &if_name)
	: caba::BaseModule(name),
	  m_vci_target_fsm(p_vci_target, mt.getSegmentList(tgtid)),
	  m_vci_init_fsm(p_vci_initiator, mt.indexForId(srcid)),
      p_clk("clk"),
      p_resetn("resetn"),
      p_vci_target("vci_target"),
      p_vci_initiator("vci_initiator"),
      p_irq("irq")
{
	m_vci_target_fsm.on_read_write(on_read, on_write);

    _fd = open("/dev/net/tun", O_RDWR);

    if ( _fd < 0 ) {
        std::cerr << name << ": Unable to open /dev/net/tun" << std::endl;
    } else {
        int flags = fcntl(_fd, F_GETFL, 0);
        fcntl(_fd, F_SETFL, flags | O_NONBLOCK);

        memset((void*)&_tap_ifr, 0, sizeof(_tap_ifr));
        _tap_ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
        strncpy(_tap_ifr.ifr_name, if_name.c_str(), IFNAMSIZ);

        if (ioctl(_fd, TUNSETIFF, (void *) &_tap_ifr) < 0) {
            close(_fd);
            _fd = -1;
            std::cerr << name << ": Unable to setup tap interface, check privileges."
#ifdef __linux__
                      << " (try: sudo setcap cap_net_admin=eip ./system.x)"
#endif
                      << std::endl;
        }

        // ioctl(_fd, SIOCGIFHWADDR, &_tap_ifr);
        // memcpy(_mac, _tap_ifr.ifr_hwaddr.sa_data, 6);
        srand(time(0) * getpid());
        _mac[0] = 0x00;
        _mac[1] = 0x16;
        _mac[2] = 0x3e;
        _mac[3] = rand();
        _mac[4] = rand();
        _mac[5] = rand();
    }

    _link_check_counter = 1;
    _rx_start = _rx_free = _rx_done = _rx_count = 0;
    _tx_start = _tx_waiting = _tx_done = _tx_count = 0;
    _dma_busy = false;
    _link_up = false;

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

tmpl(/**/)::~VciEthernet()
{
    cleanup_fifos();

    if (_fd >= 0)
        close(_fd);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


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
 *
 * Maintainers: nipo
 */

#ifndef SOCLIB_VCI_ETHERNET_H
#define SOCLIB_VCI_ETHERNET_H

#include <stdint.h>
#include <systemc>

#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>

#include "vci_target_fsm.h"
#include "vci_initiator_fsm.h"
#include "caba_base_module.h"
#include "mapping_table.h"

#define VCI_ETHERNET_FIFO_SIZE 8
#define VCI_ETHERNET_MAX_PKT_SIZE 1500

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciEthernet
	: public caba::BaseModule
{
private:
    soclib::caba::VciTargetFsm<vci_param, true> m_vci_target_fsm;
    soclib::caba::VciInitiatorFsm<vci_param> m_vci_init_fsm;
    typedef typename soclib::caba::VciInitiatorReq<vci_param> req_t;

    bool on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be);
    bool on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data);
    void read_finish( req_t *req );
    void write_finish( req_t *req );
    void transition();
    void genMoore();
    void cleanup_fifos();

    struct fifo_entry_t
    {
        uint32_t addr;
        uint32_t size;
        uint32_t status;
        uint8_t *data;
        int dma_offset;
        union {
            VciInitSimpleReadReq<vci_param> *rd_req;
            VciInitSimpleWriteReq<vci_param> *wr_req;
        };
    };

    int _rx_start;    //< index of first available rx buffer
    int _rx_free;     //< number of yet unused rx buffers
    int _rx_done;     //< number of rx buffers ready to pop
    int _rx_count;    //< total number of rx buffers

    int _tx_start;    //< index of first tx buffer
    int _tx_waiting;  //< number of yet unprocessed tx buffers
    int _tx_done;     //< number of tx buffers ready to pop
    int _tx_count;    //< total number of tx buffers

    int _link_check_counter;

    bool _dma_busy;
    bool _link_up;
    bool _link_changed;

    fifo_entry_t _tx_fifo[VCI_ETHERNET_FIFO_SIZE];
    fifo_entry_t _rx_fifo[VCI_ETHERNET_FIFO_SIZE];

    uint32_t _tx_size; //< next tx buffer size
    uint32_t _rx_size; //< next rx buffer size

    bool _rx_irq_en;
    bool _tx_irq_en;
    bool _link_irq_en;
    bool _soft_reset;

	int _fd;
    uint8_t _mac[6];

    struct ifreq _tap_ifr;

	inline void ended(int status);

protected:
    SC_HAS_PROCESS(VciEthernet);

public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    soclib::caba::VciTarget<vci_param> p_vci_target;
    soclib::caba::VciInitiator<vci_param> p_vci_initiator;
    sc_out<bool> p_irq;

	VciEthernet(sc_module_name name, const soclib::common::MappingTable &mt,
                const soclib::common::IntTab &srcid, const soclib::common::IntTab &tgtid,
                const std::string &if_name = "soclib0");

	~VciEthernet();
};

}}

#endif /* SOCLIB_VCI_ETHERNET_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


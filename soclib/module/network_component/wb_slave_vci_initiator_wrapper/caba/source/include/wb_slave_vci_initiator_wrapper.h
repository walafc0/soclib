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
 * Copyright (c) TelecomParisTECH
 *         Tarik Graba <tarik.graba@telecom-paristech.fr>, 2009
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 *
 * $Id$
 *
 */

#ifndef VCI_INITIATOR_WB_SLAVE_WRAPPER_H_
#define VCI_INITIATOR_WB_SLAVE_WRAPPER_H_

// This module can be used to wrap a wishbone master component

#include <systemc>
#include <queue>
#include "caba_base_module.h"
#include "wb_slave.h"
#include "vci_initiator.h"
#include "mapping_table.h"


namespace soclib { namespace caba {

    template<typename vci_param, typename wb_param>
        class WbSlaveVciInitiatorWrapper
        :public soclib::caba::BaseModule
        {
            // mandatory SystemC construct
            protected:
                SC_HAS_PROCESS(WbSlaveVciInitiatorWrapper);

            public:
                // ports
                sc_core::sc_in<bool>               p_clk;
                sc_core::sc_in<bool>               p_resetn;

                soclib::caba::VciInitiator<vci_param>    p_vci;
                WbSlave <wb_param>                       p_wb;

                // constructor to use with vgmn
                WbSlaveVciInitiatorWrapper (sc_core::sc_module_name  insname,
                        const soclib::common::MappingTable &mt,
                        const soclib::common::IntTab &index,
                        const bool big_endian = true);

                // constructor when vci network is not important
                WbSlaveVciInitiatorWrapper (sc_core::sc_module_name  insname,
                        const bool big_endian = true);

            private:

                static const char *const STATE_NAME[] ;
                sc_core::sc_signal<int>                  state;

                void transition();
                void genMealy();

                // initiator ID
                const uint32_t m_srcid;

                // for read requests
                bool read_cmd_not_accepted;
                // this is a fifo to hold the requests
                // true for write and false for read
                std::queue<bool> w_response;

                // for endianess

                const bool big_endian;

                inline uint32_t swap_bytes(uint32_t in){
                    return (
                            ( (in & 0xff)   << 24 ) |
                            ( (in & 0xff00) <<  8 ) |
                            ( (in >>  8) & 0xff00 ) |
                            ( (in >> 24) &   0xff )
                           );
                }

                inline uint32_t swap_bits(uint32_t in){
                    uint32_t tmp = 0;
                    for (int i = 0; i<4; i++)
                        tmp |= ((in >>(3-i)) & 0x1)<<i;
                    return tmp;
                }

        };
}}

#endif //VCI_INITIATOR_WB_SLAVE_WRAPPER_H_


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

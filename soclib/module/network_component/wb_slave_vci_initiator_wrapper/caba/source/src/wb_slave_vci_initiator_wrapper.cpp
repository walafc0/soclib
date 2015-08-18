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
 * Based on sparcv8 and mips32 code
 *         Alexis Polti <polti@telecom-paristech.fr>, 2008
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *         Alain Greiner <alain.greiner@lip6.fr>, 2007
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 *
 * $Id$
 *
 */

#include <cassert>
#include "wb_slave_vci_initiator_wrapper.h"


namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, typename wb_param> x \
    WbSlaveVciInitiatorWrapper<vci_param, wb_param>


    // constructor to use with vgmn
    tmpl(/**/)::WbSlaveVciInitiatorWrapper (sc_core::sc_module_name insname,
            const soclib::common::MappingTable &mt,
            const soclib::common::IntTab &index,
            const bool big_endian)
        : soclib::caba::BaseModule(insname),
        p_clk("p_clk"), p_resetn("p_resetn"),
        state("state"),
        m_srcid(mt.indexForId(index)),
        big_endian(big_endian)
    {
        //Test vci/wb parameters compatibility
        assert(vci_param::B*8 == wb_param::DataWidth && "VCI and WB data widths do not match!!");
        assert(vci_param::N   == wb_param::AddWidth  && "VCI and WB addr widths do not match!!");
        assert((wb_param::DataWidth == 32 || !big_endian) && "Big endian switch only for 32bits..!!");
#ifdef SOCLIB_MODULE_DEBUG
        std::cout << name() << " "
            << "correctly initialized "
            << " with srcid = " << index
            << " big endian " <<  big_endian
            << std::endl;
#endif

        SC_METHOD (transition);
        dont_initialize();
        sensitive << p_clk.pos();

        SC_METHOD (genMealy);
        dont_initialize();
        sensitive << p_wb;
        sensitive << p_vci;

    }

    // constructor when vci network is not important
    tmpl(/**/)::WbSlaveVciInitiatorWrapper (sc_core::sc_module_name insname,
            const bool big_endian)
        : soclib::caba::BaseModule(insname),
        p_clk("p_clk"), p_resetn("p_resetn"),
        state("state"),
        m_srcid(0),
        big_endian(big_endian)
    {
        //Test vci/wb parameters compatibility
        assert(vci_param::B*8 == wb_param::DataWidth && "VCI and WB data widths do not match!!");
        assert(vci_param::N   == wb_param::AddWidth  && "VCI and WB addr widths do not match!!");
        assert((wb_param::DataWidth == 32 || !big_endian) && "Big endian switch only for 32bits..!!");
#ifdef SOCLIB_MODULE_DEBUG
        std::cout << name() << " "
            << "correctly initialized "
            << " big endian " <<  big_endian
            << std::endl;
#endif

        SC_METHOD (transition);
        dont_initialize();
        sensitive << p_clk.pos();

        SC_METHOD (genMealy);
        dont_initialize();
        sensitive << p_wb;
        sensitive << p_vci;

    }

    // Transition
    tmpl(void)::transition()
    {
        if (p_resetn == false) {
            //state = IDLE;
#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " "
                << "RESET nothing to do!"
                << std::endl;
#endif
            read_cmd_not_accepted = true;
            return;
        }

        if (p_wb.STB_I){ // STB signal enables the transaction
            if (!p_vci.cmdack && read_cmd_not_accepted ) {
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << name() << " Received WB request and vci not ready" << std::endl;
#endif
            }
            if (p_vci.cmdack && read_cmd_not_accepted) {
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << name() << " Received WB request and vci ready" << std::endl;
#endif
            }
            if (!p_wb.WE_I && p_vci.cmdack && read_cmd_not_accepted) {
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << name() << " VCI accepts read request" << std::endl;
#endif
                w_response.push(false);
                read_cmd_not_accepted = false;
            }
            if (p_wb.WE_I && p_vci.cmdack) {
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << name() << " VCI accepts write request" << std::endl;
#endif
                w_response.push(true);
            }
        }
        if (p_vci.rspval  && (p_vci.rsrcid.read() == m_srcid) ) {
#ifdef SOCLIB_MODULE_DEBUG
            std::cout << name() << " VCI resp valid" ;
#endif
            if(!w_response.front())//read request
            {
                read_cmd_not_accepted = true;
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << " for a read request "<< std::endl;
#endif
            }
            if(!w_response.empty())
            {
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << " for a write request "<< std::endl;
#endif
                w_response.pop();
            }
        }
#ifdef SOCLIB_MODULE_DEBUG
        //std::cout << name() << p_wb << std::endl;
#endif
    }

    // genMealy
    tmpl(void)::genMealy()
    {
        bool tmp_ack;

        // VCI Side
        p_vci.rspack    =   true;                 // always acknowledge responces
        p_vci.cmdval    =   (p_wb.WE_I.read())? p_wb.CYC_I && p_wb.STB_I: p_wb.CYC_I && p_wb.STB_I && read_cmd_not_accepted;
        p_vci.cmd       =   (p_wb.WE_I.read())? vci_param::CMD_WRITE
            : vci_param::CMD_READ;
        p_vci.address   =   p_wb.ADR_I.read();     // always transfer requests
        p_vci.wdata     =   (big_endian)? swap_bytes(p_wb.DAT_I.read())
            :(uint32_t)p_wb.DAT_I.read();          // always transfer requests
        p_vci.be        =   (big_endian)? swap_bits(p_wb.SEL_I.read())
            :(uint32_t)p_wb.SEL_I.read();          // always transfer requests
        p_vci.eop       =   p_wb.CYC_I && p_wb.STB_I;
        p_vci.plen      =    vci_param::B;         // always one cell
        p_vci.contig    =   true;                  // No paket addressing modes
        p_vci.wrap      =   false;                 // No paket addressing modes
        p_vci.cons      =   false;                 // No paket addressing modes
        p_vci.clen      =   0;                     // No chained transactions
        p_vci.cfixed    =   false;                 // No chained transactions
        p_vci.srcid     =   m_srcid;
        p_vci.trdid     =   0;                     // No threading
        p_vci.pktid     =   0;                     // No threading

        // WB Side
        p_wb.DAT_O      =   (big_endian)? swap_bytes(p_vci.rdata.read())
            :(uint32_t)p_vci.rdata.read();         // always transfer requests
        tmp_ack =   (p_wb.WE_I.read())? p_vci.cmdack
            : ( p_vci.rspval.read() && (p_vci.rsrcid.read() == m_srcid) && (!w_response.front()) );
        // wb specification sets that ack should be deaserted if there is no
        // valid strobe
        p_wb.ACK_O      = p_wb.CYC_I && p_wb.STB_I && tmp_ack;
        p_wb.RTY_O      =   false;                 // No retry
        // we will miss write errrors as we dont wait for vci responce
        p_wb.ERR_O      =   (p_vci.rerror.read() != vci_param::ERR_NORMAL) 
                                && tmp_ack;

    }


#undef tmpl

}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

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
 * Copyright (c) UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_CABA_VCI_LOGGER_H
#define SOCLIB_CABA_VCI_LOGGER_H

#include <systemc>
#include "vci_monitor.h"
#include "vci_buffers.h"
#include "caba_base_module.h"
#include "mapping_table.h"

namespace soclib {
namespace caba {

template<typename vci_param>
class VciLoggerElem
{
    typedef std::vector<VciCmdBuffer<vci_param> > cmd_list_t;
    typedef std::vector<VciRspBuffer<vci_param> > rsp_list_t;
    cmd_list_t m_cmd_packets;
    rsp_list_t m_rsp_packets;
    bool m_cmd_ended;
    bool m_rsp_ended;

public:
    VciLoggerElem();

    void takeRsp( const VciMonitor<vci_param> &port);
    void takeCmd( const VciMonitor<vci_param> &port);
    void print(std::ostream &o) const;
    void reset();

    friend std::ostream &operator <<(std::ostream &o, const VciLoggerElem<vci_param> &c)
    {
        c.print(o);
        return o;
    }

    typename vci_param::fast_addr_t address() const
    {
        return m_cmd_packets[0].address;
    }

    typename vci_param::cmd_t command() const
    {
        return m_cmd_packets[0].cmd;
    }

    typename vci_param::cmd_t plen() const
    {
        return m_cmd_packets[0].plen;
    }

    std::vector<typename vci_param::fast_data_t> rdata() const
    {
        std::vector<typename vci_param::fast_data_t> ret;

        for ( typename rsp_list_t::const_iterator i = m_rsp_packets.begin();
              i != m_rsp_packets.end();
              ++i ) {
            ret.push_back(i->rdata);
        }
        return ret;
    }

    std::vector<typename vci_param::fast_data_t> wdata() const
    {
        std::vector<typename vci_param::fast_data_t> ret;

        for ( typename cmd_list_t::const_iterator i = m_cmd_packets.begin();
              i != m_cmd_packets.end();
              ++i ) {
            ret.push_back(i->wdata);
        }
        return ret;
    }

    typename vci_param::srcid_t srcid() const
    {
        return m_cmd_packets[0].srcid;
    }
};

template<typename vci_param>
class VciLogger
	: public soclib::caba::BaseModule
{
	VciLoggerElem<vci_param> *m_pending_commands;

protected:
	SC_HAS_PROCESS(VciLogger);
    virtual void handle_txn(const VciLoggerElem<vci_param> &elem);

public:
    sc_core::sc_in<bool> p_resetn;
    sc_core::sc_in<bool> p_clk;
    soclib::caba::VciMonitor<vci_param> p_vci;

    VciLogger(
        sc_core::sc_module_name insname,
        const soclib::common::MappingTable &mt);
    ~VciLogger();

private:
    void transition();
};

}}

#endif /* SOCLIB_CABA_VCI_LOGGER_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


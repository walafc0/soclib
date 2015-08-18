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
 *         Nicolas Pouillon <nipo@ssji.net>, 2010
 *
 * Maintainers: nipo
 */

#include "vci_mwmr_stats.h"
#include <cstring>
#include "vci_buffers.h"
#include <fstream>
#include <iomanip>
#include <cassert>
#include <vector>

namespace soclib {
namespace caba {

#define tmpl(...) template<typename vci_param> __VA_ARGS__ VciMwmrStats<vci_param>

tmpl(const char *)::field_sep = ",";

tmpl()::VciMwmrStats(
	sc_core::sc_module_name insname,
	const soclib::common::MappingTable &mt,
    const soclib::common::Loader &loader,
    const char *log_name,
    const std::vector<std::string> &fifo_name )
       : soclib::caba::VciLogger<vci_param>(insname, mt),
       m_cycle(0),
       m_log(log_name)
{
	SC_METHOD(transition);
	this->dont_initialize();
	this->sensitive << this->p_clk.pos();

    m_log.exceptions( std::ofstream::failbit | std::ofstream::badbit );
    m_log.close();
    m_log.open(log_name, std::ios_base::out | std::ios_base::app);

    if (m_log.fail()) {
        std::cout << m_log.failbit << std::endl;
        throw soclib::exception::RunTimeError("Bad file");
    }

    for ( typename std::vector<std::string>::const_iterator i = fifo_name.begin();
          i != fifo_name.end();
          ++i ) {
        const std::string name = (*i)+"_status";
        const soclib::common::BinaryFileSymbol *sym = loader.get_symbol_by_name(name);
        if ( ! sym || sym->name() != name ) {
            std::cerr << this->name() << " name not found: " << name << std::endl;
            continue;
        }
        typename vci_param::fast_addr_t addr = sym->address();
        m_mwmr_info.push_back( mwmr_info_t(addr, *i) );
    }
}

tmpl()::~VciMwmrStats()
{
}

tmpl(std::ostream &)::prefix(const VciLoggerElem<vci_param> &elem, const mwmr_info_t &mwmr)
{
    return (
        m_log
        << this->name() << field_sep
        << m_cycle << field_sep
        << mwmr.second << field_sep
        << std::dec << std::noshowbase << elem.srcid() << field_sep
        );
}

tmpl(void)::handle_txn(const VciLoggerElem<vci_param> &elem)
{
    for ( typename std::vector<mwmr_info_t>::const_iterator i = m_mwmr_info.begin();
          i != m_mwmr_info.end();
          ++i ) {
        const mwmr_info_t &mwmr = *i;

        if ( (elem.address() < mwmr.first) ||
             ((elem.address() + elem.plen()) >= (mwmr.first + sizeof(soclib_mwmr_status_s))) )
            continue;

        handle_txn(elem, mwmr);
    }
}

tmpl(void)::handle_txn(const VciLoggerElem<vci_param> &elem, const mwmr_info_t &mwmr)
{
    switch ( elem.command() ) {
    case vci_param::CMD_LOCKED_READ:
        prefix(elem, mwmr)
            << "read_lock"
            << std::endl;
        break;
    case vci_param::CMD_STORE_COND:
    {
        typename vci_param::fast_data_t rdata = elem.rdata()[0];
        typename vci_param::fast_data_t wdata = elem.wdata()[0];
        if ( wdata == 1 ) {
            if ( rdata == vci_param::STORE_COND_ATOMIC )
                prefix(elem, mwmr) << "lock_take" << std::endl;
            else
                prefix(elem, mwmr) << "lock_miss" << std::endl;
        } else {
            if ( rdata == vci_param::STORE_COND_ATOMIC )
                prefix(elem, mwmr) << "lock_release" << std::endl;
            else
                prefix(elem, mwmr) << "lock_release_miss" << std::endl;
        }
        break;
    }
    case vci_param::CMD_READ:
        break;
    case vci_param::CMD_WRITE:
    {
        typename vci_param::fast_addr_t base = elem.address() - mwmr.first;

        std::vector<typename vci_param::fast_data_t> wdata = elem.wdata();
        typename std::vector<typename vci_param::fast_data_t>::const_iterator v = wdata.begin();

        for ( size_t offset = 0;
              offset < wdata.size() * vci_param::B;
              offset += vci_param::B ) {

            switch ( base+offset ) {
            case 0:
                prefix(elem, mwmr) << "write_rptr" << field_sep << *v << std::endl;
                break;
            case 4:
                prefix(elem, mwmr) << "write_wptr" << field_sep << *v << std::endl;
                break;
            case 8:
                prefix(elem, mwmr) << "write_usage" << field_sep << *v << std::endl;
                break;
            case 12:
                if ( *v )
                    prefix(elem, mwmr) << "lock_force" << std::endl;
                else
                    prefix(elem, mwmr) << "lock_release" << std::endl;
                break;
            }
            ++v;
        }
        break;
    }
    }
}

tmpl(void)::transition()
{
    if ( this->p_resetn.read() == 0 ) {
        m_cycle = 0;
        return;
    }

    m_cycle++;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


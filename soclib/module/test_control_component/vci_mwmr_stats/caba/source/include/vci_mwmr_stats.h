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
#ifndef SOCLIB_CABA_VCI_MWMR_STATS_H
#define SOCLIB_CABA_VCI_MWMR_STATS_H

#include <systemc>
#include <ios>
#include <fstream>
#include "loader.h"
#include "vci_logger.h"
#include "mapping_table.h"
#include "mwmr_controller.h"

namespace soclib {
namespace caba {

template<typename vci_param>
class VciMwmrStats
	: public soclib::caba::VciLogger<vci_param>
{
    uint64_t m_cycle;
    typedef std::pair<
        typename vci_param::fast_addr_t,
        std::string> mwmr_info_t;
    std::vector<mwmr_info_t> m_mwmr_info;
    static const char *field_sep;
    std::ofstream m_log;

protected:
    SC_HAS_PROCESS(VciMwmrStats);
    virtual void handle_txn(const VciLoggerElem<vci_param> &elem);
    void handle_txn(const VciLoggerElem<vci_param> &elem, const mwmr_info_t &mwmr);

    std::ostream &prefix(const VciLoggerElem<vci_param> &, const mwmr_info_t &);

public:
    VciMwmrStats(
        sc_core::sc_module_name insname,
        const soclib::common::MappingTable &mt,
        const soclib::common::Loader &loader,
        const char *log_name,
        const std::vector<std::string> &fifo_name );
    ~VciMwmrStats();

private:
    void transition();
};

}}

#endif /* SOCLIB_CABA_VCI_MWMR_STATS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


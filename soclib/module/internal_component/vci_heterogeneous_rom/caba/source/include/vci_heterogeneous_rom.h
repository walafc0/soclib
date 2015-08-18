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
 *         Nicolas Pouillon <nipo@ssji.net>, 2006
 *         Alexandre Becoulet <alexandre.becoulet@lip6.fr>, 2007
 *
 * Maintainers: becoulet
 */

#ifndef SOCLIB_CABA_HET_ROM_H
#define SOCLIB_CABA_HET_ROM_H

#include <map>

#include <systemc>
#include "vci_target_fsm.h"
#include "caba_base_module.h"
#include "mapping_table.h"
#include "loader.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciHeterogeneousRom
	: public soclib::caba::BaseModule
{
    soclib::caba::VciTargetFsm<vci_param,true,false> m_vci_fsm;

    typedef uint32_t rom_t;

    rom_t ** m_assoc[1 << vci_param::S];

    typedef std::pair<soclib::common::Loader *, rom_t**> m_groups_pair_t;
    typedef std::map<soclib::common::Loader *, rom_t**> m_groups_map_t;

    m_groups_map_t m_groups;

    const MappingTable &m_mt;

protected:
	SC_HAS_PROCESS(VciHeterogeneousRom);

public:
    typedef typename vci_param::addr_t vci_addr_t;
    typedef typename vci_param::data_t vci_data_t;

    void add_srcid(soclib::common::Loader &loader, const IntTab &srcid);

    sc_in<bool> p_resetn;
    sc_in<bool> p_clk;
    soclib::caba::VciTarget<vci_param> p_vci;

    VciHeterogeneousRom(
        sc_module_name insname,
        const IntTab &index,
        const MappingTable &mt);

    ~VciHeterogeneousRom();

private:
    bool on_write(size_t seg, vci_addr_t addr, vci_data_t data, int be);
    bool on_read( size_t seg, vci_addr_t addr, vci_data_t &data );
    void transition();
    void genMoore();
};

}}

#endif /* SOCLIB_CABA_HET_ROM_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


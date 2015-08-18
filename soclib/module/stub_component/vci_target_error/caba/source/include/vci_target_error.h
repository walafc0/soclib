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
 *         Cesar Fuguet <cesar.fuguet-tortolero@lip6.fr>, 2015
 *
 * Maintainers: cfuguet
 */
#ifndef SOCLIB_CABA_TARGET_ERROR_H
#define SOCLIB_CABA_TARGET_ERROR_H

#include <systemc>
#include "vci_target_fsm.h"
#include "caba_base_module.h"
#include "mapping_table.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciTargetError : public soclib::caba::BaseModule
{
 protected:
    SC_HAS_PROCESS(VciTargetError);

 public:
    typedef typename vci_param::addr_t vci_addr_t;
    typedef typename vci_param::data_t vci_data_t;

    sc_in<bool> p_resetn;
    sc_in<bool> p_clk;
    soclib::caba::VciTarget<vci_param> p_vci;

    VciTargetError(sc_module_name name,
                   const IntTab &index,
                   const MappingTable &mt);

    ~VciTargetError();

 private:

    soclib::caba::VciTargetFsm<vci_param, true, false> m_vci_fsm;

    bool on_write(size_t seg, vci_addr_t addr, vci_data_t data, int be);
    bool on_read(size_t seg, vci_addr_t addr, vci_data_t &data);
    void transition();
    void genMoore();

    // Activity counters
    uint32_t m_cpt_cycles;
};

}}

#endif /* SOCLIB_CABA_TARGET_ERROR_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


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

#include "vci_target_error.h"

namespace soclib {
namespace caba {

using namespace soclib;

#define tmpl(x) template<typename vci_param> x VciTargetError<vci_param>

tmpl(/**/)::VciTargetError(sc_module_name name,
                           const IntTab &index,
                           const MappingTable &mt) :
    caba::BaseModule(name),
    p_resetn("resetn"),
    p_clk("clk"),
    p_vci("vci"),
    m_vci_fsm(p_vci, mt.getSegmentList(index))
{
    m_vci_fsm.on_read_write(on_read, on_write);

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

    m_cpt_cycles = 0;
}

tmpl(/**/)::~VciTargetError()
{
}

tmpl(bool)::on_write(size_t seg, vci_addr_t addr, vci_data_t data, int be)
{
#ifdef SOCLIB_MODULE_DEBUG
    std::cout << "[" << name() << "] time " << m_cpt_cycles
              << " address = " << std::hex << addr
              << std::dec << std::endl;
#endif

    return false;
}

tmpl(bool)::on_read(size_t seg, vci_addr_t addr, vci_data_t &data)
{
#ifdef SOCLIB_MODULE_DEBUG
    std::cout << "[" << name() << "] time " << m_cpt_cycles
              << " address = " << std::hex << addr
              << std::dec << std::endl;
#endif

    return false;
}

tmpl(void)::transition()
{
    m_cpt_cycles++;

    if (!p_resetn.read()) {
        m_vci_fsm.reset();
        return;
    }

    m_vci_fsm.transition();
}

tmpl(void)::genMoore()
{
    m_vci_fsm.genMoore();
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


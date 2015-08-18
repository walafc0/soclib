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
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *         Aline Vieira de Mello <Aline.Vieira-de-Mello@lip6.fr>, 2008
 *
 * Maintainers: nipo, alinev
 */
#ifndef SOCLIB_TLMT_FIFO_WRITER_H
#define SOCLIB_TLMT_FIFO_WRITER_H

#include <tlmt>
#include "tlmt_base_module.h"
#include "fifo_ports.h"
#include "vci_ports.h"
#include "process_wrapper.h"

namespace soclib { namespace tlmt {

template<typename vci_param>
class FifoWriter
    : public soclib::tlmt::BaseModule
{
private:
    soclib::common::ProcessWrapper m_wrapper;
    typename vci_param::data_t     m_read_buffer[64];
    int                            m_woffset;
    unsigned int                   m_status;
    uint32_t                       m_fifo_depth;

    tlmt_core::tlmt_thread_context c0;
    fifo_cmd_packet<vci_param>     m_cmd;
    sc_core::sc_event              m_rsp_read;

protected:
    SC_HAS_PROCESS(FifoWriter);

public:
    soclib::tlmt::FifoInitiator<vci_param> p_fifo;

    FifoWriter( sc_core::sc_module_name name,
                const std::string &bin,
                const std::vector<std::string> &argv,
                uint32_t fifo_depth);

    void readReponseReceived(int data,
                             const tlmt_core::tlmt_time &time,
                             void *private_data);

private:
    void execLoop();
};

}}

#endif /* SOCLIB_CABA_FIFO_WRITER_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

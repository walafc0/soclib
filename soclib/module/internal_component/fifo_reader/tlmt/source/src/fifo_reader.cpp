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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *         Aline Vieira de Mello <Aline.Vieira-de-Mello@lip6.fr>, 2008
 *
 * Maintainers: nipo, alinev
 */

#include "vci_param.h"
#include "../include/fifo_reader.h"

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x FifoReader<vci_param>

tmpl(/**/)::FifoReader(sc_core::sc_module_name name,
                       const std::string &bin,
                       const std::vector<std::string> &argv,
                       uint32_t depth_fifo)
    : soclib::tlmt::BaseModule(name),
      m_wrapper( bin, argv ),
      p_fifo("fifo", new tlmt_core::tlmt_callback<FifoReader,int>(this,&FifoReader<vci_param>::writeReponseReceived))
{
    m_depth_fifo = (depth_fifo/vci_param::nbytes);
    m_data = 0;
    m_woffset = 0;
    m_status = 0;
    SC_THREAD(execLoop);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// RECEIVE REPONSE OF A WRITE REQUEST 
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(void)::writeReponseReceived(int data,
                                 const tlmt_core::tlmt_time &time,
                                 void *private_data)
{
    //update time
    c0.update_time(time);
    m_rsp_write.notify(sc_core::SC_ZERO_TIME);
}

tmpl(void)::execLoop()
{
    while(true){
        for(uint32_t i = 0; i < m_depth_fifo; i++){
            m_status = m_wrapper.read(((char*)&m_data)+m_woffset, sizeof(typename vci_param::data_t)-m_woffset);
            if ( m_status <= sizeof(typename vci_param::data_t) ) {
                m_woffset += m_status;
                if ( m_woffset == sizeof(typename vci_param::data_t) ) {
                    // Got it
                    m_woffset = 0;
                    m_write_buffer[i] = m_data;
                }
            }
        }
        c0.add_time(m_depth_fifo);
        m_cmd.nwords  = m_depth_fifo;
        m_cmd.buf = m_write_buffer;
        p_fifo.send(&m_cmd, c0.time());
        wait(m_rsp_write);
    }
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


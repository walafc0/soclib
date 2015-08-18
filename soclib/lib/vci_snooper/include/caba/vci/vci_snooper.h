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
 * Copyright (c) STMicroelectronics, UPMC, Lip6, Asim
 *         Joel Porquet <joel.porquet@st.com>, 2008
 *
 */
#ifndef SOCLIB_CABA_VCI_SNOOPER_H_
#define SOCLIB_CABA_VCI_SNOOPER_H_

#include <systemc>
#include "vci_initiator.h"
#include "vci_target.h"
#include "vci_buffers.h"
#include "generic_fifo.h"
#include "caba/caba_base_module.h"

namespace soclib { namespace caba {

using namespace sc_core;

namespace _snooper {

template<typename _fifo_t> class OutputPortQueue;
template<typename _fifo_t> class InputPortQueue;
template<typename in_queue_t, typename out_queue_t> class VciLine;

}

template<typename vci_param>
class VciSnooper
{
public:

    typedef VciCmdBuffer<vci_param> cmd_pkt_t;
    typedef VciRspBuffer<vci_param> rsp_pkt_t;

    /* typedef */
    typedef GenericFifo<cmd_pkt_t> cmd_fifo_t;
    typedef GenericFifo<rsp_pkt_t> rsp_fifo_t;

private:

    /* typedef */
    typedef _snooper::OutputPortQueue<cmd_fifo_t> out_cmd_queue_t;
    typedef _snooper::OutputPortQueue<rsp_fifo_t> out_rsp_queue_t;

    typedef _snooper::InputPortQueue<cmd_fifo_t> in_cmd_queue_t;
    typedef _snooper::InputPortQueue<rsp_fifo_t> in_rsp_queue_t;

    /* variables */
    VciInitiator<vci_param> &p_vci_out;
    VciTarget<vci_param> &p_vci_in;

    cmd_fifo_t m_in_cmd_fifo;
    cmd_fifo_t m_out_cmd_fifo;
 
    rsp_fifo_t m_in_rsp_fifo;
    rsp_fifo_t m_out_rsp_fifo;

    _snooper::VciLine<in_cmd_queue_t,out_cmd_queue_t> *m_cmd_vl;
    _snooper::VciLine<in_rsp_queue_t,out_rsp_queue_t> *m_rsp_vl;

public:

    VciSnooper(
            VciInitiator<vci_param> &p_out,
            VciTarget<vci_param> &p_in,
            const size_t fifo_depth
            );

    ~VciSnooper();

    void reset();

    void transitionCmd(bool &write_fifo, cmd_pkt_t &write_pkt, bool &read_fifo);
    void transitionRsp(bool &write_fifo, rsp_pkt_t &write_pkt, bool &read_fifo);
    void genMoore();

    cmd_fifo_t& getInCmdFifo()   { return m_in_cmd_fifo;     }
    cmd_fifo_t& getOutCmdFifo()  { return m_out_cmd_fifo;    }

    rsp_fifo_t& getInRspFifo()   { return m_in_rsp_fifo;     }
    rsp_fifo_t& getOutRspFifo()  { return m_out_rsp_fifo;    }
};

}}

#endif /* SOCLIB_CABA_VCI_SNOOPER_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

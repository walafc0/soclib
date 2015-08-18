/*
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

#include <systemc>
#include <cassert>
#include "vci_snooper.h"
#include "alloc_elems.h"

#ifdef SOCLIB_MODULE_DEBUG
#define DEBUG_BEGIN do { do{} while(0)
#define DEBUG_END } while(0)
#else
#define DEBUG_BEGIN do { if (0) { do{} while(0)
#define DEBUG_END } } while(0)
#endif

namespace soclib { namespace caba {

using namespace sc_core;

namespace _snooper {

template<typename _fifo_t>
class OutputPortQueue
{
public:
    typedef _fifo_t fifo_t;
    typedef typename fifo_t::data_t vci_pkt_t;
    typedef typename fifo_t::data_t::output_port_t output_port_t;

private:
    fifo_t  &m_input_queue;

public:
    OutputPortQueue(fifo_t &input_queue)
        :m_input_queue(input_queue)
    {}

    ~OutputPortQueue()
    {}
    
    void transition( const output_port_t &port, bool &read_fifo )
    {
        read_fifo = port.peerAccepted();

DEBUG_BEGIN;
        if (read_fifo)
            std::cout << "SNOOPER sent packet " << m_input_queue.read() << std::endl;
DEBUG_END;
    }

    void genMoore( output_port_t &port )
    {
        if (m_input_queue.rok()) {
            vci_pkt_t pkt;
            pkt = m_input_queue.read();
DEBUG_BEGIN;
            std::cout << "SNOOPER packet on VCI " << pkt << std::endl;
DEBUG_END;
            pkt.writeTo(port);
        } else
            port.setVal(false);
    }
};

template<typename _fifo_t>
class InputPortQueue
{
public:
    typedef _fifo_t fifo_t;
    typedef typename fifo_t::data_t vci_pkt_t;
    typedef typename fifo_t::data_t::input_port_t input_port_t;
    
private:
    fifo_t  &m_output_queue;

public:
    InputPortQueue(fifo_t &output_queue)
        :m_output_queue(output_queue)
    {
    }

    ~InputPortQueue()
    {}

    void transition( const input_port_t &port, bool &write_fifo, vci_pkt_t &pkt )
    {
        if ( port.iAccepted() ) {
            pkt.readFrom(port);
DEBUG_BEGIN;
            std::cout << "SNOOPER accepting " << pkt << std::endl;
DEBUG_END;
            write_fifo = true;
        }
        else
            write_fifo = false;
    }

    void genMoore( input_port_t &port )
    {
        if (m_output_queue.wok())
        {
            port.setAck(true);
DEBUG_BEGIN;
            std::cout << "SNOOPER is accepting" << std::endl;
DEBUG_END;
        } else {
            port.setAck(false);
DEBUG_BEGIN;
            std::cout << "SNOOPER is not accepting " << std::endl;
DEBUG_END;
        }
    }
};

template<typename in_queue_t,typename out_queue_t>
class VciLine
{
    typedef typename out_queue_t::fifo_t::data_t pkt_t;

    typedef typename out_queue_t::fifo_t::data_t::input_port_t  input_port_t;
    typedef typename out_queue_t::fifo_t::data_t::output_port_t output_port_t;

    typedef typename out_queue_t::fifo_t    out_fifo_t;
    typedef typename in_queue_t::fifo_t     in_fifo_t;

    in_queue_t  m_in_queue;
    out_queue_t m_out_queue;

public:
    VciLine(in_fifo_t &in_fifo,
            out_fifo_t &out_fifo)
        :m_in_queue(in_fifo),
        m_out_queue(out_fifo)
    {
    }

    ~VciLine()
    {}

    void transition( 
            const input_port_t &input_port, 
            const output_port_t &output_port,
            bool    &write_fifo,
            pkt_t   &write_pkt,
            bool    &read_fifo
            )
    {
        m_in_queue.transition( input_port, write_fifo, write_pkt );
        m_out_queue.transition( output_port, read_fifo );
    }

    void genMoore( input_port_t &input_port, output_port_t &output_port )
    {
        m_in_queue.genMoore( input_port );
        m_out_queue.genMoore( output_port );
    }
};

}


#define tmpl(x) template<typename vci_param> x VciSnooper<vci_param>

tmpl(void)::transitionCmd(bool &write_fifo, cmd_pkt_t &write_pkt, bool &read_fifo)
{
    m_cmd_vl->transition( p_vci_in, p_vci_out, write_fifo, write_pkt, read_fifo );
}

tmpl(void)::transitionRsp(bool &write_fifo, rsp_pkt_t &write_pkt, bool &read_fifo)
{
    m_rsp_vl->transition( p_vci_out, p_vci_in, write_fifo, write_pkt, read_fifo );
}

tmpl(void)::genMoore()
{
    m_cmd_vl->genMoore( p_vci_in, p_vci_out );
    m_rsp_vl->genMoore( p_vci_out, p_vci_in );
}

tmpl(void)::reset()
{
    m_in_cmd_fifo.init();
    m_out_cmd_fifo.init();
    m_in_rsp_fifo.init();
    m_out_rsp_fifo.init();
}

tmpl(/**/)::VciSnooper(
        VciInitiator<vci_param> &p_out,
        VciTarget<vci_param> &p_in,
        const size_t fifo_depth
        )
        :p_vci_out(p_out),
        p_vci_in(p_in),
        m_in_cmd_fifo("m_in_cmd_fifo", fifo_depth),
        m_out_cmd_fifo("m_out_cmd_fifo", fifo_depth),
        m_in_rsp_fifo("m_in_rsp_fifo", fifo_depth),
        m_out_rsp_fifo("m_out_rsp_fifo", fifo_depth)
{
    m_cmd_vl = new _snooper::VciLine<in_cmd_queue_t,out_cmd_queue_t>(m_in_cmd_fifo, m_out_cmd_fifo);
    m_rsp_vl = new _snooper::VciLine<in_rsp_queue_t,out_rsp_queue_t>(m_in_rsp_fifo, m_out_rsp_fifo);
}

tmpl(/**/)::~VciSnooper()
{
    delete m_cmd_vl;
    delete m_rsp_vl;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

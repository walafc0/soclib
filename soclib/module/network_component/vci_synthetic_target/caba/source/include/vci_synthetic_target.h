/* -*- c++ -*-
 *
 * Authors  : alain.greiner@lip6.fr 
 * Date     : march 2013
 * Copyright: UPMC - LIP6
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
 */

#ifndef SOCLIB_CABA_VCI_SYNTHETIC_TARGET_H
#define SOCLIB_CABA_VCI_SYNTHETIC_TARGET_H

#include <systemc>
#include <cassert>
#include "caba_base_module.h"
#include "vci_target.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciSyntheticTarget
	: public soclib::caba::BaseModule
{
public:

	typedef typename vci_param::addr_t  vci_addr_t;
	typedef typename vci_param::srcid_t vci_srcid_t;
	typedef typename vci_param::trdid_t vci_trdid_t;
	typedef typename vci_param::pktid_t vci_pktid_t;

    enum fsm_state_e 
    {
        FSM_IDLE,
        FSM_RSP_READ,
        FSM_RSP_WRITE,
    };

private:

    sc_signal<int>                          r_fsm_state;
    sc_signal<size_t>                       r_flit_count;
    sc_signal<vci_srcid_t>                  r_srcid;
    sc_signal<vci_trdid_t>                  r_trdid;
    sc_signal<vci_pktid_t>                  r_pktid;

    size_t                                  m_x;
    vci_addr_t                              m_x_mask;
    size_t                                  m_x_shift;

    size_t                                  m_y;
    vci_addr_t                              m_y_mask;
    size_t                                  m_y_shift;

    size_t                                  m_l;
    vci_addr_t                              m_l_mask;
    size_t                                  m_l_shift;

protected:

	SC_HAS_PROCESS(VciSyntheticTarget);

public:

    // Ports
    sc_in<bool>                             p_resetn;
    sc_in<bool>                             p_clk;
    soclib::caba::VciTarget<vci_param>      p_vci;

    VciSyntheticTarget( sc_module_name name,
                        const size_t   tgtid,	    // global identifier (x,y,l)
                        const size_t   x_width,		// x field width             
                        const size_t   y_width,     // y field width         
                        const size_t   l_width );   // l field width          

    void print_trace();

private:

    void transition();
    void genMoore();

};

}}

#endif 


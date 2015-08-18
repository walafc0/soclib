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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: alain
 */
#ifndef SOCLIB_VCI_MULTI_TTY_H
#define SOCLIB_VCI_MULTI_TTY_H

#include <systemc>
#include "caba_base_module.h"
#include "tty_wrapper.h"
#include "mapping_table.h"
#include "int_tab.h"
#include "vci_target.h"

namespace soclib {
namespace caba {

using namespace sc_core;
using namespace soclib::common;

///////////////////////////////////////////
template<typename vci_param>
class VciMultiTty
///////////////////////////////////////////
	: public soclib::caba::BaseModule
{
public:

    typedef typename vci_param::fast_addr_t  vci_addr_t;
    typedef typename vci_param::fast_data_t  vci_data_t;
    typedef typename vci_param::srcid_t      vci_srcid_t;
    typedef typename vci_param::trdid_t      vci_trdid_t;
    typedef typename vci_param::pktid_t      vci_pktid_t;


    enum fsm_state_e
    {
        IDLE,
        RSP_WRITE,
        RSP_READ,
        RSP_ERROR,
    };

    // Ports
    sc_in<bool>                                 p_clk;
    sc_in<bool>                                 p_resetn;
    soclib::caba::VciTarget<vci_param>          p_vci;
    sc_out<bool>*                               p_irq;

	VciMultiTty( sc_module_name                 name,
		         const IntTab                   &index,
		         const MappingTable             &mt,
                 const std::vector<std::string> &names );

	VciMultiTty( sc_module_name                 name, 
                 const IntTab                   &index,
		         const MappingTable             &mt,
                 const char*                    first_name, ...);


    ~VciMultiTty();
    

private:

    // Registers
    sc_signal<int>                              r_fsm_state;
    sc_signal<vci_data_t>                       r_rdata;
    sc_signal<vci_srcid_t>                      r_srcid;
    sc_signal<vci_trdid_t>                      r_trdid;
    sc_signal<vci_pktid_t>                      r_pktid;
    sc_signal<uint32_t>                         r_cpt_read;
    sc_signal<uint32_t>                         r_cpt_write;
    sc_signal<bool>                             r_lock;
      
    std::vector<soclib::common::TtyWrapper*>    m_term;
    std::list<soclib::common::Segment>          m_seglist;

    void transition();
    void genMoore();

	void init(const std::vector<std::string> &names );

protected:

    SC_HAS_PROCESS(VciMultiTty);

public:

    void print_stats();
    void print_trace();
};

}}

#endif /* SOCLIB_VCI_MULTI_TTY_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


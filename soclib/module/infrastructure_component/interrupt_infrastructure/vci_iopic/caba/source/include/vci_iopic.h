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
 *         Alain Greiner <alain.greiner@lip6.fr> 2014
 *
 * Maintainers: alain
 */

#ifndef SOCLIB_VCI_IOPIC_H
#define SOCLIB_VCI_IOPIC_H

#include <systemc>
#include "caba_base_module.h"
#include "mapping_table.h"
#include "vci_initiator.h"
#include "vci_target.h"

namespace soclib {
namespace caba {

//////////////////////////////////////////
template<typename vci_param>
class VciIopic
//////////////////////////////////////////
	: public caba::BaseModule
{

public:

    typedef typename vci_param::fast_data_t  data_t;
    typedef typename vci_param::fast_addr_t  addr_t;
    typedef typename vci_param::srcid_t      srcid_t;
    typedef typename vci_param::trdid_t      trdid_t;
    typedef typename vci_param::pktid_t      pktid_t;

private:

    // structural constants
    const size_t                        m_srcid;          // SRCID
    const size_t                        m_channels;       // number of input HWI
    std::list<soclib::common::Segment>  m_seglist;        // segment list

    enum ini_fsm_state_e 
    {
        I_IDLE,
        I_SET_CMD,
        I_RESET_CMD,
        I_WAIT_RSP,
    };

    enum tgt_fsm_state_e 
    {
        T_IDLE,
        T_WRITE,
        T_READ,
        T_ERROR,
        T_WAIT_EOP,
    };
    
    // registers
    sc_core::sc_signal<int>         r_tgt_fsm;
    sc_core::sc_signal<srcid_t>     r_srcid;  
    sc_core::sc_signal<trdid_t>     r_trdid;  
    sc_core::sc_signal<pktid_t>     r_pktid;  
    sc_core::sc_signal<uint32_t>    r_rdata;

    sc_core::sc_signal<int>         r_ini_fsm;
    sc_core::sc_signal<size_t>      r_channel;        // selected channel 

    sc_core::sc_signal<bool>*       r_hwi;            // array: curent HWI values
    sc_core::sc_signal<bool>*       r_mask;           // array: transmitted if true
    sc_core::sc_signal<uint32_t>*   r_address;        // array: WTI address   
    sc_core::sc_signal<uint32_t>*   r_extend;         // array: WTI address extension
    sc_core::sc_signal<bool>*       r_error;          // array: WTI error received

    void transition();
    void genMoore();

protected:

    SC_HAS_PROCESS(VciIopic);

public:

    sc_core::sc_in<bool>                     p_clk;
    sc_core::sc_in<bool>                     p_resetn;
    soclib::caba::VciInitiator<vci_param>    p_vci_initiator;
    soclib::caba::VciTarget<vci_param>       p_vci_target;
    sc_core::sc_in<bool>*                    p_hwi;

	~VciIopic();

    void print_trace();

	VciIopic(
		sc_core::sc_module_name name,
		const soclib::common::MappingTable &mt,
		const soclib::common::IntTab &srcid,
		const soclib::common::IntTab &tgtid,
        const size_t                 channels );
};

}}

#endif /* SOCLIB_VCI_WTI_GENERATOR_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


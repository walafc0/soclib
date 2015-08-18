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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2008
 *
 * Maintainers: alain
 */

////////////////////////////////////////////////////////////////////////////
//  This component is a multi-segments ROM controller.
//  The VCI DATA can must be 32 bits or 64 bits.
//  The VCI ADDRESS and the PLEN fields should be multiple of 4 bytes.
//  It does not accept WRITE, LL, SC or CAS commands.
//  A READ burst command packet (such a cache line request) 
//  contains one single flit. The number of flits in the response packet 
//  depends on the PLEN field:
//  - If VCI DATA width = 32 bits, each flit contains 4 bytes, and the 
//    number of flits is PLEN/4.
//  - If VCI DATA width = 64 bits, and PLEN define an even number of words,
//    each flit contains 8 bytes, and the number of flits is PLEN/8.
//  - If VCI DATA width = 64 bits, and PLEN define an odd number of words,
//    the last flit contains only 4 bytes right justified,
//    and the number of flits is PLEN/8 + 1.
////////////////////////////////////////////////////////////////////////////
//  Implementation note: 
//  The ROM segments are implemented as a set of uint32_t arrays
//  (one array per segment). 
//  This component is controlled by a single FSM.
//  The VCI command is analysed and checked in the IDLE state.
//  The response is sent in the READ (or ERROR) state.
/////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_CABA_VCI_SIMPLE_ROM_H
#define SOCLIB_CABA_VCI_SIMPLE_ROM_H

#include <systemc>
#include <vector>
#include <list>
#include <cassert>
#include "caba_base_module.h"
#include "vci_target.h"
#include "mapping_table.h"
#include "int_tab.h"
#include "loader.h"
#include "soclib_endian.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciSimpleRom
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
        FSM_IDLE,
        FSM_RSP_READ,
        FSM_RSP_ERROR,
    };

private:

    const soclib::common::Loader            &m_loader;
    vci_addr_t                              m_mask;

    sc_signal<int>                          r_fsm_state;
    sc_signal<size_t>                       r_flit_count;
    sc_signal<bool>                         r_odd_words;
    sc_signal<size_t>                       r_seg_index;
    sc_signal<size_t>                       r_rom_index;
    sc_signal<vci_srcid_t>                  r_srcid;
    sc_signal<vci_trdid_t>                  r_trdid;
    sc_signal<vci_pktid_t>                  r_pktid;

    size_t                                  m_nbseg;
    uint32_t                                **m_rom;
    soclib::common::Segment                 **m_seg;

protected:

    SC_HAS_PROCESS(VciSimpleRom);

public:

    // Ports
    sc_in<bool>                             p_resetn;
    sc_in<bool>                             p_clk;
    soclib::caba::VciTarget<vci_param>      p_vci;

    VciSimpleRom(sc_module_name name,
                 const soclib::common::IntTab index,
                 const soclib::common::MappingTable &mt,
                 const soclib::common::Loader &loader,
                 const int nb_msb_drop = 0);

    ~VciSimpleRom();

    void print_trace();

private:

    void transition();
    void genMoore();
    void reload();
    void reset();
};

}}

#endif 

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


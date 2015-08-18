/*
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

#include <iostream>
#include <cstring>
#include "arithmetics.h"
#include "vci_simple_rom.h"

namespace soclib {
namespace caba {

using namespace soclib;

#define tmpl(x) template<typename vci_param> x VciSimpleRom<vci_param>

//////////////////////////
tmpl(/**/)::VciSimpleRom(
    sc_module_name name,
    const soclib::common::IntTab index,
    const soclib::common::MappingTable &mt,
    const soclib::common::Loader &loader,
    const int nb_msb_drop)
    : caba::BaseModule(name),
      m_loader(loader),
      m_mask((1ULL << (vci_param::N - nb_msb_drop)) - 1),

      r_fsm_state("r_fsm_state"),
      r_flit_count("r_flit_count"),
      r_odd_words("r_odd_words"),
      r_seg_index("r_seg_index"),
      r_rom_index("r_rom_index"),
      r_srcid("r_srcid"),
      r_trdid("r_trdid"),
      r_pktid("r_pktid"),

      p_resetn("p_resetn"),
      p_clk("p_clk"),
      p_vci("p_vci")
{
    std::cout << "  - Building SimpleRom " << name << std::endl;

    size_t nsegs = 0;

    // get segments list
    std::list<soclib::common::Segment> seglist = mt.getSegmentList(index);

    assert( (seglist.empty() == false) and
    "VCI_SIMPLE_ROM error : no segment allocated");

    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = seglist.begin() ; seg != seglist.end() ; seg++ ) nsegs++;

    m_nbseg = nsegs;

    assert( ((vci_param::B == 4) or (vci_param::B == 8)) and
    "VCI_SIMPLE_ROM error : The VCI DATA field must be 32 or 64 bits");

    // actual memory allocation
    m_rom = new uint32_t*[m_nbseg];
    m_seg = new soclib::common::Segment*[m_nbseg];

    int i = 0;
    for ( seg = seglist.begin() ; seg != seglist.end() ; ++seg, ++i )
    {
        const soclib::common::Segment& s = seg->masked(m_mask);
        m_rom[i] = new uint32_t[ (seg->size()+3)/4 ];
        m_seg[i] = new soclib::common::Segment(s);

        std::cout << "    => segment " << m_seg[i]->name()
                  << " / base = " << std::hex << m_seg[i]->baseAddress()
                  << " / size = " << m_seg[i]->size() << std::endl;
    }

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
}

///////////////////////////
tmpl(/**/)::~VciSimpleRom()
{
    for (size_t i=0 ; i<m_nbseg ; ++i) delete [] m_rom[i];
    for (size_t i=0 ; i<m_nbseg ; ++i) delete m_seg[i];
    delete [] m_rom;
    delete [] m_seg;
}

/////////////////////
tmpl(void)::reload()
{
    for ( size_t i=0 ; i<m_nbseg ; ++i )
    {
        m_loader.load(&m_rom[i][0], m_seg[i]->baseAddress(), m_seg[i]->size());
        for ( size_t addr = 0 ; addr < m_seg[i]->size()/vci_param::B ; ++addr )
            m_rom[i][addr] = le_to_machine(m_rom[i][addr]);
    }
}

////////////////////
tmpl(void)::reset()
{
    for ( size_t i=0 ; i<m_nbseg ; ++i ) std::memset(&m_rom[i][0], 0, m_seg[i]->size());
    r_fsm_state = FSM_IDLE;
}

//////////////////////////
tmpl(void)::print_trace()
{
    const char* state_str[] = { "IDLE", "READ", "ERROR" };
    std::cout << "SIMPLE_ROM " << name() 
              << " : state = " << state_str[r_fsm_state] 
              << " / flit_count = " << std::dec << r_flit_count << std::endl;
}

/////////////////////////
tmpl(void)::transition()
{
    if (!p_resetn) 
    {
        reset();
        reload();
        return;
    }

    switch ( r_fsm_state ) 
    {
        //////////////
        case FSM_IDLE:  // waiting a VCI command 
        {
            if ( p_vci.cmdval.read() ) 
            {
                bool error = true;

                assert( ((p_vci.address.read() & 0x3) == 0) and 
                "VCI_SIMPLE_ROM ERROR : The VCI ADDRESS must be multiple of 4");

                assert( ((p_vci.plen.read() & 0x3) == 0) and 
                "VCI_SIMPLE_ROM ERROR : The VCI PLEN must be multiple of 4");

                assert( (p_vci.plen.read() != 0) and
                "VCI_SIMPLE_ROM ERROR : The VCI PLEN should be != 0");

                assert( (p_vci.cmd.read() == vci_param::CMD_READ) and
                "VCI_SIMPLE_ROM ERROR : The VCI command must be a READ");

                assert( p_vci.eop.read() and
                "VCI_SIMPLE_ROM ERROR : The VCI command packet must be 1 flit");

                size_t index;
                unsigned int plen = p_vci.plen.read();
                vci_addr_t base = p_vci.address.read() & m_mask;
                vci_addr_t last = base + plen - 1;
                for ( index = 0 ; index < m_nbseg ; ++index)
                {
                    if ( (m_seg[index]->contains(base)) and
                         (m_seg[index]->contains(last)) )
                    {
                        error = false;
                        r_seg_index = index;
                        break;
                    }
                }

                r_srcid = p_vci.srcid.read();
                r_trdid = p_vci.trdid.read();
                r_pktid = p_vci.pktid.read();

                if ( error )
                {
                    r_fsm_state = FSM_RSP_ERROR;
                }
                else
                {
                    r_fsm_state = FSM_RSP_READ;
                    r_rom_index = (size_t)((base - m_seg[index]->baseAddress())>>2);

                    if ( vci_param::B == 8 )   // 64 bits data width
                    {
                        if ( plen & 0x4 )      // odd number of words 
                        {
                            r_flit_count = (plen>>3) + 1;
                            r_odd_words  = true;
                        }
                        else                   // even number of words
                        {
                            r_flit_count = plen>>3;
                            r_odd_words  = false;
                        }
                    }
                    else                       // 32 bits data width
                    {
                        r_flit_count = plen>>2;
                    }
                }

#ifdef SOCLIB_MODULE_DEBUG
                std::cout << "<" << name() << " FSM_IDLE>"
                    << " address = " << std::hex << base
                    << " seg_index = " << std::dec << index
                    << " seg_base = " << std::hex << m_seg[index]->baseAddress()
                    << std::dec << std::endl;
#endif
            }
            break;
        }
        //////////////////
        case FSM_RSP_READ:  // send one response flit 
        {
            if ( p_vci.rspack.read() )
            {
                r_flit_count = r_flit_count - 1;
                r_rom_index  = r_rom_index.read() + (vci_param::B>>2);
                if ( r_flit_count.read() == 1)   r_fsm_state = FSM_IDLE;

#ifdef SOCLIB_MODULE_DEBUG
            std::cout << "<" << name() << " FSM_RSP_READ>"
                      << " flit_count = " << r_flit_count.read()
                      << " / seg_index = " << r_seg_index.read()
                      << " / rom_index = " << std::hex << r_rom_index.read()
                      << std::dec << std::endl;
#endif
            }
            break;
        }
        ///////////////////
        case FSM_RSP_ERROR: // waits lat flit of a VCI CMD erroneous packet 
        {
            if ( p_vci.rspack.read() && p_vci.eop.read() )
            {
                r_fsm_state = FSM_IDLE;
            }
            break;
        }
    } // end switch fsm_state

} // end transition()

///////////////////////
tmpl(void)::genMoore()
{
    switch ( r_fsm_state.read() ) 
    {
        case FSM_IDLE:
        {
            p_vci.cmdack  = true;
            p_vci.rspval  = false;
            p_vci.rdata   = 0;
            p_vci.rsrcid  = 0;
            p_vci.rtrdid  = 0;
            p_vci.rpktid  = 0;
            p_vci.rerror  = 0;
            p_vci.reop    = false;
            break;
        }
        case FSM_RSP_READ:
        {
            vci_data_t rdata;
            size_t     seg_index = r_seg_index.read();
            size_t     rom_index = r_rom_index.read();

            if ( (vci_param::B == 4) or                                  // 32 bits data
                 ( r_odd_words.read() and (r_flit_count.read() == 1)) )  // last odd flit
            {
                rdata = (vci_data_t)m_rom[seg_index][rom_index];
            }
            else                                                         // 64 bits data
            {
                rdata = (uint64_t)m_rom[seg_index][rom_index] | 
                        (((uint64_t)m_rom[seg_index][rom_index+1]) << 32);
            }
            
            p_vci.cmdack  = false;
            p_vci.rspval  = true;
            p_vci.rdata   = rdata;
            p_vci.rsrcid  = r_srcid.read();
            p_vci.rtrdid  = r_trdid.read();
            p_vci.rpktid  = r_pktid.read();
            p_vci.rerror  = vci_param::ERR_NORMAL;
            p_vci.reop   = (r_flit_count.read() == 1);
            break;
        }
        case FSM_RSP_ERROR:
        {
            p_vci.cmdack  = false;
            p_vci.rspval  = true;
            p_vci.rdata   = 0;
            p_vci.rsrcid  = r_srcid.read();
            p_vci.rtrdid  = r_trdid.read();
            p_vci.rpktid  = r_pktid.read();
            p_vci.rerror  = vci_param::ERR_GENERAL_DATA_ERROR;
            p_vci.reop    = true;
            break;
        }
    } // end switch fsm_state
} // end genMoore()

}} 

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


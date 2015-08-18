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
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 */        

#include <strings.h>

#include "xicu.h"
#include "arithmetics.h"
#include "alloc_elems.h"
#include "../include/vci_xicu.h"

namespace soclib {
namespace caba {

using namespace soclib;

#define tmpl(t) template<typename vci_param> t VciXicu<vci_param>

#define CHECK_BOUNDS(x) do { if (idx >= (m_##x##_count)) r_fsm = RSP_ERROR ; break ; } while(0)

using namespace soclib::common;

////////////////////////
tmpl(void)::transition()
{
	if (!p_resetn.read()) 
    {
		r_fsm = IDLE;

        for ( size_t i = 0; i<m_pti_count; ++i ) 
        {
            r_pti_per[i] = 0;
            r_pti_val[i] = 0;
        }
        for ( size_t i = 0; i<m_wti_count; ++i )
        {
            r_wti_reg[i] = 0;
        }
        for ( size_t i = 0; i<m_irq_count; ++i ) 
        {
            r_msk_pti[i] = 0;
            r_msk_wti[i] = 0;
            r_msk_hwi[i] = 0;
        }
        r_pti_pending = 0;
        r_wti_pending = 0;
        r_hwi_pending = 0;

		return;
	}

    // Target FSM
	switch ( r_fsm.read() ) 
    {
        case IDLE:
        {
            if ( p_vci.cmdval.read() )
            {
   	            r_srcid = p_vci.srcid.read();
	            r_trdid = p_vci.trdid.read();
	            r_pktid = p_vci.pktid.read();

                bool     write   = ( p_vci.cmd.read() == vci_param::CMD_WRITE );
	            uint64_t address = p_vci.address.read();
                size_t   cell    = (size_t)address >> 2;
	            size_t   idx     = cell & 0x1f;
	            size_t   func    = (cell >> 5) & 0x1f;

                // get wdata for both 32 bits and 64 bits data width
                uint32_t wdata;
                if( (vci_param::B == 8) and (p_vci.be.read() == 0xF0) )
                    wdata = (uint32_t)(p_vci.wdata.read()>>32);
                else
                    wdata = (uint32_t)(p_vci.wdata.read());

                // check address errors
	            bool found = false;
	            std::list<soclib::common::Segment>::iterator seg;
	            for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ ) 
	            {
		            if ( seg->contains(address) ) found = true;
	            }
                //////////////
	            if (not found) 
                {
                    r_fsm = RSP_ERROR;
	            } 
                //////////////////////////////////////
                else if ((func == XICU_WTI_REG) and write )
                {
                    CHECK_BOUNDS(wti);
                    r_wti_reg[idx] = wdata;
                    r_wti_pending = r_wti_pending.read() | 1<<idx;
                    r_fsm = RSP_WRITE;
                }
                else if ( (func == XICU_WTI_REG) and not write )
                {
                    CHECK_BOUNDS(wti);
                    r_wti_pending = r_wti_pending.read() & ~(1<<idx);        
                    r_data = r_wti_reg[idx].read();
                    r_fsm  = RSP_READ;
                }
                ////////////////////////////////////////////
                else if ( (func == XICU_PTI_PER) and write )
                {
                    CHECK_BOUNDS(pti);
                    r_pti_per[idx] = wdata;
                    if ( !wdata ) 
                    {
                        r_pti_pending = r_pti_pending.read() & ~(1<<idx);
                        r_pti_val[idx] = 0;
                    } 
                    else if (r_pti_val[idx].read() == 0) 
                    {
                        r_pti_val[idx] = wdata;
                    }
                    r_fsm = RSP_WRITE;
                }
                else if ( (func == XICU_PTI_PER) and not write )
                {
                    CHECK_BOUNDS(pti);
                    r_data = r_pti_per[idx].read();
                    r_fsm  = RSP_READ;
                }
                ////////////////////////////////////////////
                else if ( (func == XICU_PTI_VAL) and write )
                {
                    CHECK_BOUNDS(pti);
                    r_pti_val[idx] = wdata;
                    r_fsm = RSP_WRITE;
                }
                else if ( (func == XICU_PTI_VAL) and not write )
                {
                    CHECK_BOUNDS(pti);
                    r_data = r_pti_val[idx].read();
                    r_fsm  = RSP_READ;
                }
                ////////////////////////////////////////////////
                else if ( (func == XICU_PTI_ACK) and not write )
                {
                    CHECK_BOUNDS(pti);
                    r_pti_pending = r_pti_pending.read() & ~(1<<idx);
                    r_data = 0;
                    r_fsm  = RSP_READ;
                }
                ////////////////////////////////////////////
                else if ( (func == XICU_MSK_PTI) and write )
                {
                    CHECK_BOUNDS(irq);
                    r_msk_pti[idx] = wdata;
                    r_fsm = RSP_WRITE;
                }
                else if ( (func == XICU_MSK_PTI) and not write )
                {
                    CHECK_BOUNDS(irq);
                    r_data = r_msk_pti[idx].read();
                    r_fsm  = RSP_READ;
                }
                ///////////////////////////////////////////////////
                else if ( (func == XICU_MSK_PTI_ENABLE) and write )
                {
                    CHECK_BOUNDS(irq);
                    r_msk_pti[idx] = r_msk_pti[idx].read() | wdata;
                    r_fsm = RSP_WRITE;
                }
                ///////////////////////////////////////////////////
                else if ( (func == XICU_MSK_PTI_DISABLE) and write )
                {
                    CHECK_BOUNDS(irq);
                    r_msk_pti[idx] = r_msk_pti[idx].read() & ~wdata;
                    r_fsm = RSP_WRITE;
                }
                ///////////////////////////////////////////////////
                else if ( (func == XICU_PTI_ACTIVE) and not write )
                {
                    CHECK_BOUNDS(irq);
                    r_data = r_msk_pti[idx].read() & r_pti_pending.read();
                    r_fsm  = RSP_READ;
                }
                ////////////////////////////////////////////
                else if ( (func == XICU_MSK_HWI) and write )
                {
                    CHECK_BOUNDS(irq);
                    r_msk_hwi[idx] = wdata;
                    r_fsm = RSP_WRITE;
                }
                else if ( (func == XICU_MSK_HWI) and not write )
                {
                    CHECK_BOUNDS(irq);
                    r_data = r_msk_hwi[idx].read();
                    r_fsm  = RSP_READ;
                }
                ///////////////////////////////////////////////////
                else if ( (func == XICU_MSK_HWI_ENABLE) and write )
                {
                    CHECK_BOUNDS(irq);
                    r_msk_hwi[idx] = r_msk_hwi[idx].read() | wdata;
                    r_fsm = RSP_WRITE;
                }
                ////////////////////////////////////////////////////
                else if ( (func == XICU_MSK_HWI_DISABLE) and write )
                {
                    CHECK_BOUNDS(irq);
                    r_msk_hwi[idx] = r_msk_hwi[idx].read() & ~wdata;
                    r_fsm = RSP_WRITE;
                }
                ///////////////////////////////////////////////////
                else if ( (func == XICU_HWI_ACTIVE) and not write )
                {
                    CHECK_BOUNDS(irq);
                    r_data = r_msk_hwi[idx].read() & r_hwi_pending.read();
                    r_fsm  = RSP_READ;
                }
                ////////////////////////////////////////////
                else if ( (func == XICU_MSK_WTI) and write )
                {
                    CHECK_BOUNDS(irq);
                    r_msk_wti[idx] = wdata;
                    r_fsm = RSP_WRITE;
                }
                else if ( (func == XICU_MSK_WTI) and not write )
                {
                    CHECK_BOUNDS(irq);
                    r_data = r_msk_wti[idx].read();
                    r_fsm  = RSP_READ;
                }
                ///////////////////////////////////////////////////
                else if ( (func == XICU_MSK_WTI_ENABLE) and write )
                {
                    CHECK_BOUNDS(irq);
                    r_msk_wti[idx] = r_msk_wti[idx] | wdata;
                    r_fsm = RSP_WRITE;
                }
                ////////////////////////////////////////////////////
                else if ( (func == XICU_MSK_WTI_DISABLE) and write )
                {
                    CHECK_BOUNDS(irq);
                    r_msk_wti[idx] = r_msk_wti[idx] & ~wdata;
                    r_fsm = RSP_WRITE;
                }
                ///////////////////////////////////////////////////
                else if ( (func == XICU_WTI_ACTIVE) and not write )
                {
                    CHECK_BOUNDS(irq);
                    r_data = r_msk_wti[idx].read() & r_wti_pending.read();
                    r_fsm  = RSP_READ;
                }
                /////////////////////////////////////////////
                else if ( (func == XICU_PRIO) and not write )
                {
                    CHECK_BOUNDS(irq);

                    uint32_t pti_vect = r_msk_pti[idx].read() & r_pti_pending.read();
                    uint32_t hwi_vect = r_msk_hwi[idx].read() & r_hwi_pending.read();
                    uint32_t wti_vect = r_msk_wti[idx].read() & r_wti_pending.read();

                    uint32_t pti_set = ((pti_vect) ? 1 : 0)<<0;
                    uint32_t hwi_set = ((hwi_vect) ? 1 : 0)<<1;
                    uint32_t wti_set = ((wti_vect) ? 1 : 0)<<2;

                    uint32_t pti_id = pti_set ? ((ctz<uint32_t>(pti_vect) & 0x1f)<< 8) : 0;
                    uint32_t hwi_id = hwi_set ? ((ctz<uint32_t>(hwi_vect) & 0x1f)<<16) : 0;
                    uint32_t wti_id = wti_set ? ((ctz<uint32_t>(wti_vect) & 0x1f)<<24) : 0;

                    r_data = pti_set | hwi_set | wti_set | pti_id  | hwi_id  | wti_id  ;
                    r_fsm  = RSP_READ;
                }
                ///////////////////////////////////////////////
                else if ( (func == XICU_CONFIG) and not write )
                {
                    r_data = (m_irq_count << 24) | 
                             (m_wti_count << 16) | 
                             (m_hwi_count << 8 ) | 
                             m_pti_count;
                    r_fsm  = RSP_READ;
                }
                else
                {
                    r_fsm = RSP_ERROR;
                }
            }  // end if cmdval
            break;
        }  
        case RSP_READ:
        case RSP_WRITE:
        case RSP_ERROR:
        {
            if ( p_vci.rspack.read() ) r_fsm = IDLE;
        }
	}  // end switch r_fsm

    // update timer interrupt vector
    for ( size_t i = 0; i<m_pti_count; ++i ) 
    {
        uint32_t period = r_pti_per[i].read();
        uint32_t value  = r_pti_val[i].read();
        if ( period )
        {
            if ( value == 1 ) 
            {
                r_pti_pending = r_pti_pending.read() | 1<<i;
                r_pti_val[i]  = period;
            }
            else
            {
                r_pti_val[i] = value - 1;
            }
        }
    }

    // update pending hardware interrupt vector 
    uint32_t hwi_pending = 0;
    for ( size_t i = 0; i<m_hwi_count; ++i )
        hwi_pending |= (p_hwi[i].read() ? 1 : 0) << i;
    r_hwi_pending = hwi_pending;

}  // end transition()

////////////////////////////////////////
tmpl(void)::print_trace( size_t detail )
{
    const char* fsm_str[] =
    {
        "IDLE",
        "RSP_READ",
        "RSP_WRITE",
        "RSP_ERROR",
    };
 
    std::cout << "XICU " << name() 
              << " / NB_HWI = " << m_hwi_count
              << " / NB_WTI = " << m_wti_count
              << " / NB_PTI = " << m_pti_count
              << " / NB_IRQ = " << m_irq_count
              << std::endl;

    std::cout << "  FSM = " << fsm_str[r_fsm.read()] << std::hex
              << " / PENDING_HWI = " << r_hwi_pending.read()
              << " / PENDING_WTI = " << r_wti_pending.read()
              << " / PENDING_PTI = " << r_pti_pending.read()
              << std::endl;

    if ( detail )
    {
        for ( size_t k = 0 ; k < m_irq_count ; k++ )
        {
            if ( r_msk_hwi[k].read() or r_msk_pti[k].read() or r_msk_wti[k].read() )
            std::cout << "  - channel " << std::dec << k << std::hex
                      << " : HWI_MASK = " << r_msk_hwi[k].read()
                      << " / WTI_MASK = " << r_msk_wti[k].read()
                      << " / PTI_MASK = " << r_msk_pti[k].read()
                      << std::endl;
        }
    } 
}

///////////////////////
tmpl(void)::genMoore()
{
    ////// p_vci port   
    p_vci.rsrcid = (sc_dt::sc_uint<vci_param::S>)r_srcid.read();
    p_vci.rtrdid = (sc_dt::sc_uint<vci_param::T>)r_trdid.read();
    p_vci.rpktid = (sc_dt::sc_uint<vci_param::P>)r_pktid.read();
    p_vci.reop   = true;

    switch( r_fsm.read() ) 
    {
        case IDLE:
	        p_vci.cmdack = true;
	        p_vci.rspval = false;
	        break;
        case RSP_READ:
	        p_vci.cmdack = false;
	        p_vci.rspval = true;
	        p_vci.rdata  = r_data.read();
	        p_vci.rerror = 0;
	        break;
        case RSP_WRITE:
	        p_vci.cmdack = false;
	        p_vci.rspval = true;
	        p_vci.rdata  = 0;
	        p_vci.rerror = 0;
	        break;
        case RSP_ERROR:
	        p_vci.cmdack = false;
	        p_vci.rspval = true;
	        p_vci.rdata  = 0;
	        p_vci.rerror = 1;
	        break;
    } 

    // output irqs
    for ( size_t i = 0; i<m_irq_count; ++i ) 
    {
        bool b = (r_msk_pti[i].read() & r_pti_pending.read()) ||
                 (r_msk_wti[i].read() & r_wti_pending.read()) ||
                 (r_msk_hwi[i].read() & r_hwi_pending.read()) ;

        p_irq[i] = b;
    }
}  // end genMoore()

//////////////////////////////////////////////////
tmpl(/**/)::VciXicu( sc_core::sc_module_name name,
                     const MappingTable      &mt,
                     const                   IntTab &index,
                     size_t                  pti_count,
                     size_t                  hwi_count,
                     size_t                  wti_count,
                     size_t                  irq_count )
           : caba::BaseModule(name),
           m_seglist( mt.getSegmentList(index) ),
           m_pti_count( pti_count ),
           m_hwi_count( hwi_count ),
           m_wti_count( wti_count ),
           m_irq_count( irq_count ),

           r_fsm( "r_fsm" ),
           r_data( "r_data" ),
           r_srcid( "r_srcid" ),
           r_trdid( "r_trdid" ),
           r_pktid( "r_pktid" ),
           r_msk_pti( alloc_elems<sc_signal<uint32_t> >("r_msk_pti" , irq_count) ),
           r_msk_wti( alloc_elems<sc_signal<uint32_t> >("r_msk_wti" , irq_count) ),
           r_msk_hwi( alloc_elems<sc_signal<uint32_t> >("r_msk_hwi" , irq_count) ),
           r_pti_pending( "r_pti_pending" ),
           r_wti_pending( "r_wti_pending" ),
           r_hwi_pending( "r_hwi_pending" ),
           r_pti_per( alloc_elems<sc_signal<uint32_t> >("r_pti_per" , pti_count) ),
           r_pti_val( alloc_elems<sc_signal<uint32_t> >("r_pti_val" , pti_count) ),
           r_wti_reg( alloc_elems<sc_signal<uint32_t> >("r_wti_reg" , wti_count) ),

           p_clk( "clk" ),
           p_resetn( "resetn" ),
           p_vci( "vci" ),
           p_irq( alloc_elems<sc_core::sc_out<bool> >("irq", irq_count) ),
           p_hwi( alloc_elems<sc_core::sc_in<bool> >("hwi", hwi_count) )
{
    std::cout << "  - Building VciXicu : " << name << std::endl;

    std::list<soclib::common::Segment>::iterator seg;
    for ( seg = m_seglist.begin() ; seg != m_seglist.end() ; seg++ )
    {
        std::cout << "    => segment " << seg->name()
                  << " / base = " << std::hex << seg->baseAddress()
                  << " / size = " << seg->size() << std::endl; 
    }
 
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

//////////////////////
tmpl(/**/)::~VciXicu()
{
    soclib::common::dealloc_elems<sc_core::sc_signal<uint32_t> >(r_msk_pti, m_irq_count); 
    soclib::common::dealloc_elems<sc_core::sc_signal<uint32_t> >(r_msk_wti, m_irq_count); 
    soclib::common::dealloc_elems<sc_core::sc_signal<uint32_t> >(r_msk_hwi, m_irq_count); 
    soclib::common::dealloc_elems<sc_core::sc_signal<uint32_t> >(r_pti_per, m_pti_count); 
    soclib::common::dealloc_elems<sc_core::sc_signal<uint32_t> >(r_pti_val, m_pti_count); 
    soclib::common::dealloc_elems<sc_core::sc_signal<uint32_t> >(r_wti_reg, m_wti_count); 
    soclib::common::dealloc_elems(p_irq, m_irq_count);
    soclib::common::dealloc_elems(p_hwi, m_hwi_count);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


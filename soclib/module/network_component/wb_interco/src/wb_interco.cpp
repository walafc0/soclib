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
 * Copyright (c) Telecom ParisTech
 *         Tarik Graba <tarik.graba@telecom-paristech.fr>, 2009
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 */

#include <cassert>
#include <iomanip>

#include "exception.h"
#include "wb_interco.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename wb_param> x WbInterco<wb_param>

    // constructor
    tmpl(/**/)::WbInterco ( sc_module_name name , const soclib::common::MappingTable &mtb,
                            const size_t &nb_m, const size_t &nb_s)
        : soclib::caba::BaseModule(name), m_masters_n(nb_m), m_slaves_n(nb_s), m_arbiter(nb_m)
    {
        assert( m_masters_n<=8 && "WB_INTERCON:: no more than 8 masters!!");
        assert( m_slaves_n <=8 && "WB_INTERCON:: no more than 8 slaves!!");

        if ( mtb.level() != 1 ) {
            std::stringstream o;
            o
                << "Mapping table level must be equal to \"1\"" << std::endl
                << " WbInterco is a flat interconnect!";
            throw soclib::exception::ValueError(o.str());
        }

        // get segment list from mapping table
        segts =  mtb.getAllSegmentList();

        p_to_slave    =
            _WbI_::alloc_named_o <soclib::caba::WbMaster<wb_param> >
            ("p_slave", m_slaves_n);
        p_from_master =
            _WbI_::alloc_named_o <soclib::caba::WbSlave<wb_param> >
            ("p_master", m_masters_n);

        granted = 0;

        SC_METHOD(transition);
        dont_initialize();
        sensitive << p_clk.pos();

        SC_METHOD(genMealy);
        dont_initialize();
        sensitive << p_clk.neg();
        for (size_t i=0; i< m_masters_n; i++) sensitive << p_from_master[i];
        for (size_t i=0; i< m_slaves_n ; i++) sensitive << p_to_slave[i];
        // TODO think of something simulation time less consuming

        std::cout 
            << name
            << " : created sucsessfully with "
            << m_masters_n << " masters and "
            << m_slaves_n  << " slaves"
            << std::endl;
    }

    // destructor
    tmpl(/**/)::~WbInterco ()
    {
        _WbI_::free_named_o <WbMaster<wb_param> > (p_to_slave   , m_slaves_n);
        _WbI_::free_named_o <WbSlave <wb_param> > (p_from_master, m_masters_n);
    }

    // get destination slave
    tmpl(size_t)::DestnationSlave( typename wb_param::wb_add_t addr )
    {
        std::list<soclib::common::Segment>::iterator seg_p;

        // find target segment
        for ( seg_p=segts.begin(); seg_p!=segts.end(); ++seg_p) 

            if (seg_p->contains(addr)) 

                // This is why we need a flat interconnect
                for ( size_t l = 0; l < m_slaves_n; l++ ) 
                    if( seg_p->index() == soclib::common::IntTab(l)) 
                        return l;

        // a valid slave dentifier should be less than m_slaves_n
        return m_slaves_n;
    }

    // transition on the rising edge
    tmpl(void)::transition()
    {
        if (!p_resetn) {
            // reset arbiter
            m_arbiter.reset();
        }
        else {
            // group requests
            unsigned int reqs = 0;
            for (size_t i=0; i< m_masters_n; i++)
                if (p_from_master[i].CYC_I) reqs = reqs | 1 << i ;
            // arbiter
            granted = m_arbiter.run(reqs);
#ifdef SOCLIB_MODULE_DEBUG
            std::cout
                << name()
                << " : Requests are "
                << std::hex << std::showbase
                << reqs
                << " & granted master is "
                << std::dec << std::noshowbase
                << granted
                << std::endl;
            //<< p_from_master[granted]
#endif
        }
    }// transition

    // Mealy for the bus multiplexors
    tmpl(void)::genMealy()
    {

        size_t dest_slave = DestnationSlave(p_from_master[granted].ADR_I);
#ifdef SOCLIB_MODULE_DEBUG
        std::cout
            << name()
            << " : destination slave is "
            << dest_slave
            << std::endl;
#endif
        // default values
        // masters inputs
        for (size_t i=0; i< m_masters_n; i++)
        {
            p_from_master[i].DAT_O = 0;
            p_from_master[i].ACK_O = false;
            p_from_master[i].ERR_O = false;
            p_from_master[i].RTY_O = false;
        }
        // slaves inputs
        for (size_t i=0; i< m_slaves_n; i++)
        {
            p_to_slave[i]. DAT_O = 0;
            p_to_slave[i]. ADR_O = 0;
            p_to_slave[i]. CYC_O = false;
            p_to_slave[i].LOCK_O = false;
            p_to_slave[i]. SEL_O = 0;
            p_to_slave[i]. STB_O = false;
            p_to_slave[i].  WE_O = false;
        }

        if (dest_slave == m_slaves_n)
        {
#ifdef SOCLIB_MODULE_DEBUG
            std::cout
                << name()
                << ": Address "
                << std::hex  << std::setfill(' ') << std::setw(8)
                << p_from_master[granted].ADR_I
                << " is out of reach"
                << std::endl;
#endif
            p_from_master[granted].ERR_O = true;
        }
        else {
            // connect granted master to destination slave
            p_to_slave[dest_slave]. DAT_O = p_from_master[granted]. DAT_I;
            p_to_slave[dest_slave]. ADR_O = p_from_master[granted]. ADR_I;
            p_to_slave[dest_slave]. CYC_O = p_from_master[granted]. CYC_I;
            p_to_slave[dest_slave].LOCK_O = p_from_master[granted].LOCK_I;
            p_to_slave[dest_slave]. SEL_O = p_from_master[granted]. SEL_I;
            p_to_slave[dest_slave]. STB_O = p_from_master[granted]. STB_I;
            p_to_slave[dest_slave].  WE_O = p_from_master[granted].  WE_I;

            p_from_master[granted].DAT_O = p_to_slave[dest_slave].DAT_I;
            p_from_master[granted].ACK_O = p_to_slave[dest_slave].ACK_I;
            p_from_master[granted].ERR_O = p_to_slave[dest_slave].ERR_I;
            p_from_master[granted].RTY_O = p_to_slave[dest_slave].RTY_I;
        }

    }// mealy

#undef tmpl

}} // namespace

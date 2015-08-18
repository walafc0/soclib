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

#ifndef _WB_INTERCON_H_
#define _WB_INTERCON_H_

#include <systemc>

#include <sstream>
#include <cstdlib>

#include "caba_base_module.h"

#include "mapping_table.h"

#include "wb_master.h"
#include "wb_slave.h"

#include "arbiter.h"

namespace soclib { namespace caba {

    template<typename wb_param>
        class WbInterco
        : public soclib::caba::BaseModule
        {
            public:
                sc_core::sc_in<bool> p_clk;
                sc_core::sc_in<bool> p_resetn;

                // master ports to connect slaves
                soclib::caba::WbMaster<wb_param> *p_to_slave;
                // slave ports to connect masters
                soclib::caba::WbSlave <wb_param> *p_from_master;

            private:
                const size_t m_masters_n;
                const size_t m_slaves_n;

                // the soclib methods
                void transition();
                void genMealy();

                _WbI_::Arbiter m_arbiter;
                unsigned int granted;

                std::list<soclib::common::Segment> segts;

                // returns the destination slave
                // if the address is out all slaves ranges returns the number of slaves
                size_t DestnationSlave( typename wb_param::wb_add_t addr );

            protected:
                SC_HAS_PROCESS(WbInterco);
            public:
                WbInterco( sc_module_name name,
                        const soclib::common::MappingTable &mtb,
                        const size_t &nb_m,
                        const size_t &nb_s
                        );
                ~WbInterco();
        };

    namespace _WbI_ { // private stuff

        template<typename obj>
            obj* alloc_named_o (const std::string &name, const size_t nb)
            {
                // alloc space for the objects static content
                obj *tab = (obj*) malloc(sizeof(obj)*nb);
                for (size_t i=0; i<nb; i++)
                {
                    std::ostringstream o;
                    // forge a name
                    o << name << "[" << i << "]";
                    // call each object constructor
                    // passing the forged name
                    new(&tab[i]) obj(o.str().c_str());
                }
                return tab;
            }

        template<typename obj>
            void free_named_o (obj *tab, const size_t nb)
            {
                for (size_t i=0;i<nb;i++)
                {
                    // call each object destructor
                    tab[i].~obj();
                }
                // free static space
                free (tab);
            }
    } // private namespace

}} // namespace

#endif //_WB_INTERCON_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors : Ivan MIRO PANADES
 * 
 * History :
 *
 * Comment :
 *
 */

// define node wait time (~T/2)
#define TB_NODE_WAIT 5
// define clock period for resource
#define TB_RES_CLK_PERIOD 5 // must be > 2

#include <sstream>
#include <cassert>
#include "alloc_elems.h"
#include "../include/vci_anoc_network.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, int anoc_fifo_size, int anoc_yx_size> x VciAnocNetwork<vci_param, anoc_fifo_size, anoc_yx_size>

    ////////////////////////////////
    //      constructor
    //
    ////////////////////////////////

    tmpl(/**/)::VciAnocNetwork(sc_module_name insname,
                               const soclib::common::MappingTable &mt,
                               size_t width_network,
                               size_t height_network) 
                                : soclib::caba::BaseModule(insname),
                                  m_width_network(width_network),
                                  m_height_network(height_network)
    {
        assert( m_width_network <= 15 && m_height_network <= 15 );
        if (m_width_network+m_height_network-1 >9) {
            printf("The network can not be implemented. width_network + height_network should be <= 9\n");
            sc_stop();
        }


        //
        // VCI_Interfaces
        //
        p_to_initiator = soclib::common::alloc_elems<soclib::caba::VciTarget<vci_param> >   ("p_to_initiator", m_height_network, m_width_network);
        p_to_target    = soclib::common::alloc_elems<soclib::caba::VciInitiator<vci_param> >("p_to_target",    m_height_network, m_width_network);

        anoc_router      = new anoc_node**[m_height_network];
        vci_anoc_wrapper = new VciAnocWrapper<vci_param, anoc_fifo_size, anoc_yx_size>**[m_height_network];

        empty_north = new anoc_stopper*[m_width_network];
        empty_south = new anoc_stopper*[m_width_network];
        empty_east  = new anoc_stopper*[m_height_network];
        empty_west  = new anoc_stopper*[m_height_network];

        //Build the network grid
        for(int y = 0; y < m_height_network; y++){
            anoc_router[y] = new anoc_node*[m_width_network];
            vci_anoc_wrapper[y] = new VciAnocWrapper<vci_param, anoc_fifo_size, anoc_yx_size>*[m_width_network];

            for(int x = 0; x < m_width_network; x++){
                char str_anoc_node[30];
                sprintf(str_anoc_node, "anoc_node[%i][%i]", y,x);
		        anoc_router[y][x] = new anoc_node(str_anoc_node, TB_NODE_WAIT);

                char str_vci_anoc_wrapper[30];
                sprintf(str_vci_anoc_wrapper, "vci_anoc_wrapper[%i][%i]", y,x);
                vci_anoc_wrapper[y][x] = new VciAnocWrapper<vci_param, anoc_fifo_size, anoc_yx_size>(str_vci_anoc_wrapper, mt, y, x);
            }
        }

        //Build the stoper modules for the east and west side
        for(int y = 0; y < m_height_network; y++){
            char str_empty_east[30];
            char str_empty_west[30];
            sprintf(str_empty_east, "empty_east[%i]", y);
            sprintf(str_empty_west, "empty_west[%i]", y);

            empty_east[y] = new anoc_stopper(str_empty_east, y                 , TB_RES_CLK_PERIOD, TB_NODE_WAIT);
            empty_west[y] = new anoc_stopper(str_empty_west, y+m_height_network, TB_RES_CLK_PERIOD, TB_NODE_WAIT);
        }

        //Build the stoper modules for the north and south side
        for(int x = 0; x < m_width_network; x++){
            char str_empty_north[30];
            char str_empty_south[30];
            sprintf(str_empty_north, "empty_north[%i]", x);
            sprintf(str_empty_south, "empty_south[%i]", x);

            empty_north[x] = new anoc_stopper(str_empty_north, x+m_height_network*2                , TB_RES_CLK_PERIOD, TB_NODE_WAIT);
            empty_south[x] = new anoc_stopper(str_empty_south, x+m_height_network*2+m_width_network, TB_RES_CLK_PERIOD, TB_NODE_WAIT);

        }


        //Bind the east and west stopper modules
        for(int y = 0; y < m_height_network; y++){
            empty_east[y]->noc_out->bind(anoc_router[y][m_width_network-1]->node_in[EAST]);
            anoc_router[y][m_width_network-1]->node_out[EAST]->bind(empty_east[y]->noc_in);

            empty_west[y]->noc_out->bind(anoc_router[y][0]->node_in[WEST]);
            anoc_router[y][0]->node_out[WEST]->bind(empty_west[y]->noc_in);
        }

        //Bind the north and south stopper modules
        for(int x = 0; x < m_width_network; x++){
            empty_north[x]->noc_out->bind(anoc_router[0][x]->node_in[NORTH]);
            anoc_router[0][x]->node_out[NORTH]->bind(empty_north[x]->noc_in);

            empty_south[x]->noc_out->bind(anoc_router[m_height_network-1][x]->node_in[SOUTH]);
            anoc_router[m_height_network-1][x]->node_out[SOUTH]->bind(empty_south[x]->noc_in);
        }

        //Bind the inter router wires
        for(int y = 0; y < m_height_network; y++){
            for(int x = 0; x < m_width_network; x++){
                if (x < m_width_network-1) {
                    anoc_router[y][x]->node_out[EAST]   ->bind(anoc_router[y][x+1]->node_in[WEST]);
                    anoc_router[y][x+1]->node_out[WEST] ->bind(anoc_router[y][x]->node_in[EAST]);
                }
                if (y < m_height_network-1) {
                    anoc_router[y][x]->node_out[SOUTH]   ->bind(anoc_router[y+1][x]->node_in[NORTH]);
                    anoc_router[y+1][x]->node_out[NORTH] ->bind(anoc_router[y][x]->node_in[SOUTH]);
                }
            }
        }

        //Bind the resource
        for(int y = 0; y < m_height_network; y++){
		    for(int x = 0; x < m_width_network; x++){
                printf("Bind vci_anoc_wrapper[%i][%i]\n", y, x);            
                vci_anoc_wrapper[y][x]->noc_out->bind(anoc_router[y][x]->node_in[RES]);
                anoc_router[y][x]->node_out[RES]->bind(vci_anoc_wrapper[y][x]->noc_in);

                vci_anoc_wrapper[y][x]->I_p_vci(p_to_initiator[y][x]);
                vci_anoc_wrapper[y][x]->T_p_vci(p_to_target[y][x]);

                vci_anoc_wrapper[y][x]->p_clk(p_clk);
                vci_anoc_wrapper[y][x]->p_resetn(p_resetn);
            }
        }

    }


    tmpl(/**/)::~VciAnocNetwork()
    {
        for(int y = 0; y < m_height_network; y++){
            for(int x = 0; x < m_width_network; x++){
                delete anoc_router[y][x];
                delete vci_anoc_wrapper[y][x];
            }
            delete [] anoc_router[y];
            delete [] vci_anoc_wrapper[y];
        }
        delete [] anoc_router;
        delete [] vci_anoc_wrapper;

        for(int y = 0; y < m_height_network; y++){
            delete empty_east[y];
            delete empty_west[y];
        }
        delete [] empty_east;
        delete [] empty_west;

        for(int x = 0; x < m_width_network; x++){
            delete empty_north[x];
            delete empty_south[x];
        }
        delete [] empty_north;
        delete [] empty_south;
        
		soclib::common::dealloc_elems( p_to_initiator, m_height_network, m_width_network);
		soclib::common::dealloc_elems( p_to_target,    m_height_network, m_width_network);
        
    }
}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

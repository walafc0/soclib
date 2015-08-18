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
 * Copyright (c) UPMC, Lip6, SoC
 *         Mohamed Lamine Karaoui <Mohamed.Karaoui@lip6.fr>, 2012
 *
 */
#ifndef SOCLIB_VLOADER_H_
#define SOCLIB_VLOADER_H_

#include  <stdlib.h>
#include  <cstdlib>
#include  <fcntl.h>
#include  <unistd.h>
#include  <cstdio>
#include  <string.h>
//#include  <libxml/xmlreader.h>

#include <string>
#include <ios>
#include <vector>
#include <map>
#include <fstream>

#include "stdint.h"
#include "pseg_handler.h"
#include "loader.h"

namespace soclib { namespace common {

class Loader;
class PSegHandler;

class VLoader:public Loader//to override the virtual methode "load" defined in the Loader
{
    friend class Loader;

    std::string m_path;         //map_info path name
    std::string m_wd;           //map_info path to directory TODO: make the name defined in the map_info relative to this wd.
    std::string m_simpleName;   //map_info filename TODO
    void* m_data;               //map_info structure 
    uintptr_t m_addr;           //map_info address (virtual)
    size_t m_size;              //size of the structure
    PSegHandler m_psegh;
    mutable std::map<std::string, Loader> m_loaders;

    void* load_bin(std::string name);

public:

    VLoader( const std::string &name, const size_t pageSize = 4096);
    ~VLoader();

	void load( void *buffer, uintptr_t address, size_t length ) const;

    void print( std::ostream &o ) const;

    friend std::ostream &operator<<( std::ostream &o, const VLoader &s )
    {
        s.print(o);
        return o;
    }


    //The following functions handle the map.bin structure
    //inspired from the boot_handler.c of the GIET
    mapping_pseg_t* get_pseg_base( mapping_header_t* header );
    mapping_vspace_t* get_vspace_base( mapping_header_t* header );
    mapping_vseg_t* get_vseg_base( mapping_header_t* header );
    mapping_vobj_t* get_vobj_base( mapping_header_t* header );
    void print_mapping_info(void* desc);
    void pseg_map( mapping_pseg_t* pseg);
    void vseg_map( mapping_vseg_t* vseg);
    void buildMap(void* desc);

};

}}

#endif /* SOCLIB_VLOADER_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


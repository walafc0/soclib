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
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 */

#include <cassert>
#include <sstream>
#include <fstream>
#include <ios>
#include <cstdlib>

#include "exception.h"
#include "static_init_code.h"
#include "loader.h"

namespace soclib { namespace common { namespace {

static bool _plain_load( const std::string &filename, Loader &loader )
{
    std::string::size_type at = filename.find('@');
    if ( at == std::string::npos )
        return false;
    std::string::size_type colon = filename.find(':', at+1);
    if ( colon == std::string::npos )
        return false;
    std::string name = filename.substr(0, at);
    std::string address = filename.substr(at+1, colon-at-1);
    std::string flags = filename.substr(colon+1);

    std::cout
        << "Trying to load a plain blob from file '" << name << "'"
        << " @ " << address
        << " flags: " << flags
        << std::endl;

    std::ifstream input(name.c_str(), std::ios_base::binary|std::ios_base::in);
    
    if ( ! input.good() )
        throw soclib::exception::RunTimeError("Bad file");

    input.seekg( 0, std::ifstream::end );
    size_t size = input.tellg();
    input.seekg( 0, std::ifstream::beg );

    void *data = std::malloc(size);
    if ( !data )
        return false;
    input.read( (char*)data, size );

    uintptr_t addr;
    std::istringstream( address ) >> std::hex >> addr;

    uint32_t bflags = BinaryFileSection::FLAG_LOAD;
    if ( flags.find("RO") != std::string::npos )
        bflags |= BinaryFileSection::FLAG_READONLY;
    if ( flags.find("X") != std::string::npos )
        bflags |= BinaryFileSection::FLAG_CODE;
    if ( flags.find("D") != std::string::npos )
        bflags |= BinaryFileSection::FLAG_DATA;

    std::cout
        << "Loading a plain blob from file '" << name << "'"
        << " @ " << std::hex << addr
        << ", size: " << std::dec << size << " bytes, "
        << " flags: " << bflags
        << std::endl;

    loader.addSection(BinaryFileSection(
                          name,
                          addr, addr,
                          bflags,
                          size, data ));

    loader.addSymbol(BinaryFileSymbol(
                         name, addr, size ));



	return true;
}

static bool plain_load( const std::string &filename, Loader &loader )
{
    return _plain_load(filename, loader);
}

STATIC_INIT_CODE(
	Loader::register_loader("plain", plain_load);
)

}}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


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
 */

#include <cstdio>
#include <string>
#include <sstream>

#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "string.h"

#include "soclib_endian.h"
#include "exception.h"
#include "fb_controller.h"

namespace soclib {
namespace common {

FbController::FbController(
    const std::string &basename,
    unsigned long width,
    unsigned long height,
    int subsampling )
	: m_width(width),
      m_height(height),
      m_subsampling((enum SubsamplingType)subsampling),
      m_surface_size(surface_size())
{
	char name[32];
	m_sim_surface = std::malloc(m_surface_size);
	m_surface = NULL;

    std::string tmpname = std::string("/tmp/") + basename + ".rawXXXXXX";

    std::strncpy( name, tmpname.c_str(), sizeof(name));
	m_map_fd = mkstemp(name);
	if ( m_map_fd < 0 ) {
		perror("open");
        throw soclib::exception::RunTimeError("Cant create file");
	}

	lseek(m_map_fd, m_surface_size-1, SEEK_SET);
	write(m_map_fd, "", 1);
	lseek(m_map_fd, 0, SEEK_SET);
	
	m_surface = (uint32_t*)mmap(0, m_surface_size, PROT_WRITE|PROT_READ, MAP_FILE|MAP_SHARED, m_map_fd, 0);
	if ( m_surface == ((uint32_t *)-1) ) {
		perror("mmap");
        throw soclib::exception::RunTimeError("Cant mmap file");
	}
	
    switch ( m_subsampling ) {
    case YUV420:
    case YUV422:
        std::memset(m_surface, 128, m_surface_size);
        std::memset(m_sim_surface, 128, m_surface_size);
        break;
    case RGB:
    case RGB_16:
    case RGB_32:
    case BW:
    case RGB_PALETTE_256:
        std::memset(m_surface, 0, m_surface_size);
        std::memset(m_sim_surface, 0, m_surface_size);
        break;
    }

    char *soclib_fb = std::getenv("SOCLIB_FB");
    m_headless_mode = ( soclib_fb && !std::strcmp(soclib_fb, "HEADLESS") );
    
	if (m_headless_mode == false) {
        std::vector<std::string> argv;
        std::ostringstream o;

        argv.push_back("soclib-fb");
        o << m_width;
        argv.push_back(o.str());
        o.str("");
        o << m_height;
        argv.push_back(o.str());
        o.str("");
        o << m_subsampling;
        argv.push_back(o.str());
        argv.push_back(name);

        m_screen_process = new ProcessWrapper("soclib-fb", argv);
//         std::cout << "Wait..." ;
//         std::cout.flush();
//         sleep(2);
//         std::cout << "Done" << std::endl;
	}
}

FbController::~FbController()
{
	if (m_headless_mode == false)
        delete m_screen_process;
    std::free(m_sim_surface);
}

size_t FbController::surface_size() const
{
    switch ( m_subsampling ) {
    case YUV420:
        return (m_width*m_height) * 3 / 2;
    case YUV422:
        return (m_width*m_height) * 2;
    case RGB:
        return (m_width*m_height) * 3;
    case RGB_16:
        return (m_width*m_height) * 2;
    case RGB_32:
        return (m_width*m_height) * 4;
    case RGB_PALETTE_256:
        return (m_width*m_height) + 3*256;
    case BW:
        return (m_width*m_height)/8;
    default:
        return 0;
    }
}

void FbController::update()
{
	if (m_headless_mode == false) {
        memcpy(m_surface, m_sim_surface, m_surface_size);
        m_screen_process->write("", 1);
    }
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


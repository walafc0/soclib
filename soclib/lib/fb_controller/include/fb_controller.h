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
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_COMMON_FB_CONTROLLER_H_
#define SOCLIB_COMMON_FB_CONTROLLER_H_

#include <assert.h>
#include <inttypes.h>
#include <cassert>
#include "process_wrapper.h"

namespace soclib { namespace common {

class FbController
{
	int m_map_fd;
    bool m_headless_mode;
    soclib::common::ProcessWrapper *m_screen_process;

	uint32_t *m_surface;
	void *m_sim_surface;

public:
    size_t surface_size() const;

    enum SubsamplingType {
        YUV420 = 420,
        YUV422 = 422,
        RGB = 0,
        RGB_16 = 16,
        RGB_32 = 32,
        RGB_PALETTE_256 = 256,
        BW = 1,
    };

	const unsigned long m_width, m_height;
    const enum SubsamplingType m_subsampling;
    const size_t m_surface_size;

    inline uint32_t* surface() const
    {
        return (uint32_t*)m_sim_surface;
    }

    template<typename T>
    inline T& w( size_t offset )
    {
        if ( offset <= (m_surface_size + sizeof(T) - 1) / sizeof(T) )
            return ((T*)m_sim_surface)[offset];
        return ((T*)m_sim_surface)[0];
    }

	FbController(
		const std::string &basename,
		unsigned long width,
		unsigned long height,
        int subsampling = 420);

	~FbController();

	void update();
};

}}

#endif /* SOCLIB_COMMON_FB_CONTROLLER_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


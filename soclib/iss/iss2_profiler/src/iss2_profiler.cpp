/*
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
 * Copyright (c) 2009, UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>
 *
 * Maintainers: nipo
 */

#include "iss2_profiler.h"

namespace soclib { namespace common {

template<typename CpuIss>
Iss2Profiler<CpuIss>::Iss2Profiler(const std::string & name, uint32_t ident)
    : CpuIss(name, ident),
      m_log(name.c_str()),
      m_did_req_pc(false),
      m_did_req_data(false)
{
}

template<typename CpuIss>
Iss2Profiler<CpuIss>::~Iss2Profiler()
{
    m_log.close();
}

}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

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
 * Copyright (c) 2008, UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>
 *
 * Maintainers: nipo
 */

#include "iss_profiler.h"

namespace soclib { namespace common {

static inline std::string mkname(uint32_t no)
{
    char tmp[32];
    snprintf(tmp, 32, "iss_profiler_%d.log", (int)no);
    return std::string(tmp);
}

template<typename CpuIss>
IssProfiler<CpuIss>::IssProfiler(uint32_t ident)
    : CpuIss(ident),
      m_log(mkname(ident).c_str()),
      m_did_req_pc(false),
      m_last_pc(0)
{
}

template<typename CpuIss>
IssProfiler<CpuIss>::~IssProfiler()
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

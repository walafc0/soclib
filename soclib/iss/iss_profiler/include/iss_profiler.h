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
 * Copyright (c) 2008, UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>
 *
 * Maintainers: nipo
 *
 * $Id$
 *
 */

#ifndef _SOCLIB_ISS_PROFILER_ISS_H_
#define _SOCLIB_ISS_PROFILER_ISS_H_

#include <stdint.h>
#include <fstream>
#include <iomanip>
#include "iss.h"

namespace soclib { namespace common {

template<typename CpuIss>
class IssProfiler
    : public CpuIss
{
    std::ofstream m_log;
    bool m_did_req_pc;
    uint32_t m_last_pc;
public:

    IssProfiler(uint32_t ident);
    ~IssProfiler();

    inline void log( bool running )
    {
        m_log << (running ? "R " : "F ")
              << (m_did_req_pc ? "+ " : "- ")
              << std::showbase << std::setw(8) << std::hex << std::setfill('0')
              << m_last_pc
              << std::endl;
    }

    inline void step()
    {
        log(true);
        CpuIss::step();
    }

    inline void nullStep( uint32_t i=1 )
    {
        log(false);
        CpuIss::nullStep(i);
    }

    inline void getInstructionRequest(bool &req, uint32_t &address) const
	{
        CpuIss::getInstructionRequest(req, address);
        ((IssProfiler<CpuIss>*)this)->m_did_req_pc = req;
        ((IssProfiler<CpuIss>*)this)->m_last_pc = address;
	}
};

}}

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

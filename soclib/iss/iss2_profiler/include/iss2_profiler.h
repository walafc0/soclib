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
 * $Id: iss2_profiler.h 1393 2009-11-07 13:08:31Z nipo $
 *
 */

#ifndef _SOCLIB_ISS2_PROFILER_ISS_H_
#define _SOCLIB_ISS2_PROFILER_ISS_H_

#include <stdint.h>
#include <fstream>
#include <iomanip>
#include "iss2.h"

namespace soclib { namespace common {

template<typename CpuIss>
class Iss2Profiler
    : public CpuIss
{
    std::ofstream m_log;
    bool m_did_req_pc;
    bool m_did_req_data;
public:

    Iss2Profiler(const std::string & name, uint32_t ident);
    ~Iss2Profiler();

    inline void log( bool running, uint32_t count )
    {
        m_log << std::dec << std::noshowbase << count
			  << (running ? " R " : " F ")
              << (m_did_req_pc ? "+ " : "- ")
              << std::showbase << std::setw(8) << std::hex << std::setfill('0')
              << CpuIss::debugGetRegisterValue(CpuIss::s_pc_register_no)
              << std::endl;
    }

    inline uint32_t executeNCycles(uint32_t ncycle,
								   const struct CpuIss::InstructionResponse &irsp,
								   const struct CpuIss::DataResponse &drsp,
								   uint32_t irq)
    {
		uint32_t ret = CpuIss::executeNCycles(ncycle, irsp, drsp, irq);
		bool ran = (!m_did_req_pc || irsp.valid) && (!m_did_req_data || drsp.valid);
		log(ran, ret);
		return ret;
    }

    inline void getRequests(struct CpuIss::InstructionRequest &ireq,
							struct CpuIss::DataRequest &dreq) const
	{
        CpuIss::getRequests(ireq, dreq);
        const_cast<Iss2Profiler<CpuIss>*>(this)->m_did_req_pc = ireq.valid;
        const_cast<Iss2Profiler<CpuIss>*>(this)->m_did_req_data = dreq.valid;
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

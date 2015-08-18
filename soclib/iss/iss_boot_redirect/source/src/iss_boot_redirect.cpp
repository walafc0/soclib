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
 * Copyright (c) 2010, UPMC, Lip6
 *         Eric Guthmuller
 *
 * Maintainers: guth
 */

#include "iss_boot_redirect.h"

namespace soclib { namespace common {


template<typename CpuIss>
std::map<uint32_t, typename CpuIss::addr_t> IssBootRedirect<CpuIss>::s_reset_address;

template<typename CpuIss>
typename CpuIss::addr_t IssBootRedirect<CpuIss>::s_default_reset_address = (typename CpuIss::addr_t)-1;

template<typename CpuIss>
IssBootRedirect<CpuIss>::IssBootRedirect(const std::string &name, uint32_t ident)
    : CpuIss(name, ident)
{
}

template<typename CpuIss>
IssBootRedirect<CpuIss>::~IssBootRedirect()
{
}

template<typename CpuIss>
typename CpuIss::addr_t IssBootRedirect<CpuIss>::reset_address()
{
    if ( s_reset_address.find(CpuIss::m_ident) != s_reset_address.end() )
        return s_reset_address[CpuIss::m_ident];
    return s_default_reset_address;
}

template<typename CpuIss>
void IssBootRedirect<CpuIss>::reset()
{
    typename CpuIss::addr_t addr = reset_address();
    typename CpuIss::InstructionResponse irsp = ISS_IRSP_INITIALIZER;
    typename CpuIss::DataResponse drsp = ISS_DRSP_INITIALIZER;

    assert(addr != ((typename CpuIss::addr_t)-1) && "Invalid reset address");

    CpuIss::reset();
    CpuIss::debugSetRegisterValue(CpuIss::s_pc_register_no, addr);
    CpuIss::executeNCycles(0,irsp,drsp,0);
}

template<typename CpuIss>
void IssBootRedirect<CpuIss>::set_reset_address(uint32_t ident, typename CpuIss::addr_t address)
{
    s_reset_address[ident] = address;
}

template<typename CpuIss>
void IssBootRedirect<CpuIss>::set_default_reset_address(typename CpuIss::addr_t address)
{
    s_default_reset_address = address;
}
}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

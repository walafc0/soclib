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
 * Copyright (c) 2010, UPMC, Lip6
 *         Eric Guthmuller
 *
 * Maintainers: guth
 *
 * $Id$
 *
 */

#ifndef _SOCLIB_ISS_BOOT_REDIRECT_ISS_H_
#define _SOCLIB_ISS_BOOT_REDIRECT_ISS_H_

#include <stdint.h>
#include <map>
#include "iss2.h"

namespace soclib { namespace common {

template<typename CpuIss>
class IssBootRedirect
    : public CpuIss
{
    static std::map<uint32_t, typename CpuIss::addr_t> s_reset_address;
    static typename CpuIss::addr_t s_default_reset_address;

    typename CpuIss::addr_t reset_address();

public:

    IssBootRedirect(const std::string &name, uint32_t ident);
    ~IssBootRedirect();

    void reset();

    static void set_reset_address(uint32_t ident, typename CpuIss::addr_t address);
    static void set_default_reset_address(typename CpuIss::addr_t address);
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

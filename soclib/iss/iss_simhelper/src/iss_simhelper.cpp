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

#include <iostream>
#include <systemc>

#include "iss_simhelper.h"

namespace soclib { namespace common {

template<typename iss_t>
bool IssSimhelper<iss_t>::exceptionBypassed( uint32_t cause )
{
    int signal = iss_t::cpuCauseToSignal(cause);
    if ( signal == 5 )
        sc_core::sc_stop();
    return iss_t::exceptionBypassed(cause);
}

template<typename iss_t>
IssSimhelper<iss_t>::~IssSimhelper()
{
}

}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

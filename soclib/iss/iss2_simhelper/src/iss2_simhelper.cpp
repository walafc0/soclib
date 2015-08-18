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

#include "iss2_simhelper.h"

namespace soclib { namespace common {

template<typename iss_t>
bool Iss2Simhelper<iss_t>::debugExceptionBypassed( Iss2::ExceptionClass cl, Iss2::ExceptionCause ca )
{
    switch ( cl ) {
    default:
        return iss_t::debugExceptionBypassed( cl, ca );

    case Iss2::EXCL_TRAP:
    case Iss2::EXCL_FAULT:
        // Sparc window overflow / underflow traps should never cause a simulation stop,
        // as this is a perfectly normal condition...
        if (ca==Iss2::EXCA_REGWINDOW)
            return false;
        sc_core::sc_stop();
        return true;
    }
}

template<typename iss_t>
Iss2Simhelper<iss_t>::~Iss2Simhelper()
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

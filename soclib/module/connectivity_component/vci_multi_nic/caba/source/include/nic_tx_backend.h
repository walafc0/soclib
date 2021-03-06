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
 * Copyright (c) UPMC, Lip6
 *         Sylvain Leroy <sylvain.leroy@lip6.fr>
 *
 * Maintainers: sylvain
 */

#ifndef SOCLIB_CABA_BACKEND_TX_H
#define SOCLIB_CABA_BACKEND_TX_H

#include <inttypes.h>
#include <systemc>
#include <assert.h>

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>

#define PREAMBLE_SIZE 8
#define PREAMBLE "55555555555555D5"

namespace soclib {
namespace caba {

//////////////////
class NicTxBackend
{
    virtual void write_one_packet() = 0;

public:

    virtual void reset() = 0;

    virtual void put(bool dv, uint8_t dt) = 0;

}; // end NicBackendTx

} /* caba */
} /* soclib */

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4




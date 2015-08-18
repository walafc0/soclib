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
 * Copyright (C) IRISA/INRIA, 2007-2008
 *         Francois Charot <charot@irisa.fr>
 * 	   Charles Wagner <wagner@irisa.fr>
 * 
 * Maintainer: wagner
 * 
 */
#ifndef SOCLIB_CABA_AVALON_PARAM_H_
#define SOCLIB_CABA_AVALON_PARAM_H_

#include <systemc>

namespace soclib { namespace caba {

using namespace sc_core;

/**
 * AVALON parameters grouped in a single class
 */
template<
    int AD_Width, 
    int D_Width, 
    int BC_Width    >

class AvalonParams
{
public:
    /* Standard's constants, may be used by some modules */
    static const int address_width = AD_Width;
    static const int data_width = D_Width;
    static const int burstcount_width = BC_Width;
};

}}

#endif /* SOCLIB_CABA_AVALON_PARAM_H_ */


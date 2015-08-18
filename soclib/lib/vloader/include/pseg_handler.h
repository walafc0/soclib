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
 * Copyright (c) UPMC, Lip6, SoC
 *         Mohamed Lamine Karaoui <Mohamed.Karaoui@lip6.fr>, 2012
 *          
 */
#ifndef SOCLIB_PSEG_HANDLER_H_
#define SOCLIB_PSEG_HANDLER_H_

#include <string>
#include <vector>
#include <ios>

#include "stdint.h"
#include "mapping_info.h"
#include "pseg.h"

namespace soclib { namespace common {

class VLoader;

class PSegHandler
{
    friend class VLoader;

    std::vector <PSeg> m_pSegs;

public:

    PSeg& get( size_t pos  );
    const PSeg& getByAddr( uintptr_t segAddress ) const ;

    //do some checking on all the Psegs. Called once the m_psegs is build.
    void check() const;

    void print( std::ostream &o ) const;

    friend std::ostream &operator<<( std::ostream &o, const PSegHandler &s )
    {
        s.print(o);
        return o;
    }

    PSegHandler();
    ~PSegHandler();
};

}}

#endif /* SOCLIB_PSEG_HANDLER_H_ */

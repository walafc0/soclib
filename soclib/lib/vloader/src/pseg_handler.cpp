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
 */

#include <algorithm>
#include <string.h>
#include <cassert>
#include <cstring>
#include <stdexcept>

#include <iostream>
#include <iomanip>

#include "pseg_handler.h"
#include "exception.h"

namespace soclib { namespace common {


/*
 * PSegHandler
 */

PSeg& PSegHandler::get( size_t pos  )
{
    try
    {
        return m_pSegs.at( pos );

    }catch (std::out_of_range& oor) {
        std::cerr << "PSeg "<< pos <<"(id) is missing" << std::endl;
    }
    exit(1);
}

const PSeg& PSegHandler::getByAddr(uintptr_t segAddress) const
{
    std::vector<PSeg>::const_iterator it ;
    for(it = m_pSegs.begin(); it < m_pSegs.end(); it++)
    {
        uintptr_t lma = (*it).lma();
        if( lma == segAddress )
            return *it;
    }

    std::cerr << "Unfound Physical segment: " 
              << std::hex << std::showbase 
              << segAddress << std::endl;
    exit(1);
}

void PSegHandler::check() const
{
    std::vector<PSeg>::const_iterator it ;
    for(it = m_pSegs.begin(); it < m_pSegs.end(); it++)
    {
        (*it).check();
    }
}

void PSegHandler::print( std::ostream &o ) const
{
    o << std::dec ; 
	o << "< " << m_pSegs.size() <<  " Segments (page size = " << PSeg::pageSize() << ") :"<< std::endl;
    std::vector<PSeg>::const_iterator it ;
    for(it = m_pSegs.begin(); it < m_pSegs.end(); it++)
        o << *it << std::endl;
    o << ">";
}

PSegHandler::~PSegHandler()
{
//    std::cout << "Deleted PSegHandler " << *this << std::endl;
}

PSegHandler::PSegHandler()
{
    //std::cout << "New empty PSegHandler " << *this << std::endl;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


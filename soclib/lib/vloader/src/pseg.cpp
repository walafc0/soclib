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
#include <sstream>
#include <iomanip>

#include "pseg.h"
#include "exception.h"

namespace soclib { namespace common {

/*
 * VSeg
 */

const std::string & VSeg::name() const
{
	return m_name;
}

const std::string & VSeg::file() const
{
	return m_file;
}

uintptr_t VSeg::vma() const
{
	return m_vma;
}

uintptr_t VSeg::lma() const
{
	return m_lma;
}

uintptr_t VSeg::length() const
{
	return m_length;
}

void VSeg::print( std::ostream &o ) const
{
	o << std::hex << std::noshowbase 
      << "<Virtual segment from(vaddr): 0x" 
      << std::setw (8) << std::setfill('0') 
      << m_vma << ", to(paddr) 0x"
      << std::setw (8) << std::setfill('0') 
      << m_lma << ", size: 0x"
      << std::setw (8) << std::setfill('0') 
      << m_length  << ",ident: " 
      << (m_ident ? "yes" : "no") << ", in(file): "
      << m_file << ", name: " << m_name << ">";
}

VSeg::~VSeg()
{
//    std::cout << "Deleted VSeg " << *this << std::endl;
}

VSeg & VSeg::operator=( const VSeg &ref )
{
    if ( &ref == this )
        return *this;

    //std::cout << "Copying " << ref << " to " << *this << std::endl;
    m_name = ref.m_name,
    m_file = ref.m_file;
    m_vma = ref.m_vma;
    m_lma = ref.m_lma;
    m_length = ref.m_length;
    m_ident = ref.m_ident;
	return *this;
}

VSeg::VSeg()
    : m_name("No Name"),
      m_file("Empty section"),
      m_vma(0),
      m_length(0),
      m_loadable(false),
      m_ident(0)
{
    //std::cout << "New empty VSeg " << *this << std::endl;
}

VSeg::VSeg(std::string& binaryName, std::string& name, uintptr_t vma, size_t length, bool loadable, bool ident)
    : m_name(name),
      m_file(binaryName),
      m_vma(vma),
      m_length(length),
      m_loadable(loadable),
      m_ident(ident)
{
    //std::cout << "New VSeg " << *this << std::endl;
}

VSeg::VSeg( const VSeg &ref )
    : m_name("To be copied"),
      m_file("Empty"),
      m_vma(0),
      m_length(0),
      m_loadable(false),
      m_ident(0)
{
    //std::cout << "New VSeg " << *this << " copied from " << ref << std::endl;
    (*this) = ref;
}




/*
 * PSeg
 */
uintptr_t PSeg::lma() const
{
	return m_lma;
}

uintptr_t PSeg::limit() const
{
	return m_limit;
}

uintptr_t PSeg::length() const
{
	return m_length;
}

uintptr_t PSeg::nextLma() const
{
	return m_nextLma;
}

const std::string & PSeg::name() const
{
	return m_name;
}

void PSeg::check() const
{
    size_t size = m_vsegs.size();
    size_t used[size][2];//lma, lma+length
    size_t i,j;
    
    std::vector<VSeg>::const_iterator it;
    for(it = m_vsegs.begin(), i= 0; it < m_vsegs.end(); it++, i++)
    {
        size_t it_limit = (*it).lma() + (*it).length();
        for(j=0; j< i; j++)
        {
           if(  (used[j][0] == (*it).lma() /*and (*it).legth()?*/) or //not the same lma ,
                (used[j][1] == it_limit /*and (*it).legth()?*/) or  // and not the same limit
                ((used[j][0] > (*it).lma()) and (used[j][1] > (*it).lma())) or //lma not within the used slice
                ((used[j][0] > it_limit) and (used[j][1] > it_limit)) )//limit not within the used slice
            {
                std::ostringstream err;
                err << "Ovelapping Buffers:" << std::endl 
                    << *it << std::endl << m_vsegs[j] << std::endl; 
                throw soclib::exception::RunTimeError( err.str().c_str() );
            }
        }
        used[i][0] = (*it).lma();
        used[i][1] = it_limit;
    }
}

void PSeg::setName(std::string& name )
{
    m_name = name;
}

size_t PSeg::align( unsigned toAlign, unsigned alignPow2)
{
    return ((toAlign + (1 << alignPow2) - 1 ) >> alignPow2) << alignPow2;//page aligned 
}


size_t PSeg::pageAlign( size_t toAlign )
{
    size_t pgs = pageSize();
    size_t pageSizePow2 = __builtin_ctz(pgs);
    
    return align(toAlign, pageSizePow2);//page aligned 

}

void PSeg::setLma( uintptr_t lma )
{
    m_lma = lma;
    
    m_nextLma = pageAlign(lma);//page aligned 

    m_pageLimit = pageAlign(m_lma+m_length); 

    m_limit = (m_lma + m_length);

}

void PSeg::setLength( size_t length )
{
    m_length = length;

    m_pageLimit = pageAlign(m_lma+m_length); 

    m_limit = (m_lma + m_length);

    //std::cout << std::hex << " length seted, m_limit: " << m_limit  << std::endl;
    //std::cout << *this <<std::endl;
}

void PSeg::add( VSeg& vseg )
{
    vseg.m_lma = m_nextLma;
    incNextLma(vseg.length());//for the next vseg
    m_vsegs.push_back(vseg); 
}

void PSeg::addIdent( VSeg& vseg )
{
    vseg.m_lma = vseg.m_vma;
    incNextLma(vseg.length());//to keep track of space used
    m_vsegs.push_back(vseg); 
}

void PSeg::setNextLma( uintptr_t nextLma)
{
    m_nextLma = nextLma;

    confNextLma();
}

void PSeg::incNextLma( size_t inc_next)
{

    m_nextLma += inc_next;

    confNextLma();
}

void PSeg::confNextLma()
{
    if(m_nextLma > m_limit)
    {
        std::cerr << "Erreur pseg overflow... nextLma: "
                  << std::hex << m_nextLma << ", limit: " 
                  << m_limit << std::endl;
        exit(1); 
    }

    m_nextLma = pageAlign( m_nextLma );

    if(m_nextLma > m_pageLimit)
    {
        std::cerr << "Erreur pseg page overflow... nextLma: "
                  << std::hex << m_nextLma << ", limit: " 
                  << m_pageLimit << std::endl;
        exit(1); 
    }
}

void PSeg::setPageSize(size_t pg)
{
    if( pg == 0)
    {
        std::cerr << "PageSize must be positive" << std::endl;
        return;
    }
    pageSize() = pg;
}

size_t& PSeg::pageSize()
{
    static size_t m_pageSize;
    return m_pageSize;
}

PSeg & PSeg::operator=( const PSeg &ref )
{
    if ( &ref == this )
        return *this;

    //std::cout << "Copying " << ref << " to " << *this << std::endl;

    m_name = ref.m_name;
    m_length = ref.m_length;
    m_limit = ref.m_limit;
    m_pageLimit = ref.m_pageLimit;
    m_lma = ref.m_lma;
    m_nextLma = ref.m_nextLma;
    m_vsegs = ref.m_vsegs;

	return *this;
}

void PSeg::print( std::ostream &o ) const
{
	o << "<Physical segment "
	  << std::showbase << m_name
	  << ", from: " << std::hex 
      << m_lma << " to " << m_limit 
      << ", size : "  << m_length 
      << ", filled to: "  << m_nextLma 
      << ", containing: "<< std::endl;
        std::vector<VSeg>::const_iterator it;
        for(it = m_vsegs.begin(); it < m_vsegs.end(); it++)
    o << " " << *it << std::endl;

    o << ">";
}

PSeg::PSeg( const std::string &name,
               uintptr_t lma,
                size_t length)
{
    m_name = name;
    m_length = length;

    setLma(lma);
    //std::cout <<"New PSeg :"<< *this ;          
}

PSeg::PSeg( const std::string &name):
      m_lma(0),
      m_length(0),
      m_nextLma(0),
      m_limit(0)
{
    m_name = name;
}

PSeg::PSeg( uintptr_t lma):
      m_name("No name"),
      m_lma(0),
      m_length(0),
      m_nextLma(0),
      m_limit(0)
{
    setLma(lma);
}


PSeg::PSeg()
    :
      m_name("Empty section"),
      m_lma(0),
      m_length(0),
      m_nextLma(0),
      m_limit(0)
{
    //std::cout << "New empty PSeg " << *this << std::endl;
}

PSeg::PSeg( const PSeg &ref )
    : m_name("To be copied"),
      m_lma(0),
      m_length(0),
      m_nextLma(0),
      m_limit(0)
{
    //std::cout << "New PSeg " << *this << " copied from " << ref << std::endl;
    (*this) = ref;
}

PSeg::~PSeg()
{
//    std::cout << "Deleted PSeg " << *this << std::endl;
}


}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


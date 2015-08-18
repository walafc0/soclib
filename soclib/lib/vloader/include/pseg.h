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
#ifndef SOCLIB_PSEG_H_
#define SOCLIB_PSEG_H_

#include <string>
#include <vector>
#include <ios>

#include "stdint.h"

namespace soclib { namespace common {

class VLoader;

class VSeg
{
    friend class PSegHandler;
    friend class PSeg;
    friend class VLoader;

    std::string m_name;
    std::string m_file;
    uintptr_t m_vma;   //The address of the section to load in the binary file.
    uintptr_t m_lma;   //Physical address to which we load the seg (getted from the associated PSeg), setted by PSeg::add/addIdent.
    size_t m_length;
    bool m_loadable;     // wether this is a loadable vseg ( code, data...)

public:
    bool m_ident;//Indicate if the idententy mapping is activited. used by PSegHandler::makeIdent()

    const std::string& name() const;
    const std::string& file() const;
    uintptr_t vma() const;
    uintptr_t lma() const;
    size_t length() const;
    //void add( VObj& vobj );//add a VObj

    void print( std::ostream &o ) const;
    friend std::ostream &operator<<( std::ostream &o, const VSeg &s )
    {
        s.print(o);
        return o;
    }

    VSeg& operator=( const VSeg &ref );

    VSeg();
    VSeg( const VSeg &ref );
    VSeg(std::string& binaryName, std::string& name, uintptr_t vma, size_t length, bool loadable, bool ident);

    ~VSeg();
};


class PSeg
{
    std::string m_name;
    uintptr_t m_lma;
    size_t m_length;

    uintptr_t m_pageLimit;  //The end (m_lma + m_length)  address of the segment pageSize aligned 
    uintptr_t m_nextLma;    //next free base 
    
    void confNextLma();     //check that m_nextLma has a correct value: whithin the seg limits

public:
    std::vector<VSeg> m_vsegs;
    uintptr_t m_limit;// m_lma + m_length


    const std::string& name() const;
    uintptr_t lma() const;
    size_t length() const;
    uintptr_t limit() const;
    uintptr_t nextLma() const;

    void check() const;

    void setName(std::string& name);
    void setLma( uintptr_t lma);
    void setLength(size_t length);

    static size_t align( unsigned toAlign, unsigned alignPow2);
    static size_t pageAlign( size_t toAlign );
    static void setPageSize(size_t pg);
    static size_t& pageSize();

    void add( VSeg& vseg );//add a VSeg
    void addIdent( VSeg& vseg );

    void setNextLma( uintptr_t nextLma);
    void incNextLma( size_t inc_next);

    void print( std::ostream &o ) const;

    friend std::ostream &operator<<( std::ostream &o, const PSeg &s )
    {
        s.print(o);
        return o;
    }
    PSeg & operator=( const PSeg &ref );

    PSeg();
    PSeg( const PSeg &ref );
    PSeg( const std::string &name);
    PSeg( const uintptr_t lma);
    PSeg( const std::string &name,
		     uintptr_t lma,
             size_t length);
    ~PSeg();
};


}}

#endif /* SOCLIB_PSEG_H_ */

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
 *         Mohamed Lamine KARAOUI <Mohamed.Karaoui@lip6.fr>, 2012
 */

#include <string.h>
#include <cassert>
#include <sstream>
#include <fstream>
#include <ios>
#include <iostream>
#include <cstdarg>
#include <cassert>
#include <iomanip>


#include "exception.h"
#include "vloader.h"

namespace soclib { namespace common {

#define VLOADER_DEBUG


VLoader::VLoader( const std::string &filename, const size_t pageSize)
{
    m_path = filename;
 
    PSeg::setPageSize(pageSize);

    load_bin(m_path);
#ifdef VLOADER_DEBUG
    std::cout << "Binary filename = " << m_path << std::endl;
    //print_mapping_info(m_data);
#endif

    buildMap(m_data);
    
#ifdef VLOADER_DEBUG
    std::cout << "parsing done" << std::endl ;
#endif

#ifdef VLOADER_DEBUG
    std::cout << m_psegh << std::endl;
#endif

    m_psegh.check();
    
}

void VLoader::print( std::ostream &o ) const
{
    std::cout << m_psegh << std::endl;
}

void VLoader::load( void *buffer, uintptr_t address, size_t length ) const
{
    Loader  loader;

    //TODO:  A memory init value choosed by the user ?
    memset(buffer, 0, length);

#ifdef VLOADER_DEBUG
    std::cout << "Buffer: "<< buffer << ", address: "<< address << ", length: " << length << std::endl;
#endif

    const PSeg ps = m_psegh.getByAddr(address);
    
    if(ps.length() > length)
        std::cout << std::hex
            << "Warning, loading only " << length
            << " bytes to " << ps.name()
            << ", declared size (in map_info) is: " << ps.length() 
            << std::endl;
    

    std::vector<VSeg>::const_iterator it ;
    for(it = ps.m_vsegs.begin(); it < ps.m_vsegs.end(); it++)
    {
        bool local = false;

        /* Get a hand of the corresponding loader               */
        const std::string file = (*it).file();
        if(!file.compare(m_path))
        {
            local = true; //loading the local structure
        }
        else
            loader = m_loaders[file];//prohibit the use of the const keyword for this function
        //TODO assert that the loader really existe
        
        /* The offset of the appropriate physical address       */
        size_t offset = (*it).lma() - ps.lma();
        size_t available_buf_size = length - offset;

        /* Load with (*it).m_vma() and the corresponding size  */
#ifdef VLOADER_DEBUG
        std::cout << "Loading: " << (*it) << std::endl;
#endif

        assert(ps.lma() <= (*it).lma());

        std::cout << "Loading at (physical address): 0x" 
	              << std::hex << std::noshowbase 
                  << std::setw (8) << std::setfill('0') 
                  << (*it).lma() << " || the virtual segment: ";
        if(local)
        {
            size_t copy_size = (m_size < available_buf_size)? m_size : available_buf_size;
            if ( copy_size > available_buf_size )
               std::cout << "Warning, loading only " << copy_size
                         << " bytes from " << m_path  << std::endl;
            //TODO: the printing!
            memcpy( ((char *)buffer + offset), m_data, copy_size );
        }else
            loader.match_load(((char *)buffer + offset), (*it).vma(), available_buf_size);
        std::cout << "(" << (*it).file() << ")" << std::endl;
    }

}

void* VLoader::load_bin(std::string filename)
{
    
    std::cout  << "Trying to load the binary blob from file '" << m_path << "'" << std::endl;

    std::ifstream input(m_path.c_str(), std::ios_base::binary|std::ios_base::in);

    if ( ! input.good() )
        throw soclib::exception::RunTimeError(std::string("Can't open the file: ") + m_path);

    input.seekg( 0, std::ifstream::end );
    m_size = input.tellg();
    input.seekg( 0, std::ifstream::beg );

    //m_data = new void*[m_size];
    m_data = std::malloc(m_size);
    if ( !m_data )
        throw soclib::exception::RunTimeError("malloc failed... No memory space");

    input.read( (char*)m_data, m_size );
    
    return m_data;
}


VLoader::~VLoader()
{
    //std::cout << "Deleted VLoader " << *this << std::endl;
    std::free(m_data);
}


/////////////////////////////////////////////////////////////////////////////
// various mapping_info data structure access functions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
mapping_pseg_t* VLoader::get_pseg_base( mapping_header_t* header )
{
    return   (mapping_pseg_t*)    ((char*)header +
                                  MAPPING_HEADER_SIZE +
                                  MAPPING_CLUSTER_SIZE*header->clusters);
}
/////////////////////////////////////////////////////////////////////////////
mapping_vspace_t* VLoader::get_vspace_base( mapping_header_t* header )
{
    return   (mapping_vspace_t*)  ((char*)header +
                                  MAPPING_HEADER_SIZE +
                                  MAPPING_CLUSTER_SIZE*header->clusters +
                                  MAPPING_PSEG_SIZE*header->psegs);
}
/////////////////////////////////////////////////////////////////////////////
mapping_vseg_t* VLoader::get_vseg_base( mapping_header_t* header )
{
    return   (mapping_vseg_t*)    ((char*)header +
                                  MAPPING_HEADER_SIZE +
                                  MAPPING_CLUSTER_SIZE*header->clusters +
                                  MAPPING_PSEG_SIZE*header->psegs +
                                  MAPPING_VSPACE_SIZE*header->vspaces);
}
/////////////////////////////////////////////////////////////////////////////
mapping_vobj_t* VLoader::get_vobj_base( mapping_header_t* header )
{
    return   (mapping_vobj_t*)    ((char*)header +
                                  MAPPING_HEADER_SIZE +
                                  MAPPING_CLUSTER_SIZE*header->clusters +
                                  MAPPING_PSEG_SIZE*header->psegs +
                                  MAPPING_VSPACE_SIZE*header->vsegs +
                                  MAPPING_VSPACE_SIZE*header->vspaces);
}

/////////////////////////////////////////////////////////////////////////////
// print the content of the mapping_info data structure 
////////////////////////////////////////////////////////////////////////
void VLoader::print_mapping_info(void* desc)
{
    mapping_header_t*   header = (mapping_header_t*)desc;  

    mapping_pseg_t*	    pseg    = get_pseg_base( header );;
    mapping_vspace_t*	vspace  = get_vspace_base ( header );;
    mapping_vseg_t*	    vseg    = get_vseg_base ( header );

    // header
    std::cout << std::hex << "mapping_info" << std::endl
              << " + signature = " << header->signature << std::endl
              << " + name = " << header->name << std::endl
              << " + clusters = " << header->clusters << std::endl
              << " + psegs = " << header->psegs << std::endl
             << " + ttys = " << header->ttys  << std::endl
             << " + vspaces = " << header->vspaces  << std::endl
             << " + globals = " << header->globals  << std::endl
             << " + vsegs = " << header->vsegs  << std::endl
             << " + tasks = " << header->tasks  << std::endl;

    // psegs
    for ( size_t pseg_id = 0 ; pseg_id < header->psegs ; pseg_id++ )
    {
        std::cout << "pseg " << pseg_id << std::endl
         << " + name = " << pseg[pseg_id].name << std::endl 
         << " + base = " << pseg[pseg_id].base << std::endl 
         << " + length = " << pseg[pseg_id].length << std::endl ;
    }

    // globals
    for ( size_t vseg_id = 0 ; vseg_id < header->globals ; vseg_id++ )
    {
        std::cout << "global vseg: " << vseg_id << std::endl
         << " + name = " << vseg[vseg_id].name << std::endl 
         << " + vbase = " << vseg[vseg_id].vbase << std::endl 
         << " + length = " << vseg[vseg_id].length << std::endl 
         << " + mode = " << (size_t)vseg[vseg_id].mode << std::endl 
         << " + ident = " << (bool)vseg[vseg_id].ident << std::endl 
         << " + psegname" << pseg[vseg[vseg_id].psegid].name << std::endl;
        //TODO print vobjs
    }


    // vspaces
    for ( size_t vspace_id = 0 ; vspace_id < header->vspaces ; vspace_id++ )
    {
        std::cout << "***vspace: " << vspace_id << "***" << std::endl
         << " + name = " <<  vspace[vspace_id].name  << std::endl 
         << " + ttys = " <<  vspace[vspace_id].ttys  << std::endl;

        for ( size_t vseg_id = vspace[vspace_id].vseg_offset ; 
              vseg_id < (vspace[vspace_id].vseg_offset + vspace[vspace_id].vsegs) ; 
              vseg_id++ )
        {
            std::cout << "private vseg: ";
            std::cout <<  vseg_id  << std::endl
             << " + name = " <<  vseg[vseg_id].name  << std::endl
             << " + vbase = " <<  vseg[vseg_id].vbase  << std::endl
             << " + length = " <<  vseg[vseg_id].length  << std::endl
             << " + mode = " <<  (size_t)vseg[vseg_id].mode  << std::endl
             << " + ident = " <<  (bool)vseg[vseg_id].ident  << std::endl
             << " + psegname = " << pseg[vseg[vseg_id].psegid].name  << std::endl << std::endl;
        //TODO print vobjs
        }

    }
} // end print_mapping_info()

///////////////////////////////////////////////////////////////////////////
void VLoader::pseg_map( mapping_pseg_t* pseg) 
{
    std::string name(pseg->name);
    m_psegh.m_pSegs.push_back(PSeg(name, pseg->base, pseg->length));
}

///////////////////////////////////////////////////////////////////////////
void VLoader::vseg_map( mapping_vseg_t* vseg) 
{

    mapping_vobj_t*     vobj   = get_vobj_base( (mapping_header_t*) m_data ); 
    PSeg *ps = &(m_psegh.get(vseg->psegid));// get physical segment pointer(PSegHandler::get)
    size_t cur_vaddr;
    size_t cur_paddr;
    bool first = true;
    bool aligned = false;

    VSeg     * vs = new VSeg;
    std::string s(vseg->name);
    vs->m_name = s;

    vs->m_vma = vseg->vbase;

    cur_vaddr = vseg->vbase;
    cur_paddr = ps->nextLma();
    
    vs->m_length = 0;
    mapping_vobj_t* cur_vobj;

    size_t simple_size = 0; //for debug
    
    for ( size_t vobj_id = vseg->vobj_offset ; vobj_id < (vseg->vobj_offset + vseg->vobjs) ; vobj_id++ )
    {
        cur_vobj = &vobj[vobj_id];

        std::cout << "cur vobj("<< vobj_id <<"): " << cur_vobj->name << " (" <<cur_vobj->vaddr << ")" 
                        << " size "<< cur_vobj->length << " type " <<  cur_vobj->type << std::endl;

        if(cur_vobj->type == ELF)
        {

            if(!first) 
                throw soclib::exception::RunTimeError(std::string("elf vobj type, must be placed first in a vseg"));

            std::string f(cur_vobj->binpath);
            size_t elf_size;
            std::cout << f <<  " vs "<< m_path << ", vobj_id " <<  vobj_id << std::endl;
            if(!f.compare(m_path))    //local blob: map_info
                elf_size = this->m_size;       
            else
            { 
                if(m_loaders.count(f) == 0 )
                    m_loaders[f] = *(new Loader(f));

                elf_size =  (m_loaders[f]).get_section_size(cur_vaddr);
                assert((elf_size >0) and "ELF section empty ?");
            }
            cur_vobj->length = elf_size;//set the actual size
            if(elf_size > cur_vobj->length)
               std::cout << "Warning, specified elf type vobj ("<< cur_vobj->name  <<") size is "<< cur_vobj->length
                         << ", the actual size is "  << elf_size  << std::endl;

            vs->m_file = f;
            vs->m_loadable = true;        
        }
        first = false;

        if(cur_vobj->align)
        {
            cur_paddr = PSeg::align(cur_paddr, cur_vobj->align);
            aligned = true;
        }


        cur_vaddr += cur_vobj->length;
        cur_paddr += cur_vobj->length;
        simple_size += cur_vobj->length;
    }

    assert((cur_vaddr >= vseg->vbase ));
    assert((cur_paddr >= ps->nextLma() ));

    //vs->m_length = PSeg::pageAlign(cur_vaddr - vseg->vbase); 
    vs->m_length = (cur_paddr - ps->nextLma()); //pageAlign is done by the psegs

    if(aligned)
    {
        std::cout << "vseg aligned:: base: " << std::hex << ps->nextLma() 
            <<" to "<< std::hex << ps->nextLma()+vs->m_length<< " size " << std::dec << vs->m_length << std::endl;
        std::cout << "simple vseg(same base) to "<< std::hex <<(ps->nextLma()+simple_size)  <<" size " << std::dec << simple_size << std::endl;
    }
     
    vs->m_ident = vseg->ident;      

    if ( vseg->ident != 0 )            // identity mapping required
        ps->addIdent( *vs );
    else
        ps->add( *vs );

} // end vseg_map()


/////////////////////////////////////////////////////////////////////
void VLoader::buildMap(void* desc)
{
    mapping_header_t*   header = (mapping_header_t*)desc;  

    mapping_vspace_t*   vspace = get_vspace_base( header );     
    mapping_pseg_t*     pseg   = get_pseg_base( header ); 
    mapping_vseg_t*     vseg   = get_vseg_base( header );

    // get the psegs
#ifdef VLOADER_DEBUG
std::cout << "\n******* Storing Pseg information *********\n" << std::endl;
#endif
    for ( size_t pseg_id = 0 ; pseg_id < header->psegs ; pseg_id++ )
    {
        pseg_map( &pseg[pseg_id]);
    }

    // map global vsegs
#ifdef VLOADER_DEBUG
std::cout << "\n******* mapping global vsegs *********\n" << std::endl;
#endif
    for ( size_t vseg_id = 0 ; vseg_id < header->globals ; vseg_id++ )
    {
        vseg_map( &vseg[vseg_id]);
    }

    // second loop on virtual spaces to map private vsegs
    for (size_t vspace_id = 0 ; vspace_id < header->vspaces ; vspace_id++ )
    {

#ifdef VLOADER_DEBUG
std::cout << "\n******* mapping all vsegs of " << vspace[vspace_id].name << " *********\n" << std::endl;
#endif
            
        for ( size_t vseg_id = vspace[vspace_id].vseg_offset ; 
              vseg_id < (vspace[vspace_id].vseg_offset + vspace[vspace_id].vsegs) ; 
              vseg_id++ )
        {
            vseg_map( &vseg[vseg_id]); 
        }
    } 

} // end buildMap()


}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


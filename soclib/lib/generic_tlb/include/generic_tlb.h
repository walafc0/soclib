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
 *         Alain Greiner <alain.greiner@lip6.fr> July 2008
 *
 * Maintainers: alain yang
 */

/**********************************************************************
 * File         : generic_tlb.h
 * Date         : 07/01/2012
 * Authors      : Alain Greiner
 **********************************************************************
 * This object is a generic TLB (Translation Lookaside Buffer) 
 * It is implemented as a set-associative cache.
 * It supports two sizes of page : small page of 4K bytes,
 * and big pages of 2M bytes.
 * The replacement algorithm is pseudo-LRU.
 * The virtual adress is 32 bits.
 * 
 * Each TLB entry has the following format:
 * - bool       V	    valid           
 * - bool       L	    locally accessed 
 * - bool       R	    remotely accessed 
 * - bool       C	    cachable   
 * - bool       W	    writable 
 * - bool       X	    executable 
 * - bool       U       unprotected: access in user mode allowed
 * - bool       G	    global: entry not invalidated by a TLB flush
 * - bool       D	    dirty: page has been modified
 * - bool       B	    big page (0 => 4K page / 1 => 2M page)
 * - bool       Z       recently used: pseudo-LRU replacement in TLB
 * - uint32_t   vpn     virtual page number (20 bits / 11 bits)
 * - uint32_t   ppn     physical page number (28 bits / 19 bits)
 * - paddr_t    nline	cache line index of the corresponding PTE
 *
 * This TLB supports a bypass mechanism in order to avoid access to 
 * PT1 in case of tlb miss for a small page : It keep the last valid 
 * (vpn -> ptba) translation in four specific registers.
 *
 * Implementation note:
 * Each field of a tlb entry is implemented as an (nways*nsets) array.
 *
 **********************************************************************
 * This object has 3 constructor parameters:
 * - uint32_t nways       : number of ways per associative set.
 * - uint32_t nsets       : number of associative sets.
 * - uint32_t paddr_nbits : number of bits in physical address
 * Both nways & nsets must be power of 2 no larger than 64.
 * paddr_nbits cannot be smaller than 32 or larger than 42.
 **********************************************************************/

#ifndef SOCLIB_CABA_GENERIC_TLB_H
#define SOCLIB_CABA_GENERIC_TLB_H

#include <inttypes.h>
#include <systemc>
#include <assert.h>
#include "static_assert.h"
#include "arithmetics.h"
#include <iostream>
#include <iomanip>

namespace soclib { 
namespace caba {

using namespace sc_core;

    // structure containing the 11 flags of a tlb entry
    typedef struct pte_info_s {
        bool v;    // valid             
        bool l;    // locally accessed     
        bool r;    // remotely accessed 
        bool c;    // cacheable    
        bool w;    // writable    
        bool x;    // executable  
        bool u;    // unprotected
        bool g;    // global      
        bool d;    // dirty       
        bool b;	   // big page
        bool z;    // recently used
    }pte_info_t;

    enum {  
        PTD_ID2_MASK  = 0x001FF000,
        PAGE_K_MASK   = 0x00000FFF,
        PAGE_M_MASK   = 0x001FFFFF,
    };

    enum {  
        PAGE_M_NBITS = 21,
        PAGE_K_NBITS = 12,
        INDEX1_NBITS = 11,
    };

    // masks for flags bits in PTE
    enum {
        PTE_V_MASK = 0x80000000,	// valid bit in PTE
        PTE_T_MASK = 0x40000000,	// type bit in PTE
        PTE_L_MASK = 0x20000000,	// local bit in PTE
        PTE_R_MASK = 0x10000000,	// remote bit in PTE
        PTE_C_MASK = 0x08000000,	// cacheable bit in PTE
        PTE_W_MASK = 0x04000000,	// writeable bit in PTE
        PTE_X_MASK = 0x02000000,	// executable bit in PTE
        PTE_U_MASK = 0x01000000,	// unprotected bit in PTE
        PTE_G_MASK = 0x00800000,	// global bit in PTE
        PTE_D_MASK = 0x00400000,    // dirty bit in PTE
    };

    // shifts for flags bits in PTE
    enum {  
        PTE_V_SHIFT = 31,
        PTE_T_SHIFT = 30,
        PTE_L_SHIFT = 29,
        PTE_R_SHIFT = 28,
        PTE_C_SHIFT = 27,
        PTE_W_SHIFT = 26,
        PTE_X_SHIFT = 25,
        PTE_U_SHIFT = 24,
        PTE_G_SHIFT = 23,
        PTE_D_SHIFT = 22,
    };

using soclib::common::uint32_log2;

//////////////////////////
template<typename paddr_t>
class GenericTlb
{
protected:

    // structure constants
    const std::string   m_name;
    const size_t        m_procid;
    const size_t  	    m_nways;
    const size_t  	    m_nsets;
    const size_t  	    m_paddr_nbits;
    const size_t  	    m_sets_shift;
    const size_t  	    m_sets_mask;

    // TLB content: arrays[m_nsets*m_nways]
    paddr_t		        *m_nline;
    uint32_t		    *m_ppn; 
    uint32_t		    *m_vpn;
    bool    		    *m_valid;  
    bool    		    *m_local;
    bool    		    *m_remote;
    bool    		    *m_cacheable;
    bool    		    *m_writable;
    bool    		    *m_executable;
    bool    		    *m_unprotected;
    bool    		    *m_global;
    bool    		    *m_dirty;
    bool    		    *m_big;
    bool    		    *m_recent;

    // bypass registers
    bool		m_bypass_valid;		// valid bypass registered
    uint32_t	m_bypass_id1;		// IX1 field in the VPN
    uint32_t	m_bypass_ptba;		// PTBA value 
    paddr_t		m_bypass_nline;		// cache line index for the corresponding PTE

public:

    /////////////////////////////////////////
    paddr_t get_nline(size_t way, size_t set)
    { 
        return m_nline[m_nsets*way+set];
    }
    ////////////////////////////////////////
    uint32_t get_vpn(size_t way, size_t set)
    { 
        return m_vpn[m_nsets*way+set];
    }
    ////////////////////////////////////////
    uint32_t get_ppn(size_t way, size_t set)
    { 
        return m_ppn[m_nsets*way+set];
    }
    //////////////////////////////////////
    bool get_valid(size_t way, size_t set)
    { 
        return m_valid[m_nsets*way+set];
    }
    ///////////////////////////////////////
    bool get_global(size_t way, size_t set)
    { 
        return m_global[m_nsets*way+set];
    }
    ////////////////////////////////////
    bool get_big(size_t way, size_t set)
    { 
        return m_big[(way*m_nsets)+set]; 
    }
    //////////////////////////////////////
    bool get_local(size_t way, size_t set)
    { 
        return m_local[(way*m_nsets)+set]; 
    }
    ///////////////////////////////////////
    bool get_remote(size_t way, size_t set)
    { 
        return m_remote[(way*m_nsets)+set]; 
    }
    //////////////////////////////////////////
    bool get_cacheable(size_t way, size_t set)
    { 
        return m_cacheable[(way*m_nsets)+set]; 
    }
    /////////////////////////////////////////
    bool get_writable(size_t way, size_t set)
    { 
        return m_writable[(way*m_nsets)+set]; 
    }
    ///////////////////////////////////////////
    bool get_executable(size_t way, size_t set)
    { 
        return m_executable[(way*m_nsets)+set]; 
    }
    ////////////////////////////////////////////
    bool get_unprotected(size_t way, size_t set)
    { 
        return m_unprotected[(way*m_nsets)+set]; 
    }
    //////////////////////////////////////
    bool get_dirty(size_t way, size_t set)
    { 
        return m_dirty[(way*m_nsets)+set]; 
    }
    //////////////////////////////////////
    bool get_recent(size_t way, size_t set)
    { 
        return m_recent[m_nsets*way+set];
    }

    //////////////////////////////////////////////////////////////
    // constructor checks parameters, allocates the memory
    // and computes m_page_mask, m_sets_mask and m_sets_shift
    //////////////////////////////////////////////////////////////
    GenericTlb(const std::string &name,
               size_t procid,
               size_t nways, 
               size_t nsets, 
               size_t paddr_nbits)
    : m_name(name),
      m_procid(procid),
      m_nways(nways),
      m_nsets(nsets),
      m_paddr_nbits(paddr_nbits),
      m_sets_shift(uint32_log2(nsets)),
      m_sets_mask((1 << (int) uint32_log2(nsets)) - 1)
    {
        assert(IS_POW_OF_2(nsets));
        assert(IS_POW_OF_2(nways));
        assert(nsets <= 64);
        assert(nways <= 64);

        if((m_paddr_nbits < 32) || (m_paddr_nbits > 42))
        {
            printf("Error in the genericTlb component\n");
            printf("The physical address parameter must be in the range [32,42]\n");
            exit(1);
        } 

        m_nline      	= new paddr_t[nways * nsets];
        m_ppn        	= new uint32_t[nways * nsets];
        m_vpn        	= new uint32_t[nways * nsets];
        m_valid      	= new bool[nways * nsets];
        m_local      	= new bool[nways * nsets];
        m_remote     	= new bool[nways * nsets];
        m_cacheable  	= new bool[nways * nsets];
        m_writable   	= new bool[nways * nsets];
        m_executable 	= new bool[nways * nsets];
        m_unprotected	= new bool[nways * nsets];
        m_global     	= new bool[nways * nsets];
        m_dirty		    = new bool[nways * nsets];
        m_big   	    = new bool[nways * nsets];
        m_recent	    = new bool[nways * nsets];

    } // end constructor

    /////////////
    ~GenericTlb()
    {
        delete [] m_nline;
        delete [] m_ppn;
        delete [] m_vpn;
        delete [] m_valid;
        delete [] m_local;
        delete [] m_remote;
        delete [] m_cacheable;
        delete [] m_writable;
        delete [] m_executable;
        delete [] m_unprotected;
        delete [] m_global;
        delete [] m_dirty;
        delete [] m_big;
        delete [] m_recent;
    }

    /////////////////////////////////////////////////////////////
    //  This method resets all the TLB entries
    //  as well as the bypass
    /////////////////////////////////////////////////////////////
    void reset() 
    {

        m_bypass_valid = false;
        for (size_t i = 0; i < m_nways * m_nsets; i++) {
            m_valid[i] = false;

            // Following inits to avoid potential errors from memory checkers
            m_nline[i] = 0;
            m_ppn[i] = 0;
            m_vpn[i] = 0;
            m_local[i] = false;
            m_remote[i] = false;
            m_cacheable[i] = false;
            m_writable[i] = false;
            m_executable[i] = false;
            m_unprotected[i] = false;
            m_global[i] = false;
            m_dirty[i] = false;
            m_big[i] = false;
            m_recent[i] = false;
        }

        // Following init to avoid potential errors from memory checkers
        m_bypass_nline = 0x0;
    } 

    //////////////////////////////////////////////////////////////////////////////////////////
    //  This method takes a virtual adress as input argument. It returns false in case of miss.
    //  In case of HIT, the physical address, the pte informations, way and set are returned. 
    //////////////////////////////////////////////////////////////////////////////////////////
    bool translate(  uint32_t	vaddress,      	// virtual address
                     paddr_t 	*paddress,     	// return physical address
                     pte_info_t *pte_info,  	// return flags      
                     paddr_t    *nline,		    // return nline
                     size_t 	*tw,            // return tlb way  
                     size_t 	*ts )           // return tlb set   
    {
        size_t m_set = (vaddress >> PAGE_M_NBITS) & m_sets_mask; 
        size_t k_set = (vaddress >> PAGE_K_NBITS) & m_sets_mask; 

        for( size_t way = 0; way < m_nways; way++ ) 
        {
            // TLB hit test for 2M page size
            if( get_valid(way,m_set) and get_big(way,m_set) and
               (get_vpn(way,m_set) == (vaddress >> (PAGE_M_NBITS + m_sets_shift))) ) 
            {
                pte_info->v = get_valid(way,m_set);
                pte_info->l = get_local(way,m_set);
                pte_info->r = get_remote(way,m_set);
                pte_info->c = get_cacheable(way,m_set);
                pte_info->w = get_writable(way,m_set);
                pte_info->x = get_executable(way,m_set);
                pte_info->u = get_unprotected(way,m_set);
                pte_info->g = get_global(way,m_set);
                pte_info->d = get_dirty(way,m_set);
                pte_info->b = get_big(way,m_set);
                pte_info->z = get_recent(way,m_set);

                *nline      = get_nline(way,m_set);
                *tw         = way;
                *ts         = m_set;

                *paddress = (paddr_t)((paddr_t)get_ppn(way,m_set) << PAGE_M_NBITS) | 
                            (paddr_t)(vaddress & PAGE_M_MASK);

                set_recent(way, m_set);
                return true;
            }

            // TLB hit test for 4K page size
            if( get_valid(way,k_set) and not get_big(way,k_set) and
               (get_vpn(way,k_set) == (vaddress >> (PAGE_K_NBITS + m_sets_shift))) ) 
            {  
                pte_info->v = get_valid(way,k_set);
                pte_info->l = get_local(way,k_set);
                pte_info->r = get_remote(way,k_set);
                pte_info->c = get_cacheable(way,k_set);
                pte_info->w = get_writable(way,k_set);
                pte_info->x = get_executable(way,k_set);
                pte_info->u = get_unprotected(way,k_set);
                pte_info->g = get_global(way,k_set);
                pte_info->d = get_dirty(way,k_set);
                pte_info->b = get_big(way,k_set);
                pte_info->z = get_recent(way,k_set);

                *nline      = get_nline(way,k_set);
                *tw         = way;
                *ts         = k_set;

                *paddress = (paddr_t)((paddr_t)get_ppn(way,k_set) << PAGE_K_NBITS) | 
                            (paddr_t)(vaddress & PAGE_K_MASK);

                set_recent(way, k_set);
                return true;   
            } 
        } 
        return false;
    } // end translate()

    ///////////////////////////////////////////////////////////////////////////////////////////
    //  This method takes a virtual adress as input argument. It returns false in case of miss.
    //  In case of HIT, the physical address is returned. 
    //////////////////////////////////////////////////////////////////////////////////////////
    bool translate(uint32_t vaddress, paddr_t *paddress) 
    {
        size_t m_set = (vaddress >> PAGE_M_NBITS) & m_sets_mask; 
        size_t k_set = (vaddress >> PAGE_K_NBITS) & m_sets_mask; 

        for( size_t way = 0; way < m_nways; way++ ) 
        {
            // TLB hit test for 2M page size
            if( get_valid(way,m_set) and get_big(way,m_set) and
               (get_vpn(way,m_set) == (vaddress >> (PAGE_M_NBITS + m_sets_shift))) ) 
            {
                *paddress = (paddr_t)((paddr_t)get_ppn(way,m_set) << PAGE_M_NBITS) | 
                            (paddr_t)(vaddress & PAGE_M_MASK);
 
		set_recent(way, m_set);
                return true;
            }

            // TLB hit test for 4K page size
            if( get_valid(way,k_set) and not get_big(way,k_set) and
               (get_vpn(way,k_set) == (vaddress >> (PAGE_K_NBITS + m_sets_shift))) ) 
            {  
                *paddress = ((paddr_t)get_ppn(way,k_set) << PAGE_K_NBITS) | 
                            (paddr_t)(vaddress & PAGE_K_MASK);

		set_recent(way, k_set);
                return true;   
            } 
        }
        return false;
    } // end translate()

    /////////////////////////////////////////////////////////////
    //  This method resets all valid bits in one cycle, 
    //  for non global tlb entries.
    /////////////////////////////////////////////////////////////
    void flush() 
    {
        m_bypass_valid = false;

        for( size_t way = 0; way < m_nways; way++ ) 
        {
            for(size_t set = 0; set < m_nsets; set++) 
            {
                if( not get_global(way,set) ) 
                {
                    m_valid[way*m_nsets+set] = false;
                }
            } 
        } 
    } // end flush

    /////////////////////////////////////////////////////////////
    // This method returns the values of the various fields
    // of a tlb entry identified by the way and set arguments.
    /////////////////////////////////////////////////////////////
    void get_entry(size_t 	way,
                   size_t 	set,
                   pte_info_t*  flags,   
                   uint32_t*	vpn,
                   uint32_t*	ppn,
                   paddr_t*     nline)
    {
        flags->v = m_valid[way*m_nsets+set];
        flags->l = m_local[way*m_nsets+set]; 
        flags->r = m_remote[way*m_nsets+set];
        flags->c = m_cacheable[way*m_nsets+set];
        flags->w = m_writable[way*m_nsets+set];
        flags->x = m_executable[way*m_nsets+set];
        flags->u = m_unprotected[way*m_nsets+set];
        flags->g = m_global[way*m_nsets+set];
        flags->d = m_dirty[way*m_nsets+set];
        flags->b = m_big[way*m_nsets+set];
        flags->z = m_recent[way*m_nsets+set];

        *ppn     = m_ppn[way*m_nsets+set];
        *vpn     = m_vpn[way*m_nsets+set];
        *nline   = m_nline[way*m_nsets+set];
    } // end get_entry()

    /////////////////////////////////////////////////////////////////////////
    //  This method implement the pseudo LRU policy to select a tlb slot:
    //  It returns the least recently used way in the associative set
    //  corresponding to the requested virtual address, and page type.
    //  It returns the selected slot way and set.
    /////////////////////////////////////////////////////////////////////
    void select(uint32_t 	vaddr,
                bool		pte1,
                size_t*		selway,
                size_t* 	selset)
    {
        size_t set;

        if ( pte1 ) 	set = (vaddr >> PAGE_M_NBITS) & m_sets_mask; 
        else        	set = (vaddr >> PAGE_K_NBITS) & m_sets_mask; 

        // search an invalid way
        for(size_t way = 0; way < m_nways; way++) 
        {
            if( not get_valid(way,set) ) 
            {
                *selway = way;
                *selset = set;
                return;
            }
        } 

        // search an old but non global way
        for( size_t way = 0; way < m_nways; way++ ) 
        {
            if( not get_global(way,set) and not get_recent(way,set) ) 
            {
                *selway = way;
                *selset = set;
                return;
            } 
        }
	
        // finally take the first old way
        for( size_t way = 0; way < m_nways; way++ ) 
        {
            if( not get_recent(way,set) ) 
            {
                *selway = way;
                *selset = set;
                return;
            } 
        }

        assert(false && "all TLB ways can't be new at the same time");
    } // end select()

    ////////////////////////////////////////////////////////////////////////
    //  This method writes a new entry in the TLB,
    //  in the slot defined by the way & set arguments.
    //  The big argument defines the page type (true for 2M page).
    //  PTE1 is 32 bits / PTE2 is 64 bits
    //  For both types of page, the pte_flags argument contains the flags.
    //  For 4K pages, the PPN value is contained in the pte_ppn argument.
    //  For 2M pages, the PPN value is contained in the pte_flags argument.
    ////////////////////////////////////////////////////////////////////////
    void write(bool         big,
               uint32_t 	pte_flags, 
               uint32_t 	pte_ppn,
               uint32_t		vaddr,
               size_t 		way, 
               size_t 		set, 
               paddr_t 		nline) 
    {
        if ( big )  // 2M page
        {
            assert ( (set == ((vaddr >> PAGE_M_NBITS) & m_sets_mask)) and  
                      "error in tlb write for a 2M page"); 
            m_vpn[way*m_nsets+set]     = vaddr >> (PAGE_M_NBITS + m_sets_shift);
            m_ppn[way*m_nsets+set]     = pte_flags & ((1<<(m_paddr_nbits - PAGE_M_NBITS))-1);
            m_big[way*m_nsets+set]     = true;
        }
        else        // 4K page
        {
            assert ( (set == ((vaddr >> PAGE_K_NBITS) & m_sets_mask)) and 
                      "error in tlb write for a 4K page"); 
            m_vpn[way*m_nsets+set]     = vaddr >> (PAGE_K_NBITS + m_sets_shift);
            m_ppn[way*m_nsets+set]     = pte_ppn & ((1<<(m_paddr_nbits - PAGE_K_NBITS))-1);
            m_big[way*m_nsets+set]     = false;
        }
        m_nline[way*m_nsets+set]       = nline;
        m_valid[way*m_nsets+set]       = true;
        m_local[way*m_nsets+set]       = (((pte_flags & PTE_L_MASK) >> PTE_L_SHIFT) == 1) ? true : false;
        m_remote[way*m_nsets+set]      = (((pte_flags & PTE_R_MASK) >> PTE_R_SHIFT) == 1) ? true : false;
        m_cacheable[way*m_nsets+set]   = (((pte_flags & PTE_C_MASK) >> PTE_C_SHIFT) == 1) ? true : false;
        m_writable[way*m_nsets+set]    = (((pte_flags & PTE_W_MASK) >> PTE_W_SHIFT) == 1) ? true : false;
        m_executable[way*m_nsets+set]  = (((pte_flags & PTE_X_MASK) >> PTE_X_SHIFT) == 1) ? true : false;
        m_unprotected[way*m_nsets+set] = (((pte_flags & PTE_U_MASK) >> PTE_U_SHIFT) == 1) ? true : false;
        m_global[way*m_nsets+set]      = (((pte_flags & PTE_G_MASK) >> PTE_G_SHIFT) == 1) ? true : false;
        m_dirty[way*m_nsets+set]       = (((pte_flags & PTE_D_MASK) >> PTE_D_SHIFT) == 1) ? true : false;

        set_recent(way, set);
    }  // end write()

    //////////////////////////////////////////////////////////////
    //  This method invalidates a TLB entry
    //  identified by the virtual address.
    //////////////////////////////////////////////////////////////
    bool inval(uint32_t vaddr) 
    {
        size_t m_set = (vaddr >> PAGE_M_NBITS) & m_sets_mask; 
        size_t k_set = (vaddr >> PAGE_K_NBITS) & m_sets_mask; 

        for( size_t way = 0; way < m_nways; way++ ) 
        {
            // TLB hit test for 2M page size
            if( get_valid(way,m_set) and get_big(way,m_set) and
               ( get_vpn(way,m_set) == (vaddr >> (PAGE_M_NBITS + m_sets_shift))) ) 
            {
                m_valid[way*m_nsets+m_set] = false;
                return true;
            }

            // TLB hit test for 4K page size
            if( get_valid(way,k_set) and not get_big(way,k_set) and
               ( get_vpn(way,k_set) == (vaddr >> (PAGE_K_NBITS + m_sets_shift))) ) 
            {  
                m_valid[way*m_nsets+k_set] = false;
                return true;   
            } 
        } 
        return false;
    } // end inval()

    //////////////////////////////////////////////////////////////
    //  This method conditionnally invalidates a TLB entry
    //  identified by the way and set arguments, if it matches 
    //  the nline argument.
    //  The bypass is also inalidated if it matches the nline.
    //////////////////////////////////////////////////////////////
    bool inval(paddr_t 	nline,
               size_t	way,
               size_t	set)
    {
        if ( m_bypass_nline == nline ) m_bypass_valid = false;
        
        if ( m_nline[way*m_nsets+set] == nline )
        {
            m_valid[way*m_nsets+set] = false;
            return true;
        }
        return false;
    } // end inval()

    ///////////////////////////////////////////////////
    // set local bit 
    //////////////////////////////////////////////////
    void set_local( size_t way, 
                    size_t set )
    {
        m_local[way*m_nsets+set] = true;
    }

    ///////////////////////////////////////////////////
    // set remote bit 
    //////////////////////////////////////////////////
    void set_remote( size_t way, 
                     size_t set )
    {
        m_remote[way*m_nsets+set] = true;
    }

    ///////////////////////////////////////////////////
    // set dirty bit 
    //////////////////////////////////////////////////
    void set_dirty( size_t way, 
                    size_t set )
    {
        m_dirty[way*m_nsets+set] = true;
    }

    ///////////////////////////////////////////////////
    // recent bit management for LRU policy
    // if all recent bits but the (way,set) are true,
    // all recent bits must be reset
    // else only the target bit is set
    //////////////////////////////////////////////////
    void set_recent( size_t way, 
                     size_t set )
    {
        bool reset = true;

	    for ( size_t i = 0 ; i < m_nways ; i++ ) 
       	{
            if ( (i != way) and not get_recent(i, set)) reset = false;
        }
        if ( reset )	// all recent bits must be reset
        {
           for (size_t i = 0 ; i < m_nways ; i++)
           {
               m_recent[i*m_nsets+set] = false;   
           }
        }
        else 		// only recent(way,set) is set
        {
            m_recent[way*m_nsets+set] = true;  
        }
    }

    ////////////////////////////////////////////////////////////////////////
    // This get_bypass() function implementis the first level page table 
    // bypass in case of PTE2 miss.
    // It returns the registered ptba if it is valid, and if the ID1 field
    // of the virtual address matches the regisrered ID1 value.
    /////////////////////////////////////////////////////////////////////////
    bool get_bypass(uint32_t	vaddr,
                    uint32_t*   ptba)
    {
        if ( m_bypass_valid and ((vaddr >> PAGE_M_NBITS) == m_bypass_id1) )
        {
            *ptba = m_bypass_ptba;
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////
    //  The set_bypass() method registers the last valid PTD1 
    //  (ID1 -> PTBA) translation in the  m_bypass_valid, m_bypass_id1, 
    //  m_bypass_ptba & m_bypass_nline registers.
    //////////////////////////////////////////////////////////////////////
    void set_bypass(uint32_t	vaddr,
                    uint32_t    ptba,
                    paddr_t     nline)
    {
        m_bypass_valid = true;
        m_bypass_ptba  = ptba;
        m_bypass_id1   = vaddr >> PAGE_M_NBITS;
        m_bypass_nline = nline;
    }             
    ///////////////////////////////////////////////////////////////////////
    //  The reset_bypass() method conditionnally resets the bypass 
    //  when the nline argument matches the registered nline.
    ///////////////////////////////////////////////////////////////////////
    void reset_bypass()
    {
        m_bypass_valid = false;
        // Next affectation to avoid memory checker errors
        m_bypass_nline = 0;
    }
    ///////////////////////////////////////////////////////////////////////
    //  The printTrace() method displays the TLB content
    ///////////////////////////////////////////////////////////////////////
    void printTrace()
    {
        std::cout << "     set way    V  L  R  C  W  X  U  G  D  B  Z"
                  << "   TAG        PPN          NLINE" << std::endl;

        for ( size_t set=0 ; set < m_nsets ; set++ )
        {
            for ( size_t way=0 ; way < m_nways ; way++ )
            {
                if ( m_valid[m_nsets*way+set] )
                std::cout << std::dec << std::noshowbase
                          << "     [" << set << "] [" 
                          << way << "]   ["
                          << m_valid[m_nsets*way+set] << "]["
                          << m_local[m_nsets*way+set] << "]["
                          << m_remote[m_nsets*way+set] << "]["
                          << m_cacheable[m_nsets*way+set] << "]["
                          << m_writable[m_nsets*way+set] << "]["
                          << m_executable[m_nsets*way+set] << "]["
                          << m_unprotected[m_nsets*way+set] << "]["
                          << m_global[m_nsets*way+set] << "]["
                          << m_dirty[m_nsets*way+set] << "]["
                          << m_big[m_nsets*way+set] << "]["
                          << m_recent[m_nsets*way+set] << "]["
                          << std::hex << std::showbase
                          << std::setw(7)  << m_vpn[m_nsets*way+set] << "]["
                          << std::setw(9)  << m_ppn[m_nsets*way+set] << "]["
                          << std::setw(11) << m_nline[m_nsets*way+set] << "]" 
                          << std::endl;
            }
        }
    }
              
}; // end GenericTlb

}}

#endif /* SOCLIB_CABA_GENERIC_TLB_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4




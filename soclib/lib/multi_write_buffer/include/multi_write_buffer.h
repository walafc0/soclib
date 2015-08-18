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
 * Maintainers: alain
 */

////////////////////////////////////////////////////////////////////////////
// This Write Buffer, supports several simultaneous write bursts.
// It contains an integer number of slots, and each slot can contain
// one buffer line. The m_nwords parameter defines the slot width.
// (max length of a write burst). It must be a power of 2.
// The m_nslots parameter defines the max number of concurrent transactions.
// All word adresses in a buffer slot are contiguous, and the
// slot base address is aligned on a buffer line boundary.
// One buffer line is typically a cache line, or half a cache line.
// Byte enable controlled "write after write" are supported
// in the same line, and in the same word.
// All write requests in the same buffer slot will be transmitted
// as a single VCI transaction.
// This write buffer can be described as a set of FSM : one FSM per slot.
// A slot can be in four states : EMPTY, OPEN, LOCKED, SENT.
// 1) The write(address, data, be, cacheable) method is used by the 
//    DCACHE_FM to store a write request in the buffer.
//    It first search an OPEN slot matching the address.
//    If not found, it select an EMPTY slot that goes to OPEN.
//    It returns false if the buffer is full.
// 2) The update() method should be called at each cycle.
//    It increments the circular counter r_ptr_update at each cycle,
//    and the pointed slot goes to LOCKED when it is OPEN and there
//    is not another slot (LOCKED or SENT) with the same address.
// 3) The rok(&min, &max) method is usedby the CMD_FSM. 
//    It returns true when there is at least one LOCKED slot, and returns
//    also the min and max indexes. It updates the read pointer.
// 4) The getAddress(word), getData(word) and getBe(word) methods are
//    used by the CMD_FSM to consume a word in the write bffer. It returns
//    the address, data and be of the word in the slot pointed by r_ptr_read.
// 5) The sent() method is used by the CMD_FSM when the last flit of a write 
//    burst transaction has been send, to switch the slot state to SENT.
// 6) The completed(index) method is used by the RSP_FSM to signal that a write 
//    transaction has been completed, to reset to EMPTY the corresponding slot.
// 7) The empty() method returns true when all slots are empty.
// 8) The miss(address) method can be used by the CMD_FSM to chek that a read 
//    request does not match a pending write transaction. The matching criteria 
//    is the buffer line : the word index in the buffer slot is ignored.
//
// - The general write policy is associative (when looking for an empty slot)
//   but the read policy uses a round robin pointer.
// - It can exist several EMPTY slots, several LOCKED slots,
//   several SENT slots, and several OPEN slot.
////////////////////////////////////////////////////////////////////////////
// User note :
// The update() method must be called at all cycles by the transition 
// function of the hardware component that contains this write buffer
// to update the slot states (from OPEN to LOCKED).
////////////////////////////////////////////////////////////////////////////
// It has 4 constructor parameters :
// - std::string    name
// - size_t         nwords  : buffer width (number of words)
// - size_t         nslots  : buffer depth (number of slots)
// - size_t         cache_line_words : cache_line width
// It has one template parameter :
// - addr_t defines the address format
/////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_MULTI_WRITE_BUFFER_H
#define SOCLIB_MULTI_WRITE_BUFFER_H

#include <systemc>
#include <cassert>
#include "static_assert.h"

namespace soclib { 

  using namespace sc_core;

  /*
   * CC_XCACHE_MULTI_CPU
   *   To manage bus error with multi processor per cache,
   *   need cpu_id in wbuf
   *
   */

//#define CC_XCACHE_MULTI_CPU  1

  enum slot_state_e 
    {
      EMPTY,
      OPEN,
      LOCKED,
      SENT,
    };

  //////////////////////////////
  template<typename addr_t>
  class MultiWriteBuffer
  //////////////////////////////
  {
    typedef uint32_t    data_t;
    typedef uint32_t    be_t;
    typedef uint32_t    stat_t;

    sc_signal<size_t>   r_ptr_read;         // next slot to be read
    sc_signal<size_t>   r_ptr_update;       // next slot to be update
    sc_signal<int>      *r_state;           // slot state array[nslots]
    sc_signal<addr_t>   *r_address;         // slot base address array[nslots]
    sc_signal<bool>     *r_cacheable;       // cacheable array[nslots]
    sc_signal<size_t>   *r_min;             // smallest valid word index array[nslots]
    sc_signal<size_t>   *r_max;             // largest valid word index array[nslots]

#if CC_XCACHE_MULTI_CPU
    sc_signal<size_t>   *r_cpu_id;
#endif

    data_t             **r_data;            // write data  array[nslots][nwords]
    be_t               **r_be;              // byte enable array[nslots][nwords]
    

    size_t              m_nslots;           // buffer depth (number of slots)
    size_t              m_nwords;           // buffer width (number of words)
    addr_t              m_wbuf_line_mask;   // Mask for a write buffer line
    addr_t              m_cache_line_mask;  // Mask for a cache line
 
    stat_t              m_stat_nb_cycles;
    stat_t             *m_stat_slot_utilization;
    stat_t             *m_stat_word_utilization;
    stat_t              m_stat_word_useful;
    stat_t              m_stat_nb_write;
    stat_t              m_stat_nb_write_accepted_in_open_slot;
    stat_t              m_stat_nb_write_accepted_in_same_word;
    stat_t              m_stat_nb_write_refused;
    stat_t              m_stat_nb_read_after_write_test;
    stat_t              m_stat_nb_read_after_write;
    stat_t             *m_stat_cycle_sent;
    stat_t              m_stat_sum_latence;

public:

    /////////////
    void reset()
    {
        r_ptr_read   = 0 ;
        r_ptr_update = 0;
        for( size_t slot = 0 ; slot < m_nslots ; slot++) 
        {
            r_address[slot] = 0 ;
            r_cacheable [slot] = 0 ;
            r_max[slot]     = 0 ;
            r_min[slot]     = m_nwords - 1 ;
            r_state[slot]   = EMPTY ;

#if CC_XCACHE_MULTI_CPU
            r_cpu_id[slot]  = 0;
#endif

            for( size_t word = 0 ; word < m_nwords ; word++ ) 
            {
                r_data[slot][word]      = 0 ;
                r_be  [slot][word]      = 0 ;
            }

            m_stat_cycle_sent         [slot] = 0;
        }

        for( size_t slot = 0 ; slot <= m_nslots ; slot++)
        {
            m_stat_slot_utilization [slot] = 0;
        }
        for( size_t word = 0 ; word < m_nwords ; word++ )
        {
            m_stat_word_utilization [word] = 0;
        }
        m_stat_word_useful                            = 0;
        m_stat_nb_cycles                              = 0;
        m_stat_nb_write                               = 0;
        m_stat_nb_write_accepted_in_open_slot         = 0;
        m_stat_nb_write_accepted_in_same_word         = 0;
        m_stat_nb_write_refused                       = 0;
        m_stat_nb_read_after_write_test               = 0;
        m_stat_nb_read_after_write                    = 0;
        m_stat_sum_latence                            = 0;
    } 

    /////////////////////////////////////
    inline void printTrace(size_t mode=0)
    {
        const char *wbuf_state_str[] = { "EMPTY  ", "OPEN   ", "LOCKED ", "SENT   " };

        std::cout << "  Write Buffer - ptr_read = " << r_ptr_read 
                  << " - ptr_update = " << r_ptr_update << std::endl
                  << "  [slot] state  address  cached {min,max} " << std::endl;

        for( size_t i = 0 ; i < m_nslots ; i++ )
        {
            std::cout << "  [" << i << "]    " 
                      << wbuf_state_str[r_state[i]] 
                      << std::hex << r_address[i].read() << std::dec
                      << "  " << r_cacheable[i].read()
                      << "  {" << r_min[i].read() << "," << r_max[i].read() << "}"
#if CC_XCACHE_MULTI_CPU
                    << " - " << r_cpu_id[i].read()
#endif
            ;

            if(mode & 0x1)
            {
                std::cout << "  ";
                for( size_t w = 0 ; w < m_nwords ; w++ )
                {
                    std::cout << " | " << std::hex << r_data[i][w] 
                              << std::dec << " - " << r_be[i][w];
                }
            }
            std::cout << std::endl;
        }
    }

    /////////////////////////////
    inline void printStatistics()
    {
      stat_t m_stat_nb_write_accepted = m_stat_nb_write - m_stat_nb_write_refused;
      stat_t m_stat_nb_state_sent = m_stat_nb_write_accepted-m_stat_nb_write_accepted_in_open_slot;

      std::cout << "------------------------------------" << std:: dec << std::endl;
      std::cout << "MultiWriteBuffer : " << m_nslots << "x" << m_nwords << " words" << std::endl;
      std::cout << "- NB WRITE                       : " << m_stat_nb_write              << std::endl;
      std::cout << "- NB WRITE REFUSED               : " << m_stat_nb_write_refused      << " (" << (float)m_stat_nb_write_refused*100.0/(float)m_stat_nb_write << "%)" << std::endl;
      std::cout << "- NB WRITE ACCEPTED              : " << m_stat_nb_write_accepted      << " (" << (float)m_stat_nb_write_accepted*100.0/(float)m_stat_nb_write << "%)" << std::endl;
      std::cout << "  + IN OPEN SLOT                 : " << m_stat_nb_write_accepted_in_open_slot << " (" << (float)m_stat_nb_write_accepted_in_open_slot*100.0/(float)m_stat_nb_write_accepted << "%)" << std::endl;
      std::cout << "    + IN SAME WORD               : " << m_stat_nb_write_accepted_in_same_word << " (" << (float)m_stat_nb_write_accepted_in_same_word*100.0/(float)m_stat_nb_write_accepted_in_open_slot << "%)" << std::endl;
      std::cout << "  + IN EMPTY SLOT                : " << m_stat_nb_write_accepted-m_stat_nb_write_accepted_in_open_slot << " (" << (float)m_stat_nb_state_sent*100.0/(float)m_stat_nb_write_accepted << "%)" << std::endl;
      std::cout << "- NB READ AFTER WRITE TEST       : " << m_stat_nb_read_after_write_test << std::endl;
      std::cout << "- NB READ AFTER WRITE            : " << m_stat_nb_read_after_write << " (" << (float)m_stat_nb_read_after_write*100.0/(float)m_stat_nb_read_after_write_test << "%)" << std::endl;
      std::cout << "- AVERAGE LATENCE TO COMPLETE    : " << (float)m_stat_sum_latence/(float)m_stat_nb_state_sent << " cycle(s)" << std::endl;

      stat_t m_stat_slot_utilization_sum = 0;
      for(size_t i=1 ; i<=m_nslots ; i++)
        m_stat_slot_utilization_sum += i*m_stat_slot_utilization[i];
      std::cout << "- SLOT UTILIZATION               : " << (float)m_stat_slot_utilization_sum/(float)m_stat_nb_cycles << std::endl;
      for(size_t i=0 ; i<=m_nslots ; i++)
        {
          std::cout << "  + [" << i << "] " << (float)m_stat_slot_utilization[i]*100.0/(float)m_stat_nb_cycles << "%";
          if (i!=0)
            std::cout << " - "<< (float)m_stat_slot_utilization[i]*100.0/(float)(m_stat_nb_cycles-m_stat_slot_utilization[0]) << "%";
          std::cout << std::endl;
        }

      stat_t m_stat_word_utilization_sum = 0;
      for(size_t i=0 ; i<m_nwords ; i++)
        m_stat_word_utilization_sum += (i+1)*m_stat_word_utilization[i];
      std::cout << "- WORD UTILIZATION               : " << (float)m_stat_word_utilization_sum/(float)m_stat_nb_state_sent << std::endl;
      for(size_t i=0 ; i<m_nwords ; i++)
        std::cout << "  + [" << i+1 << "] " << (float)m_stat_word_utilization[i]*100.0/(float)m_stat_nb_state_sent << "%" << std::endl;
      std::cout << "- WORD USEFUL                    : " << (float)m_stat_word_useful*100/(float)m_stat_word_utilization_sum << "%" << std::endl;
    }

    /////////////////////////////////////////////////////////
    // This method is intended to be used by the VCI_CMD FSM
    // to comply with the read after write policy, and
    // decide if a read miss transaction can be launched.
    // There is an hardware cost associated with this service,
    // because all buffer entries must be tested. 
    inline bool miss(addr_t addr)
    {
        bool miss = true;
        for( size_t i = 0 ; i < m_nslots ; i++ )
        {
            if ( (r_state[i].read() != EMPTY) and
                 ((r_address[i].read() & ~m_cache_line_mask) == 
                  (addr & ~m_cache_line_mask)) )
            {
                miss = false;
                m_stat_nb_read_after_write ++;
            }
        }

        m_stat_nb_read_after_write_test ++;

        return miss;
    }
    
    //////////////////////////////////
    // Test if all slots are empty
    inline bool empty( )
    {
        for( size_t i=0 ; i<m_nslots ; i++ )
        {
            if ( r_state[i].read() != EMPTY ) return false;
        }
        return true;
    }

    //////////////////////////////////////////////////////////////
    // This method is intended to be called by the VCI_CMD FSM,
    // to test if a locked slot is available.
    // It changes the pointer to the next available locked slot, 
    // and returns the min & max indexes when it has been found.
    inline bool rok(size_t* min, size_t* max)
    {
        bool    found = false;     
        size_t  num_slot;
        size_t  i=0;

        for(; i<m_nslots ; i++)
        {
            num_slot = (r_ptr_read+i)%m_nslots;
            if( r_state[num_slot] == LOCKED ) 
            { 
                found      = true;
                *min       = r_min[num_slot];
                *max       = r_max[num_slot];
                r_ptr_read = num_slot;
                break;
            }
        }
        return found;
    } // end rok()

    //////////////////////////////////////////////////////////////
    // This method is intended to be called by the VCI_CMD FSM,
    // to test if a locked slot is available.
    // It changes the pointer to the next available locked slot, 
    // and returns the min & max indexes, address and slot index
    inline bool rok(size_t* min, size_t* max, addr_t * addr, size_t * index)
    {
        bool    found = false;     
        size_t  num_slot;
        size_t  i=0;

        for(; i<m_nslots ; i++)
        {
            num_slot = (r_ptr_read+i)%m_nslots;
            if( r_state[num_slot] == LOCKED)
            {
                found      = true;
                *min       = r_min[num_slot].read();
                *max       = r_max[num_slot].read();
                *addr      = r_address[num_slot].read();
                *index     = num_slot;
                r_ptr_read = num_slot;
                break;
            }
        }
        return found;
    } // end rok()

    ////////////////////////////////////////////////////////////////////
    // This method is intended to be used by the VCI_CMD FSM.
    // It can only change a slot state from LOCKED to SENT when
    // the corresponding write command has been fully transmitted.
    void inline sent()  
    {
        assert( (r_state[r_ptr_read.read()] == LOCKED) &&
              "write buffer error : illegal sent command received");

        r_state[r_ptr_read.read()] = SENT;

        m_stat_cycle_sent [r_ptr_read.read()] = m_stat_nb_cycles;

        m_stat_word_utilization [r_max[r_ptr_read]-r_min[r_ptr_read]] ++;
        for( size_t word = r_min[r_ptr_read]; word <= r_max[r_ptr_read]; word++ )
            if (r_be [r_ptr_read][word] != 0) m_stat_word_useful ++;
    } 

    ////////////////////////////////////////////////////////////////////
    // This method must be called at each cycle.
    // It can change a slot state from OPEN to LOCKED,
    // using the circular counter r_ptr_update.
    // If the slot pointed by r_ptr_update is OPEN, and there is not
    // another slot (LOCKED or SENT) with the same address,
    // the pointed slot goes to LOCKED.
    void update()
    {
        m_stat_nb_cycles ++;

        size_t stat_nb_not_empty = 0;
        for(size_t i=0 ; i<m_nslots ; i++)
        {
            if (r_state[i] != EMPTY) stat_nb_not_empty ++;
        }
        m_stat_slot_utilization[stat_nb_not_empty] ++;

        bool found = false;
            
        if(r_state[r_ptr_update.read()] == OPEN)
        {
            // searching for a pending request with the same address
            for( size_t i=0 ; i<m_nslots ; i++ )
            {
                if ( (r_state[i].read() != EMPTY) and 
                     (r_state[i].read() != OPEN)  and
                     (r_address[r_ptr_update.read()].read() == r_address[i].read()) ) 
                {
                    found = true;
                    break; 
                }
            }
            if ( !found ) r_state[r_ptr_update] = LOCKED;
        }
        r_ptr_update = (r_ptr_update.read() + 1)%m_nslots;
    }

    //////////////////////////////////////////////////////////////////////////
    // This method is intended to be used by the DCACHE FSM.
    // It can only change a slot state from EMPTY to OPEN.
    // It can change the slot content : r_address, r_data, r_be, r_min, r_max
    // It searches first an open slot, and then an empty slot.
    // It returns true in case of success.
    bool write( addr_t  addr,
                be_t    be, 
                data_t  data, 
                bool    cached, 
                size_t  cpu_id=0)
    {
        assert (be != 0);

        size_t    word = (size_t)((addr &  m_wbuf_line_mask) >> 2) ;
        addr_t    address = addr & ~m_wbuf_line_mask;
        bool      found = false;     
        size_t    num_slot;

        // Search a slot to be written
        // first : search an open slot with the same address
        for( size_t i=0 ; i<m_nslots ; i++)
        {
            if( (r_state[i].read() == OPEN) and (r_address[i].read() == address)

#if CC_XCACHE_MULTI_CPU
                 and (r_cpu_id [i].read() == cpu_id)
#endif
                  )
            { 
                num_slot = i;
                found = true;
                m_stat_nb_write_accepted_in_open_slot ++;
                break;
            }
        }
        // second : search an empty slot
        if( !found )
        {
            for( size_t i=0 ; i<m_nslots ; i++)
            {
                if( r_state[i] == EMPTY )
                { 
                    num_slot = i;
                    found = true;
                    break;
                }
            }
        }

        // register the request when a slot has been found:
        // update r_state, r_address, r_be, r_data, r_min, r_max, r_tout
        if ( found )
        {
            // if instruction is uncached, force the lock (no rewrite in the same slot)
            r_state  [num_slot] = (cached)?OPEN:LOCKED;
            r_address[num_slot] = address;
            r_cacheable [num_slot] = cached;

#if CC_XCACHE_MULTI_CPU
            r_cpu_id [num_slot] = cpu_id;
#endif

            if (r_be[num_slot][word] != 0) m_stat_nb_write_accepted_in_same_word ++;

            r_be[num_slot][word]   = r_be[num_slot][word] | be;
            data_t  data_mask = 0;
            be_t    be_up = (1<<(sizeof(data_t)-1));
            for (size_t i = 0 ; i < sizeof(data_t) ; ++i) 
            {
                data_mask <<= 8;
                if ( be_up & be ) data_mask |= 0xff;
                be <<= 1;
            }
            r_data[num_slot][word] = (r_data[num_slot][word] & ~data_mask) 
                                     | (data & data_mask) ;
 
            if ( r_min[num_slot].read() > word ) r_min[num_slot] = word;
            if ( r_max[num_slot].read() < word ) r_max[num_slot] = word;
        }
        else
        {
            m_stat_nb_write_refused ++;
        }
        
        m_stat_nb_write ++;

        return found;
    } // end write()

    //////////////////////////////////////
    inline addr_t getAddress(size_t word)
    {
      return ( (addr_t)r_address[r_ptr_read.read()].read() + (addr_t)(word << 2) ) ;
    } 

    ///////////////////////////////////
    data_t inline getData(size_t word)
    {
      return r_data[r_ptr_read.read()][word] ;
    } 

    //////////////////////////////
    be_t inline getBe(size_t word)
    {
      return r_be[r_ptr_read.read()][word] ;
    } 

    /////////////////////////
    size_t inline getIndex()
    {
      return r_ptr_read.read();
    } 

    /////////////////////////
    size_t inline getCpuId(size_t index)
    {
#if CC_XCACHE_MULTI_CPU
      return r_cpu_id[index].read();
#else
      return 0;
#endif
    }

    /////////////////////////////////////////////////////////////
    // This method is intended to be used by the VCI_RSP FSM.
    // It changes a slot state from SENT to EMPTY when
    // the corresponding write transaction is completed.
    bool inline completed(size_t index)
    {
        assert( (index < m_nslots) && (r_state[index].read() == SENT) &&
              "write buffer error : illegal completed command received");

        r_max[index]        = 0 ;
        r_min[index]        = m_nwords - 1 ;
        r_state[index]      = EMPTY ;
        for( size_t w = 0 ; w < m_nwords ; w++ ) r_be[index][w] = 0 ;

        m_stat_sum_latence += (m_stat_nb_cycles-m_stat_cycle_sent [index]);

        return r_cacheable[index].read();
    } //end completed()

    /////////////////////////////////////////////////////////////////////// 
    MultiWriteBuffer(const std::string &name, 
                     size_t             wbuf_nwords, 
                     size_t             wbuf_nslots,
                     size_t             cache_nwords)
      :
      m_nslots(wbuf_nslots),
      m_nwords(wbuf_nwords),
      m_wbuf_line_mask((wbuf_nwords << 2) - 1),
      m_cache_line_mask((cache_nwords << 2) - 1)
    {
      r_address = new sc_signal<addr_t>[wbuf_nslots];
      r_cacheable  = new sc_signal<bool  >[wbuf_nslots];
      r_min     = new sc_signal<size_t>[wbuf_nslots];
      r_max     = new sc_signal<size_t>[wbuf_nslots];
      r_state   = new sc_signal<int>[wbuf_nslots];
#if CC_XCACHE_MULTI_CPU
      r_cpu_id  = new sc_signal<size_t>[wbuf_nslots];
#endif
      r_data    = new data_t*[wbuf_nslots];
      r_be      = new be_t*[wbuf_nslots];
      for( size_t i = 0 ; i < wbuf_nslots ; i++ )
        {
          assert( ((wbuf_nwords ==  1) || 
                   (wbuf_nwords ==  2) ||
                   (wbuf_nwords ==  4) || 
                   (wbuf_nwords ==  8) ||
                   (wbuf_nwords == 16)) &&
                  " the number of words must be a pawer of 2");
     
          r_data[i] = new data_t[wbuf_nwords];
          r_be[i]   = new be_t[wbuf_nwords];
        }

      m_stat_cycle_sent       = new stat_t [wbuf_nslots];
      m_stat_slot_utilization = new stat_t [wbuf_nslots+1];
      m_stat_word_utilization = new stat_t [wbuf_nwords];
    }
    ///////////////////
    ~MultiWriteBuffer()
    {
      delete [] m_stat_word_utilization;
      delete [] m_stat_slot_utilization;
      delete [] m_stat_cycle_sent;

      for( size_t i = 0 ; i < m_nslots ; i++ )
        {
          delete [] r_data[i];
          delete [] r_be[i];
        }
      delete [] r_data;
      delete [] r_be;
      delete [] r_cacheable;
      delete [] r_address;
      delete [] r_min;
      delete [] r_max;
      delete [] r_state;
#if CC_XCACHE_MULTI_CPU
      delete [] r_cpu_id;
#endif

    }
  };

} // end name space soclib

#endif /* SOCLIB_MULTI_WRITE_BUFFER_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


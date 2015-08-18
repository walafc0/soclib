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
 * Copyright (c) UPMC, Lip6, Asim
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *         Alain Greiner <alain.greiner@lip6.fr>, 2003
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_CABA_GENERIC_FIFO_H
#define SOCLIB_CABA_GENERIC_FIFO_H

#include <cstring>
#include <systemc>
#include "register.h"

namespace soclib { namespace caba {

using namespace sc_core;

template<typename T>
class GenericFifo
{
    T *m_data;
    sc_signal<int>    r_ptr;
    sc_signal<int>    r_ptw;
    sc_signal<int>    r_fill_state;
    int m_depth;
public:
    typedef T data_t;

    void print (void) const
    {
        std::cout << "r_fill_state  : " << r_fill_state << std::endl;
        std::cout << "r_ptr/ptw/max : " << r_ptr << " - " << r_ptw << " - " << m_depth << std::endl;
        std::cout << "m_data        : " << std::hex;
        int i = 0;
        while (i < r_fill_state)
        {
            std::cout << m_data[(r_ptr+i)%m_depth] << ", ";
            i++;
        }
        std::cout << std::dec << "end" << std::endl;
    }

    size_t size() const
    {
        return m_depth;
    }

    void init()
    {
        r_ptr = 0;
        r_ptw = 0;
        r_fill_state = 0;
        // To avoid potential errors from memory checkers
        std::memset(m_data, 0, sizeof(T) * m_depth);
    }

    inline uint32_t filled_status() const
    {
        return (uint32_t) r_fill_state;
    }

    inline bool empty() const
    {
        return !(uint32_t) r_fill_state;
    }

    inline bool full() const
    {
        return r_fill_state == m_depth;
    }

    void simple_put(const T &din)
    {
        if (r_fill_state != m_depth) { 
            r_fill_state = r_fill_state + 1;
            r_ptw = (r_ptw + 1) % m_depth;
            m_data[r_ptw] = din; 
        }
    }

    void simple_get()
    {
        if (r_fill_state != 0) {
            r_fill_state = r_fill_state - 1;
            r_ptr = (r_ptr + 1) % m_depth;
        }
    }

    void put_and_get(const T &din)
    {
        if (r_fill_state == m_depth) {
            r_fill_state = r_fill_state - 1;
            r_ptr = (r_ptr + 1) % m_depth;
        }
        else if (r_fill_state == 0) {
            r_fill_state = r_fill_state + 1;
            r_ptw = (r_ptw + 1) % m_depth;
            m_data[r_ptw] = din; 
        }
        else {
            r_ptr = (r_ptr + 1) % m_depth;
            r_ptw = (r_ptw + 1) % m_depth;
            m_data[r_ptw] = din; 
        }
    }

    void update (bool get, bool put, const T& din)
    {
        if (put and get)
            put_and_get(din);
        else
        {
            if (put)
                simple_put(din);
            if (get)
                simple_get();
        }
    }

    inline bool rok() const
    {
        return (r_fill_state != 0);
    }

    inline bool wok() const
    {
        return (r_fill_state != m_depth);
    }

    inline const T &read() const
    {
        return m_data[r_ptr];
    }

    GenericFifo(const std::string &name, size_t depth)
        : m_data(new T[depth]),
          r_ptr((name+"_r_ptr").c_str()),
          r_ptw((name+"_r_ptw").c_str()),
          r_fill_state((name+"_r_fill_state").c_str()),
          m_depth(depth)
    {
    }

    ~GenericFifo()
    {
        delete [] m_data;
    }
};

}}

#endif /* SOCLIB_CABA_GENERIC_FIFO_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


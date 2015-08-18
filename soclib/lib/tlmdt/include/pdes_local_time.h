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
 * Maintainers: fpecheux, alinev
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef __PDES_LOCAL_TIME_H__
#define __PDES_LOCAL_TIME_H__

#include <systemc>
#include <tlmdt>

//
// pdes_local_time class
//
// The pdes_local_time class is used to keep track of the local time in
// an initiator and to synchronize through the sending of null messages.
//  
class pdes_local_time
{

public:
  
  pdes_local_time( sc_core::sc_time time_quantum = 100 * UNIT_TIME )     // time quantum
    : m_local_time(sc_core::SC_ZERO_TIME)
  {
    m_time_quantum = time_quantum;
    m_next_sync_point = time_quantum;
  }

  ~pdes_local_time() {}

  //
  // Add the value informed to the local time
  //
  void add(const sc_core::sc_time& t)
  {
    m_local_time += t;
  }

  //
  // Helper functions to set the informed value to time the initiator.
  //
  void set(sc_core::sc_time t)
  {
    m_local_time = t;
  }
  
  //
  // Helper functions to get the time the initiator.
  // This time should be passed to a target in the nb_transport call
  //
  sc_core::sc_time get() const
  {
    return m_local_time;
  }

  //
  // Checks if a synchronization is required for this initiator. This will
  // be the case if the local time becomes greater than the local (current)
  // quantum value for this initiator.
  //
  bool need_sync()
  {
    if(m_local_time >= m_next_sync_point){
      compute_next_sync_point();
      return true;
    }
    return false;
  }

  //
  // Computes the value of the next local quantum.
  // This method should be called when an initiator sends a message
  //
  void reset_sync() 
  {
    compute_next_sync_point();
  }
  
protected:

  //
  // Computes the value of the next local quantum.
  // This method should be called when an initiator needs a synchronization
  //
  void compute_next_sync_point() 
  {
    m_next_sync_point = m_local_time + m_time_quantum;
  }
  
  
protected:
  sc_core::sc_time m_next_sync_point;
  sc_core::sc_time m_local_time;
  sc_core::sc_time m_time_quantum;
};

#endif

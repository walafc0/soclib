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

#ifndef __PDES_ACTIVITY_STATUS_H__
#define __PDES_ACTIVITY_STATUS_H__

//
// pdes_activity_status class
//
// The pdes_activity_status class is used to manage the initiator activity status.
//  
class pdes_activity_status
{

 public:
  
  pdes_activity_status()
    : m_activity(true)
  {
  }

  ~pdes_activity_status() {}

  //
  // Helper functions to set the activity status the initiator.
  //
  void set(bool a)
  {
    m_activity = a;
  }

 
  //
  // Helper functions to get the activity status the initiator.
  //
  bool get() const
  {
    return m_activity;
  }
  
 protected:
  bool m_activity;
};

#endif

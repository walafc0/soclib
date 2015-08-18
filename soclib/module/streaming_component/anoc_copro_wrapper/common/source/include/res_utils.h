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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors :
 * 
 * History :
 *
 * Comment :
 *
 */

#ifndef _RES_UTILS_H_
#define _RES_UTILS_H_

#include <string>
#include <iostream>

using namespace std;

#include "anoc_common.h"
#include "res_path.h"
#include "ni_utils.h"
//#include "res_multicore.h"

typedef enum {
  FULL_SYSC,
  CORE_RTL,
  UNIT_RTL,
  TEST_RTL,
  PC_RTL,
  TOP_RTL
} res_sim_level;

void set_path_appli();
void set_path_noccfg();
void set_path_powerdata();

res_sim_level get_sim_level(string unit_name);

// ------------------------------------------
// -- class cpu_struct_res
// ------------------------------------------

class cpu_struct_res {
 public:
  t_uint16 nb_cores;
  t_uint16 nb_fifo_in;
  t_uint16 *size_fifo_in; // @[nb_fifo_in]
  t_uint16 *core_fifo_in; // @[nb_fifo_in]
  t_uint16 nb_fifo_out;
  t_uint16 *size_fifo_out; // @[nb_fifo_out]
  t_uint16 *core_fifo_out; // @[nb_fifo_out]
  t_uint16 ctx_size_icc;
  t_uint16 ctx_size_occ;
  t_uint16 ctx_size_core;
  t_uint16 nb_sr_out;
  t_uint16 nb_cfg_icc;
  t_uint16 nb_cfg_occ;
  t_uint16 *nb_cfg_core; // @[nb_cores]
  t_uint16 *core_cfg_loaded; // @[nb_cores]
  t_uint16 nb_tasks;
  t_uint16 ctx_memory_size;
  cpu_struct_res();
  ~cpu_struct_res();

  // Method
  void get_struct_info(string res_name, int level);
};


int count_ones(sc_lv<16> word);


#endif /* _RES_UTILS_H_ */

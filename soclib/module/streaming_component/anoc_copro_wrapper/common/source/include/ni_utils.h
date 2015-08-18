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

#ifndef _NI_UTILS_H_
#define _NI_UTILS_H_

#include <string>
#include <vector>
using std::string;
#include "types.h"

// extract subname in the hierarchy from a full sc_module name
// ex: name=top.module1.submodule2.module =>
//            3       2          1      0
//     pos=2
// return: module1
string utils_extract_subname(string name, int pos);

// extract topbname in the hierarchy from a full sc_module name
// ex: name=top.module1.submodule2.module =>
//            3       2          1      0
//     pos=2
// return: top.module1
string utils_extract_topname(string name, int pos);


// remove brackets from param=[XX]
// if XX is not a positive integer return -1
int utils_remove_bracket(string param);

// remove braces from param={XX}
// if XX is not a positive integer return -1
int utils_remove_brace(string param);

// convert hexadecimal string (ex: "12F2") in integer (ex: 4850)
int hexstring2int(string param);

// convert binary string (ex: "1001") in integer (ex: 9)
int binstring2int(string param);

// read data from a file
// type      : type for expring
// level     : exprint level
// name      : module name (for exprint)
// file_name : file name to open
// v_data    : vector to store read data
void load_data_file(string type, t_uint16 level, string name, string file_name, std::vector<t_uint32> *v_data);

// sum the data in a tab
t_uint32 sum_tab(t_uint32 *tab, t_uint32 size);

#endif /* _NI_UTILS_H_ */

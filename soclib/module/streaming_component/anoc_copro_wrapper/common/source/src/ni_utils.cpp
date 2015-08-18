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

#include "ni_utils.h"
#include <sstream>
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;
using std::hex;

#ifdef SC_HIERARCHY_CHAR
#define HIERARCHY_CHAR SC_HIERARCHY_CHAR
#else
#define HIERARCHY_CHAR '.' 
#endif

#ifndef EXPRINTN
#define EXPRINTN(cat, print_level, name, str)
#endif

#ifndef EXPRINTNL
#define EXPRINTNL(cat, print_level, name, print_line, print_string)  
#endif

// extract subname in the hierarchy from a full sc_module name
// ex: name=top.module1.submodule2.module =>
//            3       2          1      0
//     pos=2
// return: module1
string utils_extract_subname(string name, int pos) {

  int idx=0;
  string return_name;

  for(int i=0; i<=pos; i++) {
    //cout << "i=" << i << endl;
    //idx=name.find_last_of('.');
    idx=name.find_last_of(HIERARCHY_CHAR);
    //cout << "idx=" << idx << endl;
    if(idx<0) {
      if(i==pos) {
        return name;
      } else {
        return "";
      }
    }
    return_name = name.substr(idx+1);
    name = name.substr(0,idx);
  }

  return return_name;

}

// extract topbname in the hierarchy from a full sc_module name
// ex: name=top.module1.submodule2.module =>
//            3       2          1      0
//     pos=2
// return: top.module1
string utils_extract_topname(string name, int pos) {

  int idx=0;
  string return_name;

  for(int i=0; i<=pos; i++) {
    //cout << "i=" << i << endl;
    //idx=name.find_last_of('.');
    idx=name.find_last_of(HIERARCHY_CHAR);
    //cout << "idx=" << idx << endl;
    if(idx<0) {
      if(i==pos) {
        return name;
      } else {
        return "";
      }
    }
    return_name = name;
    name = name.substr(0,idx);
  }

  return return_name;

}

// remove bracket from param=[XX]
// if XX is not a positive integer return -1
int utils_remove_bracket(string param) {
  int value=-1;
  int length = param.length();
  std::istringstream line;

  if(length<3) {
    cout << "WARNING : syntax invalid : " << param << endl;
  }
  if(length >=3 && param[0]=='[' && param[length-1]==']') {
    param=param.substr(1,length-2);
    line.clear();
    line.str(param + "\n");
    line >> value;
    if(!line) {
      value = -1;
    }
  }

  return value;
}

// remove brace from param={XX}
// if XX is not a positive integer return -1
int utils_remove_brace(string param) {
  int value=-1;
  int length = param.length();
  std::istringstream line;

  if(length<3) {
    cout << "WARNING : syntax invalid : " << param << endl;
  }
  if(length >=3 && param[0]=='{' && param[length-1]=='}') {
    param=param.substr(1,length-2);
    line.clear();
    line.str(param + "\n");
    line >> value;
    if(!line) {
      value = -1;
    }
  }

  return value;
}

// convert hexadecimal string (ex: "12F2") in integer (ex: 4850)
int hexstring2int(string param) {
  int value = 0;
  int length = param.length();
  char c;

  for(int i=0; i<length; i++) {
    c = param[length-1-i];
    switch(c) {
    case '0':
      break;
    case '1':
      value += (1 << (4*i));
      break;
    case '2':
      value += (2 << (4*i));
      break;
    case '3':
      value += (3 << (4*i));
      break;
    case '4':
      value += (4 << (4*i));
      break;
    case '5':
      value += (5 << (4*i));
      break;
    case '6':
      value += (6 << (4*i));
      break;
    case '7':
      value += (7 << (4*i));
      break;
    case '8':
      value += (8 << (4*i));
      break;
    case '9':
      value += (9 << (4*i));
      break;
    case 'A': case 'a':
      value += (10 << (4*i));
      break;
    case 'B': case 'b':
      value += (11 << (4*i));
      break;
    case 'C': case 'c':
      value += (12 << (4*i));
      break;
    case 'D': case 'd':
      value += (13 << (4*i));
      break;
    case 'E': case 'e':
      value += (14 << (4*i));
      break;
    case 'F': case 'f':
      value += (15 << (4*i));
      break;
    default:
      return -1;
    }
  }
  return value;
}

// convert binary string (ex: "1001") in integer (ex: 9)
int binstring2int(string param) {
  int value = 0;
  int length = param.length();
  char c;

  for(int i=0; i<length; i++) {
    c = param[length-1-i];
    switch(c) {
    case '0':
      break;
    case '1':
      value += (1 << i);
      break;
    default:
      return -1;
    }
  }
  return value;
}

// read data from a file
// level     : print level
// name      : module name (for print)
// file_name : file name to open
// v_data    : vector to store read data
void load_data_file(string type, t_uint16 level, string name, string file_name, std::vector<t_uint32> *v_data) {

  char keychar;
  bool skipped_line = false;
  string read_line;
  t_uint32 nb_read_line = 0;
  t_uint32 nb_read_data = 0;
  std::istringstream config_line(read_line);

  t_uint32 data_tmp;

  v_data->clear();

  // open configuration file
  std::ifstream file_ref(file_name.c_str());
  if (!file_ref) {
    EXPRINTN(type, 0, name, "ERROR : the file " << file_name << " does not exist");
    exit(0);
  }

  EXPRINTN(type, level, name, "  | Reading data file : " << file_name);


  nb_read_line = 0;
  while (file_ref.good()) {

    nb_read_line++;
    getline(file_ref,read_line);

    config_line.clear();
    config_line.str(read_line + "\n");
    // add end of line '\n' character otherwise single keychar line failed
    config_line >> keychar;

    skipped_line = false;
    if(!config_line) {
      // blank line skipped
      skipped_line = true;
    }
    else if(keychar=='#') {
      // comments => line skipped
      skipped_line = true;
    }
    else if (keychar == '>') {
      config_line >> hex >> data_tmp;
      v_data->push_back(data_tmp);
      nb_read_data++;
    }
    else {
      EXPRINTNL(type, 0, name, nb_read_line, "  | WARNING : invalid line : " << read_line); //FIXPRINT
      skipped_line = true;
    }
    if(!skipped_line) {
      // test the end of line
      config_line >> keychar;
      if(config_line && keychar!='#') {
        EXPRINTNL(type, 0, name, nb_read_line, "  | WARNING : too many arguments");
      }
    }
  }

  file_ref.close();

  EXPRINTN(type, level, name, "  | End reading"); //FIXPRINT

  EXPRINTN(type, level, name, "  | " <<  v_data->size() << " data read from " << file_name);

}

// sum the data in a tab
t_uint32 sum_tab(t_uint32 *tab, t_uint32 size) {
  t_uint32 sum = 0;
  for(unsigned int i=0; i<size; i++) {
    sum += tab[i];
  }
  return sum;
}

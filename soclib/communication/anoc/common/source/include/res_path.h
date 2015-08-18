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
 * Comment : Path for configuration files
 *
 */

#ifndef _RES_PATH_H_
#define _RES_PATH_H_

#include <string>

// configuration file path
#define DEFAULT_APPLI "./appli/"
#define DEFAULT_NOCCFG "./noccfg/"
#define DEFAULT_POWERDATA "./power_data/"

// path for appli files
extern std::string path_appli;		

// path for noccfg files
extern std::string path_noccfg;

// path for power_data files
extern std::string path_powerdata;

// Path for structure files
//#define PATH_STRUCT   "./data/struct/"
//#define PATH_STRUCT   "./noccfg/"
#define PATH_STRUCT path_noccfg

// Extension for structure files
#define EXT_STRUCT    ".str"

// Extension for scenario files
#define EXT_SNO    ".sno"


// Path for configuration files
#define PATH_CONFIG_SYMBOL    "./data/cfg_symbol/"

// Path for trace files
//#define PATH_TRACE_FILE       "./appli/trace/"
#define PATH_TRACE_FILE path_appli+"trace/"

// Extensions for trace files
#define EXT_TRACE_FILE "_sysc.out"
#define EXT_UNIT_TRACE_FILE "_unit.out"
#define EXT_TEST_TRACE_FILE "_test.out"
#define EXT_PC_TRACE_FILE "_pc.out"
#define EXT_TOP_TRACE_FILE "_top.out"

// Extensions for configuration files
//#define EXT_CONFIG_SYMBOL           ".cfg"
//#define EXT_ICC_CONFIG_SYMBOL   "_icc.cfg"
//#define EXT_OCC_CONFIG_SYMBOL   "_occ.cfg"
//#define EXT_CORE_CONFIG_SYMBOL "_core.cfg"

// Catalog file path
//#define PATH_CFG_CATALOG "./appli/"
#define PATH_CFG_CATALOG path_appli

// Catalog file names
#define  NAME_ICC_CFG_CATALOG  "icc.cfg"
#define  NAME_OCC_CFG_CATALOG  "occ.cfg"
#define NAME_CORE_CFG_CATALOG "core.cfg"
#define NAME_MULTICORE_CFG_CATALOG "core%i.cfg"
//#define NAME_TASK_CFG_CATALOG "task.cfg"
#define  NAME_CTX_CFG_CATALOG  "ctx.cfg"

// Extensions for context files
#define EXT_ICC_CTX   "_icc.ctx"
#define EXT_OCC_CTX   "_occ.ctx"
#define EXT_CORE_CTX "_core.ctx"
#define EXT_CPU_CTX   "_cpu.ctx"
#define EXT_NI_CTX        ".ctx"

// Extension for task files
//#define EXT_TASK  ".tsk"

// Path for application scenario files
//#define PATH_APPLI_SNO        "./appli/sno/"
#define SNO_DIR "sno"
#define PATH_APPLI_SNO path_appli+SNO_DIR+"/"

// Scenario directory file name
#define FILE_SNO_NAME "sno.name"

// CPU Scenario file name
#define FILE_CPU_SNO "cpu.sno"

// INIT Scenario file name
#define FILE_INIT_SNO "init.sno"

// Path for CPU load files
//#define PATH_UNIT_LOAD "./noccfg/"
#define PATH_UNIT_LOAD path_noccfg

// Extension for load files
#define EXT_UNIT_LOAD ".load"

// Extension for generator files
#define EXT_GEN_FILE ".gen"

// Extension for receptor files
#define EXT_REC_FILE ".rec"

// File for appli path
#define FILE_APPLI_NAME "appli.cfg"

// File for noccfg path
#define FILE_NOCCFG_NAME "noccfg.cfg"

// File for power_data path
#define FILE_POWERDATA_NAME "power_data.cfg"

// File for unit simulation levels
#define FILE_SIMLEVEL_NAME "simlevel.cfg"

#endif /* _RES_PATH_H_ */

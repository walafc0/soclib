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

#ifndef _ANOC_COMMON_H_
#define _ANOC_COMMON_H_


/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/

#ifdef MENTORG /* No comment... */
//#define USE_SC_STRING_OLD /* For SystemC 2.1 v1: use previous version as sc_string class */
#define USE_STD_STRING /* For SystemC 2.1 v1: use standard string class as sc_string */
#else
//#define SC_USE_SC_STRING_OLD /* For SystemC 2.1 v1: use previous version as sc_string class */
#define SC_USE_STD_STRING /* For SystemC 2.1 v1: use standard string class as sc_string */
#endif

// for SystemC 2.1 release: this flag enables dynamic processes
// must be define before SystemC lib inclusion
#define SC_INCLUDE_DYNAMIC_PROCESSES


#include "systemc.h"
#include <map>
#include <iomanip>


/*------------------------------------------------------------------------------
 * DEFINES
 *----------------------------------------------------------------------------*/

#include "magali_platform_version.h"

#define MAGALI_PLATFORM  MAGALI_PLATFORM_SIMU_V2     // used only in this file
#define F2_API_CPU_SYSTEMC     // used only in types.h


/*------------------------------------------------------------------------------
 * TLM
 *----------------------------------------------------------------------------*/

#include "tlm.h"
using namespace tlm;

#include "magali_platform_version.h"

#if MAGALI_PLATFORM == MAGALI_PLATFORM_SIMU_V1
namespace dvk_tlm_transrecord {}
using namespace dvk_tlm_transrecord;
#endif /* MAGALI_PLATFORM == MAGALI_PLATFORM_SIMU_V1 */

#if MAGALI_PLATFORM == MAGALI_PLATFORM_SIMU_V2
#include "tlm_tac2.h"
using namespace dvk_tlm;
using namespace dvk_tlm_transrecord;
using namespace dvk_tlm_utilities;
#endif /* MAGALI_PLATFORM == MAGALI_PLATFORM_SIMU_V2 */

/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/

#define TLM_ANOC_VERSION "2.0"

// SST2 file creation for visualisation of transactions 
//#define TLM_TRANS_RECORD => moved to DFLAGS in Makefile.def

#if defined(TLM_TRANS_RECORD_SDI2) && defined(TLM_TRANS_RECORD_SCV)     // transaction recording formats are exclusive
#undef TLM_TRANS_RECORD_SDI2                                            // SCV format is default if both are defined
#endif

#ifdef TLM_TRANS_RECORD_SDI2
#define DATA_BASE_NAME "SST2_DB"
// #define TLM_TRANS_RECORD
#endif // TLM_TRANS_RECORD_SDI2

#ifdef TLM_TRANS_RECORD_SCV
#define DATA_BASE_NAME "SCV_DB.txt"
// #define TLM_TRANS_RECORD
#endif // TLM_TRANS_RECORD_SCV

// #ifdef TLM_TRANS_RECORD
// #define DATA_BASE_NAME "SST2_DB"
// #endif // TLM_TRANS_RECORD

// VCD file creation for visualisation of sc_signals
#define TRACE_VCD

#ifdef TRACE_VCD
#define VCD_FILE_NAME "SIG_trace"
#endif //TRACE_VCD

// Print general information on simulation...
#define VERBOSE_GLOBAL
 
// time base / VCD & SST2 files consideration
#define CLOCK_PERIOD 20
#define TIMESCALE -9

// Minimum step of simulation ( wait(sc_time) )
#define STEP_SIM CLOCK_PERIOD
#define STEP_SIM_UNIT SC_PS

// reset duration for co-simulated blocks
#define RESET_STARTED 2
#define RESET_DURATION 100
#define RESET_RELEASED 2

// clk ref period
#define CLK_REF_PERIOD 10000

// default debug level : 0 / 1 / 2
// this value can be overiden in simulation by main() -debug option
#define DEBUG_LEVEL 0

// usefull macro to help print txt info, including simulation time, and instance name
// (can only be used within sc_module's(), in order to name() function to be defined)
#define PRINT(print_level, print_string) 	\
	if (print_level<=debug_level)		\
	  cout << std::setw(6)			\
	       << sc_time_stamp() << ": "	\
	       << this->name() << ": "		\
	       << print_string			\
	       << endl

#define ERROR(error_string)			\
	{ PRINT(0,"ERROR : " << error_string);	\
	anoc_close_simu(0); }

// usefull macro to help print txt info outside sc_modules.
#define PRINTN(print_level, name_string, print_string) 	\
	if (print_level<=debug_level)		\
	  cout << std::setw(6)			\
	       << sc_time_stamp() << ": "	\
	       << name_string << ": "	        \
	       << print_string			\
	       << endl

#define PRINTL(print_level, print_line, print_string)	\
  PRINT(print_level, "[LINE " << print_line << "] " << print_string) 

#define PRINTNL(print_level, name_string, print_line, print_string)	\
  PRINTN(print_level, name_string, "[LINE " << print_line << "] " << print_string) 

#define COUTV(cout_var) cout << #cout_var << ": " << cout_var << endl;

#define EXPRINT(cat, print_level, str) \
  do { \
    if (EXPRINT_LEVELS.find(#cat) == EXPRINT_LEVELS.end()) \
      EXPRINT_LEVELS[#cat] = debug_level; /* default trace level */ \
    if (print_level <= EXPRINT_LEVELS[#cat]) \
      cout << std::setw(6) << sc_time_stamp() << ": [" << #cat << "] " \
	   << this->name() << ": " << str << endl; \
  } while(false)

#define EXPRINTN(cat, print_level, name, str) \
  do { \
    if (EXPRINT_LEVELS.find(#cat) == EXPRINT_LEVELS.end()) \
      EXPRINT_LEVELS[#cat] = debug_level; /* default trace level */ \
    if (print_level <= EXPRINT_LEVELS[#cat]) \
      cout << std::setw(6) << sc_time_stamp() << ": [" << #cat << "] " \
	   << name << ": " << str << endl; \
  } while(false)

#define EXPRINTL(cat, print_level, print_line, print_string)	\
  EXPRINT(cat, print_level, "[LINE " << print_line << "] " << print_string) 

#define EXPRINTNL(cat, print_level, name, print_line, print_string)	\
  EXPRINTN(cat, print_level, name, "[LINE " << print_line << "] " << print_string) 

//exprint function, called by the application code.
//all the messages fromù the application uses the "app" category
extern "C" void exprint(char const *cat_, int debug_level_, char const *name_, char const *format_, ...);

// usefull macro to help fifo trace declaration
// #define TRACE_FIFO(fifo_name) fifo_name.trace(pt_trace_file,#fifo_name)
#define TRACE_FIFO(fifo_name) fifo_name.trace(pt_trace_file,std::string(name()) + "." + #fifo_name)

#include "types.h"

/*------------------------------------------------------------------------------
 * Global Variables							       
 *----------------------------------------------------------------------------*/


// Effective  VCD file record ( parameter of command line -vcd )
extern bool VCD_Record;

// trace file pointer
extern sc_trace_file *pt_trace_file;

// debug level for printing messages
extern int debug_level;

// Performance statistics printing
extern bool print_perf;

// Indicate if a database has been open (-trans)
extern bool data_base_to_close;

extern std::map<std::string, int> EXPRINT_LEVELS;
		
/*------------------------------------------------------------------------------
 * Functions							       
 *----------------------------------------------------------------------------*/

extern void anoc_init_simu();
extern void anoc_close_simu(int arg);
inline void set_exprint_level(char *name, unsigned int level) {
  EXPRINT_LEVELS[*new std::string(name)] = level;
}
inline void set_exprint_level(std::string name, unsigned int level) {
  EXPRINT_LEVELS[*new std::string(name)] = level;
}

#endif /* _ANOC_COMMON_H_ */

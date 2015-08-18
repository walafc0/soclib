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

/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/

#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>

#include "anoc_common.h"

#ifdef TLM_TRANS_RECORD  
//#include "tlm_transaction_counter.h"
#include "tlm_transrecord_database.h"
#endif  // TLM_TRANS_RECORD


/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 * Global Variables							       
 *----------------------------------------------------------------------------*/


// Effective  VCD file record ( parameter of command line -vcd )
bool VCD_Record = false;

// Indicate if a database has been open (-trans)
bool data_base_to_close = false;

// trace file pointer
sc_trace_file *pt_trace_file = NULL;

// debug level for printing messages : 0 / 1 / 2
int debug_level = DEBUG_LEVEL;

// Performance statistics printing
bool print_perf = true;

// Exprint trace levels
std::map<std::string, int> EXPRINT_LEVELS;
		

/*------------------------------------------------------------------------------
 * anoc_init_simu()							       
 *----------------------------------------------------------------------------*/

void anoc_init_simu() {

  // Resolution time ( time stamp ) in nano second;
#ifdef NC_SYSTEMC
  sc_set_time_resolution( 1, SC_NS );
#endif

  // transaction generation
#ifdef TLM_TRANS_RECORD  
  tlm_transrecord_database::enable_global_transaction_recording(); 
  if (tlm_transrecord_database::is_global_transaction_recording_enabled()) {
    tlm_transrecord_database::open_database(DATA_BASE_NAME, SC_NS);
  }
#endif // TLM_TRANS_RECORD

  // open & create trace file
#ifdef TRACE_VCD
  if (VCD_Record==true) {
    pt_trace_file = sc_create_vcd_trace_file(VCD_FILE_NAME);	// will automatically add .vcd extension
    ((vcd_trace_file *)pt_trace_file)->sc_set_vcd_time_unit(TIMESCALE);
  }
#endif // TRACE_VCD

  // Clean Stop SystemC
  signal(SIGINT,anoc_close_simu); // CTRL-C
}


/*------------------------------------------------------------------------------
 * End simulation & print results
 *----------------------------------------------------------------------------*/

void anoc_close_simu(int arg) {

  printf("\n\t\t\t\t---- End of simulation ----\n");  

  float simulation_time;
  simulation_time = (float)clock()/CLOCKS_PER_SEC;
  
#ifdef TLM_TRANS_RECORD  
  if (tlm_transrecord_database::is_global_transaction_recording_enabled()) {
    tlm_transrecord_database::close_database();
    printf("closed data base %s\n",DATA_BASE_NAME);
  }
#endif  // TLM_TRANS_RECORD
   
#ifdef TRACE_VCD
//   if ((VCD_Record==true) && (pt_trace_file!=NULL)) {
//     sc_close_vcd_trace_file(pt_trace_file);
//     printf("closed vcd file %s\n",VCD_FILE_NAME);
//   }
#endif // TRACE_VCD

  // print simulation performances
  if (print_perf)
    {
      fprintf(stderr, "\tSimulation Time: %9.9f s - Simulation step: %9.9f s\n",
	     (float)sc_time_stamp().to_seconds(),CLOCK_PERIOD*pow(10.0,TIMESCALE));
  
      fprintf(stderr, "\tReal simulation time: %9.4f s\n", simulation_time );
      printf("\tSimulation Time: %9.9f s - Simulation step: %9.9f s\n",
	     (float)sc_time_stamp().to_seconds(),CLOCK_PERIOD*pow(10.0,TIMESCALE));
  
      printf("\tReal simulation time: %9.4f s\n", simulation_time );
#ifdef TLM_TRANS_RECORD  
//       printf("\tReal simulation time: %9.4f s - Transactions: %u (%llu KBytes)\n",
// 	     simulation_time,
// 	     tlm_transaction_counter::get_transaction_number(),
// 	     tlm_transaction_counter::get_byte_number()/1024
// 	     );

//       printf("\tTransactions/Sec: %9.0f - KBytes/Sec: %9.0f\n",
// 	     (float)tlm_transaction_counter::get_transaction_number()/simulation_time,
// 	     (float)tlm_transaction_counter::get_byte_number()/(1024*simulation_time));   

//       printf("\tByte/Transaction(Av.) : %9.2f \n",
// 	     (float)tlm_transaction_counter::get_byte_number()/(float)tlm_transaction_counter::get_transaction_number());
#endif  // TLM_TRANS_RECORD

    }


  printf("\n\n");
}

/*------------------------------------------------------------------------------
 * dummy sc_main() function for MENTOR ModelSim simulation
 *----------------------------------------------------------------------------*/

// #ifdef MENTORG 
// int sc_main( int, char*[] ) {}
// #endif


//exprint function, called by the application code.
//all the messages fromù the application uses the "app" category
void exprint(char const *cat_, int print_level_, const char *name_, char const *format_, ...)
{
  if (EXPRINT_LEVELS.find(cat_) == EXPRINT_LEVELS.end())
    EXPRINT_LEVELS[cat_] = debug_level; /* default trace level */
  if (print_level_ <= EXPRINT_LEVELS[cat_])
  {
    char str[256];
    va_list args;
    va_start (args, format_);
    vsprintf (str,format_, args);
    va_end (args);

    cout << std::setw(6) << sc_time_stamp() << ": [" << cat_ << "] "
      << name_ << ": " << str << endl;
  }
}

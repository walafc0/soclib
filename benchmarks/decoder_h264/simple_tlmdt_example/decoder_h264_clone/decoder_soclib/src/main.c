/*****************************************************************************

  main.c -- real main
  - Open the file
  - Launch the decoding process

Authors:
Florian Broekaert, THALES COM - AAL, 2006-04-07
Fabien Colas-Bigey THALES COM - AAL, 2008
Pierre-Edouard BEAUCAMPS, THALES COM - AAL, 2009

Copyright (C) THALES & Martin Fiedler All rights reserved.

This code is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This code is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this code (see file COPYING).  If not, see
<http://www.gnu.org/licenses/>.

This License does not grant permission to use the name of the copyright
owner, except only as required above for reproducing the content of
the copyright notice.
*****************************************************************************/

/****************************************************************************
  Include section
****************************************************************************/
#include <stdio.h>
#include <string.h>

#include <mutek/printk.h>

#include "h264.h"
#include "conf.h"

/****************************************************************************
  Global variables and structures
****************************************************************************/
static pthread_t main_thread;
char *file_out_name = "TEST.YUV";


/****************************************************************************
  Sub Main functions
****************************************************************************/
void * main_process(void *arg);
void   do_bench(bool_t mode);


/****************************************************************************
  Main for both host and target implementations
****************************************************************************/
int32_t app_start(int32_t argc, char *argv[])
{
  /* Main thread: the app_start function cannot be considered as a thread */
  pthread_create(&main_thread, NULL, main_process, NULL);
  
  return 0;
}


/****************************************************************************
  Where the program really starts
****************************************************************************/
void * main_process(void *arg)
{
  //int err = 0;
  //void * ret = NULL;
  
  /* Open files, configure the system and reads the headers
     of the .264 sequence */
  if (!h264_open(file_in_name,file_out_name,slice_numbers))
    printk("-- MAIN -- Error while opening H.264 stream\n");

  /* Running the decoder */
  do_bench(1);

  /* Closing files and releasing threads, semaphores and others */
  h264_close();

  /* Killing the main thread once decoding is finished
  if((err=pthread_join(main_thread, &ret))){
    printk("-- MAIN ERROR -- 'pthread_join' error for main thread\n");
    abort();
  }*/
  
  printk("-- Main -- Main thread is finished\n");

  //cpu_mem_write_32(0xd4200000, 1);
  cpu_mem_write_32(SEG_SIMHELPER_ADDR, 1);

  printk("End of simulation\n");
  
  return 0;
}


/****************************************************************************
 * This function recursively calls decode_frame. It is used
 * to decode a whole sequence.
 * mode = 0 --> no perf evaluation
 * mode = 1 --> perf evaluation
 ****************************************************************************/
void do_bench(bool_t mode)
{
  void *frameptr = NULL;
  uint32_t cpu_cycle = 0;

#if (defined(CONFIG_DRIVER_TIMER_SOCLIB) || defined(CONFIG_DRIVER_TIMER_EMU))
  //init_timer();
  //printk("-- MAIN -- Initalization finished in %d ms\n", get_timer_value());
  cpu_cycle = cpu_cycle_count();
  printk("-- MAIN -- Initalization finished in %d cycles\n", cpu_cycle);
#endif

  if (mode==0)
    {
      do {
	frameptr = h264_decode_frame();
      }
      while(frameptr != NULL);
    }
  
  else
    {
      do {
	frameptr = h264_decode_frame();
      }
      while(frameptr != NULL);
    }
}

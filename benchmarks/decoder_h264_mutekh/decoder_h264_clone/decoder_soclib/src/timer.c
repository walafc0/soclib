/*****************************************************************************

  timer.c -- Function dealing with the timer 


Authors:
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
#include "timer.h"

/****************************************************************************
  Global variables and structures
****************************************************************************/
//#if defined(CONFIG_DRIVER_TIMER_SOCLIB)
//extern struct device_s icu_dev;
static struct device_s icu_dev[CONFIG_CPU_MAXCOUNT];
//#endif
// static struct device_s timer_dev[4];
extern struct device_s *timerms_dev;

/*************************************
   Timer Initialisation
*************************************/
void init_timer()
{

  /* ICU initialisations */
/*   printk("Start initialisation of ICU for %d cpus\n",CONFIG_CPU_MAXCOUNT); */
/*   int cpu = 0; */
/*   //for (cpu=0; cpu<CONFIG_CPU_MAXCOUNT;cpu++){ */
/*     struct device_s *icu = &icu_dev[cpu]; */
/*     device_init(icu); */
/*     icu->addr[0] = 0xd2200000; */
/*     icu->irq = 0; */
/*     icu->icudev = icu; */
/*     //struct xicu_filter_param_s params = { */
/*     //  .output_line = cpu, */
/*     //}; */
/*     //xicu_filter_init(icu,&params); */
/*     //} */
    
/*     //xicu_timer_setvalue(icu,0,1000000000); */
/*     xicu_timer_setvalue(icu,0,2147483647); */
/*     printk("test\n"); */
/*     printk("value 1 = %d\n",(uint32_t)xicu_timer_getvalue(icu,0)); */
/*     printk("value 2 = %d\n",(uint32_t)xicu_timer_getvalue(icu,0)); */
/*     printk("value 3 = %d\n",(uint32_t)xicu_timer_getvalue(icu,0)); */
/*     //} */






  /*----------------------------------------------------------------------------*/


  /* /\* ICU initialisations *\/ */
/*   printk("Start initialisation of ICU for %d cpus\n",CONFIG_CPU_MAXCOUNT); */
/*   int cpu = 0; */
/*   //for (cpu=0; cpu<CONFIG_CPU_MAXCOUNT;cpu++){ */
/*     struct device_s *icu = &icu_dev[cpu]; */
/*     device_init(icu); */
/*     icu->addr[0] = 0xd2200000; */
/*     icu->irq = 0; */
/*     icu->icudev = icu; */
/*     //struct xicu_filter_param_s params = { */
/*     //  .output_line = cpu, */
/*     //}; */
/*     //xicu_filter_init(icu,&params); */
/*     //} */
    
/*   /\* Timer Initialisations *\/ */
/*   printk("Start initialisation of Timer\n"); */
/*   cpu = 0; */
/*   //for (cpu=0; cpu<CONFIG_CPU_MAXCOUNT;cpu++){ */
  
/*   struct device_s * timer; */
/*   printk("test %d\n",(int)timer); */
    
/*     //struct device_s * timer = &timer_dev[cpu]; */
/*     //struct device_s * timer = device_get_child(&icu_dev[0],0); */
/*   timer = device_get_child(&icu_dev[0],0); */
/*     printf("helloo %d\n",(int)timer); */
/*     //device_init(timer); */
/*     //timer->addr[0] = 0x11220000; */
/*     //timer->irq = 0; */
/*     //timer->icudev = &icu_dev[cpu]; */
/*     //timer_soclib_init(&timer_dev,NULL); */
    
/*     //timer_soclib_setperiod(&timer_dev,0,2000000); */
/*     xicu_timer_setperiod(timer,0,100); */
/*     printk("test\n"); */
/*     printk("value 1 = %d\n",(uint32_t)xicu_timer_getvalue(timer,0)); */
/*     printk("value 2 = %d\n",(uint32_t)xicu_timer_getvalue(timer,0)); */
/*     printk("value 3 = %d\n",(uint32_t)xicu_timer_getvalue(timer,0)); */
/*     //} */

  
/*----------------------------------------------------------------------------*/

  /*
  device_init(&timer_dev);

#if defined(CONFIG_DRIVER_TIMER_EMU)
  timer_emu_init(&timer_dev, NULL, NULL);
#elif defined(CONFIG_DRIVER_TIMER_SOCLIB)
  timer_dev.addr[0] = DSX_SEGMENT_TIMER_ADDR;
  timer_dev.irq = 0;
  timer_dev.icudev = &icu_dev;
  timer_soclib_init(&timer_dev,NULL);
#endif
  */
}

/*************************************
  Return Timer Value
 *************************************/
uint32_t get_timer_value()
{/*
#if defined(CONFIG_DRIVER_TIMER_EMU)
	return (uint32_t) timer_emu_getvalue(&timer_dev, 0);
#elif defined(CONFIG_DRIVER_TIMER_SOCLIB)
	return (uint32_t) timer_soclib_getvalue(&timer_dev, 0);
#else
 */
  //xicu_timer_getvalue(timerms_dev,0);
  return -1;
  //#endif
}

/*************************************
  Set Timer Value
 *************************************/
void set_timer_value(uint32_t value)
{/*
#if defined(CONFIG_DRIVER_TIMER_EMU)
	timer_emu_setvalue(&timer_dev, 0, value);
#elif defined(CONFIG_DRIVER_TIMER_SOCLIB)
	timer_soclib_setvalue(&timer_dev, 0, value);
#endif
 */
  //xicu_timer_setvalue(&icu_dev[0],0,4294967295);
/*   xicu_timer_setvalue(timerms_dev,0,value); */

}





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
 * Copyright (C) IRISA/INRIA, 2007
 *         Francois Charot <charot@irisa.fr>
 *
 */

#include "soclib/timer.h"
#include "system.h"
#include "stdio.h"

#include "../segmentation.h"


static const volatile void* timer_address = TIMER_BASE;


int c[100];

int a[132] = { -2, 668, -10, 6019, 531, -3950, -275, 54, -1, -41, -6, -536, 1,
	       20, -10, -7564, 1918, -1095, 268, -3, 56, -25, 1069, 0, -20, 61, -2380,
	       -1, 60, 6845, 250, -49, -3038, -1001, -3, -12596, 39, -6, 0, -23386,
	       -22, -74, 25, -5, -3847, 7007, -1, 12, -103, 12, 5, -353, 7, -186, 504,
	       -549, 4, 11, -1, -23, 668, -691, -7822, 125, -3929, -1, -2077, 3, 121,
	       -1, -6, 0, -4, 1020, 517, -992, -1021, -12, 7050, 2, 5829, -124, 17,
	       213, -158, -114, -53, -2516, -1, -224, 2709, 772, -7603, 148, -6, -88,
	       198, 438, -124, -62, -125, -13, -6169, -3, 6762, 52, 2, -1, 0, 9,
	       17793, 21, 27, 476, -36, 483, -9, -40, 417, 205, -4185, -152, -3814,
	       1503, 2, -1, 12, 4, 1595, -171, -17, 421 };

int b[100] = { 2837, -77, 0, -4824, -1, 786, -737, -14, -935, -3, 0, 2, -22,
	       -1, 1, 0, 240, -6768, -1, -6086, 74, -1, -22, -1386, 10, 1193, 121,
	       -54, -568, 215, -5206, 1, 0, -2, 711, 1686, 0, -1, -17, -28424, -47,
	       76, -185, 191, -57, 29, -17, 662, -1196, -81, 25072, 1656, 6, -43,
	       19920, -839, 265, -9, -22, 192, 23728, 29276, 327, 232, 714,
	       -1, -1, 0, 537, 15445, 6, -81, -100, -619, -10993, 1, -9642, 107,
	       -1000, -911, -1483, -20, -11547, 3, -1432, -2, 1, 76, -546, -14713,
	       -19, 1, -1, 31, 0, -3106, 2, 1204, -455, -127 };

/* Sample C code */
void fir_n(const int x[], const int h[], int y[]) {
  int i, j, sum;
  int zz;
  printf("    entering fir_n function ");
  printf("\n");

  for (j = 0; j < 100; j++) {
    sum = 0;
    for (i = 0; i < 32; i++)
      sum += x[i + j] * h[i];
    sum = sum >> 15;
    y[j] = sum;
    printf("%d \n", sum);
  }
  printf("    leaving fir_n function ");
  printf("\n");
}

/*void fir(const int x[], const int h[], int y[])
  {
  int i, j, sum0, sum1;
  int x0,x1,h0,h1;

  for (j = 0; j < 100; j+=2)
  {
  sum0 = 0;
  sum1 = 0;
  x0 = x[j];
  for (i = 0; i < 32; i+=2)
  {
  x1 = x[j+i+1];
  h0 = h[i];
  sum0 += x0 * h0;
  sum1 += x1 * h0;
  x0 = x[j+i+2];
  h1 = h[i+1];
  sum0 += x1 * h1;
  sum1 += x0 * h1;
  }
  y[j] = sum0 >> 15;
  y[j+1] = sum1 >> 15;
  }
  }
*/

/****************************************************************************/
/* TOP LEVEL DRIVER FOR THE TEST.                                           */
/****************************************************************************/
int main(void) {
  uint32_t time = 0;
  int cpu = procnum();

  printf("hello From Nios II processor %d\n", procnum());

  printf("start FIR application excecution");
  printf("\n");

  soclib_io_set(timer_address, TIMER_VALUE, 0);
  soclib_io_set(timer_address, TIMER_MODE, TIMER_RUNNING);


  fir_n(a, b, c);

  //  time = soclib_io_get(timer_address, TIMER_VALUE);

  //  printf("\nInterruption caught on proc ");
  //  printf("%d", cpu); printf(" at time ");
  //  printf("%d \n", (int) time);
  //  printf("\n");

  printf("end of FIR testing ");
  printf("\n");

  int *p=(int*)0x0100A000;
  *p = -1;

  while (1)
    ;
  /*   TRAP; */

  return (0);
}


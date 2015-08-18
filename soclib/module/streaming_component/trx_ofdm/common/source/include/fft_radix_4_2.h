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
 * Authors : Dimitri KTENAS (CEA-LETI)
 * 
 */

#ifndef FFT_RADIX_4_2_H
#define FFT_RADIX_4_2_H

#include "anoc_common.h"
#include "res_path.h"

#define SYSTEMC

#ifndef SYSTEMC
#ifdef  __cplusplus
extern "C" {
#endif			
#else
#include "systemc.h"
#endif			

/*
** Application header types.
*/

#include "FixC.h"
 
/*
** System header file.
*/

#include <math.h>          /* for cos, sin, log, floor, pow */

/*
** Control flow structure.
*/

#define MAX_SIZE_FFT	2048	/* max size of the FFT : for internal array */
#define	MAX_BIT_REVERSE	11	/* max number of bits for the bit reverse internal array : 2^11=2048 */


typedef struct {

  int FftSize;	         /*	size of the Fast Fourier Transform      	*/
  int FftType;	         /*	FftType = '1' : FFT , FftType = '0' : IFFT	*/
  int OverflowTest;      /*     OverflowTest = '1' : test of the overflow       */
  int NormalizationPower; /*     NormalizationPower = '1' means normalization    */

} FFT_RADIX_4_CTRL;


/*
** Function prototype
*/

//extern void FftRadix_4          (COMPLEX *ptVectIn, COMPLEX *ptVectOut, FFT_RADIX_4_CTRL *ptCtrl);
  //extern void FftRadix_4_2        (COMPLEX *ptVectIn, COMPLEX *ptVectOut, FFT_RADIX_4_CTRL *ptCtrl);
extern void FixedFftRadix_4_2 (FixC *ptVectIn, FixC *ptVectOut,FFT_RADIX_4_CTRL *ptCtrl);


#ifndef SYSTEMC
#ifdef  __cplusplus
}
#endif	  
#endif

#endif    /* #ifndef FFT_RADIX_4_2_H */

/*****************************************************************************
  intra_pred.c  -- File performing the intra prediction 

  Original Author: 
  Martin Fiedler, CHEMNITZ UNIVERSITY OF TECHNOLOGY, 2004-06-01	

  Thales Authors:
  Florian Broekaert, THALES COMMUNICATIONS - AAL, 2006
  Fabien Colas-Bigey, THALES COMMUNICATIONS - AAL, 2008

  THE FUNCTION VERT_RIGHT_PRED HAS BEEN MODIFIED TAKING INTO ACCOUNT
  INFORMATION FROM THE JM IMPLEMENTATION. THE FUNCTIONS DIAG AND OTHERS
  MIGHT HAVE TO BE MODIFIED ALSO. THIS HAS TO BE TESTED WITH OTHER STREAMS

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
#include "intra_pred.h"

//////////////////////////////////////////////////// INTRA_4X4 PREDICTION /////
/****************************************************************************
  Variables and structures
****************************************************************************/
#define p(x,y)  L_pixel(f,bx+(x),by+(y))
#define left(y) ref[3-(y)]
#define top(x)  ref[5+(x)]

/****************************************************************************
  Static functions
*****************************************************************************/
static inline void Intra_4x4_Vertical(frame * f, int32_t *ref, int32_t bx, int32_t by)
{
  int32_t x, y;
  for (y = 0; y < 4; ++y)
    for (x = 0; x < 4; ++x)
      p(x, y) = top(x);
}

static inline void Intra_4x4_Horizontal(frame * f, int32_t *ref, int32_t bx,
					int32_t by)
{
  int32_t x, y;
  for (y = 0; y < 4; ++y)
    for (x = 0; x < 4; ++x)
      p(x, y) = left(y);
}

static inline void Intra_4x4_DC(frame * f, mode_pred_info * mpi, int32_t x,
				int32_t y)
{
  int32_t i, sum = 0, count = 0;
  /*
  if (mpi->mpi_firstMb_in_slice == DEBUG_MPI){
    printf("INtraDC\n");
    printf("%d -- %d for x = %d and y = %d\n",ModePredInfo_Intra4x4PredMode(mpi, (x >> 2) - 1, y >> 2),
	   ModePredInfo_Intra4x4PredMode(mpi, x >> 2, (y >> 2) - 1),x,y);
  }
  */
  
  /* Left available */
  if ( x>0 && (x!=mpi->x_first || y>=mpi->y_first+16)
       && ModePredInfo_Intra4x4PredMode(mpi, (x >> 2) - 1, y >> 2) >= 0)
    for (i = 0; i < 4; ++i, ++count)
      sum += L_pixel(f, x - 1, y + i);
  /* Up available */
  if ( ( (x<mpi->x_first && y>=(mpi->y_first+16+4))
	 || (x>=mpi->x_first && y>=(mpi->y_first+4)) )
       && ModePredInfo_Intra4x4PredMode(mpi, x >> 2, (y >> 2) - 1) >= 0)
    for (i = 0; i < 4; ++i, ++count)
      sum += L_pixel(f, x + i, y - 1);
  /*
  // Old code : original code
  if (x > 0
      && ModePredInfo_Intra4x4PredMode(mpi, (x >> 2) - 1, y >> 2) >= 0)
    for (i = 0; i < 4; ++i, ++count)
      sum += L_pixel(f, x - 1, y + i);
  if (y > 0 
      && ModePredInfo_Intra4x4PredMode(mpi, x >> 2, (y >> 2) - 1) >= 0)
    for (i = 0; i < 4; ++i, ++count)
      sum += L_pixel(f, x + i, y - 1);
  */
  if (count == 8)
    sum = (sum + 4) >> 3;
  else if (count == 4)
    sum = (sum + 2) >> 2;
  else
    sum = 128;
  sum = Clip(sum);
  for (i = 0; i < 4; ++i)
    memset(&L_pixel(f, x, y + i), sum, 4);
  /*
  if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
    printf("sum = %d\n",sum);
  */
}

static inline void Intra_4x4_Diagonal_Down_Left(frame * f, int32_t *ref,
						int32_t bx, int32_t by)
{
  int32_t x, y, i;
  for (y = 0; y < 4; ++y)
    for (x = 0; x < 4; ++x) {
      if ((x & y) == 3)
	i = top(6) + 3 * top(7) + 2;
      else
	i = top(x + y) + 2 * top(x + y + 1) + top(x + y + 2) + 2;
      p(x, y) = i >> 2;
    }
}

static inline void Intra_4x4_Diagonal_Down_Right(frame * f, int32_t *ref,
						 int32_t bx, int32_t by)
{
  int32_t x, y, i;
  for (y = 0; y < 4; ++y)
    for (x = 0; x < 4; ++x) {
      if (x > y)
	i = top(x - y - 2) + 2 * top(x - y - 1) + top(x - y) + 2;
      else if (x < y)
	i = left(y - x - 2) + 2 * left(y - x - 1) + left(y - x) +
	  2;
      else
	i = top(0) + 2 * top(-1) + left(0) + 2;
      p(x, y) = i >> 2;
    }
}

static inline void Intra_4x4_Vertical_Right(frame * f, int32_t *ref, int32_t bx,
					    int32_t by)
{
  p(0,0) = 
    p(1,2) = (left(-1) + top(0) + 1) / 2;
  p(1,0) = 
    p(2,2) = (top(0) + top(1) + 1) / 2;
  p(2,0) = 
    p(3,2) = (top(1) + top(2) + 1) / 2;
  p(3,0) = (top(2) + top(3) + 1) / 2;
  p(0,1) = 
    p(1,3) = (left(0) + 2*left(-1) + top(0) + 2) / 4;
  p(1,1) = 
    p(2,3) = (left(-1) + 2*top(0) + top(1) + 2) / 4;
  p(2,1) = 
    p(3,3) = (top(0) + 2*top(1) + top(2) + 2) / 4;
  p(3,1) = (top(1) + 2*top(2) + top(3) + 2) / 4;
  p(0,2) = (left(-1) + 2*left(0) + left(1) + 2) / 4;
  p(0,3) = (left(0) + 2*left(1) + left(2) + 2) / 4;
}

/*
static inline void Intra_4x4_Vertical_Right(frame * f, int32_t *ref, int32_t bx,
					    int32_t by)
{
  int32_t x, y, i, zVR;
  for (y = 0; y < 4; ++y)
    for (x = 0; x < 4; ++x) {
      zVR = 2 * x - y;
      if (zVR < -1)
	i = left(y - 1) + 2 * left(y - 2) + left(y - 3) + 2;
      else if (zVR < 0)
	i = left(0) + 2 * left(-1) + top(-1) + 2;
      else if (zVR & 1)
	i = top(x - (y >> 1) - 2) + 2 * top(x - (y >> 1) - 1) +
	  top(x - (y >> 1)) + 2;
      else
	i = 2 * top(x - (y >> 1) - 1) + 2 * top(x - (y >> 1)) + 2;
      p(x, y) = i >> 2;
    }
}
*/

static inline void Intra_4x4_Horizontal_Down(frame * f, int32_t *ref, int32_t bx,
					     int32_t by)
{
  int32_t x, y, i, zHD;
  for (y = 0; y < 4; ++y)
    for (x = 0; x < 4; ++x) {
      zHD = 2 * y - x;
      if (zHD < -1)
	i = top(x - 1) + 2 * top(x - 2) + top(x - 3) + 2;
      else if (zHD < 0)
	i = left(0) + 2 * left(-1) + top(0) + 2;
      else if (zHD & 1)
	i = left(y - (x >> 1) - 2) + 2 * left(y - (x >> 1) - 1) +
	  left(y - (x >> 1)) + 2;
      else
	i = 2 * left(y - (x >> 1) - 1) + 2 * left(y - (x >> 1)) +
	  2;
      p(x, y) = i >> 2;
    }
}


static inline void Intra_4x4_Vertical_Left(frame * f, int32_t *ref, int32_t bx,
					   int32_t by)
{
  p(0,0) = (top(0) + top(1) + 1) / 2;
  p(1,0) =
    p(0,2) = (top(1) + top(2) + 1) / 2;
  p(2,0) =
    p(1,2) = (top(2) + top(3) +1) / 2;
  p(3,0) =
    p(2,2) = (top(3) + top(4) + 1) / 2;
  p(3,2) = (top(4) + top(5) + 1) / 2;
  p(0,1) = (top(0) + 2*top(1) + top(2) + 2) / 4;
  p(1,1) =
    p(0,3) = (top(1) + 2*top(2) + top(3) + 2) / 4;
  p(2,1) = 
    p(1,3) = (top(2) + 2*top(3) + top(4) + 2) / 4;
  p(3,1) =
    p(2,3) = (top(3) + 2*top(4) + top(5) + 2) / 4;
  p(3,3) = (top(4) + 2*top(5) + top(6) + 2) / 4;
}

/*
static inline void Intra_4x4_Vertical_Left(frame * f, int32_t *ref, int32_t bx,
					   int32_t by)
{
  int32_t x, y, i;
  for (y = 0; y < 4; ++y)
    for (x = 0; x < 4; ++x) {
      if (y & 1)
	i = top(x + (y >> 1)) + 2 * top(x + (y >> 1) + 1) + top(x +
								(y
								 >>
								 1)
								+
								2)
	  + 2;
      else
	i = 2 * top(x + (y >> 1)) + 2 * top(x + (y >> 1) + 1) + 2;
      p(x, y) = i >> 2;
    }
}
*/

static inline void Intra_4x4_Horizontal_Up(frame * f, int32_t *ref, int32_t bx,
					   int32_t by)
{
  int32_t x, y, i, zHU;
  for (y = 0; y < 4; ++y)
    for (x = 0; x < 4; ++x) {
      zHU = x + 2 * y;
      if (zHU > 5)
	i = 4 * left(3);
      else if (zHU == 5)
	i = left(2) + 3 * left(3) + 2;
      else if (zHU & 1)
	i = left(y + (x >> 1)) + 2 * left(y + (x >> 1) + 1) +
	  left(y + (x >> 1) + 2) + 2;
      else
	i = 2 * left(y + (x >> 1)) + 2 * left(y + (x >> 1) + 1) +
	  2;
      p(x, y) = i >> 2;
    }
}

////////////////////////////////

void Intra_4x4_Dispatch(frame * f, mode_pred_info * mpi, int32_t x, int32_t y,
			int32_t luma4x4BlkIdx)
{
  int32_t ref[13];
  int32_t mode = ModePredInfo_Intra4x4PredMode(mpi, x >> 2, y >> 2);
  
  if (mode != 2) {
    int32_t i;
    if (x > 0) {
      for (i = 0; i < 4; ++i)
	left(i) = L_pixel(f, x - 1, y + i);
      if (y > 0)
	left(-1) = L_pixel(f, x - 1, y - 1);
    }
    if (y > 0) {
      for (i = 0; i < 4; ++i)
	top(i) = L_pixel(f, x + i, y - 1);
      if ((luma4x4BlkIdx & 3) == 3 || luma4x4BlkIdx == 13 ||
	  (luma4x4BlkIdx == 5 && x >= f->Lwidth - 4))
	for (i = 4; i < 8; ++i)
	  top(i) = top(3);
      else
	for (i = 4; i < 8; ++i)
	  top(i) = L_pixel(f, x + i, y - 1);
    }
  }
  /*
  if (mpi->mpi_firstMb_in_slice == DEBUG_MPI){
    printf("mode = %d\n",mode);
    if (mode != 2){
      int32_t i;
      if (x > 0) {
	for (i = 0; i < 4; ++i)
	  printf("%d, ",L_pixel(f, x - 1, y + i));
	if (y > 0)
	  printf("%d, ",L_pixel(f, x - 1, y - 1));
      }
      if (y > 0) {
	for (i = 0; i < 4; ++i)
	  printf("%d, ",L_pixel(f, x + i, y - 1));
	if ((luma4x4BlkIdx & 3) == 3 || luma4x4BlkIdx == 13 ||
	    (luma4x4BlkIdx == 5 && x >= f->Lwidth - 4))
	  {}
	else
	  for (i = 4; i < 8; ++i)
	    printf("%d, ",L_pixel(f, x + i, y - 1));
      }
      printf("\n");
    }
  }
  */

  switch (mode) {
  case 0:
    Intra_4x4_Vertical(f, ref, x, y);
    break;
  case 1:
    Intra_4x4_Horizontal(f, &ref[0], x, y);
    break;
  case 2:
    Intra_4x4_DC(f, mpi, x, y);
    break;
  case 3:
    Intra_4x4_Diagonal_Down_Left(f, &ref[0], x, y);
    break;
  case 4:
    Intra_4x4_Diagonal_Down_Right(f, &ref[0], x, y);
    break;
  case 5:
    Intra_4x4_Vertical_Right(f, &ref[0], x, y);
    break;
  case 6:
    Intra_4x4_Horizontal_Down(f, &ref[0], x, y);
    break;
  case 7:
    Intra_4x4_Vertical_Left(f, &ref[0], x, y);
    break;
  case 8:
    Intra_4x4_Horizontal_Up(f, &ref[0], x, y);
    break;
  default:
    printf("unsupported Intra4x4PredMode %d at %d,%d!\n", mode, x, y);
  }
}

#undef p
#undef top
#undef left


////////////////////////////////////////////////// INTRA_16X16 PREDICTION /////
/****************************************************************************
  Variables section
****************************************************************************/
#define p(x,y)  L_pixel(f,bx+(x),by+(y))


/****************************************************************************
  Static functions
****************************************************************************/

static inline void Intra_16x16_Vertical(frame * f, int32_t bx, int32_t by)
{
  int32_t x, y;
  for (y = 0; y < 16; ++y)
    for (x = 0; x < 16; ++x)
      p(x, y) = p(x, -1);
}

static inline void Intra_16x16_Horizontal(frame * f, int32_t bx, int32_t by)
{
  int32_t x, y;
  for (y = 0; y < 16; ++y)
    for (x = 0; x < 16; ++x)
      p(x, y) = p(-1, y);
}

static inline void Intra_16x16_DC(frame * f, mode_pred_info * mpi, int32_t bx,
				  int32_t by, int32_t constrained_intra_pred)
{
  int32_t i, sum = 0, count = 0;
  i = get_mb_mode(mpi, (bx >> 4) - 1, by >> 4);
  if (!((i == NA) || (IsInter(i) && constrained_intra_pred)))
    for (i = 0; i < 16; ++i, ++count)
      sum += p(-1, i);
  i = get_mb_mode(mpi, bx >> 4, (by >> 4) - 1);
  if (!((i == NA) || (IsInter(i) && constrained_intra_pred)))
    for (i = 0; i < 16; ++i, ++count)
      sum += p(i, -1);
  if (count == 32)
    sum = (sum + 16) >> 5;
  else if (count == 16)
    sum = (sum + 8) >> 4;
  else
    sum = 128;
  sum = Clip(sum);
  for (i = 0; i < 16; ++i)
    memset(&p(0, i), sum, 16);
}

static inline void Intra_16x16_Plane(frame * f, int32_t bx, int32_t by)
{
  int32_t a, b, c, H, V, x, y;
  for (x = 0, H = 0; x < 8; ++x)
    H += (x + 1) * (p(8 + x, -1) - p(6 - x, -1));
  for (y = 0, V = 0; y < 8; ++y)
    V += (y + 1) * (p(-1, 8 + y) - p(-1, 6 - y));
  a = 16 * (p(-1, 15) + p(15, -1));
  b = (5 * H + 32) >> 6;
  c = (5 * V + 32) >> 6;
  for (y = 0; y < 16; ++y)
    for (x = 0; x < 16; ++x)
      p(x, y) = Clip((a + b * (x - 7) + c * (y - 7) + 16) >> 5);
}

void Intra_16x16_Dispatch(frame * f, mode_pred_info * mpi, int32_t mode, int32_t x,
			  int32_t y, int32_t constrained_intra_pred)
{
  /* if (mpi->mpi_firstMb_in_slice == DEBUG_MPI){ */
/*     printf("Intra16x16 mode = %d\n",mode); */
/*   } */

  switch (mode) {
  case 0:
    Intra_16x16_Vertical(f, x, y);
    break;
  case 1:
    Intra_16x16_Horizontal(f, x, y);
    break;
  case 2:
    Intra_16x16_DC(f, mpi, x, y, constrained_intra_pred);
    break;
  case 3:
    Intra_16x16_Plane(f, x, y);
    break;
  default:
    printf("unsupported Intra16x16PredMode %d at %d,%d!\n", mode, x,
	   y);
  }
}

#undef p

///////////////////////////////////////////////// INTRA_CHROMA PREDICTION /////
/****************************************************************************
  Variables section
****************************************************************************/
#define r(x,y)  Cr_pixel(f,bx+(x),by+(y))
#define b(x,y)  Cb_pixel(f,bx+(x),by+(y))

#define ICDCsumL(chan,offs) chan(-1,offs)+chan(-1,offs+1)+chan(-1,offs+2)+chan(-1,offs+3)
#define ICDCsumT(chan,offs) chan(offs,-1)+chan(offs+1,-1)+chan(offs+2,-1)+chan(offs+3,-1)
#define ICDCfill(offx,offy,valr,valb) \
  do { \
    int32_t sy,vr=valr,vb=valb; \
    for(sy=0; sy<4; ++sy) { \
      memset(&r(offx,offy+sy),vr,4); \
      memset(&b(offx,offy+sy),vb,4); \
    } \
  } while(0)


/****************************************************************************
  Static functions
****************************************************************************/
static inline void Intra_Chroma_DC(frame * f, mode_pred_info * mpi, int32_t bx,
				   int32_t by, int32_t constrained_intra_pred)
{
  int32_t i;
  int32_t left = 1, top = 1;
  int32_t l0r = 512, l0b = 512, l4r = 512, l4b = 512;
  int32_t t0r = 512, t0b = 512, t4r = 512, t4b = 512;

  i = get_mb_mode(mpi, (bx >> 3) - 1, by >> 3);
  if ((i == NA) || (IsInter(i) && constrained_intra_pred))
    left = 0;
  if (left) {
    l0r = ICDCsumL(r, 0);
    l0b = ICDCsumL(b, 0);
    l4r = ICDCsumL(r, 4);
    l4b = ICDCsumL(b, 4);
  }

  i = get_mb_mode(mpi, bx >> 3, (by >> 3) - 1);
  if ((i == NA) || (IsInter(i) && constrained_intra_pred))
    top = 0;
  if (top) {
    t0r = ICDCsumT(r, 0);
    t0b = ICDCsumT(b, 0);
    t4r = ICDCsumT(r, 4);
    t4b = ICDCsumT(b, 4);
  }

  if (top) {
    if (left)
      ICDCfill(0, 0, (l0r + t0r + 4) >> 3, (l0b + t0b + 4) >> 3);
    else
      ICDCfill(0, 0, (t0r + 2) >> 2, (t0b + 2) >> 2);
  } else {
    if (left)
      ICDCfill(0, 0, (l0r + 2) >> 2, (l0b + 2) >> 2);
    else
      ICDCfill(0, 0, 128, 128);
  }

  if (top)
    ICDCfill(4, 0, (t4r + 2) >> 2, (t4b + 2) >> 2);
  else if (left)
    ICDCfill(4, 0, (l0r + 2) >> 2, (l0b + 2) >> 2);
  else
    ICDCfill(4, 0, 128, 128);

  if (left)
    ICDCfill(0, 4, (l4r + 2) >> 2, (l4b + 2) >> 2);
  else if (top)
    ICDCfill(0, 4, (t0r + 2) >> 2, (t0b + 2) >> 2);
  else
    ICDCfill(0, 4, 128, 128);

  if (top) {
    if (left)
      ICDCfill(4, 4, (l4r + t4r + 4) >> 3, (l4b + t4b + 4) >> 3);
    else
      ICDCfill(4, 4, (t4r + 2) >> 2, (t4b + 2) >> 2);
  } else {
    if (left)
      ICDCfill(4, 4, (l4r + 2) >> 2, (l4b + 2) >> 2);
    else
      ICDCfill(4, 4, 128, 128);
  }
}


/****************************************************************************
  Non static functions
****************************************************************************/
void Intra_Chroma_Horizontal(frame * f, int32_t bx, int32_t by)
{
  int32_t x, y;
  for (y = 0; y < 8; ++y)
    for (x = 0; x < 8; ++x) {
      r(x, y) = r(-1, y);
      b(x, y) = b(-1, y);
    }
}

void Intra_Chroma_Vertical(frame * f, int32_t bx, int32_t by)
{
  int32_t x, y;
  for (y = 0; y < 8; ++y)
    for (x = 0; x < 8; ++x) {
      r(x, y) = r(x, -1);
      b(x, y) = b(x, -1);
    }
}

void Intra_Chroma_Plane(frame * f, int32_t bx, int32_t by)
{
  int32_t A, B, C, H, V, x, y;
  // Intra_Chroma_Plane prediction for Cr channel
  for (x = 0, H = 0; x < 4; ++x)
    H += (x + 1) * (r(4 + x, -1) - r(2 - x, -1));
  for (y = 0, V = 0; y < 4; ++y)
    V += (y + 1) * (r(-1, 4 + y) - r(-1, 2 - y));
  A = 16 * (r(-1, 7) + r(7, -1));
  B = (17 * H + 16) >> 5;
  C = (17 * V + 16) >> 5;
  for (y = 0; y < 8; ++y)
    for (x = 0; x < 8; ++x)
      r(x, y) = Clip((A + B * (x - 3) + C * (y - 3) + 16) >> 5);
  // Intra_Chroma_Plane prediction for Cr channel
  for (x = 0, H = 0; x < 4; ++x)
    H += (x + 1) * (b(4 + x, -1) - b(2 - x, -1));
  for (y = 0, V = 0; y < 4; ++y)
    V += (y + 1) * (b(-1, 4 + y) - b(-1, 2 - y));
  A = 16 * (b(-1, 7) + b(7, -1));
  B = (17 * H + 16) >> 5;
  C = (17 * V + 16) >> 5;
  for (y = 0; y < 8; ++y)
    for (x = 0; x < 8; ++x)
      b(x, y) = Clip((A + B * (x - 3) + C * (y - 3) + 16) >> 5);
}

void Intra_Chroma_Dispatch(frame * f, mode_pred_info * mpi, int32_t mode,
			   int32_t x, int32_t y, int32_t constrained_intra_pred)
{
  /* if (mpi->mpi_firstMb_in_slice == DEBUG_MPI) */
/*     printf("Cr = %d\n",mode); */

  switch (mode) {
  case 0:
    Intra_Chroma_DC(f, mpi, x, y, constrained_intra_pred);
    break;
  case 1:
    Intra_Chroma_Horizontal(f, x, y);
    break;
  case 2:
    Intra_Chroma_Vertical(f, x, y);
    break;
  case 3:
    Intra_Chroma_Plane(f, x, y);
    break;
  default:
    printf("unsupported IntraChromaPredMode %d at %d,%d!\n", mode,
	   x << 1, y << 1);
  }
}

#undef r
#undef b

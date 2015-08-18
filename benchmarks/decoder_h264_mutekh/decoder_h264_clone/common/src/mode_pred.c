/*****************************************************************************
  mode_pred.c -- File giving functions and data structures for mode_prediction
                 of mbs
  
  Original Author: 
  Martin Fiedler, CHEMNITZ UNIVERSITY OF TECHNOLOGY, 2004-06-01	
  
  Thales Authors:
  Florian Broekaert, THALES COMMUNICATIONS - AAL, 2006
  Fabien Colas-Bigey, THALES COMMUNICATIONS - AAL, 2008

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
#include "mode_pred.h"


/****************************************************************************
  Non static functions
****************************************************************************/
/*Memory Allocation of the structure*/
mode_pred_info *alloc_mode_pred_info(int32_t img_width, int32_t img_height, int32_t slice_numbers,
				     int32_t slice_compteur)
{
  /* Mode prediction structure allocation */
  mode_pred_info *mpi = (mode_pred_info *)calloc(1, sizeof(mode_pred_info));
  
  /* Image information */
  mpi->img_MbWidth  = img_width  >> 4;
  mpi->img_MbHeight = img_height >> 4;
  mpi->img_MbSize   = mpi->img_MbWidth * mpi->img_MbHeight;

  /* Translating informations for one slice */
  int32_t width = img_width;
  mpi->slice_MbSize = mpi->img_MbSize / slice_numbers;
  
  /* Structures allocations */
  /* Consider luma 16x16 blocks for one slice */
  mpi->MbMode = (char *)malloc(mpi->slice_MbSize * sizeof(char));
  mpi->QPy    = (int32_t *)malloc(mpi->img_MbSize * sizeof(int32_t)); /* y*slice_numbers to get whole image and not only one slice */
  mpi->QPc    = (int32_t *)malloc(mpi->img_MbSize * sizeof(int32_t)); /* y*slice_numbers to get whole image and not only one slice */

  /* Consider chroma 4x4 blocks for one slice */
  mpi->TotalCoeffC[0] = (char *)malloc( (mpi->slice_MbSize * 4) * sizeof(unsigned char));
  mpi->TotalCoeffC[1] = (char *)malloc( (mpi->slice_MbSize * 4) * sizeof(unsigned char));
  
  /* Consider luma 4x4 blocks for one slice */
  mpi->TotalCoeffL      = (char *)malloc( (mpi->slice_MbSize * 16) * sizeof(unsigned char));
  mpi->Intra4x4PredMode = (char *)malloc( (mpi->slice_MbSize * 16) * sizeof(char));
  mpi->MVx = (int16_t *)malloc( (mpi->slice_MbSize * 16) * sizeof(int16_t));
  mpi->MVy = (int16_t *)malloc( (mpi->slice_MbSize * 16) * sizeof(int16_t));
  
  /* per-macroblock information     (16x16) */
  mpi->MbPitch = width >> 4;
  /* per-macroblock information     (4x4) */
  mpi->TbPitch = width >> 2;
  /* per-chroma block information    (8x8) */
  mpi->CbPitch = width >> 3;
  
  mpi->ID = slice_compteur;
    
  return mpi;
}

/*Set the fields of the structure*/
void clear_mode_pred_info(mode_pred_info * mpi, slice_header * sh)
{
  if (!mpi)
    return;

  if (mpi->MbMode)
    memset(mpi->MbMode, NA,
	   mpi->slice_MbSize * sizeof(char));
  if (mpi->TotalCoeffC[0])
    memset(mpi->TotalCoeffC[0], 0,
	   mpi->slice_MbSize * 4 * sizeof(unsigned char));
  if (mpi->TotalCoeffC[1])
    memset(mpi->TotalCoeffC[1], 0,
	   mpi->slice_MbSize * 4 * sizeof(unsigned char));
  if (mpi->TotalCoeffL)
    memset(mpi->TotalCoeffL, 0,
	   mpi->slice_MbSize * 16 * sizeof(unsigned char));
  if (mpi->Intra4x4PredMode)
    memset(mpi->Intra4x4PredMode, 0xFF,
	   mpi->slice_MbSize * 16 * sizeof(char));
  
  /* Motion vector */
  if (mpi->MVx)
    memset(mpi->MVx, MV_NA & 0xFF,
	   mpi->slice_MbSize * 16 * sizeof(int16_t));
  if (mpi->MVy)
    memset(mpi->MVy, MV_NA & 0xFF,
	   mpi->slice_MbSize * 16 * sizeof(int16_t));

  /* Deblocking filter */
  if(mpi->QPy)            
    memset(mpi->QPy,0xFF,
	   mpi->img_MbSize * sizeof(int32_t));
  if(mpi->QPc)              
    memset(mpi->QPc,0xFF,
	   mpi->img_MbSize * sizeof(int32_t));
  
  /* Multislice */
  
  /* First MB of the slice */
  mpi->mpi_firstMb_in_slice = sh->first_mb_in_slice;
  mpi->x_first = (mpi->mpi_firstMb_in_slice % mpi->MbPitch) << 4;
  mpi->y_first = (mpi->mpi_firstMb_in_slice / mpi->MbPitch) << 4;
}

/*Memory desallocation*/
void free_mode_pred_info(mode_pred_info * mpi)
{
  if (!mpi)
    return;
  if (mpi->MbMode)
    free(mpi->MbMode);
  if (mpi->TotalCoeffC[0])
    free(mpi->TotalCoeffC[0]);
  if (mpi->TotalCoeffC[1])
    free(mpi->TotalCoeffC[1]);
  if (mpi->TotalCoeffL)
    free(mpi->TotalCoeffL);
  if (mpi->Intra4x4PredMode)
    free(mpi->Intra4x4PredMode);
  if (mpi->MVx)
    free(mpi->MVx);
  if (mpi->MVy)
    free(mpi->MVy);
    
  /* Deblocking filter */
  if(mpi->QPy)             
    free(mpi->QPy);
  if(mpi->QPc)
    free(mpi->QPc);
    
  /* Free Semaphore of the threads */
  if(sem_destroy(&(mpi->sem_start_th))!=0)
    printf("Fatal error during destroying the sem \n");
  if(sem_destroy(&(mpi->sem_stop_th))!=0)
    printf("Fatal error during destroying the sem \n");

  free(mpi);
}

///// MbMode retrieval /////

int32_t get_mb_mode(mode_pred_info * mpi, int32_t mb_x, int32_t mb_y)
{
  /* Parameters expressed in 16x16 blocks */
  if (mb_x < (mpi->x_first >> 4)){
    if ( (mb_x < 0) || (mb_y < ((mpi->y_first >> 4) + 1)) )
      return -1;
  }
  else{
    if (mb_y < (mpi->y_first >> 4))
      return -1;
  }

  return ModePredInfo_MbMode(mpi, mb_x, mb_y);
}


///// nC / TotalCoeff stuff /////

static inline int32_t get_luma_nN(mode_pred_info * mpi, int32_t x, int32_t y)
{
  /*
  if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
    printf("get_luma_nN x = %d, y = %d\n",x,y);
  */
  /* Parameters expressed in pixels */
  if (x < mpi->x_first){
    if ( (x < 0) || (y < (mpi->y_first + 16)) )
      return -1;
  }
  else{
    if (y < mpi->y_first)
      return -1;
  }
  
  return ModePredInfo_TotalCoeffL(mpi, x >> 2, y >> 2);
}

static inline int32_t get_chroma_nN(mode_pred_info * mpi, int32_t x, int32_t y,
				int32_t iCbCr)
{
  /* Parameters expressed in pixels */
  if (x < mpi->x_first){
    if ( (x < 0) || (y < (mpi->y_first + 16)) )
      return -1;
  }
  else{
    if (y < mpi->y_first)
      return -1;
  }
  
  return ModePredInfo_TotalCoeffC(mpi, x >> 3, y >> 3, iCbCr);
}


int32_t get_luma_nC(mode_pred_info * mpi, int32_t x, int32_t y)
{
  /*
  if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
    printf("\nget_luma_nC x = %d, y = %d\n",x,y);
  */
  int32_t nA = get_luma_nN(mpi, x - 4, y);
  int32_t nB = get_luma_nN(mpi, x, y - 4);
  /*
  if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
    printf("nA = %d, nB = %d, x = %d, y = %d\n",
	   nA,nB,x,y);
  */
  if (nA < 0 && nB < 0)
    return 0;
  if (nA >= 0 && nB >= 0)
    return (nA + nB + 1) >> 1;
  if (nA >= 0)
    return nA;
  else
    return nB;
}

int32_t get_chroma_nC(mode_pred_info * mpi, int32_t x, int32_t y, int32_t iCbCr)
{
  int32_t nA = get_chroma_nN(mpi, x - 8, y, iCbCr);
  int32_t nB = get_chroma_nN(mpi, x, y - 8, iCbCr);
  
  /* if (mpi->mpi_firstMb_in_slice == DEBUG_MPI) */
/*     printf("nA = %d, nB = %d, x = %d, y = %d\n", */
/* 	   nA,nB,x,y); */
  
  if (nA < 0 && nB < 0)
    return 0;
  if (nA >= 0 && nB >= 0)
    return (nA + nB + 1) >> 1;
  if (nA >= 0)
    return nA;
  else
    return nB;
}


///// Intra_4x4 Prediction Mode Prediction /////
static inline int32_t get_Intra4x4PredModeN(mode_pred_info * mpi, int32_t x, int32_t y)
{
  /* Parameters expressed in pixels */
  int32_t i;
  if (x < mpi->x_first){
    if ( (x < 0) || (y < (mpi->y_first + 16)) )
      return -1;		/* force Intra_4x4_DC */
  }
  else{
    if (y < mpi->y_first)
      return -1;		/* force Intra_4x4_DC */
  }
  
  i = ModePredInfo_Intra4x4PredMode(mpi, x >> 2, y >> 2);
  return i;
}

int32_t get_predIntra4x4PredMode(mode_pred_info * mpi, int32_t x, int32_t y)
{
  /* Parameters expressed in pixels */
  int32_t A = get_Intra4x4PredModeN(mpi, x - 4, y);
  int32_t B = get_Intra4x4PredModeN(mpi, x, y - 4);
  //printf("A = %d, B = %d\n",A,B);
  int32_t mode = (A < B) ? A : B;
  if (mode < 0)
    mode = 2;
  return mode;
}


///// Motion Vector Prediction /////

static unsigned char is_out_slice(mode_pred_info *mpi, int32_t x, int32_t y)
{
  /* Parameters x and Y are expressed as 4x4 positions */
  int32_t mb_pos = (y >> 2) * mpi->MbPitch + (x >>2);
  if ((mb_pos < mpi->mpi_firstMb_in_slice) || (mb_pos >= (mpi->mpi_firstMb_in_slice + mpi->slice_MbSize)))
    return 1;
  else
    return 0;
}

static inline mv get_MV(mode_pred_info * mpi, int32_t x, int32_t y)
{
  /* Declarations and initialisations */
  mv res = { 0, 0, 0, 0 };
  x >>= 2;			/* to address 4x4 blocks */
  y >>= 2;			/* to address 4x4 blocks */

  if (x < 0 || x >= mpi->TbPitch || is_out_slice(mpi,x,y))
    {
      //printf("return res on %d %d %d\n",x,y);
      return res;
    }
  
  res.x = ModePredInfo_MVx(mpi, x, y);
  res.y = ModePredInfo_MVy(mpi, x, y);
  //if (res.x == MV_NA) {
  if (res.x == -32640) {
    res.x = 0;
    res.y = 0;
    if (IsIntra(ModePredInfo_MbMode(mpi, x >> 2, y >> 2)))
      res.available = 1;
  } else {
    res.available = 1;
    res.valid = 1;
  }
  return res;
}


mv PredictMV(mode_pred_info * mpi,
	     int32_t org_x, int32_t org_y, int32_t width, int32_t height)
{
  mv A, B, C, res = {0,0,0,0};
  // derive candidate MVs
  A = get_MV(mpi, org_x - 1, org_y);
  B = get_MV(mpi, org_x, org_y - 1);
  C = get_MV(mpi, org_x + width, org_y - 1);
  if (!C.available)
    C = get_MV(mpi, org_x - 1, org_y - 1);
  /*
    printf("PredictMV @ %d,%d + %dx%d:",org_x,org_y,width,height);if(A.valid)
    printf(" A=%d,%d",A.x,A.y);if(B.valid) printf(" B=%d,%d",B.x,B.y);if(C.valid)
    printf(" C/D=%d,%d",C.x,C.y);printf("\n");
  */
  // Directional segmentation prediction for 8x16 / 16x8 partitions
  if (width == 16 && height == 8) {
    if (org_y & 8) {
      if (A.valid)
	return A;
    } else {
      if (B.valid)
	return B;
    }
  }
  if (width == 8 && height == 16) {
    if (org_x & 8) {
      if (C.valid)
	return C;
    } else {
      if (A.valid)
	return A;
    }
  }
  // If one and only one of the candidate predictors is available and valid,
  // it is returned
  if (!B.valid && !C.valid)
    return A;
  if (!A.valid && B.valid && !C.valid)
    return B;
  if (!A.valid && !B.valid && C.valid)
    return C;
  // median prediction
  res.x = Median(A.x, B.x, C.x);
  res.y = Median(A.y, B.y, C.y);
  return res;
}

mv Predict_P_Skip_MV(mode_pred_info * mpi, int32_t org_x, int32_t org_y)
{
  /* Parameters expressed in pix */
  mv zero = { 0, 0, 0 };

  if (org_x < mpi->x_first){
    if ( (org_x <= 0) || (org_y <= (mpi->y_first) + 16) )
      return zero;
  }
  else{
    if (org_x == mpi->x_first || org_y <= mpi->y_first)
      return zero;
  }

  if (ModePredInfo_MVx(mpi, (org_x >> 2) - 1, org_y >> 2) == 0 &&
      ModePredInfo_MVy(mpi, (org_x >> 2) - 1, org_y >> 2) == 0)
    return zero;
  if (ModePredInfo_MVx(mpi, org_x >> 2, (org_y >> 2) - 1) == 0 &&
      ModePredInfo_MVy(mpi, org_x >> 2, (org_y >> 2) - 1) == 0)
    return zero;
  return PredictMV(mpi, org_x, org_y, 16, 16);
}

void FillMVs(mode_pred_info * mpi,
	     int32_t org_x, int32_t org_y, int32_t width, int32_t height, int16_t mvx, int16_t mvy)
{
  int32_t x, y;
  org_x >>= 2;
  org_y >>= 2;
  width >>= 2;
  height >>= 2;
  for (y = org_y + height - 1; y >= org_y; --y)
    for (x = org_x + width - 1; x >= org_x; --x) {
      set_ModePredInfo_MVx(mpi, x, y, mvx);
      set_ModePredInfo_MVy(mpi, x, y, mvy);
    }
}

void DeriveMVs(mode_pred_info * mpi,
	       int32_t org_x, int32_t org_y,
	       int32_t width, int32_t height, int16_t mvdx, int16_t mvdy)
{
  mv v = PredictMV(mpi, org_x, org_y, width, height);
  //printf("MV @ %d,%d + %dx%d: pred=%d,%d diff=%d,%d\n",org_x,org_y,width,height,v.x,v.y,mvdx,mvdy);
  FillMVs(mpi, org_x, org_y, width, height, v.x + mvdx, v.y + mvdy);
}

void Derive_P_Skip_MVs(mode_pred_info * mpi, int32_t org_x, int32_t org_y)
{
  mv v = Predict_P_Skip_MV(mpi, org_x, org_y);
  //printf("P_Skip MV @ %d,%d: mv=%d,%d\n",org_x,org_y,v.x,v.y);
  FillMVs(mpi, org_x, org_y, 16, 16, v.x, v.y);
}



char ModePredInfo_MbMode(mode_pred_info * mpi, int32_t x, int32_t y)
{
  /* Parameters expressed in 16x16 blocks */
  int32_t mb_pos = y * mpi->MbPitch + x;
  int32_t mb_off = mb_pos - mpi->mpi_firstMb_in_slice;
  return mpi->MbMode[mb_off];
}

void set_ModePredInfo_MbMode(mode_pred_info * mpi, int32_t x, int32_t y, char val)
{
  /* Parameters expressed in 16x16 blocks */
  int32_t mb_pos = y * mpi->MbPitch + x;
  int32_t mb_off = mb_pos - mpi->mpi_firstMb_in_slice;
  mpi->MbMode[mb_off] = val;
}

char ModePredInfo_TotalCoeffC(mode_pred_info *mpi, int32_t x, int32_t y, int32_t iCbCr)
{
  /* Parameters expressed in 8x8 blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>1) * mpi->MbPitch + (x>>1);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 8x8 offset */
  int32_t mb_off_8x8 = (y%2)*2 + (x%2);
  int32_t pos = mb_off_16x16 * 4 + mb_off_8x8;

  return mpi->TotalCoeffC[iCbCr][pos];
}

void set_ModePredInfo_TotalCoeffC(mode_pred_info *mpi, int32_t x, int32_t y, int32_t iCbCr, char val)
{
  /* Parameters expressed in 8x8 blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>1) * mpi->MbPitch + (x>>1);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 8x8 offset */
  int32_t mb_off_8x8 = (y%2)*2 + (x%2);
  int32_t pos = mb_off_16x16 * 4 + mb_off_8x8;

  mpi->TotalCoeffC[iCbCr][pos] = val;
}

char ModePredInfo_TotalCoeffL(mode_pred_info *mpi, int32_t x, int32_t y)
{
  /* Parameters expressed in 4x4 luma blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>2) * mpi->MbPitch + (x>>2);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 4x4 offset */
  int32_t mb_off_4x4 = (y%4)*4 + (x%4);
  int32_t pos = mb_off_16x16 * 16 + mb_off_4x4;

  return mpi->TotalCoeffL[pos];
}

void set_ModePredInfo_TotalCoeffL(mode_pred_info *mpi, int32_t x, int32_t y, char val)
{
  /* Parameters expressed in 4x4 luma blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>2) * mpi->MbPitch + (x>>2);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 4x4 offset */
  int32_t mb_off_4x4 = (y%4)*4 + (x%4);
  int32_t pos = mb_off_16x16 * 16 + mb_off_4x4;

  mpi->TotalCoeffL[pos] = val;
  /*
  printf("Print Mode pred L, x = %d, y = %d, off = %d, val = %d\n",
	 x,y,mb_off,val);
  */
}

char ModePredInfo_Intra4x4PredMode(mode_pred_info *mpi, int32_t x, int32_t y)
{
  /* Parameters expressed in 4x4 luma blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>2) * mpi->MbPitch + (x>>2);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 4x4 offset */
  int32_t mb_off_4x4 = (y%4)*4 + (x%4);
  int32_t pos = mb_off_16x16 * 16 + mb_off_4x4;

  return mpi->Intra4x4PredMode[pos];
}

void set_ModePredInfo_Intra4x4PredMode(mode_pred_info *mpi, int32_t x, int32_t y, char val)
{
  /* Parameters expressed in 4x4 luma blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>2) * mpi->MbPitch + (x>>2);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 4x4 offset */
  int32_t mb_off_4x4 = (y%4)*4 + (x%4);
  int32_t pos = mb_off_16x16 * 16 + mb_off_4x4;

  mpi->Intra4x4PredMode[pos] = val;
}

int16_t ModePredInfo_MVx(mode_pred_info *mpi, int32_t x, int32_t y)
{
  /* Parameters expressed in 4x4 luma blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>2) * mpi->MbPitch + (x>>2);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 4x4 offset */
  int32_t mb_off_4x4 = (y%4)*4 + (x%4);
  int32_t pos = mb_off_16x16 * 16 + mb_off_4x4;

  return mpi->MVx[pos];
}

void set_ModePredInfo_MVx(mode_pred_info *mpi, int32_t x, int32_t y, int16_t val)
{
  /* Parameters expressed in 4x4 luma blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>2) * mpi->MbPitch + (x>>2);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 4x4 offset */
  int32_t mb_off_4x4 = (y%4)*4 + (x%4);
  int32_t pos = mb_off_16x16 * 16 + mb_off_4x4;

  mpi->MVx[pos] = val;
}

int16_t ModePredInfo_MVy(mode_pred_info *mpi, int32_t x, int32_t y)
{
  /* Parameters expressed in 4x4 luma blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>2) * mpi->MbPitch + (x>>2);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 4x4 offset */
  int32_t mb_off_4x4 = (y%4)*4 + (x%4);
  int32_t pos = mb_off_16x16 * 16 + mb_off_4x4;
  
  return mpi->MVy[pos];
}

void set_ModePredInfo_MVy(mode_pred_info *mpi, int32_t x, int32_t y, int16_t val)
{
  /* Parameters expressed in 4x4 luma blocks */
  /* 16x16 offset */
  int32_t mb_pos_16x16 = (y>>2) * mpi->MbPitch + (x>>2);
  int32_t mb_off_16x16 = mb_pos_16x16 - mpi->mpi_firstMb_in_slice;
  /* 4x4 offset */
  int32_t mb_off_4x4 = (y%4)*4 + (x%4);
  int32_t pos = mb_off_16x16 * 16 + mb_off_4x4;
  
  mpi->MVy[pos] = val;
}

int32_t ModePredInfo_QPy(mode_pred_info *mpi, int32_t x, int32_t y)
{
  /* Parameters expressed in 16x16 blocks */
  int32_t mb_pos = y * mpi->MbPitch + x;
  int32_t mb_off = mb_pos - mpi->mpi_firstMb_in_slice;
  return mpi->QPy[mb_off];
}

void set_ModePredInfo_QPy(mode_pred_info *mpi, int32_t x, int32_t y, int32_t val)
{
  /* Parameters expressed in 16x16 blocks */
  int32_t mb_pos = y * mpi->MbPitch + x;
  int32_t mb_off = mb_pos - mpi->mpi_firstMb_in_slice;
  if (mb_off > mpi->img_MbSize)
    printf("error\n");
  mpi->QPy[mb_off] = val;
}

int32_t ModePredInfo_QPc(mode_pred_info *mpi, int32_t x, int32_t y)
{
  /* Parameters expressed in 16x16 blocks */
  int32_t mb_pos = y * mpi->MbPitch + x;
  int32_t mb_off = mb_pos - mpi->mpi_firstMb_in_slice;
  return mpi->QPc[mb_off];
}

void set_ModePredInfo_QPc(mode_pred_info *mpi, int32_t x, int32_t y, int32_t val)
{
  /* Parameters expressed in 16x16 blocks */
  int32_t mb_pos = y * mpi->MbPitch + x;
  int32_t mb_off = mb_pos - mpi->mpi_firstMb_in_slice;
  if (mb_off > mpi->img_MbSize)
    printf("error\n");
  mpi->QPc[mb_off] = val;
}

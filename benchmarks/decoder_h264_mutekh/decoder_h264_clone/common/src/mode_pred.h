/*****************************************************************************
  mode_pred.h -- File giving functions and data structures for mode_prediction
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

#ifndef __MODE_PRED_H__
#define __MODE_PRED_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "common.h"
#include "mbmodes.h"
#include "mode_pred.h"
#include "slicehdr.h"

#include <semaphore.h>


/****************************************************************************
  Variables and structures
****************************************************************************/
#define MV_NA 0x80808080

typedef struct _mv 
{
  int16_t x,y;
  /*   int32_t available;  /\* i.e. inside the image *\/ */
  /*   int32_t valid;      /\*  i.e. usable for prediction *\/ */
  char available;  /* i.e. inside the image */
  char valid; 
} mv;

typedef struct _mode_pred_info 
{
  /* Image information */
  int32_t img_MbWidth, img_MbHeight, img_MbSize;

  /* Slice information */
  int32_t slice_MbSize;

  /* per-macroblock information     (16x16) */
  char MbWidth, MbHeight, MbPitch;
  char *MbMode;

  /* per-chroma block information    (8x8) */
  char CbWidth, CbHeight, CbPitch;
  char *TotalCoeffC[2];

  /* per-transform block information (4x4) */
  char TbWidth, TbHeight, TbPitch;
  char TbHeight_slice;
  char *TotalCoeffL;
  char *Intra4x4PredMode;
  int16_t *MVx,*MVy;

  /* Deblocking Filter */
  int32_t *QPy, *QPc;

  /* Information for multislice control management and also for threads */
  int32_t ID; /* Number of the thread that will have this mpi structure */
  int16_t mpi_firstMb_in_slice; /* Number of the first MB in the slice affected to this mpi and to this thread */ 
  int32_t x_first, y_first;

  /* semaphores required by thread synchronisation */
  sem_t sem_start_th;		/* evaluated before starting thread */
  sem_t sem_stop_th;		/* indicates to main the thread has finished */
} mode_pred_info;

/* Prediction of Motion Vectors */
#define Max(a,b) ((a)>(b)?(a):(b))
#define Min(a,b) ((a)<(b)?(a):(b))
#define Median(a,b,c) Max(Min(a,b),Min(c,Max(a,b)))


/****************************************************************************
  Non static functions
****************************************************************************/
char ModePredInfo_MbMode(mode_pred_info * mpi, int32_t x, int32_t y);
void set_ModePredInfo_MbMode(mode_pred_info * mpi, int32_t x, int32_t y, char val);

char ModePredInfo_TotalCoeffC(mode_pred_info *mpi, int32_t x, int32_t y, int32_t iCbCr);
void set_ModePredInfo_TotalCoeffC(mode_pred_info *mpi, int32_t x, int32_t y, int32_t iCbCr, char val);

char ModePredInfo_TotalCoeffL(mode_pred_info *mpi, int32_t x, int32_t y);
void set_ModePredInfo_TotalCoeffL(mode_pred_info *mpi, int32_t x, int32_t y, char val);

char ModePredInfo_Intra4x4PredMode(mode_pred_info *mpi, int32_t x, int32_t y);
void set_ModePredInfo_Intra4x4PredMode(mode_pred_info *mpi, int32_t x, int32_t y, char val);

int16_t ModePredInfo_MVx(mode_pred_info *mpi, int32_t x, int32_t y);
void set_ModePredInfo_MVx(mode_pred_info *mpi, int32_t x, int32_t y, int16_t val);
int16_t ModePredInfo_MVy(mode_pred_info *mpi, int32_t x, int32_t y);
void set_ModePredInfo_MVy(mode_pred_info *mpi, int32_t x, int32_t y, int16_t val);

int32_t ModePredInfo_QPy(mode_pred_info *mpi, int32_t x, int32_t y);
void set_ModePredInfo_QPy(mode_pred_info *mpi, int32_t x, int32_t y, int32_t val);
int32_t ModePredInfo_QPc(mode_pred_info *mpi, int32_t x, int32_t y);
void set_ModePredInfo_QPc(mode_pred_info *mpi, int32_t x, int32_t y, int32_t val);


/*
#define ModePredInfo_MbMode(mpi,x,y) (mpi->MbMode[(((y)%mpi_mod_MbMode)*mpi->MbPitch+(x))])
#define ModePredInfo_TotalCoeffC(mpi,x,y,iCbCr) (mpi->TotalCoeffC[iCbCr][(((y)%mpi_mod_C)*mpi->CbPitch+(x))])
#define ModePredInfo_TotalCoeffL(mpi,x,y) (mpi->TotalCoeffL[(((y)%mpi_mod_L)*mpi->TbPitch+(x))])
#define ModePredInfo_Intra4x4PredMode(mpi,x,y) (mpi->Intra4x4PredMode[(((y)%mpi_mod_L)*mpi->TbPitch+(x))])
#define ModePredInfo_MVx(mpi,x,y) (mpi->MVx[(((y)%mpi_mod_L)*mpi->TbPitch+(x))])
#define ModePredInfo_MVy(mpi,x,y) (mpi->MVy[(((y)%mpi_mod_L)*mpi->TbPitch+(x))])
*/

/* Deblocking Filter */
//#define ModePredInfo_QPy(mpi,x,y) (mpi->QPy[(y)*mpi->MbPitch+(x)])
//#define ModePredInfo_QPc(mpi,x,y) (mpi->QPc[(y)*mpi->MbPitch+(x)])


mode_pred_info *alloc_mode_pred_info(int32_t img_width, int32_t img_height, int32_t slice_numbers,
				     int32_t slice_compteur);/*Modification For multislice
							   by passing the slice_compteur info*/
void clear_mode_pred_info(mode_pred_info *mpi,slice_header *sh);/*Modification For multislice
								  by passing the sh information*/
void free_mode_pred_info(mode_pred_info *mpi);

int32_t get_mb_mode(mode_pred_info *mpi, int32_t mb_x, int32_t mb_y);

int32_t get_luma_nC(mode_pred_info *mpi, int32_t x, int32_t y);
int32_t get_chroma_nC(mode_pred_info *mpi, int32_t x, int32_t y, int32_t iCbCr);

int32_t get_predIntra4x4PredMode(mode_pred_info *mpi, int32_t x, int32_t y);

mv PredictMV(mode_pred_info *mpi, int32_t org_x, int32_t org_y, int32_t width, int32_t height);
mv Predict_P_Skip_MV(mode_pred_info *mpi, int32_t org_x, int32_t org_y);
void FillMVs(mode_pred_info *mpi, int32_t org_x, int32_t org_y, int32_t width, int32_t height,
             int16_t mvx, int16_t mvy);
void DeriveMVs(mode_pred_info *mpi,   int32_t org_x, int32_t org_y,
               int32_t width, int32_t height, int16_t mvdx, int16_t mvdy);
void Derive_P_Skip_MVs(mode_pred_info *mpi, int32_t org_x, int32_t org_y);

/*
unsigned char get_border_num(mode_pred_info * mpi, int32_t x, int32_t y);
unsigned char check_border(mode_pred_info * mpi, int32_t x, int32_t y);
*/

#endif /*__MODE_PRED_H__*/

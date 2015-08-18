/*****************************************************************************

  filter.c -- File managing the deblocking filter process

  Original Author: 
  Joel Porquet, THALES COM - AAL, 2006-07-01
  Fabien Colas-Bigey THALES COM - AAL, 2008

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

#include "filter.h"


static const char alpha_t[52] = {  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0, 
                                   4,   4,   5,   6,  7,  8,  9, 10, 12, 13,  15,  17,  20,  22,  25,  28, 
				   32,  36,  40,  45, 50, 56, 63, 71, 80, 90, 101, 113, 127, 144, 162, 182, 
				   203, 226, 255, 255};
static const char beta_t[52] = {  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0, 
                                  2,   2,   2,   3,  3,  3,  3,  4,  4,  4,   6,   6,   7,   7,   8,   8, 
                                  9,   9,  10,  10, 11, 11, 12, 12, 13, 13,  14,  14,  15,  15,  16,  16, 
				  17,  17,  18,  18};

static const char tc0_t[3][52] = {
  /* bS == 1 */
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1, 
    1,  2,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  6,  6,  7,  8,
    9, 10, 11, 13 },
  /* bS == 2 */
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2, 
    2,  2,  2,  3,  3,  3,  4,  4,  5,  5,  6,  7,  8,  8, 10, 11, 
    12, 13, 15, 17 },
  /* bS == 3 */
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  3, 
    3,  3,  4,  4,  4,  5,  6,  6,  7,  8,  9, 10, 11, 13, 14, 16, 
    18, 20, 23, 25 } };

typedef struct{
  unsigned char p[4];
  unsigned char q[4];
}s_qp;


static int32_t PicWidthInMbs;


//----------------------------------------------------------------
/*!
  Calculation of the boundary strength : 0, 1, 2, 3 or 4.
 */
//----------------------------------------------------------------
typedef struct _res_mv
{ int16_t x,y; } res_mv;

static unsigned char is_out_slice(mode_pred_info *mpi, int32_t x, int32_t y)
{
  /* Parameters x and Y are expressed as 4x4 positions */
  int32_t mb_pos = (y >> 2) * mpi->MbPitch + (x >>2);
  if ((mb_pos < mpi->mpi_firstMb_in_slice) || (mb_pos >= (mpi->mpi_firstMb_in_slice + mpi->slice_MbSize)))
    return 1;
  else
    return 0;
}

static inline res_mv get_MV(mode_pred_info * mpi, int32_t x, int32_t y)
{
  /* Declarations and initialisations */
  res_mv res = { 0, 0 };
  x >>= 2;			/* to address 4x4 blocks */
  y >>= 2;			/* to address 4x4 blocks */

  if (x < 0 || x >= mpi->TbPitch || is_out_slice(mpi,x,y))
    return res;
  
  res.x = ModePredInfo_MVx(mpi, x, y);
  res.y = ModePredInfo_MVy(mpi, x, y);
  //if (res.x == MV_NA) {
  if (res.x == -32640) {
    res.x = 0;
    res.y = 0;
  }
  return res;
}

static int32_t get_bS (int32_t qx, int32_t qy, int32_t px, int32_t py, mode_pred_info *mpi)
{
  /* filtering strength */
  int32_t bS = 0;

  //printf("start bs\n");
  
  if (qx%16 == 0 && (IsIntra(ModePredInfo_MbMode(mpi, qx>>4, qy>>4))
		     || IsIntra(ModePredInfo_MbMode(mpi, px>>4, py>>4))))
    bS = 4;

  else if (IsIntra(ModePredInfo_MbMode(mpi, qx>>4, qy>>4)))
    bS = 3;

  else if (ModePredInfo_TotalCoeffL(mpi, qx>>2, qy>>2) != 0
	   || ModePredInfo_TotalCoeffL(mpi, px>>2, py>>2) != 0)	/* added by fabien */
    bS = 2;
  else{
    res_mv mvp, mvq;
    mvp = get_MV(mpi,px,py);
    mvq = get_MV(mpi,qx,qy);
    if (abs( mvq.x - mvp.x ) > 4 || abs( mvq.y - mvp.y ) > 4)
      bS = 1;
  }

  /*
    else if (abs(ModePredInfo_MVx(mpi, qx>>2, qy>>2) -
    ModePredInfo_MVx(mpi, px>>2, py>>2)) > 4
    || abs(ModePredInfo_MVy(mpi, qx>>2, qy>>2) - 
    ModePredInfo_MVy(mpi, px>>2, py>>2)) > 4)
    bS = 1;
  */
  //printf("bs = %d and %d and %d\n",bS,qx,qy);
  /*
    if (qx==145 && qy==248)
    printf("%d %d %d %d\n",
    ModePredInfo_MVx(mpi, qx>>2, qy>>2),ModePredInfo_MVx(mpi, px>>2, py>>2),
    ModePredInfo_MVy(mpi, qx>>2, qy>>2),ModePredInfo_MVy(mpi, px>>2, py>>2));
  */
  /*
    if (qx==144 && qy==224)
    printf("mode_q = %d, mode_p = %d\n",ModePredInfo_MbMode(mpi, qx>>4, qy>>4),
    ModePredInfo_MbMode(mpi, px>>4, py>>4));
  */
  return bS;
}

static s_qp filter_block (int32_t qx, int32_t qy, int32_t px, int32_t py, mode_pred_info *mpi, int32_t chroma_flag, s_qp qp, int32_t bS, slice_header *sh)
{
  s_qp res = qp;
    
  /* threshold */
  int32_t QPq;
  int32_t QPp;
  if (chroma_flag == 0){
    QPq = ModePredInfo_QPy(mpi, qx>>4, qy>>4);
    QPp = ModePredInfo_QPy(mpi, px>>4, py>>4);
  } 
  else {
    QPq = ModePredInfo_QPc(mpi, qx>>4, qy>>4);
    QPp = ModePredInfo_QPc(mpi, px>>4, py>>4);
  }
  int32_t QPav = (QPq + QPp + 1)>>1;
    
  int32_t indexA = CustomClip(QPav + (sh->slice_alpha_c0_offset_div2<<2), 0, 51);
  int32_t indexB = CustomClip(QPav + (sh->slice_beta_offset_div2<<2),     0, 51);
  int32_t alpha   = alpha_t[indexA];
  int32_t beta    = beta_t[indexB];

  int32_t filterSampleFlag = ( bS!=0 
			   && abs(qp.p[0] - qp.q[0])<alpha 
			   && abs(qp.p[1] - qp.p[0])<beta 
			   && abs (qp.q[1] - qp.q[0])<beta);

  if (filterSampleFlag == 1 && bS < 4)
    {
      int32_t tc0, Ap, Aq, tc, delta;

      Ap = abs(qp.p[2] - qp.p[0]);
      Aq = abs(qp.q[2] - qp.q[0]);
      tc0 = tc0_t[bS][indexA];
        
      if (chroma_flag == 0)
	tc = tc0 + ((Ap<beta)?1:0) + ((Aq<beta)?1:0);
      else
	tc = tc0 + 1;
            
      delta = CustomClip(((((qp.q[0] - qp.p[0])<<2) + (qp.p[1] - qp.q[1]) + 4)>>3), -tc, tc);

      res.p[0] = Clip(qp.p[0] + delta);
      res.q[0] = Clip(qp.q[0] - delta);


      if (Ap < beta && chroma_flag == 0)
	res.p[1] += CustomClip((qp.p[2] + ((qp.p[0]+qp.q[0]+1)>>1) - (qp.p[1]<<1))>>1, -tc0, tc0);

      if (Aq < beta && chroma_flag == 0)
	res.q[1] += CustomClip((qp.q[2] + ((qp.p[0]+qp.q[0]+1)>>1) - (qp.q[1]<<1))>>1, -tc0, tc0);
            
        
    } else if (filterSampleFlag == 1 && bS == 4)
      {
        int32_t Ap, Aq;

        Ap = abs(qp.p[2] - qp.p[0]);
        Aq = abs(qp.q[2] - qp.q[0]);

        if (chroma_flag == 0 && Ap < beta && abs(qp.p[0] - qp.q[0]) < ((alpha>>2)+2))
	  {
            res.p[0] = (qp.p[2] + 2*qp.p[1] + 2*qp.p[0] + 2*qp.q[0] + qp.q[1] + 4)>>3;
            res.p[1] = (qp.p[2] + qp.p[1] + qp.p[0] + qp.q[0] + 2)>>2;
            res.p[2] = (2*qp.p[3] + 3*qp.p[2] + qp.p[1] + qp.p[0] + qp.q[0] + 4)>>3;
	  }
	else {
	  res.p[0] = (2*qp.p[1] + qp.p[0] + qp.q[1] + 2)>>2;
	}
        if (chroma_flag == 0 && Aq < beta && abs(qp.p[0] - qp.q[0]) < ((alpha>>2)+2))
	  {
            res.q[0] = (qp.p[1] + 2*qp.p[0] + 2*qp.q[0] + 2*qp.q[1] + qp.q[2] + 4)>>3;
            res.q[1] = (qp.p[0] + qp.q[0] + qp.q[1] + qp.q[2] + 2)>>2;
            res.q[2] = (2*qp.q[3] + 3*qp.q[2] + qp.q[1] + qp.q[0] + qp.p[0] + 4)>>3;
	  }
	else
	  {
            res.q[0] = (2*qp.q[1] + qp.q[0] + qp.p[1] + 2)>>2;
	  }
      }
  
  return res;
}

//----------------------------------------------------------------
/*!
  Applies the deblocking filter on a maximum set of 8 pixels
  around the edge.
 */
//----------------------------------------------------------------
static void filter_vertical_L (int32_t x, int32_t y, frame *this, mode_pred_info *mpi, slice_header *sh)
{
  s_qp qp;
  int32_t i;

  /* stores pixels around the edge in specific structure */
  for (i=0; i<4; i++){
    qp.q[i] = L_pixel(this, x + i, y);
    //printf("pix = %d at %d %d\n",qp.q[i],x+i,y);
    qp.p[i] = L_pixel(this, x-i-1, y);
    //printf("pix = %d at %d %d\n",qp.p[i],x-i-1,y);
  }

  //printf("filt x=%d y=%d\n",x,y);
  int32_t bS = get_bS(x, y, x-16, y, mpi);

  s_qp res = filter_block(x, y, x-16, y, mpi, 0, qp, bS, sh);

  /* writes back filtered pixels into the frame buffer */
  for (i=0; i<4; i++){
    L_pixel(this, x + i, y) = res.q[i];
    //printf("pix = %d at %d %d\n",res.q[i],x+i,y);
    L_pixel(this, x-i-1, y) = res.p[i];
    //printf("pix = %d at %d %d\n",res.p[i],x-i-1,y);
  }
}

static void filter_horizontal_L (int32_t x, int32_t y, frame *this, mode_pred_info *mpi, slice_header *sh)
{
  s_qp qp;
  int32_t i;

  for (i=0; i<4; i++){
    qp.q[i] = L_pixel(this, x, y+i);
    qp.p[i] = L_pixel(this, x, y-i-1);
  }

  int32_t bS = get_bS(x, y, x, y-16, mpi);

  s_qp res = filter_block(x, y, x, y-16, mpi, 0, qp, bS, sh);

  for (i=0; i<4; i++) {
    L_pixel(this, x, y+i) = res.q[i];
    L_pixel(this, x, y-i-1) = res.p[i];
  }
}

/* Chroma */
static void filter_vertical_C (int32_t x, int32_t y, frame *this, mode_pred_info *mpi, slice_header *sh, int32_t iCbCr)
{
  s_qp qp;
  int32_t i;
  
  for (i=0; i<4; i++){
    qp.q[i] = C_pixel(this, iCbCr, (x>>1) + i, (y>>1));
    qp.p[i] = C_pixel(this, iCbCr, (x>>1)-i-1, (y>>1));
  }
  
  int32_t bS = get_bS(x, y, x-16, y, mpi);
  
  s_qp res = filter_block(x, y, x-16, y, mpi, 1, qp, bS, sh);
  
  for (i=0; i<4; i++){
    C_pixel(this, iCbCr, (x>>1) + i, (y>>1)) = res.q[i];
    C_pixel(this, iCbCr, (x>>1)-i-1, (y>>1)) = res.p[i];
  }
}

static void filter_horizontal_C (int32_t x, int32_t y, frame *this, mode_pred_info *mpi, slice_header *sh, int32_t iCbCr)
{
  s_qp qp;
  int32_t i;

  for (i=0; i<4; i++){
    qp.q[i] = C_pixel(this, iCbCr, (x>>1), (y>>1)+i);
    qp.p[i] = C_pixel(this, iCbCr, (x>>1), (y>>1)-i-1);
  }

  int32_t bS = get_bS(x, y, x, y-16, mpi);

  s_qp res = filter_block(x, y, x, y-16, mpi, 1, qp, bS, sh);

  for (i=0; i<4; i++){
    C_pixel(this, iCbCr, (x>>1), (y>>1)+i) = res.q[i];
    C_pixel(this, iCbCr, (x>>1), (y>>1)-i-1) = res.p[i];
  }
}

//----------------------------------------------------------------
/*!
  Applies the deblocking filter on the whole slice.
  - The left and top edges of the image are ignored.

 */
//----------------------------------------------------------------
void filter_slice (slice_header *sh,
		   seq_parameter_set *sps, pic_parameter_set *pps,
		   frame *this, mode_pred_info *mpi)
{
  /*************************************
     Internal declarations
  *************************************/
  int32_t CurrMbAddr = 0;
  int32_t MbCount=0;
  int32_t mb_pos_x,mb_pos_y = 0;
  int32_t SliceHeightInMbs = 0;
  int32_t OffsetMb = 0;

  /*************************************
     Internal declaration
  *************************************/
  PicWidthInMbs = sps->PicWidthInMbs;
  SliceHeightInMbs = sh->PicHeightInMbs / pps->num_slice_groups;
  MbCount = PicWidthInMbs*SliceHeightInMbs;
  
  /* Selection of boundaries depending on slice number */
  OffsetMb = sh->first_mb_in_slice * (1 + sh->MbaffFrameFlag);
  
  //printf("new filter slice %d\n",MbCount+OffsetMb);
  
  /* Main process */
  for (CurrMbAddr = OffsetMb; CurrMbAddr<(MbCount+OffsetMb); CurrMbAddr++)
    {
      //printf("filter mb = %d\n",CurrMbAddr);

      /* Position of the upper left pixel of the 16x16 MB
	 considered here */
      mb_pos_x = (CurrMbAddr%PicWidthInMbs) << 4;
      mb_pos_y = (CurrMbAddr/PicWidthInMbs) << 4;

      /****************/
      /* LUMA SAMPLES */
      /****************/
      /* L vertical edges filtering (from left to right) */
      int32_t vertical_edge;
      //unsigned char v_border = 1;
      for (vertical_edge = 0; vertical_edge <4; vertical_edge++)
        {
	  /* skip left boundary of the slice */
	  /*
	  if (mb_pos_x == 0 && v_border == 1)
	    v_border = 0;
	  else {
	  */
	  if (mb_pos_x == 0)
	    continue;
	  /* iterates on the 16 lines of the Mb */
	  int32_t lines;
	  for (lines=0; lines<16; lines++){
	    filter_vertical_L (mb_pos_x + (vertical_edge<<2),
			       mb_pos_y + lines,
			       this, mpi, sh);
	    }
	  //}
	}
      
      /* L horizontal edges filtering (from top to bottom) */
      int32_t horizontal_edge;
      //unsigned char h_border = 1;
      for (horizontal_edge = 0; horizontal_edge<4; horizontal_edge++)
        {
	  /* skip top boundary of the slice */
	  /*
	  if ( mb_pos_y%(SliceHeightInMbs*16) == 0 && h_border == 1)
	    h_border = 0;
	  else {
	  */
	  if (mb_pos_y%(SliceHeightInMbs*16) == 0)
	    continue;
	  /* iterates on the 16 columns of the Mb */
	  int32_t cols;
	  for (cols=0; cols<16; cols++)
	    filter_horizontal_L (mb_pos_x + cols,
				 mb_pos_y + (horizontal_edge<<2),
				 this, mpi, sh);
	  //}
        }

      /******************/
      /* CHROMA SAMPLES */
      /******************/
      int32_t iCbCr;
      for (iCbCr = 0; iCbCr<2; iCbCr++)
        {
	  // C vertical edges filtering
	  int32_t vertical_edge;
	  //unsigned char v_border = 1;
	  for (vertical_edge = 0; vertical_edge <2; vertical_edge++)
            {
	      // Skip left boundary of the slice
	      /*
	      if (mb_pos_x == 0 && v_border == 1)
		v_border = 0;
	      else {
	      */
	      if (mb_pos_x == 0)
		continue;
	      int32_t lines;
	      for (lines=0; lines<8; lines++)   
		filter_vertical_C(mb_pos_x + (vertical_edge<<2),
				  mb_pos_y + lines,
				  this, mpi, sh, iCbCr);
	      //}
            }

	  // C horizontal edges filtering
	  int32_t horizontal_edge;
	  //unsigned char h_border = 1;
	  for (horizontal_edge = 0; horizontal_edge<2; horizontal_edge++)
            {
	      // Skip top boundary of the slice
	      /*
	      if ( mb_pos_y%(SliceHeightInMbs*16) == 0 && h_border == 1)
		h_border = 0;
	      else {
	      */
	      if (mb_pos_y%(SliceHeightInMbs*16) == 0)
		continue;
	      int32_t cols;
	      for (cols=0; cols<8; cols++)
		filter_horizontal_C (mb_pos_x + cols,
				     mb_pos_y + (horizontal_edge<<2),
				     this, mpi, sh, iCbCr);
	      //}
            }
        }
    }
}

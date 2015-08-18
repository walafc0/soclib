/*****************************************************************************

  slice.c -- File where all the major steps for decoding are done 
   - decode_slice_data_function call all the others functions
   to manage the decoding process and rendering operations
   - Add of control to support multislices

  Original Author: 
  Martin Fiedler, CHEMNITZ UNIVERSITY OF TECHNOLOGY, 2004-06-01	
  
  Thales Author:
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

#include "slice.h"

/****************************************************************************
  Variables and structures
****************************************************************************/
extern int32_t frame_no;
extern int32_t CodedBlockPatternMapping_Intra4x4[];
extern int32_t CodedBlockPatternMapping_Inter[];

static int32_t Intra4x4ScanOrder[16][2] =
  { {0, 0}, {4, 0}, {0, 4}, {4, 4},
    {8, 0}, {12, 0}, {8, 4}, {12, 4},
    {0, 8}, {4, 8}, {0, 12}, {4, 12},
    {8, 8}, {12, 8}, {8, 12}, {12, 12}
  };
static char QPcTable[22] =
  { 29, 30, 31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38,
    38, 39, 39, 39, 39
  };

extern uint32_t stream_height;
extern uint32_t stream_width;

void decode_slice_data(slice_header * sh,
		       seq_parameter_set * sps, pic_parameter_set * pps,
		       nal_unit * nalu,
		       frame * this, frame * ref, mode_pred_info * mpi)
{
  /*************************************
     Internal declaration
  *************************************/

  uint32_t predict_mb[16][16];

  int32_t CurrMbAddr = 0;	// Adress of the current mb
  int32_t moreDataFlag = 1;
  int32_t prevMbSkipped = 0;
  int32_t MbCount = 0;//sh->PicSizeInMbs;	// Tot number of mb in the frame

  int32_t mb_skip_run;
  int32_t mb_qp_delta;
  int32_t QPi;
  char QPy, QPc;
  int32_t intra_chroma_pred_mode = 0;

  int32_t mb_pos_x, mb_pos_y;
  mb_mode mb;
  sub_mb_mode sub[4];

  int32_t LumaDCLevel[16];		// === Intra16x16DCLevel
  int32_t LumaACLevel[16][16];		// === Intra16x16ACLevel
  int32_t ChromaDCLevel[2][4];
  int32_t ChromaACLevel[2][4][16];

#if (defined(MB_DISPLAY) && !defined(CONFIG_ARCH_EMU))
  uint32_t offset = 0;
  uint32_t block_line = 0;
  uint32_t luma_block_size = 16;
  uint32_t chroma_block_size = 8;
#endif

  /*************************************
     Initialiaation
  *************************************/
  CurrMbAddr = sh->first_mb_in_slice * (1 + sh->MbaffFrameFlag);
  //MbCount = CurrMbAddr + pps->SliceGroupChangeRate;
  MbCount = CurrMbAddr + mpi->slice_MbSize;

  // printf("Thread: %d -- First Mb in slice: %d\n",mpi->ID,mpi->mpi_slice_ID);
  // Init of struct containing all the prediction modes for the Slice
  clear_mode_pred_info(mpi, sh);

  // Init of quantization params
  QPy = sh->SliceQPy;
  QPi = QPy + pps->chroma_qp_index_offset;
  QPi = CustomClip(QPi, 0, 51);
  if (QPi < 30) QPc = QPi;
  else QPc = QPcTable[QPi - 30];
  //QPc = QPy;
  
  // Is there more info to be read in the nal
  moreDataFlag = more_rbsp_data_thread(nalu, mpi);
  /*
  if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
    printf("Slice process thread n°%d\n",mpi->ID);
  */
  //printf("\nCurrMbAddr = %d, MbCount = %d\n",CurrMbAddr, MbCount);

  /*************************************
     Main process
  *************************************/
  while (moreDataFlag && CurrMbAddr <= MbCount) {
    /*
    if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
      printf("CurrMbAddr = %d\n",CurrMbAddr);
    */
//    printk("CurrMbAddr = %d\n",CurrMbAddr);
    /*************************************
       Skipped macroblocks
    *************************************/
    if (sh->slice_type != I_SLICE && sh->slice_type != SI_SLICE) {
      mb_skip_run = get_unsigned_exp_golomb_thread(mpi->ID);
      prevMbSkipped = (mb_skip_run > 0);

      for (; mb_skip_run; --mb_skip_run, ++CurrMbAddr) {
	// MultiSlice
	if (pps->num_slice_groups == 1) {	// Case of 1Slice/Frame
	  if (CurrMbAddr >= MbCount) {
	    return;
	  }
	} else {	// Other Cases
	  if ((CurrMbAddr - mpi->mpi_firstMb_in_slice) >= pps->SliceGroupChangeRate) {	// Test if the mb isn't inside the current slice
	    return;
	  }
	}
	
	// Position expressed in number of 16x16 mbs
	mb_pos_x = CurrMbAddr % sps->PicWidthInMbs;
	mb_pos_y = CurrMbAddr / sps->PicWidthInMbs;
	
	// Mb mode assignement
	set_ModePredInfo_MbMode(mpi, mb_pos_x, mb_pos_y,P_Skip);
	
	// Position expressed in number of pix
	mb_pos_x <<= 4;
	mb_pos_y <<= 4;

	// Motion Vectors calculation
	// printf("\n[frame_Num:%d : currMbAddr:%d] mb_pos_x:%d ,mb_pos_y:%d MB: P_Skip\n",frame_no,CurrMbAddr,mb_pos_x,mb_pos_y);
	Derive_P_Skip_MVs(mpi, mb_pos_x, mb_pos_y);
	
	// Rendering process : reconstruction of predicted macroblock (inter pred)
	MotionCompensateMB(this, ref, mpi, mb_pos_x, mb_pos_y);

#if (defined(MB_DISPLAY) && !defined(CONFIG_ARCH_EMU))
	// Add MB printing here
	for (block_line = 0 ; block_line < luma_block_size ; block_line += 1) {
	  offset = mb_pos_y * stream_width + mb_pos_x + block_line * stream_width;
	  memcpy((void *) (SEG_BUFF_ADDR + offset), &(this->L[offset]), luma_block_size);
	}
	for (block_line = 0 ; block_line < chroma_block_size ; block_line += 1) {
	  offset = (mb_pos_y>>1) * stream_width / 2 + (mb_pos_x>>1) + block_line * stream_width / 2;
	  memcpy((void *) (SEG_BUFF_ADDR + offset + stream_width * stream_height), &(this->C[0][offset]), chroma_block_size);
	  memcpy((void *) (SEG_BUFF_ADDR + offset + stream_width * stream_height + stream_width * stream_height / 4), &(this->C[1][offset]), chroma_block_size);
	}
#endif
      }
      moreDataFlag = more_rbsp_data_thread(nalu, mpi);
    }

    // Added in case of MultiSlice process
    if (pps->num_slice_groups == 1) { // Case of 1Slice/Frame
      if (CurrMbAddr >= MbCount)
	return;
    } else {			// Other Cases
      if ((CurrMbAddr - mpi->mpi_firstMb_in_slice) >= pps->SliceGroupChangeRate) // Test if the mb isn't inside the current slide
	return;
    }
    
    /*************************************
       Macroblock layer
    *************************************/
    if (moreDataFlag) {
      decode_mb_mode(&mb, sh->slice_type, get_unsigned_exp_golomb_thread(mpi->ID));
      //printf("mode = %d\n",mb.mb_type);
      
      // Position expressed in number of 16x16 mbs
      mb_pos_x = CurrMbAddr % sps->PicWidthInMbs;
      mb_pos_y = CurrMbAddr / sps->PicWidthInMbs;
 
      // Set the macrobloc prediction modes of mode_pred_info
      set_ModePredInfo_MbMode(mpi, mb_pos_x, mb_pos_y, mb.mb_type);

      // Set coefficient for DBF
      set_ModePredInfo_QPy(mpi,mb_pos_x,mb_pos_y,QPy);
      set_ModePredInfo_QPc(mpi,mb_pos_x,mb_pos_y,QPc);
      
      // Position expressed in number of pix
      mb_pos_x <<= 4;
      mb_pos_y <<= 4;

      //printf("[%d:%d] {%d,%d} ",frame_no,CurrMbAddr,mb_pos_x,mb_pos_y);_dump_mb_mode(&mb);

      /*************************************
          I_PCM : original pix values are
          sent without any compression
      *************************************/
      if (mb.mb_type == I_PCM) {
	int32_t x, y, iCbCr;
	unsigned char *pos;
	input_align_to_next_byte_thread(mpi->ID);
	pos = &L_pixel(this, mb_pos_x, mb_pos_y);

	for (y = 16; y; --y) {
	  for (x = 16; x; --x)
	    *pos++ = input_get_byte_thread(mpi->ID);
	  pos += this->Lpitch - 16;
	}
	for (iCbCr = 0; iCbCr < 2; ++iCbCr) {
	  pos = &C_pixel(this, iCbCr, mb_pos_x >> 1,
			 mb_pos_y >> 1);
	  for (y = 8; y; --y) {
	    for (x = 8; x; --x)
	      *pos++ = input_get_byte_thread(mpi->ID);
	    pos += this->Cpitch - 8;
	  }
	}
	// fix mode_pred_info->TotalCoeff data
	for (y = 0; y < 4; ++y)
	  for (x = 0; x < 4; ++x)
	    set_ModePredInfo_TotalCoeffL(mpi, (mb_pos_x >> 2) + x,
					 (mb_pos_y >> 2) + y, 16);
	for (y = 0; y < 2; ++y)
	  for (x = 0; x < 2; ++x) {
	    set_ModePredInfo_TotalCoeffC(mpi, (mb_pos_x >> 3) + x,
					 (mb_pos_y >> 3) + y, 0, 16);
	    set_ModePredInfo_TotalCoeffC(mpi, (mb_pos_x >> 3) + x,
					 (mb_pos_y >> 3) + y, 1, 16);
	  }
	
	// Deblocking Filter information
	set_ModePredInfo_QPy(mpi,mb_pos_x>>4,mb_pos_y>>4,0);
        set_ModePredInfo_QPc(mpi,mb_pos_x>>4,mb_pos_y>>4,0);
	
	// in this case we can get luma, chroma, qp
      }

      /*************************************
          Standard mb processing
      *************************************/
      else {
	/*************************************
          Inter mb : 8x8, need to decode sub
          partitions modes : sub_mb_pred
	*************************************/
	if (mb.MbPartPredMode[0] != Intra_4x4 &&
	    mb.MbPartPredMode[0] != Intra_16x16 && mb.NumMbPart == 4) {
	  int32_t mbPartIdx, subMbPartIdx;
	  for (mbPartIdx = 0; mbPartIdx < 4; ++mbPartIdx)
	    decode_sub_mb_mode(&sub[mbPartIdx], sh->slice_type,
			       get_unsigned_exp_golomb_thread(mpi->ID));
	  //  {int32_t i;printf("sub_mb_pred():");for(i=0;i<4;++i)printf(" %s(%d)",_str_sub_mb_type(sub[i].sub_mb_type),sub[i].NumSubMbPart);printf("\n");}
	  /* Motion Vectors calculation  */
	  for (mbPartIdx = 0; mbPartIdx < 4; ++mbPartIdx)
	    if (sub[mbPartIdx].sub_mb_type != B_Direct_8x8 && sub[mbPartIdx].SubMbPredMode != Pred_L1) { // SOF = "scan order factor"
	      int32_t SOF =	(sub[mbPartIdx].sub_mb_type == P_L0_8x4) ? 2 : 1;
	      for (subMbPartIdx = 0; subMbPartIdx < sub[mbPartIdx].NumSubMbPart; ++subMbPartIdx) {
		int16_t mvdx = get_signed_exp_golomb_thread(mpi->ID);
		int16_t mvdy = get_signed_exp_golomb_thread(mpi->ID);
		DeriveMVs(mpi, mb_pos_x + Intra4x4ScanOrder[(mbPartIdx << 2) + subMbPartIdx * SOF][0],
			  mb_pos_y + Intra4x4ScanOrder[(mbPartIdx << 2) + subMbPartIdx * SOF][1],
			  sub[mbPartIdx].SubMbPartWidth, sub[mbPartIdx].SubMbPartHeight,
			  mvdx, mvdy);
	      }
	    }
	}
	else {
	  /*************************************
             Intra mb : mode decoding
	  *************************************/
	  if (mb.MbPartPredMode[0] == Intra_4x4 || mb.MbPartPredMode[0] == Intra_16x16) {
	    if (mb.MbPartPredMode[0] == Intra_4x4) {
	      int32_t luma4x4BlkIdx;
	      for (luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; ++luma4x4BlkIdx) {
		// Get the mb_pred_mode according the 2 neighbouring mbs
		int32_t predIntra4x4PredMode = get_predIntra4x4PredMode(mpi, mb_pos_x + Intra4x4ScanOrder[luma4x4BlkIdx][0],
									mb_pos_y + Intra4x4ScanOrder[luma4x4BlkIdx][1]);
		/*
		if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
		  printf("mb = %d, pred = %d\n",CurrMbAddr,predIntra4x4PredMode);
		*/
		if (input_get_one_bit_thread(mpi->ID)) {
		  //printf("test\n");
		  Intra4x4PredMode(luma4x4BlkIdx,predIntra4x4PredMode); // Macro: stores the mb_pred_mode
		}
		else {
		  int32_t rem_intra4x4_pred_mode = input_get_bits_thread(3, mpi->ID);
		  if (rem_intra4x4_pred_mode < predIntra4x4PredMode)
		    Intra4x4PredMode(luma4x4BlkIdx,rem_intra4x4_pred_mode);	// Store the mb_pred_mode
		  else
		    Intra4x4PredMode(luma4x4BlkIdx,rem_intra4x4_pred_mode + 1);
		}
	      }
	    }
	    intra_chroma_pred_mode = get_unsigned_exp_golomb_thread(mpi->ID);
	    /*
	    if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
	      printf("mb = %d, pred_mod = %d, chroma mode = %d\n",
		     CurrMbAddr, mb.MbPartPredMode[0], intra_chroma_pred_mode);
	    */
	  }
	  /*************************************
             Inter mb : MV prediction
	  *************************************/
	  else {
	    int32_t mbPartIdx;
	    int32_t SOF = (mb.mb_type == P_L0_L0_16x8) ? 8 : 4;

	    for (mbPartIdx = 0; mbPartIdx < mb.NumMbPart; ++mbPartIdx)
	      if (mb.MbPartPredMode[mbPartIdx] != Pred_L1) {
		int16_t mvdx = get_signed_exp_golomb_thread(mpi->ID);
		int16_t mvdy = get_signed_exp_golomb_thread(mpi->ID);
		DeriveMVs(mpi, mb_pos_x + Intra4x4ScanOrder[mbPartIdx * SOF][0],
			  mb_pos_y + Intra4x4ScanOrder[mbPartIdx * SOF][1],
			  mb.MbPartWidth, mb.MbPartHeight,
			  mvdx, mvdy);
	      }
	  }
	}

	/*************************************
             Coded block patterns decoding
	*************************************/
	if (mb.MbPartPredMode[0] != Intra_16x16) {
	  int32_t coded_block_pattern = get_unsigned_exp_golomb_thread(mpi->ID);
	  if (mb.MbPartPredMode[0] == Intra_4x4)
	    coded_block_pattern = CodedBlockPatternMapping_Intra4x4[coded_block_pattern];
	  else
	    coded_block_pattern = CodedBlockPatternMapping_Inter[coded_block_pattern];
	  
	  mb.CodedBlockPatternLuma = coded_block_pattern & 15;
	  mb.CodedBlockPatternChroma = coded_block_pattern >> 4;
	  //if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
	  //  _dump_mb_mode(&mb);
	}
	/* Before parsing the residual data, set all coefficients to zero. In
	   the original H.264 documentation, this is done either in
	   residual_block() at the very beginning or by setting values to zero
	   according to the CodedBlockPattern values. So, there's only little
	   overhead if we do it right here.*/
	memset(LumaDCLevel, 0, sizeof(LumaDCLevel));
	memset(LumaACLevel, 0, sizeof(LumaACLevel));
	memset(ChromaDCLevel, 0, sizeof(ChromaDCLevel));
	memset(ChromaACLevel, 0, sizeof(ChromaACLevel));

	/*************************************
             Residual decodinng
	*************************************/
	if (mb.CodedBlockPatternLuma > 0 || mb.CodedBlockPatternChroma > 0
	    || mb.MbPartPredMode[0] == Intra_16x16) {
	  int32_t i8x8, i4x4, iCbCr;
	  
	  // DBF coefficients calculation
	  mb_qp_delta = get_signed_exp_golomb_thread(mpi->ID);
	  QPy = (QPy + mb_qp_delta + 52) % 52;
	  QPi = QPy + pps->chroma_qp_index_offset;
	  QPi = CustomClip(QPi, 0, 51);
	  if (QPi < 30) QPc = QPi;
	  else QPc = QPcTable[QPi - 30];
	  set_ModePredInfo_QPy(mpi,mb_pos_x>>4,mb_pos_y>>4,QPy);
	  set_ModePredInfo_QPc(mpi,mb_pos_x>>4,mb_pos_y>>4,QPc);
	  //printf("mb_qp_delta=%d QPy=%d QPi=%d QPc=%d\n",mb_qp_delta,QPy,QPi,QPc);

	  // OK, now let's parse the hell out of the stream ;)        
	  if (mb.MbPartPredMode[0] == Intra_16x16)
	    residual_block(&LumaDCLevel[0], 16, LumaDC_nC,
			   mpi->ID);
	  for (i8x8 = 0; i8x8 < 4; ++i8x8)
	    for (i4x4 = 0; i4x4 < 4; ++i4x4)
	      if (mb.CodedBlockPatternLuma & (1 << i8x8)) {
		if (mb.MbPartPredMode[0] == Intra_16x16)
		  {
		    int32_t tmp;
		    tmp = residual_block(&LumaACLevel[(i8x8 << 2) + i4x4][1], 15,
					 LumaAC_nC, mpi->ID);
		    LumaAdjust(tmp);
		  }
		else
		  {
		    int32_t tmp;
		    tmp = residual_block(&LumaACLevel[(i8x8 << 2) + i4x4][0], 16,
					 LumaAC_nC, mpi->ID);
		    /*
		      if (MbCount == 99)
		      printf("Luma_nc = %d\n",LumaAC_nC);
		    */
		    LumaAdjust(tmp);
		    /*
		      int32_t aa = ( ( (mb_pos_y+Intra4x4ScanOrder[(i8x8<<2)+i4x4][1]) >> 2 ) * mpi->TbPitch) + ( (mb_pos_x+Intra4x4ScanOrder[(i8x8<<2)+i4x4][0]) >> 2);
		      int32_t bb = ((mpi->y_first)>>2) * mpi->TbPitch +  (mpi->x_first>>2);
		      printf("offset = %d\n",aa-bb);
		    */
		  }
	      };
	  //printf("pattern chroma %d\n",mb.CodedBlockPatternChroma);
	  for (iCbCr = 0; iCbCr < 2; iCbCr++)
	    if (mb.CodedBlockPatternChroma & 3)
	      {
		int32_t tmp;
		tmp = residual_block(&ChromaDCLevel[iCbCr][0], 4,
				     ChromaDC_nC, mpi->ID);
	      }
	  for (iCbCr = 0; iCbCr < 2; iCbCr++)
	    for (i4x4 = 0; i4x4 < 4; ++i4x4)
	      if (mb.CodedBlockPatternChroma & 2)
		{
		  int32_t tmp;
		  tmp = residual_block(&ChromaACLevel[iCbCr][i4x4][1], 15,
				       ChromaAC_nC, mpi->ID);
		  //printf("chromaacnc = %d\n",ChromaAC_nC);
		  ChromaAdjust(tmp);
		  /*
		    int32_t aa = ( (mb_pos_y+((i4x4>>1)<<3))>>3 ) * mpi->CbPitch + (( mb_pos_x+ ( (i4x4&1) << 3 ) ) >> 3 );
		    int32_t bb = ((mpi->y_first)>>3) * mpi->CbPitch +  (mpi->x_first>>3);
		    printf("aa = %d, bb = %d\n",aa,bb);
		    printf("x = %d, y = %d\n",( (mb_pos_y+((i4x4>>1)<<3))>>3 ), (( mb_pos_x+ ( (i4x4&1) << 3 ) ) >> 3 ));
		    printf("offset = %d\n",aa-bb);
		  */
		}
	  
	  /* if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
	  	  { int32_t i; printf("L:"); for(i=0; i<16; ++i) printf(" %d",LumaDCLevel[i]); printf("\n");
	  	  	                 for(i8x8=0; i8x8<16; ++i8x8) { printf("  [%2d]",i8x8);
	  	  	                 for(i=0; i<16; ++i) printf(" %d",LumaACLevel[i8x8][i]); printf("\n"); }
	  	  	                 printf("Cb:"); for(i=0; i<4; ++i) printf(" %d",ChromaDCLevel[0][i]); printf("\n");
	  	  	                 for(i8x8=0; i8x8<4; ++i8x8) { printf("  [%d]",i8x8);
	  	  	                 for(i=0; i<16; ++i) printf(" %d",ChromaACLevel[0][i8x8][i]); printf("\n"); }
	  	  	                 printf("Cr:"); for(i=0; i<4; ++i) printf(" %d",ChromaDCLevel[1][i]); printf("\n");
	  	  	                 for(i8x8=0; i8x8<4; ++i8x8) { printf("  [%d]",i8x8);
	  	  	                 for(i=0; i<16; ++i) printf(" %d",ChromaACLevel[1][i8x8][i]); printf("\n"); } } */
	}
	
	/*************************************
             Rendering
	*************************************/
	if (mb.MbPartPredMode[0] == Intra_4x4) {
	  int32_t i;
	  for (i = 0; i < 16; ++i) {
	    int32_t x = mb_pos_x + Intra4x4ScanOrder[i][0];
	    int32_t y = mb_pos_y + Intra4x4ScanOrder[i][1];

	    Intra_4x4_Dispatch(this, mpi, x, y, i);
	    
	    // For debug purpose
	   /*  int32_t aa,bb;
	    if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
	      printf("Pixels ");
	    for(bb=0;bb<4;bb++)
	      for(aa=0;aa<4;aa++){
		predict_mb[Intra4x4ScanOrder[i][0]+bb][Intra4x4ScanOrder[i][1]+aa]=L_pixel(this,x+bb,y+aa);
		if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
		  printf("%d ",L_pixel(this,x+bb,y+aa));
	      }
	    if (mpi->mpi_firstMb_in_slice == DEBUG_MPI)
	      printf("\n"); */
	    
	    enter_luma_block(&LumaACLevel[i][0], this, x, y,QPy, 0);
	  }
	  
	  //printf("pb here 1 for Mb %d mode = %d\n",CurrMbAddr,intra_chroma_pred_mode);
	  // Decodes & organises the mbs in the frame With the previous infos about how to decode the mb
	  Intra_Chroma_Dispatch(this, mpi, intra_chroma_pred_mode, mb_pos_x >> 1, mb_pos_y >> 1, pps->constrained_intra_pred_flag);


	 /*  if (mpi->mpi_firstMb_in_slice == DEBUG_MPI){
	    int32_t aa, bb;
	    printf("Cb Pixels ");
	    for(bb=0;bb<4;bb++)
	      for(aa=0;aa<4;aa++){
		printf("%d ",Cb_pixel(this,(mb_pos_x>>1)+bb,(mb_pos_y>>1)+aa));
	      }
	    printf("\n");
	    printf("Cr Pixels ");
	    for(bb=0;bb<4;bb++)
	      for(aa=0;aa<4;aa++){
		printf("%d ",Cr_pixel(this,(mb_pos_x>>1)+bb,(mb_pos_y>>1)+aa));
	      }
	    printf("\n");
	  } */
	    
	}
	else if (mb.MbPartPredMode[0] == Intra_16x16) {
	  int32_t i, j;
	
	  /* if(mpi->mpi_firstMb_in_slice == DEBUG_MPI)
	    printf("test\n"); */


	  Intra_16x16_Dispatch(this, mpi, mb.Intra16x16PredMode,
			       mb_pos_x, mb_pos_y,
			       pps->constrained_intra_pred_flag);
	  // For debug purpose
	  int32_t aa,bb;
	  for(aa=0;aa<16;aa++)
	    for(bb=0;bb<16;bb++)
	      predict_mb[bb][aa]=L_pixel(this,mb_pos_x+bb,mb_pos_y+aa);
	  
	  transform_luma_dc(&LumaDCLevel[0], &LumaACLevel[0][0],QPy);
	  for (i = 0; i < 16; ++i) {
	    int32_t x = mb_pos_x + Intra4x4ScanOrder[i][0];
	    int32_t y = mb_pos_y + Intra4x4ScanOrder[i][1];
	    enter_luma_block(&LumaACLevel[i][0], this, x, y,QPy, 1);
	  }
	  
	  //printf("pb here 2 for Mb %d mode = %d\n",CurrMbAddr,intra_chroma_pred_mode);
	  Intra_Chroma_Dispatch(this, mpi,intra_chroma_pred_mode,
				mb_pos_x >> 1, mb_pos_y >> 1,pps->constrained_intra_pred_flag);
	  // act as if all transform blocks inside this macroblock were
	  // predicted using the Intra_4x4_DC prediction mode
	  // (without constrained_intra_pred, we'd have to do the same for
	  // inter MBs)

	  for (i = 0; i < 4; ++i)
	    for (j = 0; j < 4; ++j)
	      set_ModePredInfo_Intra4x4PredMode(mpi,(mb_pos_x >> 2) + j,
						(mb_pos_y >> 2) + i,2);
	}
	else {

	  int32_t i;
	  /*
	    {int x,y;for(y=0;y<4;++y){for(x=0;x<4;++x){i=((mb_pos_y>>2)+y)*mpi->TbPitch+(mb_pos_x>>2)+x;
	    printf("|%4d,%-4d",mpi->MVx[i],mpi->MVy[i]);}printf("\n");}}
	  */

	  MotionCompensateMB(this, ref, mpi, mb_pos_x, mb_pos_y);


	  for (i = 0; i < 16; ++i) {
	    int32_t x = mb_pos_x + Intra4x4ScanOrder[i][0];
	    int32_t y = mb_pos_y + Intra4x4ScanOrder[i][1];
	    enter_luma_block(&LumaACLevel[i][0], this, x, y,
			     QPy, 0);
	  }
	}		/*else
			  printf("unsupported prediction mode at %d,%d!\n",mb_pos_x,mb_pos_y); */

	if (mb.CodedBlockPatternChroma) {	///////////////// Chroma Residual
	  int32_t iCbCr, i;

	  for (iCbCr = 0; iCbCr < 2; ++iCbCr) {
	    transform_chroma_dc(&ChromaDCLevel[iCbCr][0], QPc);
	    for (i = 0; i < 4; ++i)
	      ChromaACLevel[iCbCr][i][0] = ChromaDCLevel[iCbCr][i];
	    for (i = 0; i < 4; ++i)
	      enter_chroma_block(&ChromaACLevel[iCbCr][i][0],
				 this, iCbCr, (mb_pos_x >> 1) + Intra4x4ScanOrder[i][0],
				 (mb_pos_y >> 1) + Intra4x4ScanOrder[i][1],
				 QPc, 1);
	  }
	}
      }

#if (defined(MB_DISPLAY) && !defined(CONFIG_ARCH_EMU))
      // Add MB printing here
      for (block_line = 0 ; block_line < luma_block_size ; block_line += 1) {
	offset = mb_pos_y * stream_width + mb_pos_x + block_line * stream_width;
	memcpy((void *) (SEG_BUFF_ADDR + offset), &(this->L[offset]), luma_block_size);
      }

      for (block_line = 0 ; block_line < chroma_block_size ; block_line += 1) {
	offset = (mb_pos_y>>1) * stream_width / 2 + (mb_pos_x>>1) + block_line * stream_width / 2;
	memcpy((void *) (SEG_BUFF_ADDR + offset + stream_width * stream_height), &(this->C[0][offset]), chroma_block_size);
	memcpy((void *) (SEG_BUFF_ADDR + offset + stream_width * stream_height + stream_width * stream_height / 4), &(this->C[1][offset]), chroma_block_size);
      }
#endif
    } ///////////// End of macroblock_layer() /////////////////////////////////

    moreDataFlag = more_rbsp_data_thread(nalu, mpi);

#if defined(PROGRESS)
    printk("\033[1A\n-- H.264 -- Frame %d decoding : %2d %% (%d/%d) slice %d\033[K",
	   sh->frame_num + 1, 100 * (CurrMbAddr-sh->first_mb_in_slice)/mpi->slice_MbSize,
	   CurrMbAddr - sh->first_mb_in_slice, mpi->slice_MbSize, sh->first_mb_in_slice/mpi->slice_MbSize);
#endif

    ++CurrMbAddr;
  } // End  while(moreDataFlag && CurrMbAddr<=MbCount)
  return;
}



void fb_display_frame(frame *this)
{
#if (!defined(MB_DISPLAY) && !defined(CONFIG_ARCH_EMU))
# if defined(USE_DMA)
    cpu_mem_write_32(DSX_SEGMENT_DMA_ADDR, endian_le32(&(this->L[0])));
    cpu_mem_write_32(DSX_SEGMENT_DMA_ADDR + 0x4, endian_le32(SEG_BUFF_ADDR));
    cpu_mem_write_32(DSX_SEGMENT_DMA_ADDR + 0x8, endian_le32(stream_width * stream_height));
    while(endian_le32(cpu_mem_read_32(DSX_SEGMENT_DMA_ADDR + 0x8)))
        ;

    cpu_mem_write_32(DSX_SEGMENT_DMA_ADDR, endian_le32(&(this->C[0][0])));
    cpu_mem_write_32(DSX_SEGMENT_DMA_ADDR + 0x4, endian_le32(SEG_BUFF_ADDR + stream_width * stream_height));
    cpu_mem_write_32(DSX_SEGMENT_DMA_ADDR + 0x8, endian_le32(stream_width * stream_height / 4));
    while(endian_le32(cpu_mem_read_32(DSX_SEGMENT_DMA_ADDR + 0x8)))
        ;

    cpu_mem_write_32(DSX_SEGMENT_DMA_ADDR, endian_le32(&(this->C[1][0])));
    cpu_mem_write_32(DSX_SEGMENT_DMA_ADDR + 0x4, endian_le32(SEG_BUFF_ADDR + stream_width * stream_height + stream_width * stream_height / 4));
    cpu_mem_write_32(DSX_SEGMENT_DMA_ADDR + 0x8, endian_le32(stream_width * stream_height / 4));
    while(endian_le32(cpu_mem_read_32(DSX_SEGMENT_DMA_ADDR + 0x8)))
        ;
# else
    //printk("1 = %d, 2 = %d\n",stream_width,stream_height);
    memcpy((void *) SEG_BUFF_ADDR, &(this->L[0]), stream_width * stream_height);
    memcpy((void *) (SEG_BUFF_ADDR + stream_width * stream_height), &(this->C[0][0]), stream_width * stream_height / 4);
    memcpy((void *) (SEG_BUFF_ADDR + stream_width * stream_height + stream_width * stream_height / 4), &(this->C[1][0]), stream_width * stream_height / 4);
# endif
#endif
}

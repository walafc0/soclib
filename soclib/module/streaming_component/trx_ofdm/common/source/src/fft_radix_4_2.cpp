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
/* ##########################################################################################################
**
** Function name        : FixedFftRadix4_2
** Language             : C Ansi
** Function prototype   : void FftRadix4_2 (COMPLEX *ptVectIn, COMPLEX *ptVectOut,FFT_CTRL *ptCtrl)
** Short description    : This function realizes the Fixed-point version (I)FFT radix-4(2) with decimation in frequency.
**                        This alogorithm also realizes a power normalization (output power = input power)
** In arguments	        : COMPLEX *ptVectIn  : input complex data flow
** Out arguments        : COMPLEX *ptVectOut : output complex data flow
** Control arguments    : The control information is encapsulated in a FFT_CTRL structure :
**                           - FftSize			:   size of the FFT
**                           - FftType			:   ='1': FFT, ="0" : IFFT
** History              : 08/12/03 creation by Dimitri KTENAS (CEA) : Fixed point Radix4 + floating point Radix 2
**                        28/04/04 modification by Dimitri KTENAS (CEA) : Fixed Point Radix 2 is taking into account
** Functions  called    : None
** Detailed description : If the size of the FFT is not a power of 4, the first stages are radix-4
**                        and the last stage is radix-2. The size of the FFT must be a power of 2.
**                        FFT radix decimation in frequency (Sande-Tukey algorithm) ->chap.2 la transformation
**                        de Fourier discrete, dans "Traitement numerique du signal", Maurice Bellanger, Dunod, 1998
**                        W = exp(-2.PI.i/N) if FFT and W=exp(+2.PI.i/N) if IFFT. we've got the relation W^N=1
**                        where N is the size of the FFT. log is the natural logarithm ln 
**                        The basic butterfly for FFT radix-4 is the following :
**                        X0	x0	+	x1	+	x2	+	x3
**                        X1	x0	-	ix1	-	x2	+	ix3
**                        X2    x0	-	x1	+	x2	-	x3
**                        X3	x0	+	ix1	-	x2	-	ix3
**                        
**                        The basic butterfly for FFT radix-2 is the following :
**                        X0	x0	+	x1
**                        X1	x0	-	x1
**                         
**                        According to theoretical formulas, in IFFT, you've got to divide the results by N
**                        whereas it's not the case. Nevertheless as we consider a version with power normalization,
**                        we only have to divide by sqrt(N) either for FFT and IFFT.
**
** ####################################################################################################### */

/* ------------------------------ */
/* Include the module header file */
/* ------------------------------ */

#include "fft_radix_4_2.h"

 
/* ----------------------------------------- */
/* Static function prototypes (if necessary) */
/* ----------------------------------------- */

static void Int2Bin (int Number, int *ptStateVector, int NbBits);

static void Bin2Int (int *ptStateVector, int *ptNumber, int NbBits);


/* --------------------------------------------- */
/* Static storage class variables (if necessary) */
/* --------------------------------------------- */

/* N/A */


/* ***************** */
/* C called function */
/* ***************** */

void FixedFftRadix_4_2 (FixC *ptVectIn, FixC *ptVectOut,FFT_RADIX_4_CTRL *ptCtrl)
{
    #define N 18  // 18 (16+2) ou 14 (12+2) ou 16 (14+2)
    #define M 9   // 9 (7+2) ou 7 (5+2) 
    #define P 16
    #define Q 0

	int		FftSize;						/*	size of the Fft	                                                          */
	int		CpxCnt;							/*	counter over the complex numbers	                                      */
	int		CpxCnt2;						/*	counter over the complex numbers	                                      */
	int		NbStages;						/*	number of stages for the FFT, ie number of iteration	                  */
	int		StageCnt;						/*	counter over the number of stages 	                                      */
	int		NbButterflies;					/*	number of butterflies	                                                  */
	int		ButterflyCnt;					/*	counter over the butterflies	                                          */
	int		Length;							/*	length between the 2 nearest branches of the elementary butterfly	      */
	int		m;								/*	start value for each butterfly	                                          */
	int		FftIndex;						/*	power of W in processing of butterflies                                   */ 
	int		BitCnt;							/*  counter over the bits                                                     */ 
	int		StateVector1[MAX_BIT_REVERSE];	/*	for digit reverse process	                                              */
	int		StateVector2[MAX_BIT_REVERSE];	/*	for digit reverse process	                                              */ 
	int		NbBits;							/*	number of bits for the digit reverse process	                          */
	int		Incr;							/*	value for incrementation step of FftIndex	                              */
	int		FlagRadix2;						/*	if ="1", it indicates that last stage is radix-2 processing	              */
	float	ReW, ImW;						/*	real and imaginary part of W, where W^N=1, where N is the size of the FFT */
	float	ReFftCoeff[MAX_SIZE_FFT];		/*	real part of the FFT coefficients W^n where n ranges from 0 to N-1	      */
	float   ImFftCoeff[MAX_SIZE_FFT];		/*	imaginary part of the FFT coefficients	                                  */
    
    static FixC    FixedReFftCoeff[MAX_SIZE_FFT]; /* real part of W, where W^N=1, where N is the size of the FFT      */
    static FixC    FixedImFftCoeff[MAX_SIZE_FFT]; /* imaginary part of W, where W^N=1, where N is the size of the FFT */
    static FixC    FixedReBuffer[MAX_SIZE_FFT];   /* buffer for real part of processing values                        */
    static FixC    FixedImBuffer[MAX_SIZE_FFT];   /* imaginary part of processing values                              */           
    static FixC    FixedReTmp0(N,M), FixedReTmp0_1(N - 1,M - 1),FixedReTmp0_2(N - 1,M - 1); /* temporary value 0 in butterfly, for real part      */
    static FixC    FixedImTmp0(N,M), FixedImTmp0_1(N - 1,M - 1),FixedImTmp0_2(N - 1,M - 1); /* temporary value 0 in butterfly, for imaginary part */
    static FixC    FixedReTmp1(N,M), FixedReTmp1_1(N - 1,M - 1),FixedReTmp1_2(N - 1,M - 1); /* temporary value 1 in butterfly, for real part      */
    static FixC    FixedImTmp1(N,M), FixedImTmp1_1(N - 1,M - 1),FixedImTmp1_2(N - 1,M - 1); /* temporary value 1 in butterfly, for imaginary part */
    static FixC    FixedReTmp2(N,M), FixedReTmp2_1(N - 1,M - 1),FixedReTmp2_2(N - 1,M - 1); /* temporary value 2 in butterfly, for real part      */
    static FixC    FixedImTmp2(N,M), FixedImTmp2_1(N - 1,M - 1),FixedImTmp2_2(N - 1,M - 1); /* temporary value 2 in butterfly, for imaginary part */
    static FixC    FixedReTmp3(N,M), FixedReTmp3_1(N - 1,M - 1),FixedReTmp3_2(N - 1,M - 1); /* temporary value 3 in butterfly, for real part      */
    static FixC    FixedImTmp3(N,M), FixedImTmp3_1(N - 1,M - 1),FixedImTmp3_2(N - 1,M - 1); /* temporary value 3 in butterfly, for imaginary part */

    static FixC    FixedReTmpBuffer(N+11+1,M+1),  FixedReTmpBuffer_1(N+11,M), FixedReTmpBuffer_2(N+11,M); 
    static FixC    FixedImTmpBuffer(N+11+1,M+1),  FixedImTmpBuffer_1(N+11,M), FixedImTmpBuffer_2(N+11,M); 

    static FixC    FixedConst(P,Q); // value coding 1/sqrt(2) = 0.707107.. : for radix-2 purpose
    static FixC    FixedReValue1(N - 1 + P,M - 1 + Q),  FixedImValue1(N - 1 + P,M - 1 + Q); /* for radix-2 computation */
    static FixC    FixedReValue2(N - 1 + P,M - 1 + Q),  FixedImValue2(N - 1 + P,M - 1 + Q); /* for radix-2 computation */
    
    InitFixC1d(FixedReFftCoeff, MAX_SIZE_FFT, "FixedReFftCoeff", 11, 0, "sr");
    InitFixC1d(FixedImFftCoeff, MAX_SIZE_FFT, "FixedImFftCoeff", 11, 0, "sr");

    InitFixC1d(FixedReBuffer, MAX_SIZE_FFT, "FixedReBuffer", N-2, M-2, "sr");
    InitFixC1d(FixedImBuffer, MAX_SIZE_FFT, "FixedReBuffer", N-2, M-2, "sr");

    // initialization of parameters

	FftSize = ptCtrl->FftSize;

	FixedConst = sqrt(0.5);

	// First, we check if the size of the FFT is a power of 2
	int value = 1;
	while (value < FftSize) {
		value = value << 1;
	}
	if (value != FftSize )
	{
		printf ( "FUNCTION fft : size of the fft is not a power of 2\n" );
		exit(-1); 
	}


	// Then, we compute the total number of stages (radix-4 + radix-2)

	NbStages = (int) ceil (log((double)FftSize) / log((double)4));	

	// We see if FftSize is a power of 4 or a power of 2
	
	if ( (int) (pow ((double)4, NbStages)) != FftSize )	// FFT size is not a power of 4 
    {
		FlagRadix2 = 1;
    }
	else										// FFT size is a power of 4
	{
		FlagRadix2 = 0;
	}

	// we compute the number of bits for the base-4 Digit reverse operation (Radix4)

	NbBits = (int) floor ((log((double)FftSize) / log((double)2)) + 0.5);

	//////////////////////////////////
	// copy of input complex vector //
	//////////////////////////////////

	for ( CpxCnt = 0 ; CpxCnt < FftSize ; CpxCnt++ )
	{
		FixedReBuffer[CpxCnt] = ptVectIn[2 * CpxCnt];
		FixedImBuffer[CpxCnt] = ptVectIn[2 * CpxCnt + 1];
		//cout << "CpxCnt = " << CpxCnt << " : ptVectIn[2 * CpxCnt] = " <<  (float)ptVectIn[2 * CpxCnt] << " and FixedReBuffer[CpxCnt] = " << (float)FixedReBuffer[CpxCnt] << endl;
	}

	//////////////////////////////////////////////////////
	//	recursive computation of the FFT coefficients	//
	//	W^n = W^(n-1) * W								//
	//////////////////////////////////////////////////////
	
	ReW = (float) cos (2 * 3.14159265358979 / FftSize);

	if ( ptCtrl->FftType == 1 )	// FFT
	{
	  //cout << "fftType is fft" << endl;
	  ImW = - (float) sin (2 * 3.14159265358979 / FftSize);
	}
	else						// IFFT
	{
	  //cout << "fftType is ifft" << endl;
	  ImW = (float) sin (2 * 3.14159265358979 / FftSize);
	}

	ReFftCoeff[0] = (float) 1;
	//ReFftCoeff[0] = (float) 0.999023; // WARNING !!!!!!! Fab' modification !
	ImFftCoeff[0] = (float) 0;

	for ( CpxCnt = 1 ; CpxCnt < FftSize / 2 ; CpxCnt++ )
	{
		ReFftCoeff[CpxCnt] = ReFftCoeff[CpxCnt - 1] * ReW - ImFftCoeff[CpxCnt - 1] * ImW;
		ImFftCoeff[CpxCnt] = ReFftCoeff[CpxCnt - 1] * ImW + ImFftCoeff[CpxCnt - 1] * ReW;
	}

	 
	ReFftCoeff[FftSize / 2] = (float) -1; 
	//ReFftCoeff[FftSize / 2] = (float) -0.999023; // WARNING !!!!!!! Fab' modification !
	ImFftCoeff[FftSize / 2] = (float) 0;

	for ( CpxCnt = (FftSize / 2) + 1 ; CpxCnt < FftSize ; CpxCnt++ )
	{
		ReFftCoeff[CpxCnt] = ReFftCoeff[CpxCnt - 1] * ReW - ImFftCoeff[CpxCnt - 1] * ImW;
		ImFftCoeff[CpxCnt] = ReFftCoeff[CpxCnt - 1] * ImW + ImFftCoeff[CpxCnt - 1] * ReW;
	}

	// computation of fixed Fft coefficient values
 	for ( CpxCnt = 0 ; CpxCnt < FftSize ; CpxCnt++ ) // FixC bug correction ! FC
	  {
	    if (ReFftCoeff[CpxCnt] > 0.999023)
	      ReFftCoeff[CpxCnt] = 0.999023;
	    if (ReFftCoeff[CpxCnt] < -0.999023)
	      ReFftCoeff[CpxCnt] = -0.999023;
	    if (ImFftCoeff[CpxCnt] > 0.999023)
	      ImFftCoeff[CpxCnt] = 0.999023;
	    if (ImFftCoeff[CpxCnt] < -0.999023)
	      ImFftCoeff[CpxCnt] = -0.999023;
	  }
	string bin_file;
	bin_file = PATH_TRACE_FILE;
	bin_file +="coeff_sysc.bin";
	//std::ofstream file_res("./appli/trace/coeff_sysc.bin");
	std::ofstream file_res(bin_file.c_str());
	if (!file_res) {
	  cout << "ERROR : the output file coeff_sysc.bin cannot be open : " << endl;
	  exit(0);
	}
	int coeff_int;

	for ( CpxCnt = 0 ; CpxCnt < FftSize ; CpxCnt++ )
	  {
	    FixedReFftCoeff[CpxCnt] = ReFftCoeff[CpxCnt];
	    FixedImFftCoeff[CpxCnt] = ImFftCoeff[CpxCnt];
	    // cout << "coeff " << CpxCnt << " : ( " << (float)FixedReFftCoeff[CpxCnt] << " , " << (float)FixedImFftCoeff[CpxCnt] << " )" << endl;

	    // coeff are put in a file
	    file_res << "when \"";
	    //file_res.width(8);
	    //file_res.fill('0');	
	    //file_res << hex << CpxCnt;
	    int tmp=128;
	    for (int i=0; i<8; i++) {
	      if ((CpxCnt&tmp) == tmp)
		file_res << "1";
	      else
		file_res << "0";
	      tmp = tmp >> 1;
	    }
	    file_res << "\" => re_data_i(10 downto 0) <= \"";
	    //file_res.width(11);
	    //file_res.fill('0');
	    coeff_int = (int)((double)FixedReFftCoeff[CpxCnt] * pow((double)2,10));
	    if (coeff_int<0) coeff_int = -coeff_int;
	    tmp=1024;
	    for (int i=0; i<11; i++) {
	      if (((coeff_int&0x7ff)&tmp) == tmp)
		file_res << "1";
	      else
		file_res << "0";
	      tmp = tmp >> 1;
	    }
	    //file_res << hex << coeff_int;
	    file_res << "\"; im_data_i(10 downto 0) <= \"";
	    //file_res.width(11);
	    //file_res.fill('0');
	    coeff_int = (int)((double)FixedImFftCoeff[CpxCnt] * pow((double)2,10));
	    if (coeff_int>0) coeff_int = -coeff_int;
	    tmp=1024;
	    for (int i=0; i<11; i++) {
	      if (((coeff_int&0x7ff)&tmp) == tmp)
		file_res << "1";
	      else
		file_res << "0";
	      tmp = tmp >> 1;
	    }
	    //file_res << hex << coeff_int;
	    file_res << "\";" << endl;

	    //FixedReFftCoeff[CpxCnt].Display();	  
	    //FixedImFftCoeff[CpxCnt].Display();	  
	  }

	file_res.close();
	
	////////////////////////////////////////////////////
	// Loop over the number of stages (ie iterations) //
	////////////////////////////////////////////////////

	NbButterflies = FftSize / 4;

	Length = FftSize / 4;

	Incr = 1;

	// We compute the number of stages.
	// if N is a power of 4, all stages are radix-4
	// if N is a power of 4, all stages are radix-4 except the last which is radix2
	
	if ( FlagRadix2 == 1 )	
	{
		NbStages = NbStages - 1;
	}	

	for ( StageCnt = 0 ; StageCnt < NbStages ; StageCnt++ )
	{	
		m = 0;

		FftIndex = 0;

		for ( ButterflyCnt = 0 ; ButterflyCnt < NbButterflies ; ButterflyCnt++ )
		{			
			// Buffer[m + k * N/4], Length = N/4;
			// for a butterfly m is fixed and k ranges from 0 to N/4 - 1 (included).
			// then we go to the next butterfly (m = counter over the butterflies)
			
			// Multiplication by the FFT matrix of rank 4, IFFT=(FFT)^-1=conj(FFT)
			//	1	1	1	1 
			//	1	-i	-1	i
			//	1	-1	1	-1
			//	1	i	-1	-i
			// IFFT matrix is the same except we swap Tmp1 and Tmp3 (2nd row and 4th row)
             
		 	 FixedReTmp0_1 = FixedReBuffer[m]              + FixedReBuffer[m + Length];

			 //cout << "m = " << m << " : FixedReBuffer[m] = " << (float)FixedReBuffer[m] << " and FixedReBuffer[m + Length] = " << (float)FixedReBuffer[m + Length] << endl;

             FixedReTmp0_2 = FixedReBuffer[m + 2 * Length] + FixedReBuffer[m + 3 * Length];
             FixedReTmp0   = FixedReTmp0_1                 + FixedReTmp0_2;

			 FixedImTmp0_1 = FixedImBuffer[m]              + FixedImBuffer[m + Length];
             FixedImTmp0_2 = FixedImBuffer[m + 2 * Length] + FixedImBuffer[m + 3 * Length];
             FixedImTmp0   = FixedImTmp0_1                 + FixedImTmp0_2;

			
			 FixedReTmp1_1 = FixedReBuffer[m]              + FixedImBuffer[m + Length];
             FixedReTmp1_2 = FixedReBuffer[m + 2 * Length] + FixedImBuffer[m + 3 * Length];
             FixedReTmp1   = FixedReTmp1_1                 - FixedReTmp1_2;

             FixedImTmp1_1 = FixedImBuffer[m]              - FixedReBuffer[m + Length];
			 FixedImTmp1_2 = FixedReBuffer[m + 3 * Length] - FixedImBuffer[m + 2 * Length];
             FixedImTmp1   = FixedImTmp1_1                 + FixedImTmp1_2;


			 FixedReTmp2_1 = FixedReBuffer[m]              - FixedReBuffer[m + Length]; 
             FixedReTmp2_2 = FixedReBuffer[m + 2 * Length] - FixedReBuffer[m + 3 * Length];
             FixedReTmp2   = FixedReTmp2_1                 + FixedReTmp2_2;

			 FixedImTmp2_1 = FixedImBuffer[m]              - FixedImBuffer[m + Length];
             FixedImTmp2_2 = FixedImBuffer[m + 2 * Length] - FixedImBuffer[m + 3 * Length];
             FixedImTmp2   = FixedImTmp2_1                 + FixedImTmp2_2;

			
			 FixedReTmp3_1 = FixedReBuffer[m]              - FixedImBuffer[m + Length];
             FixedReTmp3_2 = FixedImBuffer[m + 3 * Length] - FixedReBuffer[m + 2 * Length];
             FixedReTmp3   = FixedReTmp3_1                 + FixedReTmp3_2;

			 FixedImTmp3_1 = FixedImBuffer[m]              + FixedReBuffer[m + Length];
             FixedImTmp3_2 = FixedImBuffer[m + 2 * Length] + FixedReBuffer[m + 3 * Length];
             FixedImTmp3   = FixedImTmp3_1                 - FixedImTmp3_2;             

		
			// Multiplication by W^0

            if ( ptCtrl->NormalizationPower == 1 ) // power normalization
            {
              FixedReBuffer[m] = FixedReTmp0 >> 1;  // * sqrt(0.25)
              FixedImBuffer[m] = FixedImTmp0 >> 1;
            }
            else // no power normalization
            {
              if ( ptCtrl->FftType == 0 )  // IFFT
              {
                FixedReBuffer[m] = FixedReTmp0;
                FixedImBuffer[m] = FixedImTmp0;
              }
              else // FFT 
              {
                FixedReBuffer[m] = FixedReTmp0 >> 2; // division by 4 : >> 2     
                FixedImBuffer[m] = FixedImTmp0 >> 2; // division by 4 : >> 2
              }
            }
		
			// Multiplication by W^k
			
			if ( ptCtrl->FftType == 0 )	// IFFT
			{
                // FixedReBuffer[m + Length] = (float) sqrt(0.25) * (FixedReTmp3 * FixedReFftCoeff[FftIndex % FftSize] - FixedImTmp3 * FixedImFftCoeff[FftIndex % FftSize]);
                FixedReTmpBuffer_1        = FixedReTmp3        * FixedReFftCoeff[FftIndex % FftSize];
                FixedReTmpBuffer_2        = FixedImTmp3        * FixedImFftCoeff[FftIndex % FftSize];
                FixedReTmpBuffer          = FixedReTmpBuffer_1 - FixedReTmpBuffer_2;

                if ( ptCtrl->NormalizationPower == 1 )
                {
                  FixedReBuffer[m + Length] = FixedReTmpBuffer   >> 1;
                }
                else
                {
                  FixedReBuffer[m + Length] = FixedReTmpBuffer; 
                }
				// FixedImBuffer[m + Length] = (float) sqrt(0.25) * (FixedReTmp3 * FixedImFftCoeff[FftIndex % FftSize] + FixedImTmp3 * FixedReFftCoeff[FftIndex % FftSize]);
                FixedImTmpBuffer_1         = FixedReTmp3        * FixedImFftCoeff[FftIndex % FftSize];
                FixedImTmpBuffer_2         = FixedImTmp3        * FixedReFftCoeff[FftIndex % FftSize];
                FixedImTmpBuffer           = FixedImTmpBuffer_1 + FixedImTmpBuffer_2; 
                if ( ptCtrl->NormalizationPower == 1 )
                {
                  FixedImBuffer[m + Length]  = FixedImTmpBuffer   >> 1;
                }
                else
                {
                  FixedImBuffer[m + Length] = FixedImTmpBuffer; 
                }			}
			else						// FFT
			{
                // FixedReBuffer[m + Length] = sqrt(0.25) * (FixedReTmp1 * FixedReFftCoeff[FftIndex % FftSize] - FixedImTmp1 * FixedImFftCoeff[FftIndex % FftSize]);
                FixedReTmpBuffer_1        = FixedReTmp1        * FixedReFftCoeff[FftIndex % FftSize];
                FixedReTmpBuffer_2        = FixedImTmp1        * FixedImFftCoeff[FftIndex % FftSize];
                FixedReTmpBuffer          = FixedReTmpBuffer_1 - FixedReTmpBuffer_2;
                if ( ptCtrl->NormalizationPower == 1 )
                {
                  FixedReBuffer[m + Length] = FixedReTmpBuffer >> 1;
                }
                else
                {
                  FixedReBuffer[m + Length] = FixedReTmpBuffer >> 2;   // >> 2
                }             
                // FixedImBuffer[m + Length] = sqrt(0.25) * (FixedReTmp1 * FixedImFftCoeff[FftIndex % FftSize] + FixedImTmp1 * FixedReFftCoeff[FftIndex % FftSize]);									
                FixedImTmpBuffer_1         = FixedReTmp1        * FixedImFftCoeff[FftIndex % FftSize];
                FixedImTmpBuffer_2         = FixedImTmp1        * FixedReFftCoeff[FftIndex % FftSize];
                FixedImTmpBuffer           = FixedImTmpBuffer_1 + FixedImTmpBuffer_2; 
                if ( ptCtrl->NormalizationPower == 1 )
                {
                  FixedImBuffer[m + Length]  = FixedImTmpBuffer   >> 1;
                }
                else
                {
                  FixedImBuffer[m + Length]  = FixedImTmpBuffer   >> 2;  // >> 2
                }
			}

            if ( ptCtrl->OverflowTest == 1 )
            {
              if ( (FixedReBuffer[m + Length].GetMantissa() == (int) pow((double)2, N-2-1) - 1) || (FixedReBuffer[m + Length].GetMantissa() == -(int) pow((double)2, N-2-1)) )
              {
                cout << "Overflow at the output of the stage " <<  FixedReBuffer[m + Length].GetMantissa() << endl;
              }

              if ( (FixedImBuffer[m + Length].GetMantissa() == (int) pow((double)2, N-2-1) - 1) || (FixedImBuffer[m + Length].GetMantissa() == -(int) pow((double)2, N-2-1)) )
              {
                cout << "Overflow at the output of the stage " <<  FixedImBuffer[m + Length].GetMantissa() << endl;
              }
            }
			
			// Multiplication by W^(2k)

			// FixedReBuffer[m + 2 * Length] = (float) sqrt(0.25) * (FixedReTmp2 * FixedReFftCoeff[(2 * FftIndex) % FftSize] - FixedImTmp2 * FixedImFftCoeff[(2 * FftIndex) % FftSize]);
			FixedReTmpBuffer_1            = FixedReTmp2         * FixedReFftCoeff[(2 * FftIndex) % FftSize];
            FixedReTmpBuffer_2            = FixedImTmp2         * FixedImFftCoeff[(2 * FftIndex) % FftSize];
            FixedReTmpBuffer              = FixedReTmpBuffer_1 - FixedReTmpBuffer_2;
            if ( ptCtrl->NormalizationPower == 1 )
            {
              FixedReBuffer[m + 2 * Length] = FixedReTmpBuffer    >> 1;
            }
            else
            {
              if ( ptCtrl->FftType == 0 ) // IFFT case
              {
                FixedReBuffer[m + 2 * Length] = FixedReTmpBuffer; 
              }
              else // FFT case
              {
                FixedReBuffer[m + 2 * Length] = FixedReTmpBuffer >> 2; // >> 2
              }
            }

            // FixedImBuffer[m + 2 * Length] = (float) sqrt(0.25) * (FixedReTmp2 * FixedImFftCoeff[(2 * FftIndex) % FftSize] + FixedImTmp2 * FixedReFftCoeff[(2 * FftIndex) % FftSize]);			
            FixedImTmpBuffer_1            = FixedReTmp2        * FixedImFftCoeff[(2 * FftIndex) % FftSize];
            FixedImTmpBuffer_2            = FixedImTmp2        * FixedReFftCoeff[(2 * FftIndex) % FftSize];
            FixedImTmpBuffer              = FixedImTmpBuffer_1 + FixedImTmpBuffer_2;
            if ( ptCtrl->NormalizationPower == 1 )
            {
              FixedImBuffer[m + 2 * Length] = FixedImTmpBuffer   >> 1;
            }
            else
            {
              if ( ptCtrl->FftType == 0 ) // IFFT case
              {
                FixedImBuffer[m + 2 * Length] = FixedImTmpBuffer; 
              }
              else // FFT case
              {
                FixedImBuffer[m + 2 * Length] = FixedImTmpBuffer >> 2; // >> 2
              }
            }	

            if ( ptCtrl->OverflowTest == 1 )
            {
              if ( (FixedReBuffer[m + 2 * Length].GetMantissa() == (int) pow((double)2, N-2-1) - 1) || (FixedReBuffer[m + 2 * Length].GetMantissa() == -(int) pow((double)2, N-2-1)) )
              {
                cout << "Overflow at the output of the stage " <<  FixedReBuffer[m + 2 * Length].GetMantissa() << endl;
              }

              if ( (FixedImBuffer[m + 2 * Length].GetMantissa() == (int) pow((double)2, N-2-1) - 1) || (FixedImBuffer[m + 2 * Length].GetMantissa() == -(int) pow((double)2, N-2-1)) )
              {
                cout << "Overflow at the output of the stage " <<  FixedImBuffer[m + 2 * Length].GetMantissa() << endl;
              }
            }
		
			// Multiplication by W^(3k)
			
			if ( ptCtrl->FftType == 0 )	// IFFT
			{
                // FixedReBuffer[m + 3 * Length] = (float) sqrt(0.25) * (FixedReTmp1 * FixedReFftCoeff[(3 * FftIndex) % FftSize] - FixedImTmp1 * FixedImFftCoeff[(3 * FftIndex) % FftSize]);
                FixedReTmpBuffer_1            = FixedReTmp1        * FixedReFftCoeff[(3 * FftIndex) % FftSize];
                FixedReTmpBuffer_2            = FixedImTmp1        * FixedImFftCoeff[(3 * FftIndex) % FftSize];
                FixedReTmpBuffer              = FixedReTmpBuffer_1 - FixedReTmpBuffer_2; 

                if ( ptCtrl->NormalizationPower == 1 )
                {
                  FixedReBuffer[m + 3 * Length] = FixedReTmpBuffer   >> 1;
                }
                else
                {
                  FixedReBuffer[m + 3 * Length] = FixedReTmpBuffer; 
                }

				// FixedImBuffer[m + 3 * Length] = (float) sqrt(0.25) * (FixedReTmp1 * FixedImFftCoeff[(3 * FftIndex) % FftSize] + FixedImTmp1 * FixedReFftCoeff[(3 * FftIndex) % FftSize]);
                FixedImTmpBuffer_1            = FixedReTmp1        * FixedImFftCoeff[(3 * FftIndex) % FftSize];
                FixedImTmpBuffer_2            = FixedImTmp1        * FixedReFftCoeff[(3 * FftIndex) % FftSize];
                FixedImTmpBuffer              = FixedImTmpBuffer_1 + FixedImTmpBuffer_2;

                if ( ptCtrl->NormalizationPower == 1 )
                {
                  FixedImBuffer[m + 3 * Length] = FixedImTmpBuffer   >> 1;
                }
                else
                {
                  FixedImBuffer[m + 3 * Length] = FixedImTmpBuffer;
                }

			}
			else						// FFT
			{
                // FixedReBuffer[m + 3 * Length] = sqrt(0.25)* (FixedReTmp3 * FixedReFftCoeff[(3 * FftIndex) % FftSize] - FixedImTmp3 * FixedImFftCoeff[(3 * FftIndex) % FftSize]);
                FixedReTmpBuffer_1            = FixedReTmp3        * FixedReFftCoeff[(3 * FftIndex) % FftSize];
                FixedReTmpBuffer_2            = FixedImTmp3        * FixedImFftCoeff[(3 * FftIndex) % FftSize];
                FixedReTmpBuffer              = FixedReTmpBuffer_1 - FixedReTmpBuffer_2; 
                if ( ptCtrl->NormalizationPower == 1 )
                {
                  FixedReBuffer[m + 3 * Length] = FixedReTmpBuffer   >> 1;
                }
                else
                {
                  FixedReBuffer[m + 3 * Length] = FixedReTmpBuffer   >> 2; // >> 2
                }

		// FixedImBuffer[m + 3 * Length] = sqrt(0.25)* (FixedReTmp3 * FixedImFftCoeff[(3 * FftIndex) % FftSize] + FixedImTmp3 * FixedReFftCoeff[(3 * FftIndex) % FftSize]);	
								
                FixedImTmpBuffer_1            = FixedReTmp3        * FixedImFftCoeff[(3 * FftIndex) % FftSize];
                FixedImTmpBuffer_2            = FixedImTmp3        * FixedReFftCoeff[(3 * FftIndex) % FftSize];
                FixedImTmpBuffer              = FixedImTmpBuffer_1 + FixedImTmpBuffer_2;
                if ( ptCtrl->NormalizationPower == 1 )
                {
                  FixedImBuffer[m + 3 * Length] = FixedImTmpBuffer   >> 1;
                }
                else
                {
                  FixedImBuffer[m + 3 * Length] = FixedImTmpBuffer   >> 2; // >> 2
                }
			}          

            if ( ptCtrl->OverflowTest == 1 )
            {
              if ( (FixedReBuffer[m + 3 * Length].GetMantissa() == (int) pow((double)2, N-2-1) - 1) || (FixedReBuffer[m + 3 * Length].GetMantissa() == -(int) pow((double)2, N-2-1)) )
              {
                cout << "Overflow at the output of the stage " <<  FixedReBuffer[m + 3 * Length].GetMantissa() << endl;
              }

              if ( (FixedImBuffer[m + 3 * Length].GetMantissa() == (int) pow((double)2, N-2-1) - 1) || (FixedImBuffer[m + 3 * Length].GetMantissa() == -(int) pow((double)2, N-2-1)) )
              {
                cout << "Overflow at the output of the stage " <<  FixedImBuffer[m + 3 * Length].GetMantissa() << endl;
              }
            }

			// computation of the index m of the next butterfly
			// and
			// computation of the new FFtIndex for the next butterfly
			
			if ( ( (ButterflyCnt + 1) % Length ) != 0 )
			{
				m++;
				FftIndex	= FftIndex + Incr;
			}
			else	// ButterflyCnt = length - 1
			{
				m			= 4 * (ButterflyCnt + 1);
				FftIndex	= 0;
			}			
			
		}	// end of loop on NbButterflies
		
		Length	= Length / 4;

		Incr	*= 4;
		
	}	// end of loop on NbStages


	// if N is not a power of 4, the last stage is a radix-2 operation

	if ( FlagRadix2 == 1 )	
	{
	  // Multiplication by the FFT matrix of rank 2, IFFT=(FFT)^-1=(FFT)
	  //	1	1
	  //	1	-1
	  
	  for ( m = 0 ; m < FftSize ; m+=2 )
	    {
	      FixedReTmp0_1 = FixedReBuffer[m];
	      FixedImTmp0_1 = FixedImBuffer[m];
	      
	      FixedReTmp1_1    = FixedReBuffer[m] + FixedReBuffer[m + 1];
	      if ( ptCtrl->NormalizationPower == 1 )
		{
		  FixedReValue1    = FixedConst * FixedReTmp1_1;
		  FixedReBuffer[m] = FixedReValue1;
		}
	      else
		{
		  if ( ptCtrl->FftType == 0 ) // IFFT case
		    {
		      FixedReBuffer[m] = FixedReTmp1_1;
		    }
		  else // FFT case
		    {
		      FixedReBuffer[m] = FixedReTmp1_1 >> 1;
		    }
		}	     
	      //
	      FixedImTmp1_1    = FixedImBuffer[m] + FixedImBuffer[m + 1];
	      if ( ptCtrl->NormalizationPower == 1 )
		{
		  FixedImValue1    = FixedConst * FixedImTmp1_1;
		  FixedImBuffer[m] = FixedImValue1;
		}
	      else
		{
		  if ( ptCtrl->FftType == 0 ) // IFFT case
		    {
		      FixedImBuffer[m] = FixedImTmp1_1;
		    }
		  else
		    {
		      FixedImBuffer[m] = FixedImTmp1_1 >> 1;
		    }              
		}

	      FixedReTmp2_1        = FixedReTmp0_1 - FixedReBuffer[m + 1];
	      if ( ptCtrl->NormalizationPower == 1 )
		{
		  FixedReValue2        = FixedConst * FixedReTmp2_1;
		  FixedReBuffer[m + 1] = FixedReValue2;
		}
	      else
		{
		  if ( ptCtrl->FftType == 0 ) // IFFT case
		    {
		      FixedReBuffer[m + 1] =  FixedReTmp2_1;
		    }
		  else  // FFT case
		    {
		      FixedReBuffer[m + 1] =  FixedReTmp2_1 >> 1;
		    }
		}
	      
	      FixedImTmp2_1        = FixedImTmp0_1 - FixedImBuffer[m + 1];
	      if ( ptCtrl->NormalizationPower == 1 )
		{
		  FixedImValue2        = FixedConst * FixedImTmp2_1; 
		  FixedImBuffer[m + 1] = FixedImValue2; 
		}
	      else
		{
		  if ( ptCtrl->FftType == 0 ) // IFFT case
		    {
		      FixedImBuffer[m + 1] = FixedImTmp2_1;
		    }
		  else // FFT case
		    {
		      FixedImBuffer[m + 1] = FixedImTmp2_1 >> 1;
		    }
		} 
	    }
	}

	///////////////////////////////////////////////////////////////////
	// rearrangement order for the output vector					 //
	// base 4 digit-reversal for radix 4							 //
	///////////////////////////////////////////////////////////////////	

	if  ( FlagRadix2 == 0 )	// base 4 digit-reversal
	{
		for ( CpxCnt = 0 ; CpxCnt < FftSize ; CpxCnt++ )
		{
			// we transform the base-10 number CpxCnt in binary representation on NbBits
			
			Int2Bin (CpxCnt, StateVector1, NbBits);
			
			// base-4 digit reverse
			
			for ( BitCnt = 0 ; BitCnt < NbBits / 2 ; BitCnt++ )
			{
				StateVector2[2 * BitCnt]	 = StateVector1[NbBits - 1 - (2 * BitCnt + 1)]; 			
				StateVector2[2 * BitCnt + 1] = StateVector1[NbBits - 1 - 2 * BitCnt];			
			}
			
			// transformation of binary StaVector2 in base-10 number CpxCnt2
			
			Bin2Int (StateVector2, &CpxCnt2, NbBits);
			
			// affectation of the output with the new order
			
			ptVectOut[2 * CpxCnt2]     = FixedReBuffer[CpxCnt];
			ptVectOut[2 * CpxCnt2 + 1] = FixedImBuffer[CpxCnt];            
		}
	}
	else					// modified base 4 digit-reversal for "radix 2"
	{
		for ( CpxCnt = 0 ; CpxCnt < FftSize ; CpxCnt++ )
		{
			// we transform the base-10 number CpxCnt in binary representation on NbBits
			
			Int2Bin (CpxCnt, StateVector1, NbBits);
			
			// base-4 digit reverse

			for ( BitCnt = 0 ; BitCnt < (int) floor ((NbBits / 2) + 0.5) ; BitCnt++ )
			{
				StateVector2[2 * BitCnt]	 = StateVector1[NbBits - 1 - (2 * BitCnt + 1)];
				StateVector2[2 * BitCnt + 1] = StateVector1[NbBits - 1 - (2 * BitCnt)];
			}

			StateVector2[NbBits - 1] = StateVector1[0];

			// transformation of binary StaVector2 in base-10 number CpxCnt2
			
			Bin2Int (StateVector2, &CpxCnt2, NbBits);
			
			// affectation of the output with the new order

			ptVectOut[2 * CpxCnt]     = FixedReBuffer[CpxCnt2];
			ptVectOut[2 * CpxCnt + 1] = FixedImBuffer[CpxCnt2];
		}
	}
}


/* ##########################################################################################################
**
** Organisation         : CEA / LETI
**
** Function name        : Int2Bin
** Language             : C/C++
** Function prototype   : void Int2Bin (int Number, int *ptStateVector, int NbBits)
** Short description    : Static function that converts integer to binary
** In arguments         : int Number			:	number to convert into binary 
** Out arguments        : int *ptStateVector	:	binary vector corresponding to Number. LSB on the right
** Control arguments    : int NbBits			:	number of bits on which the number is coded.
** Return parameter     : none
** History              : 02/09/02 creation by Dimitri KTENAS (CEA)
** Called from          : Fft
** Functions  called    : /
** Detailed description : ptStateVector[0] = MSB, ptStateVector[NbBits - 1] = LSB
**                        
** COPYRIGHT            : CEA 
**
** ######################################################################################################## */

void Int2Bin (int Number, int *ptStateVector, int NbBits)
{
	int BitCounter; /* counter over the number of bits */

	for ( BitCounter = 0 ; BitCounter < NbBits ; BitCounter++ )
	{
		ptStateVector[BitCounter] = ( Number >> ((NbBits - 1) - BitCounter) ) & 1; /* logical and */
	}
}

/* ##########################################################################################################
**
** Organisation         : CEA / LETI
**
** Function name        : Bin2Int
** Language             : C/C++
** Function prototype   : void Bin2Int (int *ptStateVector, int *ptNumber, int NbBits)
** Short description    : Static function that converts binary to integer
** In arguments         : int *ptStateVector	:	binary vector corresponding to Number. LSB on the right
** Out arguments        : int *ptNumber			:	number corresponding to binary vector ptStateVector
** Control arguments    : int NbBits			:	number of bits on which the number is coded.
** Return parameter     : none
** History              : 02/09/02 creation by Dimitri KTENAS (CEA)
** Called from          : Fft
** Functions  called    : /
** Detailed description : ptStateVector[0] = MSB, ptStateVector[NbBits - 1] = LSB
**                        
** COPYRIGHT            : CEA
**
** ######################################################################################################## */

void Bin2Int (int *ptStateVector, int *ptNumber, int NbBits)
{
	int BitCounter; /* counter over the bits */

	*ptNumber  = 0; /* initialization of *ptNumber for cumulative sum */
	
	for ( BitCounter = 0 ; BitCounter < NbBits ; BitCounter++ )
	{
		*ptNumber += (ptStateVector[NbBits - (BitCounter + 1)] << BitCounter);
	}
}


#include <srl.h>
#include "iqzz_proto.h"
#include "srl_endianness.h"
#include "../common/jpeg.h"

/* Minimum and maximum values a `signed int' can hold.  */

/* Useful constants: */


FUNC(iqzz)
{
    srl_mwmr_t input  = GET_ARG(input);
    srl_mwmr_t quanti = GET_ARG(quanti);
    srl_mwmr_t output = GET_ARG(output);
    static int ZZ[64] = {0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63};

    int i,j;
    uint8_t tn[BLOCK_SIZE];
    int16_t fn[BLOCK_SIZE];
    int32_t fn_out[BLOCK_SIZE];


    //srl_log(TRACE, "IDCT thread is alive!\n");

    //Remplir le tableau Quanti
    srl_mwmr_read(quanti, tn, (BLOCK_SIZE*sizeof(*tn)));

    for (i=0; i<NBLOCKS; ++i ) {
      //srl_log_printf(TRACE, "IQZZ processing block %d/%d\n",i, NBLOCKS);
      
      //lire le bloc a traiter
      srl_mwmr_read(input, fn, (BLOCK_SIZE*sizeof(*fn)));

      //Quantisation Inverse  Fn'[i] = fn[i] * tn[i] (fn=bloc, tn=tableau quanti)
      //ZigZag utilise le tableau ZZ pour faire le réordonnancement des pixels du bloc
      for(j=0; j<BLOCK_SIZE;++j){
	fn_out[ZZ[j]] = fn[j] * tn[j];
	//fn_out[j] = fn[ZZ[j]] * tn[ZZ[j]];
      }
  
      //Sortie
      srl_mwmr_write( output, fn_out, (BLOCK_SIZE*sizeof(*fn_out)));
     }
}

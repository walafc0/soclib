#include <srl.h>
#include "libu_proto.h"
#include "../common/jpeg.h"

/* Minimum and maximum values a `signed int' can hold.  */

/* Useful constants: */


FUNC(libu)
{
    srl_mwmr_t input  = GET_ARG(input);
    srl_mwmr_t output = GET_ARG(output);

    int i,j, line, pixel;
    uint8_t buffer[8][WIDTH];
    uint8_t bloc[BLOCK_SIZE];

    //srl_log(TRACE, "LIBU thread is alive!\n");

    for (i=0; i<BLOCKS_H; ++i) {
      //srl_log_printf(TRACE, "LIBU processing block %d/%d\n",i, BLOCKS_H);

      //pour chaque 0 jusqu'a la largeur de l'image en blocs(BLOCKS_W)
      for (j=0; j<BLOCKS_W; ++j) {
	//lire le bloc a traiter
	srl_mwmr_read(input, bloc, (BLOCK_SIZE*sizeof(*bloc)));

	//copie un bloc à leur place dans buffer
	for (line=0;line<8; ++line) {
	  for(pixel=0;pixel<8;++pixel){
	     buffer[line][pixel+(8*j)] = bloc[pixel+(line*8)];
	  }
	} 
      }

      //sortie
      srl_mwmr_write( output, &buffer, (8*WIDTH*sizeof(uint8_t)));
    }
}

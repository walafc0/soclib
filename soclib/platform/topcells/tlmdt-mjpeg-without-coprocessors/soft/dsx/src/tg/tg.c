#include <srl.h>
#include "tg_proto.h"

#include "../common/jpeg.h"

#include "image.h"

FUNC(tg)
{
  srl_mwmr_t output = GET_ARG(output);
  
  int r;
  for ( r=0; r<sizeof(image); r+=32 )
    srl_mwmr_write(output,&image[r],32);

}


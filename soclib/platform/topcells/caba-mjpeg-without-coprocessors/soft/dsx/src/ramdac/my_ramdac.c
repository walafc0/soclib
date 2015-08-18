#include "srl.h"
#include "srl_endianness.h"
#include "ramdac_proto.h"

#include "../common/jpeg.h"

FUNC(ramdac)
{
    srl_mwmr_t input = GET_ARG(input);
    uint8_t frame_buffer[8*MAX_WIDTH];
    int32_t i;

    for (i=0;i<MAX_HEIGHT;i+=8) {
      //srl_log_printf(TRACE, "copying line %d->%d\n", i, i+7);
      srl_mwmr_read( input,
		     frame_buffer,
		     8*MAX_WIDTH );
    }
}

#include "srl.h"
#include "srl_endianness.h"
#include "ramdac_proto.h"

#include "../common/jpeg.h"

#include "ramdac_x11.h"

static uint8_t *frame_buffer;

FUNC(bootstrap)
{
    frame_buffer = fb_init(WIDTH, HEIGHT);
}

FUNC(ramdac)
{
    srl_mwmr_t input = GET_ARG(input);
    int32_t i;

    for (i=0;i<MAX_HEIGHT;i+=8) {
      //srl_log_printf(TRACE, "copying line %d->%d\n", i, i+7);
      srl_mwmr_read( input,
		     &frame_buffer[i*MAX_WIDTH],
		     8*MAX_WIDTH );
    }
    //srl_log(TRACE, "RAMDAC: display!\n");
    fb_update();
}

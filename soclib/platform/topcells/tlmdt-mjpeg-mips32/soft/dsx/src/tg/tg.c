#include <stdio.h>
#include <srl.h>
#include "tg_proto.h"

#include "../common/jpeg.h"

static FILE *fi;

FUNC(bootstrap)
{
    fi = fopen(FILE_NAME, "r");
	if ( ! fi ) {
		perror("fopen("FILE_NAME")");
		exit(1);
	}

    srl_mwmr_config(tg_ctrl, 0, 1);
}

FUNC(tg)
{
    srl_mwmr_t output = GET_ARG(output);
    uint8_t e[32];

	int r;
	r = fread(e, 1, 32, fi);
	if ( r < 32 ) {
		fseek( fi, 0, SEEK_SET );
		fread(e+r, 1, 32-r, fi);
	}
	srl_mwmr_write(output,e,32);
}


/*
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
 * Authors : Ivan MIRO-PANADES
 * 
 * History :
 *
 * Comment :
 *
 */

#include "system.h"
#include "stdio.h"
#include "stdlib.h"
#include "soclib/mwmr_controller.h"
#include "api_mwmr.h"


#include "../segmentation.h"
#include "stimulii.h"

//MWMR
#define NB_SAMPLES 64
#define WIDTH 4
#define DEPTH NB_SAMPLES

static uint32_t data_in[NB_SAMPLES] = DATA_IN;
static uint32_t data_out[NB_SAMPLES/4];

int main(void)
{
    int i;

    //CAUTION the FIFO data, the Lock and the FIFO status have to be in a non-cached memory
    ///////////////////////////////////////////////////////////////////////////////////////
    uint32_t *fifo_data_in    = (uint32_t*)(base(MWMRd)+0x000); //0x20200000;
    uint32_t *fifo_data_out   = (uint32_t*)(base(MWMRd)+0x400); //0x20200200;
    
    uint32_t *lock_in     = (uint32_t*)(base(LOCKS)+0x00);
    uint32_t *lock_out    = (uint32_t*)(base(LOCKS)+0x20);
    
    mwmr_t *p_mwmr_in    = (mwmr_t*)(base(MWMRd)+0x1000);
    mwmr_t *p_mwmr_out   = (mwmr_t*)(base(MWMRd)+0x1200);
    
    //MWMR configuration
    ////////////////////
    mwmr_initialize_pointer(p_mwmr_in,    WIDTH, DEPTH, fifo_data_in,    lock_in   );
    mwmr_initialize_pointer(p_mwmr_out,   WIDTH, DEPTH/4, fifo_data_out,   lock_out  );
    
    // DEBUG
    printf("\nTaille = %d\n", p_mwmr_in->width);
    printf("Profondeur = %d\n", p_mwmr_in->gdepth);
    //printf("Status = %d\n", p_mwmr_in->width);

    //MWMR programming
    ////////////////////
    printf("Programming the MWMR");
    putchar('\n');
    mwmr_hw_init(base(MWMR), MWMR_TO_COPROC, 0 , p_mwmr_in);
    mwmr_hw_init(base(MWMR), MWMR_FROM_COPROC, 0  , p_mwmr_out);
 
    //Send the data to FIFO and read the processed data from FIFO 
    /////////////////////////////////////////////////////////////
    printf("Sending the data to the downsampling component");
    putchar('\n');
    mwmr_write(p_mwmr_in, data_in, WIDTH*DEPTH);
    
    for(i=0; i<NB_SAMPLES; i++) 
    {
       printf("data_in[%d] = %d\n",i, p_mwmr_in->buffer[i]);
    }
    //Wait until the coprocessos has finished    
    //Read the output data
    printf("Reading the output data");
    putchar('\n');
    mwmr_read(p_mwmr_out, data_out, WIDTH*DEPTH/4);
    for(i=0; i<NB_SAMPLES/4; i++) 
    {
       printf("data_out[%d] = %d\n",i, data_out[i]);
    }
    printf("Data read OK");
    putchar('\n');

    while(1); 

    return 0;
}


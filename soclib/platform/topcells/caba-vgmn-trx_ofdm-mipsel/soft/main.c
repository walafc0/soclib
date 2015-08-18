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
#include "api_trx_ofdm.h"


#include "../segmentation.h"
#include "fifo_datain0.h"

//MWMR
#define WIDTH 4
#define DEPTH 128

static uint32_t config_data[132];
static uint32_t data_in0[4096]   = DATA_IN0;
static uint32_t data_out0[4096];

int main(void)
{
    int i;

    //CAUTION the FIFO data, the Lock and the FIFO status have to be in a non-cached memory
    ///////////////////////////////////////////////////////////////////////////////////////
    uint32_t *fifo_data_in0    = (uint32_t*)(base(MWMRd)+0x000); //0x20200000;
    uint32_t *fifo_data_in1    = (uint32_t*)(base(MWMRd)+0x200); //0x20200100;
    uint32_t *fifo_data_out0   = (uint32_t*)(base(MWMRd)+0x400); //0x20200200;
    uint32_t *fifo_data_out1   = (uint32_t*)(base(MWMRd)+0x600); //0x20200300;
    uint32_t *fifo_data_config = (uint32_t*)(base(MWMRd)+0x800); //0x20200400;

    uint32_t *lock_in0     = (uint32_t*)(base(LOCKS)+0x00);
    uint32_t *lock_in1     = (uint32_t*)(base(LOCKS)+0x10);
    uint32_t *lock_out0    = (uint32_t*)(base(LOCKS)+0x20);
    uint32_t *lock_out1    = (uint32_t*)(base(LOCKS)+0x30);
    uint32_t *lock_config  = (uint32_t*)(base(LOCKS)+0x40);

    mwmr_t *p_mwmr_in0    = (mwmr_t*)(base(MWMRd)+0x1000);
    mwmr_t *p_mwmr_in1    = (mwmr_t*)(base(MWMRd)+0x1100);
    mwmr_t *p_mwmr_out0   = (mwmr_t*)(base(MWMRd)+0x1200);
    mwmr_t *p_mwmr_out1   = (mwmr_t*)(base(MWMRd)+0x1300);
    mwmr_t *p_mwmr_config = (mwmr_t*)(base(MWMRd)+0x1400);

    //MWMR configuration
    ////////////////////
    mwmr_initialize_pointer(p_mwmr_in0,    WIDTH, DEPTH, fifo_data_in0,    lock_in0   );
    mwmr_initialize_pointer(p_mwmr_in1,    WIDTH, DEPTH, fifo_data_in1,    lock_in1   );
    mwmr_initialize_pointer(p_mwmr_out0,   WIDTH, DEPTH, fifo_data_out0,   lock_out0  );
    mwmr_initialize_pointer(p_mwmr_out1,   WIDTH, DEPTH, fifo_data_out1,   lock_out1  );
    mwmr_initialize_pointer(p_mwmr_config, WIDTH, DEPTH, fifo_data_config, lock_config);

    //MWMR programming
    ////////////////////
    printf("Programming the MWMR");
    putchar('\n');
    mwmr_hw_init(base(MWMR), MWMR_TO_COPROC,   TRX_OFDM_IN0   , p_mwmr_in0);
    mwmr_hw_init(base(MWMR), MWMR_TO_COPROC,   TRX_OFDM_IN1   , p_mwmr_in1);
    mwmr_hw_init(base(MWMR), MWMR_FROM_COPROC, TRX_OFDM_OUT0  , p_mwmr_out0);
    mwmr_hw_init(base(MWMR), MWMR_FROM_COPROC, TRX_OFDM_OUT1  , p_mwmr_out1);
    mwmr_hw_init(base(MWMR), MWMR_TO_COPROC,   TRX_OFDM_CONFIG, p_mwmr_config);


    ////////////////////////////////////
    //Define de configuration parameters
    ////////////////////////////////////
    for(i=0; i<132; i++) {
        config_data[i] = 0;     //Clear the config data vector
    }
    //Configure the Mask_data values
    for(i=7; i<25; i++) {
        config_data[i] = 0xffffffff;     //Clear the config data vector
    }
    config_data[ 6] = 0xfff00000;
    config_data[16] = 0xfffffffe;
    config_data[25] = 0x00001fff;

    //Configure the parameters
    config_data[TRX_OFDM_FFT_CFG    ] = TRX_OFDM_SHIFT_CARRIER | TRX_OFDM_FFT_TYPE | TRX_OFDM_NORMALIZE_POWER | TRX_OFDM_SIZE_FFT_1024;
    config_data[TRX_OFDM_GI_CFG     ] = 0;
    config_data[TRX_OFDM_FRAMING_CFG] = 0;
    config_data[TRX_OFDM_IT_CFG     ] = 0;

    ///////////////////////////////////////////////////
    //Send a configuration to the trx_ofdm coprocessor
    ///////////////////////////////////////////////////
    printf("Loading the configuration in the trx_ofdm");
    putchar('\n');
    configure_trx_ofdm(base(MWMR), p_mwmr_config, 0x000, config_data, 132); //Configure a FFT on Slot 0

    config_data[TRX_OFDM_FFT_CFG] = TRX_OFDM_SHIFT_CARRIER | TRX_OFDM_NORMALIZE_POWER | TRX_OFDM_SIZE_FFT_1024;
    configure_trx_ofdm(base(MWMR), p_mwmr_config, 0x100, config_data, 132); //Configure a IFFT on Slot 1


    /////////////////////////////
    // First block of data
    /////////////////////////////

    //Send the data to FIFO 0 and read the processed data from FIFO 0
    /////////////////////////////////////////////////////////////////
    printf("Sending the data to the trx_ofdm");
    putchar('\n');
    start_trx_ofdm(base(MWMR), 0);          //Start the FFT (SlotId 0)
    write_trx_ofdm_data( p_mwmr_in0, data_in0, 1024);

    //Wait until the coprocessos has finished
    /////////////////////////////////////////
    wait_EOC(base(MWMR));
    int value = get_trx_ofdm_status(base(MWMR));
    printf("The trx_ofdm is ready with status %i", value);
    putchar('\n');
    
    //Read the output data
    //////////////////////
    printf("Reading the output data");
    putchar('\n');
    read_trx_ofdm_data( p_mwmr_out0, data_out0, 600 );     //Read 600 words of data
    printf("Data read OK");
    putchar('\n');

    /////////////////////////////
    // Second block of data
    /////////////////////////////

    //Send the data to FIFO 0 and read the processed data from FIFO 0
    /////////////////////////////////////////////////////////////////
    printf("Sending the data to the trx_ofdm");
    putchar('\n');
    start_trx_ofdm(base(MWMR), 1);          //Start the IFFT (SlotId 1)
    write_trx_ofdm_data( p_mwmr_in0, data_in0+1024, 600);
    wait_EOC(base(MWMR));

    //Read the output data
    //////////////////////
    printf("Reading the output data");
    putchar('\n');
    read_trx_ofdm_data( p_mwmr_out0, data_out0, 1024 );     //Read 600 words of data
    printf("Data read OK");
    putchar('\n');

    while(1); 

    return 0;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

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
#include "api_fht.h"


#include "../segmentation.h"
#include "fifo_datain0.h"

//MWMR
#define WIDTH 4
#define DEPTH 128

static uint32_t config_data[3];
static uint32_t data_in0[4096]   = DATA_IN0;
static uint32_t data_out0[4096];

int main(void)
{
    int end_write, end_read;
    int idx_write, idx_read;

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
    mwmr_hw_init(base(MWMR), MWMR_TO_COPROC,   FHT_IN0   , p_mwmr_in0);
    mwmr_hw_init(base(MWMR), MWMR_TO_COPROC,   FHT_IN1   , p_mwmr_in1);
    mwmr_hw_init(base(MWMR), MWMR_FROM_COPROC, FHT_OUT0  , p_mwmr_out0);
    mwmr_hw_init(base(MWMR), MWMR_FROM_COPROC, FHT_OUT1  , p_mwmr_out1);
    mwmr_hw_init(base(MWMR), MWMR_TO_COPROC,   FHT_CONFIG, p_mwmr_config);


    ////////////////////////////////////
    //Define de configuration parameters
    ////////////////////////////////////
    config_data[FHT_CONF] = FHT_NB_BUF(96) | FHT_NB_SHIFT(3) | FHT_SIZE_32 | FHT_SAT_RST_AT_LOAD;
    config_data[FHT_USER_MASK0] = 0xffffffff;
    config_data[FHT_USER_MASK1] = 0x00000000;

    ///////////////////////////////////////////////////
    //Send a configuration to the fht coprocessor
    ///////////////////////////////////////////////////
    printf("Loading the configuration in the fht");
    putchar('\n');

    configure_fht(base(MWMR), p_mwmr_config, 0x0, config_data, 3); //Configure a FFT on Slot 0


    /////////////////////////////////////////////////////////////////
    //Send the data to FIFO 0 and read the processed data from FIFO 0
    /////////////////////////////////////////////////////////////////
    printf("Sending the data to the fht");
    putchar('\n');
    start_fht(base(MWMR), 0);          //Start the FHT (SlotId 0)

    end_write = end_read = 0;
    idx_write = idx_read = 0;
    do {
        if (end_write == 0)   //Write new data
            idx_write += try_write_fht_data( p_mwmr_in0, data_in0+idx_write, 3072-idx_write);
        if (end_read == 0) //Read new data
            idx_read  += try_read_fht_data( p_mwmr_out0, data_out0+idx_read, 3072-idx_read);
        printf("Written data: %i, Read data: %i",idx_write,idx_read);
        putchar('\n');
        if (idx_write == 3072)  //Stop writing data at the end of the vector
            end_write = 1;
        if (idx_read == 3072)   //Stop reading data at the end of the vector
            end_read = 1;
    } while ((end_write == 0) || (end_read == 0));
    printf("End of operation");
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

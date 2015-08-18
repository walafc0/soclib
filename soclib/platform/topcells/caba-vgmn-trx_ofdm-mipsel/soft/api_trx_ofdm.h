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

#ifndef API_TRX_OFDM_H
#define API_TRX_OFDM_H

#include "api_mwmr.h"

#define TRX_OFDM_EXEC       0
#define TRX_OFDM_SlotId     1
#define TRX_OFDM_ConfigAddr 5

#define TRX_OFDM_EOC        0
#define TRX_OFDM_Status     1

#define TRX_OFDM_IN0        0
#define TRX_OFDM_IN1        1
#define TRX_OFDM_OUT0       0
#define TRX_OFDM_OUT1       1
#define TRX_OFDM_CONFIG     2

//Register addresses
#define TRX_OFDM_FFT_CFG        0x80
#define TRX_OFDM_GI_CFG         0x81
#define TRX_OFDM_FRAMING_CFG    0x82
#define TRX_OFDM_IT_CFG         0x83

//FFT_size
#define TRX_OFDM_SIZE_FFT_32    5
#define TRX_OFDM_SIZE_FFT_64    6
#define TRX_OFDM_SIZE_FFT_126   7
#define TRX_OFDM_SIZE_FFT_256   8
#define TRX_OFDM_SIZE_FFT_512   9
#define TRX_OFDM_SIZE_FFT_1024  10
#define TRX_OFDM_SIZE_FFT_2048  11

//Config parameters
//FFT_CFG
#define TRX_OFDM_NORMALIZE_POWER 0x020
#define TRX_OFDM_FFT_TYPE        0x040   
#define TRX_OFDM_BYPASS_FFT      0x080
#define TRX_OFDM_SHIFT_CARRIER   0x100
#define TRX_OFDM_SHIFT_PARITY    0x200

//GI_CFG
#define TRX_OFDM_GI_INSERTION   0x80000000

//FRAMING_CFG
#define TRX_OFDM_FLOC           0x1



inline void start_trx_ofdm(void *coproc, int SlotId) {
    mwmr_config( coproc, TRX_OFDM_SlotId, SlotId);        //Write the SlotId to 0
    mwmr_config( coproc, TRX_OFDM_EXEC,   0  );           //Initialize the Start vector before change its value
    mwmr_config( coproc, TRX_OFDM_EXEC,   1  );           //Start a new processing
}

void configure_trx_ofdm( void *coproc, mwmr_t *mwmr,  int address, int *data, int num_words) {
    mwmr_wait_fifo_empty(coproc, MWMR_TO_COPROC,   2, mwmr);    //Wait until the FIFO is empty (if any)
    mwmr_config( coproc, TRX_OFDM_ConfigAddr, address+1 );      //Initialize the configuartion address before changing its value
    mwmr_config( coproc, TRX_OFDM_ConfigAddr, address   );      //Configuration data starting address 
    mwmr_write( mwmr, data, num_words*4);                       //Write the configuration
    mwmr_wait_fifo_empty(coproc, MWMR_TO_COPROC,   2, mwmr);    //Wait the end of the configuration
}

inline void wait_EOC(void *coproc) {
    while (mwmr_status( coproc, TRX_OFDM_EOC ) == 0);  //Wait the End of Compute of the trx_ofdm coprocessor
}

inline int get_trx_ofdm_status(void *coproc) {
    return(mwmr_status( coproc, TRX_OFDM_Status ));
}

inline void write_trx_ofdm_data(mwmr_t *mwmr, int *data, int num_words) {
    mwmr_write( mwmr, data, num_words*4);
}

inline void read_trx_ofdm_data(mwmr_t *mwmr, int *data, int num_words) {
    mwmr_read( mwmr, data, num_words*4);
}
                                  
#endif /* API_TRX_OFDM_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

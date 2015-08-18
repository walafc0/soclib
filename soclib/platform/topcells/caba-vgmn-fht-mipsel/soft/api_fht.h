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

#ifndef API_FHT_H
#define API_FHT_H

#include "api_mwmr.h"

#define FHT_EXEC       0
#define FHT_SlotId     1
#define FHT_ConfigAddr 5

#define FHT_EOC        0
#define FHT_Status     1

#define FHT_IN0        0
#define FHT_IN1        1
#define FHT_OUT0       0
#define FHT_OUT1       1
#define FHT_CONFIG     2

//Register addresses
#define FHT_CONF        0x0
#define FHT_USER_MASK0  0x1
#define FHT_USER_MASK1  0x2

//FHT_size
#define FHT_SIZE_8   0
#define FHT_SIZE_16  1
#define FHT_SIZE_32  2

//Config parameters
#define FHT_NB_BUF(x)   x<<5
#define FHT_NB_SHIFT(x) x<<2

//Saturation
#define FHT_SAT_RST_AT_LOAD 1<<17



inline void start_fht(void *coproc, int SlotId) {
    mwmr_config( coproc, FHT_SlotId, SlotId);        //Write the SlotId to 0
    mwmr_config( coproc, FHT_EXEC,   0  );           //Initialize the Start vector before change its value
    mwmr_config( coproc, FHT_EXEC,   1  );           //Start a new processing
}

void configure_fht( void *coproc, mwmr_t *mwmr,  int address, int *data, int num_words) {
    mwmr_wait_fifo_empty(coproc, MWMR_TO_COPROC,   2, mwmr);    //Wait until the FIFO is empty (if any)
    mwmr_config( coproc, FHT_ConfigAddr, address+1 );           //Initialize the configuartion address before changing its value
    mwmr_config( coproc, FHT_ConfigAddr, address   );           //Configuration data starting address 
    mwmr_write( mwmr, data, num_words*4);                       //Write the configuration
    mwmr_wait_fifo_empty(coproc, MWMR_TO_COPROC,   2, mwmr);    //Wait the end of the configuration
}

inline void wait_EOC(void *coproc) {
    while (mwmr_status( coproc, FHT_EOC ) == 0);  //Wait the End of Compute of the fht coprocessor
}

inline int get_fht_status(void *coproc) {
    return(mwmr_status( coproc, FHT_Status ));
}

inline void write_fht_data(mwmr_t *mwmr, int *data, int num_words) {
    mwmr_write( mwmr, data, num_words*4);
}

inline void read_fht_data(mwmr_t *mwmr, int *data, int num_words) {
    mwmr_read( mwmr, data, num_words*4);
}

inline int try_write_fht_data(mwmr_t *mwmr, int *data, int num_words) {
    return (mwmr_try_write( mwmr, data, num_words*4) >> 2);
}

inline int try_read_fht_data(mwmr_t *mwmr, int *data, int num_words) {
    return(mwmr_try_read( mwmr, data, num_words*4) >> 2);
}
                                
#endif /* API_FHT_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

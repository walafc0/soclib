#ifndef HUFF_TABLES_H_
#define HUFF_TABLES_H_

#include "../common/jpeg.h"
#include "bit_reading.h"
#include "../common/block_io.h"

#define MAX_SIZE(class)         ((class)?162:14) /* Memory size of HTables */

typedef struct {
    uint8_t *HTable[4];
    int32_t MinCode[4][16];
    int32_t MaxCode[4][16];
    int32_t ValPtr[4][16];
    uint8_t DC_Table0[MAX_SIZE(DC_CLASS)];
    uint8_t DC_Table1[MAX_SIZE(DC_CLASS)];
    uint8_t AC_Table0[MAX_SIZE(AC_CLASS)];
    uint8_t AC_Table1[MAX_SIZE(AC_CLASS)];
} huff_context;

void huff_tables_init( huff_context *cont );
int32_t huff_load_tables( huff_context *cont, block_io *input );
int32_t huff_get_symbol( bitreader_context *cont, huff_context *huff, int32_t select );

#endif /* HUFF_TABLES_H_ */


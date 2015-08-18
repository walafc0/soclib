#ifndef BIT_READING_H_
#define BIT_READING_H_

#include <srl.h>
#include "../common/block_io.h"

typedef struct {
    block_io *io;
    uint8_t current;
    uint8_t available;
} bitreader_context;

uint32_t bitreader_get(bitreader_context *cont, int32_t number);

void bitreader_clear(bitreader_context *cont);

uint8_t bitreader_get_one(bitreader_context *cont);

void bitreader_init(bitreader_context *cont, block_io *chan);

#endif /* BIT_READING_H_ */

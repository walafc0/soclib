#ifndef BLOCK_IO_H
#define BLOCK_IO_H

#include <srl.h>

typedef struct block_io {
    int32_t size;
    int32_t left;
    uint8_t *ptr;
    uint8_t *buffer;
    srl_mwmr_t io;
} block_io;

void block_io_init_in( block_io *io, void *buffer, uint32_t size, srl_mwmr_t mwmr );
void block_io_init_out( block_io *io, void *buffer, uint32_t size, srl_mwmr_t mwmr );

uint8_t block_io_read_int8( block_io *io );
uint16_t block_io_read_int16( block_io *io );
uint32_t block_io_read_int32( block_io *io );

void block_io_write_int8( block_io *io, uint8_t val );
void block_io_write_int16( block_io *io, uint16_t val );
void block_io_write_int32( block_io *io, uint32_t val );

void block_io_io(block_io *in, block_io *o, int32_t len);

void block_io_skip( block_io *io, int32_t len );

void block_io_flush( block_io *io );
void block_io_discard( block_io *io );

#endif /* BLOCK_IO_H */

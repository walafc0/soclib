#include <srl.h>

#include "../common/block_io.h"

/* static inline void io_refill( block_io *io ) */
/* { */
/*     srl_mwmr_read( io->io, io->buffer, io->size ); */
/*     io->left = io->size; */
/*     io->ptr = io->buffer; */
/* } */

/* static inline void io_flush( block_io *io ) */
/* { */
/*     srl_mwmr_write( io->io, io->buffer, io->size ); */
/*     io->left = io->size; */
/*     io->ptr = io->buffer; */
/* } */

/* static void io_dump( block_io *io ) */
/* { */
/*     uint32_t i; */
/*     for ( i=0; i<io->size; ++i ) */
/*         srl_log_printf(DEBUG, "%02x ", io->buffer[i]); */
/*     srl_log_printf(DEBUG, "\n"); */
/* } */

#define io_refill( bio )                                 \
do                                                       \
{                                                        \
    srl_log(DEBUG, "Refilling block\n" ); \
    srl_mwmr_read( bio->io, bio->buffer, bio->size );  \
    bio->left = bio->size;                               \
    bio->ptr = bio->buffer;                              \
} while (0)

#define io_flush( bio )                                  \
do                                                       \
{                                                        \
    srl_log(DEBUG, "Flushing block\n" ); \
    srl_mwmr_write( bio->io, bio->buffer, bio->size ); \
    bio->left = bio->size;                               \
    bio->ptr = bio->buffer;                              \
} while (0)

void block_io_flush( block_io *io )
{
    srl_log(DEBUG, "Forcing " );
    if ( io->left != io->size )
        io_flush(io);
}

void block_io_discard( block_io *io )
{
    srl_log(DEBUG, "Forcing block discard\n" );
    io->left = 0;
}

void block_io_init_out( block_io *io, void *buffer, uint32_t size, srl_mwmr_t mwmr )
{
    io->io = mwmr;
    io->buffer = buffer;
    io->ptr = io->buffer;
    io->size = size;
    io->left = size;
}

void block_io_init_in( block_io *io, void *buffer, uint32_t size, srl_mwmr_t mwmr )
{
    io->io = mwmr;
    io->buffer = buffer;
    io->ptr = io->buffer;
    io->size = size;
    io->left = 0;
}


uint8_t block_io_read_int8( block_io *io )
{
    uint8_t ret;

    if ( io->left == 0 )
        io_refill(io);
    io->left -= 1;
    ret = *(io->ptr++);
    srl_log_printf(DEBUG, "bio_read_int8: %02x\n", ret);
    return ret;
}

uint16_t block_io_read_int16( block_io *io )
{
    uint16_t ret;

    if ( io->left == 1 ) {
        ret = *(io->ptr++);
        io_refill(io);
        io->left -= 1;
        io->ptr++;
        return (ret << 8) | io->buffer[0];
    }
    if ( io->left == 0 )
        io_refill(io);
    /* On sera pas toujours aligne */
    io->left -= 2;
    ret = *(io->ptr++);
    ret = (ret<<8) | *(io->ptr++);
    srl_log_printf(DEBUG, "bio_read_int16: %04x\n", ret);
    return ret;
}

uint32_t block_io_read_int32( block_io *io )
{
    uint32_t ret = 0;
    int32_t left, i;

    left = io->left;

    for ( i=0; i<4; ++i ) {
        if ( left == 0 ) {
            io_refill(io);
            left = io->size;
        }
        ret <<= 8;
        ret |= *(io->ptr++);
        left -= 1;
    }
    io->left = left;
    srl_log_printf(DEBUG, "bio_read_int32: %08x\n", ret);
    return ret;
}


void block_io_write_int8( block_io *io, uint8_t val )
{
    if ( io->left == 0 )
        io_flush(io);
    io->left -= 1;
    *(io->ptr++) = val;
}

void block_io_write_int16( block_io *io, uint16_t val )
{
    uint8_t tmp = val>>8;

    if ( io->left == 0 )
        io_flush(io);
    io->left -= 1;
    *(io->ptr++) = tmp;
    if ( io->left == 0 )
        io_flush(io);
    io->left -= 1;
    *(io->ptr++) = val;
}

void block_io_write_int32( block_io *io, uint32_t val )
{
    uint8_t tmp3 = val>>24,
        tmp2 = val>>16,
        tmp1 = val>>8;

    if ( io->left == 0 )
        io_flush(io);
    io->left -= 1;
    *(io->ptr++) = tmp3;
    if ( io->left == 0 )
        io_flush(io);
    io->left -= 1;
    *(io->ptr++) = tmp2;
    if ( io->left == 0 )
        io_flush(io);
    io->left -= 1;
    *(io->ptr++) = tmp1;
    if ( io->left == 0 )
        io_flush(io);
    io->left -= 1;
    *(io->ptr++) = val;
}


void block_io_io(block_io *in, block_io *out, int32_t len)
{
    int32_t left = len;

    while (left) {
        int32_t cs = left;

        if (in->left == 0)
            io_refill(in);
        if (out->left == 0)
            io_flush(out);
        if (cs > in->left)
            cs = in->left;
        if (cs > out->left)
            cs = out->left;
        srl_assert(cs);
        memcpy(out->ptr, in->ptr, cs);
        out->left -= cs;
        in->left -= cs;
        out->ptr += cs;
        in->ptr += cs;
        left -= cs;
    }
}

void block_io_skip( block_io *io, int32_t len )
{
	srl_log_printf(DEBUG, "Skipping %d bytes\n", len);
    while ( len ) {
        int32_t rlen;
        if (io->left == 0)
            io_refill(io);
        rlen = (len<io->left)?len:io->left;
        io->left -= rlen;
        io->ptr += rlen;
        len -= rlen;
    }
}


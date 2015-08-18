#include <srl.h>
#include "vld_proto.h"

#include "../common/jpeg.h"
#include "../common/block_io.h"
#include "bit_reading.h"
#include "huff_tables.h"

/* component descriptor structure */
typedef struct {
	uint8_t	CID;	/* component ID */
	char QT;	/* QTable index, 2bits 	*/
	char DC_HT;	/* DC table index, 1bit */
	char AC_HT;	/* AC table index, 1bit */
	int32_t PRED;	/* DC predictor value */
} cd_t;

static int32_t vld_decode_unpack_block(
    bitreader_context *bits,
    srl_mwmr_t output,
    huff_context *huff,
    cd_t *comp
    );

inline static int32_t intceil(int32_t N, int32_t D)
{
    return (N+D-1)/D;
}

/* transform JPEG number format into usual 2's complement format */
inline static long reformat(uint32_t S, int32_t good)
{
    uint32_t ext, sign;

    if (!good)
        return 0;
    sign = !((1<<(good-1))&(S));
    ext = 0-(sign<<good);
    return (S|ext)+sign;
}


FUNC(vld)
{
    srl_mwmr_t input_mwmr = GET_ARG(input);
    srl_mwmr_t output = GET_ARG(output);
    srl_mwmr_t huffman_mwmr = GET_ARG(huffman);
    cd_t comp;
    huff_context huff;
    int32_t left;
    int32_t x_size, y_size;
    int32_t mx_size, my_size;
    uint8_t tmp;
    bitreader_context bits;

    uint8_t input_buffer[32];
    block_io input;

    uint8_t huffman_buffer[32];
    block_io huffman;

    block_io_init_in(&input, input_buffer, 32, input_mwmr);
    block_io_init_in(&huffman, huffman_buffer, 32, huffman_mwmr);

    //srl_log(TRACE, "VLD thread is alive\n");
    huff_tables_init(&huff);
    /* DHT */
    huff_load_tables(&huff, &huffman);
    block_io_discard(&huffman);
    huff_load_tables(&huff, &huffman);
    block_io_discard(&huffman);

    //srl_log(TRACE, "huffman tables loaded\n");
//    huff_dump( &huff );

#if 0
    /*
     * This code can help if we want to tolerate desynchronizations
     */
    //srl_log(TRACE, "Looking for magic... ");
    do {
        int32_t magic = block_io_read_int32(&input);
        if ( magic == DATA_MAGIC ) {
	  //srl_log(TRACE, "got it\n");
            break;
        }
        block_io_discard(&input);
    } while (1);
#else
    srl_assert ( block_io_read_int32(&input) == DATA_MAGIC );
#endif

    x_size = block_io_read_int16(&input);
    y_size = block_io_read_int16(&input);

    block_io_read_int8(&input);
    srl_log_printf(DEBUG, "picture size: %dx%d\n", x_size, y_size);

    mx_size = intceil(x_size, BLOCK_WIDTH);
    my_size = intceil(y_size, BLOCK_HEIGHT);

    /* SOS */
    block_io_read_int32(&input);

    tmp = block_io_read_int8(&input);
    comp.DC_HT = first_quad(tmp);
    comp.AC_HT = second_quad(tmp);
    srl_log_printf(DEBUG,
                    "DC: %d, AC: %d\n",
                    comp.DC_HT, comp.AC_HT);

    block_io_read_int8(&input);
    block_io_read_int16(&input);

    bitreader_init(&bits, &input);
    comp.PRED = 0;
    left = mx_size*my_size;

    while (left > 0) {
        vld_decode_unpack_block(&bits, output, &huff, &comp);
        srl_log_printf(DEBUG, "Put uncompressed block no %d\n", left);
        --left;
    }
}


/*
 * private huffman.c defines and macros
 */
#define HUFF_EOB                0x00
#define HUFF_ZRL                0xF0

/*
 * here we unpack a 8x8 DCT block
 */
static int32_t vld_decode_unpack_block(
    bitreader_context *bits,
    srl_mwmr_t output,
    huff_context *huff,
    cd_t *comp
    )
{
    uint32_t temp;
    uint32_t i, run, cat;
    int32_t value;
    uint8_t symbol;
    int16_t T[BLOCK_SIZE];

    for (i = 0; i < BLOCK_SIZE; i++)
        T[i] = 0;

    /* first get the DC coefficient */
    symbol = huff_get_symbol( bits, huff, HUFF_ID(DC_CLASS,comp->DC_HT) );
    temp = bitreader_get( bits, symbol );
    value = reformat( temp, symbol );
    value += comp->PRED;
    comp->PRED = value;

	/* reoganize and unquantify -> move to ZZ and IQ  */
    T[0] = value;
	/* then the AC ones
     * if symbol found is EOB and process not finish, missing values
     * are replaced by zero
     */
    for ( i=1; i<BLOCK_SIZE; i++ ) {
        symbol = huff_get_symbol( bits, huff, HUFF_ID(AC_CLASS,comp->AC_HT) );
        srl_log_printf(DEBUG, "huff_symbol %d: %x\n", i, symbol);
        if (symbol == HUFF_EOB) {
            srl_log(DEBUG, "HUFF_EOB\n");
            break;
        }
        if (symbol == HUFF_ZRL) {
            srl_log(DEBUG, "HUFF_ZRL\n");
            i += 15;
            continue;
        }
        cat = symbol & 0xf;
        run = symbol >> 4;
        i += run;
        temp = bitreader_get( bits, cat );
        value = reformat( temp, cat );
        T[i] = value;
    }
    srl_mwmr_write(output, (uint8_t *)T, 2*BLOCK_SIZE);
    return 0;
}

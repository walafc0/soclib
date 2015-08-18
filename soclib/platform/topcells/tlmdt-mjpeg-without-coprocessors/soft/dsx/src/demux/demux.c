#include "srl.h"
#include "demux_proto.h"
#include "srl_endianness.h"

#include "../common/jpeg.h"

#include "../common/block_io.h"

uint16_t get_next_mk(block_io *f)
{
    uint8_t buf, ffmet = 0;
    
    do {
        buf = block_io_read_int8(f);
        switch (buf) {
        case 0xff:
            ffmet = 1;
            break;
        default:
            if (ffmet) {
	      srl_log_printf(DEBUG, "found marker ff%02x\n", buf);
	      return buf|0xff00;
            }
        case 0:
            ffmet = 0;
            break;
        }
    } while (1);
    return EOF;
}

void load_quant_tables(block_io *input, block_io *quanti)
{
    uint8_t aux;
    uint32_t size;

    size = block_io_read_int16(input)-2;
    //srl_log_printf(TRACE, "Loading quanti tables, size: %d\n", (int)size);
    size /= 65;
    while (size>0) {
        aux = block_io_read_int8(input);
        block_io_io(input, quanti, 64);
        --size;
    }
}

void skip_segment(block_io *f)
{
    uint32_t size;
    uint8_t tag[8];

    size = block_io_read_int16(f);
    size -= 2;
    if (size > 5) {
        *(uint32_t*)tag = block_io_read_int32(f);
        tag[4] = 0;
        srl_log_printf(DEBUG, "SKIP: tag: %s, size: %d\n", tag, size);
        size -= 4;
    }

    block_io_skip(f, size);
}

FUNC(demux) {
    uint32_t aux;
    uint16_t mark = 0, pot_mark;
    uint8_t marker;
	uint8_t input_buffer[32];
	block_io input;
	uint8_t output_buffer[32];
	block_io output;
	uint8_t huffman_buffer[32];
	block_io huffman;
	uint8_t quanti_buffer[64];
	block_io quanti;
	uint32_t picture_no;

    int32_t found_mk, done, end_nb = 0, in_frame = 0;

    srl_mwmr_t input_mwmr = GET_ARG(input);
    srl_mwmr_t output_mwmr = GET_ARG(output);
    srl_mwmr_t huffman_mwmr = GET_ARG(huffman);
    srl_mwmr_t quanti_mwmr = GET_ARG(quanti);

    /*
     * Les block_io sont des optimisation des acces aux canaux mwmr
     * quand ceux-ci servent a faire transiter des donnees de tailles
     * non fixes.
     *
     * Les transferts inities par les block_io se feront par blocs de
     * n _octets_, n etant la taille passee lors de leur intialisation
     * (ici 32 ou 64 octets, soit 8 ou 16 mots 32 bits).
     */
    block_io_init_in(&input, input_buffer, 32, input_mwmr);
    block_io_init_out(&output, output_buffer, 32, output_mwmr);
    block_io_init_out(&huffman, huffman_buffer, 32, huffman_mwmr);
    block_io_init_out(&quanti, quanti_buffer, 64, quanti_mwmr);

	picture_no = 0;

	while(1) {
	do {
		aux = get_next_mk(&input);
	} while (aux != SOI_MK);
	//srl_log(TRACE, "found the SOI marker\n");
	//srl_log_printf(TRACE, "picture %d\n", ++picture_no);
	found_mk = 0;
	done = 0;
	while (!done) {
		if (!found_mk)
			mark = get_next_mk(&input);
		switch (mark) {
		case SOF_MK:
		  //srl_log(TRACE, "found SOF marker\n");
			in_frame = 1;
			block_io_write_int32(&output, DATA_MAGIC);
			/* Dont care about Header size */
			block_io_read_int16(&input);
			/* Dont care about precision & sizes */
			block_io_read_int32(&input);
			block_io_write_int16(&output, WIDTH);
			block_io_write_int16(&output, HEIGHT);

			/* # of components */
			/* Sampling factor & QuantT index */
			aux = block_io_read_int32(&input);
			block_io_write_int8(&output, aux>>16);

			break;
		case DHT_MK:
		  //srl_log(TRACE, "huffman table marker\n");
			found_mk = 0;
			while (!found_mk) {
				marker = block_io_read_int8(&input);
				if (marker == 0xff) {
					pot_mark = block_io_read_int8(&input);
					if (pot_mark == 0) {
						block_io_write_int8(&huffman, 0xff);
					} else {
						mark = 0xff00|pot_mark;
						srl_log_printf(DEBUG, "found marker %04x while in huff def\n", mark);
						found_mk = 1;
					}
				} else /* isn't 0xff */ {
					block_io_write_int8(&huffman, marker);
				}
			}
			block_io_flush(&huffman);
			break;

		case DQT_MK:
		  //srl_log(TRACE, "quantization table marker\n");
		  load_quant_tables(&input, &quanti);
		  block_io_flush(&quanti);
		  break;

		case DRI_MK:
			/* skip sizes */
			//srl_log(TRACE, "DRI marker\n");
			block_io_read_int32(&input);
			break;

		case SOS_MK:
		  //srl_log(TRACE, "sos marker\n");
			found_mk = 0;
			while (!found_mk) {
				marker = block_io_read_int8(&input);
				srl_log_printf(DEBUG, "data %02x\n", marker);
				if (marker == 0xff) {
					pot_mark = block_io_read_int8(&input);
					if (pot_mark == 0) {
						block_io_write_int8(&output, 0xff);
					} else {
						mark = 0xff00|pot_mark;
						srl_log_printf(DEBUG, "found marker %04x while in data\n", mark);
						found_mk = 1;
					}
				} else {
					/* buf isn't 0xff */
					block_io_write_int8(&output, marker);
				}
			}
			//srl_log(TRACE, "flushing data\n");
			in_frame = 0;
			break;

		case EOI_MK:
			block_io_flush(&output);
			//srl_log_printf(TRACE, "picture %d end marker\n", ++end_nb );
			done = 1;
			break;

		case COM_MK:
		  //srl_log(TRACE, "comments marker, skipping\n");
			skip_segment(&input);
			break;
		default:
			if ( (mark&MK_MSK) == APP_MK ) {
			  //srl_log(TRACE, "application data marker, skipping\n");
				skip_segment(&input);
				break;
			}
			if (RST_MK(mark)) {
			  //srl_log(TRACE, "found RST marker\n");
				break;
			}
			done = 1;
			++end_nb;
			break;
		}
	}
	}
}

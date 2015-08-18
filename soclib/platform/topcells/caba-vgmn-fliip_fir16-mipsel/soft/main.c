
#include "stdio.h"
#include "stdlib.h"
#include "system.h"
#include "soclib_io.h"
#include "soclib/dma_regs.h"

#include "../segmentation.h"

#define DO_FILTER

static inline void set_ctrl(void * base,
		unsigned int addr, unsigned int data) {
	soclib_io_set(base, CTRL_ADDR_REG, addr);
	soclib_io_set(base, CTRL_DATA_REG, data);
}

signed short image[256*256] = {
#include "image.dat"
};

signed short dst_buffer[256*256];

static void wait_dma() {
	while (1) {
		unsigned int t1, t2;
		if (soclib_io_get(base(IP), DMA_START_REG) != 1) {
			break;
		}
		// wait a little before trying again
		for (t1 = 0; t1 < 0xFFFF; ++t1) {
			for (t2 = 0; t2 < 0xFFFF; ++t2) {
				;
			}
		}
	}
	printf("End of dma transferts\n\n");
}

static void configure_and_start_dma(unsigned int size,
		unsigned int from, unsigned int to) {

	printf("Configure DMA controller\n\n");
	soclib_io_set(base(IP), DMA_RESET_REG, 0);

	// Read configuration

	soclib_io_set(base(IP), DMA_INFO_REG, DMA_INFO(DMA_READ, DMA_MEM_CONTIG, 0));
	soclib_io_set(base(IP), DMA_MEM_REG, from);
	soclib_io_set(base(IP), DMA_PHASE_REG, 0);
	soclib_io_set(base(IP), DMA_LENGTH_REG, (size*2)>>2);

	soclib_io_set(base(IP), DMA_READ_LOOP_REG,
			DMA_LOOP_COUNT_STRIDE(1, 0));

	// Write configuration

	soclib_io_set(base(IP), DMA_INFO_REG, DMA_INFO(DMA_WRITE, DMA_MEM_CONTIG, 0));
	soclib_io_set(base(IP), DMA_MEM_REG, to);
	soclib_io_set(base(IP), DMA_PHASE_REG, 0);
	soclib_io_set(base(IP), DMA_LENGTH_REG, (size*2)>>2);

	soclib_io_set(base(IP), DMA_WRITE_LOOP_REG,
			DMA_LOOP_COUNT_STRIDE(1, 0));

	soclib_io_set(base(IP), DMA_START_REG, 0 /* unused */);
}


int main(void) {

	uint8_t *fb = FB_BASE;
	unsigned int index = 0;
	const unsigned int width = 256;
	const unsigned int height= 256;
	const unsigned int size = width * height;

	unsigned int i, j;

#ifdef DO_FILTER
	printf("Apply filter\n\n");
	configure_and_start_dma(size, (unsigned int)image, (unsigned int)dst_buffer);
	wait_dma();
#endif

	printf("Copy image to frame buffer\n\n");
	index = 0;
	for (j = 0; j < height; ++j) {
		for (i = 0; i < width; ++i) {
#ifdef DO_FILTER
			// division by 8 is to normalize
			// the output of the FIR
			*fb++ = dst_buffer[index++]>>3;
#else
			*fb++ = image[index++];
#endif
		}
	}
	printf("End\n\n");

	printf("cycles %d\n", run_cycles());

	exit(0);

	return 0;
}

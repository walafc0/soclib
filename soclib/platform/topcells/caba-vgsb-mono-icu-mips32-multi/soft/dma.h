#ifndef DMA_REGS_H
#define DMA_REGS_H

enum SoCLibDmaRegisters {
   	DMA_SRC         = 0,
   	DMA_DST         = 1,
   	DMA_LEN         = 2,
   	DMA_RESET       = 3,
   	DMA_IRQ_DISABLE = 4,
	};

enum DmaStatusValues {
        DMA_IDLE        = 0,     
	DMA_SUCCESS     = 1,
	DMA_READ_ERROR  = 2,
	DMA_WRITE_ERROR	= 3,
        };

#endif 



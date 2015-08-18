/****************************************************************************************
	File : syscalls.c
 	Written by Alain Greiner & Nicolas Pouillon
	Date : november 2009

	Basic System Calls for peripheral access.
	These system calls are used by the MIPS GIET, that is running
	on the MIPS32 processor architecture.

	The supported peripherals are:
	- the SoClib pibus_multi_tty
	- the SocLib pibus_timer
	- the SocLib pibus_dma
	- The SoCLib pibus_icu
	- The SoClib pibus_locks
	- The SoCLib pibus_gcd
	- The SoCLib pibus_frame_buffer
	- The SoCLib pibus_block_device

	The NB_NTASKS / NB_PROCS / NB_LOCKS / NB_TIMERS / NB_TTYS
	parameters must be defined in the ldscript.
	
****************************************************************************************/

#include "syscalls.h"
#include "icu.h"
#include "block_device.h"
#include "dma.h"

struct plouf;

extern struct plouf seg_icu_base;
extern struct plouf seg_timer_base;
extern struct plouf seg_tty_base;
extern struct plouf seg_gcd_base;
extern struct plouf seg_dma_base;
extern struct plouf seg_locks_base;
extern struct plouf seg_fb_base;
extern struct plouf seg_ioc_base;

extern struct plouf NB_PROCS;
extern struct plouf NB_TASKS;
extern struct plouf NB_TIMERS;
extern struct plouf NB_TTYS;
extern struct plouf NB_LOCKS;

#define in_syscalls __attribute__((section (".syscalls")))
#define in_kdata    __attribute__((section (".kdata")))

////////////////////////////////////////////////////////////////////////////////////////
//	Global variables used for synchronization between syscalls and ISRs
////////////////////////////////////////////////////////////////////////////////////////

in_kdata int	volatile	_dma_busy  = 0;
in_kdata int	volatile	_dma_status = 0;

in_kdata int	volatile	_ioc_busy  = 0;
in_kdata int	volatile	_ioc_status = 0;

in_kdata char	volatile	_tty_get_buf[16];
in_kdata int	volatile	_tty_get_full[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
in_kdata char	volatile	_tty_put_buf[16];
in_kdata int	volatile	_tty_put_full[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

////////////////////////////////////////////////////////////////////////////////////////
//	_procid()
// Access CP0 and returns processor ident
////////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _procid()
{
	int ret;
	asm volatile( "mfc0 %0, $15, 1": "=r"(ret) );
	return (ret & 0x7);
} 
////////////////////////////////////////////////////////////////////////////////////////
//	_proctime()
// Access CP0 and returns processor time
////////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _proctime()
{
	int ret;
	asm volatile( "mfc0 %0, $9": "=r"(ret) );
	return ret;
} 
////////////////////////////////////////////////////////////////////////////////////////
//	_it_mask()
// Access CP0 and mask IRQs
////////////////////////////////////////////////////////////////////////////////////////
in_syscalls void _it_mask()
{
	int tmp;
        asm volatile("mfc0	%0, $12" 	: "=r" (tmp) );
	asm volatile("ori	%0, %0, 1"	: "=r" (tmp) );
	asm volatile("mtc0	%0, $12" 	: "=r" (tmp) );	
} 
////////////////////////////////////////////////////////////////////////////////////////
//	_it_enable()
// Access CP0 and enable IRQs
////////////////////////////////////////////////////////////////////////////////////////
in_syscalls void _it_enable()
{
	int tmp;
        asm volatile("mfc0	%0, $12" 	: "=r" (tmp) );
	asm volatile("addi	%0, %0, -1"	: "=r" (tmp) );
	asm volatile("mtc0	%0, $12" 	: "=r" (tmp) );	
} 
//////////////////////////////////////////////////////////////////////
// 	_dcache_buf_invalidate()
// Invalidate all cache lines corresponding to a memory buffer.
// This is used by the block_device driver.
/////////////////////////////////////////////////////////////////////////
in_syscalls void _dcache_buf_invalidate(const void * buffer, size_t size)
{
	size_t i;
	size_t dcache_line_size;

	// retrieve dcache line size from config register (bits 12:10)
	asm volatile("mfc0 %0, $16, 1" : "=r" (dcache_line_size));

	dcache_line_size = 2 << ((dcache_line_size>>10) & 0x7);

	// iterate on lines to invalidate each one of them
	for ( i=0; i<size; i+=dcache_line_size )
	  asm volatile(" cache %0, %1"
		       :
		       :"i" (0x11), "R" (*((char*)buffer+i)));
}

/////////////////////////////////////////////////////////////////////////
// 	_itoa_dec()
// convert a 32 bits unsigned int to a string of 10 decimal characters.
/////////////////////////////////////////////////////////////////////////
in_syscalls void _itoa_dec(unsigned val, char* buf)
{
    const char	DecTab[] = "0123456789";
    unsigned int i;
    for( i=0 ; i<10 ; i++ )
    {
        if( (val!=0) || (i==0) ) buf[9-i] = DecTab[val % 10];
        else	                 buf[9-i] = 0x20;
        val /= 10;
    }
}
//////////////////////////////////////////////////////////////////////////
// 	_itoa_hex()
// convert a 32 bits unsigned int to a string of 8 hexadecimal characters.
///////////////////////////////////////////////////////////////////////////
in_syscalls void _itoa_hex(int val, char* buf)
{
    const char	HexaTab[] = "0123456789ABCD";
    unsigned int i;
    for( i=0 ; i<8 ; i++ )
    {
        buf[7-i] = HexaTab[val % 16];
        val /= 16;
    }
}
///////////////////////////////////////////////////////////////////////////////////////
//	_timer_write()
// Write a 32 bits word in a memory mapped register of the VCI_MULTI_TIMER
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _timer_write(	int 	timer_index,
				int	register_index,
				int	value)
{
	volatile int*	timer_address;
	int		max;

	max = (int)&NB_TIMERS;
	if( (timer_index >= max) || (register_index >= 4) ) return -1;

	timer_address = (int*)(void*)&seg_timer_base + (timer_index*TIMER_SPAN);
	timer_address[register_index] = value;			// write word
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_timer_read()
// Read a 32 bits word in a memory mapped register of the VCI_MULTI_TIMER
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _timer_read(	int 	timer_index,
				int	register_index,
				int*	buffer)
{
	volatile int*	timer_address;
	int		max;

	max = (int)&NB_TIMERS;
	if( (timer_index >= max) || (register_index >= 4) ) return -1;

	timer_address = (int*)(void*)&seg_timer_base + (timer_index*TIMER_SPAN);
	*buffer = timer_address[register_index];		// read word
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_tty_write()
// Write one or several characters directly from a fixed length user buffer 
// to the TTY_WRITE[tty_index] register of the TTY controler.
// The tty_index value is computed as (proc_index*NB_TASKS + task_index)
// It doesn't use the IRQ_PUT interrupt and the associated kernel buffer.
// This is a non blocking call : it test the TTY_STATUS register. 
// If the TTY_STATUS_WRITE bit is set, the transfer stops and the function
// returns  the number of characters that have been actually written.
// It returns -1 in case of error (proc_index too large)
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _tty_write(	int 	proc_index,
				char*	buffer,
				int	length)
{
    int		max_tasks = (int)&NB_TASKS;
    int		max_procs = (int)&NB_PROCS;
    int		i;
    int		nwritten = 0;
    int		tty_index;
    int		task_index;
    char*	tty_address;

    if( proc_index >= max_procs) 	return -1;
    task_index = _current_task_array[proc_index];
    if( task_index >= max_tasks ) 	return -1;
    tty_index = proc_index*max_tasks + task_index;
    tty_address = (char*)(void*)&seg_tty_base + (tty_index*16);

    for ( i=0 ; i < length ; i++ )  
    {
        if((tty_address[4] & 0x2) == 0x2)  break;
        else
        {
            tty_address[0] = buffer[i];	// write character
            nwritten++;
        }
    }
    return nwritten;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_tty_read_irq()
// Fetch one character from the _tty_get_buf[tty_index] kernel buffer 
// and writes this character to the user buffer.
// The tty_index value is computed as (proc_index*NB_TASKS + task_index)
// The _tty_get_full[proc_index] is reset.
// It uses the IRQ_GET interrupt and the associated kernel buffer, that has been 
// written by the ISR.
// This is a non blocking call : it returns 0 if the kernel buffer is empty,
// and returns 1 if the buffer is full.
// It returns -1 in case of error (proc_index too large or length != 1)
// The length argument is not used in this implementation, and has been
// introduced for future implementations.
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _tty_read_irq(	int 	proc_index,
				char*	buffer,
				int	length)
{
    int			max_tasks = (int)&NB_TASKS;
    int			max_procs = (int)&NB_PROCS;
    int			tty_index;
    int 		task_index;

    if( proc_index >= max_procs ) 	return -1;
    if( length != 1)			return -1;
    task_index = _current_task_array[proc_index];
    if( task_index >= max_tasks )	return -1;
    tty_index = proc_index*max_tasks + task_index;
    if( _tty_get_full[tty_index] == 0 )	return 0;
    
    *buffer = _tty_get_buf[tty_index];
    _tty_get_full[tty_index] = 0;
    return 1;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_tty_read()
// Fetch one character directly from the TTY_READ[tty_index] register of the TTY 
// controler, and writes this character to the user buffer.
// The tty_index value is computed as (proc_index*NB_TASKS + task_index)
// It doesn't use the IRQ_GET interrupt and the associated kernel buffer.
// This is a non blocking call : it returns 0 if the register is empty,
// and returns 1 if the register is full.
// It returns -1 in case of error (proc_index too large or length != 1)
// The length argument is not used in this implementation, and has been
// introduced for future implementations.
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _tty_read(int 	proc_index,
				char*	buffer,
				int	length)
{
    int		max_tasks = (int)&NB_TASKS;
    int		max_procs = (int)&NB_PROCS;
    int		tty_index;
    int 	task_index;
    char*	tty_address;

    if( proc_index >= max_procs ) 	return -1;
    if( length != 1)			return -1;
    task_index = _current_task_array[proc_index];
    if( task_index >= max_tasks )	return -1;
    tty_index = proc_index*max_tasks + task_index;

    tty_address = (char*)(void*)&seg_tty_base + (tty_index*16);
    
    if((tty_address[4] & 0x1) == 0x1)
    {
        buffer[0] = tty_address[8];
        return 1;
    }
    else
    {
        return 0;
    }
}
///////////////////////////////////////////////////////////////////////////////////////
//	_exit()
// Exit (suicide) after printing message on  a TTY terminal.
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int	_exit()
{
    char buf[] = "\n\n!!!  Exit  Processor          !!!\n";
    int	pid = _procid();
    buf[24] = '0';
    buf[25] = 'x';
    buf[26] = (char)((pid>>8) & 0xF) + 0x30;
    buf[27] = (char)((pid>>4) & 0xF) + 0x30;
    buf[28] = (char)(pid & 0xF)      + 0x30;
    _tty_write(pid, buf, 36);
    while(1) asm volatile("nop");	// infinite loop...
}

///////////////////////////////////////////////////////////////////////////////////////
//	_icu_write()
// Write a 32 bits word in a memory mapped register of the ICU peripheral
// The base address is defined by the processor ID
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _icu_write(	int 	register_index,
				int	value)
{
    volatile	int*	icu_address;
    int processor_id = _procid();
    if( register_index >= 5 ) return -1;
    if( processor_id   >= 8 ) return -1;

    icu_address = (int*)(void*)&seg_icu_base;
    icu_address[8*processor_id + register_index] = value;	// write word
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_icu_read()
// Read a 32 bits word in a memory mapped register of the ICU peripheral
// The ICU base address is defined by the processor ID
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _icu_read(	int 	register_index,
				int*	buffer)
{
    volatile	int*	icu_address;
    int processor_id = _procid();
    if( register_index >= 5 ) return -1;
    if( processor_id   >= 8 ) return -1;

    icu_address = (int*)(void*)&seg_icu_base;
    *buffer = icu_address[8*processor_id + register_index];	// read word
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_gcd_write()
// Write a 32 bits word in a memory mapped register of the GCD coprocessor
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _gcd_write(	int 	register_index,
				int	value)
{
	volatile	int*	gcd_address;
	if( register_index >= 4 ) return -1;

	gcd_address = (int*)(void*)&seg_gcd_base;
	gcd_address[register_index] = value;			// write word
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_gcd_read()
// Read a 32 bits word in a memory mapped register of the GCD coprocessor
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _gcd_read(	int 	register_index,
				int*	buffer)
{
	volatile	int*	gcd_address;
	if( register_index >= 4 ) return -1;

	gcd_address = (int*)(void*)&seg_gcd_base;
	*buffer = gcd_address[register_index];			// read word
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_locks_write()
// Release a spin-lock in the LOCKS peripheral
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _locks_write(	int 	lock_index)
			
{
	volatile	int*	locks_address;
	int		max;

	max = (int)&NB_LOCKS;
	if( lock_index >= max ) return -1;

	locks_address = (int*)(void*)&seg_locks_base;
	locks_address[lock_index] = 0;				// write 0
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_locks_read()
// Try to take a spin-lock in the LOCKS peripheral.
// This is a blocking call, as there is a busy-waiting loop,
// until the lock is granted to the requester.
// There is an internal delay of about 100 cycles between
// two successive lock read, to avoid bus saturation.
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _locks_read(	int 	lock_index)
{
    volatile int*	locks_address;
    int			max;
    int			delay;
    int			i;

    max = (int)&NB_LOCKS;
    if( lock_index >= max ) return -1;

    locks_address = (int*)(void*)&seg_locks_base;

    while( locks_address[lock_index] != 0) 
    {
        delay = (_proctime() & 0xF) << 4; 
        for( i=0 ; i<delay ; i++)	// busy waiting
        {		                // with a pseudo random
            asm volatile("nop");	// delay between bus accesses
        }
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//	I/O BLOCK_DEVICE 
// The three syscalls defined below use a polling policy to test
// the global variable _ioc_busy and guaranty one single simultaneous transfer,
// and detect transfer completion. The _ioc_busy variable is set by the _ioc_write
// or _ioc_read functions, and it is reset by the ISR associated to the IOC IRQ.
// Errors related to the user buffer address are immediately reported by the _ioc_write 
// and _ioc_read functions. 
// Errors during the transfer are signaled by the ISR in the _ioc_status variable, 
// and reported by the _ioc_completed function.
// The _ioc_write and _ioc_read functions are blocking, polling the _ioc_busy variable
// until the device is available.
// In a multi-processing environment, this polling policy must be replaced by a 
// descheduling policy for the requesting process, and accesses to the _ioc_busy
// variable must be implemented as a lock.
///////////////////////////////////////////////////////////////////////////////////////
//	_ioc_write()
// Transfer data from a memory buffer to a file on the block_device.
// - lba 	: first block index on the disk
// - buffer	: base address of the memory buffer
// - count	: number of blocks to be transfered
// The source buffer must be in user address space.
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _ioc_write(	size_t	lba,
				void*	buffer,
				size_t	count)
{
    volatile	int*	ioc_address = (int*)(void*)&seg_ioc_base;
    int			delay;
    int			i;

    // buffer must be in user space
    size_t block_size = ioc_address[BLOCK_DEVICE_BLOCK_SIZE];
    if( ( (size_t)buffer + block_size*count ) >= 0x80000000 ) return -1;
    if( ( (size_t)buffer                    ) >= 0x80000000 ) return -1;

    // polling until block_device is available
    while (_ioc_busy != 0) 
    {
        delay = (_proctime() & 0xF) << 4; 
        for( i=0 ; i<delay ; i++)	// busy waiting
        {		                // with a pseudo random
            asm volatile("nop");	// delay between bus accesses
        }
    }	
    _ioc_busy  = 1;

    // block_device configuration
    ioc_address[BLOCK_DEVICE_BUFFER] = (int)buffer;
    ioc_address[BLOCK_DEVICE_COUNT] = count;
    ioc_address[BLOCK_DEVICE_LBA] = lba;
    ioc_address[BLOCK_DEVICE_IRQ_ENABLE] = 1;	
    ioc_address[BLOCK_DEVICE_OP] = BLOCK_DEVICE_WRITE;	
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_ioc_read()
// Transfer data from a file on the block device to a memory buffer.
// - lba 	: first block index on the disk
// - buffer	: base address of the memory buffer
// - count	: number of blocks to be transfered
// The destination buffer must be in user address space.
// All cache lines corresponding to the the target buffer must be invalidated
// for cache coherence.
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _ioc_read(	size_t 	lba,
				void*	buffer,
				size_t	count)
{
    volatile	int*	ioc_address = (int*)(void*)&seg_ioc_base;
    int		delay;
    int		i;

    // buffer must be in user space
    size_t block_size = ioc_address[BLOCK_DEVICE_BLOCK_SIZE];
    if( ( (size_t)buffer + block_size*count ) >= 0x80000000 ) return -1;
    if( ( (size_t)buffer                    ) >= 0x80000000 ) return -1;

    // polling until block_device is available
    while (_ioc_busy != 0) 
    {
        delay = (_proctime() & 0xF) << 4; 
        for( i=0 ; i<delay ; i++)	// busy waiting
        {		                // with a pseudo random
            asm volatile("nop");	// delay between bus accesses
        }
    }	
    _ioc_busy  = 1;
        
    // block_device configuration
    ioc_address[BLOCK_DEVICE_BUFFER] = (int)buffer;
    ioc_address[BLOCK_DEVICE_COUNT] = count;
    ioc_address[BLOCK_DEVICE_LBA] = lba;
    ioc_address[BLOCK_DEVICE_IRQ_ENABLE] = 1;	
    ioc_address[BLOCK_DEVICE_OP] = BLOCK_DEVICE_READ;

    //_dcache_buf_invalidate(buffer, block_size*count);

    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_ioc_completed()
// This blocking function cheks completion of an I/O transfer and reports errors.
// As this a blocking call, the processor is stalled in  wait instruction
// until the next interrupt.
// It returns 0 if the tarnsfer is successfully completed.
// It returns -1 if an error has been reported.
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _ioc_completed()
{
    while (_ioc_busy != 0) 
    {
        asm volatile("nop");
    }	
    if((_ioc_status != BLOCK_DEVICE_READ_SUCCESS) && 
       (_ioc_status != BLOCK_DEVICE_WRITE_SUCCESS)) 	return -1;
    else						return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//	FRAME_BUFFER
// The _fb_sync_write & _fb_sync_read syscalls use a memcpy strategy to implement the
// transfer between a data buffer (user space) and the frame buffer (kernel space).
// They are blocking until completion of the transfer.
//////////////////////////////////////////////////////////////////////////////////////
//	_fb_sync_write()
// Transfer data from an user buffer to the frame_buffer device with a memcpy.
// - offset 	: offset (in bytes) in the frame buffer
// - buffer	: base address of the memory buffer
// - length	: number of bytes to be transfered
//////////////////////////////////////////////////////////////////////////////////////
in_syscalls int	_fb_sync_write(	size_t	offset,
				void*	buffer,
				size_t	length)
{
	volatile char* 	fb = (char*)(void*)&seg_fb_base + offset;
	char*		ub = buffer;
	size_t		i;

	// buffer must be in user space
	if( ( (size_t)buffer + length ) >= 0x80000000 ) return -1;
	if( ( (size_t)buffer          ) >= 0x80000000 ) return -1;

	// memory copy 
	for(i=0 ; i<length ; i++) fb[i] = ub[i];
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_fb_sync_read()
// Transfer data from the frame_buffer device to an user buffer with a memcpy.
// - offset 	: offset (in bytes) in the frame buffer
// - buffer	: base address of the memory buffer
// - length	: number of bytes to be transfered
//////////////////////////////////////////////////////////////////////////////////////
in_syscalls int	_fb_sync_read(	size_t	offset,
				void*	buffer,
				size_t	length)
{
	volatile char* 	fb = (char*)(void*)&seg_fb_base + offset;
	char*		ub = buffer;
	size_t		i;

	// buffer must be in user space
	if( ( (size_t)buffer + length ) >= 0x80000000 ) return -1;
	if( ( (size_t)buffer          ) >= 0x80000000 ) return -1;

	// memory copy
	for(i=0 ; i<length ; i++) ub[i] = fb[i];
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
// The _fb_write(), _fb_read() & _fb_completed() syscalls use the DMA 
// coprocessor to transfer data between the user buffer and the frame buffer.
// Similarly to the block_device, these three syscalls use a polling policy to test
// the global variable _dma_busy and guaranty one single simultaneous transfer,
// or detect transfer completion. The _dma_busy variable is reset by the ISR
// associated to the DMA device IRQ.
// In a multi-processing environment, this polling policy must be replaced by a 
// descheduling policyy for the requesting process, and accesses to the _dma_busy
// variable must be implemented as a lock.
///////////////////////////////////////////////////////////////////////////////////////
//	_fb_write()
// Transfer data from an user buffer to the frame_buffer device using DMA.
// - offset 	: offset (in bytes) in the frame buffer
// - buffer	: base address of the memory buffer
// - length	: number of bytes to be transfered
//////////////////////////////////////////////////////////////////////////////////////
in_syscalls int	_fb_write( 	size_t	offset,
				void*	buffer,
				size_t	length)
{
    volatile char* 	fb  = (char*)(void*)&seg_fb_base + offset;
    volatile int* 	dma = (int*)(void*)&seg_dma_base;
    int			delay;
    int			i;

    // checking buffer boundaries (bytes)
    if( ( (size_t)buffer + length ) >= 0x80000000 ) return -1;
    if( ( (size_t)buffer          ) >= 0x80000000 ) return -1;

    // waiting until DMA device is available
    while (_dma_busy != 0) 
    {
        delay = (_proctime() & 0xF) << 4; 
        for( i=0 ; i<delay ; i++)	// busy waiting
        {		                // with a pseudo random
            asm volatile("nop");	// delay between bus accesses
        }
	asm volatile("nop");
    }	
    _dma_busy = 1;

    // DMA configuration
    dma[DMA_IRQ_DISABLE] = 0; 
    dma[DMA_SRC] = (int)buffer;
    dma[DMA_DST] = (int)fb;
    dma[DMA_LEN] = (int)length>>2;
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_fb_read()
// Transfer data from the frame_buffer device to an user buffer using DMA.
// - offset 	: offset (in bytes) in the frame buffer
// - buffer	: base address of the memory buffer
// - length	: number of bytes to be transfered
//////////////////////////////////////////////////////////////////////////////////////
in_syscalls int	_fb_read( 	size_t	offset,
				void*	buffer,
				size_t	length)
{
    volatile char* 	fb  = (char*)(void*)&seg_fb_base + offset;
    volatile int* 	dma = (int*)(void*)&seg_dma_base;
    int			delay;
    int			i;

    // checking buffer boundaries (bytes)
    if( ( (size_t)buffer + length ) >= 0x80000000 ) return -1;
    if( ( (size_t)buffer          ) >= 0x80000000 ) return -1;

    // waiting until DMA device is available
    while (_dma_busy != 0) 
    {
        delay = (_proctime() & 0xF) << 4; 
        for( i=0 ; i<delay ; i++)	// busy waiting
        {		                // with a pseudo random
            asm volatile("nop");	// delay between bus accesses
        }
    }	
    _dma_busy = 1;

    // DMA configuration
    dma[DMA_IRQ_DISABLE] = 0; 
    dma[DMA_SRC] = (int)fb;
    dma[DMA_DST] = (int)buffer;
    dma[DMA_LEN] = (int)length>>2;

    _dcache_buf_invalidate(buffer, length);

    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//	_fb_completed()
// This blocking function cheks completion of a DMA transfer to or fom the frame buffer.
// The MIPS32 wait instruction stall the processor until the next interrupt.
// It returns 0 if the transfer is successfully completed
// It returns -1 if an error has been reported.
///////////////////////////////////////////////////////////////////////////////////////
in_syscalls int _fb_completed()
{
    while (_dma_busy != 0) 
    {
	asm volatile("nop");
    }	
    if(_dma_status == DMA_SUCCESS)	return 0;
    else				return -1;
}
//////////////////////////////////////////////////////////////////////////////////////

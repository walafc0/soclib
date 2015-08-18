/*********************************************************************************
	fichier stdio.h  
	Written Alain greiner & Nicolas Pouillon   
	Date : 15/09/2009
*********************************************************************************/

#ifndef _STDIO_H_
#define _STDIO_H_

#define SYSCALL_PROCID		0x00
#define SYSCALL_PROCTIME	0x01
#define SYSCALL_TTY_WRITE	0x02
#define SYSCALL_TTY_READ 	0x03
#define SYSCALL_TIMER_WRITE	0x04
#define SYSCALL_TIMER_READ 	0x05
#define SYSCALL_GCD_WRITE	0x06
#define SYSCALL_GCD_READ	0x07
#define SYSCALL_ICU_WRITE	0x08
#define SYSCALL_ICU_READ	0x09
#define SYSCALL_TTY_READ_IRQ    0x0A
#define SYSCALL_TTY_WRITE_IRQ   0x0B
#define SYSCALL_LOCKS_WRITE	0x0C
#define SYSCALL_LOCKS_READ	0x0D
#define SYSCALL_EXIT    	0x0E

#define SYSCALL_FB_SYNC_WRITE   0x10
#define SYSCALL_FB_SYNC_READ    0x11
#define SYSCALL_FB_WRITE   	0x12
#define SYSCALL_FB_READ    	0x13
#define SYSCALL_FB_COMPLETED	0x14
#define SYSCALL_IOC_WRITE   	0x15
#define SYSCALL_IOC_READ    	0x16
#define SYSCALL_IOC_COMPLETED	0x17

typedef unsigned int	size_t;

/****************************************************************
this is a generic C function to implement all system calls.
- The first argument is the system call index.
- The four next arguments are the system call arguments.
They will be written in registers $2, $4, $5, $6, $7.
****************************************************************/
int    sys_call(int call_no, 
		int arg_o, 
		int arg_1, 
		int arg_2, 
		int arg_3);

/****************************************************************
These functions access the MIPS protected registers
****************************************************************/
int 	procid();
int 	proctime();
int 	exit();
int	rand();

/****************************************************************
These functions access the MULTI_TTY peripheral
****************************************************************/
int 	tty_puts(char* string);
int  	tty_putc(char byte);
int 	tty_putw(int word);
int 	tty_gets(char* buf, int bufsize);
int 	tty_getc(char* byte);
int 	tty_getw(int* word);
int	tty_printf(char* format,...);

/****************************************************************
These functions access the MULTI_TIMER peripheral
****************************************************************/
int	timer_set_mode(int timer_index, int mode);
int	timer_set_period(int timer_index, int period);
int	timer_reset_irq(int timer_index);
int	timer_get_time(int timer_index, int* time);

/****************************************************************
These functions access the GCD peripheral
****************************************************************/
int	gcd_set_opa(int val);
int	gcd_set_opb(int val);
int	gcd_start();
int	gcd_get_result(int* val);
int	gcd_get_status(int* val);

/****************************************************************
These functions access the ICU peripheral
****************************************************************/
int	icu_set_mask(int val);
int	icu_clear_mask(int val);
int	icu_get_mask(int* buffer);
int	icu_get_irqs(int* buffer);
int	icu_get_index(int* buffer);
		
/****************************************************************
These functions access the LOCKS peripheral
****************************************************************/
int	lock_acquire(int lock_index);
int	lock_release(int lock_index);

/****************************************************************
These functions access the BLOCK_DEVICE peripheral
****************************************************************/
int	ioc_read(size_t lba, void* buffer, size_t count);
int	ioc_write(size_t lba, void* buffer, size_t count);
int	ioc_completed();

/****************************************************************
These functions access the FRAME_BUFFER peripheral
****************************************************************/
int	fb_read(size_t offset, void* buffer, size_t length);
int	fb_write(size_t offset, void* buffer, size_t length);
int	fb_completed();
int	fb_sync_read(size_t offset, void* buffer, size_t length);
int	fb_sync_write(size_t offset, void* buffer, size_t length);

#endif

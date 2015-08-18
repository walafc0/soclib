/*********************************************************************
	fichier stdio.c  
	Written Alain greiner & Nicolas Pouillon   
	Date : 19/10/2009

These function implement the drivers for the SoCLib peripherals.
*********************************************************************/

#include <stdarg.h>

#include "stdio.h"

#include "timer.h"
#include "tty.h"
#include "gcd.h"
#include "icu.h"
#include "dma.h"
#include "block_device.h"

/*********************************************************************
We define a generic C function to implement all system calls.
*********************************************************************/
inline int  sys_call(	int call_no,
			int arg_0, 
			int arg_1, 
			int arg_2, 
			int arg_3)
{
	register	int	reg_no_and_output asm("v0") = call_no;
	register	int	reg_a0 asm("a0") = arg_0;
	register	int	reg_a1 asm("a1") = arg_1;
	register	int	reg_a2 asm("a2") = arg_2;
	register	int	reg_a3 asm("a3") = arg_3;

	asm volatile(
		"syscall"
		: "=r" (reg_no_and_output) 	// arguments de sortie
		: "r" (reg_a0),			// arguments d'entrée
		  "r" (reg_a1),
		  "r" (reg_a2), 
		  "r" (reg_a3),
	          "r" (reg_no_and_output)
		: "memory", 			// ressources modifiees:
		   "at",
		   "v1",
		   "ra",			// Ces registres persistants seront sauvegardes
		   "t0",			// sur la pile par le compilateur
		   "t1",			// seulement s'ils contiennent des donnees
		   "t2",			// calculees par la fonction effectuant le syscall,
		   "t3",			// et que ces valeurs sont reutilisees par cette
		   "t4",			// fonction au retour du syscall.
		   "t5",
		   "t6",
		   "t7",
		   "t8",
		   "t9"
	);
	return reg_no_and_output;
}

/********************************************************************
	procid()     
Returns the processor ident.
********************************************************************/
int 	procid()
{
	return	sys_call(SYSCALL_PROCID, 0, 0, 0, 0);
}
/********************************************************************
	proctime()
Returns the local processor time.
********************************************************************/
int	proctime()
{
	return sys_call(SYSCALL_PROCTIME, 0, 0, 0, 0);
}
/********************************************************************
	exit()     
Exit the program with a TTY message, and enter an infinite loop...
********************************************************************/
int	exit()
{
	int	proc_index = procid();
	return  sys_call(SYSCALL_EXIT, proc_index, 0, 0, 0);
}
/********************************************************************
	rand()
Returns a pseudo-random value derived from the processor cycle count.
This value is comprised between 0 & 65535.
********************************************************************/
int	rand()
{
	int	x = sys_call(SYSCALL_PROCTIME, 0, 0, 0, 0);
	if((x & 0xF) > 7) 	return (x*x & 0xFFFF);
	else			return (x*x*x & 0xFFFF);
}

/********************************************************************
	MULTI-TTY
*********************************************************************
	tty_puts()
Display a string on a terminal.
The terminal index is implicitely defined by the processor ID.
(and by the task ID in case of multi-tasking)
The string must be terminated by a NUL character.
It doesn't use the IRQ_PUT interrupt, and the associated kernel buffer.
This function returns 0 in case of success.
********************************************************************/
int 	tty_puts(char* string)
{
	int	proc_index = procid();
	int 	length = 0;
	while ( string[length] != 0) {
		length++;
	}
	return	sys_call(SYSCALL_TTY_WRITE, 
		proc_index,
		(int)string,
		length,
		0);
}
/********************************************************************
	tty_putc()
Display a single ascii character on a terminal.
The terminal index is implicitely defined by the processor ID.
(and by the task ID in case of multi-tasking)
It doesn't use the IRQ_PUT interrupt, and the associated kernel buffer.
This function returns 0 in case of success.
********************************************************************/
int  	tty_putc(char byte)
{
	int	proc_index = procid();
	return sys_call(SYSCALL_TTY_WRITE, 
		proc_index,
		(int)(&byte),
		1,
		0);
}
/********************************************************************
	tty_putw()
Display the value of a 32 bits word (decimal characters).
The terminal index is implicitely defined by the processor ID.
(and by pthe task ID in case of multi-tasking)
It doesn't use the IRQ_PUT interrupt, and the associated kernel buffer.
This function returns 0 in case of success.
********************************************************************/
int  	tty_putw(int val)
{
	int	proc_index = procid();
	char	buf[10];
	int	i;
	for( i=0 ; i<10 ; i++ ) {
		buf[9-i] = (val % 10) + 0x30;
		val = val / 10;
	}
	return sys_call(SYSCALL_TTY_WRITE, 
		proc_index,
		(int)buf,
		10,
		0);
}
/********************************************************************
 	tty_gets()
Fetch a string from a terminal to a bounded length buffer.
The terminal index is implicitely defined by the processor ID.
(and by the task ID in case of multi-tasking)
It uses the IRQ_GET interrupt, anf the associated kernel buffer.
It is a blocking function that returns 0 if a valid string is stored 
in the buffer, and returns -1 in case of error.
Up to (bufsize - 1) characters (including the non printable
characters) will be copied into buffer, and the string is 
always completed by a NUL character.
The <LF> character is interpreted, as the function close
the string with a NUL character if <LF> is read. 
The <DEL> character is interpreted, and the corresponding 
character(s) are removed from the target buffer.
********************************************************************/
int  	tty_gets(char* buf, int bufsize)
{
    int			ret;
    unsigned char	byte;
    unsigned int	index = 0;

    while( index < (bufsize-1) )
    {
        ret = sys_call(SYSCALL_TTY_READ_IRQ,
			procid(),
			(int)(&byte),
			1,
			0);	

        if ((ret < 0) || (ret > 1)) return -1;	// return error

        else if ( ret == 1 )			// valid character
        {
            if ( byte == 0x0A )	break; // LF
            else if ((byte == 0x7F) && (index>0)) index--; // DEL
            else
            {
                buf[index] = byte;
                index++;
            }
        }
    } // end while
    buf[index] = 0;
    return 0;		// return ok
}
/********************************************************************
 	tty_getc()
Fetch a single ascii character from a terminal.
The terminal index is implicitely defined by the processor ID.
(and by the task ID in case of multi-tasking)
It doesn't use the IRQ_GET interrupt, and the associated kernel buffer.
It is a blocking function that returns 0 if a valid char is stored 
in the buffer, and returns -1 in case of error.
********************************************************************/
int 	tty_getc(char* buf)
{
    int 		ret;
    unsigned int	done = 0;

    while( done == 0 )
    {
        ret = sys_call(SYSCALL_TTY_READ,
			procid(),
			(int)buf,
			1,
			0);
        if ((ret < 0) || (ret > 1)) return -1;	// return error
        else if ( ret == 1 )	    done =  1; 
    }
    return 0;	// return ok
}
/********************************************************************
	tty_getw()
Fetch a string of decimal characters (most significant digit first)
to build a 32 bits unsigned int. 
The terminal index is implicitely defined by the processor ID.
(and by the task ID in case of multi-tasking)
This is a blocking function that returns 0 if a valid unsigned int 
is stored in the buffer, and returns -1 in case of error.
It uses the IRQ_GET interrupt, anf the associated kernel buffer.
The non-blocking system function _tty_read is called several times,
and the decimal characters are written in a 32 characters buffer
until a <LF> character is read.
The <DEL> character is interpreted, and previous characters can be
cancelled. All others characters are ignored.
When the <LF> character is received, the string is converted to 
an unsigned int value. If the number of decimal digit is too large 
for the 32 bits range, the zero value is returned.
********************************************************************/
int	tty_getw(int* word_buffer)
{
    unsigned char 	buf[32];
    unsigned char	byte;
    unsigned int 	save = 0;
    unsigned int 	val = 0;
    unsigned int	done = 0;
    unsigned int	overflow = 0;
    unsigned int 	max = 0;
    unsigned int 	i;
    int 		ret;

    while(done == 0)
    {
        ret = sys_call(SYSCALL_TTY_READ_IRQ,
			procid(),
			(int)(&byte),
			1,
			0);
        if ((ret < 0) || (ret > 1)) return -1;	// return error
        else if ( ret == 1 )
        {
	    if (( byte > 0x29) && (byte < 0x40))  // decimal character
            {
                buf[max] = byte;
                max++;
                tty_putc(byte);
            }
            else if ( byte == 0x0A )		// LF character
            {
                done = 1;
            }
            else if ( byte == 0x7F )		// DEL character
            {
                if (max > 0) 
                {
                    max--;			// cancel the character
                    tty_putc(0x08);
                    tty_putc(0x20);
                    tty_putc(0x08);
                }
            }
            if ( max == 32 )			// decimal string overflow
            {
                for( i=0 ; i<max ; i++) 	// cancel the string
                {
                    tty_putc(0x08); 
                    tty_putc(0x20);
                    tty_putc(0x08);
                }
                tty_putc(0x30);
                *word_buffer = 0;			// return 0 value
                return 0;
            }
        }
    } // end while
            
    // string conversion
    for( i=0 ; i<max ; i++ )
    {
        val = val*10 + (buf[i] - 0x30);
        if (val < save) overflow = 1;
        save = val;
    }
    if (overflow == 0)
    {
        *word_buffer = val;		// return decimal value 	
    }
    else
    {
        for( i=0 ; i<max ; i++) 	// cancel the string
        {
            tty_putc(0x08);
            tty_putc(0x20);
            tty_putc(0x08);
        }
        tty_putc(0x30);
        *word_buffer = 0;		// return 0 value
    }
    return 0;	
}
/*********************************************************************
	tty_printf()
This function is a simplified version of the mutek_printf() function.
The terminal index is implicitely defined by the processor ID.
(and by the task ID in case of multi-tasking)
It doesn't use the IRQ_PUT interrupt, anf the associated kernel buffer.
Only a limited number of formats are supported:
	- %d : signed decimal
	- %u : unsigned decimal
	- %x : hexadecimal
	- %c : char
	- %s : string
*********************************************************************/
int 	tty_printf(char *format, ...)
{
	va_list ap;
	va_start(ap, format);

printf_text:

	while (*format) {
      		unsigned int i;
      		for (i = 0; format[i] && format[i] != '%'; i++)
		;
      		if (i) {
			sys_call(SYSCALL_TTY_WRITE,
				procid(),
				(int)format,
				i,0);
	  		format += i;
		}
      		if (*format == '%') {
	  		format++;
	  		goto printf_arguments;
		}
    	} // end while 

	va_end(ap);
    	return 0;

printf_arguments: 

    	{
    	int			val = va_arg(ap, long);
    	char			buf[20];
    	char*			pbuf;	
    	unsigned int		len = 0;
    	static const char	HexaTab[] = "0123456789ABCDEF";
    	unsigned int		i;

    	switch (*format++) {
      	case ('c'): 			// char conversion 
		len = 1;
		buf[0] = val;
		pbuf = buf;
		break;
      	case ('d'): 			// decimal signed integer 
		if (val < 0) {
	    		val = -val;
			sys_call(SYSCALL_TTY_WRITE,
				procid(),
				(int)"-",
				1,0);
	  	}
      	case ('u'): 			// decimal unsigned integer 
        	for( i=0 ; i<10 ; i++) {
        	  	buf[9-i] = HexaTab[val % 10];  
          		if (!(val /= 10)) break;
        	}
		len =  i+1;
		pbuf = &buf[9-i];
		break;
      	case ('x'): 			// hexadecimal integer 
		sys_call(SYSCALL_TTY_WRITE,
			procid(),
			(int)"0x",
			2,0);
        	for( i=0 ; i<8 ; i++) {
          		buf[7-i] = HexaTab[val % 16];  
          		if (!(val /= 16)) break;
        	}
		len =  i+1;
		pbuf = &buf[7-i];
		break;
      	case ('s'):  			// string 
        	{
		char *str = (char*)val;
        	while ( str[len] ) len++;
		pbuf = (char*)val;
        	}	
        	break;
      	default:
		goto printf_text;
      	} // end switch 

	sys_call(SYSCALL_TTY_WRITE,
		procid(),
		(int)pbuf,
		len,0);
    	goto printf_text;
  	}
} // end printf()

/********************************************************************
	MULTI-TIMER
*********************************************************************
	timer_set_mode()
********************************************************************/
int 	timer_set_mode(int timer_index, int val) 
{
	return sys_call(SYSCALL_TIMER_WRITE,
			timer_index, 
			TIMER_MODE,
			val,
			0);
}
/********************************************************************
	timer_set_period()
********************************************************************/
int 	timer_set_period(int timer_index, int val) 
{
	return sys_call(SYSCALL_TIMER_WRITE,
			timer_index,
			TIMER_PERIOD,
			val,
			0);
}
/********************************************************************
	timer_reset_irq()
********************************************************************/
int 	timer_reset_irq(int timer_index) 
{
	return sys_call(SYSCALL_TIMER_WRITE,
			timer_index,
			TIMER_RESETIRQ,
			0, 0);
}
/********************************************************************
	timer_get_time()
********************************************************************/
int	timer_get_time(int timer_index, int* time)
{
	return sys_call(SYSCALL_TIMER_READ,
			timer_index,
			TIMER_VALUE,
			(int)time,
			0);
}

/********************************************************************
	GCD COPROCESSOR
*********************************************************************
	gcd_set_opa(int val)
Set operand A in the GCD (Greater Common Divider) coprocessor.
********************************************************************/
int 	gcd_set_opa(int val)
{
	return sys_call(SYSCALL_GCD_WRITE,
			GCD_OPA,
			val,
			0, 0);
}
/********************************************************************
	gcd_set_opb(int val)
Set operand B in the GCD (Greater Common Divider) coprocessor.
********************************************************************/
int 	gcd_set_opb(int val)
{
	return sys_call(SYSCALL_GCD_WRITE,
			GCD_OPB,
			val,
			0, 0);
}
/********************************************************************
	gcd_start()
Start computation in the GCD (Greater Common Divider) coprocessor.
********************************************************************/
int	gcd_start(int val)
{
	return sys_call(SYSCALL_GCD_WRITE,
			GCD_START,
			0, 0, 0);
}
/********************************************************************
	gcd_get_status(int* val)
Get status fromn the GCD (Greater Common Divider) coprocessor.
The value is nul when the coprocessor is idle (computation completed)
********************************************************************/
int 	gcd_get_status(int* val)
{
	return sys_call(SYSCALL_GCD_READ,
			GCD_STATUS,
			(int)val,
			0, 0);
}
/********************************************************************
	gcd_get_result(int* val)
Get result fromn the GCD (Greater Common Divider) coprocessor.
********************************************************************/
int 	gcd_get_result(int* val)
{
	return sys_call(SYSCALL_GCD_READ,
			GCD_OPA,
			(int)val,
			0, 0);
}

/********************************************************************
	ICU(s)
*********************************************************************
	icu_set_mask()
Set some bits in the Interrupt Enable Mask of the ICU component.
Each bit set in the written word will be set in the Mask Enable.
********************************************************************/
int 	icu_set_mask(int val)
{
	return sys_call(SYSCALL_ICU_WRITE,
		ICU_MASK_SET,
		val,
		0, 0);
}
/********************************************************************
	icu_clear_mask()
Reset some bits in the Interrupt Enable Mask of the ICU component.
Each bit set in the written word will be reset in the Mask Enable.
********************************************************************/
int 	icu_clear_mask(int val)
{
	return sys_call(SYSCALL_ICU_WRITE,
		ICU_MASK_CLEAR,
		val,
		0, 0);
}
/********************************************************************
	icu_get_mask()
Read the Interrupt Enable Mask of the ICU component.
********************************************************************/
int 	icu_get_mask(int* buffer)
{
	return sys_call(SYSCALL_ICU_READ,
		ICU_MASK,
		(int)buffer,
		0, 0);
}
/********************************************************************
	icu_get_irqs()
Read the value of the 32 interrupt lines (IRQ inputs).
********************************************************************/
int 	icu_get_irqs(int* buffer)
{
	return sys_call(SYSCALL_ICU_READ,
		ICU_INT,
		(int)buffer,
		0, 0);
}
/********************************************************************
	icu_get_index()
Read the index of the highest priority active interrupt.
(If no active interrupt, -1 is returned).
********************************************************************/
int 	icu_get_index(int* buffer)
{
	return sys_call(SYSCALL_ICU_READ,
		ICU_IT_VECTOR,
		(int)buffer,
		0, 0);
}

/********************************************************************
	LOCKS
*********************************************************************
	lock_acquire()
This system call performs a spin-lock acquisition.
It is dedicated to the SoCLib LOCKS peripheral.
In case of busy waiting, there is a random delay 
of about 100 cycles between two successive lock read, 
to avoid bus saturation.
********************************************************************/
int	lock_acquire(int lock_index)
{
	return sys_call(SYSCALL_LOCKS_READ,
			lock_index,
			0, 0, 0);
}

/********************************************************************
	lock_release()
You must use this system call to release a spin-lock, 
as the LOCKS peripheral is in the kernel segment.
********************************************************************/
int	lock_release(int lock_index)
{
	return sys_call(SYSCALL_LOCKS_WRITE,
		lock_index,
		0, 0, 0);
}

/********************************************************************
	I/O BLOCK DEVICE
*********************************************************************
	ioc_write()
Transfer data from a memory buffer to a file on the block_device.
- lba        : Logical Block Address (first block index)
- buffer     : base address of the memory buffer
- count      : number of blocks to be transfered
This function returns 0 if the transfert can be done.
It returns -1 if the buffer is not in user address space.
********************************************************************/
int 	ioc_write(size_t lba, void* buffer, size_t count) 
{
	return sys_call(SYSCALL_IOC_WRITE,
			lba,
			(int)buffer,
			count,
			0);
}
/********************************************************************
	ioc_read()
Transfer data from a file on the block_device to a memory buffer.
- lba        : Logical Block Address (first block index)
- buffer     : base address of the memory buffer
- count      : number of blocks to be transfered
This function returns 0 if the transfert can be done.
It returns -1 if the buffer is not in user address space.
********************************************************************/
int 	ioc_read(size_t lba, void* buffer, size_t count) 
{
	return sys_call(SYSCALL_IOC_READ,
			lba,
			(int)buffer,
			count,
			0);
}
/********************************************************************
	ioc_completed()
This blocking function returns 0 when the I/O transfer is 
successfully completed, and returns -1 if an address error
has been detected.
********************************************************************/
int 	ioc_completed()
{
	return sys_call(SYSCALL_IOC_COMPLETED,
			0, 0, 0, 0);
}

/********************************************************************
	FRAME BUFFER
*********************************************************************
	fb_sync_write()
This blocking function use a memory copy strategy to transfer data
from a user buffer to the frame buffer device in kernel space,
- offset     : offset (in bytes) in the frame buffer
- buffer     : base address of the memory buffer
- length     : number of bytes to be transfered
It returns 0 when the transfer is completed.
********************************************************************/
int 	fb_sync_write(size_t offset, void* buffer, size_t length)
{
	return sys_call(SYSCALL_FB_SYNC_WRITE,
			offset,
			(int)buffer,
			length,
			0);
}
/********************************************************************
	fb_sync_read()
This blocking function use a memory copy strategy to transfer data
from the frame buffer device in kernel space to an user buffer.
- offset     : offset (in bytes) in the frame buffer
- buffer     : base address of the user buffer
- length     : number of bytes to be transfered
It returns 0 when the transfer is completed.
********************************************************************/
int 	fb_sync_read(size_t offset, void* buffer, size_t length)
{
	return sys_call(SYSCALL_FB_SYNC_READ,
			offset,
			(int)buffer,
			length,
			0);
}
/********************************************************************
	fb_write()
This non-blocking function use the DMA coprocessor to transfer data
from a user buffer to the frame buffer device in kernel space,
- offset     : offset (in bytes) in the frame buffer
- buffer     : base address of the user buffer
- length     : number of bytes to be transfered
It returns 0 when the transfer can be started.
It returns -1 if the buffer is not in user address space.
The transfer completion is signaled by an IRQ, and must be
tested by the fb_completed() function.
********************************************************************/
int 	fb_write(size_t offset, void* buffer, size_t length)
{
	return sys_call(SYSCALL_FB_WRITE,
			offset,
			(int)buffer,
			length,
			0);
}
/********************************************************************
	fb_read()
This non-blocking function use the DMA coprocessor to transfer data
from the frame buffer device in kernel space to an user buffer.
- offset     : offset (in bytes) in the frame buffer
- buffer     : base address of the memory buffer
- length     : number of bytes to be transfered
It returns 0 when the transfer can be started.
It returns -1 if the buffer is not in user address space.
The transfer completion is signaled by an IRQ, and must be
tested by the fb_completed() function.
********************************************************************/
int 	fb_read(size_t offset, void* buffer, size_t length)
{
	return sys_call(SYSCALL_FB_READ,
			offset,
			(int)buffer,
			length,
			0);
}
/********************************************************************
	fb_completed()
This blocking function returns when the transfer is completed.
It returns 0 if the transfer is successful.
It returns -1 if an address error has been detected.
********************************************************************/
int 	fb_completed()
{
	return sys_call(SYSCALL_FB_COMPLETED,
			0, 0, 0, 0);
}


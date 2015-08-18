/****************************************************************************************
	File : syscalls.h
 	Written by Alain Greiner & Nicolas Pouillon
	Date : september 2009

	These system calls are used by the MIPS GIET, that is running
	on the MIPS32 processor architecture.

	The supported peripherals are:
	- the SoClib vci_multi_tty
	- the SocLib vci_multi_timer
	- the SocLib vci_dma
	- The SoCLib vci_icu
	- The SoClib vci_locks
	- The SoCLib vci_gcd
	- The SoCLib vci_frame_buffer
	- The SoCLib vci_block_device
****************************************************************************************/

#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include "tty.h"
#include "dma.h"
#include "gcd.h"
#include "timer.h"
#include "icu.h"
#include "block_device.h"

typedef unsigned int	size_t;

// system global variables

extern	unsigned int	_task_context_array[];	
extern	unsigned int	_current_task_array[];
extern	unsigned int	_task_number_array[];
	
// system functions

int _procid();
int _proctime();
int _exit();

int _timer_write(int timer_index, int register_index, int value);
int _timer_read(int timer_index, int register_index, int* buffer);

int _tty_write(int tty_index, char* buffer, int	length);
int _tty_read(int tty_index, char* buffer, int length);

int _io_write(size_t lba, void* buffer, size_t count);
int _io_read(size_t lba, void* buffer, size_t count);
int _io_completed();

int _icu_write(int register_index, int	value);
int _icu_read(int register_index, int*	buffer);

int _gcd_write(int register_index, int	value);
int _gcd_read(int register_index, int*	buffer);

int _locks_write(int lock_index);
int _locks_read(int lock_index);

int _fb_sync_write(size_t offset, void* buffer, size_t length);
int _fb_sync_read(size_t offset, void* buffer, size_t length);

int _fb_write(size_t offset, void* buffer, size_t length);
int _fb_read(size_t offset, void* buffer, size_t length);
int _fb_completed();

#endif


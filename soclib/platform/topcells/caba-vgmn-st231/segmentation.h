/*
 *  Copyright (c) 2008,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 * Based on code written by Nicolas Pouillon for the dma example.
 */
 
/* TODO : check addresses to work accordingly to the ARM966 processor */
//#define RESET_BASE  0x00000000
//#define RESET_SIZE  0x0000ffff
#define RESET_BASE  0xf0000000
#define RESET_SIZE  0x0fffffff

#define	TEXT_BASE	0x08000000
#define	TEXT_SIZE	0x00007938

//#define	DATA_BASE	0x08008428
#define	DATA_BASE	0x080083a0
#define	DATA_SIZE	0x01000D2C

#define	LOC0_BASE	0x20000000
#define	LOC0_SIZE	0x00001000

#define	TTY_BASE	0xC0200000
#define	TTY_SIZE	0x00000258

#define	TIMER_BASE	0xA0200000
#define	TIMER_SIZE	0x00000100

#define	DMA_BASE	0xD1200000
#define	DMA_SIZE	0x00001000

#define FB_WIDTH 320
#define FB_HEIGHT 200

#define	FB_BASE	0xE2200000
#define	FB_SIZE	FB_WIDTH*FB_HEIGHT*2

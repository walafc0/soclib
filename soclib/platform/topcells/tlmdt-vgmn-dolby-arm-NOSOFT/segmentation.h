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
// #define RESET_BASE  0xffff0000
#define RESET_BASE  0xF0000000
#define RESET_SIZE  0x00010000
//#define RESET_BASE	0xffff0000
//#define RESET_SIZE  0x00001000
// #define	RESET_BASE	0xBFC00000
// #define	RESET_SIZE	0x00000200

// #define	EXCEP_BASE	0x80000080
// #define	EXCEP_SIZE	0x00010000
#define BOOT_BASE 0x00000000
#define	BOOT_SIZE	0x00040000

#define TEXT_BASE 0x00400000
#define TEXT_SIZE 0x00040000

#define DATA_BASE 0x10000000
#define DATA_SIZE 0x00800000

#define TTY_BASE  0xBA000000
#define TTY_SIZE  0x00000258

#define AUDIO_FIFO_BASE                     0xB2200000
#define AUDIO_FIFO_NCHANNELS_REG            (AUDIO_FIFO_BASE+0x00)
#define AUDIO_FIFO_FRATE_REG                (AUDIO_FIFO_BASE+0x04)
#define AUDIO_FIFO_VALIDSAMPLE_REG          (AUDIO_FIFO_BASE+0x08)
#define AUDIO_FIFO_CHANNEL_SAMPLE_BASE      (AUDIO_FIFO_BASE+0x10)
#define AUDIO_FIFO_SIZE 32


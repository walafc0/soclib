/*
 * This file is part of DSX, development environment for static
 * SoC applications.
 * 
 * This file is distributed under the terms of the GNU General Public
 * License.
 * 
 * Copyright (c) 2005, Nicolas Pouillon, <nipo@ssji.net>
 *     Laboratoire d'informatique de Paris 6 / ASIM, France
 * 
 *  $Id: srl_endianness.h 960 2007-02-01 15:53:49Z nipo $
 */
#ifndef SRL_ENDIANNESS_H_
#define SRL_ENDIANNESS_H_

#if SRL_LLVM_MODE

uint32_t srl_uint32_le_to_machine(uint32_t x);
uint32_t srl_uint32_machine_to_le(uint32_t x);
uint32_t srl_uint32_be_to_machine(uint32_t x);
uint32_t srl_uint32_machine_to_be(uint32_t x);
uint16_t srl_uint16_le_to_machine(uint16_t x);
uint16_t srl_uint16_machine_to_le(uint16_t x);
uint16_t srl_uint16_be_to_machine(uint16_t x);
uint16_t srl_uint16_machine_to_be(uint16_t x);


#else // SRL_LLVM_MODE

#include <inttypes.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __linux__

#  include <endian.h>

#elif defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)

#  include <machine/endian.h>
#  define __BYTE_ORDER BYTE_ORDER
#  define __LITTLE_ENDIAN LITTLE_ENDIAN
#  define __BIG_ENDIAN BIG_ENDIAN

#else

#  ifdef __LITTLE_ENDIAN__
#    define __BYTE_ORDER __LITTLE_ENDIAN
#  endif

#  if defined(i386) || defined(__i386__)
#    define __BYTE_ORDER __LITTLE_ENDIAN
#  endif

#  if defined(sun) && defined(unix) && defined(sparc)
#    define __BYTE_ORDER __BIG_ENDIAN
#  endif

#endif /* os switches */

#ifndef __BYTE_ORDER
# error Need to know endianess
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
# define srl_uint32_le_to_machine(x) (x)
# define srl_uint32_machine_to_le(x) (x)
# define srl_uint32_be_to_machine(x) srl_uint32_swap(x)
# define srl_uint32_machine_to_be(x) srl_uint32_swap(x)

# define srl_uint16_le_to_machine(x) (x)
# define srl_uint16_machine_to_le(x) (x)
# define srl_uint16_be_to_machine(x) srl_uint16_swap(x)
# define srl_uint16_machine_to_be(x) srl_uint16_swap(x)
#else
# define srl_uint32_le_to_machine(x) srl_uint32_swap(x)
# define srl_uint32_machine_to_le(x) srl_uint32_swap(x)
# define srl_uint32_be_to_machine(x) (x)
# define srl_uint32_machine_to_be(x) (x)

# define srl_uint16_le_to_machine(x) srl_uint16_swap(x)
# define srl_uint16_machine_to_le(x) srl_uint16_swap(x)
# define srl_uint16_be_to_machine(x) (x)
# define srl_uint16_machine_to_be(x) (x)
#endif

static inline uint32_t srl_uint32_swap(uint32_t x)
{
    return (
        ( (x & 0xff)   << 24 ) |
        ( (x & 0xff00) <<  8 ) |
        ( (x >>  8) & 0xff00 ) |
        ( (x >> 24) &   0xff )
        );
}

static inline uint16_t srl_uint16_swap(uint16_t x)
{
    return ((x << 8) | (x >> 8));
}

#endif // SRL_LLVM_MODE

#endif /* SRL_ENDIANNESS_H_ */

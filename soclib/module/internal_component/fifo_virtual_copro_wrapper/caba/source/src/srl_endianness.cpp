#if SRL_LLVM_MODE

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
# define uint32_le_to_machine(x) (x)
# define uint32_machine_to_le(x) (x)
# define uint32_be_to_machine(x) srl_uint32_swap(x)
# define uint32_machine_to_be(x) srl_uint32_swap(x)

# define uint16_le_to_machine(x) (x)
# define uint16_machine_to_le(x) (x)
# define uint16_be_to_machine(x) srl_uint16_swap(x)
# define uint16_machine_to_be(x) srl_uint16_swap(x)
#else
# define uint32_le_to_machine(x) srl_uint32_swap(x)
# define uint32_machine_to_le(x) srl_uint32_swap(x)
# define uint32_be_to_machine(x) (x)
# define uint32_machine_to_be(x) (x)

# define uint16_le_to_machine(x) srl_uint16_swap(x)
# define uint16_machine_to_le(x) srl_uint16_swap(x)
# define uint16_be_to_machine(x) (x)
# define uint16_machine_to_be(x) (x)
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

uint32_t srl_uint32_le_to_machine(uint32_t x)
{
	return uint32_le_to_machine(x);
}

uint32_t srl_uint32_machine_to_le(uint32_t x)
{
	return uint32_machine_to_le(x);
}

uint32_t srl_uint32_be_to_machine(uint32_t x)
{
	return uint32_be_to_machine(x);
}

uint32_t srl_uint32_machine_to_be(uint32_t x)
{
	return uint32_machine_to_be(x);
}

uint16_t srl_uint16_le_to_machine(uint16_t x)
{
	return uint16_le_to_machine(x);
}

uint16_t srl_uint16_machine_to_le(uint16_t x)
{
	return uint16_machine_to_le(x);
}

uint16_t srl_uint16_be_to_machine(uint16_t x)
{
	return uint16_be_to_machine(x);
}

uint16_t srl_uint16_machine_to_be(uint16_t x)
{
	return uint16_machine_to_be(x);
}

#endif

/*
 *
 * SOCLIB_GPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU GPLv2.
 * 
 * SoCLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 * 
 * SOCLIB_GPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: nipo
 */

#include "system.h"
#include "soclib/fd_access.h"

#include "fcntl.h"

int errno;

int strlen( const char *const buf )
{
	const char *n = buf;
	if ( *n == 0 )
		return 0;
	while (*++n)
		;
	return n-buf;
}

int open( const char *path, const int how, const int mode )
{
	soclib_io_set(base(FD), FD_ACCESS_BUFFER, (uint32_t)path);
	soclib_io_set(base(FD), FD_ACCESS_SIZE, strlen(path));
	soclib_io_set(base(FD), FD_ACCESS_HOW, how);
	soclib_io_set(base(FD), FD_ACCESS_MODE, mode);
	soclib_io_set(base(FD), FD_ACCESS_OP, FD_ACCESS_OPEN);
	while (soclib_io_get(base(FD), FD_ACCESS_OP))
		;
	errno = soclib_io_get(base(FD), FD_ACCESS_ERRNO);
	return soclib_io_get(base(FD), FD_ACCESS_RETVAL);
}

int close( const int fd )
{
	soclib_io_set(base(FD), FD_ACCESS_FD, fd);
	soclib_io_set(base(FD), FD_ACCESS_OP, FD_ACCESS_CLOSE);
	while (soclib_io_get(base(FD), FD_ACCESS_OP))
		;
	errno = soclib_io_get(base(FD), FD_ACCESS_ERRNO);
	return soclib_io_get(base(FD), FD_ACCESS_RETVAL);
}

int read( const int fd, const void *buffer, const size_t len )
{
    cache_flush(buffer, len);

	soclib_io_set(base(FD), FD_ACCESS_FD, fd);
	soclib_io_set(base(FD), FD_ACCESS_BUFFER, (uint32_t)buffer);
	soclib_io_set(base(FD), FD_ACCESS_SIZE, len);
	soclib_io_set(base(FD), FD_ACCESS_OP, FD_ACCESS_READ);
	while (soclib_io_get(base(FD), FD_ACCESS_OP))
		;
	errno = soclib_io_get(base(FD), FD_ACCESS_ERRNO);
	return soclib_io_get(base(FD), FD_ACCESS_RETVAL);
}

int write( const int fd, const void *buffer, const size_t len )
{
	soclib_io_set(base(FD), FD_ACCESS_FD, fd);
	soclib_io_set(base(FD), FD_ACCESS_BUFFER, (uint32_t)buffer);
	soclib_io_set(base(FD), FD_ACCESS_SIZE, len);
	soclib_io_set(base(FD), FD_ACCESS_OP, FD_ACCESS_WRITE);
	while (soclib_io_get(base(FD), FD_ACCESS_OP))
		;
	errno = soclib_io_get(base(FD), FD_ACCESS_ERRNO);
	return soclib_io_get(base(FD), FD_ACCESS_RETVAL);
}

int lseek( const int fd, const off_t offset, int whence )
{
	soclib_io_set(base(FD), FD_ACCESS_FD, fd);
	soclib_io_set(base(FD), FD_ACCESS_SIZE, offset);
	soclib_io_set(base(FD), FD_ACCESS_WHENCE, whence);
	soclib_io_set(base(FD), FD_ACCESS_OP, FD_ACCESS_LSEEK);
	while (soclib_io_get(base(FD), FD_ACCESS_OP))
		;
	errno = soclib_io_get(base(FD), FD_ACCESS_ERRNO);
	return soclib_io_get(base(FD), FD_ACCESS_RETVAL);
}

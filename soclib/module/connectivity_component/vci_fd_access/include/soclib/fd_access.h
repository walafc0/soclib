/*
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, Asim
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 */
#ifndef FD_ACCESS_REGS_H
#define FD_ACCESS_REGS_H

enum SoclibFdAccessRegisters {
    FD_ACCESS_FD,
    FD_ACCESS_BUFFER,
    FD_ACCESS_SIZE,
    FD_ACCESS_HOW,
    FD_ACCESS_WHENCE,
    FD_ACCESS_OP,
    FD_ACCESS_RETVAL,
    FD_ACCESS_ERRNO,
    FD_ACCESS_IRQ_ENABLE,
    FD_ACCESS_MODE = FD_ACCESS_WHENCE,
};

enum SoclibFdOp {
	FD_ACCESS_NOOP,
	FD_ACCESS_OPEN,
	FD_ACCESS_CLOSE,
	FD_ACCESS_READ,
	FD_ACCESS_WRITE,
	FD_ACCESS_LSEEK,

	FD_ACCESS_NODE_LOOKUP,
	FD_ACCESS_NODE_CREATE,
	FD_ACCESS_NODE_FREE,
	FD_ACCESS_NODE_LINK,
	FD_ACCESS_NODE_UNLINK,
	FD_ACCESS_NODE_READ,
	FD_ACCESS_NODE_READDIR,
	FD_ACCESS_NODE_WRITE,
	FD_ACCESS_NODE_STAT,
};

#ifdef SYSTEMC_SOURCE
#define FD_ACCESS_OS_PROT(x) FD_ACCESS_##x
#else
#define FD_ACCESS_OS_PROT(x) x
#endif

enum {
	FD_ACCESS_OS_PROT(O_RDONLY)   = 0000001,
	FD_ACCESS_OS_PROT(O_WRONLY)   = 0000002,
	FD_ACCESS_OS_PROT(O_RDWR)     = 0000004,
	FD_ACCESS_OS_PROT(O_CREAT)    = 0000010,
	FD_ACCESS_OS_PROT(O_EXCL)     = 0000020,
	FD_ACCESS_OS_PROT(O_NOCTTY)   = 0000040,
	FD_ACCESS_OS_PROT(O_TRUNC)    = 0000100,
	FD_ACCESS_OS_PROT(O_APPEND)   = 0000200,
	FD_ACCESS_OS_PROT(O_NONBLOCK) = 0000400,
	FD_ACCESS_OS_PROT(O_SYNC)     = 0001000,
	FD_ACCESS_OS_PROT(O_DIRECT)   = 0004000,
	FD_ACCESS_OS_PROT(O_LARGEFILE)= 0010000,
	FD_ACCESS_OS_PROT(O_DIRECTORY)= 0020000,
	FD_ACCESS_OS_PROT(O_NOFOLLOW) = 0040000,
	FD_ACCESS_OS_PROT(O_NOATIME)  = 0100000,
	FD_ACCESS_OS_PROT(O_NDELAY)   = FD_ACCESS_OS_PROT(O_NONBLOCK),

    FD_ACCESS_OS_PROT(S_IFMT   ) =  0170000, //   bit mask for the file type bit fields
    FD_ACCESS_OS_PROT(S_IFSOCK ) =  0140000, //   socket
    FD_ACCESS_OS_PROT(S_IFLNK  ) =  0120000, //   symbolic link
    FD_ACCESS_OS_PROT(S_IFREG  ) =  0100000, //   regular file
    FD_ACCESS_OS_PROT(S_IFBLK  ) =  0060000, //   block device
    FD_ACCESS_OS_PROT(S_IFDIR  ) =  0040000, //   directory
    FD_ACCESS_OS_PROT(S_IFCHR  ) =  0020000, //   character device
    FD_ACCESS_OS_PROT(S_IFIFO  ) =  0010000, //   FIFO
    FD_ACCESS_OS_PROT(S_ISUID  ) =  0004000, //   set UID bit
    FD_ACCESS_OS_PROT(S_ISGID  ) =  0002000, //   set-group-ID bit (see below)
    FD_ACCESS_OS_PROT(S_ISVTX  ) =  0001000, //   sticky bit (see below)
};

#endif /* FD_ACCESS_REGS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


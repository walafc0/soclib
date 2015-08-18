/* -*- c++ -*-
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLIB.
 *
 * SoCLIB is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SoCLIB is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SoCLIB; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo
 */

#include <cstdio>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/select.h>

#include "pts_wrapper.h"
#include "exception.h"

namespace soclib { namespace common {

PtsWrapper::PtsWrapper()
{
    m_fd = getmpt();

    if ( m_fd < 0 )
        throw soclib::exception::RunTimeError("Unabe to allocate a pts");

    m_name = ptsname(m_fd);

    grantpt(m_fd);
    unlockpt(m_fd);

    fcntl( m_fd, F_SETFL, O_NONBLOCK );

    m_poller = FdPoller( m_fd );

    int fd = open(m_name.c_str(), O_RDWR);
    if ( fd < 0 )
        throw soclib::exception::RunTimeError(
            std::string("PTY `")+m_name+"' unavailable");
    close(fd);

    char *autorun = getenv("SOCLIB_PTS_COMMAND");
    if ( autorun && (strlen(autorun) < 128) ) {
        char cmd[128];
        snprintf(cmd, 128, autorun, m_name.c_str());
        system(cmd);
    }
}

PtsWrapper::~PtsWrapper()
{
    m_poller = FdPoller();
    close(m_fd);
}

ssize_t PtsWrapper::read( void *data, size_t len )
{
    return ::read( m_fd, data, len );
}

ssize_t PtsWrapper::write( void *data, size_t len )
{
    return ::write( m_fd, data, len );
}

int PtsWrapper::getmpt()
{
    int pty;

    /* Linux style */
    pty = open("/dev/ptmx", O_RDWR|O_NOCTTY);
    if ( pty >= 0 )
        return pty;

    /* BSD style */
    char name[] = "/dev/ptyp0";
    int tty;
    while (access(name, 0) == 0) {
        if ((pty = open(name, O_RDWR)) >= 0) {
            name[5] = 't';
            if ((tty = open(name, O_RDWR)) >= 0) {
                close(tty);
                return pty;
            }
            name[5] = 'p';
            close(pty);
        }

        /* get next pty name */
        if (name[9] == 'f') {
            name[8]++;
            name[9] = '0';
        } else if (name[9] == '9')
            name[9] = 'a';
        else
            name[9]++;
    }
    errno = ENOENT;
    return -1;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


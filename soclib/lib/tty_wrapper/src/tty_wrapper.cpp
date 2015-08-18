/* -*- c++ -*-
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

#include "process_wrapper.h"
#include "xterm_wrapper.h"
#include "tty_wrapper.h"
#include "pts_wrapper.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

namespace soclib { namespace common {

namespace _tty_wrapper {

class XtermWrapper
    : public TtyWrapper
{
    const std::string m_name;
    soclib::common::XtermWrapper m_process;
public:
    XtermWrapper(const std::string &name)
        : m_name(name),
          m_process(name)
    {
    }

    char getc()
    {
        char c;
        m_process.read(&c, 1);
        return c;
    }

    void putc( char c )
    {
        m_process.write(&c, 1);
    }

    bool hasData()
    {
        return m_process.poll();
    }
};

class TtyPtsWrapper
    : public TtyWrapper
{
    soclib::common::PtsWrapper m_pts;
public:
    TtyPtsWrapper(const std::string &name)
        : m_pts()
    {
        std::cout
            << "PTS for " << name
            << " successfully allocated, please use "
            << m_pts.pty_path() << std::endl;
    }

    char getc()
    {
        char tmp;
        m_pts.read(&tmp, 1);
        return tmp;
    }

    void putc( char c )
    {
        m_pts.write(&c, 1);
    }

    bool hasData()
    {
        return m_pts.has_data();
    }

    ~TtyPtsWrapper()
    {
    }
};

class TermWrapper
    : public TtyWrapper
{
public:
    TermWrapper(const std::string &)
    {
    }

    char getc()
    {
        return -1;
    }

    void putc( char c )
    {
        if ( c < ' ' && c != '\r' && c != '\n' && c != '\t' )
            c = '.';
        std::cout << c;
        std::cout.flush();
    }

    bool hasData()
    {
        return false;
    }

    ~TermWrapper()
    {
    }
};

class FileWrapper
    : public TtyWrapper
{
    std::ofstream m_file;
public:
    FileWrapper(const std::string &name)
            : m_file(name.c_str())
    {
    }

    char getc()
    {
        return -1;
    }

    void putc( char c )
    {
        m_file << c;
        m_file.flush();
    }

    bool hasData()
    {
        return false;
    }

    ~FileWrapper()
    {
        m_file.close();
    }
};

} // private namespace

TtyWrapper *allocateTty( const std::string &name )
{
	typedef enum { USE_XTERM, USE_TERM, USE_FILES, USE_PTS, USE_OTHER } tty_flavor_t;
	const char *const vals[] = { "XTERM", "TERM", "FILES", "PTS" };

	char *use_env = getenv("SOCLIB_TTY");
	int tty_flavor = USE_OTHER;
	if ( use_env ) {
		for (tty_flavor = 0; tty_flavor<USE_OTHER; tty_flavor++) {
			if ( !strcmp(vals[tty_flavor], use_env) )
				break;
		}
	}
	if ( tty_flavor == USE_OTHER )
		tty_flavor = USE_XTERM;
    if ( tty_flavor <= USE_XTERM && !getenv("DISPLAY") )
        tty_flavor = USE_TERM;

    switch (tty_flavor) {
    case USE_XTERM:
        return new _tty_wrapper::XtermWrapper(name);
    case USE_TERM:
        return new _tty_wrapper::TermWrapper(name);
    case USE_FILES:
        return new _tty_wrapper::FileWrapper(name);
    case USE_PTS:
        return new _tty_wrapper::TtyPtsWrapper(name);
    default:
        abort();
        return NULL;
    };
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

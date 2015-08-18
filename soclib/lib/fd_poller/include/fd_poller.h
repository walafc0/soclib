/* -*- c++ -*-
 *
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
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_FD_POLLER_H_
#define SOCLIB_FD_POLLER_H_

#include <list>
#include <pthread.h>
#include <sys/types.h>
#include <ios>

namespace soclib { namespace common {

class FdPoller
{
    typedef std::list<FdPoller*> poller_list_t;
	static poller_list_t s_pollers;
	static pthread_t s_thread;
	static pthread_mutex_t s_lock;
	static bool s_thread_running;
	static int s_changed[2];

	int m_fd;
	bool m_poll_input;
	volatile bool m_has_data;

	static void init();
	static void add( FdPoller * );
	static void remove( FdPoller * );
	static void* thread( void * );

public:
	FdPoller( int fd, bool poll_input = true );
	FdPoller( const FdPoller &ref );
	FdPoller();
	const FdPoller &operator=( const FdPoller &ref );
	~FdPoller();

	bool has_data() const
	{
		return m_has_data;
	}

    void reset();

    friend std::ostream &operator<<( std::ostream &o, const FdPoller &f )
    {
        f.print(o);
        return o;
    }
    void print( std::ostream & ) const;
};

}}

#endif /* SOCLIB_FD_POLLER_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


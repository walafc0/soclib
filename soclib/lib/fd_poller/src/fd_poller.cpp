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

#include <unistd.h>
#include <list>
#include <iostream>
#include <pthread.h>
#include <poll.h>
#include <sys/types.h>
#include <fcntl.h>

#include "../include/fd_poller.h"

namespace soclib { namespace common {

FdPoller::poller_list_t FdPoller::s_pollers;
pthread_t FdPoller::s_thread;
pthread_mutex_t FdPoller::s_lock;
bool FdPoller::s_thread_running;
int FdPoller::s_changed[2];

FdPoller::~FdPoller()
{
	remove(this);
}

FdPoller::FdPoller()
	: m_fd(-1),
	  m_poll_input(false),
	  m_has_data(false)
{
    init();
	add(this);
}

FdPoller::FdPoller( int fd, bool poll_input )
	: m_fd(fd),
	  m_poll_input(poll_input),
	  m_has_data(false)
{
    init();
	add(this);
}

FdPoller::FdPoller( const FdPoller &ref )
	: m_fd(ref.m_fd),
	  m_poll_input(ref.m_poll_input),
	  m_has_data(ref.m_has_data)
{
    init();
	add(this);
}

const FdPoller &FdPoller::operator =( const FdPoller &ref )
{
	remove(this);
	m_fd = ref.m_fd;
	m_poll_input = ref.m_poll_input;
	m_has_data = ref.m_has_data;
	add(this);
    return *this;
}

void FdPoller::reset()
{
	m_has_data = false;
    write( s_changed[1], "", 1 );
}

void FdPoller::add( FdPoller *p )
{
	pthread_mutex_lock(&s_lock);
	bool start_thread = s_pollers.empty();

#if defined(SOCLIB_MODULE_DEBUG)
    std::cout << "Adding " << *p << std::endl;
#endif
	s_pollers.push_back(p);

    write( s_changed[1], "", 1 );
	if ( start_thread ) {
		s_thread_running = true;
		pthread_create( &s_thread, NULL, &FdPoller::thread, NULL );
	}
	pthread_mutex_unlock(&s_lock);
}

void FdPoller::remove( FdPoller *p )
{
	pthread_mutex_lock(&s_lock);
#if defined(SOCLIB_MODULE_DEBUG)
    std::cout << "Removing " << *p << std::endl;
#endif
	s_pollers.remove(p);
	bool stop_thread = s_pollers.empty();
	if ( stop_thread )
		s_thread_running = false;
    write( s_changed[1], "", 1 );
	pthread_mutex_unlock(&s_lock);
	if ( stop_thread ) {
		pthread_join( s_thread, NULL );
    }
}

void FdPoller::init()
{
    static bool done = false;

    if ( done )
        return;

    pipe(s_changed);

    fcntl( s_changed[0], F_SETFL, O_NONBLOCK );

    done = true;

    pthread_mutex_init(&s_lock, NULL);
    s_thread_running = false;
}

void* FdPoller::thread( void *unused )
{
    size_t n_entries = 0;
    struct pollfd *pfd = NULL;
    bool changed = true;

	while (s_thread_running) {
        if ( changed ) {
#if defined(SOCLIB_MODULE_DEBUG)
            std::cout << "Changed" << std::endl;
#endif
            changed = false;
            pthread_mutex_lock(&s_lock);
            if (pfd) {
                delete [] pfd;
                pfd = NULL;
            }
            pfd = new struct pollfd[s_pollers.size()+1];

            n_entries = 1;
            pfd[0].fd = s_changed[0];
            pfd[0].events = POLLIN;

            int index = 0;
            for ( poller_list_t::iterator i = s_pollers.begin();
                  i != s_pollers.end();
                  ++i, ++index ) {
                FdPoller *p = *i;
                struct pollfd *s = &pfd[n_entries];

                if ( p->m_fd >= 0 && !p->m_has_data ) {
                    s->fd = p->m_fd;
                    s->events = POLLERR | POLLHUP | (
                        p->m_poll_input ? POLLIN : POLLOUT );
                    n_entries++;
                }
            }
            pthread_mutex_unlock(&s_lock);
        }

		if ( poll( &pfd[0], n_entries, -1 ) <= 0 )
			continue;

		if ( !s_thread_running )
			break;

		pthread_mutex_lock(&s_lock);
		for ( size_t i=0;
			  i < n_entries;
			  ++i ) {
			if ( !pfd[i].revents )
				continue;

            if ( i == 0 ) {
                changed = true;
                char tmp;
                read( s_changed[0], &tmp, 1 );
                continue;
            }

            poller_list_t::iterator si;
            for ( si = s_pollers.begin();
                  si != s_pollers.end();
                  ++si ) {
                if ( (*si)->m_fd == pfd[i].fd )
                    break;
            }
			if ( si == s_pollers.end() )
				continue;

			if ( (*si)->m_has_data )
                continue;

			(*si)->m_has_data = true;
#if defined(SOCLIB_MODULE_DEBUG)
            std::cout << **si << " has data" << std::endl;
#endif
            changed = true;
		}
		pthread_mutex_unlock(&s_lock);
	}

    if ( pfd )
        delete [] pfd;
    return NULL;
}

void FdPoller::print( std::ostream &o ) const
{
    o << "<FdPoller " << m_fd << ">";
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


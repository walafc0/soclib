/* -*- c++ -*-
 *
 * DSX is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * DSX is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DSX; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Copyright (c) UPMC, Lip6, Asim
 */

#ifndef SRL_LOG_H_
#define SRL_LOG_H_


namespace dsx { namespace caba {


//******* srl log **********
#define SRL_TRACE_FILE "srl_trace.txt"
#define LOG_BUFSZ 1024

#define srl_log( l, c ) _srl_log( VERB_ ## l, c )
#define srl_log_printf( l, c... ) _srl_log_printf( VERB_ ## l, c )

#define srl_assert(expr)                                           \
    do {                                                            \
        if ( ! (expr) ) {                                           \
            srl_log_printf( NONE, "assertion (%s) failed on %s:%d !\n",  \
                             #expr, __FILE__, __LINE__ );           \
            exit(2);                                                \
        }                                                           \
    } while(0)

enum __srl_verbosity {
    VERB_NONE,
    VERB_TRACE,
    VERB_DEBUG,
    VERB_MAX,
};

void _srl_log_printf( int level, const char *fmt, ...);
void _srl_log( int level, const char *msg);
int srl_log_open();


}}

#endif /* SRL_LOG_H_ */

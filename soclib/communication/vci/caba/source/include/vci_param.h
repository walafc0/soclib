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
 * Copyright (c) UPMC, Lip6, Asim
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo joel.porquet@lip6.fr
 */
#ifndef SOCLIB_CABA_SIGNAL_VCI_PARAM_H_
#define SOCLIB_CABA_SIGNAL_VCI_PARAM_H_

#include <systemc>
#include <sstream>
#include <inttypes.h>
#include "static_assert.h"
#include "static_fast_int.h"

namespace soclib { namespace caba {

using namespace sc_core;

static std::string VciParamsString(
    int b, int k, int n, int e, int q,
    int f, int s, int p, int t, int w )
{
    std::ostringstream o;
    o << "vci_param<"
      << b << ',' << k << ',' << n << ',' << e << ','
      << q << ',' << f << ',' << s << ',' << p << ','
      << t << ',' << w << '>';
    return o.str();
}

/********************************************
 * VCI parameters grouped in a single class
 ********************************************/
template<
    int cell_size,
    int plen_size,
    int addr_size,
    int rerror_size,
    int clen_size,
    int rflag_size,
    int srcid_size,
    int pktid_size,
    int trdid_size,
	int wrplen_size
    >
class VciParams
{
	/* Obey standart */

	// This is a check for a pow of 2
    soclib_static_assert(!((cell_size)&(cell_size-1)));
	soclib_static_assert(plen_size <= 9);
    // We need more than 32 bits for addr, so we dont check
    //soclib_static_assert(addr_size <= 32);
	soclib_static_assert(rerror_size <= 3);
	soclib_static_assert(clen_size <= 8);
    // We need more than 5 bits for srcid, so we dont check
    //soclib_static_assert(srcid_size <= 5);
	soclib_static_assert(pktid_size <= 8);
	soclib_static_assert(wrplen_size <= 5);

public:
    /* Standart's constants, may be used by some modules */
    static const int B = cell_size;
    static const int K = plen_size;
    static const int N = addr_size;
    static const int E = rerror_size;
    static const int Q = clen_size;
    static const int F = rflag_size;
    static const int S = srcid_size;
    static const int P = pktid_size;
    static const int T = trdid_size;
    static const int W = wrplen_size;

    /* The basic signal types */
	/* Handshake */
	typedef bool ack_t;
	typedef bool val_t;
	/* Request content */
	typedef sc_dt::sc_uint<N> addr_t;
	typedef sc_dt::sc_uint<B> be_t;
	typedef bool cfixed_t;
	typedef sc_dt::sc_uint<Q> clen_t;
	typedef sc_dt::sc_uint<2> cmd_t;
	typedef bool contig_t;
	typedef sc_dt::sc_uint<B*8> data_t;
	typedef bool eop_t;
	typedef bool const_t;
	typedef sc_dt::sc_uint<K> plen_t;
	typedef bool wrap_t;
	/* Response content */
	typedef sc_dt::sc_uint<E> rerror_t;

	/* The advanced signal types */
	/* Request content */
	typedef bool defd_t;
	typedef sc_dt::sc_uint<W> wrplen_t;
	/* Response content */
	typedef sc_dt::sc_uint<F> rflag_t;
	/* Threading */
	typedef sc_dt::sc_uint<S> srcid_t;
	typedef sc_dt::sc_uint<T> trdid_t;
	typedef sc_dt::sc_uint<P> pktid_t;

    typedef typename ::soclib::common::fast_int_t<addr_size>::int_t fast_addr_t;
    typedef typename ::soclib::common::fast_int_t<cell_size*8>::int_t fast_data_t;
    typedef typename ::soclib::common::fast_int_t<trdid_size>::int_t fast_trdid_t;

    enum {
        CMD_NOP,
        CMD_READ,
        CMD_WRITE,
        CMD_LOCKED_READ,
        CMD_STORE_COND = CMD_NOP,
    };

    static const unsigned int _err_mask = (1<<rerror_size)-1;
    typedef enum {
        ERR_NORMAL = 0 & _err_mask,
        ERR_GENERAL_DATA_ERROR = 1 & _err_mask,
        ERR_BAD_DATA = 5 & _err_mask,
        ERR_ABORT_DISCONNECT = 7 & _err_mask,
    } vci_error_e;

    enum {
        STORE_COND_ATOMIC = 0,
        STORE_COND_NOT_ATOMIC = 1,
    };

    static std::string string( const std::string &name = "" )
    {
        std::string vp = VciParamsString(B,K,N,E,Q,F,S,P,T,W);
        if ( name == "" )
            return vp;
        return name+'<'+vp+'>';
    }

    static inline fast_data_t be2mask( fast_data_t be )
    {
        fast_data_t ret = (be * 0x0100040010004000ULL | be * 0x0002000800200080ULL) & 0x8080808080808080ULL;
        ret |= ret >> 1; ret |= ret >> 2; ret |= ret >> 4;
        return ret;
    }
};  // end class VciParams

/****************************************************
 * This class VciParamsBis can be used when a module
 * has two VCI ports with different parameters.
 * This is actually a temporary (?) patch to solve
 * a soclib-cc limitation.
 ****************************************************/
template<
    int cell_size_bis,
    int plen_size_bis,
    int addr_size_bis,
    int rerror_size_bis,
    int clen_size_bis,
    int rflag_size_bis,
    int srcid_size_bis,
    int pktid_size_bis,
    int trdid_size_bis,
	int wrplen_size_bis
    >
class VciParamsBis
{
	/* Obey standart */

	// This is a check for a pow of 2
    soclib_static_assert(!((cell_size_bis)&(cell_size_bis-1)));
	soclib_static_assert(plen_size_bis <= 9);
    // We need more than 32 bits for addr, so we dont check
    //soclib_static_assert(addr_size_bis <= 32);
	soclib_static_assert(rerror_size_bis <= 3);
	soclib_static_assert(clen_size_bis <= 8);
    // We need more than 5 bits for srcid, so we dont check
    //soclib_static_assert(srcid_size_bis <= 5);
	soclib_static_assert(pktid_size_bis <= 8);
	soclib_static_assert(wrplen_size_bis <= 5);

public:
    /* Standart's constants, may be used by some modules */
    static const int B = cell_size_bis;
    static const int K = plen_size_bis;
    static const int N = addr_size_bis;
    static const int E = rerror_size_bis;
    static const int Q = clen_size_bis;
    static const int F = rflag_size_bis;
    static const int S = srcid_size_bis;
    static const int P = pktid_size_bis;
    static const int T = trdid_size_bis;
    static const int W = wrplen_size_bis;

    /* The basic signal types */
	/* Handshake */
	typedef bool ack_t;
	typedef bool val_t;
	/* Request content */
	typedef sc_dt::sc_uint<N> addr_t;
	typedef sc_dt::sc_uint<B> be_t;
	typedef bool cfixed_t;
	typedef sc_dt::sc_uint<Q> clen_t;
	typedef sc_dt::sc_uint<2> cmd_t;
	typedef bool contig_t;
	typedef sc_dt::sc_uint<B*8> data_t;
	typedef bool eop_t;
	typedef bool const_t;
	typedef sc_dt::sc_uint<K> plen_t;
	typedef bool wrap_t;
	/* Response content */
	typedef sc_dt::sc_uint<E> rerror_t;

	/* The advanced signal types */
	/* Request content */
	typedef bool defd_t;
	typedef sc_dt::sc_uint<W> wrplen_t;
	/* Response content */
	typedef sc_dt::sc_uint<F> rflag_t;
	/* Threading */
	typedef sc_dt::sc_uint<S> srcid_t;
	typedef sc_dt::sc_uint<T> trdid_t;
	typedef sc_dt::sc_uint<P> pktid_t;

    typedef typename ::soclib::common::fast_int_t<addr_size_bis>::int_t fast_addr_t;
    typedef typename ::soclib::common::fast_int_t<cell_size_bis*8>::int_t fast_data_t;
    typedef typename ::soclib::common::fast_int_t<trdid_size_bis>::int_t fast_trdid_t;

    enum {
        CMD_NOP,
        CMD_READ,
        CMD_WRITE,
        CMD_LOCKED_READ,
        CMD_STORE_COND = CMD_NOP,
    };

    static const unsigned int _err_mask = (1<<rerror_size_bis)-1;
    typedef enum {
        ERR_NORMAL = 0 & _err_mask,
        ERR_GENERAL_DATA_ERROR = 1 & _err_mask,
        ERR_BAD_DATA = 5 & _err_mask,
        ERR_ABORT_DISCONNECT = 7 & _err_mask,
    } vci_error_e;

    enum {
        STORE_COND_ATOMIC = 0,
        STORE_COND_NOT_ATOMIC = 1,
    };

    static std::string string( const std::string &name = "" )
    {
        std::string vp = VciParamsString(B,K,N,E,Q,F,S,P,T,W);
        if ( name == "" )
            return vp;
        return name+'<'+vp+'>';
    }

    static inline fast_data_t be2mask( fast_data_t be )
    {
        fast_data_t ret = (be * 0x0100040010004000ULL | be * 0x0002000800200080ULL) & 0x8080808080808080ULL;
        ret |= ret >> 1; ret |= ret >> 2; ret |= ret >> 4;
        return ret;
    }
};  // end class VciParamsBis

/****************************************************
 * This class VciParamsTer can be used when a module
 * has three VCI ports with different parameters.
 * This is actually a temporary (?) patch to solve
 * a soclib-cc limitation.
 ****************************************************/
template<
    int cell_size_ter,
    int plen_size_ter,
    int addr_size_ter,
    int rerror_size_ter,
    int clen_size_ter,
    int rflag_size_ter,
    int srcid_size_ter,
    int pktid_size_ter,
    int trdid_size_ter,
	int wrplen_size_ter
    >
class VciParamsTer
{
	/* Obey standart */

	// This is a check for a pow of 2
    soclib_static_assert(!((cell_size_ter)&(cell_size_ter-1)));
	soclib_static_assert(plen_size_ter <= 9);
    // We need more than 32 bits for addr, so we dont check
    //soclib_static_assert(addr_size_ter <= 32);
	soclib_static_assert(rerror_size_ter <= 3);
	soclib_static_assert(clen_size_ter <= 8);
    // We need more than 5 bits for srcid, so we dont check
    //soclib_static_assert(srcid_size_ter <= 5);
	soclib_static_assert(pktid_size_ter <= 8);
	soclib_static_assert(wrplen_size_ter <= 5);

public:
    /* Standart's constants, may be used by some modules */
    static const int B = cell_size_ter;
    static const int K = plen_size_ter;
    static const int N = addr_size_ter;
    static const int E = rerror_size_ter;
    static const int Q = clen_size_ter;
    static const int F = rflag_size_ter;
    static const int S = srcid_size_ter;
    static const int P = pktid_size_ter;
    static const int T = trdid_size_ter;
    static const int W = wrplen_size_ter;

    /* The basic signal types */
	/* Handshake */
	typedef bool ack_t;
	typedef bool val_t;
	/* Request content */
	typedef sc_dt::sc_uint<N> addr_t;
	typedef sc_dt::sc_uint<B> be_t;
	typedef bool cfixed_t;
	typedef sc_dt::sc_uint<Q> clen_t;
	typedef sc_dt::sc_uint<2> cmd_t;
	typedef bool contig_t;
	typedef sc_dt::sc_uint<B*8> data_t;
	typedef bool eop_t;
	typedef bool const_t;
	typedef sc_dt::sc_uint<K> plen_t;
	typedef bool wrap_t;
	/* Response content */
	typedef sc_dt::sc_uint<E> rerror_t;

	/* The advanced signal types */
	/* Request content */
	typedef bool defd_t;
	typedef sc_dt::sc_uint<W> wrplen_t;
	/* Response content */
	typedef sc_dt::sc_uint<F> rflag_t;
	/* Threading */
	typedef sc_dt::sc_uint<S> srcid_t;
	typedef sc_dt::sc_uint<T> trdid_t;
	typedef sc_dt::sc_uint<P> pktid_t;

    typedef typename ::soclib::common::fast_int_t<addr_size_ter>::int_t fast_addr_t;
    typedef typename ::soclib::common::fast_int_t<cell_size_ter*8>::int_t fast_data_t;
    typedef typename ::soclib::common::fast_int_t<trdid_size_ter>::int_t fast_trdid_t;

    enum {
        CMD_NOP,
        CMD_READ,
        CMD_WRITE,
        CMD_LOCKED_READ,
        CMD_STORE_COND = CMD_NOP,
    };

    static const unsigned int _err_mask = (1<<rerror_size_ter)-1;
    typedef enum {
        ERR_NORMAL = 0 & _err_mask,
        ERR_GENERAL_DATA_ERROR = 1 & _err_mask,
        ERR_BAD_DATA = 5 & _err_mask,
        ERR_ABORT_DISCONNECT = 7 & _err_mask,
    } vci_error_e;

    enum {
        STORE_COND_ATOMIC = 0,
        STORE_COND_NOT_ATOMIC = 1,
    };

    static std::string string( const std::string &name = "" )
    {
        std::string vp = VciParamsString(B,K,N,E,Q,F,S,P,T,W);
        if ( name == "" )
            return vp;
        return name+'<'+vp+'>';
    }

    static inline fast_data_t be2mask( fast_data_t be )
    {
        fast_data_t ret = (be * 0x0100040010004000ULL | be * 0x0002000800200080ULL) & 0x8080808080808080ULL;
        ret |= ret >> 1; ret |= ret >> 2; ret |= ret >> 4;
        return ret;
    }
};  // end class VciParamsTer

}}

#endif /* SOCLIB_CABA_SIGNAL_VCI_PARAM_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


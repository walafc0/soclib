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
 *         Etienne Le Grand <etilegr@hotmail.com>, 2009
 */

#ifndef SOCLIB_CABA_SIGNAL_HHT_PARAM_H
#define SOCLIB_CABA_SIGNAL_HHT_PARAM_H

#include <systemc>
#include <sstream>
#include <inttypes.h>
#include "static_assert.h"
#include "static_fast_int.h"
namespace soclib { namespace caba {

using namespace sc_core;

static std::string HhtParamString(int a, int b)
{
    std::ostringstream o;
    o << "hht_param<"
      << a << ',' << b << '>';
    return o.str();
}

/**
 * HHT parameters grouped in a single class
 */
template<
	int ctrl_size,
	int data_size
    >
class HhtParam
{

public:
    /* Fifo sizes */
	typedef sc_dt::sc_uint<ctrl_size> ctrl_t;
	typedef sc_dt::sc_uint<data_size> data_t;
    typedef sc_dt::sc_uint<4> 	seqid_t;
	typedef sc_dt::sc_uint<5> 	unitid_t;
	typedef sc_dt::sc_uint<5> 	srctag_t;
	typedef bool			 	passpw_t;
	typedef sc_dt::sc_uint<6> 	cmd_t;
	typedef sc_dt::sc_uint<40> 	addr_t;
	typedef sc_dt::sc_uint<4> 	mskcnt_t;
	typedef bool 				compat_t;
	
	typedef bool			 	isoc_t;
	typedef sc_dt::sc_uint<2> 	error_t;
	typedef sc_dt::sc_uint<4> 	count_t;
	typedef bool			 	bridge_t;
	typedef sc_dt::sc_uint<6> 	rvcset_t;
	
	typedef typename ::soclib::common::fast_int_t<ctrl_size>::int_t fast_ctrl_t;
    typedef typename ::soclib::common::fast_int_t<data_size>::int_t fast_data_t;
	
	// page 88 of Hypertransport 3.1 InterconnectTechnology Book
	static const int cmd_shift =		00;
	static const int cmd_mask  = 		0x3F;
    static const int mskcnt0_shift =	026;
	static const int mskcnt0_mask  =	0x3;
    static const int mskcnt1_shift =	030;
	static const int mskcnt1_mask  =	0x3;
    static const int passpw_shift =		017;
	static const int passpw_mask  =		0x1;
    static const int unitid_shift =		010;
	static const int unitid_mask  =		0x1F;
	static const int srctag_shift =		020;
	static const int srctag_mask  =		0x1F;
    
	//page 88 of Hypertransport 3.1 InterconnectTechnology Book
	static const int seqid0_shift =		06;
	static const int seqid0_mask  = 	0x3;
    static const int seqid1_shift =		015;
	static const int seqid1_mask  = 	0x3;
    static const int compat_shift =		025;
	static const int compat_mask  = 	0x1;
    static const int dataerror_shift =	024;
	static const int dataerror_mask  = 	0x1;
    static const int chain_shift =		023;
	static const int chain_mask  = 		0x1;
	static const int addr0_shift =		032;
	static const int addr0_mask  = 		0x3F;
	static const int addr1_shift =		040;
	static const int addr1_mask =		0xFFFFFFFF;
	
	// page 104 of Hypertransport 3.1 InterconnectTechnology Book
	static const int isoc_shift =		07;
	static const int isoc_mask  = 		0x1;
    static const int bridge_shift =		016;
	static const int bridge_mask  =		0x1;
    static const int rquid_shift =		036;
	static const int rquid_mask  =		0x3;
    static const int rvcset_shift =		032;
	static const int rvcset_mask  =		0x7;
    static const int error0_shift =		025;
	static const int error0_mask  =		0x1;
    static const int error1_shift =		035;
	static const int error1_mask  =		0x1;
    
	enum { // page 80-81 of Hypertransport 3.1 InterconnectTechnology Book
		CMD_NOP					= 0x00,	// 000000
        CMD_UNKNOWN				= 0x01,	// 000001
        CMD_SYNC				= 0x3F,	// 111111
        CMD_ADDREXT				= 0x3E,	// 111110
		CMD_BROADCAST			= 0x3A,	// 111010
		CMD_FLUSH				= 0x02,	// 000010
		CMD_FENCE				= 0x3C,	// 111100
		
		CMD_READRSP				= 0x30,	// 110000
		CMD_TARGETDONE			= 0x33,	// 110011
		
		CMD_READ_MODIFY_WRITE	= 0x3D,	// 111101

        CMD_WRITE				= 0x08,	// x01xxx
		CMD_WRITE_MASK			= 0x18, // 011000
		CMD_READ				= 0x10,	// 01xxxx
		CMD_READ_MASK			= 0x30,	// 110000
		
		CMD_POSTED_FLAG			= 0x20,	// 100000
		CMD_RPASSPW_FLAG		= 0x08,	// 001000
        CMD_DWORD_FLAG			= 0x04,	// 000100
        CMD_ISOC_FLAG			= 0x02,	// 000010
        CMD_COHERENT_FLAG		= 0x01	// 000001
        
    };

    static std::string string( const std::string &name = "" )
    {
        std::string hp = HhtParamString(ctrl_size,data_size);
        if ( name == "" )
            return hp;
        return name+'<'+hp+'>';
    }
};

}}

#endif /* SOCLIB_CABA_SIGNAL_HHT_PARAM_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


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
 * Author   : Abdelmalek SI MERABET 
 * Date     : Februrary 2011
 * Copyright: UPMC - LIP6
 */

#ifndef SOCLIB_CABA_RING_SIGNALS_FAST_H_
#define SOCLIB_CABA_RING_SIGNALS_FAST_H_

namespace soclib { namespace caba {

class SimpleRingSignals
{
public:
	bool       cmd_grant;
        uint64_t   cmd_data;
    	bool       cmd_w;       // in : cmd_rok
    	bool       cmd_r;       // in : cmd_wok
    	bool       rsp_grant;   
    	uint64_t   rsp_data;
    	bool       rsp_w;       // in : rsp_rok
    	bool       rsp_r;       // in : rsp_wok

	SimpleRingSignals()
	{}

};


class LocalRingSignals
{
public:
	bool       cmd_grant;
        uint64_t   cmd_data;
    	bool       cmd_w;       // in : cmd_rok
    	bool       cmd_r;       // in : cmd_wok
        bool       cmd_preempt;
        uint32_t   cmd_palloc;
        bool       cmd_header;

    	bool       rsp_grant;   
    	uint64_t   rsp_data;
    	bool       rsp_w;       // in : rsp_rok
    	bool       rsp_r;       // in : rsp_wok
        bool       rsp_preempt;
        uint32_t   rsp_palloc;
        bool       rsp_header;

	LocalRingSignals()
	{}

};

// to keep trace
struct cmd_str  {
         const char* state;
         bool        cmdval;
         uint64_t    flit; // to replace with addr_t
         
};

struct rsp_str  {
         const char* state;
         bool        rspval;
         uint64_t    flit; // to replace with addr_t
         
};

}} // end namespace

#endif /* SOCLIB_CABA_RING_SIGNALS_FAST_H_ */

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
 
#ifndef SOCLIB_CABA_MY_GENERIC_FIFO_H
#define SOCLIB_CABA_MY_GENERIC_FIFO_H

#include "generic_fifo.h"
namespace soclib {
namespace caba {

    using namespace sc_core;
	
/********************************
** Simple GenericFifo Encapsulation
********************************/
template <typename T>
class LazyFifo{
public:
	bool get;
	bool put;
	bool has_got;
	bool has_put;
	T has_read;
	T has_written;
	T write;
	soclib::caba::GenericFifo<T> fifo;
	inline void init(){
		fifo.init();
		has_got=false;
		has_put=false;
		get=false;
		put=false;
	}
	inline T read(){	return fifo.read();	}
	inline bool wok(){	return fifo.wok();  }
	inline bool rok(){	return fifo.rok();  }
	inline bool will_get(){return (fifo.rok() && get);}
	inline bool will_put(){return (fifo.wok() && put);}
	
	void fsm(){
		has_read=read();
		has_written=write;
		has_got=will_get();
		has_put=will_put();
		if (get){
			if (put)
				fifo.put_and_get(write);
			else
				fifo.simple_get();
		}else if(put)
			fifo.simple_put(write);
		get=false;
		put=false;
	}
	LazyFifo(const std::string &name, size_t depth)
        : fifo(name,depth)
    {
    }
	
};

}}

#endif /* SOCLIB_CABA_MY_GENERIC_FIFO_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


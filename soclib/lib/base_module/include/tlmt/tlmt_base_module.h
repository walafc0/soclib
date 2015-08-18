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
 */
#ifndef SOCLIB_TLMT_BASE_MODULE_H_
#define SOCLIB_TLMT_BASE_MODULE_H_

#include <sstream>
#include <tlmt>

namespace soclib { namespace tlmt {

class BaseModule
    : public tlmt_core::tlmt_module
{
	const std::string m_name;
    BaseModule();
    BaseModule(const BaseModule &);
    const BaseModule &operator=(const BaseModule &);
public:
    BaseModule( sc_core::sc_module_name &name )
		: tlmt_core::tlmt_module(name),
		  m_name((const char*)name)
	{}
    virtual ~BaseModule()
	{}
	inline const std::string &name() const
	{
		return m_name;
	}
};

}}

#endif /* SOCLIB_TLMT_BASE_MODULE_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


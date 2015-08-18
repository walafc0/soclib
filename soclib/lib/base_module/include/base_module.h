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
 * Maintainers: nipo
 */
#ifndef SOCLIB_COMMON_BASE_MODULE_H_
#define SOCLIB_COMMON_BASE_MODULE_H_

#include <systemc>
#include <string>

namespace soclib {

class BaseModule
    : public sc_core::sc_module
{
private:
    std::string m_name;

public:
    BaseModule(const sc_core::sc_module_name &name)
            : sc_core::sc_module(name), m_name((const char *)name)
    {
    }

    inline const std::string name() const
    {
        return m_name;
    }

    virtual void stateDump() const
    {
        std::cerr << "Warning: state saving not handled in "<<m_name<<std::endl;
    }

    virtual void stateRestore()
    {
        std::cerr << "Warning: state restoring not handled in "<<m_name<<std::endl;
    }

    virtual ~BaseModule()
    {
    }
};

}

#define SOCLIB_WARNING(x)                                               \
do {                                                                    \
    static bool warned##__LINE__ = false;                               \
    if ( !warned##__LINE__ ) {                                          \
        std::cerr << "Warning at " << __FILE__ << ':' << __LINE__       \
                  << std::endl << x << std::endl;                       \
        warned##__LINE__ = true;                                        \
    }                                                                   \
} while(0)

#endif /* SOCLIB_COMMON_BASE_MODULE_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


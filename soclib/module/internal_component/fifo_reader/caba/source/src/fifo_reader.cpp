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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Maintainers: nipo
 */

#include "../include/fifo_reader.h"
#include "register.h"
#include "base_module.h"
#include "soclib_endian.h"

namespace soclib {
namespace caba {

#define tmpl(x) template<typename word_t> x FifoReader<word_t>

tmpl(/**/)::FifoReader(
	sc_module_name insname,
    const std::string &bin,
    const std::vector<std::string> &argv )
           : soclib::caba::BaseModule(insname),
           p_clk("clk"),
           p_resetn("resetn"),
           p_fifo("fifo"),
           m_wrapper( bin, argv )
{
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();

	m_buffer = 0;
}

tmpl(void)::transition()
{
	if ( p_resetn == false )
	{
		m_woffset = 0;
		m_usage = 0;
		return;
	}

	if ( p_fifo.wok ) {
		// Pas important de savoir si il y en avait un ou pas
		m_usage = 0;
	}

	if ( !m_usage ) {
        unsigned int status;
        
		status = m_wrapper.read(((char*)&m_buffer)+m_woffset, sizeof(word_t)-m_woffset);
        if ( status <= sizeof(word_t) ) {
			m_woffset += status;
			if ( m_woffset == sizeof(word_t) ) {
				// Got it
				m_woffset = 0;
				m_usage++;
			}
		}
	}
}

tmpl(void)::genMoore()
{
	p_fifo.w = (bool)m_usage;
	p_fifo.data = machine_to_le((word_t)m_buffer);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


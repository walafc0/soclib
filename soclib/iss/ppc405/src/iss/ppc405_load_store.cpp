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
 * Copyright (c) UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>, 2007-2009
 *
 * Maintainers: nipo
 *
 * $Id: ppc405_load_store.cpp 1120 2009-05-27 12:57:27Z nipo $
 */

#include <stdint.h>
#include "base_module.h"
#include "ppc405.h"
#include "soclib_endian.h"
#include "arithmetics.h"

namespace soclib { namespace common {


void Ppc405Iss::mem_load_imm( DataOperationType type, uint32_t nb, bool update, bool reversed, bool unsigned_ )
{
    uint32_t base = (m_ins.d.ra || update) ? r_gp[m_ins.d.ra] : 0;
    uint32_t address = base + sign_ext(m_ins.d.imm, 16);
    if ( update )
        r_gp[m_ins.d.ra] = address;
    if ( type != XTN_READ && (address % nb) != 0 )
        {
            m_exception = EXCEPT_ALIGNMENT;
            return;
        }
    m_dreq.valid = true;
    m_dreq.be = ((1<<nb)-1) << (address % 4);
    m_dreq.type = type;
    if ( type == XTN_READ )
        m_dreq.addr = nb*4;
    else
        m_dreq.addr = address;
    r_mem_dest = &r_gp[m_ins.d.rd];
    r_mem_reversed = reversed;
    r_mem_unsigned = unsigned_;
#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": "
        << __FUNCTION__ << ": " << m_dreq
        << "->" << r_mem_dest << " Rev:" << r_mem_reversed << " U:" << r_mem_unsigned
        << std::endl;
#endif
}

void Ppc405Iss::mem_load_indexed( DataOperationType type, uint32_t nb, bool update, bool reversed, bool unsigned_ )
{
    uint32_t base = (m_ins.x.ra || update) ? r_gp[m_ins.x.ra] : 0;
    uint32_t address = base + r_gp[m_ins.x.rb];
    if ( update )
        r_gp[m_ins.d.ra] = address;
    if ( type != XTN_READ && (address % nb) != 0 )
        {
            m_exception = EXCEPT_ALIGNMENT;
            return;
        }
    m_dreq.valid = true;
    m_dreq.be = ((1<<nb)-1) << (address % 4);
    m_dreq.type = type;
    if ( type == XTN_READ )
        m_dreq.addr = nb*4;
    else
        m_dreq.addr = address;
    r_mem_dest = &r_gp[m_ins.d.rd];
    r_mem_reversed = reversed;
    r_mem_unsigned = unsigned_;
#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": "
        << __FUNCTION__ << ": " << m_dreq
        << "->" << r_mem_dest << " Rev:" << r_mem_reversed << " U:" << r_mem_unsigned
        << std::endl;
#endif
}

void Ppc405Iss::mem_xtn( DataOperationType type, uint32_t op, uint32_t data )
{
    m_dreq.valid = true;
    m_dreq.be = 0xf;
    m_dreq.type = type;
    m_dreq.addr = op*4;
    m_dreq.wdata = data;
    r_mem_dest = NULL;
    r_mem_reversed = false;
    r_mem_unsigned = false;
}

void Ppc405Iss::mem_store_imm( DataOperationType type, uint32_t nb, bool update, uint32_t data )
{
    uint32_t base = (m_ins.d.ra || update) ? r_gp[m_ins.d.ra] : 0;
    uint32_t address = base + sign_ext(m_ins.d.imm, 16);
    if ( update )
        r_gp[m_ins.d.ra] = address;
    if ( type != XTN_READ && (address % nb) != 0 )
        {
            m_exception = EXCEPT_ALIGNMENT;
            return;
        }

    data <<= 8 * (4 - nb);
    data = soclib::endian::uint32_swap(data);
    data <<= 8 * (address % 4);

    m_dreq.valid = true;
    m_dreq.be = ((1<<nb)-1) << (address % 4);
    m_dreq.type = type;
    if ( type == XTN_WRITE )
        m_dreq.addr = nb*4;
    else
        m_dreq.addr = address;
    m_dreq.wdata = data;
    r_mem_dest = NULL;
    r_mem_reversed = false;
    r_mem_unsigned = false;
#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": "
        << __FUNCTION__ << ": " << m_dreq
        << "->" << r_mem_dest << " Rev:" << r_mem_reversed << " U:" << r_mem_unsigned
        << std::endl;
#endif
}

void Ppc405Iss::mem_store_indexed( DataOperationType type, uint32_t nb, bool update, uint32_t data )
{
    uint32_t base = (m_ins.x.ra || update) ? r_gp[m_ins.x.ra] : 0;
    uint32_t address = base + r_gp[m_ins.x.rb];
    if ( update )
        r_gp[m_ins.d.ra] = address;
    if ( type != XTN_READ && (address % nb) != 0 )
        {
            m_exception = EXCEPT_ALIGNMENT;
            return;
        }

    data <<= 8 * (4 - nb);
    data = soclib::endian::uint32_swap(data);
    data <<= 8 * (address % 4);

    m_dreq.valid = true;
    m_dreq.be = ((1<<nb)-1) << (address % 4);
    m_dreq.type = type;
    if ( type == XTN_WRITE )
        m_dreq.addr = nb*4;
    else
        m_dreq.addr = address;
    m_dreq.wdata = data;
    r_mem_dest = NULL;
    r_mem_reversed = false;
    r_mem_unsigned = false;
#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": "
        << __FUNCTION__ << ": " << m_dreq
        << "->" << r_mem_dest << " Rev:" << r_mem_reversed << " U:" << r_mem_unsigned
        << std::endl;
#endif
}


void Ppc405Iss::mem_load_word( uint32_t address, uint32_t *dest )
{
    m_dreq.valid = true;
    m_dreq.be = 0xf;
    m_dreq.type = DATA_READ;
    m_dreq.addr = address;
    r_mem_dest = dest;
    r_mem_reversed = false;
    r_mem_unsigned = true;
#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": "
        << __FUNCTION__ << ": " << m_dreq
        << "->" << r_mem_dest
        << std::endl;
#endif
}

void Ppc405Iss::mem_store_word( uint32_t address, uint32_t data )
{
    data = soclib::endian::uint32_swap(data);

    m_dreq.valid = true;
    m_dreq.be = 0xf;
    m_dreq.type = DATA_WRITE;
    m_dreq.addr = address;
    m_dreq.wdata = data;
    r_mem_dest = NULL;
    r_mem_reversed = false;
    r_mem_unsigned = false;
#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": "
        << __FUNCTION__ << ": " << m_dreq
        << "->" << r_mem_dest
        << std::endl;
#endif
}


void Ppc405Iss::mem_load_byte( uint32_t address, uint32_t *dest )
{
    m_dreq.valid = true;
    m_dreq.be = 1 << (address & 0x3);
    m_dreq.type = DATA_READ;
    m_dreq.addr = address;
    r_mem_dest = dest;
    r_mem_reversed = false;
    r_mem_unsigned = true;
#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": "
        << __FUNCTION__ << ": " << m_dreq
        << "->" << r_mem_dest
        << std::endl;
#endif
}

void Ppc405Iss::mem_store_byte( uint32_t address, uint8_t data )
{
    m_dreq.valid = true;
    m_dreq.be = 1 << (address & 0x3);
    m_dreq.type = DATA_WRITE;
    m_dreq.addr = address;
    m_dreq.wdata = data << (8*(address & 0x3));
    r_mem_dest = NULL;
    r_mem_reversed = false;
    r_mem_unsigned = false;
#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": "
        << __FUNCTION__ << ": " << m_dreq
        << "->" << r_mem_dest
        << std::endl;
#endif
}


void Ppc405Iss::do_lmw()
{
    uint32_t address = m_microcode_state.lstmw.address;
    uint32_t rd = m_microcode_state.lstmw.rd;

#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": mem_special_op: "
        << __FUNCTION__ << ": @" << address
		<< " -> " << rd
        << std::endl;
#endif

    m_microcode_state.lstmw.address += 4;
	mem_load_word( address, rd != m_ins.d.ra ? &r_gp[rd] : NULL );
    if ( m_microcode_state.lstmw.rd == 31 )
        m_microcode_func = NULL;
    else
        m_microcode_state.lstmw.rd += 1;
}

void Ppc405Iss::do_stmw()
{
    uint32_t address = m_microcode_state.lstmw.address;
    uint32_t rd = m_microcode_state.lstmw.rd;

#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": mem_special_op: "
        << __FUNCTION__ << ": @" << address
		<< " <- " << rd
        << std::endl;
#endif

    m_microcode_state.lstmw.address += 4;
    mem_store_word( address, r_gp[rd] );
    if ( m_microcode_state.lstmw.rd == 31 )
        m_microcode_func = NULL;
    else
        m_microcode_state.lstmw.rd += 1;
}


void Ppc405Iss::do_lswi()
{
    uint32_t address = m_microcode_state.lstswi.address;
    uint32_t rd = m_microcode_state.lstswi.cur_reg;
	
#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": mem_special_op: "
        << __FUNCTION__ << ": @" << address
		<< " -> " << rd << '/' << m_microcode_state.lstswi.byte_in_reg
		<< "; "
		<< " read " << m_microcode_state.lstswi.tmp
		<< " -> " << m_microcode_state.lstswi.dest
		<< '/' << m_microcode_state.lstswi.dest_byte
        << std::endl;
#endif

	if ( m_microcode_state.lstswi.dest ) {
		uint32_t data = m_microcode_state.lstswi.tmp
			<< (8*m_microcode_state.lstswi.dest_byte);
		uint32_t &dest = *m_microcode_state.lstswi.dest;
		uint32_t mask = 0xff << (8*m_microcode_state.lstswi.dest_byte);
#if SOCLIB_MODULE_DEBUG
		std::cout
			<< m_name << ": mem_special_op: "
			<< __FUNCTION__ << ": @" << address << ", data: "
			<< dest << "&~" << mask << '|' << data
			<< std::endl;
#endif
		dest = (dest & ~mask) | data;
	}

    if ( m_microcode_state.lstswi.byte_count-- == 0 ) {
        m_microcode_func = NULL;
		return;
	}

	if ( rd == m_ins.d.ra ) {
		m_microcode_state.lstswi.dest = NULL;
	} else {
		if ( m_microcode_state.lstswi.byte_in_reg == 3 )
			r_gp[rd] = 0;
		m_microcode_state.lstswi.dest = &r_gp[rd];
		m_microcode_state.lstswi.dest_byte =
			m_microcode_state.lstswi.byte_in_reg;
	}

	mem_load_byte( address, &m_microcode_state.lstswi.tmp );

	if ( m_microcode_state.lstswi.byte_in_reg == 0 ) {
		m_microcode_state.lstswi.byte_in_reg = 3;
		m_microcode_state.lstswi.cur_reg =
			(m_microcode_state.lstswi.cur_reg + 1) % 32;
	} else {
		m_microcode_state.lstswi.byte_in_reg--;
	}
	m_microcode_state.lstswi.address++;
}

void Ppc405Iss::do_stswi()
{
    uint32_t address = m_microcode_state.lstswi.address;
    uint32_t data = r_gp[m_microcode_state.lstswi.cur_reg];

#if SOCLIB_MODULE_DEBUG
    std::cout
        << m_name << ": mem_special_op: "
        << __FUNCTION__ << ": @" << address
		<< " <- " << m_microcode_state.lstswi.cur_reg
		<< '/' << m_microcode_state.lstswi.byte_in_reg
        << std::endl;
#endif

	data = data >> (8*m_microcode_state.lstswi.byte_in_reg);

	mem_store_byte( address, data );

	if ( m_microcode_state.lstswi.byte_in_reg == 0 ) {
		m_microcode_state.lstswi.byte_in_reg = 3;
		m_microcode_state.lstswi.cur_reg =
			(m_microcode_state.lstswi.cur_reg + 1) % 32;
	} else {
		m_microcode_state.lstswi.byte_in_reg--;
	}
	m_microcode_state.lstswi.address++;
    if ( --m_microcode_state.lstswi.byte_count == 0 )
        m_microcode_func = NULL;
}


}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

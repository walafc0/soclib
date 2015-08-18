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
 * Copyright (c) UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: nipo
 */

#include "../include/ississ2.h"

namespace soclib { namespace common {

#define tmpl(...) template<typename iss_t> __VA_ARGS__ IssIss2<iss_t>

tmpl()::~IssIss2()
{
}

tmpl()::IssIss2( const std::string &name, uint32_t ident )
	   : Iss2( name, ident ),
	   m_iss( ident ),
	   m_i_access_ok(false),
	   m_d_access_ok(false)
{
}

tmpl(void)::reset()
{
	m_iss.reset();
}

tmpl(uint32_t)::executeNCycles(
    uint32_t ncycle, const struct Iss2::InstructionResponse &irsp,
    const struct Iss2::DataResponse &drsp, uint32_t irq_bit_field )
{
    {
        m_i_access_ok = ! m_did_ireq || irsp.valid;
        
        if ( m_did_ireq && irsp.valid )
            m_iss.setInstruction( irsp.error, irsp.instruction );
    }

    {
        data_t rdata = 0;

        m_d_access_ok = ! m_did_dreq || drsp.valid;

        if ( !m_did_dreq || !drsp.valid )
            goto no_data;

        switch (last_dtype) {
        case iss_t::READ_WORD:
        case iss_t::STORE_COND:
        case iss_t::READ_LINKED:
            rdata = drsp.rdata;
            break;
        case iss_t::READ_HALF:
            rdata = (drsp.rdata >> ((last_daddr&0x2)*8)) & 0xffff;
            rdata = rdata | (rdata<<16);
            break;
        case iss_t::READ_BYTE:
            rdata = (drsp.rdata >> ((last_daddr&0x3)*8)) & 0xff;
            rdata = rdata |
                (rdata<<8) |
                (rdata<<16) |
                (rdata<<24);
            break;
        case iss_t::LINE_INVAL:
        case iss_t::WRITE_WORD:
        case iss_t::WRITE_HALF:
        case iss_t::WRITE_BYTE:
            break;
        }

        m_iss.setDataResponse(drsp.error, rdata);
      no_data:
        {}
    }

    if ( !ncycle )
        return 0;

    {
        uint32_t busy = m_iss.isBusy(), cycles_done;
        m_iss.setIrq(irq_bit_field);
        if ( busy || ! m_i_access_ok || ! m_d_access_ok ) {
            m_iss.nullStep(ncycle);
            cycles_done = ncycle;
        } else {
            m_iss.step();
            cycles_done = 1;
        }
        return cycles_done;
    }
}

tmpl(void)::getRequests( struct InstructionRequest &ireq, struct DataRequest &dreq ) const
{
    // Instruction part
	m_iss.getInstructionRequest( ireq.valid, ireq.addr );
    ireq.mode = MODE_HYPER;
	((IssIss2*)this)->m_did_ireq = ireq.valid;	

    // Data part
    typename iss_t::DataAccessType datype;
	m_iss.getDataRequest( dreq.valid, datype, dreq.addr, dreq.wdata );
	((IssIss2*)this)->last_daddr = dreq.addr;
	((IssIss2*)this)->last_dtype = datype;
	switch (datype) {
	case iss_t::READ_WORD:
		assert((dreq.addr % 4) == 0);
		dreq.be = 0xf;
		dreq.addr &= ~0x3;
		dreq.type = DATA_READ;
		break;
	case iss_t::READ_HALF:
		assert((dreq.addr % 2) == 0);
		dreq.be = 0x3 << (dreq.addr % 4);
		dreq.addr &= ~0x3;
		dreq.type = DATA_READ;
		break;
	case iss_t::READ_BYTE:
		dreq.be = 0x1 << (dreq.addr % 4);
		dreq.addr &= ~0x3;
		dreq.type = DATA_READ;
		break;
	case iss_t::LINE_INVAL:
		dreq.be = 0;
		dreq.wdata = dreq.addr;
		dreq.type = XTN_WRITE;
		dreq.addr = XTN_DCACHE_INVAL;
		break;
	case iss_t::WRITE_WORD:
		assert((dreq.addr % 4) == 0);
		dreq.be = 0xf;
		dreq.addr &= ~0x3;
		dreq.type = DATA_WRITE;
		break;
	case iss_t::WRITE_HALF:
		assert((dreq.addr % 2) == 0);
		dreq.be = 0x3 << (dreq.addr % 4);
		dreq.addr &= ~0x3;
		dreq.type = DATA_WRITE;
                dreq.wdata =
					(dreq.wdata & 0xffff) |
					(dreq.wdata << 16);
                break;
	case iss_t::WRITE_BYTE:
		dreq.be = 0x1 << (dreq.addr % 4);
		dreq.addr &= ~0x3;
		dreq.type = DATA_WRITE;
                dreq.wdata =
					((dreq.wdata&0xff)) |
					((dreq.wdata&0xff) <<  8) |
					((dreq.wdata&0xff) << 16) |
					((dreq.wdata&0xff) << 24);
                break;
	case iss_t::STORE_COND:
		assert((dreq.addr % 4) == 0);
		dreq.be = 0xf;
		dreq.addr &= ~0x3;
		dreq.type = DATA_SC;
		break;
	case iss_t::READ_LINKED:
		assert((dreq.addr % 4) == 0);
		dreq.be = 0xf;
		dreq.addr &= ~0x3;
		dreq.type = DATA_LL;
		break;
	}
	((IssIss2*)this)->m_did_dreq = dreq.valid;
    dreq.mode = MODE_HYPER;
}

tmpl(void)::setWriteBerr()
{
	m_iss.setWriteBerr();
}

tmpl(void)::setCacheInfo( const struct CacheInfo &info )
{
	m_iss.setICacheInfo(
        info.icache_line_size,
        info.icache_assoc,
        info.icache_n_lines );
	m_iss.setDCacheInfo(
        info.dcache_line_size,
        info.dcache_assoc,
        info.dcache_n_lines );
}

tmpl(unsigned int)::debugGetRegisterCount() const
{
	return m_iss.getDebugRegisterCount();
}

tmpl(Iss2::debug_register_t)::debugGetRegisterValue(unsigned int reg) const
{
    if ( reg == s_pc_register_no )
        return m_iss.getDebugPC();
    else
        return m_iss.getDebugRegisterValue(reg);
}

tmpl(void)::debugSetRegisterValue(unsigned int reg, debug_register_t value)
{
    if ( reg == s_pc_register_no )
        m_iss.setDebugPC( value );
    else
        m_iss.setDebugRegisterValue(reg, value);
}

tmpl(size_t)::debugGetRegisterSize(unsigned int reg) const
{
    return m_iss.getDebugRegisterSize(reg);
}

tmpl(void)::dump() const
{
}

tmpl(bool)::debugExceptionBypassed( Iss2::ExceptionClass cl, Iss2::ExceptionCause ca )
{
	return false;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

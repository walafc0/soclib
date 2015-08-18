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
#ifndef _SOCLIB_ISS2_TO_ISS_WRAPPER_H_
#define _SOCLIB_ISS2_TO_ISS_WRAPPER_H_

#include <inttypes.h>
#include <signal.h>
#include "iss2.h"

namespace soclib { namespace common {

template<typename iss_t>
class IssIss2
    : public Iss2
{
    iss_t m_iss;
    bool m_did_ireq;
    bool m_did_dreq;
    bool m_i_access_ok;
    bool m_d_access_ok;

    typename iss_t::DataAccessType last_dtype;
    typename Iss2::addr_t last_daddr;

public:
    static const size_t n_irq = iss_t::n_irq;
    static const Iss2::debugCpuEndianness s_endianness = Iss2::ISS_BIG_ENDIAN;
    static const unsigned int s_sp_register_no = iss_t::s_sp_register_no;
    static const unsigned int s_fp_register_no = iss_t::s_fp_register_no;
    static const unsigned int s_pc_register_no = iss_t::s_pc_register_no;

    ~IssIss2();
    IssIss2( const std::string &name, uint32_t ident );

    // simulation
    void reset();
    uint32_t executeNCycles( uint32_t ncycle, const struct Iss2::InstructionResponse &irsp,
                             const struct Iss2::DataResponse &drsp, uint32_t irq_bit_field );
    void getRequests( struct InstructionRequest &, struct DataRequest & ) const;
    void setWriteBerr();

    // cache info
    void setCacheInfo( const struct CacheInfo &info );

    // debug
    unsigned int debugGetRegisterCount() const;
    debug_register_t debugGetRegisterValue(unsigned int reg) const;
    void debugSetRegisterValue(unsigned int reg, debug_register_t value);
    size_t debugGetRegisterSize(unsigned int reg) const;

    void dump(void) const;

protected:
    
    virtual bool debugExceptionBypassed( Iss2::ExceptionClass cl, Iss2::ExceptionCause ca = EXCA_OTHER  );
};

}}

#endif // _SOCLIB_ISS2_TO_ISS_WRAPPER_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

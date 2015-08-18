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
#ifndef INITIATOR_FSM_HANDLER_H
#define INITIATOR_FSM_HANDLER_H

#include <stdint.h>
#include <systemc>
#include <vector>
#include <list>
#include <cassert>
#include "vci_initiator.h"
#include "mapping_table.h"
#include "caba_base_module.h"

#define ON_T(x) \
(void (soclib::caba::BaseModule::*)(soclib::caba::VciInitiatorReq<vci_param> *req))(&SC_CURRENT_USER_MODULE::x)

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciInitiatorReq
{
public:
    typedef void (BaseModule::*on_t)(VciInitiatorReq *req);
    typedef typename vci_param::pktid_t pkt_t;
    typedef typename vci_param::trdid_t trd_t;
    typedef typename vci_param::addr_t addr_t;
    typedef typename vci_param::data_t data_t;
protected:
    uint32_t m_thread;
    uint32_t m_packet;
    bool m_failed;

    BaseModule *m_on_done_module;
    on_t m_on_done_func;
    uint32_t m_expected_packets;
private:
    uint32_t m_sent_packets;
    uint32_t m_received_packets;
    
    VciInitiatorReq& operator=( const VciInitiatorReq &ref );
    VciInitiatorReq( const VciInitiatorReq &ref );
public:
    VciInitiatorReq();
    virtual ~VciInitiatorReq();
    void setThread( trd_t );
    void setPacket( pkt_t );
    void setDone( BaseModule *module, on_t callback );
    virtual void cmdOk( bool );
    virtual bool putCmd( VciInitiator<vci_param> &p_pvi, uint32_t ) const = 0;
    virtual void gotRsp( const VciInitiator<vci_param> &p_pvi );
    inline bool failed() const
    {
        return m_failed;
    }
};

template<typename vci_param>
class VciInitiatorSimpleReq
    : public VciInitiatorReq<vci_param>
{
protected:
    uint8_t * const m_dest_buffer;
    const uint32_t m_base_addr;
    const size_t m_len;
    size_t m_cmd_ptr;
    size_t m_rsp_ptr;


    inline uint32_t next_addr( uint32_t addr ) const
    {
        // If base address is not aligned, first word is partial, and
        // frist thing is to align the address
        if ( addr == 0 )
            return addr + vci_param::B - (m_base_addr%vci_param::B);
        else
            return addr + vci_param::B;
    }
public:
    VciInitiatorSimpleReq( uint8_t *, uint32_t, size_t );
    virtual ~VciInitiatorSimpleReq();
    virtual void cmdOk( bool );
};

template<typename vci_param>
class VciInitSimpleReadReq
    : public VciInitiatorSimpleReq<vci_param>
{
public:
    VciInitSimpleReadReq( uint8_t *, uint32_t, size_t );
    virtual ~VciInitSimpleReadReq();
    virtual bool putCmd( VciInitiator<vci_param> &p_pvi, uint32_t ) const;
    virtual void gotRsp( const VciInitiator<vci_param> &p_pvi );
};

template<typename vci_param>
class VciInitSimpleWriteReq
    : public VciInitiatorSimpleReq<vci_param>
{
public:
    VciInitSimpleWriteReq( uint32_t, uint8_t *, size_t );
    virtual ~VciInitSimpleWriteReq();
    virtual bool putCmd( VciInitiator<vci_param> &p_pvi, uint32_t ) const;
    virtual void gotRsp( const VciInitiator<vci_param> &p_pvi );
};

/**
 * \brief Full VCI Initiator port handler
 *
 * This handles a VCI Initiator port, calls back owner module when
 * data is needed or available.
 *
 * \param vci_param VCI fields parameters
 */
template<typename vci_param>
class VciInitiatorFsm
{
private:
    VciInitiator<vci_param> &p_vci;

	const uint32_t m_ident;

    typedef typename vci_param::addr_t addr_t;
    typedef typename vci_param::data_t data_t;

	VciInitiatorReq<vci_param> *m_current_req;
    bool m_current_req_gone;

public:

    /**
     * \brief Constructor
     *
     * \param _vci VCI Initiator port reference to send requests to
     * \param index Initiator index
     */
    VciInitiatorFsm(
        VciInitiator<vci_param> &_vci,
        const uint32_t index );

    /**
     * \brief Desctructor
     */
    ~VciInitiatorFsm();

	void doReq( VciInitiatorReq<vci_param> *req );

    /**
     * \brief Resets internal state
     *
     * Should be called on reset of the owning component
     */
    void reset();

    /**
     * \brief Performs internal state machine transition
     *
     * Should be called on transitions of the owning component
     */
    void transition();

    /**
     * \brief Performs moore generation function and drives signals on
     * VCI port
     *
     * Should be called when generating outputs from the owning
     * component
     */
    void genMoore();
};

}}

#endif /* INITIATOR_FSM_HANDLER_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


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
#ifndef TARGET_FSM_HANDLER_H
#define TARGET_FSM_HANDLER_H

#include <systemc>
#include <vector>
#include <list>
#include <cassert>
#include "vci_target.h"
#include "mapping_table.h"
#include "caba/caba_base_module.h"

#include "linked_access_buffer.h"

namespace soclib {
namespace caba {

using namespace sc_core;

using namespace soclib::common;

#define __rcast3 bool (soclib::caba::BaseModule::*)(int, typename vci_param::addr_t, typename vci_param::data_t &)
#define __wcast3 bool (soclib::caba::BaseModule::*)(int, typename vci_param::addr_t, typename vci_param::data_t, int)

/**
 * \brief Full VCI Target port handler
 *
 * This handles a VCI Target port, calls back owner module when data
 * changes (on read or on write). This also handles multiple-segment
 * targets looking up which segment is targetted by query.
 *
 * \param VCI_TMPL_PARAM_DECL VCI fields parameters
 * \param default_target whether the FSM must handle out-of-segments
 * queries answering a VCI Error packed.
 */
template<
    typename vci_param,
    bool default_target,
    bool support_llsc = false>
class VciTargetFsm
{
    void handle_one();
private:

    VciTarget<vci_param> &p_vci;

    soclib::common::LinkedAccessBuffer<
        typename vci_param::addr_t,
        unsigned int> m_atomic;

    std::vector<soclib::common::Segment> m_segments;

    struct tx_info_s {
        typename vci_param::cmd_t cmd;
        typename vci_param::addr_t addr;
        typename vci_param::addr_t base_addr;
        typename vci_param::srcid_t srcid;
        typename vci_param::trdid_t trdid;
        typename vci_param::pktid_t pktid;
        typename vci_param::data_t  rdata;
        typename vci_param::data_t  wdata;
        typename vci_param::eop_t   eop;
        typename vci_param::be_t   be;
        typename vci_param::rerror_t error;
        typename vci_param::plen_t plen;
    };
    typedef struct tx_info_s tx_info_t;

    tx_info_t m_current_cmd;

    enum mode_t {
        MODE_IDLE,
        MODE_INOUT_QUERY,
        MODE_SIZED_READ,
        MODE_SIZED_WRITE,
        MODE_SIZED_READ_FLUSH_CMD,
        MODE_FLUSH_CMD,
    };
    mode_t m_mode;
    size_t m_cmd_word;
    size_t m_cells_to_go;

    bool m_send_rsp;

    typedef typename vci_param::addr_t addr_t;
    typedef typename vci_param::data_t data_t;

    typedef bool (soclib::caba::BaseModule::*wrapper_read_t)(int segno, addr_t offset, data_t &data);
    typedef bool (soclib::caba::BaseModule::*wrapper_write_t)(int segno, addr_t offset, data_t data, int be);

    wrapper_read_t m_on_read_f;
    wrapper_write_t m_on_write_f;

    soclib::caba::BaseModule *m_owner;

public:

    /**
     * \brief Constructor
     *
     * Takes a reference to the VCI Target port to handler queries
     * from, and a list of segment to handle.
     *
     * \param _vci VCI Target port reference
     * \param seglist list of target's segments
     */
    VciTargetFsm(
        VciTarget<vci_param> &_vci,
        const std::list<soclib::common::Segment> &seglist );

    /**
     * \brief Callback setting
     *
     * Sets which functions should be called when requests asks for
     * data, or changes data
     * \param owner_module module owning target port
     * \param read_func function to call back when data is read from
     * component
     * \param write_func function to call back when data is written to
     * component
     */
    void _on_read_write(
        soclib::caba::BaseModule *owner_module,
        wrapper_read_t read_func,
        wrapper_write_t write_func );

#define on_read_write(rf, wf)                   \
_on_read_write(this,                            \
(__rcast3)&SC_CURRENT_USER_MODULE::rf,          \
(__wcast3)&SC_CURRENT_USER_MODULE::wf )

    /**
     * \brief Desctructor
     */
    ~VciTargetFsm();

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

    /**
     * \brief Gets a segment's size
     *
     * \param seg wanted segment's number
     * \return segment's size (bytes)
     */
    inline typename vci_param::addr_t getSize(size_t seg) const
    {
        assert(seg < m_segments.size());
        return m_segments[seg].size();
    }

    /**
     * \brief Gets a segment's end address
     *
     * \param seg wanted segment's number
     * \return segment's end address
     */
    inline typename vci_param::addr_t getEnd(size_t seg) const
    {
        assert(seg < m_segments.size());
        return m_segments[seg].baseAddress()+m_segments[seg].size();
    }

    /**
     * \brief Gets a segment's base address
     *
     * \param seg wanted segment's number
     * \return segment's base address
     */
    inline typename vci_param::addr_t getBase(size_t seg) const
    {
        assert(seg < m_segments.size());
        return m_segments[seg].baseAddress();
    }

    /**
     * \brief Gets a segment's name
     *
     * \param seg wanted segment's number
     * \return segment's name
     */
    inline const char * getName(size_t seg) const
    {
        assert(seg < m_segments.size());
        return m_segments[seg].name().c_str();
    }

    /**
     * \brief Get number of handled segments
     *
     * \return number of handled segments
     */
    inline size_t nbSegments() const
    {
        return m_segments.size();
    }

    /**
     * \brief Get source ID for current served transaction
     *
     * \return the source ID
     */
    inline int currentSourceId() const
    {
        return m_current_cmd.srcid;
    }

    inline const std::string name() const
    {
        return m_owner->name()+"_target_fsm";
    }
};

}}

#endif /* TARGET_FSM_HANDLER_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


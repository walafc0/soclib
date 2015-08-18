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
#ifndef SOCLIB_VCI_FD_ACCESS_H
#define SOCLIB_VCI_FD_ACCESS_H

#include <map>
#include <set>
#include <stdint.h>
#include <systemc>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "vci_target_fsm.h"
#include "vci_initiator_fsm.h"
#include "caba_base_module.h"
#include "mapping_table.h"

#define SYSTEMC_SOURCE
#include "fd_access.h"
#undef SYSTEMC_SOURCE


namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciFdAccess
	: public caba::BaseModule
{
private:
    soclib::caba::VciTargetFsm<vci_param, true> m_vci_target_fsm;
    soclib::caba::VciInitiatorFsm<vci_param> m_vci_init_fsm;
    typedef typename soclib::caba::VciInitiatorReq<vci_param> req_t;

    bool on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be);
    bool on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data);
    void read_done( req_t *req );
    void write_finish( req_t *req );
    void transition();
    void genMoore();

	uint32_t m_fd;
	int m_op;
	uint32_t m_buffer;
	uint32_t m_how;
	uint32_t m_size;
	uint32_t m_whence;
	uint32_t m_retval;
	uint32_t m_errno;
	bool m_irq_enabled;
    uint32_t m_chunck_offset;
	bool r_irq;

	int m_current_op;

	uint8_t *m_data;

	inline void ended();
	void next_req();

    std::string _tmp_path;
    unsigned int _tmp_id;

    enum node_type_e {
        node_dir,
        node_file,
    };

    struct Fd
    {
        /** root create */
        Fd(VciFdAccess *fda, const char *name);
        /** create node */
        Fd(VciFdAccess *fda, enum node_type_e t, uint32_t &id);
        /** lookup node */
        Fd(VciFdAccess *fda, Fd *parent, const char *name, uint32_t &size, uint32_t &mode, uint32_t &id);
        ~Fd();

        int link(Fd *child, const char *name);
        int unlink(const char *name);
        ssize_t read(off_t offset, size_t size, void *buf);
        int readdir(off_t index, char *name, uint32_t &mode);
        ssize_t write(off_t offset, size_t size, const void *buf);
        int stat(uint32_t &size, uint32_t &mode);

        bool _has_tmp;
        enum node_type_e _type;
        int _fd;
        VciFdAccess *_fda;
        std::string _path;
    };

    std::map<uint32_t, Fd*> _ino_map;

protected:
    SC_HAS_PROCESS(VciFdAccess);

public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    soclib::caba::VciTarget<vci_param> p_vci_target;
    soclib::caba::VciInitiator<vci_param> p_vci_initiator;
    sc_out<bool> p_irq;

	VciFdAccess(
		sc_module_name name,
		const soclib::common::MappingTable &mt,
		const soclib::common::IntTab &srcid,
		const soclib::common::IntTab &tgtid );

	~VciFdAccess();
};

}}

#endif /* SOCLIB_VCI_FD_ACCESS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


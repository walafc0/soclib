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
 *         Nicolas Pouillon <nipo@ssji.net>, 2006
 * Copyright (c) 2012 Institut Telecom / Telecom ParisTech
 *         Alexandre Becoulet <alexandre.becoulet@telecom-paristech.fr>
 */
#ifndef SOCLIB_CABA_FDT_ROM_H
#define SOCLIB_CABA_FDT_ROM_H

#include <systemc>
#include "vci_target_fsm.h"
#include "caba_base_module.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciFdtRom
	: public soclib::caba::BaseModule
{
    soclib::caba::VciTargetFsm<vci_param,true,true> m_vci_fsm;

public:
    typedef typename vci_param::addr_t vci_addr_t;
    typedef typename vci_param::data_t vci_data_t;

private:
    typename vci_param::fast_data_t **m_contents;

protected:
	SC_HAS_PROCESS(VciFdtRom);

public:
    sc_in<bool> p_resetn;
    sc_in<bool> p_clk;
    soclib::caba::VciTarget<vci_param> p_vci;

    VciFdtRom(
        sc_module_name insname,
        const IntTab &index,
        const MappingTable &mt);
    ~VciFdtRom();

    /** Start a section */
    void begin_node(const std::string &name);

    /** Start cpus section */
    void begin_cpus();

    /** Starts a new cpu node, must be paired with end_node */
    void begin_cpu_node(const std::string &compatible, int cpu_id);
    int get_cpu_phandle(int cpu_id) const;

    /** Starts a new device node, must be paired with end_node */
    void begin_device_node(const std::string &segname, const std::string &compatible);

    /** Get phandle associated with node in fdt tree */
    int get_device_phandle(const std::string &segname) const;

    /** Get name associated with node in fdt tree */
    std::string get_device_name(const std::string &segname) const;

    /** End device or cpu node */
    void end_node();

    void add_property(const std::string &name);
    void add_property(const std::string &name, int value);
    void add_property(const std::string &name, const std::string &value);
    void add_property(const std::string &name, const std::vector<int> &values);
    void add_property(const std::string &name, const int * values, size_t size);

private:
    bool on_write(size_t seg, vci_addr_t addr, vci_data_t data, int be);
    bool on_read(size_t seg, vci_addr_t addr, vci_data_t &data);
    void transition();
    void genMoore();

    struct fdt_header_s
    {
        uint32_t magic;
        uint32_t totalsize;
        uint32_t off_dt_struct;
        uint32_t off_dt_strings;
        uint32_t off_mem_rsvmap;
        uint32_t version;
        uint32_t last_comp_version;
        uint32_t boot_cpuid_phys;
        uint32_t size_dt_strings;
        uint32_t size_dt_struct;
    };

    struct fdt_mem_reserve_map_s
    {
        uint64_t addr;
        uint64_t size;
    };

    struct fdt_prop_s
    {
        const uint32_t size;
        const uint32_t strid;
        const char data[0];
    };

    struct fdt_header_s *header;
    struct fdt_mem_reserve_map_s *mem_reserve_ptr;
    uint32_t *struct_begin;
    uint32_t *struct_ptr;
    char *rst;
    char *end;
    int err;

    const Segment *find_segment(const std::string &segname) const;

    int fdt_writer_init(void *blob, size_t available_size);
    uint32_t fdt_writer_push_string(const char *str);
    int fdt_writer_finalize(size_t *real_size);

    void fdt_writer_add_rsvmap(uint64_t addr, uint64_t size);
    uint32_t fdt_writer_node_entry(const char *name);
    void fdt_writer_node_prop(const char *name, const void *data, size_t len);
    void fdt_writer_node_leave();

    bool _finalized;
    std::vector<uint8_t> _data;
    const MappingTable &_mt;
};

}}

#endif /* SOCLIB_CABA_MULTI_ROM_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


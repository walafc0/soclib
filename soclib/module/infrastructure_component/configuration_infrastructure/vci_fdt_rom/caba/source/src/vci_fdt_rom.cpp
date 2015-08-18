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
 * Copyright (c) 2012 Institut Telecom / Telecom ParisTech
 *         Alexandre Becoulet <alexandre.becoulet@telecom-paristech.fr>
 *
 * Maintainers: nipo
 */

#include "vci_fdt_rom.h"
#include "soclib_endian.h"

#include <cstring>

namespace soclib {
namespace caba {

using namespace soclib;

#define tmpl(x) template<typename vci_param> x VciFdtRom<vci_param>

#define FDT_MAGIC 0xd00dfeed
#define FDT_NODE_START 0x1
#define FDT_NODE_END 0x2
#define FDT_PROP 0x3
#define FDT_NOP 0x4
#define FDT_END 0x9

tmpl(int)::fdt_writer_init(void *blob, size_t available_size)
{
    memset(blob, 0, available_size);

	header = (struct fdt_header_s*)blob;
	mem_reserve_ptr = (struct fdt_mem_reserve_map_s*)(header+1);
	struct_begin
		= struct_ptr
		= (uint32_t*)(mem_reserve_ptr+1);
	end = (char*)((uintptr_t)blob + available_size - 1);
	*end = 0;
	rst = end;
	err = 0;

	return 0;
}

tmpl(void)::fdt_writer_add_rsvmap(	uint64_t addr, uint64_t size)
{
	mem_reserve_ptr->addr = uint64_machine_to_be(addr);
	mem_reserve_ptr->size = uint64_machine_to_be(size);
	mem_reserve_ptr++;
	struct_begin
		= struct_ptr
		= (uint32_t*)(mem_reserve_ptr+1);
}


tmpl(uint32_t)::fdt_writer_node_entry(	const char *name)
{
	if ( err || (void*)struct_ptr > (void*)rst ) {
		err = -1;
		return 0;
	}

	size_t len = strlen(name);

	size_t offset = 4 * (struct_ptr - struct_begin);
	*struct_ptr++ = uint32_machine_to_be(FDT_NODE_START);
	memcpy(struct_ptr, name, len);
	struct_ptr += (len+4) >> 2;

	return offset;
}

tmpl(uint32_t)::fdt_writer_push_string(const char *str)
{
	const char *lookup = rst;
	size_t len = strlen(str);

	while (lookup < end) {
		if ( !strcmp(lookup, str) )
			return end - lookup - len - 1;
		lookup += strlen(lookup)+1;
	}

	rst -= len+1;
	memcpy(rst, str, len+1);
	return end - rst - len - 1;
}

tmpl(void)::fdt_writer_node_prop(const char *name, const void *data, size_t len)
{
	if ( err || (char*)struct_ptr + 8 + len > (char*)rst ) {
		err = -1;
		return;
	}

	*struct_ptr++ = uint32_machine_to_be(FDT_PROP);
	*struct_ptr++ = uint32_machine_to_be(len);
	*struct_ptr++ = uint32_machine_to_be(fdt_writer_push_string(name));
	memcpy(struct_ptr, data, len);
	struct_ptr += (len+3) >> 2;
}


tmpl(void)::fdt_writer_node_leave()
{
	if ( err || (void*)struct_ptr > (void*)rst ) {
		err = -1;
		return;
	}
	*struct_ptr++ = uint32_machine_to_be(FDT_NODE_END);
}

tmpl(int)::fdt_writer_finalize(size_t *real_size)
{
	if ( err )
		return err;

	*struct_ptr++ = uint32_machine_to_be(FDT_END);

	char *string_table = (char*)struct_ptr;
	const char *string_orig = rst;
	while ( string_orig < end ) {
		size_t len = strlen(string_orig);
		size_t offset = end - string_orig - len - 1;
		memcpy(string_table + offset, string_orig, len + 1);
		string_orig += len + 1;
	}

    size_t size = (uintptr_t)(string_table + (end - rst)) - (uintptr_t)header; 
    if (real_size)
        *real_size = size;

	header->magic = uint32_machine_to_be(FDT_MAGIC);
	header->totalsize = uint32_machine_to_be(size);
	header->off_dt_struct = uint32_machine_to_be(
		(uintptr_t)(struct_begin) - (uintptr_t)header);
	header->off_dt_strings = uint32_machine_to_be(
		(uintptr_t)(string_table) - (uintptr_t)header);
	header->off_mem_rsvmap = uint32_machine_to_be(
		sizeof(*header));
	header->version = uint32_machine_to_be(
		17);
	header->last_comp_version = uint32_machine_to_be(
		16);
	header->boot_cpuid_phys = uint32_machine_to_be(
		0);
	header->size_dt_strings = uint32_machine_to_be(
		end - rst);
	header->size_dt_struct = uint32_machine_to_be(
		4 * (struct_ptr - struct_begin));

	return 0;
}

tmpl(/**/)::VciFdtRom(
	sc_module_name insname,
	const IntTab &index,
	const MappingTable &mt)
	: caba::BaseModule(insname),
	  m_vci_fsm(p_vci, mt.getSegmentList(index)),
      p_resetn("resetn"),
      p_clk("clk"),
      p_vci("vci"),
      _mt(mt)
{
	m_vci_fsm.on_read_write(on_read, on_write);

    _finalized = false;
    _data.resize(m_vci_fsm.getSize(0));

    fdt_writer_init(&_data[0], _data.size());
    fdt_writer_node_entry("");
    uint32_t val = uint32_machine_to_be(1);
    fdt_writer_node_prop("#address-cells", &val, 4);
    val = uint32_machine_to_be(1);
    fdt_writer_node_prop("#size-cells", &val, 4);

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();
	
	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

tmpl(/**/)::~VciFdtRom()
{
}

tmpl(void)::begin_node(const std::string &name)
{
    fdt_writer_node_entry(name.c_str());
}

tmpl(void)::begin_cpus()
{
    fdt_writer_node_entry("cpus");
    uint32_t val = uint32_machine_to_be(1);
    fdt_writer_node_prop("#address-cells", &val, 4);
    val = uint32_machine_to_be(0);
    fdt_writer_node_prop("#size-cells", &val, 4);
}

tmpl(const Segment *)::find_segment(const std::string &segname) const
{
    std::list<Segment>::const_iterator i;

    for (i = _mt.getAllSegmentList().begin(); ; i++)
        {
            if (i == _mt.getAllSegmentList().end())
                throw exception::RunTimeError(std::string("VciFdtRom: no such segment: ") + segname);
            if (i->name() == segname)
                return &*i;
        }
}

tmpl(void):: begin_cpu_node(const std::string &compatible, int id)
{
    char name[512];
    snprintf(name, 512, "cpu@%u", id);
    fdt_writer_node_entry(name);

    uint32_t val = uint32_machine_to_be(id);
    fdt_writer_node_prop("linux,phandle", &val, 4);

    fdt_writer_node_prop("compatible", compatible.c_str(), compatible.size());

    val = uint32_machine_to_be(id);
    fdt_writer_node_prop("reg", &val, 4);

    fdt_writer_node_prop("interrupt-controller", NULL, 0);
}

tmpl(int)::get_cpu_phandle(int cpu_id) const
{
    return cpu_id;
}

tmpl(std::string):: get_device_name(const std::string &segname) const
{
    const Segment *s = find_segment(segname);

    char name[512];
    snprintf(name, 512, "%s@%08x", s->name().c_str(), (unsigned int)s->baseAddress());
    return std::string(name);
}

tmpl(void):: begin_device_node(const std::string &segname, const std::string &compatible)
{
    const Segment *s = find_segment(segname);

    char name[512];
    snprintf(name, 512, "%s@%08x", s->name().c_str(), (unsigned int)s->baseAddress());
    fdt_writer_node_entry(name);

    uint32_t val = uint32_machine_to_be(0x1000 + s->index().sum());
    fdt_writer_node_prop("linux,phandle", &val, 4);

    fdt_writer_node_prop("compatible", compatible.c_str(), compatible.size());

    uint32_t reg[2] = { uint32_machine_to_be(s->baseAddress()), uint32_machine_to_be(s->size()) };
    fdt_writer_node_prop("reg", reg, 8);
}

tmpl(int)::get_device_phandle(const std::string &segname) const
{
    const Segment *s = find_segment(segname);

    return 0x1000 + s->index().sum();
}

tmpl(void):: add_property(const std::string &name)
{
    fdt_writer_node_prop(name.c_str(), "", 0);
}
 
tmpl(void):: add_property(const std::string &name, int value)
{
    uint32_t val = uint32_machine_to_be(value); /* FIXME */
    fdt_writer_node_prop(name.c_str(), &val, 4);
}

tmpl(void):: add_property(const std::string &name, const std::string &value)
{
    fdt_writer_node_prop(name.c_str(), value.c_str(), value.size() + 1);
}

tmpl(void):: add_property(const std::string &name, const std::vector<int> &values)
{
    uint32_t a[values.size()];
    for (unsigned int i = 0; i < values.size(); i++)
        a[i] = uint32_machine_to_be(values[i]);
    fdt_writer_node_prop(name.c_str(), a, 4*values.size());
}

tmpl(void):: add_property(const std::string &name, const int * values, size_t size)
{
    uint32_t a[size];
    for (unsigned int i = 0; i < size; i++)
        a[i] = uint32_machine_to_be(values[i]);
    fdt_writer_node_prop(name.c_str(), a, 4*size);
}

tmpl(void):: end_node()
{
    fdt_writer_node_leave();
}

tmpl(bool)::on_write(size_t seg, vci_addr_t addr, vci_data_t data, int be)
{
    return false;
}

tmpl(bool)::on_read(size_t seg, vci_addr_t addr, vci_data_t &data )
{
    if (!_finalized)
        {
            fdt_writer_node_leave();
            fdt_writer_finalize(NULL);
            _finalized = true;
        }

    assert(vci_param::B == 4);
	data = _data[addr] | (_data[addr+1] << 8) | (_data[addr+2] << 16) | (_data[addr+3] << 24);

	return true;
}

tmpl(void)::transition()
{
	if (!p_resetn) {
		m_vci_fsm.reset();
		return;
	}
	m_vci_fsm.transition();
}

tmpl(void)::genMoore()
{
	m_vci_fsm.genMoore();
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


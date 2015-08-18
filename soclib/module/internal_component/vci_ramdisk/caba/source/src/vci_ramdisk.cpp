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
 * Maintainers: joel
 */

#include "vci_ramdisk.h"
#include "soclib_endian.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace soclib {
namespace caba {

using namespace soclib;

#define tmpl(x) template<typename vci_param> x VciRamDisk<vci_param>

tmpl(/**/)::VciRamDisk(
	sc_module_name insname,
	const IntTab &index,
	const MappingTable &mt,
    const std::string &filename
    )
	: caba::BaseModule(insname),
	  m_vci_fsm(p_vci, mt.getSegmentList(index)),
      m_filename(filename),
      p_resetn("resetn"),
      p_clk("clk"),
      p_vci("vci")
{
    assert(m_vci_fsm.nbSegments() == 1 && "RamDisk has only one segment");
	m_vci_fsm.on_read_write(on_read, on_write);
	
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();
	
	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

tmpl(/**/)::~VciRamDisk()
{
}

tmpl(void)::reload()
{
    int diskimg;
    if ((diskimg = open(m_filename.c_str(), O_RDWR)) < 0)
        throw soclib::exception::RunTimeError(
            std::string("Cant open binary image ")+m_filename);
    
    struct stat statbuf;
    /* find size of image file */
    if (fstat (diskimg, &statbuf) < 0)
        throw soclib::exception::RunTimeError(
            std::string("fstat error on ")+m_filename);

    if (m_vci_fsm.getSize(0) < statbuf.st_size)
        throw soclib::exception::RunTimeError(
            std::string("Segment too small for binary image ")+m_filename);

    /* mmap the image file */
    if ((m_contents = mmap (0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, diskimg, 0)) < 0)
        throw soclib::exception::RunTimeError(
            std::string("mmap failed for binary image ")+m_filename);

    close(diskimg);
}

tmpl(void)::reset()
{
    m_cpt_read = 0;
    m_cpt_write = 0;
    m_cpt_idle = 0;
}

tmpl(bool)::on_write(size_t seg, vci_addr_t addr, vci_data_t data, int be)
{
#ifdef SOCLIB_MODULE_DEBUG
    printf("write on ramdisk\n");
#endif

    int index = addr / vci_param::B;
    ram_t *tab = (ram_t*)m_contents;
	unsigned int cur = tab[index];
    uint32_t mask = vci_param::be2mask(be);
    
    tab[index] = (cur & ~mask) | (machine_to_le(data) & mask);
    m_cpt_write++;

    return true;
}

tmpl(bool)::on_read(size_t seg, vci_addr_t addr, vci_data_t &data )
{
    int index = addr / vci_param::B;
    ram_t *tab = (ram_t*)m_contents;
	data = le_to_machine(tab[index]);
    m_cpt_read++;
	return true;
}

tmpl(void)::transition()
{
	if (!p_resetn) {
		m_vci_fsm.reset();
		reset();
		reload();
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


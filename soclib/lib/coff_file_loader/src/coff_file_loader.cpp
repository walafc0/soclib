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
 * Loading of a coff binary file in a ram
 *
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (C) IRISA/INRIA, 2007-2008
 *         Francois Charot <charot@irisa.fr>
 *
 *
 * coff_file_loader.cpp : COFF file reader
 *
 * Maintainers: charot
 */

#include <algorithm>
#include "exception.h"
#include "soclib_endian.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include "static_init_code.h"
#include "loader.h"

namespace soclib {
namespace common {

#define DEBUG_COFF 0

#define COFF_MAGIC_2 0302
#define ISCOFF(x) ((x) == COFF_MAGIC_2)

#define COFF_STYP_REG 0X0
#define COFF_STYP_DSECT 0X1
#define COFF_STYP_COPY 0X10
#define COFF_STYP_TEXT 0x520
#define COFF_STYP_DATA 0x40
#define COFF_STYP_DATA1 0x280
#define COFF_STYP_BSS  0x080

struct coff_state
{
	uintptr_t mask;
	Loader *loader;
};

char * coff_buffer;
char * file_ptr;
char * file_ptr_saved, * file_ptr_sections;
bool byte_swapped;
int nbOfSections;

struct coff_file_header {
	uint16_t f_magic; 	/* magic number                         */
	uint16_t f_nscns; 	/* number of sections                   */
	uint32_t f_timdat; 	/* time & date stamp                    */
	uint32_t f_symptr; 	/* file pointer to symtab               */
	uint32_t f_nsyms; 	/* number of symtab entries             */
	uint16_t f_opthdr;	/* sizeof(optional hdr)                 */
	uint16_t f_flags; 	/* flags                                */
	uint16_t f_target_id; /* target architecture id               */
};

struct coff_file_optional_header {
	uint16_t magic; 	/* type of file    */
	uint16_t vstamp; 	/* version stamp                        */
	uint32_t tsize; 	/* text size in bytes, padded to FW bdry*/
	uint32_t dsize; 	/* initialized data "  "                */
	uint32_t bsize; 	/* uninitialized data "   "             */
	uint32_t entrypt; 	/* entry pt.                            */
	uint32_t text_start; /* base of text used for this file      */
	uint32_t data_start; /* base of data used for this file      */
};

struct coff_file_section_header {
	char s_name[8]; 	/* osection name      		       */
	uint32_t s_paddr; 	/* physical address                    */
	uint32_t s_vaddr; 	/* virtual address                     */
	uint32_t s_size; 	/* section size                        */
	uint32_t s_scnptr; 	/* file ptr to raw data for section    */
	uint32_t s_relptr; 	/* file ptr to relocation              */
	uint32_t s_lnnoptr; /* file ptr to line numbers            */
	uint32_t s_nreloc; 	/* number of relocation entries        */
	uint32_t s_nlnno; 	/* number of line number entries       */
	uint32_t s_flags; 	/* flags                               */
	uint16_t s_reserved;/* reserved 2 bytes                    */
	uint16_t s_page; /* memory page id                      */
};


void process_file_header(const std::string &filename) {

	coff_file_header * file_header;

	file_header = (coff_file_header *) file_ptr;
	byte_swapped = false;

#if DEBUG_COFF
	std::cout << "File header" << std::endl;
	std::cout << "	Magic number: " << std::hex << std::showbase << file_header->f_magic << std::dec << std::endl <<
	"	number of sections: " << file_header->f_nscns << std::endl <<
	"	time & date: " << file_header->f_timdat << std::endl <<
	"	file pointer to symtab: " << std::hex << std::showbase << file_header->f_symptr << std::dec << std::endl <<
	"	number of symtab entries: " << file_header->f_nsyms << std::endl <<
	"	size of opthdr: " << file_header->f_opthdr << std::endl <<
	"	flags: " << std::hex << std::showbase << file_header->f_flags << std::dec << std::endl;
#endif

	// check magic number
	if (!ISCOFF(file_header->f_magic)) {
		soclib::endian::uint16_swap(file_header->f_magic);
		if (!ISCOFF(file_header->f_magic)) {
			std::ostringstream oss;
			oss << file_header->f_magic;
			std::string magic = oss.str();
			throw soclib::exception::RunTimeError(std::string("Bad magic number ")+magic+
					std::string(" in target binary ")+filename+
					std::string(" (not an executable)") );
		}
		byte_swapped = true;

		// swap the rest of the header
		soclib::endian::uint16_swap(file_header->f_nscns);
		soclib::endian::uint32_swap(file_header->f_timdat);
		soclib::endian::uint32_swap(file_header->f_symptr);
		soclib::endian::uint32_swap(file_header->f_nsyms);
		soclib::endian::uint16_swap(file_header->f_opthdr);
		soclib::endian::uint16_swap(file_header->f_flags);
		soclib::endian::uint16_swap(file_header->f_target_id);
	}
	nbOfSections = file_header->f_nscns;
}


void process_file_optional_header() {

	coff_file_optional_header * file_optional_header;

	// goto the optional header
	// location of file_optional_header does not rely on sizeof(coff_file_header)
	// we only have to read 22 bytes
	file_ptr = file_ptr + 22;
	file_optional_header = (coff_file_optional_header *) file_ptr;

	// swap bytes in the file_optional_header if necessary
	if (byte_swapped) {
		soclib::endian::uint16_swap(file_optional_header->magic);
		soclib::endian::uint16_swap(file_optional_header->vstamp);
		soclib::endian::uint32_swap(file_optional_header->tsize);
		soclib::endian::uint32_swap(file_optional_header->dsize);
		soclib::endian::uint32_swap(file_optional_header->bsize);
		soclib::endian::uint32_swap(file_optional_header->entrypt);
		soclib::endian::uint32_swap(file_optional_header->text_start);
		soclib::endian::uint32_swap(file_optional_header->data_start);
	}

#if DEBUG_COFF
	std::cout << "Byte swapped: " << byte_swapped << std::endl;
	std::cout << "Optional File header" << std::endl;
	std::cout << "	Type of file: " << std::hex << std::showbase << file_optional_header->magic << std::dec << std::endl <<
	"	version stamp: " << file_optional_header->vstamp << std::endl <<
	"	text size (in bytes): " << file_optional_header->tsize << std::endl <<
	"	initialized data: " << file_optional_header->dsize << std::dec << std::endl <<
	"	uninitialized data : " << file_optional_header->bsize << std::endl <<
	"	entry pt: " << file_optional_header->entrypt << std::endl <<
	"	base of text used for this file: " << std::hex << std::showbase << file_optional_header->text_start << std::dec << std::endl <<
	"	base of data used for this file: " << std::hex << std::showbase << file_optional_header->data_start << std::dec << std::endl;
#endif

	// ready to iterate through the section headers and then the sections (see load)
	file_ptr = file_ptr + sizeof(coff_file_optional_header);
	file_ptr_saved = file_ptr;
	file_ptr_sections = file_ptr;
}

void add_section(coff_file_section_header *sect, coff_state *state, char * ptr)
{
	uint32_t flags = 0;
    void * blob;

	if ( sect->s_flags & COFF_STYP_REG) flags |= BinaryFileSection::FLAG_LOAD;
	if ( sect->s_flags & COFF_STYP_TEXT ) flags |= BinaryFileSection::FLAG_CODE;
	if ( sect->s_flags & COFF_STYP_DATA ) flags |= BinaryFileSection::FLAG_DATA;

#if DEBUG_COFF
	std::cout << "	Section " << std::string(sect->s_name) << " starting at " <<
	std::hex << std::showbase << sect->s_vaddr << " with " << std::dec <<
	sect->s_size << " bytes " << std::endl;
#endif

	blob = malloc(sect->s_size);
	if (blob == NULL && sect->s_size)
		throw soclib::exception::RunTimeError(std::string("Error no memory"));

	memcpy(blob, ptr, sect->s_size);

	state->loader->addSection(
			BinaryFileSection(
					sect->s_name,
					sect->s_vaddr & state->mask, sect->s_paddr & state->mask,
					//sect->s_vaddr, sect->s_paddr,
					flags,
					sect->s_size, blob));

#if DEBUG_COFF
	for (unsigned int j = 0; j < sect->s_size; j+=4) {
		uint8_t a, b, c, d;
		uint32_t data;
		a = *(ptr+j);
		b = *(ptr+j+1);
		c = *(ptr+j+2);
		d = *(ptr+j+3);
		data = (d << 24) | (c << 16) | (b << 8) | (a);
		std::cout << "	" << std::hex << std::showbase << "(" << sect->s_vaddr + j << ") " << data << std::dec << std::endl;
	}
#endif
}


void map_over_sections(coff_state *state) {

	coff_file_section_header * file_section_header;

#if DEBUG_COFF
	std::cout << "Processing " << nbOfSections << " sections" << std::endl;
#endif

	file_ptr = file_ptr_sections;

	for (int section = 0; section < nbOfSections; section++) {

		file_section_header = (coff_file_section_header *) file_ptr;
		file_ptr += sizeof(coff_file_section_header);
		file_ptr_saved = file_ptr;

		file_ptr = coff_buffer + file_section_header->s_scnptr;

		/* swap bytes in the file_section_header if necessary, except s_name */
		if (byte_swapped) {
			soclib::endian::uint32_swap(file_section_header->s_paddr);
			soclib::endian::uint32_swap(file_section_header->s_vaddr);
			soclib::endian::uint32_swap(file_section_header->s_size);
			soclib::endian::uint32_swap(file_section_header->s_scnptr);
			soclib::endian::uint32_swap(file_section_header->s_relptr);
			soclib::endian::uint32_swap(file_section_header->s_lnnoptr);
			soclib::endian::uint32_swap(file_section_header->s_nreloc);
			soclib::endian::uint32_swap(file_section_header->s_nlnno);
			soclib::endian::uint32_swap(file_section_header->s_flags);
			soclib::endian::uint16_swap(file_section_header->s_reserved);
			soclib::endian::uint16_swap(file_section_header->s_page);
		}

#if DEBUG_COFF
		std::cout << "section " << section << std::endl;
		std::cout << "	Section header" << std::endl <<
		"		name: " << std::string(file_section_header->s_name) << std::endl <<
		"		physical address: " << std::hex << std::showbase << file_section_header->s_paddr << std::endl <<
		"		virtual address: " << file_section_header->s_vaddr << std::endl << std::dec <<
		"		section size: " << file_section_header->s_size << std::endl <<
		"		file ptr ro raw data for section: " << std::hex << std::showbase << file_section_header->s_scnptr << std::endl <<
		"		file ptr to relocation: " << file_section_header->s_relptr << std::endl <<
		"		file ptr to line numbers: " << file_section_header->s_lnnoptr << std::endl << std::dec <<
		"		number of relocation entries: " << file_section_header->s_nreloc << std::endl <<
		"		number of line number entries: " << file_section_header->s_nlnno << std::endl <<
		"		flags: " << std::hex << std::showbase << file_section_header->s_flags << std::dec << std::endl;
#endif
		// is it a relevant section
		if (!(file_section_header->s_flags & COFF_STYP_COPY))  {

#if DEBUG_COFF
			std::cout << "section " << section << std::endl;
			std::cout << "	Section header" << std::endl <<
			"		name: " << std::string(file_section_header->s_name) << std::endl <<
			"		physical address: " << std::hex << std::showbase << file_section_header->s_paddr << std::endl <<
			"		virtual address: " << file_section_header->s_vaddr << std::endl << std::dec <<
			"		section size: " << file_section_header->s_size << std::endl <<
			"		file ptr ro raw data for section: " << std::hex << std::showbase << file_section_header->s_scnptr << std::endl <<
			"		file ptr to relocation: " << file_section_header->s_relptr << std::endl <<
			"		file ptr to line numbers: " << file_section_header->s_lnnoptr << std::endl << std::dec <<
			"		number of relocation entries: " << file_section_header->s_nreloc << std::endl <<
			"		number of line number entries: " << file_section_header->s_nlnno << std::endl <<
			"		flags: " << std::hex << std::showbase << file_section_header->s_flags << std::dec << std::endl;
#endif

			if (file_section_header->s_size > 0) {
#if DEBUG_COFF
				for (unsigned int j = 0; j < file_section_header->s_size; j+=4) {
					uint8_t a, b, c, d;
					uint32_t data;
					a = *(file_ptr+j);
					b = *(file_ptr+j+1);
					c = *(file_ptr+j+2);
					d = *(file_ptr+j+3);
					data = (d << 24) | (c << 16) | (b << 8) | (a);

					std::cout << "	" << std::hex << std::showbase << "(" << file_section_header->s_vaddr + j << ") " << data << std::dec << std::endl;
				}
#endif
				add_section(file_section_header, state, file_ptr);
			}
		}
		file_ptr = file_ptr_saved;
	}
}

bool coff_load( const std::string &filename, Loader &loader )
{
	struct coff_state state;
	state.loader = &loader;
	state.mask = (uintptr_t)-1;

	struct stat file_stat;
	std::ifstream coff_file;
	std::ifstream::pos_type size_coff_file;

	// open the file for reading
	coff_file.open(filename.c_str(), std::ios::in|std::ios::ate);
	if (!coff_file.is_open()) {
		throw soclib::exception::RunTimeError(std::string("Cant open binary image ")+filename+
				std::string(" for reading "));
	}
	// is it a non empty file ? look at the size of the binary file
	if (stat(filename.c_str(), &file_stat)!= 0) {
		throw soclib::exception::RunTimeError(std::string("Cant open binary image ")+filename+
				std::string(" for getting file size "));
	}

	// reading of the complete binary file
	size_coff_file = coff_file.tellg();
	coff_buffer = new char [size_coff_file];
	coff_file.seekg(0, std::ios::beg);
	coff_file.read(coff_buffer, size_coff_file);
	coff_file.close();
	file_ptr = coff_buffer;
#if DEBUG_COFF
	std::cout << "Target binary " << filename << " read in " << size_coff_file << " bytes" << std::endl;
#endif
	process_file_header(filename);
	process_file_optional_header();
	map_over_sections(&state);

	return true;
}

STATIC_INIT_CODE(
		Loader::register_loader("coff", coff_load);
)

}}
// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

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
 * TMS320C6X Instruction Set Simulator for the TMS320C6X processor core
 * developed for the SocLib Projet
 * 
 * Copyright (C) IRISA/INRIA, 2008
 *         Francois Charot <charot@irisa.fr>
 *
 * 
 * Maintainer: charot
 *
 * Functional description:
 * The following files: 
 * 				tms320c62.h
 *              tms320c62.cpp
 *              tms320c62_instructions.cpp
 *              tms320c62_decoding.cpp
 *
 * define the Instruction Set Simulator for the TMS320C62 processor.
 *
 * 
 */

#include "tms320c62.h"

namespace soclib {
namespace common {

void Tms320C6xIss::print_creg(uint32_t inst) {
	char creg, z;

	creg = inst >> 29 & 0x7;
	z = inst >> 28 & 0x1;

	if (creg == 0 && z == 0) {
		std::cout << "          ";
	} else {
		if (z == 1)
			std::cout << "    [!";
		else
			std::cout << "    [ ";
		if (creg == CREG_B0)
			std::cout << "B0] ";
		else if (creg == CREG_B1)
			std::cout << "B1] ";
		else if (creg == CREG_B2)
			std::cout << "B2] ";
		else if (creg == CREG_A1)
			std::cout << "A1] ";
		else if (creg == CREG_A2)
			std::cout << "A2] ";
	}
}

void Tms320C6xIss::iprint(const char *inst, int32_t src1, int32_t src2,
		int32_t dst, const char *unit, uint8_t s, char x, char src1or2) {
	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << ((x==1 && src1or2
			==1) ? ab[!s] : ab[s]) << src1 << ", "
			<< ((x==1 && src1or2==2) ? ab[!s] : ab[s]) << src2 << ", " << ab[s]
			<< dst;
}

void Tms320C6xIss::iprintc1(const char *inst, int32_t src1, int32_t src2,
		int32_t dst, const char *unit, uint8_t s, char x, char src1or2) {
	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << std::hex << src1
			<< std::dec << " " << ((x==1 && src1or2==2) ? ab[!s] : ab[s])
			<< src2 << ", " << ab[s] << dst;
}

void Tms320C6xIss::iprintc2(const char *inst, int32_t src1, int32_t src2,
		int32_t dst, const char *unit, uint8_t s, char x, char src1or2) {
	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << ((x==1 && src1or2
			==1) ? ab[!s] : ab[s]) << src1 << ", " << src2 << ", " << ab[s]
			<< dst;
}

void Tms320C6xIss::iprintld(const char *inst, int32_t src1, int32_t src2,
		int32_t dst, const char *unit, uint8_t s, uint8_t x) {

	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << ab[x] << src2
			<< "[ " << ab[x] << src1 << " ]," << ab[s] << dst;
}

void Tms320C6xIss::iprintldc(const char *inst, int32_t src1, int32_t src2,
		int32_t dst, const char *unit, uint8_t s, uint8_t x) {

	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << ab[x] << src2
			<< "[ " << src1 << " ]," << ab[s] << dst;
}
void Tms320C6xIss::iprintst(const char *inst, int32_t src1, int32_t src2,
		int32_t dst, const char *unit, uint8_t s, uint8_t x) {

	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << ab[s] << dst
			<< ", " << ab[x] << src2 << "[ " << ab[x] << src1 << " ]";
}

void Tms320C6xIss::iprintstc(const char *inst, int32_t src1, int32_t src2,
		int32_t dst, const char *unit, uint8_t s, uint8_t x) {

	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << ab[s] << dst
			<< ", " << ab[x] << src2 << "[ " << src1 << " ]";
}

void Tms320C6xIss::iprint4(const char *inst, int32_t csta, int32_t cstb,
		int32_t src2, int32_t dst, const char *unit, uint8_t s, char x,
		char src1or2) {
	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << csta << ", "
			<< cstb << ", " << ((x==1 && src1or2==2) ? ab[!s] : ab[s]) << ", "
			<< src2 << ab[s] << dst;
}

void Tms320C6xIss::iprint2(const char *inst, int32_t src2, int32_t dst,
		const char *unit, uint8_t s, char x, char src1or2) {
	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << ((x==1 && src1or2
			==2) ? ab[!s] : ab[s]) << ", " << src2 << ab[s] << dst;
}

void Tms320C6xIss::iprint2c(const char *inst, int32_t src2, int32_t dst,
		const char *unit, uint8_t s, char x, char src1or2) {
	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << std::hex << src2
			<< std::dec << ", " << ab[s] << dst;
}

void Tms320C6xIss::iprint1(const char *inst, int32_t dst, const char *unit,
		uint8_t s, char x, char src1or2) {
	char ab[3] = "AB";

	std::cout << inst << " " << "." << unit << s+1 << " " << ab[s] << dst;
}

void Tms320C6xIss::iprint1c(const char *inst, int32_t dst, const char *unit,
		char s, char x, char src1or2) {

	std::cout << inst << " " << "." << unit << s+1 << " " << std::hex << dst
			<< std::dec;
}

void Tms320C6xIss::iprint0(const char *inst) {
	std::cout << inst;
}

void Tms320C6xIss::iprint01(const char *inst, uint32_t num) {
	std::cout << inst << " " << num;
}

void Tms320C6xIss::instruction_print(uint32_t inst) {
	//instruction print
	//decode instructions, source & destination registers
	//and print them out
	//

	char creg, z, op = 0, unit = 0;
	char dst, src1, src2, x, s;
	uint32_t uicst1, uitmp;
	int32_t sicst1;
	int64_t slcst1;
	uint64_t ulcst1;
	char y, mode;

	creg = (inst >> 29) & 0x07;
	z = (inst >> 28) & 0x01;
	dst = (inst >> 23) & 0x1f;
	src2 = (inst >> 18) & 0x1f;
	src1 = (inst >> 13) & 0x1f;
	x = 0;
	s = (inst >> 1) & 0x01;
	if (getUnit_3bit(inst) == LUNIT) {
		// .L unit instruction
		x = (inst >> 12) & 0x01;
		op = (inst >> 5) & 0x7f;
		unit = LUNIT;

	} else if (getUnit_5bit(inst) == MUNIT) {
		// .M unit instruction
		x = (inst >> 12) & 0x01;
		op = (inst >> 7) & 0x1f;
		unit = MUNIT;

	} else if (getUnit_5bit(inst) == DUNIT) {
		// .D unit instruction
		op = (inst >> 7) & 0x3f;
		unit = DUNIT;

	} else if (getUnit_2bit(inst) == DUNIT_LDSTOFFSET) {
		// .D unit load/store
		op = (inst >> 4) & 0x07;
		unit = DUNIT_LDSTOFFSET;

	} else if (getUnit_2bit(inst) == DUNIT_LDSTBASEROFFSET) {
		// .D unit load/store with baseR/offsetR specified
		op = (inst >> 4) & 0x07;
		unit = DUNIT_LDSTBASEROFFSET;

	} else if (getUnit_4bit(inst) == SUNIT) {
		// .S unit instruction
		x = (inst >> 12) & 0x01;
		op = (inst >> 6) & 0x3f;
		unit = SUNIT;

	} else if (getUnit_5bit(inst) == SUNIT_ADDK) {
		// .S unit ADDK instruction
		op = 0; //nothing specified
		unit = SUNIT_ADDK;

	} else if (getUnit_4bit(inst) == SUNIT_IMMED) {
		// .S unit Field operations (immediate forms)
		op = (inst >> 6) & 0x03;
		unit = SUNIT_IMMED;

	} else if (getUnit_4bit(inst) == SUNIT_MVK) {
		// .S unit MVK
		op = 0; // nothing specified
		unit = SUNIT_MVK;

	} else if (getUnit_5bit(inst) == SUNIT_BCOND) {
		// .S unit Bcond disp
		op = 0; // nothing specified
		unit = SUNIT_BCOND;

	} else if (getUnit_11bit(inst) == NOP) {
		// NOP instruction
		op = 0;
		unit = 0;

	} else if (getUnit_16bit(inst) == IDLEINST) {
		op = IDLEOP;
		unit = IDLEUNIT;
	} else {
		/* unknown instruction kind */
		std::cout
				<< "unknown instruction type encountered in instruction_print function "
				<< std::endl;
	}

	print_creg(inst);

	//find constants
	sicst1 = ((int32_t) src1 << 27) >> 27; /*taking care of sign bit extn*/
	uicst1 = (uint32_t) src1;
	slcst1 = ((int64_t) src1 << 59) >> 59; /*taking care of sign bit extn*/
	ulcst1 = (uint64_t) src1;

	if (op == 0x1a && unit == LUNIT) {
		/*ABS xsint, sint*/
		iprint2("ABS {xsint, sint}", src2, dst, "L", s, x, 2);

	} else if (op == 0x38 && unit == LUNIT) {
		/*ABS slong, slong*/
		iprint2("ABS {slong, slong}", src2, dst, "L", s, x, 0);

	} else if (op == 0x03 && unit == LUNIT) {
		/*ADD sint, xsint, sint*/
		iprint("ADD {sint, xsint, sint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x07 && unit == SUNIT) {
		/*ADD sint, xsint, sint*/
		iprint("ADD {sint, xsint, sint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x23 && unit == LUNIT) {
		/*ADD sint, xsint, slong*/
		iprint("ADD {sint, xsint, slong}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x2b && unit == LUNIT) {
		/*ADDU usint, xusint, uslong*/
		iprint("ADDU {usint, xusint, uslong}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x21 && unit == LUNIT) {
		/*ADD xsint, slong, slong*/
		iprint("ADD {xsint, slong, slong}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x29 && unit == LUNIT) {
		/*ADDU xusint, uslong, uslong*/
		iprint("ADD {xusint, uslong, uslong}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x02 && unit == LUNIT) {
		/*ADD scst5, xsint, sint*/
		iprintc1("ADD {scst5, xsint, sint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x06 && unit == SUNIT) {
		/*ADD scst5, xsint, sint*/
		iprintc1("ADD {scst5, xsint, sint}", sicst1, src2, dst, "S", s, x, 2);

	} else if (op == 0x20 && unit == LUNIT) {
		/*ADD scst5, slong, slong*/
		iprintc1("ADD {scst5, slong, slong}", sicst1, src2, dst, "L", s, x, 0);

	} else if (op == 0x10 && unit == DUNIT) {
		/* ADD sint, sint, sint*/
		iprint("ADD {sint, sint, sint}", src1, src2, dst, "D", s, x, 0);

	} else if (op == 0x12 && unit == DUNIT) {
		/* ADD ucst5, sint, sint*/
		iprintc1("ADD {ucst5, sint, sint}", uicst1, src2, dst, "D", s, x, 0);

	} else if (op == 0x30 && unit == DUNIT) {
		/*ADDAB Add Byte with Addressing sint, sint, sint*/
		iprintld("ADDAB {+baseR[offsetR], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x34 && unit == DUNIT) {
		/*ADDAH Add Byte with Addressing sint, sint, sint*/
		iprintld("ADDAH {+baseR[offsetR], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x38 && unit == DUNIT) {
		/*ADDAW Add Byte with Addressing sint, sint, sint*/
		iprintld("ADDAW {+baseR[offsetR], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x32 && unit == DUNIT) {
		/*ADDAB Add Byte with Addressing ucsut5, sint, sint*/
		iprintldc("ADDAB {+baseR[offset], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x36 && unit == DUNIT) {
		/*ADDAH Add Byte with Addressing ucsut5, sint, sint*/
		iprintldc("ADDAH {+baseR[offset], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x3a && unit == DUNIT) {
		/*ADDAW Add Byte with Addressing ucsut5, sint, sint*/
		iprintldc("ADDAW {+baseR[offset], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0 && unit == SUNIT_ADDK) {
		/*ADDK cst16, usint*/
		iprint2c("ADDK {cst16, usint}", ((inst >> 7 &0x0000ffff) << 16) >> 16,
				dst, "S", s, x, 0);

	} else if (op == 0x01 && unit == SUNIT) {
		/*ADD2 sint, xsint, sint*/
		iprint("ADD2 {sint, xsint, sint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x7b && unit == LUNIT) {
		/*AND usint, xusint, usint*/
		iprint("AND {usint, xusint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x1f && unit == SUNIT) {
		/*AND usint, xusint, usint*/
		iprint("AND {usint, xusint, usint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x7a && unit == LUNIT) {
		/*AND scst5, xusint, usint*/
		iprintc1("AND {scst5, xusint, usint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x1e && unit == SUNIT) {
		/*AND scst5, xusint, usint*/
		iprintc1("AND {scst5, xusint, usint}", sicst1, src2, dst, "S", s, x, 2);

	} else if (op == 0x00 && unit == SUNIT_BCOND) {
		/*Branch Using a Displacement*/
		iprint1c("B {scst21}", (((inst >> 7) & 0x001fffff) << 11) >> 11, "S",
				s, x, 0);

	} else if (op == 0x0d && unit == SUNIT) {
		/*Branch using a Register*/
		iprint1("B {xusint}", src2, "S", s, x, 2);

	} else if (op == 0x03 && unit == SUNIT &&src2 == 0x06) {
		/*Branch Using an IRP - Interrupt Return Pointer*/
		iprint0("B IRP ");

	} else if (op == 0x03 && unit == SUNIT && src2 == 0x07) {
		/*Branch using NMI Return Pointer & set NMIE in crf[IER]*/
		iprint0("B NRP ");

	} else if (op == 0x03 && unit == SUNIT_IMMED) {
		/*CLR Clear Bit Fields of src2 whose bounds are given by csta & cstb*/
		/*ucst5, ucst5, usint, usint*/
		iprint4("CLR {ucst5, ucst5, usint, usint}", ((inst >> 13) & 0x1f),
				((inst >> 8) & 0x1f), src2, dst, "S", s, x, 0);

	} else if (op == 0x3f && unit == SUNIT) {
		/*CLR Clear Bit Fields of src2 whose bounds are given by 5-9 & 0-4 of src1*/
		/*usint, xusint, usint*/
		iprint("CLR {usint, xusint, usint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x53 && unit == LUNIT) {
		/*CMPEQ sint, xsint, usint*/
		iprint("CMPEQ {sint, xsint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x52 && unit == LUNIT) {
		/*CMPEQ scst5, xsint, usint*/
		iprintc1("CMPEQ {scst5, xsint, usint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x51 && unit == LUNIT) {
		/*CMPEQ xsint, slong, usint*/
		iprint("CMPEQ {xsint, slong, usint}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x50 && unit == LUNIT) {
		/*CMPEQ scst5, slong, usint*/
		iprintc1("CMPEQ {scst5, slong, usint}", sicst1, src2, dst, "L", s, x, 0);

	} else if (op == 0x47 && unit == LUNIT) {
		/*CMPGT sint, xsint, usint*/
		iprint("CMPGT {sint, xsint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x46 && unit == LUNIT) {
		/*CMPGT scst5, xsint, usint*/
		iprintc1("CMPGT {scst5, xsint, usint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x45 && unit == LUNIT) {
		/*CMPGT xsint, slong, usint*/
		iprint("CMPGT {xsint, slong, usint}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x44 && unit == LUNIT) {
		/*CMPGT scst5, slong, usint*/
		iprintc1("CMPGT {scst5, slong, usint}", sicst1, src2, dst, "L", s, x, 0);

	} else if (op == 0x4f && unit == LUNIT) {
		/*CMPGTU usint, xusint, usint*/
		iprint("CMPGTU {usint, xusint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x4e && unit == LUNIT) {
		/*CMPGTU ucst5, xusint, usint*/
		iprintc1("CMPGTU {ucst5, xusint, usint}", uicst1, src2, dst, "L", s, x,
				2);

	} else if (op == 0x4d && unit == LUNIT) {
		/*CMPGTU xusint, uslong, usint*/
		iprint("CMPGTU {xusint, uslong, usint}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x4c && unit == LUNIT) {
		/*CMPGTU ucst5, uslong, usint*/
		iprintc1("CMPGTU {ucst5, uslong, usint}", uicst1, src2, dst, "L", s, x,
				0);

	} else if (op == 0x57 && unit == LUNIT) {
		/*CMPLT sint, xsint, usint*/
		iprint("CMPLT {sint, xsint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x56 && unit == LUNIT) {
		/*CMPLT scst5, xsint, usint*/
		iprintc1("CMPLT {scst5, xsint, usint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x55 && unit == LUNIT) {
		/*CMPLT xsint, slong, usint*/
		iprint("CMPLT {xsint, slong, usint}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x54 && unit == LUNIT) {
		/*CMPLT scst5, slong, usint*/
		iprintc1("CMPLT {scst5, slong, usint}", sicst1, src2, dst, "L", s, x, 0);

	} else if (op == 0x5f && unit == LUNIT) {
		/*CMPLTU usint, xusint, usint*/
		iprint("CMPLTU {usint, xusint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x5e && unit == LUNIT) {
		/*CMPLTU ucst5, xusint, usint*/
		iprintc1("CMPLTU {ucst5, xusint, usint}", uicst1, src2, dst, "L", s, x,
				2);

	} else if (op == 0x5d && unit == LUNIT) {
		/*CMPLTU xusint, uslong, usint*/
		iprint("CMPLTU {xusint, uslong, usint}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x5c && unit == LUNIT) {
		/*CMPLTU ucst5, uslong, usint*/
		iprintc1("CMPLTU {ucst5, uslong, usint}", uicst1, src2, dst, "L", s, x,
				0);

	} else if (op == 0x01 && unit == SUNIT_IMMED) {
		/*EXT Extract & Sign Extend a bit field*/
		/*ucst5, ucst5, sint, sint*/
		iprint4("EXT {ucst5, ucst5, sint, sint}", ((inst >> 13) & 0x1f), ((inst
				>> 8) & 0x1f), src2, dst, "S", s, x, 0);

	} else if (op == 0x2f && unit == SUNIT) {
		/*EXT Extract & Sign Extend a bit field*/
		/*usint, xsint, sint*/
		iprint("EXT {usint, xsint, sint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x00 && unit == SUNIT_IMMED) {
		/*EXTU Extract & Zero Extend a bit field*/
		/*ucst5, ucst5, usint, usint*/
		iprint4("EXT {ucst5, ucst5, usint, usint}", ((inst >> 13) & 0x1f),
				((inst >> 8) & 0x1f), src2, dst, "S", s, x, 0);

	} else if (op == 0x2b && unit == SUNIT) {
		/*EXTU Extract & Zero Extend a bit field*/
		/*usint, xusint, usint*/
		iprint("EXT {usint, xusint, usint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x78 && unit == IDLEUNIT) {
		//**** TODO "IDLE"
		iprint0("IDLE");

	} else if (op == 0x02 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDB Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		switch (mode) {
		case 0x5:
			iprintld("LDB {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x4:
			iprintld("LDB {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xd:
			iprintld("LDB {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xc:
			iprintld("LDB {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xf:
			iprintld("LDB {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xe:
			iprintld("LDB {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x1:
			iprintldc("LDB {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x0:
			iprintldc("LDB {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x9:
			iprintldc("LDB {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x8:
			iprintldc("LDB {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xb:
			iprintldc("LDB {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xa:
			iprintldc("LDB {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		default:
			iprintld("LDB **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;

		}

	} else if (op == 0x01 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDBU Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		switch (mode) {
		case 0x5:
			iprintld("LDBU {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x4:
			iprintld("LDBU {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xd:
			iprintld("LDBU {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xc:
			iprintld("LDBU {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xf:
			iprintld("LDBU {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xe:
			iprintld("LDBU {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x1:
			iprintldc("LDBU {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x0:
			iprintldc("LDBU {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x9:
			iprintldc("LDBU {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x8:
			iprintldc("LDBU {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xb:
			iprintldc("LDBU {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xa:
			iprintldc("LDBU {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		default:
			iprintld("LDBU **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;

		}

	} else if (op == 0x04 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDH Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		switch (mode) {
		case 0x5:
			iprintld("LDH {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x4:
			iprintld("LDH {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xd:
			iprintld("LDH {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xc:
			iprintld("LDH {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xf:
			iprintld("LDH {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xe:
			iprintld("LDH {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x1:
			iprintldc("LDH {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x0:
			iprintldc("LDH {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x9:
			iprintldc("LDH {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x8:
			iprintldc("LDH {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xb:
			iprintldc("LDH {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xa:
			iprintldc("LDH {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		default:
			iprintld("LDH **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;

		}

	} else if (op == 0x00 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDHU Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		switch (mode) {
		case 0x5:
			iprintld("LDHU {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x4:
			iprintld("LDHU {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xd:
			iprintld("LDHU {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xc:
			iprintld("LDHU {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xf:
			iprintld("LDHU {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xe:
			iprintld("LDHU {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x1:
			iprintldc("LDHU {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x0:
			iprintldc("LDHU {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x9:
			iprintldc("LDHU {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x8:
			iprintldc("LDHU {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xb:
			iprintldc("LDHU {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xa:
			iprintldc("LDHU {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		default:
			iprintld("LDHU **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;

		}

	} else if (op == 0x06 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDW Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		switch (mode) {
		case 0x5:
			iprintld("LDW {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x4:
			iprintld("LDW {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xd:
			iprintld("LDW {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xc:
			iprintld("LDW {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xf:
			iprintld("LDW {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xe:
			iprintld("LDW {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x1:
			iprintldc("LDW {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x0:
			iprintldc("LDW {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x9:
			iprintldc("LDW {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0x8:
			iprintldc("LDW {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xb:
			iprintldc("LDW {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		case 0xa:
			iprintldc("LDW {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;

		default:
			iprintld("LDW **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;

		}

	} else if (op == 0x02 && unit == DUNIT_LDSTOFFSET) {
		/*LDB Load Byte ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		iprintldc("LDB {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x01 && unit == DUNIT_LDSTOFFSET) {
		/*LDBU Load Byte ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 0;
		iprintldc("LDBU {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x04 && unit == DUNIT_LDSTOFFSET) {
		/*LDH Load HlafByte ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 1;
		iprintldc("LDH {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x00 && unit == DUNIT_LDSTOFFSET) {
		/*LDHU Load HalfByte ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 1;
		iprintldc("LDHU {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x06 && unit == DUNIT_LDSTOFFSET) {
		/*LDW Load Word ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 2;
		iprintldc("LDW {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x6b && unit == LUNIT) {
		/*LMBD - Left Most Bit Detection usint, xusint, usint*/
		iprint("LMBD {usint, xusint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x6a && unit == LUNIT) {
		/*LMBD - Left Most Bit Detection ucst5, xusint, usint*/
		iprintc1("LMBD {ucst5, xusint, usint}", uicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x19 && unit == MUNIT) {
		/*MPY Integer Multiply 16lsb x 16lsb slsb16, xslsb16, sint*/
		iprint("MPY {slsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x1f && unit == MUNIT) {
		/*MPYU Integer Multiply 16lsb x 16lsb ulsb16, xulsb16, usint*/
		iprint("MPYU {ulsb16, xulsb16, usint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x1d && unit == MUNIT) {
		/*MPYUS Integer Multiply 16lsb x 16lsb ulsb16, xslsb16, sint*/
		iprint("MPYUS {ulsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x1b && unit == MUNIT) {
		/*MPYSU Integer Multiply 16lsb x 16lsb slsb16, xulsb16, sint*/
		iprint("MPYSU {slsb16, xulsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x18 && unit == MUNIT) {
		/*MPY Integer Multiply 16lsb x 16lsb scst5, xslsb16, sint*/
		iprintc1("MPY {scst5, xslsb16, sint}", sicst1, src2, dst, "M", s, x, 2);

	} else if (op == 0x1e && unit == MUNIT) {
		/*MPYSU Integer Multiply 16lsb x 16lsb scst5, xulsb16, sint*/
		iprintc1("MPYSU {scst5, xulsb16, sint}", sicst1, src2, dst, "M", s, x,
				2);

	} else if (op == 0x01 && unit == MUNIT) {
		/*MPYH Integer Multiply 16msb x 16msb smsb16, xsmsb16, sint*/
		iprint("MPYH {smsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x07 && unit == MUNIT) {
		/*MPYHU Integer Multiply 16msb x 16msb umsb16, xumsb16, usint*/
		iprint("MPYHU {umsb16, xumsb16, usint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x05 && unit == MUNIT) {
		/*MPYHUS Integer Multiply 16msb x 16msb umsb16, xsmsb16, sint*/
		iprint("MPYHUS {umsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x03 && unit == MUNIT) {
		/*MPYHSU Integer Multiply 16msb x 16msb smsb16, xumsb16, sint*/
		iprint("MPYHSU {smsb16, xumsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x09 && unit == MUNIT) {
		/*MPYHL Integer Multiply 16msb x 16lsb smsb16, xslsb16, sint*/
		iprint("MPYHL {smsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x0f && unit == MUNIT) {
		/*MPYHLU Integer Multiply 16msb x 16lsb umsb16, xulsb16, usint*/
		iprint("MPYHLU {umsb16, xulsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x0d && unit == MUNIT) {
		/*MPYHULS Integer Multiply 16msb x 16lsb umsb16, xslsb16, sint*/
		iprint("MPYHULS {umsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x0b && unit == MUNIT) {
		/*MPYHSLU Integer Multiply 16msb x 16lsb smsb16, xulsb16, sint*/
		iprint("MPYHSLU {smsb16, xulsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x11 && unit == MUNIT) {
		/*MPYLH Integer Multiply 16lsb x 16msb slsb16, xsmsb16, sint*/
		iprint("MPYLH {slsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x17 && unit == MUNIT) {
		/*MPYLHU Integer Multiply 16lsb x 16msb ulsb16, xumsb16, usint*/
		iprint("MPYLHU {ulsb16, xumsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x15 && unit == MUNIT) {
		/*MPYLUHS Integer Multiply 16lsb x 16msb ulsb16, xsmsb16, sint*/
		iprint("MPYLUHS {ulsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x11 && unit == MUNIT) {
		/*MPYLSHU Integer Multiply 16lsb x 16msb slsb16, xumsb16, sint*/
		iprint("MPYLSHU {slsb16, xumsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x0f && unit == SUNIT) {
		/*MVC Move between Control File & Register File usint, usint*/
		iprint2("MVC {usint, usint}", src2, dst, "S", s, x, 0);

	} else if (op == 0x0e && unit == SUNIT) {
		/*MVC Move between Control File & Register File xusint, usint*/
		iprint2("MVC {xusint, usint}", src2, dst, "S", s, x, 2);

	} else if (op == 0x00 && unit == SUNIT_MVK && (inst >> 6 & 0x01) == 0x00) {
		/*MVK Move a 16bit signed constant into a Register & Sign Extend scst16, sint*/
		iprint2c("MVK {scst16, sint}", ((inst >> 7 & 0x0000ffff) << 16) >>16,
				dst, "S", s, x, 0);

	} else if (op == 0x00 && unit == SUNIT_MVK && (inst >> 6 & 0x01) == 0x01) {
		/*MVKH Move 16bit constant into the Upper Bits of a Register uscst16, sint*/
		iprint2c("MVKH {uscst16, sint}", (((inst >> 7) & 0x0000ffff) << 16),
				dst, "S", s, x, 0);

	} else if (op == 0x00 && unit == 0x00) {
		iprint01("NOP", ((inst >> 13) & 0x000f) + 1);

	} else if (op == 0x63 && unit == LUNIT) {
		/*NORM Normalize Integer, # of redundant sign bits are found xsint, usint*/
		iprint2("NORM {xsint, usint}", src2, dst, "L", s, x, 2);

	} else if (op == 0x60 && unit == LUNIT) {
		/*NORM Normalize Integer, # of redundant sign bits are found slong, usint*/
		iprint2("NORM {slong, usint}", src2, dst, "L", s, x, 0);

	} else if (op == 0x7f && unit == LUNIT) {
		/*Bitwise OR usint, xusint, usint*/
		iprint("OR {usint, xusint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x1b && unit == SUNIT) {
		/*Bitwise OR usint, xusint, usint*/
		iprint("OR {usint, xusint, usint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x7e && unit == LUNIT) {
		/*Bitwise OR scst5, xusint, usint*/
		iprintc1("OR {scst5, xusint, usint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x1a && unit == SUNIT) {
		/*Bitwise OR scst5, xusint, usint*/
		iprintc1("OR {scst5, xusint, usint}", sicst1, src2, dst, "S", s, x, 2);

	} else if (op == 0x13 && unit == LUNIT) {
		/*SADD Integer addition with saturation to result size sint, xsint, sint*/
		iprint("SADD {sint, xsint, sint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x31 && unit == LUNIT) {
		/*SADD Integer addition with saturation to result size xsint, slong, slong*/
		iprint("SADD {xsint, slong, slong}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x12 && unit == LUNIT) {
		/*SADD Integer addition with saturation to result size scst5, xsint, sint*/
		iprintc1("SADD {scst5, xsint, sint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x30 && unit == LUNIT) {
		/*SADD Integer addition with saturation to result size scst5, slong, slong*/
		iprintc1("SADD {scst5, slong, slong}", sicst1, src2, dst, "L", s, x, 0);

	} else if (op == 0x40 && unit == LUNIT && src1 == 0x00) {
		/*SAT saturate a 40bit integer to a 32bit integer slong, sint*/
		iprint2("SAT {slong, sint}", src2, dst, "L", s, x, 0);

	} else if (op == 0x02 && unit == SUNIT_IMMED) {
		/*SET Set Bit Fields of src2 whose bounds are given by csta & cstb*/
		/*ucst5, ucst, usint, usint*/
		iprint4("SET {ucst5, ucst5, usint, usint}", ((inst >> 13) & 0x1f),
				((inst >> 8) & 0x1f), src2, dst, "S", s, x, 0);

	} else if (op == 0x3b && unit == SUNIT) {
		/*SET Set Bit Fields of src2 whose bounds are given by 5-9 & 0-4 of src1*/
		/*usint, xusint, usint*/
		iprint("SET {usint, xusint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x33 && unit == SUNIT) {
		/*SHL Shift Left by amount given in src1 usint, xsint, sint*/
		iprint("SHL {usint, xsint, sint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x31 && unit == SUNIT) {
		/*SHL Shift Left by amount given in src1 usint, slong, slong*/
		iprint("SHL {usint, slong, slong}", src1, src2, dst, "S", s, x, 0);

	} else if (op == 0x13 && unit == SUNIT) {
		/*SHL Shift Left by amount given in src1 usint, xusint, uslong*/
		iprint("SHL {usint, xusint, uslong}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x32 && unit == SUNIT) {
		/*SHL Shift Left by amount given in src1 ucst5, xsint, sint*/
		iprintc1("SHL {ucst5, xsint, sint}", uicst1, src2, dst, "S", s, x, 2);

	} else if (op == 0x30 && unit == SUNIT) {
		/*SHL Shift Left by amount given in src1 ucst5, slong, slong*/
		iprintc1("SHL {ucst5, slong, slong}", uicst1, src2, dst, "S", s, x, 0);

	} else if (op == 0x12 && unit == SUNIT) {
		/*SHL Shift Left by amount given in src1 ucst5, xusint, uslong*/
		iprintc1("SHL {ucst5, xusint, uslong}", uicst1, src2, dst, "S", s, x, 2);

	} else if (op == 0x37 && unit == SUNIT) {
		/*SHR Shift Right  by amount given in src1 usint, xsint, sint*/
		iprint("SHR {usint, xsint, sint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x35 && unit == SUNIT) {
		/*SHR Shift Right by amount given in src1 usint, slong, slong*/
		iprint("SHR {usint, slong, slong}", src1, src2, dst, "S", s, x, 0);

	} else if (op == 0x36 && unit == SUNIT) {
		/*SHR Shift Right by amount given in src1 ucst5, xsint, sint*/
		iprintc1("SHR {ucst5, xsint, sint}", uicst1, src2, dst, "S", s, x, 2);

	} else if (op == 0x34 && unit == SUNIT) {
		/*SHR Shift Right by amount given in src1 ucst5, slong, slong*/
		iprintc1("SHR {ucst5, slong, slong}", uicst1, src2, dst, "S", s, x, 0);

	} else if (op == 0x27 && unit == SUNIT) {
		/*SHRU Logical Shift Right by amount given in src1 usint, xusint, usint*/
		iprint("SHRU {usint, xusint, usint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x25 && unit == SUNIT) {
		/*SHRU Logical Shift Right by amount given in src1 usint, uslong, uslong*/
		iprint("SHRU {usint, uslong, uslong}", src1, src2, dst, "S", s, x, 0);

	} else if (op == 0x26 && unit == SUNIT) {
		/*SHRU Logical Shift Right by amount given in src1 ucst5, xusint, usint*/
		iprintc1("SHRU {ucst5, xusint, usint}", uicst1, src2, dst, "S", s, x, 2);

	} else if (op == 0x24 && unit == SUNIT) {
		/*SHRU Logical Shift Right by amount given in src1 ucst5, uslong, uslong*/
		iprintc1("SHRU {ucst5, uslong, uslong}", uicst1, src2, dst, "S", s, x,
				0);

	} else if (op == 0x1a && unit == MUNIT) {
		/*SMPY slsb16, xslsb16, sint*/
		iprint("SMPY {slsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x0a && unit == MUNIT) {
		/*SMPYHL smsb16, xslsb16, sint*/
		iprint("SMPYHL {smsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x12 && unit == MUNIT) {
		/*SMPYLH slsb16, xsmsb16, sint*/
		iprint("SMPYLH {slsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x02 && unit == MUNIT) {
		/*SMPYH smsb16, xsmsb16, sint*/
		iprint("SMPYH {smsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x23 && unit == SUNIT) {
		/*SSHL Shift Left with Saturation usint, xsint, sint*/
		iprint("SSHL {usint, xsint, sint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x22 && unit == SUNIT) {
		/*SSHL Shift Left with Saturation ucst5, xsint, sint*/
		iprintc1("SSHL {ucst5, xsint, sint}", uicst1, src2, dst, "S", s, x, 2);

	} else if (op == 0x0f && unit == LUNIT) {
		/*SSUB Integer addition with saturation to result size sint, xsint, sint*/
		iprint("SSUB {sint, xsint, sint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x1f && unit == LUNIT) {
		/*SSUB Integer addition with saturation to result size xsint, sint, sint*/
		iprint("SSUB {xsint, sint, sint}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x0e && unit == LUNIT) {
		/*SSUB Integer addition with saturation to result size scst5, xsint, sint*/
		iprintc1("SSUB {scst5, xsint, sint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x2c && unit == LUNIT) {
		/*SSUB Integer addition with saturation to result size scst5, slong, slong*/
		iprintc1("SSUB {scst5, slong, slong}", sicst1, src2, dst, "L", s, x, 0);

	} else if (op == 0x03 && unit == DUNIT_LDSTBASEROFFSET) {
		/*STB Store Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		switch (mode) {
		case 0x5:
			iprintst("STB {src, +baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0x4:
			iprintst("STB {src, -baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xd:
			iprintst("STB {src, ++baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xc:
			iprintst("STB {src, --baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xf:
			iprintst("STB {src, baseR++[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xe:
			iprintst("STB {src, baseR--[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0x1:
			iprintstc("STB {src, +baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0x0:
			iprintstc("STB {src, -baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0x9:
			iprintstc("STB {src, ++baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0x8:
			iprintstc("STB {src, --baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0xb:
			iprintstc("STB {src, baseR++[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0xa:
			iprintstc("STB {src, baseR--[offset]}", src1, src2, dst, "D", s, y);
			break;

		default:
			iprintst("STB **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;

		}

	} else if (op == 0x05 && unit == DUNIT_LDSTBASEROFFSET) {
		/*STH Store Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		switch (mode) {
		case 0x5:
			iprintst("STH {src, +baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0x4:
			iprintst("STH {src, -baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xd:
			iprintst("STH {src, ++baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xc:
			iprintst("STH {src, --baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xf:
			iprintst("STH {src, baseR++[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xe:
			iprintst("STH {src, baseR--[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0x1:
			iprintstc("STH {src, +baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0x0:
			iprintstc("STH {src, -baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0x9:
			iprintstc("STH {src, ++baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0x8:
			iprintstc("STH {src, --baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0xb:
			iprintstc("STH {src, baseR++[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0xa:
			iprintstc("STH {src, baseR--[offset]}", src1, src2, dst, "D", s, y);
			break;

		default:
			iprintst("STH **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;

		}

	} else if (op == 0x07 && unit == DUNIT_LDSTBASEROFFSET) {
		/*STW Store Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		switch (mode) {
		case 0x5:
			iprintst("STW {src, +baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0x4:
			iprintst("STW {src, -baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xd:
			iprintst("STW {src, ++baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xc:
			iprintst("STW {src, --baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xf:
			iprintst("STW {src, baseR++[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0xe:
			iprintst("STW {src, baseR--[offsetR]}", src1, src2, dst, "D", s, y);
			break;

		case 0x1:
			iprintstc("STW {src, +baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0x0:
			iprintstc("STW {src, -baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0x9:
			iprintstc("STW {src, ++baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0x8:
			iprintstc("STW {src, --baseR[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0xb:
			iprintstc("STW {src, baseR++[offset]}", src1, src2, dst, "D", s, y);
			break;

		case 0xa:
			iprintstc("STW {src, baseR--[offset]}", src1, src2, dst, "D", s, y);
			break;

		default:
			iprintst("STW **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;

		}

	} else if (op == 0x03 && unit == DUNIT_LDSTOFFSET) {
		/*STB Store Byte*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 0;
		iprintstc("STB {src, +baseR[offset]}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x05 && unit == DUNIT_LDSTOFFSET) {
		/*STH Store Halfword*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 1;
		iprintstc("STH {src, +baseR[offset]}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x07 && unit == DUNIT_LDSTOFFSET) {
		/*STW Store Word*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 2;
		iprintstc("STW {src, +baseR[offset]}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x07 && unit == LUNIT) {
		/*SUB without saturation sint, xsint, sint*/
		iprint("SUB {sint, xsint, sint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x17 && unit == SUNIT) {
		/*SUB without saturation sint, xsint, sint*/
		iprint("SUB {sint, xsint, sint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x17 && unit == LUNIT) {
		/*SUB without saturation xsint, sint, sint*/
		iprint("SUB {xsint, sint, sint}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x27 && unit == LUNIT) {
		/*SUB without saturation sint, xsint, slong*/
		iprint("SUB {sint, xsint, slong}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x37 && unit == LUNIT) {
		/*SUB without saturation xsint, sint, slong*/
		iprint("SUB {xsint, sint, slong}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x2f && unit == LUNIT) {
		/*SUBU without saturation usint, xusint, uslong*/
		iprint("SUB {usint, xusint, uslong}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x3f && unit == LUNIT) {
		/*SUBU without saturation xusint, usint, uslong*/
		iprint("SUB {xusint, usint, uslong}", src1, src2, dst, "L", s, x, 1);

	} else if (op == 0x06 && unit == LUNIT) {
		/*SUB without saturation scst5, xsint, sint*/
		iprintc1("SUB {scst5, xsint, sint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x16 && unit == SUNIT) {
		/*SUB without saturation scst5, xsint, sint*/
		iprintc1("SUB {scst5, xsint, sint}", sicst1, src2, dst, "S", s, x, 2);

	} else if (op == 0x24 && unit == LUNIT) {
		/*SUB without saturation scst5, slong, slong*/
		iprintc1("SUB {scst5, slong, slong}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x11 && unit == DUNIT) {
		/*SUB without saturation sint, sint, sint*/
		iprint("SUB {sint, sint, sint}", src1, src2, dst, "D", s, x, 0);

	} else if (op == 0x13 && unit == DUNIT) {
		/*SUB without saturation ucst5, sint, sint*/
		iprintc1("SUB {ucst5, sint, sint}", uicst1, src2, dst, "D", s, x, 0);

	} else if (op == 0x31 && unit == DUNIT) {
		/*SUBAB Add Byte with Addressing sint, sint, sint*/
		iprintld("SUBAB {+baseR[offsetR], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x35 && unit == DUNIT) {
		/*SUBAH Add Byte with Addressing sint, sint, sint*/
		iprintld("SUBAH {+baseR[offsetR], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x39 && unit == DUNIT) {
		/*SUBAW Add Byte with Addressing sint, sint, sint*/
		iprintld("SUBAW {+baseR[offsetR], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x33 && unit == DUNIT) {
		/*SUBAB Add Byte with Addressing ucsut5, sint, sint*/
		iprintldc("SUBAB {+baseR[offset], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x37 && unit == DUNIT) {
		/*SUBAH Add Byte with Addressing ucsut5, sint, sint*/
		iprintldc("SUBAH {+baseR[offset], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x3b && unit == DUNIT) {
		/*SUBAW Add Byte with Addressing ucsut5, sint, sint*/
		iprintldc("SUBAW {+baseR[offset], dst}", src1, src2, dst, "D", s, x);

	} else if (op == 0x4b && unit == LUNIT) {
		/*SUBC Conditional Integer Subtract and Shift - used for division usint, xusint, usint*/
		iprint("SUBC {usint, xusint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x11 && unit == SUNIT) {
		/*SUB2 Subtractions of lower and upper halfs sint, xsint, sint*/
		iprint("SUB2 {sint, xsint, sint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x6f && unit == LUNIT) {
		/*XOR usint, xusint, usint*/
		iprint("XOR {usint, xusint, usint}", src1, src2, dst, "L", s, x, 2);

	} else if (op == 0x0b && unit == SUNIT) {
		/*XOR usint, xusint, usint*/
		iprint("XOR {usint, xusint, usint}", src1, src2, dst, "S", s, x, 2);

	} else if (op == 0x6e && unit == LUNIT) {
		/*XOR scst5, xusint, usint*/
		iprintc1("XOR {scst5, xusint, usint}", sicst1, src2, dst, "L", s, x, 2);

	} else if (op == 0x0a && unit == SUNIT) {
		/*XOR scst5, xusint, usint*/
		iprintc1("XOR {scst5, xusint, usint}", sicst1, src2, dst, "S", s, x, 2);

	} else {
		iprint0("***UNIDENTIFIED INSTRUCTION***\n");

	}
}

void Tms320C6xIss::instruction_print_mpy(uint32_t inst) {
	/*instruction print
	 *decode instructions, source & destination registers
	 *and print them out
	 */

	char creg, z, op = 0, unit = 0;
	char dst, src1, src2, x, s;
	uint32_t uicst1;
	int32_t sicst1;
	int64_t slcst1;
	uint64_t ulcst1;

	creg = (inst >> 29) & 0x07;
	z = (inst >> 28) & 0x01;
	dst = (inst >> 23) & 0x1f;
	src2 = (inst >> 18) & 0x1f;
	src1 = (inst >> 13) & 0x1f;
	x = 0;
	s = (inst >> 1) & 0x01;
	if (getUnit_3bit(inst) == LUNIT) {
		/*.L unit instruction*/
		x = (inst >> 12) & 0x01;
		op = (inst >> 5) & 0x7f;
		unit = LUNIT;

	} else if (getUnit_5bit(inst) == MUNIT) {
		/*.M unit instruction*/
		x = (inst >> 12) & 0x01;
		op = (inst >> 7) & 0x1f;
		unit = MUNIT;

	} else if (getUnit_5bit(inst) == DUNIT) {
		/*.D unit instruction*/
		op = (inst >> 7) & 0x3f;
		unit = DUNIT;

	} else if (getUnit_2bit(inst) == DUNIT_LDSTOFFSET) {
		/*.D unit load/store*/
		op = (inst >> 4) & 0x07;
		unit = DUNIT_LDSTOFFSET;

	} else if (getUnit_2bit(inst) == DUNIT_LDSTBASEROFFSET) {
		/*.D unit load/store with baseR/offsetR specified*/
		op = (inst >> 4) & 0x07;
		unit = DUNIT_LDSTBASEROFFSET;

	} else if (getUnit_4bit(inst) == SUNIT) {
		/*.S unit instruction*/
		x = (inst >> 12) & 0x01;
		op = (inst >> 6) & 0x3f;
		unit = SUNIT;

	} else if (getUnit_5bit(inst) == SUNIT_ADDK) {
		/*.S unit ADDK instruction*/
		op = 0; /*nothing specified*/
		unit = SUNIT_ADDK;

	} else if (getUnit_4bit(inst) == SUNIT_IMMED) {
		/*.S unit Field operations (immediate forms)*/
		op = (inst >> 6) & 0x03;
		unit = SUNIT_IMMED;

	} else if (getUnit_4bit(inst) == SUNIT_MVK) {
		/*.S unit MVK*/
		op = 0; /*nothing specified*/
		unit = SUNIT_MVK;

	} else if (getUnit_5bit(inst) == SUNIT_BCOND) {
		/*.S unit Bcond disp*/
		op = 0; /*nothing specified*/
		unit = SUNIT_BCOND;

	} else if (getUnit_11bit(inst) == NOP) {
		/*NOP instruction*/
		op = 0;
		unit = 0;

	} else if (getUnit_16bit(inst) == IDLEINST) {
		op = IDLEOP;
		unit = IDLEUNIT;
	} else {
		/* unknown instruction kind */
		std::cout
				<< "unknown instruction type  encountered in instruction_print_mpy function "
				<< std::endl;
	}

	/*find constants*/
	sicst1 = ((int32_t) src1 << 27) >> 27; /*taking care of sign bit extn*/
	uicst1 = (uint32_t) src1;
	slcst1 = ((int64_t) src1 << 59) >> 59; /*taking care of sign bit extn*/
	ulcst1 = (uint64_t) src1;

	if (op == 0x19 && unit == MUNIT) {
		/*MPY Integer Multiply 16lsb x 16lsb slsb16, xslsb16, sint*/
		print_creg(inst);
		iprint("MPY {slsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x1f && unit == MUNIT) {
		/*MPYU Integer Multiply 16lsb x 16lsb ulsb16, xulsb16, usint*/
		print_creg(inst);
		iprint("MPYU {ulsb16, xulsb16, usint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x1d && unit == MUNIT) {
		/*MPYUS Integer Multiply 16lsb x 16lsb ulsb16, xslsb16, sint*/
		print_creg(inst);
		iprint("MPYUS {ulsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x1b && unit == MUNIT) {
		/*MPYSU Integer Multiply 16lsb x 16lsb slsb16, xulsb16, sint*/
		print_creg(inst);
		iprint("MPYSU {slsb16, xulsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x18 && unit == MUNIT) {
		/*MPY Integer Multiply 16lsb x 16lsb scst5, xslsb16, sint*/
		print_creg(inst);
		iprintc1("MPY {scst5, xslsb16, sint}", sicst1, src2, dst, "M", s, x, 2);

	} else if (op == 0x1e && unit == MUNIT) {
		/*MPYSU Integer Multiply 16lsb x 16lsb scst5, xulsb16, sint*/
		print_creg(inst);
		iprintc1("MPYSU {scst5, xulsb16, sint}", sicst1, src2, dst, "M", s, x,
				2);

	} else if (op == 0x01 && unit == MUNIT) {
		/*MPYH Integer Multiply 16msb x 16msb smsb16, xsmsb16, sint*/
		print_creg(inst);
		iprint("MPYH {smsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x07 && unit == MUNIT) {
		/*MPYHU Integer Multiply 16msb x 16msb umsb16, xumsb16, usint*/
		print_creg(inst);
		iprint("MPYHU {umsb16, xumsb16, usint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x05 && unit == MUNIT) {
		/*MPYHUS Integer Multiply 16msb x 16msb umsb16, xsmsb16, sint*/
		print_creg(inst);
		iprint("MPYHUS {umsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x03 && unit == MUNIT) {
		/*MPYHSU Integer Multiply 16msb x 16msb smsb16, xumsb16, sint*/
		print_creg(inst);
		iprint("MPYHSU {smsb16, xumsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x09 && unit == MUNIT) {
		/*MPYHL Integer Multiply 16msb x 16lsb smsb16, xslsb16, sint*/
		print_creg(inst);
		iprint("MPYHL {smsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x0f && unit == MUNIT) {
		/*MPYHLU Integer Multiply 16msb x 16lsb umsb16, xulsb16, usint*/
		print_creg(inst);
		iprint("MPYHLU {umsb16, xulsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x0d && unit == MUNIT) {
		/*MPYHULS Integer Multiply 16msb x 16lsb umsb16, xslsb16, sint*/
		print_creg(inst);
		iprint("MPYHULS {umsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x0b && unit == MUNIT) {
		/*MPYHSLU Integer Multiply 16msb x 16lsb smsb16, xulsb16, sint*/
		print_creg(inst);
		iprint("MPYHSLU {smsb16, xulsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x11 && unit == MUNIT) {
		/*MPYLH Integer Multiply 16lsb x 16msb slsb16, xsmsb16, sint*/
		print_creg(inst);
		iprint("MPYLH {slsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x17 && unit == MUNIT) {
		/*MPYLHU Integer Multiply 16lsb x 16msb ulsb16, xumsb16, usint*/
		print_creg(inst);
		iprint("MPYLHU {ulsb16, xumsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x15 && unit == MUNIT) {
		/*MPYLUHS Integer Multiply 16lsb x 16msb ulsb16, xsmsb16, sint*/
		print_creg(inst);
		iprint("MPYLUHS {ulsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x13 && unit == MUNIT) {
		/*MPYLSHU Integer Multiply 16lsb x 16msb slsb16, xumsb16, sint*/
		print_creg(inst);
		iprint("MPYLSHU {slsb16, xumsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x1a && unit == MUNIT) {
		/*SMPY slsb16, xslsb16, sint*/
		print_creg(inst);
		iprint("SMPY {slsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x0a && unit == MUNIT) {
		/*SMPYHL smsb16, xslsb16, sint*/
		print_creg(inst);
		iprint("SMPYHL {smsb16, xslsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x12 && unit == MUNIT) {
		/*SMPYLH slsb16, xsmsb16, sint*/
		print_creg(inst);
		iprint("SMPYLH {slsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	} else if (op == 0x02 && unit == MUNIT) {
		/*SMPYH smsb16, xsmsb16, sint*/
		print_creg(inst);
		iprint("SMPYH {smsb16, xsmsb16, sint}", src1, src2, dst, "M", s, x, 2);

	}
}

void Tms320C6xIss::instruction_print_ld(uint32_t inst) {
	/*instruction print
	 *decode instructions, source & destination registers
	 *and print them out
	 */

	char creg, z, op = 0, unit = 0;
	char dst, src1, src2, x, s;
	uint32_t uicst1, uitmp;
	int32_t sicst1;
	int64_t slcst1;
	uint64_t ulcst1;
	char y, mode;

	creg = (inst >> 29) & 0x07;
	z = (inst >> 28) & 0x01;
	dst = (inst >> 23) & 0x1f;
	src2 = (inst >> 18) & 0x1f;
	src1 = (inst >> 13) & 0x1f;
	x = 0;
	s = (inst >> 1) & 0x01;
	if (getUnit_3bit(inst) == LUNIT) {
		/*.L unit instruction*/
		x = (inst >> 12) & 0x01;
		op = (inst >> 5) & 0x7f;
		unit = LUNIT;

	} else if (getUnit_5bit(inst) == MUNIT) {
		/*.M unit instruction*/
		x = (inst >> 12) & 0x01;
		op = (inst >> 7) & 0x1f;
		unit = MUNIT;

	} else if (getUnit_5bit(inst) == DUNIT) {
		/*.D unit instruction*/
		op = (inst >> 7) & 0x3f;
		unit = DUNIT;

	} else if (getUnit_2bit(inst) == DUNIT_LDSTOFFSET) {
		/*.D unit load/store*/
		op = (inst >> 4) & 0x07;
		unit = DUNIT_LDSTOFFSET;

	} else if (getUnit_2bit(inst) == DUNIT_LDSTBASEROFFSET) {
		/*.D unit load/store with baseR/offsetR specified*/
		op = (inst >> 4) & 0x07;
		unit = DUNIT_LDSTBASEROFFSET;

	} else if (getUnit_4bit(inst) == SUNIT) {
		/*.S unit instruction*/
		x = (inst >> 12) & 0x01;
		op = (inst >> 6) & 0x3f;
		unit = SUNIT;

	} else if (getUnit_5bit(inst) == SUNIT_ADDK) {
		/*.S unit ADDK instruction*/
		op = 0; /*nothing specified*/
		unit = SUNIT_ADDK;

	} else if (getUnit_4bit(inst) == SUNIT_IMMED) {
		/*.S unit Field operations (immediate forms)*/
		op = (inst >> 6) & 0x03;
		unit = SUNIT_IMMED;

	} else if (getUnit_4bit(inst) == SUNIT_MVK) {
		/*.S unit MVK*/
		op = 0; /*nothing specified*/
		unit = SUNIT_MVK;

	} else if (getUnit_5bit(inst) == SUNIT_BCOND) {
		/*.S unit Bcond disp*/
		op = 0; /*nothing specified*/
		unit = SUNIT_BCOND;

	} else if (getUnit_11bit(inst) == NOP) {
		/*NOP instruction*/
		op = 0;
		unit = 0;

	} else if (getUnit_16bit(inst) == IDLEINST) {
		op = IDLEOP;
		unit = IDLEUNIT;
	} else {
		/* unknown instruction kind */
		std::cout
				<< "unknown instruction type encountered in instruction_print_ld function "
				<< std::endl;
	}

	/*find constants*/
	sicst1 = ((int32_t) src1 << 27) >> 27; /*taking care of sign bit extn*/
	uicst1 = (uint32_t) src1;
	slcst1 = ((int64_t) src1 << 59) >> 59; /*taking care of sign bit extn*/
	ulcst1 = (uint64_t) src1;

	if (op == 0x02 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDB Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		print_creg(inst);
		switch (mode) {
		case 0x5:
			iprintld("LDB {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x4:
			iprintld("LDB {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xd:
			iprintld("LDB {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xc:
			iprintld("LDB {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xf:
			iprintld("LDB {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xe:
			iprintld("LDB {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x1:
			iprintldc("LDB {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x0:
			iprintldc("LDB {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x9:
			iprintldc("LDB {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x8:
			iprintldc("LDB {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xb:
			iprintldc("LDB {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xa:
			iprintldc("LDB {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		default:
			iprintld("LDB **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;
		}

	} else if (op == 0x01 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDBU Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		print_creg(inst);
		switch (mode) {
		case 0x5:
			iprintld("LDBU {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x4:
			iprintld("LDBU {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xd:
			iprintld("LDBU {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xc:
			iprintld("LDBU {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xf:
			iprintld("LDBU {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xe:
			iprintld("LDBU {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x1:
			iprintldc("LDBU {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x0:
			iprintldc("LDBU {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x9:
			iprintldc("LDBU {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x8:
			iprintldc("LDBU {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xb:
			iprintldc("LDBU {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xa:
			iprintldc("LDBU {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		default:
			iprintld("LDBU **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;
		}

	} else if (op == 0x04 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDH Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		print_creg(inst);
		switch (mode) {
		case 0x5:
			iprintld("LDH {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x4:
			iprintld("LDH {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xd:
			iprintld("LDH {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xc:
			iprintld("LDH {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xf:
			iprintld("LDH {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xe:
			iprintld("LDH {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x1:
			iprintldc("LDH {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x0:
			iprintldc("LDH {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x9:
			iprintldc("LDH {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x8:
			iprintldc("LDH {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xb:
			iprintldc("LDH {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xa:
			iprintldc("LDH {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		default:
			iprintld("LDH **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;
		}

	} else if (op == 0x00 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDHU Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		print_creg(inst);
		switch (mode) {
		case 0x5:
			iprintld("LDHU {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x4:
			iprintld("LDHU {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xd:
			iprintld("LDHU {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xc:
			iprintld("LDHU {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xf:
			iprintld("LDHU {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xe:
			iprintld("LDHU {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x1:
			iprintldc("LDHU {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x0:
			iprintldc("LDHU {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x9:
			iprintldc("LDHU {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x8:
			iprintldc("LDHU {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xb:
			iprintldc("LDHU {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xa:
			iprintldc("LDHU {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		default:
			iprintld("LDHU **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;
		}

	} else if (op == 0x06 && unit == DUNIT_LDSTBASEROFFSET) {
		/*LDW Load Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		print_creg(inst);
		switch (mode) {
		case 0x5:
			iprintld("LDW {+baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x4:
			iprintld("LDW {-baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xd:
			iprintld("LDW {++baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xc:
			iprintld("LDW {--baseR[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xf:
			iprintld("LDW {baseR++[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xe:
			iprintld("LDW {baseR--[offsetR], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x1:
			iprintldc("LDW {+baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x0:
			iprintldc("LDW {-baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x9:
			iprintldc("LDW {++baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0x8:
			iprintldc("LDW {--baseR[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xb:
			iprintldc("LDW {baseR++[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		case 0xa:
			iprintldc("LDW {baseR--[offset], dst}", src1, src2, dst, "D", s, y);
			break;
		default:
			iprintld("LDW **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;
		}

	} else if (op == 0x02 && unit == DUNIT_LDSTOFFSET) {
		/*LDB Load Byte ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		print_creg(inst);
		iprintldc("LDB {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x01 && unit == DUNIT_LDSTOFFSET) {
		/*LDBU Load Byte ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 0;
		print_creg(inst);
		iprintldc("LDBU {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x04 && unit == DUNIT_LDSTOFFSET) {
		/*LDH Load HlafByte ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 1;
		print_creg(inst);
		iprintldc("LDH {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x00 && unit == DUNIT_LDSTOFFSET) {
		/*LDHU Load HalfByte ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 1;
		print_creg(inst);
		iprintldc("LDHU {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x06 && unit == DUNIT_LDSTOFFSET) {
		/*LDW Load Word ucst15, dst*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 2;
		print_creg(inst);
		iprintldc("LDW {+baseR[offset], dst}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	}

}

void Tms320C6xIss::instruction_print_st(uint32_t inst) {
	/*instruction print
	 *decode instructions, source & destination registers
	 *and print them out
	 */

	char creg, z, op = 0, unit = 0;
	char dst, src1, src2, x, s;
	uint32_t uicst1, uitmp;
	int32_t sicst1;
	int64_t slcst1;
	uint64_t ulcst1;
	char y, mode;

	creg = (inst >> 29) & 0x07;
	z = (inst >> 28) & 0x01;
	dst = (inst >> 23) & 0x1f;
	src2 = (inst >> 18) & 0x1f;
	src1 = (inst >> 13) & 0x1f;
	x = 0;
	s = (inst >> 1) & 0x01;
	if (getUnit_3bit(inst) == LUNIT) {
		/*.L unit instruction*/
		x = (inst >> 12) & 0x01;
		op = (inst >> 5) & 0x7f;
		unit = LUNIT;

	} else if (getUnit_5bit(inst) == MUNIT) {
		/*.M unit instruction*/
		x = (inst >> 12) & 0x01;
		op = (inst >> 7) & 0x1f;
		unit = MUNIT;

	} else if (getUnit_5bit(inst) == DUNIT) {
		/*.D unit instruction*/
		op = (inst >> 7) & 0x3f;
		unit = DUNIT;

	} else if (getUnit_2bit(inst) == DUNIT_LDSTOFFSET) {
		/*.D unit load/store*/
		op = (inst >> 4) & 0x07;
		unit = DUNIT_LDSTOFFSET;

	} else if (getUnit_2bit(inst) == DUNIT_LDSTBASEROFFSET) {
		/*.D unit load/store with baseR/offsetR specified*/
		op = (inst >> 4) & 0x07;
		unit = DUNIT_LDSTBASEROFFSET;

	} else if (getUnit_4bit(inst) == SUNIT) {
		/*.S unit instruction*/
		x = (inst >> 12) & 0x01;
		op = (inst >> 6) & 0x3f;
		unit = SUNIT;

	} else if (getUnit_5bit(inst) == SUNIT_ADDK) {
		/*.S unit ADDK instruction*/
		op = 0; /*nothing specified*/
		unit = SUNIT_ADDK;

	} else if (getUnit_4bit(inst) == SUNIT_IMMED) {
		/*.S unit Field operations (immediate forms)*/
		op = (inst >> 6) & 0x03;
		unit = SUNIT_IMMED;

	} else if (getUnit_4bit(inst) == SUNIT_MVK) {
		/*.S unit MVK*/
		op = 0; /*nothing specified*/
		unit = SUNIT_MVK;

	} else if (getUnit_5bit(inst) == SUNIT_BCOND) {
		/*.S unit Bcond disp*/
		op = 0; /*nothing specified*/
		unit = SUNIT_BCOND;

	} else if (getUnit_11bit(inst) == NOP) {
		/*NOP instruction*/
		op = 0;
		unit = 0;

	} else if (getUnit_16bit(inst) == IDLEINST) {
		op = IDLEOP;
		unit = IDLEUNIT;
	} else {
		/* unknown instruction kind */
		std::cout
				<< "unknown instruction type encountered in instruction_print_st function "
				<< std::endl;

	}

	/*find constants*/
	sicst1 = ((int32_t) src1 << 27) >> 27; /*taking care of sign bit extn*/
	uicst1 = (uint32_t) src1;
	slcst1 = ((int64_t) src1 << 59) >> 59; /*taking care of sign bit extn*/
	ulcst1 = (uint64_t) src1;

	if (op == 0x03 && unit == DUNIT_LDSTBASEROFFSET) {
		/*STB Store Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		print_creg(inst);
		switch (mode) {
		case 0x5:
			iprintst("STB {src, +baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0x4:
			iprintst("STB {src, -baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xd:
			iprintst("STB {src, ++baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xc:
			iprintst("STB {src, --baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xf:
			iprintst("STB {src, baseR++[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xe:
			iprintst("STB {src, baseR--[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0x1:
			iprintstc("STB {src, +baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0x0:
			iprintstc("STB {src, -baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0x9:
			iprintstc("STB {src, ++baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0x8:
			iprintstc("STB {src, --baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0xb:
			iprintstc("STB {src, baseR++[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0xa:
			iprintstc("STB {src, baseR--[offset]}", src1, src2, dst, "D", s, y);
			break;
		default:
			iprintst("STB **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;
		}

	} else if (op == 0x05 && unit == DUNIT_LDSTBASEROFFSET) {
		/*STH Store Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		print_creg(inst);
		switch (mode) {
		case 0x5:
			iprintst("STH {src, +baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0x4:
			iprintst("STH {src, -baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xd:
			iprintst("STH {src, ++baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xc:
			iprintst("STH {src, --baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xf:
			iprintst("STH {src, baseR++[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xe:
			iprintst("STH {src, baseR--[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0x1:
			iprintstc("STH {src, +baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0x0:
			iprintstc("STH {src, -baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0x9:
			iprintstc("STH {src, ++baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0x8:
			iprintstc("STH {src, --baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0xb:
			iprintstc("STH {src, baseR++[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0xa:
			iprintstc("STH {src, baseR--[offset]}", src1, src2, dst, "D", s, y);
			break;
		default:
			iprintst("STH **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;
		}

	} else if (op == 0x07 && unit == DUNIT_LDSTBASEROFFSET) {
		/*STW Store Byte*/
		y = inst >> 7 & 0x1;
		mode = inst >> 9 & 0xf;

		print_creg(inst);
		switch (mode) {
		case 0x5:
			iprintst("STW {src, +baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0x4:
			iprintst("STW {src, -baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xd:
			iprintst("STW {src, ++baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xc:
			iprintst("STW {src, --baseR[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xf:
			iprintst("STW {src, baseR++[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0xe:
			iprintst("STW {src, baseR--[offsetR]}", src1, src2, dst, "D", s, y);
			break;
		case 0x1:
			iprintstc("STW {src, +baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0x0:
			iprintstc("STW {src, -baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0x9:
			iprintstc("STW {src, ++baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0x8:
			iprintstc("STW {src, --baseR[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0xb:
			iprintstc("STW {src, baseR++[offset]}", src1, src2, dst, "D", s, y);
			break;
		case 0xa:
			iprintstc("STW {src, baseR--[offset]}", src1, src2, dst, "D", s, y);
			break;
		default:
			iprintst("STW **unknown mode**", 0, 0, 0, "D", 0, 0);
			break;
		}

	} else if (op == 0x03 && unit == DUNIT_LDSTOFFSET) {
		/*STB Store Byte*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 0;
		print_creg(inst);
		iprintstc("STB {src, +baseR[offset]}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x05 && unit == DUNIT_LDSTOFFSET) {
		/*STH Store Halfword*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 1;
		print_creg(inst);
		iprintstc("STH {src, +baseR[offset]}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	} else if (op == 0x07 && unit == DUNIT_LDSTOFFSET) {
		/*STW Store Word*/
		y = inst >> 7 & 0x1;
		uitmp = inst >> 8 & 0x7fff;
		uitmp = uitmp << 2;
		print_creg(inst);
		iprintstc("STW {src, +baseR[offset]}", uitmp, (y==0) ? 14 : 15, dst,
				"D", s, sideB);

	}

}

}
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

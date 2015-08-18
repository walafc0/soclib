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
 * Copyright (c) TelecomParisTECH
 *         Tarik Graba <tarik.graba@telecom-paristech.fr>, 2009
 *
 * Based on sparcv8 and mips32 code
 *         Alexis Polti <polti@telecom-paristech.fr>, 2008
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *         Alain Greiner <alain.greiner@lip6.fr>, 2007
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 *
 * $Id$
 *
 * History:
 * - 2011-02-10
 *   Tarik Graba : The instructions issu delay are grouped in the opcode table
 * - 2010-04-16
 *   Tarik Graba : Added a template parameter to specify the endianess
 * - 2009-07-08
 *   Tarik Graba : the iss is now sensitive to high level irqs
 * - 2009-02-15
 *   Tarik Graba : Forked mips32 and sparcv8 to begin lm32
 */

#include "lm32.h"

namespace soclib { namespace common {

#define tmpl(x) template<bool lEndianInterface> x  LM32Iss<lEndianInterface>

    tmpl(uint32_t)::get_absolute_dest_reg(ins_t ins) const
    {
        switch(LM32Iss::OpcodesTable [ins.J.op].instformat) {
            case JI:
                return 0;
            case RI:
                return ins.I.rX;
            case RR:
                return ins.R.rX;
            case USR:
                return ins.R.rX;
            case CR:
                return ins.C.rR;
        }
        return 0;
    }

#define OPTABLE(x,y,d) {&LM32Iss<lEndianInterface>::OP_LM32_##x, #x, y, d}

    template<bool lEndianInterface> 
    typename LM32Iss<lEndianInterface>::LM32_op_entry 
    const LM32Iss<lEndianInterface>::OpcodesTable [64]= {
        OPTABLE(srui   , RI , 1), OPTABLE(nori  , RI , 1),
        OPTABLE(muli   , RI , 1), OPTABLE(sh    , RR , 1),
        OPTABLE(lb     , RR , 3), OPTABLE(sri   , RI , 1),  
        OPTABLE(xori   , RI , 1), OPTABLE(lh    , RR , 3),
        OPTABLE(andi   , RI , 1), OPTABLE(xnori , RI , 1),  
        OPTABLE(lw     , RR , 3), OPTABLE(lhu   , RR , 3),
        OPTABLE(sb     , RR , 1), OPTABLE(addi  , RI , 1),  
        OPTABLE(ori    , RI , 1), OPTABLE(sli   , RI , 1),
        OPTABLE(lbu    , RR , 3), OPTABLE(be    , RR , 4),
        OPTABLE(bg     , RI , 4), OPTABLE(bge   , RI , 4),  
        OPTABLE(bgeu   , RI , 4), OPTABLE(bgu   , RI , 4),
        OPTABLE(sw     , RR , 1), OPTABLE(bne   , RR , 4),
        OPTABLE(andhi  , RI , 1), OPTABLE(cmpei , RI , 1),
        OPTABLE(cmpgi  , RI , 1), OPTABLE(cmpgei, RI , 1),
        OPTABLE(cmpgeui, RI , 1), OPTABLE(cmpgui, RI , 1),
        OPTABLE(orhi   , RI , 1), OPTABLE(cmpnei, RI , 1),
        OPTABLE(sru    , RR , 1), OPTABLE(nor   , RR , 1),
        OPTABLE(mul    , RR , 1), OPTABLE(divu  , RR ,34),
        OPTABLE(rcsr   , CR , 1), OPTABLE(sr    , RR , 1),
        OPTABLE(xor    , RR , 1), OPTABLE(div   , RR ,34),
        OPTABLE(and    , RR , 1), OPTABLE(xnor  , RR , 1),
        OPTABLE(reser  , JI , 1), OPTABLE(raise , JI , 4),
        OPTABLE(sextb  , RR , 1), OPTABLE(add   , RR , 1),
        OPTABLE(or     , RR , 1), OPTABLE(sl    , RR , 1),
        OPTABLE(b      , RR , 4), OPTABLE(modu  , RR ,34),
        OPTABLE(sub    , RR , 1), OPTABLE(user  , USR, 1),
        OPTABLE(wcsr   , CR , 1), OPTABLE(mod   , RR ,34),
        OPTABLE(call   , RR , 4), OPTABLE(sexth , RR , 1),
        OPTABLE(bi     , JI , 4), OPTABLE(cmpe  , RR , 1),
        OPTABLE(cmpg   , RR , 1), OPTABLE(cmpge , RR , 1),
        OPTABLE(cmpgeu , RR , 1), OPTABLE(cmpgu , RR , 1),
        OPTABLE(calli  , JI , 4), OPTABLE(cmpne , RR , 1),
    };

#undef OPTABLE

    tmpl(std::string)::get_ins_name( void ) const
    {
        return this->OpcodesTable[m_inst.J.op].name;
    }

    // Instruction delay
#define setInsDelay(d)    do {m_ins_delay = d-1;} while(0)
#define unsetInsDelay()   do {m_ins_delay = 0;} while(0)
    // Run the instruction 
    tmpl(void)::run() {
        // m_inst.J.op contains the opcode
        // The opcode is the same field for all instruction types
        setInsDelay(LM32Iss::OpcodesTable [m_inst.J.op].cycles_for_issue);
        void (LM32Iss::*func)() = LM32Iss::OpcodesTable [m_inst.J.op].func;
        (this->*func)();
    }

    // The instructions
#define LM32_function(x) tmpl(void)::OP_LM32_##x()

    LM32_function(raise) {
        //Soft exception fonction
        if ((m_inst.ins & 0x7) == 0x7) {
            m_exception = true; 
            m_exception_cause = X_SYSTEM_CALL ; // scall instruction
        }
        else if((m_inst.ins & 0x3) == 0x2) {
            m_exception = true; 
            m_exception_cause = X_BREAK_POINT ; // break instruction
        }
        else {
            std::cout   << name() 
                << " This raise exception is not implemented !!" 
                << std::endl;
            exit (-1);
        }
    }

    // User defined instructions
    LM32_function(user) {
        //TODO ...Add user instructions support
        std::cout << "Destination reg "<< m_inst.U.rX 
            << " "
            << "Source regs " << m_inst.U.rY << "and " << m_inst.U.rZ
            << " "
            << "with user defined instruction " << m_inst.U.User_defined_inst
            << std::endl;
        std::cout << name() << "ERROR: LM32 user defined instruction!!" 
            " "<< "not yet implemented !!" << std::endl;
        exit (-1);
    }

    // reserved opcode
    LM32_function(reser ) {
        std::cout << name() << "ERROR: LM32 Iss gets reserved opcode !!" << std::endl;
        exit (-1);
    }

    //!Instruction srui behavior method.
    LM32_function( srui ){// shift rghit unsigned immediate
        r_gp[m_inst.I.rX] = (unsigned)r_gp[m_inst.I.rY] >> (m_inst.I.imd & 0x1F);
    }

    //!Instruction nori behavior method.
    LM32_function( nori ){// not or immediate
        r_gp[m_inst.I.rX] = ~(r_gp[m_inst.I.rY] | m_inst.I.imd);
    }

    //!Instruction muli behavior method.
    LM32_function( muli ){// immediate multiplication
        r_gp[m_inst.I.rX] = r_gp[m_inst.I.rY] * sign_ext(m_inst.I.imd, 16);
    }

    //!Instruction sri behavior method.
    LM32_function( sri ){// shift right immediate
        r_gp[m_inst.I.rX] = (signed)r_gp[m_inst.I.rY] >> (m_inst.I.imd & 0x1F);
    }

    //!Instruction xori behavior method.
    LM32_function( xori ){// exclusive or immediate
        r_gp[m_inst.I.rX] = r_gp[m_inst.I.rY] ^ m_inst.I.imd;
    }

    //!Instruction andi behavior method.
    LM32_function( andi ){// and immediate
        r_gp[m_inst.I.rX] = r_gp[m_inst.I.rY] & m_inst.I.imd;
    }

    //!Instruction xnori behavior method.
    LM32_function( xnori ){// not exclusive or immediate
        r_gp[m_inst.I.rX] = ~(r_gp[m_inst.I.rY] ^ m_inst.I.imd);
    }

    //!Instruction addi behavior method.
    LM32_function( addi ){// add immediate
        r_gp[m_inst.I.rX] = r_gp[m_inst.I.rY] + sign_ext(m_inst.I.imd, 16);
    }

    //!Instruction ori behavior method.
    LM32_function( ori ){// or immediate
        r_gp[m_inst.I.rX] = r_gp[m_inst.I.rY] | m_inst.I.imd;
    }

    //!Instruction sli behavior method.
    LM32_function( sli ){// shift left immediate
        r_gp[m_inst.I.rX] = r_gp[m_inst.I.rY] << (m_inst.I.imd & 0x1F);
    }

    //!Instruction be behavior method.
    LM32_function( be ){// branch if equal
        if (r_gp[m_inst.I.rY] == r_gp[m_inst.I.rX])
        {
            m_next_pc = r_pc+ (sign_ext(m_inst.I.imd, 16)<<2);
        }
        else unsetInsDelay();
    }

    //!Instruction bg behavior method.
    LM32_function( bg ){// branch if greater
        if ((signed)r_gp[m_inst.I.rY] > (signed)r_gp[m_inst.I.rX])
        {
            m_next_pc = r_pc+ (sign_ext(m_inst.I.imd, 16)<<2);
        }
        else unsetInsDelay();
    }

    //!Instruction bge behavior method.
    LM32_function( bge ){// branch if greater or equal
        if ((signed)r_gp[m_inst.I.rY] >= (signed)r_gp[m_inst.I.rX])
        {
            m_next_pc = r_pc+ (sign_ext(m_inst.I.imd, 16)<<2);
        }
        else unsetInsDelay();
    }

    //!Instruction bgeu behavior method.
    LM32_function( bgeu ){// branch if greater or equal unsigned
        if ((unsigned)r_gp[m_inst.I.rY] >= (unsigned)r_gp[m_inst.I.rX])
        {
            m_next_pc = r_pc+ (sign_ext(m_inst.I.imd, 16)<<2);
        }
        else unsetInsDelay();
    }

    //!Instruction bgu behavior method.
    LM32_function( bgu ){// branch if greater unsigned
        if ((unsigned)r_gp[m_inst.I.rY] > (unsigned)r_gp[m_inst.I.rX])
        {
            m_next_pc = r_pc+ (sign_ext(m_inst.I.imd, 16)<<2);
        }
        else unsetInsDelay();
    }

    //!Instruction bne behavior method.
    LM32_function( bne ){// branch if not equal
        if (r_gp[m_inst.I.rY] != r_gp[m_inst.I.rX])
        {
            m_next_pc = r_pc+ (sign_ext(m_inst.I.imd, 16)<<2);
        }
        else unsetInsDelay();
    }

    //!Instruction andhi behavior method.
    LM32_function( andhi ){// and immediate with high 16bits
        r_gp[m_inst.I.rX] = r_gp[m_inst.I.rY] & (m_inst.I.imd << 16);
    }

    //!Instruction cmpei behavior method.
    LM32_function( cmpei ){// compare if equal immediate
        r_gp[m_inst.I.rX] = ((unsigned) r_gp[m_inst.I.rY] == (unsigned) sign_ext(m_inst.I.imd, 16));
    }

    //!Instruction cmpgi behavior method.
    LM32_function( cmpgi ){// compare if greater  immediate
        r_gp[m_inst.I.rX] = (signed)r_gp[m_inst.I.rY] >  (signed)sign_ext(m_inst.I.imd, 16);
    }

    //!Instruction cmpgei behavior method.
    LM32_function( cmpgei ){// compare if greater or equal immediate
        r_gp[m_inst.I.rX] = (signed)r_gp[m_inst.I.rY] >= (signed)sign_ext(m_inst.I.imd, 16);
    }

    //!Instruction cmpgeui behavior method.
    LM32_function( cmpgeui ){// compare if greater or equal immediate unsigned
        r_gp[m_inst.I.rX] = (unsigned)r_gp[m_inst.I.rY] >= (unsigned)m_inst.I.imd;
    }

    //!Instruction cmpgui behavior method.
    LM32_function( cmpgui ){// compare if greater immediate unsigned
        r_gp[m_inst.I.rX] = (unsigned)r_gp[m_inst.I.rY] > (unsigned)m_inst.I.imd;
    }

    //!Instruction orhi behavior method.
    LM32_function( orhi ){// or immediate high 16bits
        r_gp[m_inst.I.rX] = r_gp[m_inst.I.rY] | (m_inst.I.imd << 16);
    }

    //!Instruction cmpnei behavior method.
    LM32_function( cmpnei ){// compare if not equal immediate
        r_gp[m_inst.I.rX] = ((unsigned)r_gp[m_inst.I.rY] != (unsigned)sign_ext(m_inst.I.imd, 16));
    }

    //!Instruction sru behavior method.
    LM32_function( sru ){// shift right unsigned
        r_gp[m_inst.R.rX] = (unsigned)r_gp[m_inst.R.rY] >> (r_gp[m_inst.R.rZ] & 0x1F);
    }

    //!Instruction nor behavior method.
    LM32_function( nor ){// not or
        r_gp[m_inst.R.rX] = ~(r_gp[m_inst.R.rY] | r_gp[m_inst.R.rZ]);
    }

    //!Instruction mul behavior method.
    LM32_function( mul ){// multiplication
        r_gp[m_inst.R.rX] = r_gp[m_inst.R.rY] * r_gp[m_inst.R.rZ];
    }

    //!Instruction divu behavior method.
    LM32_function( divu ){// unsigned integer division
        if (r_gp[m_inst.R.rZ] == 0){
            m_exception = true;
            m_exception_cause = X_DIVISION_BY_ZERO; // division by 0
            unsetInsDelay();
        }
        else {
            r_gp[m_inst.R.rX] = (unsigned)r_gp[m_inst.R.rY] / (unsigned)r_gp[m_inst.R.rZ];
        }
    }

    //!Instruction sr behavior method.
    LM32_function( sr ){// shift right signed
        r_gp[m_inst.R.rX] = (signed)r_gp[m_inst.R.rY] >> (r_gp[m_inst.R.rZ] & 0x1F);
    }

    //!Instruction xor behavior method.
    LM32_function( xor ){// exclusive or
        r_gp[m_inst.R.rX] = r_gp[m_inst.R.rY] ^ r_gp[m_inst.R.rZ];
    }

    //!Instruction div behavior method.
    LM32_function( div ){// integer division
        if (r_gp[m_inst.R.rZ] == 0){
            m_exception = true;
            m_exception_cause = X_DIVISION_BY_ZERO; // division by 0
            unsetInsDelay();
        }
        else {
            r_gp[m_inst.R.rX] = (signed)r_gp[m_inst.R.rY] / (signed)r_gp[m_inst.R.rZ];
        }
    }

    //!Instruction and behavior method.
    LM32_function( and ){
        r_gp[m_inst.R.rX] = r_gp[m_inst.R.rY] & r_gp[m_inst.R.rZ];
    }

    //!Instruction xnor behavior method.
    LM32_function( xnor ){// not exclusive or
        r_gp[m_inst.R.rX] = ~(r_gp[m_inst.R.rY] ^ r_gp[m_inst.R.rZ]);
    }

    //!Instruction sextb behavior method.
    LM32_function( sextb ){// sign extension of a byte
        r_gp[m_inst.R.rX] = ((signed)(r_gp[m_inst.R.rY]<<24))>>24;
    }

    //!Instruction add behavior method.
    LM32_function( add ){// addition
        r_gp[m_inst.R.rX] = r_gp[m_inst.R.rY] + r_gp[m_inst.R.rZ];
    }

    //!Instruction or behavior method.
    LM32_function( or ){// or
        r_gp[m_inst.R.rX] = r_gp[m_inst.R.rY] | r_gp[m_inst.R.rZ];
    }

    //!Instruction sl behavior method.
    LM32_function( sl ){// shift left
        r_gp[m_inst.R.rX] = r_gp[m_inst.R.rY] << (r_gp[m_inst.R.rZ] & 0x1F);
    }

    //!Instruction b behavior method.
    LM32_function( b ){ // branch
        m_next_pc = r_gp[m_inst.R.rY];
        if (m_inst.R.rY == 30)      // eret // return from exception
            r_IE.IE = r_IE.EIE;
        else if (m_inst.R.rY == 31) // bret // return from breakpoint
            r_IE.IE = r_IE.BIE;
    }

    //!Instruction modu behavior method.
    LM32_function( modu ){// unsigned modulo
        if (r_gp[m_inst.R.rZ] == 0){
            m_exception = true;
            m_exception_cause = X_DIVISION_BY_ZERO; // division by 0
            unsetInsDelay();
        }
        else {
            r_gp[m_inst.R.rX] = (unsigned)r_gp[m_inst.R.rY] % (unsigned)r_gp[m_inst.R.rZ]; 
        }
    }

    //!Instruction sub behavior method.
    LM32_function( sub ){// soustraction
        r_gp[m_inst.R.rX] = r_gp[m_inst.R.rY] - r_gp[m_inst.R.rZ]; 
    }

    //!Instruction mod behavior method.
    LM32_function( mod ){// signed modulo
        if (r_gp[m_inst.R.rZ] == 0){
            m_exception = true;
            m_exception_cause = X_DIVISION_BY_ZERO; // division by 0
            unsetInsDelay();
        }
        else {
            r_gp[m_inst.R.rX] = (signed)r_gp[m_inst.R.rY] % (signed)r_gp[m_inst.R.rZ]; 
        }
    }

    //!Instruction call behavior method.
    LM32_function( call ){// jump to sub routine
        r_gp[ra] = r_npc ;// is pc + 4!!// return address
        m_next_pc = r_gp[m_inst.R.rY];
    }

    //!Instruction calli behavior method.
    LM32_function( calli ){//jump to sub routine immediate
        r_gp[ra] = r_npc ; // is pc + 4!!// return address
        m_next_pc = r_pc + (sign_ext(m_inst.J.imd,26)<<2);
    }

    //!Instruction bi behavior method.
    LM32_function( bi ){// branch immediate
        m_next_pc = r_pc + (sign_ext(m_inst.J.imd,26)<<2);
    }

    //!Instruction sexth behavior method.
    LM32_function( sexth ){// sign extension of a half
        r_gp[m_inst.R.rX] = ((signed)(r_gp[m_inst.R.rY]<<16))>>16;
    }

    //!Instruction cmpe behavior method.
    LM32_function( cmpe ){// compare if equal
        r_gp[m_inst.R.rX] = r_gp[m_inst.R.rY] == r_gp[m_inst.R.rZ];
    }

    //!Instruction cmpg behavior method.
    LM32_function( cmpg ){// compare if greater signed
        r_gp[m_inst.R.rX] = (signed)r_gp[m_inst.R.rY] > (signed)r_gp[m_inst.R.rZ];
    }

    //!Instruction cmpge behavior method.
    LM32_function( cmpge ){// compare if greater or equal signed
        r_gp[m_inst.R.rX] = (signed)r_gp[m_inst.R.rY] >= (signed)r_gp[m_inst.R.rZ];
    }

    //!Instruction cmpgeu behavior method.
    LM32_function( cmpgeu ){// compare if greater or equal unsigned
        r_gp[m_inst.R.rX] = (unsigned) r_gp[m_inst.R.rY] >= (unsigned) r_gp[m_inst.R.rZ];
    }

    //!Instruction cmpgu behavior method.
    LM32_function( cmpgu ){// compare if greater unsigned
        r_gp[m_inst.R.rX] = (unsigned) r_gp[m_inst.R.rY] > (unsigned) r_gp[m_inst.R.rZ];
    }

    //!Instruction cmpne behavior method.
    LM32_function( cmpne ){// compare if not equal
        r_gp[m_inst.R.rX] = r_gp[m_inst.R.rY] != r_gp[m_inst.R.rZ];
    }

    //!Instruction rcsr behavior method.
    LM32_function( rcsr ){// read control & status register
        switch (m_inst.C.csr){
            case 0x0:  // interrupt enable
                r_gp[m_inst.C.rR] =  (r_IE.BIE << 2) | (r_IE.EIE << 1) | r_IE.IE;
                break;
            case 0x1:  // interrupt mask
                r_gp[m_inst.C.rR] = r_IM ;
                break;
            case 0x2:  // interrupt pending
                r_gp[m_inst.C.rR] = r_IP ; 
                break;
            case 0x3:  // inst cache control
                r_gp[m_inst.C.rR] = 0x0; // write only!! no indication about the read value
                break;
            case 0x4:  // data cache control
                r_gp[m_inst.C.rR] = 0x0; // write only!! no indication about the read value
                break;
            case 0x5:  // cycle counter
                r_gp[m_inst.C.rR] = r_CC ;
                break;
            case 0x6:  // conf register 
                r_gp[m_inst.C.rR] = 
                        r_CFG.REV << 26 | r_CFG.WP  << 22 |
                        r_CFG.BP  << 18 | r_CFG.INT << 12 |
                        r_CFG.J   << 11 | r_CFG.R   << 10 |
                        r_CFG.H   <<  9 | r_CFG.G   <<  8 |
                        r_CFG.DC  <<  7 | r_CFG.IC  <<  6 |
                        r_CFG.CC  <<  5 | r_CFG.X   <<  4 |
                        r_CFG.U   <<  3 | r_CFG.S   <<  2 |
                        r_CFG.D   <<  1 | r_CFG.M   <<  0 ;
                break;
            case 0x7:  // Exception base address
                r_gp[m_inst.C.rR] = r_EBA ;
                break;
            case 0x10: // break point 0
            case 0x11: // break point 1
            case 0x12: // break point 2
            case 0x13: // break point 3
                r_gp[m_inst.C.rR] = 
                                      r_BP[(m_inst.C.csr & 0x3)].A << 2 
                                    | r_BP[(m_inst.C.csr & 0x3)].E ;
                break;
            case 0x18: // watch point 0
            case 0x19: // watch point 1
            case 0x1a: // watch point 2
            case 0x1b: // watch point 3
                r_gp[m_inst.C.rR] = r_WP[(m_inst.C.csr & 0x3)];
                break;
            default:
                std::cout   << name()
                    << "Error: Read to Unkown CSR !!"<<std::endl;
                exit (-1);
                break;
        }
    }

    // XTN_CACHE must be XTN_DCACHE_INVAL or XTN_ICACHE_INVAL
#define FLUSH(XTN_CACHE)                                       \
    do {                                                       \
        struct DataRequest null_dreq = ISS_DREQ_INITIALIZER;   \
        m_dreq.req = null_dreq;                                \
        m_dreq.sign_extend = false;                            \
        m_dreq.dest_reg = 0;                                   \
        m_dreq.addr = 0;                                       \
        m_dreq.req.valid = true;                               \
        m_dreq.req.addr = XTN_CACHE*4;                         \
        m_dreq.req.wdata = 0;                                  \
        m_dreq.req.be = 0;                                     \
        m_dreq.req.type = XTN_WRITE;                           \
        m_dreq.req.mode = MODE_USER;                           \
    } while(0)

#define WRITE_BP__(NUM,DATA)                    \
    do {                                        \
        r_BP[NUM].E = DATA & 0x1;               \
        r_BP[NUM].A = (DATA & 0xfffffffc) >> 2; \
    }while(0)

    //!Instruction wcsr behavior method.
    LM32_function( wcsr ){// write control & status register
        uint32_t wData = r_gp[m_inst.C.rW];
        switch (m_inst.C.csr){
            case 0x0: // interrupt enable
                r_IE.IE  = (0x1 & wData)? 1: 0;
                r_IE.EIE = (0x2 & wData)? 1: 0;
                r_IE.BIE = (0x4 & wData)? 1: 0;
                break;
            case 0x1: // interrupt mask
                r_IM = wData;
                break;
            case 0x2: // interrupt pending
                    // Bits are cleared by writing '1'!!!
                        r_IP = r_IP & ~wData;
                break;
            case 0x3:  // inst cache ctrl
                FLUSH(XTN_ICACHE_INVAL);
                r_ICC  = 0x1; //a write invalidate the Icache
                break;
            case 0x4:  // data cache ctrl
                FLUSH(XTN_DCACHE_INVAL);
                r_DCC  = 0x1; //a read invalidate de Dcache
                break;
            case 0x5: // cycle counter 
                //r_CC  = wData; //read only
                break;
            case 0x6: // cfg register
                //r_CFG = wData; //read only
                break;
            case 0x7: // Exception base address
                r_EBA  = wData & 0xFFFFFF00;
                break;
            case 0x10: // break point 0
            case 0x11: // break point 1
            case 0x12: // break point 2
            case 0x13: // break point 3
                WRITE_BP__((m_inst.C.csr & 0x3),wData);
                break;
            case 0x18: // watch point 0
            case 0x19: // watch point 1
            case 0x1a: // watch point 2
            case 0x1b: // watch point 3
                r_WP[(m_inst.C.csr & 0x3)] = wData;
                break;
            default:
                std::cout   << name()
                    << "Error: Write to Unkown CSR !!"<<std::endl;
                exit (-1);
                break;
        }
    }
#undef setInsDelay
#undef unsetInsDelay
#undef WRITE_BP
#undef FLUSH
#undef LM32_function
#undef tmpl
}}

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

#ifndef _SOCLIB_LM32_ISS_H_
#define _SOCLIB_LM32_ISS_H_

#include <iostream>
#include <iomanip>
#include <cassert>

#include "iss2.h"
#include "register.h"
#include "base_module.h"
#include "soclib_endian.h"
#include "arithmetics.h"


namespace soclib { namespace common {

    // soclib default vci interface is little endian
template <bool lEndianInterface >
    class LM32Iss
        : public soclib::common::Iss2
    {
        public:
            static const int n_irq = 32;

        private:
            typedef unsigned char except_t;

            //////////////////////////////////////////
            // Control and status registers indexes
            //////////////////////////////////////////
            //   IE   @ 0x0,   // Interrupt enable
            //   IM   @ 0x1,   // Interrupt mask
            //   IP   @ 0x2,   // Interrupt pending
            //   ICC  @ 0x3,   // Instruction cache control
            //   DCC  @ 0x4,   // Data cache control
            //   CC   @ 0x5,   // Cycle counter
            //   CFG  @ 0x6,   // Configuration
            //   EBA  @ 0x7,   // Exception base address
            //   DC   @ 0x8,    // Debug control
            //   DEBA @ 0x9,    // Debug exception base address
            //   JTX  @ 0xe,    // JTAG UART transmit
            //   JRX  @ 0xf,    // JTAG UART receive
            //   BP0  @ 0x10,   // Breakpoint address 0
            //   BP1  @ 0x11,   // Breakpoint address 1
            //   BP2  @ 0x12,   // Breakpoint address 2
            //   BP3  @ 0x13,   // Breakpoint address 3
            //   WP0  @ 0x18,   // Watchpoint address 0
            //   WP1  @ 0x19,   // Watchpoint address 1
            //   WP2  @ 0x1a,   // Watchpoint address 2
            //   WP3  @ 0x1b,   // Watchpoint address 3

            // Interrupt enable
            REG32_BITFIELD(
                    uint32_t not_used:29,
                    uint32_t BIE:1, // copy IE if breakpoint
                    uint32_t EIE:1, // copy IE if exception
                    uint32_t  IE:1, // int ena
                    )r_IE; 
            //Interrupt mask
            uint32_t r_IM; // Int ena if bit set to 1
            //Interrupt pending
            uint32_t r_IP; // Int pending if 1, cleared by writing 1!!
            // Cache control
            uint32_t r_ICC,r_DCC; // writing in these registers invalidate the corresponding cache
            // Cycle counter
            uint32_t r_CC; // 0 at reset and incremented at each clock cycle
            // Configuration 
            REG32_BITFIELD(
                    uint32_t REV:6,     // Processor Rev. number (0-63)
                    uint32_t WP:4,      // number of watch points (0-4) 
                    uint32_t BP:4,      // number of break points (0-4)
                    uint32_t INT:6,     // number of interupt (0-32) 
                    uint32_t J:1,       // JTAG uart 
                    uint32_t R:1,       // ROM debug 
                    uint32_t H:1,       // H/W debug 
                    uint32_t G:1,       // debug 
                    uint32_t DC:1,      // data  cache 
                    uint32_t IC:1,      // inst. cache
                    uint32_t CC:1,      // cycle counter 
                    uint32_t X:1,       // sign extension 
                    uint32_t U:1,       // user defined instructions
                    uint32_t S:1,       // barrel shifter
                    uint32_t D:1,       // divider
                    uint32_t M:1,       // multiplier
                    ) r_CFG;
            // Exception base address
            addr_t r_EBA;         // Exception are 256 byte aligned The lower byte have to be forced to 0
            // Debug control
            REG32_BITFIELD(
                    uint32_t not_used:22,
                    uint32_t C3:2,      // 11 Break on read/write
                    uint32_t C2:2,      // 10 Break on write
                    uint32_t C1:2,      // 01 Break on read
                    uint32_t C0:2,      // 00 WP disabled
                    uint32_t RE:1,      // Remap all exceptions
                    uint32_t SS:1,      // Single step
                    ) r_DC;

            // Debug exception base address
            addr_t r_DEBA;         // Exception are 256 byte aligned The lower byte have to be forced to 0

            // JTAG UART transmit
            REG32_BITFIELD(
                    uint32_t not_used:23,
                    uint32_t F:1,       // TXD reg is full
                    uint8_t TXD,        // Transmits data
                    ) r_JTX;

            // JTAG UART receive
            REG32_BITFIELD(
                    uint32_t not_used:23,
                    uint32_t F:1,       // RXD reg is full
                    uint8_t RXD,        // Receives data
                    ) r_JRX;

            // Break point 
            typedef REG32_BITFIELD(
                    uint32_t A:30,      // The BP address word aligned
                    uint32_t not_used:1,
                    uint32_t E:1,       // Enable the breakpoint
                    )bp_t;
            bp_t r_BP[4];                // 4 possible break points

            // Watch point
            addr_t r_WP [4];              // 4 possible watch points

            //////////////////////////////////////////
            // End of Control and status registers
            //////////////////////////////////////////

            //Specific registers indexes
            enum {
                gp        =  26,   // global pointer
                fp        =  27,   // frame pointer
                sp        =  28,   // stack pointer
                ra        =  29,   // return address
                ea        =  30,   // exception return address
                ba        =  31    // breakpoint return address
            };

            // Reset and debug/exception base addresses
            enum {
                RESET_ADDRESS     = 0x00000000,
                DEBA_RESET        = 0x00000000
            };

            // Exception causes
            enum {
                X_RESET             = 0,
                X_BREAK_POINT       = 1,
                X_INST_BUS_ERROR    = 2,
                X_WATCH_POINT       = 3,
                X_DATA_BUS_ERROR    = 4,
                X_DIVISION_BY_ZERO  = 5,
                X_INTERRUPT         = 6,
                X_SYSTEM_CALL       = 7
            };

            // Instruction type
            typedef union {
                union {
                    PACKED_BITFIELD(
                            uint32_t op:6,
                            uint32_t imd:26
                            ) J;// immediate branche
                    PACKED_BITFIELD(
                            uint32_t op:6,
                            uint32_t rY:5,
                            uint32_t rX:5,
                            uint32_t imd:16
                            ) I;// register immediate
                    PACKED_BITFIELD(
                            uint32_t op:6,
                            uint32_t rY:5,
                            uint32_t rZ:5,
                            uint32_t rX:5,
                            uint32_t Not_used:11
                            ) R;// register register
                    PACKED_BITFIELD(
                            uint32_t op:6,
                            uint32_t csr:5,
                            uint32_t rW:5,
                            uint32_t rR:5,
                            uint32_t Not_used:11
                            ) C;// control registers
                    PACKED_BITFIELD(
                            uint32_t op:6,
                            uint32_t rY:5,
                            uint32_t rZ:5,
                            uint32_t rX:5,
                            uint32_t User_defined_inst:11
                            ) U; // User defined instructions
                } __attribute__((packed));
                uint32_t ins;
            } ins_t;

            enum InstFormat {JI, RI, RR, CR, USR};

            // member variables (internal registers)
            data_t 	    r_pc;			// Program Counter
            data_t 	    r_npc;		    // Next Program Counter
            data_t      r_gp[32];       // General Registers

            ins_t       m_inst;         // Current instruction
            bool        m_ibe;          // Inst bus error
            bool        m_dbe;          // Data bus error

            uint32_t    m_ins_delay;    // Instruction simulation delay

            bool        m_ireq_pending; // True while an instruction request is pending
            bool        m_dreq_pending; // True while a data request is pending
            bool        m_hazard;       // True if there is a hazard between previous load/store and the current instruction

            bool        m_exception;        // True if an exception is generated
            except_t    m_exception_cause;  // Exception cause

            addr_t      m_next_pc;          // Next PC value, stored in r_npc at the end of instruction execution


            // Data transaction in LM32 are simple (byte, short, word)
            // The data can be optionnaly sign extended
            typedef struct mem_access {
                addr_t      addr;           // Address of transaction
                bool        sign_extend;    // Do we sign extend the value
                DataRequest req;            // data request
                uint32_t    dest_reg;       // destination reg for mem read
            } mem_access_t;

            mem_access_t    m_dreq;         // The data transation request


        public:
            // Constructor
            LM32Iss(const std::string &name, uint32_t ident);

            // Debug public function for gdb server
            void  dump() const;

            // Reset function 
            void reset();
            /******  ISS2 API  ******/
            // Run functions. For CABA, called in this order:
            //    m_iss.getRequests( ireq, dreq );
            //    m_iss.executeNCycles(1, iresp, dresp, it);

            void getRequests( 
                    struct InstructionRequest &ireq ,
                    struct DataRequest &dreq
                    ) const;


            uint32_t executeNCycles( 
                    uint32_t ncycle, 
                    const struct InstructionResponse &irsp,
                    const struct DataResponse &drsp,
                    uint32_t irq_bit_field 
                    );
            // internal functions called by executeNCycles
            void _setInstruction(const struct InstructionResponse &rsp);
            void _setData(const struct DataResponse &rsp);

            // Data bus error
            inline void setWriteBerr()
            {
                m_dbe = true;
            }

            static inline void setBoostrapCpuId(int id = -1)
            {
                assert(id < 1);
            }

            /******  END ISS2 API  ******/

            /******     DEBUG API ******/
            // processor internal registers access API, used by
            // debugger. LM32 order is 26+6 (gp,fp,sp,ra,ea,ba)general-purpose; 
            // PC;EID;EBA;DEBA;IE
            unsigned int debugGetRegisterCount() const;
            debug_register_t debugGetRegisterValue(unsigned int reg) const;
            size_t debugGetRegisterSize(unsigned int reg) const;
            void debugSetRegisterValue(unsigned int reg,  debug_register_t value);
            addr_t debugGetPC() const;
            void debugSetPC(addr_t pc);

            static const unsigned int s_sp_register_no = 28;
            static const unsigned int s_fp_register_no = 27;
            static const unsigned int s_pc_register_no = 32;

            static const Iss2::debugCpuEndianness s_endianness = Iss2::ISS_BIG_ENDIAN;

            /****** END DEBUG API ******/

        private:
            void  run();

            // Opcode entry type
            // Each entry contain a function pointer and
            // the opcode name
            typedef struct {
                void (LM32Iss<lEndianInterface>::*func)();
                const std::string name;
                const InstFormat instformat;
                const uint32_t cycles_for_issue;
            } LM32_op_entry;

            // Ordered opcode table
            static LM32_op_entry const OpcodesTable [64];

            uint32_t get_absolute_dest_reg(ins_t ins) const;

            // debug functions
            void  dump_pc(const std::string &msg) const;
            void  dump_regs(const std::string &msg) const;
            std::string GetExceptioName (int type) const;
            std::string get_ins_name( void ) const;


            // macro to declare all functions
#define OP_function(x) OP_LM32_##x()
            void OP_function( reser );
            void OP_function( user );
            void OP_function( srui );
            void OP_function( nori );
            void OP_function( muli );
            void OP_function( sh );
            void OP_function( lb );
            void OP_function( sri );
            void OP_function( xori );
            void OP_function( lh );
            void OP_function( andi );
            void OP_function( xnori );
            void OP_function( lw );
            void OP_function( lhu );
            void OP_function( sb );
            void OP_function( addi );
            void OP_function( ori );
            void OP_function( sli );
            void OP_function( lbu );
            void OP_function( be );
            void OP_function( bg );
            void OP_function( bge );
            void OP_function( bgeu );
            void OP_function( bgu );
            void OP_function( sw );
            void OP_function( bne );
            void OP_function( andhi );
            void OP_function( cmpei );
            void OP_function( cmpgi );
            void OP_function( cmpgei );
            void OP_function( cmpgeui );
            void OP_function( cmpgui );
            void OP_function( orhi );
            void OP_function( cmpnei );
            void OP_function( sru );
            void OP_function( nor );
            void OP_function( mul );
            void OP_function( divu );
            void OP_function( sr );
            void OP_function( xor );
            void OP_function( div );
            void OP_function( and );
            void OP_function( xnor );
            void OP_function( raise );
            void OP_function( sextb );
            void OP_function( add );
            void OP_function( or );
            void OP_function( sl );
            void OP_function( b );
            void OP_function( modu );
            void OP_function( sub );
            void OP_function( mod );
            void OP_function( call );
            void OP_function( calli );
            void OP_function( sexth );
            void OP_function( cmpe );
            void OP_function( cmpg );
            void OP_function( cmpge );
            void OP_function( cmpgeu );
            void OP_function( cmpgu );
            void OP_function( cmpne );
            void OP_function( rcsr );
            void OP_function( wcsr );
            void OP_function( bi );
#undef OP_function

    };


    // Inline functions
#define tmpl(x) template<bool lEndianInterface> x  LM32Iss<lEndianInterface>

    // give back instruction to iss
    tmpl(inline void)::_setInstruction(const struct InstructionResponse &rsp)
    {
        // Is there a response ?
        m_ireq_pending = !rsp.valid;
        if (m_ireq_pending)
            return;

        // LM32 is big endian
        if (lEndianInterface)
            m_inst.ins = soclib::endian::uint32_swap(rsp.instruction);
        else
            m_inst.ins = rsp.instruction ;
        m_ibe = rsp.error;

        // Hazard is possible when two consecutive instructions use the same destination
        // register and the first one make an external data access. This is due to the 
        // simulation pipline, the data access of the first instruction is effectively done
        // after the execution of the second instruction
        uint32_t dest_reg = get_absolute_dest_reg(m_inst);
        m_hazard = (dest_reg == m_dreq.dest_reg) && m_dreq.req.valid;
    } // _setInstrucrion


    // Set the data response
    tmpl(inline void)::_setData(const struct DataResponse &rsp) {
        if (!m_dreq.req.valid) // if no pending data request
        {
            m_dreq_pending = false;
            return;
        }

        // From here, a data request is pending
        // If the response is not valid, confirm that it is pending
        // and do nothing
        if(!rsp.valid) {
            m_dreq_pending = true;
            return;
        }

        // From here, we have a valid answer
        // We can de-assert the data request
        m_dreq.req.valid = false;

        // If there is a bus error, record it and do nothing
        m_dbe = rsp.error;
        if (rsp.error) return; 

        // lm32 is big endian
        // If request was a read (load) then get the swaped data
        if ((m_dreq.req.type == DATA_READ) || (m_dreq.req.type == XTN_READ)) {
            data_t data ;
            if (lEndianInterface) {
                data =  soclib::endian::uint32_swap(rsp.rdata);
                switch (m_dreq.req.be) {
                    case 8 : 
                        data = data & 0xff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 8);
                        break;
                    case 4 :
                        data = (data>>8) & 0xff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 8);
                        break;

                    case 2 :
                        data = (data>>16) & 0xff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 8);
                        break;

                    case 1 :
                        data = (data>>24) & 0xff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 8);
                        break;

                    case 12 :
                        data = data & 0xffff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 16);
                        break;

                    case 3 :
                        data = (data>>16) & 0xffff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 16);
                        break;
                }
            }
            else {
                data =  rsp.rdata ;
                switch (m_dreq.req.be) {
                    case 8 : 
                        data = (data>>24) & 0xff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 8);
                        break;
                    case 4 :
                        data = (data>>16) & 0xff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 8);
                        break;

                    case 2 :
                        data = (data>>8) & 0xff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 8);
                        break;

                    case 1 :
                        data = data & 0xff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 8);
                        break;

                    case 12 :
                        data = (data>>16) & 0xffff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 16);
                        break;

                    case 3 :
                        data = data & 0xffff;
                        if (m_dreq.sign_extend)
                            data = soclib::common::sign_ext(data, 16);
                        break;
                }
            }

            if (m_dreq.dest_reg != 0)
                r_gp[m_dreq.dest_reg] = data;
#ifdef SOCLIB_MODULE_DEBUG
            std::cout
                << name()
                << " setData: " << rsp
                << " type: " << m_dreq.req.type
                << " sign_ext: " << m_dreq.sign_extend
                << " dest: " << m_dreq.dest_reg
                << " data: " << data
                << std::endl;
#endif

        }
        else {
            // Transaction was a write... Just dump it !
#ifdef SOCLIB_MODULE_DEBUG
            std::cout
                << name()
                << " setData: " << rsp
                << " type: " << m_dreq.req.type
                << " sign_ext: " << m_dreq.sign_extend
                << std::endl;
#endif

        }
    } // _setData

}}

#undef tmpl
#endif // _SOCLIB_LM32_ISS_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


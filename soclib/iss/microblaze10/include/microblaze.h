/*
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
 * Copyright (c) UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Based on mips code
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *         Alain Greiner <alain.greiner@lip6.fr>, 2007
 *
 * Maintainers: nipo
 *
 * $Id$
 *
 * History:
 * - 2011-01-28
 *   Ramzi Raïs & Rufflart Thibaut :
 *   Forked mips32 to start Microblaze Iss2 ressource descriptions
 * - 2011/10/10
 *   Frédéric Pétrot
 *   Took over the results of the project, that did not even compile, uh!
 */

#ifndef _SOCLIB_MB_ISS_H_
#define _SOCLIB_MB_ISS_H_

#include <cassert>

#include "iss2.h"
#include "soclib_endian.h"
#include "register.h"
#include <strings.h>


#define R_IR_NOP        0x80000000

namespace soclib {
   namespace common {

      class MicroblazeIss:public Iss2 {

       public:
         static const size_t n_irq = 1;
       protected:

         enum Vectors {
            RESET_VECTOR     = 0x00000000,
            USER_VECTOR      = 0x00000008,
            INTERRUPT_VECTOR = 0x00000010,
            BREAK_VECTOR     = 0x00000018,
            EXCEPTION_VECTOR = 0x00000020,
            RESERVED_VECTOR  = 0x00000028,
         };

         enum ExceptCause {
            X_FSL,              //fast simplex link exception
            X_U,                //unaligned data access exception
            X_IOP,              //illegal opcode
            X_IPLB,             //instruction bus error exception
            X_DB,               //data bus error exception
            X_D,                //divide exception
            X_FPU,              //floating point unit exception
            X_PI,               //privileged instruction exception
            X_INT,              //interrupt exception
            X_ENMB,             //external non maskable break
            X_EMB,              //External maskable break
            X_DS,               //data storage exception
            X_IS,               //instruction storage exception
            X_DTLBM,            //data tlb miss exception
            X_ITLBM,            //instruction tlb miss exception
            NO_EXCEPTION,
         };
         // CPU registers
         data_t r_pc;           // Program Counter
         data_t r_npc;          // next program counter
         data_t r_gp[32];       // General Registers
         data_t r_imm;          // Temporary register for an immediate value from "imm"

         struct DataRequest m_dreq;
         int r_mem_byte_le;
         int r_mem_byte_count;
         uint32_t *r_mem_dest;

         bool m_ibe;
         bool m_dbe;

         // true if the last instruction was an "imm"
         bool m_imm;
         bool m_reservation;

         // Instruction latency simulation
         uint32_t m_ins_delay;

         struct CacheInfo m_cache_info;

         typedef union {
            struct {
               union {

                  // Format for type A instructions
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t rd:5,
                                  uint32_t ra:5,
                                  uint32_t rb:5,
                                  uint32_t sh:11) typeA;

                  // Format for type B instructions 
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t rd:5,
                                  uint32_t ra:5,
                                  uint32_t imm:16) typeB;

                  // Format for barrel shift instructions
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t rd:5,
                                  uint32_t ra:5,
                                  uint32_t zero:5,
                                  uint32_t st:6,
                                  uint32_t imm:5) typeBSI;

                  // Format for float comparison instructions
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t rd:5,
                                  uint32_t ra:5,
                                  uint32_t rb:5,
                                  uint32_t o:4,
                                  uint32_t opsel:3,
                                  uint32_t z:4) typeFCMP;

                  // Format for instructions concerning float operations
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t rd:5,
                                  uint32_t ra:5,
                                  uint32_t rb:5,
                                  uint32_t o:1,
                                  uint32_t fc:6,
                                  uint32_t zero:4) typeFLOAT;


                  // Format for mfs instruction
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t rd:5,
                                  uint32_t zero:5,
                                  uint32_t sel:2,
                                  uint32_t rs:14) typeMFS;

                  // Format for msrset/msrclr instructions
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t rd:5,
                                  uint32_t cc:6,
                                  uint32_t imm15:15,) typeMSR;

                  // Format for mts instruction
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t zero:5,
                                  uint32_t ra:5,
                                  uint32_t sel:2,
                                  uint32_t rs:14) typeMTS;

                  // Format for put/get instructions
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t rd:5,
                                  uint32_t ra:5,
                                  uint32_t put:1,
                                  uint32_t n:1,
                                  uint32_t c:1,
                                  uint32_t t:1,
                                  uint32_t a:1,
                                  uint32_t e:1,
                                  uint32_t zero:6,
                                  uint32_t fslx:4) typeFSLX;

                  // Format for putd/getd instructions
                  PACKED_BITFIELD(uint32_t op:6,
                                  uint32_t rd:5,
                                  uint32_t ra:5,
                                  uint32_t rb:5,
                                  uint32_t put:1,
                                  uint32_t n:1,
                                  uint32_t c:1,
                                  uint32_t t:1,
                                  uint32_t a:1,
                                  uint32_t e:1,
                                  uint32_t zero:5) typeFSLXD;

               } __attribute__ ((packed));
            } __attribute__ ((packed));
            uint32_t ins;
         } ins_t;

         enum MicroblazeMode {
            MB_KERNEL,
            MB_USER,
            MB_VIRTUAL,
         };

         enum Iss2::ExecMode r_bus_mode;
         enum MicroblazeMode r_cpu_mode;


         uint32_t r_count;
         uint32_t r_compare;

         uint32_t m_irqs;


         /* Configuration options */
         static const uint32_t C_DATA_SIZE = 32;
         static const uint32_t C_DYNAMIC_BUS_SIZING = 1;
         static const uint32_t C_SCO = 0;
         uint32_t C_AREA_OPTIMIZED;
         uint32_t C_PVR;
         uint32_t C_PVR_USER1;
         uint32_t C_PVR_USER2;
         uint32_t C_RESET_MSR;
         uint32_t C_USE_BARREL;
         uint32_t C_USE_DIV;
         uint32_t C_USE_HW_MUL;
         uint32_t C_USE_FPU;
         uint32_t C_USE_MSR_INSTR;
         uint32_t C_USE_PCMP_INSTR;
         uint32_t C_UNALIGNED_EXCEPTION;
         uint32_t C_ILL_OPCODE_EXCEPTION;
         uint32_t C_IPLB_BUS_EXCEPTION;
         uint32_t C_DPLB_BUS_EXCEPTION;
         uint32_t C_IOPB_BUS_EXCEPTION;
         uint32_t C_DOPB_BUS_EXCEPTION;
         uint32_t C_DIV_ZERO_EXCEPTION;
         uint32_t C_FPU_EXCEPTION;
         uint32_t C_OPCODE_0x0_ILLEGAL;
         uint32_t C_FSL_EXCEPTION;
         uint32_t C_DEBUG_ENABLED;
         uint32_t C_NUMBER_OF_RD_ADDR_BRK;
         uint32_t C_NUMBER_OF_WR_ADDR_BRK;
         uint32_t C_INTERRUPT_IS_EDGE;
         uint32_t C_EDGE_IS_POSITIVE;
         uint32_t C_FSL_LINKS;
         uint32_t C_USE_EXTENDED_FSL_INSTR;
         uint32_t C_ICACHE_BASEADDR;
         uint32_t C_ICACHE_HIGHADDR;
         uint32_t C_USE_ICACHE;
         uint32_t C_ALLOW_ICACHE_WR;
         uint32_t C_ICACHE_LINELEN;
         uint32_t C_ICACHE_ALWAYS_USED;
         uint32_t C_ICACHE_INTERFACE;
         uint32_t C_ADDR_TAG_BITS;
         uint32_t C_CACHE_BYTE_SIZE;
         uint32_t C_DCACHE_BASEADDR;
         uint32_t C_DCACHE_HIGHADDR;
         uint32_t C_USE_DCACHE;
         uint32_t C_ALLOW_DCACHE_WR;
         uint32_t C_DCACHE_LINELEN;
         uint32_t C_DCACHE_ALWAYS_USED;
         uint32_t C_DCACHE_INTERFACE;
         uint32_t C_DCACHE_USE_WRITEBACK;
         uint32_t C_DCACHE_ADDR_TAG;
         uint32_t C_DCACHE_BYTE_SIZE;
         uint32_t C_USE_MMU;
         uint32_t C_MMU_DTLB_SIZE;
         uint32_t C_MMU_ITLB_SIZE;
         uint32_t C_MMU_TLB_ACCESS;
         uint32_t C_MMU_ZONES;
         uint32_t C_USE_INTERRUPT;
         uint32_t C_USE_EXT_BRK;
         uint32_t C_USE_EXT_NM_BRK;

         // Special purpose registers bitfields

         // Machine Status Register
         typedef REG32_BITFIELD(uint32_t cc:1,
                                uint32_t reserved0:16,
                                uint32_t vms:1,
                                uint32_t vm:1,
                                uint32_t ums:1,
                                uint32_t um:1,
                                uint32_t pvr:1,
                                uint32_t eip:1,
                                uint32_t ee:1,
                                uint32_t dce:1,
                                uint32_t dzo:1,
                                uint32_t ice:1,
                                uint32_t fsl:1,
                                uint32_t bip:1,
                                uint32_t c:1,
                                uint32_t ie:1,
                                uint32_t be:1) msr_t;

         // Processor Version Registers
         typedef REG32_BITFIELD(uint32_t cfg:1,
                                uint32_t bs:1,
                                uint32_t div:1,
                                uint32_t mul:1,
                                uint32_t fpu:1,
                                uint32_t exc:1,
                                uint32_t icu:1,
                                uint32_t dcu:1,
                                uint32_t mmu:1,
                                uint32_t reserved:7,
                                uint32_t mbv:8,
                                uint32_t usr1:8) pvr0_t;

         typedef REG32_BITFIELD(uint32_t dopb:1,
                                uint32_t dlmb:1,
                                uint32_t iopb:1,
                                uint32_t ilmb:1,
                                uint32_t irqedge:1,
                                uint32_t irqpos:1,
                                uint32_t dplb:1,
                                uint32_t iplb:1,
                                uint32_t intercon:1,
                                uint32_t reserved:3,
                                uint32_t fsl:1,
                                uint32_t fslexc:1,
                                uint32_t msr:1,
                                uint32_t pcmp:1,
                                uint32_t area:1,
                                uint32_t bs:1,
                                uint32_t div:1,
                                uint32_t mul:1,
                                uint32_t fpu:1,
                                uint32_t mul64:1,
                                uint32_t fpu2:1,
                                uint32_t iplbexc:1,
                                uint32_t dplbexc:1,
                                uint32_t op0exc:1,
                                uint32_t unexc:1,
                                uint32_t opexc:1,
                                uint32_t iopbexc:1,
                                uint32_t dopbexc:1,
                                uint32_t divexc:1,
                                uint32_t fpuexc:1) pvr2_t;

         typedef REG32_BITFIELD(uint32_t debug:1,
                                uint32_t reserved:2,
                                uint32_t pcbrk:4,
                                uint32_t reserved1:3,
                                uint32_t rdaddr:3,
                                uint32_t reserved2:3,
                                uint32_t wraddr:3,
                                uint32_t reserved3:1,
                                uint32_t fsl:5,
                                uint32_t reserved4:7) pvr3_t;

         typedef REG32_BITFIELD(uint32_t icu:1,
                                uint32_t icts:5,
                                uint32_t reserved:1,
                                uint32_t icw:1,
                                uint32_t icll:3,
                                uint32_t icbs:5,
                                uint32_t iau:1,
                                uint32_t reserved1:1,
                                uint32_t ici:1,
                                uint32_t reserved2:13) pvr4_t;

         typedef REG32_BITFIELD(uint32_t dcu:1,
                                uint32_t dcts:5,
                                uint32_t reserved:1,
                                uint32_t dcw:1,
                                uint32_t dcll:3,
                                uint32_t dcbs:5,
                                uint32_t dau:1,
                                uint32_t dwb:1,
                                uint32_t dci:1,
                                uint32_t reserved1:13) pvr5_t;

         typedef REG32_BITFIELD(uint32_t arch:8,
                                uint32_t reserved:24) pvr10_t;

         typedef REG32_BITFIELD(uint32_t mmu:2,
                                uint32_t itlb:3,
                                uint32_t dtlb:3,
                                uint32_t tlbacc:2,
                                uint32_t zones:5,
                                uint32_t reserved:6,
                                uint32_t rstmsr:11) pvr11_t;

         // Exception Status Register
         typedef union {
            struct {
               union {

                  PACKED_BITFIELD(uint32_t reserved0:19,
                                  uint32_t ds:1,
                                  uint32_t ess:7,
                                  uint32_t ec:5) typeBASE;

                  PACKED_BITFIELD(uint32_t reserved0:19,
                                  uint32_t ds:1,
                                  uint32_t tw:1,
                                  uint32_t ts:1,
                                  uint32_t rx:5,
                                  uint32_t ec:5) typeUDA;

                  PACKED_BITFIELD(uint32_t reserved0:19,
                                  uint32_t ds:1,
                                  uint32_t ess:7,
                                  uint32_t ec:5) typeDIV;

                  PACKED_BITFIELD(uint32_t reserved0:19,
                                  uint32_t ds:1,
                                  uint32_t res:3,
                                  uint32_t fsl:4,
                                  uint32_t ec:5) typeFSL;


               } __attribute__ ((packed));
            } __attribute__ ((packed));
            uint32_t esr;
         } esr_t;

         // Floating Point Status Register
         typedef REG32_BITFIELD(uint32_t reserved0:27,
                                uint32_t io:1,
                                uint32_t dz:1,
                                uint32_t of:1,
                                uint32_t uf:1,
                                uint32_t dou:1) fsr_t;

         // Process Identifier Register
         typedef REG32_BITFIELD(uint32_t reserved0:24,
                                uint32_t pid:8) pid_t;

         // Zone Protection Register
         typedef REG32_BITFIELD(uint32_t zp0:2,
                                uint32_t zp1:2,
                                uint32_t zp2:2,
                                uint32_t zp3:2,
                                uint32_t zp4:2,
                                uint32_t zp5:2,
                                uint32_t zp6:2,
                                uint32_t zp7:2,
                                uint32_t zp8:2,
                                uint32_t zp9:2,
                                uint32_t zp10:2,
                                uint32_t zp11:2,
                                uint32_t zp12:2,
                                uint32_t zp13:2,
                                uint32_t zp14:2,
                                uint32_t zp15:2) zpr_t;

         // Translation Look-Aside Buffer Low Register
         typedef REG32_BITFIELD(uint32_t rpn:22,
                                uint32_t ex:1,
                                uint32_t wr:1,
                                uint32_t zsel:4,
                                uint32_t w:1,
                                uint32_t i:1,
                                uint32_t m:1,
                                uint32_t g:1) tlblo_t;

         // Translation Look-Aside Buffer High Register
         typedef REG32_BITFIELD(uint32_t tag:22,
                                uint32_t size:3,
                                uint32_t v:1,
                                uint32_t e:1,
                                uint32_t uo:1,
                                uint32_t reserved0:4) tlbhi_t;

         // Translation Look-Aside Buffer Index Register
         typedef REG32_BITFIELD(uint32_t miss:1,
                                uint32_t reserved0:25,
                                uint32_t index:6) tlbx_t;

         // Translation Look-Aside Buffer Search Index Register
         typedef REG32_BITFIELD(uint32_t vpn:22,
                                uint32_t reserved0:10) tlbsx_t;

    /*\
     * MicroBlaze Special Registers
    \*/
         msr_t   r_msr;         //machine status register
         data_t  r_ear;         //exception address register
         esr_t   r_esr;         //exception status register
         data_t  r_btr;         //branch target register
         fsr_t   r_fsr;         //floating point register
         data_t  r_edr;         //exception data register
         pid_t   r_pid;         //process identifier register
         zpr_t   r_zpr;         //zone protection register
         tlblo_t r_tlblo;       //translation Look-Aside Buffer Low Register
         tlbhi_t r_tlbhi;       //translation Look-Aside Buffer High Register
         tlbx_t  r_tlbx;
         tlbsx_t r_tlbsx;

         /* MicroBlaze Processor Version Registers
          * pvr[1] (USR_2 as it is called) is procnum */
         uint32_t r_pvr[12];

         // member variables used for communication between
         // member functions (they are not registers)
         ins_t m_ins;
         addr_t m_ifetch_addr;
         addr_t m_next_pc;
         addr_t m_jump_pc;

         uint32_t m_exec_cycles;

         addr_t m_pc_for_dreq;
         bool m_pc_for_dreq_is_ds;

         enum ExceptCause m_exception;
         addr_t m_resume_pc;
         addr_t m_error_addr;

         static const uint32_t m_reset_address = RESET_VECTOR;

       public:
         MicroblazeIss(const std::string & name, uint32_t ident);
         void dump() const;

         uint32_t executeNCycles(uint32_t ncycle,
                                 const struct InstructionResponse &irsp,
                                 const struct DataResponse &drsp,
                                 uint32_t irq_bit_field);

         inline void getRequests(struct InstructionRequest &ireq,
                                 struct DataRequest &dreq) const
         {
            ireq.valid = 1;
            ireq.addr = m_ifetch_addr;
            ireq.mode = r_bus_mode;
            dreq = m_dreq;
         }

         inline void setWriteBerr()
         {
            m_dbe = true;
         }
         void reset();

         inline unsigned int debugGetRegisterCount() const
         {
            return 36;
         }

         virtual debug_register_t debugGetRegisterValue(unsigned int reg) const;

         inline size_t debugGetRegisterSize(unsigned int reg) const
         {
            return 32;
         }

         virtual void debugSetRegisterValue(unsigned int reg, debug_register_t value);

         void setCacheInfo(const struct CacheInfo &info);

       protected:
         void run();

         void run_for(uint32_t &ncycle, uint32_t &time_spent,
                      uint32_t in_pipe, uint32_t stalled);

         inline void setInsDelay(uint32_t delay)
         {
            assert(delay > 0);
            m_ins_delay = delay - 1;
         }

         addr_t exceptAddr(enum ExceptCause cause) const;

         inline bool isPriviliged() const
         {
            return r_cpu_mode != MB_USER;
         }

         inline bool isPrivDataAddr(addr_t addr) const
         {
            if (m_cache_info.has_mmu)
               return false;
            return addr & (addr_t)0x80000000;
         }

         /*
          * Instruction support and declarations.
          */
         typedef void (MicroblazeIss::*func_t) ();

         static func_t const opcod_table[64];
         static func_t const float_table[64];
         static func_t const cb_table[32];
         static func_t const cbi_table[32];
         static func_t const ub_table[32];
         static func_t const ubi_table[32];
         static func_t const bs_table[8];
         static func_t const bsi_table[8];
         static func_t const icmp_table[4];
         static func_t const div_table[4];
         static func_t const msr_table[4];
         static func_t const mult_table[4];

         static const char *name_table[64];

         /*
          * Decoding helpers
          */
         void insn_andpc();
         void insn_cb();
         void insn_cbi();
         void insn_float();
         void insn_load();
         void insn_mult();
         void insn_orpc();
         void insn_return();
         void insn_store();
         void insn_xorpc();

         /*
          * Special cases
          */
         void insn_ill();
         void insn_unimpl();

         /*
          * Instructions
          */
         void insn_add();
         void insn_addc();
         void insn_addi();
         void insn_addic();
         void insn_addik();
         void insn_addikc();
         void insn_addk();
         void insn_addkc();
         void insn_and();
         void insn_andi();
         void insn_andn();
         void insn_andni();
         void insn_beq();
         void insn_beqd();
         void insn_beqi();
         void insn_beqid();
         void insn_bge();
         void insn_bged();
         void insn_bgei();
         void insn_bgeid();
         void insn_bgt();
         void insn_bgtd();
         void insn_bgti();
         void insn_bgtid();
         void insn_ble();
         void insn_bled();
         void insn_blei();
         void insn_bleid();
         void insn_blt();
         void insn_bltd();
         void insn_blti();
         void insn_bltid();
         void insn_bne();
         void insn_bned();
         void insn_bnei();
         void insn_bneid();
         void insn_br();
         void insn_bra();
         void insn_brad();
         void insn_brai();
         void insn_braid();
         void insn_brald();
         void insn_bralid();
         void insn_brd();
         void insn_bri();
         void insn_brim();
         void insn_brid();
         void insn_brk();
         void insn_brki();
         void insn_brld();
         void insn_brlid();
         void insn_bs();
         void insn_bsi();
         void insn_bsll();
         void insn_bslli();
         void insn_bsra();
         void insn_bsrai();
         void insn_bsrl();
         void insn_bsrli();
         void insn_clz();
         void insn_cmp();
         void insn_cmpu();
         void insn_div();
         void insn_fadd();
         void insn_fcmpeq();
         void insn_fcmpge();
         void insn_fcmpgt();
         void insn_fcmple();
         void insn_fcmplt();
         void insn_fcmpne();
         void insn_fcmpun();
         void insn_fidv();
         void insn_fint();
         void insn_flt();
         void insn_fmul();
         void insn_frsub();
         void insn_fsl();
         void insn_fsld();
         void insn_fsqrt();
         void insn_icmp();
         void insn_idiv();
         void insn_idivu();
         void insn_imm();
         void insn_lbu();
         void insn_lbui();
         void insn_lhu();
         void insn_lhui();
         void insn_lw();
         void insn_lwi();
         void insn_lwx();
         void insn_mbar();
         void insn_mfs();
         void insn_misc();
         void insn_msr();
         void insn_msrclr();
         void insn_msrset();
         void insn_mts();
         void insn_mul();
         void insn_mulh();
         void insn_mulhsu();
         void insn_mulhu();
         void insn_muli();
         void insn_or();
         void insn_ori();
         void insn_pcmpbf();
         void insn_pcmpeq();
         void insn_pcmpne();
         void insn_rsub();
         void insn_rsubc();
         void insn_rsubi();
         void insn_rsubic();
         void insn_rsubik();
         void insn_rsubikc();
         void insn_rsubk();
         void insn_rsubkc();
         void insn_rtbd();
         void insn_rted();
         void insn_rtid();
         void insn_rtsd();
         void insn_sb();
         void insn_sbi();
         void insn_sext16();
         void insn_sext8();
         void insn_sh();
         void insn_shi();
         void insn_sra();
         void insn_src();
         void insn_srl();
         void insn_sw();
         void insn_swi();
         void insn_swx();
         void insn_ub();
         void insn_ubi();
         void insn_wdc();
         void insn_wdcclear();
         void insn_wdcflush();
         void insn_wic();
         void insn_xor();
         void insn_xori();
         void insn_xorp();

         void do_mem_access(addr_t address,
                            int byte_count,
                            uint32_t * dest,
                            data_t wdata,
                            enum DataOperationType operation);


         typedef enum {
            USE_NONE = 0,
            USE_T = 1,
            USE_S = 2,
            USE_ST = 3,
            USE_SPECIAL = 4,
         } use_t;

         use_t curInstructionUsesRegs();

         void update_mode();

         inline void jump(addr_t dest, bool now);
         inline bool check_irq_state() const;
         bool handle_ifetch(const struct InstructionResponse &irsp);
         bool handle_dfetch(const struct DataResponse &drsp);
         void handle_exception();
      };


}}

#endif // _SOCLIB_MB_ISS_H_

// vim: filetype=cpp:expandtab:shiftwidth=3:tabstop=3:softtabstop=3

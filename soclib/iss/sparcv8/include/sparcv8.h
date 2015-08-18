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
 * Copyright (c) Telecom ParisTech
 *         Alexis Polti <polti@telecom-paristech.fr>, 2008
 *
 * Based on mips32 code
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *         Alain Greiner <alain.greiner@lip6.fr>, 2007
 *
 * $Id$
 *
 * History:
 * - 2008-10-10
 *   Alexis Polti : Forked mips32 to begin sparcv8
 */

/*
 * This component is a SPARC V8 processor core.
 * 
 * This simulation model is build as a two-stages pipeline, to be used 
 * in the ISS2 Soclib wrapper.
 *   - the first stage corresponds to instruction fetch / execute
 *   - the second stage corresponds to memory acces
 * The delayed branch is accurately modelised.
 *
 * It has the following features (or limitations) :
 * 
 * - This model implements a simple Integer Unit (IU). The Coprocessor (CP)
 *   is not implemented. An optional  FPU may be enable by defining the 
 *   compilation flag FPU. Any CP instruction (or FPU instruction when FPU 
 *   is not compiled in) causes a TRAP.
 *
 * - This model has a four bits interrupt interface, defining the
 * interrupt priority level. The '0' value means "no pending interrupt".
 * The '15' value is the highest (non maskable) priority.
 * 
 * - SWAP and LDSTUB are currently emulated, the same way as LL/SC on MIPS.
 *   When executed, the infrastructure verify that they succeeded. When not, 
 *   they may either trap or be automatically re-executed until successfull
 *   execution. The compilation flag SWAP_TRAPS decides which strategy to use.
 *   Warning : when automatic re-execution is choosen, the system may deadlock.
 *
 * - ASI are currently not supported.
 *
 * - This model mplements two "ancillary State register" (ASR)
 * ASR[0] xis the YYY register used by multiply/divide instructions
 * ASR[16] is the IDENT register, that contains the processor ID.
 * The WRASR & RDASR instructions must execute in kernel mode.
 *
 * This component has one "template parameters :
 *   - NWINDOWS  : defines the number of register windows [2...32]
 *
 * TODO : 
 *   - implement ASI
 *   - support MMU (through vcache_wrapper)
 *   - refactor instruction decoding
 *   - make FPU compilation flag to a template parameter
 */

#ifndef _SOCLIB_SPARCV8_ISS_H_
#define _SOCLIB_SPARCV8_ISS_H_

#include <cassert>
#include "iss2.h"
#include "soclib_endian.h"
#include "register.h"

#define FPU 1

#define TP_RESET                                0x00
#define TP_INSTRUCTION_ACCESS_MMU_MISS          0x20
#define TP_INSTRUCTION_ACCESS_ERROR             0x21 
#define TP_INSTRUCTION_ACCESS_EXCEPTION         0x01
#define TP_PRIVILEGED_INSTRUCTION               0x03
#define TP_ILLEGAL_INSTRUCTION                  0x02
#define TP_FP_DISABLED                          0x04
#define TP_CP_DISABLED                          0x24
#define TP_WINDOW_OVERFLOW                      0x05
#define TP_WINDOW_UNDERFLOW                     0x06
#define TP_MEM_ADDRESS_NOT_ALIGNED              0x07
#define TP_FP_EXCEPTION                         0x08
#define TP_CP_EXCEPTION                         0x28
#define TP_DATA_ACCESS_ERROR                    0x29  
#define TP_DATA_ACCESS_MMU_MISS                 0x2C
#define TP_DATA_ACCESS_EXCEPTION                0x09
#define TP_TAG_OVERFLOW                         0x0A
#define TP_DIVISION_BY_ZERO                     0x2A
#define TP_TRAP_INSTRUCTION(x)                  (0x80 | (x & 0x7f))
#define TP_INTERRUPT_LEVEL(x)                   (0x10 | (x & 0xf))
#define TP_UNIMPLEMENTED_INSTRUCTION            0x0F

#define TP_IEEE_754_EXCEPTION                   0x01
#define TP_UNFINISHED_FPOP                      0x02
#define TP_UNIMPLEMENTED_FPOP                   0x03
#define TP_SEQUENCE_ERROR                       0x04
#define TP_HARDWARE_ERROR                       0x05
#define TP_INVALID_FP_REGISTER                  0x06

#define IEEE754_INVALID                         0x10
#define IEEE754_OVERFLOW                        0x08
#define IEEE754_UNDERFLOW                       0x04
#define IEEE754_DIVBYZERO                       0x02
#define IEEE754_INEXACT                         0x01


#define DEFAULT_TBA 0x00000000;

// GPR(x) with x = (0, ... , 31) : register x (depends on current CWP)
#define RR_CWP(x, cwp)                                  \
	(((x >= 0) && (x <= 7)) ?  x                        \
     : ((x - 8 + (cwp * 16)) % (16 * NWINDOWS)) + 8)
#define RR(x) RR_CWP(x, r_psr.cwp)
#define GPR_CWP(x, cwp) r_gp[RR_CWP(x, cwp)]
#define GPR(x) GPR_CWP(x, r_psr.cwp)

namespace soclib { namespace common {

template <unsigned int NWINDOWS>
class Sparcv8Iss
    : public Iss2
{
public:
    static const int n_irq = 4; 
    
private:
    typedef unsigned char trap_t;
    // Type of destination register for LOADs : 
    // global purpose, floating point reg, FSR, coproc reg, CSR, ...
    typedef enum{TYPE_NONE, TYPE_GP, TYPE_F, TYPE_FSR, TYPE_C, TYPE_CSR} regtype_t;         

    // member variables (internal registers)
    typedef REG32_BITFIELD(
                           uint32_t impl:4,
                           uint32_t ver:4,
                           uint32_t n:1,
                           uint32_t z:1,
                           uint32_t v:1,
                           uint32_t c:1,
                           uint32_t reserved:6,
                           uint32_t ec:1,
                           uint32_t ef:1,
                           uint32_t pil:4,
                           uint32_t s:1,
                           uint32_t ps:1,
                           uint32_t et:1,
                           uint32_t cwp:5
                           ) psr_t;
    
    typedef REG32_BITFIELD(
                           uint32_t tba:20,
                           uint32_t tt:8,
                           uint32_t zero:4
                           ) tbr_t;

    typedef REG32_BITFIELD(
                       uint32_t rd:2,
                       uint32_t unused:2,
                       uint32_t tem:5,
                       uint32_t ns:1,
                       uint32_t reserved:2,
                       uint32_t ver:3,
                       uint32_t ftt:3,
                       uint32_t qne:1,
                       uint32_t unused2:1,
                       uint32_t fcc:2,
                       uint32_t aexc:5,
                       uint32_t cexc:5,
                       ) fsr_t;

    typedef union {
        union {
            PACKED_BITFIELD(
                            uint32_t op:2,
                            uint32_t disp:30
                            ) call;
            
            PACKED_BITFIELD(
                            uint32_t op:2,
                            uint32_t rd:5,
                            uint32_t op2:3,
                            uint32_t imm:22
                            ) sethi;
            PACKED_BITFIELD(
                            uint32_t op:2,
                            uint32_t a:1,
                            uint32_t cond:4,
                            uint32_t op2:3,
                            uint32_t disp:22
                            ) branches;
                
            PACKED_BITFIELD(
                            uint32_t op:2,
                            uint32_t rd:5,
                            uint32_t op3:6,
                            uint32_t rest:19,
                            ) format3;
            PACKED_BITFIELD(
                            uint32_t op:2,
                            uint32_t rd:5,
                            uint32_t op3:6,
                            uint32_t rs1:5,
                            uint32_t i:1,
                            uint32_t asi:8,
                            uint32_t rs2:5
                            ) format3a;
            PACKED_BITFIELD(
                            uint32_t op:2,
                            uint32_t rd:5,
                            uint32_t op3:6,
                            uint32_t rs1:5,
                            uint32_t i:1,
                            uint32_t imm:13
                            ) format3b;
            PACKED_BITFIELD(
                            uint32_t op:2,
                            uint32_t rd:5,
                            uint32_t op3:6,
                            uint32_t rs1:5,
                            uint32_t opf:9,
                            uint32_t rs2:5
                            ) format3c;
        } __attribute__((packed));
        uint32_t ins;
    } ins_t;



	data_t      r_pc;                   // Program Counter
	data_t      r_npc;                  // Next Program Counter
	data_t      r_prev_pc;              // Previous Program Counter (in case of DBE)
    data_t      r_gp[NWINDOWS*16+8];    // Register file
    psr_t       r_psr;                  // Program Status Register
    psr_t       r_psr_write;            // Program Status Register delayed write
    int         r_psr_delay;            // Program Status Register delay counter
    data_t      r_wim;                  // Window Invalid Mask
    tbr_t       r_tbr;                  // Trap Base Register
    data_t      r_y;                    // Multiply / divide (ASR 0)
    bool        r_error_mode;           // True is sparc is in error mode
    int32_t     r_f[32];                // 32 floating point registers
    fsr_t       r_fsr;                  // Program Status Register

    ins_t       m_ins;                  // Current instruction
    bool        m_ibe;                  // Instruction bus error
    bool        m_dbe;                  // Data bus error

    uint32_t    m_ins_delay;            // Instruction latency simulation

    // These three members are used to check if an instruction is allowed to run or not
    bool        m_ireq_pending;         // True while an instruction request is pending
    bool        m_dreq_pending;         // True while a data request is pending
    bool        m_hazard;               // true if there is a hazard between previous load/store and the current instruction
    bool        m_wait_irq;             // processor in idle state, waiting for irq

    static int  m_bootstrap_cpu_id;
    bool        m_reset_wait_irq;       // procrssor in idle state after reset

    bool        m_exception;            // true if a trap is generated
    trap_t      m_exception_cause;      // explain which trap it is

    bool        m_cancel_next_ins;      // True if instruction in delay slot has to be canceled
    addr_t      m_next_pc;              // Next PC value (stored in r_npc at the end of instruction execution)


    // A data transaction in Sparc can be either simple (byte, short, word) 
    // or complex : load double word, store double word, load and store, swap, ...
    // Complex transactions are split in two simple sub-requests. This structure holds
    // the fields to describe a transaction (either simple or complex).
    typedef struct mem_access {
        addr_t          addr;           // Address (fully qualified) of transaction
        bool            sign_extend;    // Need to sign extend the loaded value ?        
        DataRequest     req;            // First (or only) data request
        DataRequest     ext_req;        // For double words requests (SWAP, LDD, LDSTUB), this is the second 
                                        // part of the request
        int32_t         cmp_reg;        // in case of compare and swap, register used for compare. -1 in other LL/SC cases 
        uint32_t        dest_reg;       // Destination register (for a load). In case of LDD/STD, this is the first
                                        // of the register pair. This index is an absolute index : a reference in the
                                        // flat register file (ie : independent of CWP).
        uint32_t        ext_dest_reg;   // Destination register for the extended request (SWAP, LDD, STD, LDSTUB...).
        regtype_t       regtype;        // The type of the destination registers (GP, F, FSR, CP...)
    } mem_access_t;

    mem_access_t        m_dreq;         // The data transaction


public:
    // Constructor
    Sparcv8Iss(const std::string &name, uint32_t ident);

    ~Sparcv8Iss();

    // Reset function
    void reset();

	void getRequests( struct InstructionRequest &ireq,
                      struct DataRequest &dreq ) const;
    uint32_t executeNCycles( uint32_t ncycle,
                             const struct InstructionResponse &irsp,
                             const struct DataResponse &drsp,
                             uint32_t irq_bit_field );

    // Data bus error are signaled asynchronously
    inline void setWriteBerr()
    {
		m_dbe = true;
    }

    // Processor internal registers access API, used by debugger. 
    // SPARC v8 order is 32 general-purpose; sr; lo; hi; bad; cause; pc;
    unsigned int debugGetRegisterCount() const;
    debug_register_t debugGetRegisterValue(unsigned int reg) const;
    void debugSetRegisterValue(unsigned int reg, debug_register_t value);
    size_t debugGetRegisterSize(unsigned int reg) const;

    static inline void setBoostrapCpuId(int id = -1)
    {
        m_bootstrap_cpu_id = id;
    }

    static const unsigned int s_sp_register_no = 14; // o6
    static const unsigned int s_fp_register_no = 30; // i6
    static const unsigned int s_pc_register_no = 68;
    static const Iss2::debugCpuEndianness s_endianness = Iss2::ISS_BIG_ENDIAN;

private:
    void run();

    void setData(const struct DataResponse &drsp);

    // Opcode tables
    typedef struct {
        void (Sparcv8Iss::*func)();
        const std::string name;
    } func_entry;

    static func_entry const format2_table[8];
    static func_entry const branch_table[16];
    static func_entry const fbranch_table[16];
    static func_entry const cbranch_table[16];
    static func_entry const logical_table[64];
    static func_entry const ticc_table[16];
    static func_entry const loadstore_table[64];
    static func_entry const fpop1_table[256];
    static func_entry const fpop2_table[8];

    bool evaluate_icc(unsigned char cond);
    bool inline evaluate_ficc(unsigned char cond);
    uint32_t get_absolute_dest_reg(ins_t ins) const;

    inline void setInsDelay( uint32_t delay )
    {
        assert( delay > 0 );
        m_ins_delay = delay-1;
    }

    // Debug functions
    void dump_pc(const std::string &msg) const;
    void dump_regs(const std::string &msg) const;
    std::string get_ins_name(void) const;
    static std::string GetTrapName(int type);
    

    // This declares all op functions
#define op(x) void op_##x()
#define op2(x, y) op(x); op(y);
#define op3(x, y, z) op(x); op(y); op(z);
#define op4(x, y, z, t) op(x); op(y); op(z); op(t);
    op3(unimp,  sethi,  call);
    op3(branch, cbranch, fbranch);
    op4(bn,     be,     ble,    bl );
    op4(bleu,   bcs,    bneg,   bvs);
    op4(ba,     bne,    bg,     bge);
    op4(bgu,    bcc,    bpos,   bvc);
    op4(fbn,    fbne,   fblg,   fbul);
    op4(fbl,    fbug,   fbg,    fbu);
    op4(fba,    fbe,    fbue,   fbge);
    op4(fbuge,  fble,   fbule,  fbo);

    op4(fmovs,  fnegs,  fabss,  fsqrts);
    op4(fsqrtd, fsqrtq, fadds,  faddd);
    op4(faddq,  fsubs,  fsubd,  fsubq);
    op4(fmuls,  fmuld,  fmulq,  fdivs);
    op4(fdivd,  fdivq,  fsmuld, fdmulq);
    op4(fitos,  fdtos,  fqtos,  fitod);
    op4(fstod,  fqtod,  fitoq,  fstoq);
    op4(fdtoq,  fstoi,  fdtoi,  fqtoi);
    op4(fcmps,  fcmpd,  fcmpq,  fcmpes);
    op3(fcmped, fcmpeq, fpill);

    op4(cbn,    cb123,  cb12,   cb13);
    op4(cb1,    cb23,   cb2,    cb3);
    op4(cba,    cb0,    cb03,   cb02);
    op4(cb023,  cb01,   cb013,  cb012);
    op4(add,    and,    or,     xor);
    op4(sub,    andn,   orn,    xnor);
    op3(addx,   umul,   smul);
    op3(subx,   udiv,   sdiv);
    op4(addcc,  andcc,  orcc,   xorcc);
    op4(subcc,  andncc, orncc,  xnorcc);
    op3(addxcc, umulcc, smulcc);
    op3(subxcc, udivcc, sdivcc);
    op4(taddcc, tsubcc, taddcctv, tsubcctv);
    op4(mulscc, sll,    srl,    sra);
    op4(rdasr,  rdpsr,  rdwim,  rdtbr);
    op4(wrasr,  wrpsr,  wrwim,  wrtbr);
    op4(fpop1,  fpop2,  cpop1,  cpop2);
    op4(jmpl,   rett,   ticc,   flush);
    op2(save,   restore);
    op4(ld,     ldub,   lduh,   ldd);
    op4(st,     stb,    sth,    std);
    op2(ldsb,   ldsh);
    op2(ldstub, swap);
    op4(lda,    lduba,  lduha,  ldda);
    op4(sta,    stba,   stha,   stda);
    op2(ldsba,  ldsha);
    op3(ldstuba,swapa,  casa);
    op3(ldf,    ldfsr,  lddf);
    op4(stf,    stfsr,  stdfq,  stdf);
    op3(ldc,    ldcsr,  lddc);
    op4(stc,    stcse,  stdcq,  stdc);
    op4(tn,     te,     tle,    tl);
    op4(tleu,   tcs,    tneg,   tvs);
    op4(ta,     tne,    tg,     tge);
    op4(tgu,    tcc,    tpos,   tvc);
#undef op
#undef op2
#undef op3
#undef op4


};

}}

#endif // _SOCLIB_SPARCV8_ISS_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

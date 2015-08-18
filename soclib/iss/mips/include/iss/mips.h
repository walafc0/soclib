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
 * Copyright (c) UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *         Alain Greiner <alain.greiner@lip6.fr>, 2007
 *
 * Maintainers: nipo
 *
 * $Id$
 *
 * History:
 * - 2007-06-15
 *   Nicolas Pouillon, Alain Greiner: Model created
 */

/*
 * Functionnal description:
 * The tree files mips_iss.h, mips_iss.cpp, & mips_instructions.cpp
 * define an Instruction Set Simulator for the MIPS R3000 processor.
 * The same ISS has been wrapped to build CABA, TLMT, and PV simulation
 * models,  using appropriate wrappers.
 *
 * - One instruction is executed in one "step".
 *   (In case of a timed model one step corresponds to one cycle).
 * - The MIPS R3000 delayed branchs are supported: The ISS internal
 *   state contains two program counters (r_pc & r_npc), in order
 *   to describe the delayed branch behaviour.
 * - The MIPS R3000 delayed load behaviour : in case of RaW dependency
 *   between two successive instructions, the processor is dtalled
 *   during one step.
 * - The MIPS R3000 delayed multiply and divide behaviour is NOT
 *   supported : the mul and div instructions are supposed to be
 *   executed in one step.
 */

#ifndef _SOCLIB_MIPS_ISS_H_
#define _SOCLIB_MIPS_ISS_H_

#include "iss.h"
#include "soclib_endian.h"
#include "register.h"

#define MIPS_DEBUG 0

namespace soclib { namespace common {

class MipsIss
    : public soclib::common::Iss
{
public:
    static const int n_irq = 6;
    static const unsigned int s_sp_register_no = 29;
    static const unsigned int s_pc_register_no = 37;

private:
    enum Addresses {
        EXCEPT_ADDRESS  = 0x80000080,
        RESET_ADDRESS   = 0xbfc00000
    };

    enum ExceptCause {
        X_INT,      // Interrupt
        X_MOD,      // TLB Modification
        X_TLBL,     // TLB Load error
        X_TLBS,     // TLB Store error
        X_ADEL,     // Address error (load or fetch)
        X_ADES,     // Address error (store)
        X_IBE,      // Ins bus error
        X_DBE,      // Data bus error (load/store)
        X_SYS,      // Syscall
        X_BP,       // Break point
        X_RI,       // Reserved
        X_CPU,      // Coproc unusable
        X_OV,       // Overflow
        X_TR,       // Trap
        X_reserved,     // Reserved
        X_FPE,      // Floating point
    };

    // member variables (internal registers)

	uint32_t 	r_pc;			// Program Counter
	uint32_t 	r_npc;			// Next Program Counter
	uint32_t 	r_ppc;			// Previous Program Counter
    uint32_t    r_gp[32];       // General Registers
    uint32_t    r_hi;           // Multiply result (MSB bits)
    uint32_t    r_lo;           // Multiply result (LSB bits)

    bool        r_mem_req;
    bool        r_mem_unsigned;
	enum DataAccessType 	r_mem_type;  		// Data Cache access type
	uint32_t 	r_mem_addr;  		// Data Cache address
	uint32_t 	r_mem_wdata;  		// Data Cache data value (write)
	uint32_t	r_mem_dest;  		// Data Cache destination register (read)
    int         r_mem_shift;

    bool        r_branch;        // Identify a branch inst
    bool        r_branch_delay;  // Identify a branch delayed slot
    
	uint32_t	m_rdata;
	uint32_t 	m_irq;
	bool		m_ibe;
	bool		m_dbe;

    // Instruction latency simulation
    uint32_t m_ins_delay;


    typedef union {
        struct {
            union {
                PACKED_BITFIELD(
                    uint32_t op:6,
                    uint32_t imd:26
                    ) j;
                PACKED_BITFIELD(
                    uint32_t op:6,
                    uint32_t rs:5,
                    uint32_t rt:5,
                    uint32_t imd:16
                    ) i;
                PACKED_BITFIELD(
                    uint32_t op:6,
                    uint32_t rs:5,
                    uint32_t rt:5,
                    uint32_t rd:5,
                    uint32_t sh:5,
                    uint32_t func:6
                    ) r;
                PACKED_BITFIELD(
                    uint32_t op:6,
                    uint32_t action:5,
                    uint32_t rt:5,
                    uint32_t rd:5,
                    uint32_t zero:8,
                    uint32_t sel:3
                    ) coproc;
            } __attribute__((packed));
        } __attribute__((packed));
        uint32_t ins;
    } ins_t;

    typedef REG32_BITFIELD(
        uint32_t reserved0:16,
        uint32_t im:6,
        uint32_t sim:2,
        uint32_t reserved1:2,
        uint32_t kuo:1,
        uint32_t ieo:1,
        uint32_t kup:1,
        uint32_t iep:1,
        uint32_t kuc:1,
        uint32_t iec:1,
        ) status_t;

    typedef REG32_BITFIELD(
        uint32_t bd:1,
        uint32_t reserved0:15,
        uint32_t irq:6,
        uint32_t swi:2,
        uint32_t reserved1:2,
        uint32_t xcode:4,
        uint32_t reserved2:2,
        ) cause_t;

    typedef REG32_BITFIELD(
        uint32_t m:1,
        uint32_t k23:3,
        uint32_t ku:3,
        uint32_t impl:9,
        uint32_t be:1,
        uint32_t at:2,
        uint32_t ar:3,
        uint32_t mt:3,
        uint32_t zero:3,
        uint32_t vi:1,
        uint32_t k0:3,
        ) config_t;

    status_t r_status;
    cause_t r_cause;
    uint32_t r_bar;
    uint32_t r_epc;
    uint32_t r_count;
    
    // member variables used for communication between
    // member functions (they are not registers)

    ins_t       m_ins;
    uint32_t    m_exception;
    uint32_t    m_rs;
    uint32_t    m_rt;
    uint32_t    m_next_pc;
    bool m_hazard;

    config_t m_config;

    const bool m_little_endian;

public:
    MipsIss(uint32_t ident, bool little_endian);

    void dump() const;
    void step();

    inline void nullStep( uint32_t time_passed = 1 )
    {
        if ( m_ins_delay ) {
            if ( m_ins_delay > time_passed )
                m_ins_delay -= time_passed;
            else
                m_ins_delay = 0;
        }
        m_hazard = false;
        r_count += time_passed;
    }

    inline uint32_t isBusy()
    {
        return m_ins_delay;
    }

	inline void getInstructionRequest(bool &req, uint32_t &address) const
	{
        req = true;
		address = r_pc;
	}

	virtual inline void getDataRequest(
        bool &valid,
        enum DataAccessType &type,
        uint32_t &address,
        uint32_t &wdata) const
	{
        valid = r_mem_req;
		address = r_mem_addr;
		wdata = r_mem_wdata;
		type = r_mem_type;
	}

	inline void setWriteBerr()
	{
		m_dbe = true;
	}

	inline void setIrq(uint32_t irq)
	{
		m_irq = irq;
	}

    void reset();

	virtual inline void setInstruction(bool error, uint32_t val)
	{
		m_ibe = error;
		m_ins.ins = val;
	}

    void setDataResponse(bool error, uint32_t rdata);

    // processor internal registers access API, used by
    // debugger. Mips order is 32 general-purpose; sr; lo; hi; bad; cause; pc;

    inline unsigned int getDebugRegisterCount() const
    {
        return 32 + 6;
    }

    virtual uint32_t getDebugRegisterValue(unsigned int reg) const;

    inline size_t getDebugRegisterSize(unsigned int reg) const
    {
        return 32;
    }

    virtual void setDebugRegisterValue(unsigned int reg, uint32_t value);

    inline uint32_t getDebugPC() const
    {
        return r_pc;
    }

    inline void setDebugPC(uint32_t pc)
    {
        r_pc = pc;
        r_npc = pc+4;
    }

    int cpuCauseToSignal( uint32_t cause ) const;

private:
    void run();

    inline void setInsDelay( uint32_t delay )
    {
        assert( delay > 0 );
        m_ins_delay = delay-1;
    }

    inline bool isInUserMode() const
    {
        return r_status.kuc;
    }

    inline bool isHighPC() const
    {
        return (uint32_t)r_pc & 0x80000000;
    }

    inline bool isPrivDataAddr( uint32_t addr ) const
    {
        return addr & 0x80000000;
    }

    typedef void (MipsIss::*func_t)();

    static func_t const opcod_table[64];
    static func_t const special_table[64];

    void do_load( uint32_t address, enum DataAccessType type,
                  bool unsigned_, int shift = 0 );
    void do_store( uint32_t address, enum DataAccessType type, uint32_t data );

    void op_special();
    void op_bcond();
    void op_j();
    void op_jal();
    void op_beq();
    void op_bne();
    void op_blez();
    void op_bgtz();
    void op_addi();
    void op_addiu();
    void op_slti();
    void op_sltiu();
    void op_andi();
    void op_ori();
    void op_xori();
    void op_lui();
    void op_copro();
    void op_ill();
    void op_lb();
    void op_lh();
    void op_ll();
    void op_lw();
    void op_lwl();
    void op_lwr();
    void op_lbu();
    void op_lhu();
    void op_sb();
    void op_sh();
    void op_sw();
    void op_swl();
    void op_swr();
    void op_sc();

    void special_sll();
    void special_srl();
    void special_sra();
    void special_sllv();
    void special_srlv();
    void special_srav();
    void special_jr();
    void special_jalr();
    void special_sysc();
    void special_brek();
    void special_mfhi();
    void special_movn();
    void special_movz();
    void special_mthi();
    void special_mflo();
    void special_mtlo();
    void special_mult();
    void special_multu();
    void special_div();
    void special_divu();
    void special_add();
    void special_addu();
    void special_sub();
    void special_subu();
    void special_and();
    void special_or();
    void special_xor();
    void special_nor();
    void special_slt();
    void special_sltu();
    void special_ill();

    void special_tlt();
    void special_tltu();
    void special_tge();
    void special_tgeu();
    void special_teq();
    void special_tne();

    typedef enum {
        USE_NONE = 0,
        USE_T    = 1,
        USE_S    = 2,
        USE_ST   = 3,
        USE_SPECIAL = 4,
    } use_t;

    use_t curInstructionUsesRegs();
    bool  curInstructionIsBranch();

    static const char* name_table[64];
    static use_t const use_table[64];
    static use_t const use_special_table[64];

    static bool  const bd_table[64];
    static bool  const bd_special_table[64];

    uint32_t cp0Get( uint32_t reg ) const;
    void cp0Set( uint32_t reg, uint32_t value );

    // Make sure users dont try to instanciate MipsIss class
    virtual inline void please_use_MipsElIss_or_MipsEbIss() = 0;
};

class MipsElIss
    : public MipsIss
{
public:
    MipsElIss(uint32_t ident)
        : MipsIss(ident, true)
    {}
private:
    virtual inline void please_use_MipsElIss_or_MipsEbIss()
    {}
};

class MipsEbIss
    : public MipsIss
{
public:
    MipsEbIss(uint32_t ident)
        : MipsIss(ident, false)
    {}

	inline void getDataRequest(
        bool &valid,
        enum DataAccessType &type,
        uint32_t &address,
        uint32_t &wdata) const
	{
        uint32_t data;
        MipsIss::getDataRequest(valid, type, address, data);
        wdata = soclib::endian::uint32_swap(data);
	}

	inline void setInstruction(bool error, uint32_t val)
	{
        MipsIss::setInstruction(error, soclib::endian::uint32_swap(val));
	}

    void setDataResponse(bool error, uint32_t rdata);

    uint32_t getDebugRegisterValue(unsigned int reg) const
    {
        return soclib::endian::uint32_swap(
            MipsIss::getDebugRegisterValue(reg));
    }

    void setDebugRegisterValue(unsigned int reg, uint32_t value)
    {
        MipsIss::setDebugRegisterValue(reg,soclib::endian::uint32_swap(value));
    }

private:
    virtual inline void please_use_MipsElIss_or_MipsEbIss()
    {}
};

}}

#endif // _SOCLIB_MIPS_ISS_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

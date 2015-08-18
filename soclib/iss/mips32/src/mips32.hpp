#ifndef MIPS32_HPP
#define MIPS32_HPP

#include "mips32.h"
#include "arithmetics.h"

namespace soclib { namespace common {

void Mips32Iss::jump(addr_t dest, bool now)
{
	if ( dest & 3 ) {
		m_exception = X_ADEL;
		m_error_addr = dest;
		return;
	}
	if ( now ) {
		m_next_pc = dest;
		m_jump_pc = m_next_pc+4;
	} else {
		m_jump_pc = dest;
	}
}

void Mips32Iss::jump_imm16(bool taken, bool likely)
{
	if ( likely ) {
		if ( taken ) {
			jump(sign_ext(m_ins.i.imd, 16)*4 + r_npc, false);
		} else {
			jump(r_npc+4, true);
		}
	} else {
		if ( taken )
			jump(sign_ext(m_ins.i.imd, 16)*4 + r_npc, false);
	}
}

bool Mips32Iss::check_irq_state() const
{
    return (m_microcode_func == NULL)
        && r_status.ie
        && !r_status.exl
        && !r_status.erl;
}



}}

#endif

/*\
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
 * 01/10/2011
 * Frédéric Pétrot <Frederic.Petrot@imag.fr>, TIMA Lab, CNRS/Grenoble-INP/UJF
\*/

#include "microblaze.h"
#include "microblaze.hpp"

//TODO: moreorless everything, ...

namespace soclib { namespace common {
		
void MicroblazeIss::insn_fadd()
{
   insn_unimpl();
}

void MicroblazeIss::insn_frsub()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fmul()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fidv()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fcmpun()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fcmplt()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fcmpeq()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fcmple()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fcmpgt()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fcmpne()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fcmpge()
{
   insn_unimpl();
}

void MicroblazeIss::insn_flt()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fint()
{
   insn_unimpl();
}

void MicroblazeIss::insn_fsqrt()
{
   insn_unimpl();
}

MicroblazeIss::func_t const MicroblazeIss::float_table[]= {
   op4(   fadd,    ill,    ill,    ill),
   op4(    ill,    ill,    ill,    ill),
                                  
   op4(   frsub,   ill,    ill,    ill),
   op4(     ill,   ill,    ill,    ill),
                                  
   op4(   fmul,    ill,    ill,    ill),
   op4(    ill,    ill,    ill,    ill),
                                  
   op4(   fidv,    ill,    ill,    ill),
   op4(    ill,    ill,    ill,    ill),

   op4( fcmpun, fcmplt, fcmpeq, fcmple),
   op4( fcmpgt, fcmpne, fcmpge,    ill),

   op4(    flt,   ill,     ill,    ill),
   op4(    ill,   ill,     ill,    ill),
                                  
   op4(   fint,   ill,     ill,    ill),
   op4(    ill,   ill,     ill,    ill),
                                  
   op4(  fsqrt,   ill,     ill,    ill),
   op4(    ill,   ill,     ill,    ill),
};


void MicroblazeIss::insn_float()
{
	func_t func = float_table[m_ins.typeFLOAT.fc];
	(this->*func)();
}

}}

// vim: filetype=cpp:expandtab:shiftwidth=3:tabstop=3:softtabstop=3

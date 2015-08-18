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
 * Copyright (c) UPMC, Lip6, Asim
 *         Alain Greiner <alain.greiner@lip6.fr>, 2012
 *
 * Maintainers: alain
 */

#ifndef NOC_MMU_H
#define NOC_MMU_H

enum SoclibNocMmuRegisters 
{
    NOC_MMU_PTPR    = 0,        // R/W  : Page Table Pointer Register
    NOC_MMU_MODE    = 1,        // R/W  : NOC_MMU mode register
    NOC_MMU_BVAR    = 2,        // R    : Bad Virtual Address
    NOC_MMU_XCODE   = 3,        // R    : Error type
    NOC_MMU_INVAL   = 4,        // W    : Invalidate PTE / Virtual Address
    //
    NOC_MMU_VM_SPAN = 8,        // 32 bytes per VM
};

#endif 

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

 

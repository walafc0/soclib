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
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo
 */

#ifndef XICU_REGS_H
#define XICU_REGS_H

enum SoclibXicuFunc {
    XICU_WTI_REG         = 0,
    XICU_PTI_PER         = 1,
    XICU_PTI_VAL         = 2,
    XICU_PTI_ACK         = 3,

    XICU_MSK_PTI         = 4,
    XICU_MSK_PTI_ENABLE  = 5,
    XICU_MSK_PTI_DISABLE = 6,
    XICU_PTI_ACTIVE      = 6,

    XICU_MSK_HWI         = 8,
    XICU_MSK_HWI_ENABLE  = 9,
    XICU_MSK_HWI_DISABLE = 10,
    XICU_HWI_ACTIVE      = 10,

    XICU_MSK_WTI         = 12,
    XICU_MSK_WTI_ENABLE  = 13,
    XICU_MSK_WTI_DISABLE = 14,
    XICU_WTI_ACTIVE      = 14,

    XICU_PRIO            = 15,

    XICU_CONFIG          = 16,
};

#define XICU_REG(func, idx) (((func)<<5)|(idx))

#endif /* XICU_REGS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


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
 * Copyright (c) Telecom ParisTech
 *         Alexandre Becoulet <alexandre.becoulet@telecom-paristech.fr>, 2011
 *
 */
#ifndef RTTIMER_REGS_H
#define RTTIMER_REGS_H

enum SoclibRtTimerRegisters {
    SOCLIB_RTTIMER_SCCNT    = 0,
    SOCLIB_RTTIMER_SCRLD    = 1,
    SOCLIB_RTTIMER_CFG      = 2,
    SOCLIB_RTTIMER_CTRL     = 3,
    SOCLIB_RTTIMER_RTCL     = 4,
    SOCLIB_RTTIMER_RTCH     = 5,
    SOCLIB_RTTIMER_RTCTMP   = 6,
    //                        7,
    SOCLIB_RTTIMER_PE       = 8,
    SOCLIB_RTTIMER_IE       = 9,
    SOCLIB_RTTIMER_IP       = 10,
    SOCLIB_RTTIMER_COPY     = 11,
    SOCLIB_RTTIMER_CANCEL   = 12,

    SOCLIB_RTTIMER_DLN1     = 32,
    SOCLIB_RTTIMER_DLN1S    = 33,
    SOCLIB_RTTIMER_DLN1A    = 34,
    SOCLIB_RTTIMER_DLN1P    = 35,
};

enum SoclibRtTimerCtrl {
    SOCLIB_RTTIMER_CTRL_CE  = 1,
    SOCLIB_RTTIMER_CTRL_IEW = 2,
    SOCLIB_RTTIMER_CTRL_IES = 4,
    SOCLIB_RTTIMER_CTRL_IEA = 8,
    SOCLIB_RTTIMER_CTRL_IEC = 16,
};

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


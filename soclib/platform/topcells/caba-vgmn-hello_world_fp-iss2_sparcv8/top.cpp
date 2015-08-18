/*
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
 * Maintainers: ALEXIS
 */

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "iss2_simhelper.h"
#include "sparcv8.h"
#include "vci_xcache_wrapper.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_vgmn.h"

#include "segmentation.h"

int _main(int argc, char *argv[])
{
        using namespace sc_core;
        // Avoid repeating these everywhere
        using soclib::common::IntTab;
        using soclib::common::Segment;

        // Define our VCI parameters
        typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

        // Mapping table
        soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x20000000);

        maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
        maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
        maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));

        // Signals
        sc_clock        signal_clk("signal_clk");
        sc_signal<bool> signal_resetn("signal_resetn");

        sc_signal<bool> signal_sparc0_it0("signal_sparc0_it0");
        sc_signal<bool> signal_sparc0_it1("signal_sparc0_it1");
        sc_signal<bool> signal_sparc0_it2("signal_sparc0_it2");
        sc_signal<bool> signal_sparc0_it3("signal_sparc0_it3");

        soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");
        soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
        soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
        soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");

        sc_signal<bool> signal_tty_irq0("signal_tty_irq0");
        sc_signal<bool> signal_tty_irq1("signal_tty_irq1");
        sc_signal<bool> signal_tty_irq2("signal_tty_irq2");
        sc_signal<bool> signal_tty_irq3("signal_tty_irq3");

        // Components
        soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Sparcv8Iss<8> > >
          sparc0("sparc", 0, maptab,IntTab(0), 4,1,8, 4,1,8);
        soclib::common::Loader loader("soft/bin.soft");
        soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
        soclib::caba::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);
        soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",        IntTab(2), maptab, "vcitty0", NULL);

        soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 1, 3, 2, 8);

        // Net-List
        sparc0.p_clk(signal_clk);
        sparc0.p_resetn(signal_resetn);
        sparc0.p_irq[0](signal_sparc0_it0);
        sparc0.p_irq[1](signal_sparc0_it1);
        sparc0.p_irq[2](signal_sparc0_it2);
        sparc0.p_irq[3](signal_sparc0_it3);
        sparc0.p_vci(signal_vci_m0);

        vcimultiram0.p_clk(signal_clk);
        vcimultiram0.p_resetn(signal_resetn);
        vcimultiram0.p_vci(signal_vci_vcimultiram0);

        vcimultiram1.p_clk(signal_clk);
        vcimultiram1.p_resetn(signal_resetn);
        vcimultiram1.p_vci(signal_vci_vcimultiram1);

        vcitty.p_clk(signal_clk);
        vcitty.p_resetn(signal_resetn);
        vcitty.p_vci(signal_vci_tty);
        vcitty.p_irq[0](signal_sparc0_it0);
        
        vgmn.p_clk(signal_clk);
        vgmn.p_resetn(signal_resetn);
        vgmn.p_to_initiator[0](signal_vci_m0);
        vgmn.p_to_target[0](signal_vci_vcimultiram0);
        vgmn.p_to_target[1](signal_vci_vcimultiram1);
        vgmn.p_to_target[2](signal_vci_tty);

        sc_start(sc_core::sc_time(0, SC_NS));
        signal_resetn = false;
        sc_start(sc_core::sc_time(1, SC_NS));
        signal_resetn = true;

#ifdef SOCVIEW
        debug();
#else
        sc_start();
#endif
        return EXIT_SUCCESS;
}

int sc_main (int argc, char *argv[])
{
  return _main(argc, argv);
}

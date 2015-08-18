#include <iostream>
#include <systemc>
#include <tlmt>
#include "vci_ports.h"
#include "mapping_table.h"
#include "vci_vgmn.h"
#include "vci_simple_initiator.h"
#include "vci_simple_target.h"
#include "vci_ram.h"

int sc_main(int argc, char **argv)
{
	typedef soclib::tlmt::VciParams<uint32_t,uint32_t,4> vci_param;

	/////////////////////////////////////////////////////////////////////////////
	// SOFT 
	/////////////////////////////////////////////////////////////////////////////
	soclib::common::Loader loader("soft/bin.soft");
  
	/////////////////////////////////////////////////////////////////////////////
	// MAPPING TABLE 
	/////////////////////////////////////////////////////////////////////////////
	soclib::common::MappingTable maptab(32, soclib::common::IntTab(8), soclib::common::IntTab(8), 0x00200000);

	maptab.add(soclib::common::Segment("boot",  0xbfc00000,       2048, soclib::common::IntTab(1), 1));
	maptab.add(soclib::common::Segment("cram0", 0x10000000, 0x00100000, soclib::common::IntTab(0), 1));
	maptab.add(soclib::common::Segment("cram1", 0x20000000, 0x00100000, soclib::common::IntTab(1), 1));
	maptab.add(soclib::common::Segment("excep", 0x80000080,       2048, soclib::common::IntTab(1), 1));
	maptab.add(soclib::common::Segment("tty0",  0x90200000,         32, soclib::common::IntTab(2), 0));
	maptab.add(soclib::common::Segment("uram0", 0x10200000, 0x00100000, soclib::common::IntTab(0), 0));
	maptab.add(soclib::common::Segment("uram1", 0x20200000, 0x00100000, soclib::common::IntTab(1), 0));
	
	/////////////////////////////////////////////////////////////////////////////
	// VCI_VGMN 
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmt::VciVgmn<vci_param> vgmn(1,1,maptab,10);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_SIMPLE_INITIATOR 
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmt::VciSimpleInitiator<vci_param> i("init");
	i.p_vci(vgmn.m_RspArbCmdRout[0]->p_vci);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_RAM
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmt::VciRam<vci_param> ram("ram", 0, soclib::common::IntTab(0), maptab, loader);
	vgmn.m_CmdArbRspRout[0]->p_vci(ram.p_vci);

	/////////////////////////////////////////////////////////////////////////////
	// START
	/////////////////////////////////////////////////////////////////////////////
	sc_core::sc_start();
	return 0;
}


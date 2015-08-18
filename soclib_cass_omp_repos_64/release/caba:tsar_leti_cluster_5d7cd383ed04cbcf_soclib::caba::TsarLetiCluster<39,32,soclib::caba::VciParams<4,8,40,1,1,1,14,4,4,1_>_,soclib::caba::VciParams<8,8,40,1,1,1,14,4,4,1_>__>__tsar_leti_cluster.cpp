#include "tsar_leti_cluster.h"
#include "dspin_router.h"
#include "vci_buffers.h"
#include "vci_dspin_initiator_wrapper.h"
#include "vci_dspin_target_wrapper.h"
#include "generic_tlb.h"
#include "vci_block_device_tsar.h"
#include "vci_xicu.h"
#include "fd_poller.h"
#include "dspin_interface.h"
#include "block_device_tsar.h"
#include "xterm_wrapper.h"
#include "mem_cache_directory.h"
#include "vci_cc_vcache_wrapper.h"
#include "dspin_local_crossbar.h"
#include "mapping_table.h"
#include "segment.h"
#include "multi_write_buffer.h"
#include "elfpp.hh"
#include "binary_file_section.h"
#include "vci_initiator.h"
#include "exception.h"
#include "xicu.h"
#include "vci_local_crossbar.h"
#include "dpp_placeholder.h"
#include "generic_fifo.h"
#include "generic_cache.h"
#include "mips32.h"
#include "vci_mem_cache.h"
#include "binary_file_symbol.h"
#include "pts_wrapper.h"
#include "gdbserver.h"
#include "iss2.h"
#include "generic_llsc_global_table.h"
#include "vci_target_fsm.h"
#include "mem_cache.h"
#include "dspin_dhccp_param.h"
#include "vci_multi_tty.h"
#include "base_module.h"
#include "process_wrapper.h"
#include "vci_target.h"
#include "linked_access_buffer.h"
#include "xram_transaction.h"
#include "update_tab.h"
#include "vci_param.h"
#include "tty.h"
#include "address_decoding_table.h"
#include "int_tab.h"
#include "address_masking_table.h"
#include "caba_base_module.h"
#include "vci_simple_ram.h"
#include "tty_wrapper.h"
#include "loader.h"
#include "/users/cao/peneau/workspace/tsar/platforms/tsar_generic_leti/tsar_leti_cluster/caba/source/src/tsar_leti_cluster.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::TsarLetiCluster<39,32,soclib::caba::VciParams<4,8,40,1,1,1,14,4,4,1 > ,soclib::caba::VciParams<8,8,40,1,1,1,14,4,4,1 >  > ;
#endif
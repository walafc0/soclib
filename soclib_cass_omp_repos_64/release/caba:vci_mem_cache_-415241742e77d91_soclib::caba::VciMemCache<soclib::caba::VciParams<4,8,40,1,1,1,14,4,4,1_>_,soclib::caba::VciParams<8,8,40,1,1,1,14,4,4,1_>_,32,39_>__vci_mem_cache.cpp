#include "dspin_dhccp_param.h"
#include "int_tab.h"
#include "dspin_interface.h"
#include "generic_fifo.h"
#include "mapping_table.h"
#include "segment.h"
#include "binary_file_section.h"
#include "vci_initiator.h"
#include "exception.h"
#include "mem_cache_directory.h"
#include "vci_mem_cache.h"
#include "binary_file_symbol.h"
#include "generic_llsc_global_table.h"
#include "mem_cache.h"
#include "caba_base_module.h"
#include "vci_target.h"
#include "xram_transaction.h"
#include "update_tab.h"
#include "vci_param.h"
#include "address_decoding_table.h"
#include "address_masking_table.h"
#include "base_module.h"
#include "loader.h"
#include "/users/cao/peneau/workspace/tsar/modules/vci_mem_cache/caba/source/src/vci_mem_cache.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::VciMemCache<soclib::caba::VciParams<4,8,40,1,1,1,14,4,4,1 > ,soclib::caba::VciParams<8,8,40,1,1,1,14,4,4,1 > ,32,39 > ;
#endif

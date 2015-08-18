#include "binary_file_symbol.h"
#include "binary_file_section.h"
#include "mapping_table.h"
#include "address_masking_table.h"
#include "segment.h"
#include "vci_param.h"
#include "address_decoding_table.h"
#include "exception.h"
#include "vci_simple_ram.h"
#include "int_tab.h"
#include "base_module.h"
#include "caba_base_module.h"
#include "vci_target.h"
#include "linked_access_buffer.h"
#include "loader.h"
#include "/users/cao/peneau/workspace/soclib/soclib/module/internal_component/vci_simple_ram/caba/source/src/vci_simple_ram.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::VciSimpleRam<soclib::caba::VciParams<8,8,40,1,1,1,14,4,4,1 >  > ;
#endif

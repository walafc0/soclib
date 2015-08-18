#include "vci_buffers.h"
#include "mapping_table.h"
#include "dpp_placeholder.h"
#include "vci_param.h"
#include "address_decoding_table.h"
#include "exception.h"
#include "vci_target_fsm.h"
#include "address_masking_table.h"
#include "int_tab.h"
#include "base_module.h"
#include "caba_base_module.h"
#include "segment.h"
#include "linked_access_buffer.h"
#include "/users/cao/peneau/workspace/soclib/soclib/lib/vci_target_fsm/src/caba/vci/vci_target_fsm.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::VciTargetFsm<soclib::caba::VciParams<4,8,40,1,1,1,14,4,4,1 > ,true,false > ;
#endif

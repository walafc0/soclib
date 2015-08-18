#include "vci_buffers.h"
#include "mapping_table.h"
#include "dpp_placeholder.h"
#include "segment.h"
#include "vci_param.h"
#include "address_decoding_table.h"
#include "vci_initiator.h"
#include "exception.h"
#include "vci_local_crossbar.h"
#include "address_masking_table.h"
#include "int_tab.h"
#include "base_module.h"
#include "caba_base_module.h"
#include "vci_target.h"
#include "/users/cao/peneau/workspace/soclib/soclib/module/network_component/vci_local_crossbar/caba/source/src/vci_local_crossbar.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::VciLocalCrossbar<soclib::caba::VciParams<4,8,40,1,1,1,14,4,4,1 >  > ;
#endif

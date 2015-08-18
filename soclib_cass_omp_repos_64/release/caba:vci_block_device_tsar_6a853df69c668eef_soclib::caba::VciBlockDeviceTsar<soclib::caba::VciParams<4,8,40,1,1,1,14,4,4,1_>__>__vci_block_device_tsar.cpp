#include "mapping_table.h"
#include "segment.h"
#include "vci_param.h"
#include "address_decoding_table.h"
#include "vci_initiator.h"
#include "exception.h"
#include "address_masking_table.h"
#include "vci_block_device_tsar.h"
#include "int_tab.h"
#include "base_module.h"
#include "block_device_tsar.h"
#include "caba_base_module.h"
#include "vci_target.h"
#include "/users/cao/peneau/workspace/tsar/modules/vci_block_device_tsar/caba/source/src/vci_block_device_tsar.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::VciBlockDeviceTsar<soclib::caba::VciParams<4,8,40,1,1,1,14,4,4,1 >  > ;
#endif

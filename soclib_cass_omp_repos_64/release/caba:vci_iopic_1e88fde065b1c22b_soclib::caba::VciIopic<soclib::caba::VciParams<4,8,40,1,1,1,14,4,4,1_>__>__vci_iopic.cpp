#include "mapping_table.h"
#include "segment.h"
#include "vci_param.h"
#include "address_decoding_table.h"
#include "vci_initiator.h"
#include "exception.h"
#include "address_masking_table.h"
#include "int_tab.h"
#include "vci_iopic.h"
#include "base_module.h"
#include "caba_base_module.h"
#include "vci_target.h"
#include "/users/cao/peneau/workspace/tsar/platforms/tsar_generic_leti/vci_iopic/caba/source/src/vci_iopic.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::VciIopic<soclib::caba::VciParams<4,8,40,1,1,1,14,4,4,1 >  > ;
#endif

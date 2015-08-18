#include "mapping_table.h"
#include "segment.h"
#include "vci_param.h"
#include "chbuf_dma.h"
#include "address_decoding_table.h"
#include "vci_initiator.h"
#include "exception.h"
#include "address_masking_table.h"
#include "caba_base_module.h"
#include "vci_chbuf_dma.h"
#include "base_module.h"
#include "int_tab.h"
#include "vci_target.h"
#include "/users/cao/peneau/workspace/soclib/soclib/module/infrastructure_component/dma_infrastructure/vci_chbuf_dma/caba/source/src/vci_chbuf_dma.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::VciChbufDma<soclib::caba::VciParams<4,8,40,1,1,1,14,4,4,1 >  > ;
#endif

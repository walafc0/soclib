#include "fifo_multi_buffer.h"
#include "nic_rx_tap.h"
#include "int_tab.h"
#include "nic_rx_backend.h"
#include "generic_fifo.h"
#include "mapping_table.h"
#include "vci_target.h"
#include "exception.h"
#include "nic_tx_chbuf.h"
#include "nic_tx_gmii.h"
#include "nic_tx_backend.h"
#include "nic_rx_gmii.h"
#include "vci_multi_nic.h"
#include "caba_base_module.h"
#include "segment.h"
#include "multi_nic.h"
#include "ethernet_crc.h"
#include "nic_rx_chbuf.h"
#include "vci_param.h"
#include "address_decoding_table.h"
#include "nic_tx_tap.h"
#include "address_masking_table.h"
#include "base_module.h"
#include "/users/cao/peneau/workspace/soclib/soclib/module/connectivity_component/vci_multi_nic/caba/source/src/vci_multi_nic.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::VciMultiNic<soclib::caba::VciParams<4,8,40,1,1,1,14,4,4,1 >  > ;
#endif

#include "generic_fifo.h"
#include "mapping_table.h"
#include "address_decoding_table.h"
#include "exception.h"
#include "address_masking_table.h"
#include "base_module.h"
#include "caba_base_module.h"
#include "dspin_interface.h"
#include "int_tab.h"
#include "dspin_local_crossbar.h"
#include "segment.h"
#include "/users/cao/peneau/workspace/soclib/soclib/module/network_component/dspin_local_crossbar/caba/source/src/dspin_local_crossbar.cpp"
#ifndef ENABLE_SCPARSE
template class soclib::caba::DspinLocalCrossbar<32 > ;
#endif

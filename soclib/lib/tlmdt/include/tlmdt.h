#ifndef __TLMDT_H__
#define __TLMDT_H__

#include <systemc>
#include <limits>
#include "vci_param.h"                         // VCI parameters header

//convert unsigned char[4] to an unsigned integer
#define atou(data, idx)	((data[idx]) + (data[idx + 1] << 8) + (data[idx + 2] << 16) + (data[idx + 3] << 24))  

//convert an unsigned integer to a unsigned char 
#define utoa(num,data,idx) ( data[idx]     = num,	\
			     data[idx + 1] = num >> 8,  \
			     data[idx + 2] = num >> 16, \
			     data[idx + 3] = num >> 24  )


const sc_core::sc_time UNIT_TIME = sc_core::sc_time(1,sc_core::SC_PS);
const uint64_t MAX_TIME  = std::numeric_limits<uint64_t>::max();

#define MAXIMUM_PACKET_SIZE 100 //100 words

#include "soclib_payload_extension.h"                  // PAYLOAD EXTENSION
#include "pdes_local_time.h"                           // PDES LOCAL TIME
#include "pdes_activity_status.h"                      // PDES ACTIVITY STATUS

#endif /* __TLMDT_H__ */

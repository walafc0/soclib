/* *****************************************************************************
                                    BSD LICENSE
********************************************************************************
Copyright (c) 2006, INRIA
Authors: Zheng LI (zheng.x.li@inria.fr)

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 - Neither the name of the INRIA nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************** */

#ifndef _ST231_SIMOSCALL_H_
#define _ST231_SIMOSCALL_H_

#include "../xst_simoscall/xst_simoscall.h"

namespace st231{
xiss_bool_t simoscall_mem_read(void *sd,
                                   xiss_addr_t address,
                                   xiss_size_t size,
                                   xiss_byte_t *buffer);

xiss_bool_t simoscall_mem_write(void *sd,
                                    xiss_addr_t address,
                                    xiss_size_t size,
                                    const xiss_byte_t *buffer);

xiss_bool_t simoscall_reg_read(void * desc,
				    xiss_uint32_t slice_id,
				    xiss_uint32_t thread_id,
				    xiss_uint32_t reg_no,
				    xiss_uint32_t *value);

xiss_bool_t simoscall_reg_write(void * desc,
				    xiss_uint32_t slice_id,
				    xiss_uint32_t thread_id,
				    xiss_uint32_t reg_no,
				    xiss_uint32_t value);

xiss_bool_t simoscall_exit(void * desc,
				xiss_uint32_t slice_id,
				xiss_uint32_t thread_id,
				xiss_int32_t status);

}
#endif


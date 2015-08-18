/*
 *  Copyright (c) 2008,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __ST231_MEMORY_OP_HH__
#define __ST231_MEMORY_OP_HH__

#include <inttypes.h>

namespace st231
{

class MemoryOp
{public:
  typedef uint32_t address_t;
  typedef enum
  { READ,
    WRITE,
    PREFETCH
  } type_t;

  /**
   * \brief Build a READ memory op
   */
  MemoryOp(address_t address, uint32_t size, uint32_t &_dest, int32_t &_counter, bool aligned, bool read_signed) : dest(_dest) , counter(_counter) 
  { type = READ;
    this->address = address;
    this->size = size;
    target_reg = dest;
    this->read_signed = read_signed;
    this->aligned = aligned;
  }

  /**
   * \brief Build a WRITE memory op
   */
  MemoryOp(address_t address, uint32_t size, uint32_t value, int32_t &_counter) : dest(dest_none), counter(_counter)
  { type = WRITE;
    this->address = address;
    this->size = size;
    write_value = value;
  }

  /**
   * \brief Build a PREFETCH memory op
   */
  MemoryOp(address_t address) : dest(dest_none), counter(counter_none)
  { type = PREFETCH;
    this->address = address;
  }

  ~MemoryOp() {}

  type_t GetType() const
  {  return type;
  }

  address_t GetAddress() const
  { return address;
  }

  uint32_t GetSize() const
  { return size;
  }

  uint32_t GetTargetReg() const
  { return target_reg;
  }

  uint32_t GetWriteValue() const
  { return write_value;
  }

  bool NeedAlignment() const
  { return !aligned;
  }

  bool IsSigned() const
  { return read_signed;
  }

 private:
  address_t address;
  type_t type;
  uint32_t size;
  uint32_t target_reg;
  uint32_t write_value;
  bool read_signed;
  bool aligned;
  uint32_t dest_none;
  int32_t counter_none;

 public:
  uint32_t &dest;
  int32_t &counter;
};

} // end of namespace st231

#endif

/* *****************************************************************************
                                    BSD LICENSE
********************************************************************************
Copyright (c) 2006, INRIA
Authors: Sylvain Girbal

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

#ifndef __ST231_BUNDLE_HH__
#define __ST231_BUNDLE_HH__

namespace st231{

using namespace std;

class CPU_tlmt;

class Bundle
{public:
  static const int OP_PER_BUNDLE = 4;
  typedef uint32_t address_t;

  // constructor and destructor
  Bundle(address_t _addr, CPU_tlmt *_cpu);

  //Bundle operations
  void Fetch();
  void Decode();
  void Execute();

  bool fetched();
  bool decoded();
  bool executed();

  void dump_fetch(ostream &os);
  void dump_decode(ostream &os);
  void dump_execute(ostream &os);
  friend ostream & operator<<(ostream &os, const Bundle &b);
 
 private:
  //properties
  uint32_t opcodes[OP_PER_BUNDLE];         // Filled during the VLIW fetch
  Operation* operations[OP_PER_BUNDLE];    // Filled during the VLIW decode
  uint32_t imms[OP_PER_BUNDLE];
  int32_t num_fetched;
  int32_t num_decoded;
  bool is_decoded;
  bool is_executed;
  address_t addr;
  CPU_tlmt *cpu;
};

}

#endif

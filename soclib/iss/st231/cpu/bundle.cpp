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

#include <stdint.h>
#include <st231_isa.hh>
#include <cpu_tlmt.hh>
#include <bundle.hh>
#include <soclib_memory_op.hh>
#include <queue>
#include <iomanip>

namespace st231{

using namespace std;
typedef uint32_t address_t;

Bundle::Bundle(address_t _addr, CPU_tlmt *_cpu)
{ num_fetched=0;
  num_decoded=0;
  is_decoded = false;
  is_executed = false;
  addr = _addr;
  cpu = _cpu;
  for(int i=0; i<OP_PER_BUNDLE; i++)
  { opcodes[i] = 0;
    operations[i] = NULL;
    imms[i] = 0;
  }
}

void Bundle::Fetch()
{ address_t fetch_addr = addr;
  for(int i=0; i<OP_PER_BUNDLE; i++)
  { cpu->FetchSubOp(fetch_addr, opcodes[i], num_fetched);
    fetch_addr += 4;
  }
}

void Bundle::Decode()
{ cpu->SetNia(addr);
  if(addr!=cpu->GetCia())
  { cerr << "unmatched fetch and execute !" << endl;
  }
  is_decoded = true;
  for(int i=0; i<OP_PER_BUNDLE; i++)
  { operations[i] = cpu->Decode(addr+i*4,opcodes[i]);
    if(IMM_SEL_MASK(opcodes[i])==IMML_MASK)
    {  imms[i-1] = IMM(opcodes[i]);
    }
    else if(IMM_SEL_MASK(opcodes[i])==IMMR_MASK)
    {  imms[i+1] = IMM(opcodes[i]);
    }
    num_decoded++;
    cpu->SetNia(cpu->GetNia()+4);
    if(opcodes[i] & STOP_BIT)
    { return;
    }
  }
  cerr << "unterminated bundle" << endl;
  exit(-1);
}

void Bundle::Execute()
{ for(int i=0; i<num_decoded; i++)
  { cpu->setImm(imms[i]);
    cpu->current_operation = operations[i];

//    cerr << "before execute: cpu.cia: " << hex << cpu->GetCia() << dec << endl;
//    cerr << "before execute: cpu.nia: " << hex << cpu->GetNia() << dec << endl;
  
    operations[i]->execute(cpu);

//    operations[i]->disasm(cpu,cerr);
//    cerr << endl;

//    cerr << "after execute: cpu.cia: " << hex << cpu->GetCia() << dec << endl;
//    cerr << "after execute: cpu.nia: " << hex << cpu->GetNia() << dec << endl;

  }

  // The bundle is fully executed swap the register banks
  cpu->bundle_commit();
  is_executed = true;
}

/**
 * \brief Returns true if the bundle is fully fetched
 */ 
bool Bundle::fetched()
{ if(num_fetched<4) return false;
  if(num_fetched==4) return true;
  cerr << "fetched too much instructions" << endl;
  exit(-1);
}

/**
 * \brief Returns true if the bundle is decoded.
 *
 * Note that num_decoded <= num_fetched as an instruction can be stop_bit enabled.
 */
bool Bundle::decoded()
{ return is_decoded;
}

/**
 * \brief Returns true if the bundle was executed
 */
bool Bundle::executed()
{ return is_executed;
}

void Bundle::dump_fetch(ostream &os)
{ os << "@" << hex << addr << dec << " ";
  os << "(" << num_fetched << "/" << OP_PER_BUNDLE << ") [ " << hex << setfill('0');
  for(int i=0; i<num_fetched; i++)
  { os << setw(8) << right << opcodes[i] << " ";
  }
  os << "]" << dec << setfill(' ');
}

void Bundle::dump_decode(ostream &os)
{ os << "@" << hex << addr << dec << " ";
  os << "(" << num_decoded << ") [ ";
  for(int i=0; i<num_decoded; i++)
  { operations[i]->disasm(cpu,os);
    os << " ; ";
  }
  os << "]";
}

void Bundle::dump_execute(ostream &os)
{ dump_decode(os);
}

/**
 * \brief Pretty printer
 */ 
ostream & operator<<(ostream &os, const Bundle &b)
{ os << "@" << hex << b.addr << dec << endl;
  os << "(" << b.num_fetched << "/" << b.OP_PER_BUNDLE << ") [ " << hex << setfill('0');
  for(int i=0; i<b.num_fetched; i++)
  { os << setw(8) << right << b.opcodes[i] << " ";
  }
  os << "]" << dec << setfill(' ') << endl;
  os << "imms:       (" << b.num_decoded << ") [ ";
  for(int i=0; i<b.num_decoded; i++)
  { os << b.imms[i] << " ";
  }
  os << "]" << dec << setfill(' ') << endl;
  os << "operations: (" << b.num_decoded << ") [ ";
  for(int i=0; i<b.num_decoded; i++)
  { b.operations[i]->disasm(b.cpu,os);
    os << " ; ";
  }
  os << "]" << endl;
  return os;
}

} //namespace st231

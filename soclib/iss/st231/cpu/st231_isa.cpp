#include "cpu/st231_isa.hh"
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

namespace st231 { 
Operation::Operation(CodeType code, uint32_t addr, const char *name)
{
	this->encoding = code;
	this->addr = addr;
	this->name = name;
}
Operation::~Operation()
{
}

#line 52 "sim_loadstore.isa"
uint32_t
#line 21 "cpu/st231_isa.cpp"
Operation::loadstore_effective_address(
#line 52 "sim_loadstore.isa"
	CPU *
#line 25 "cpu/st231_isa.cpp"

#line 52 "sim_loadstore.isa"
	cpu
#line 29 "cpu/st231_isa.cpp"
)
{

#line 52 "sim_loadstore.isa"
{ 
  return cpu->GetEA();
}
#line 37 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 42 "cpu/st231_isa.cpp"
Operation::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 46 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 50 "cpu/st231_isa.cpp"
)
{

#line 23 "sim_loadstore.isa"
{  
  cerr << "ISS Error: action memory_acces_size not set for: ";
  disasm(cpu,cerr);
  cerr << endl;
  exit(1);
}
#line 61 "cpu/st231_isa.cpp"
}

#line 4 "sim_loadstore.isa"
int
#line 66 "cpu/st231_isa.cpp"
Operation::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 70 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 74 "cpu/st231_isa.cpp"
)
{

#line 4 "sim_loadstore.isa"
{ 
  cerr << "ISS Error: action loadstore_target_register not set for: ";
  disasm(cpu,cerr);
  cerr << endl;
  exit(1);
}
#line 85 "cpu/st231_isa.cpp"
}

void
Operation::initialize_function()
{

#line 28 "sim_functions.isa"
{
  function = FnInvalid;
}
#line 96 "cpu/st231_isa.cpp"
}

void
Operation::initialize_latencies()
{

#line 15 "sim_latencies.isa"
{
}
#line 106 "cpu/st231_isa.cpp"
}

void
Operation::initialize_operands()
{

#line 51 "sim_dependencies.isa"
{ 
  std::cerr << "ST231 ISS: action initialize_operands not defined for instruction `"\
            << GetName() <<  "' at 0x" << std::hex << GetAddr() << " with encoding 0x"\
            <<  GetEncoding() << std::dec << std::endl;
  noperands=0;
}
#line 120 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 125 "cpu/st231_isa.cpp"
Operation::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 129 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 133 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 138 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 142 "cpu/st231_isa.cpp"
)
{

#line 58 "isa/st231.isa"
{
	os << "?";
}
#line 150 "cpu/st231_isa.cpp"
}

#line 53 "isa/st231.isa"
void
#line 155 "cpu/st231_isa.cpp"
Operation::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 159 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 163 "cpu/st231_isa.cpp"
)
{

#line 53 "isa/st231.isa"
{
	printf("Unknown instruction\n");
	exit(-1);
}
#line 172 "cpu/st231_isa.cpp"
}
class OpPswclr : public Operation
{
public:
	OpPswclr(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t src2;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 227 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 231 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 235 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 241 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 245 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 249 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 254 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 258 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpPswset : public Operation
{
public:
	OpPswset(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t src2;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 331 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 335 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 339 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 345 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 349 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 353 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 358 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 362 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSync : public Operation
{
public:
	OpSync(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 434 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 438 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 442 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 448 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 452 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 456 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 461 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 465 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpPrginspg : public Operation
{
public:
	OpPrginspg(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 539 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 543 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 547 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 553 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 557 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 561 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 566 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 570 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpPrgset : public Operation
{
public:
	OpPrgset(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 644 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 648 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 652 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 658 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 662 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 666 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 671 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 675 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpPrgadd : public Operation
{
public:
	OpPrgadd(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 749 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 753 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 757 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 763 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 767 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 771 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 776 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 780 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpPft : public Operation
{
public:
	OpPft(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 854 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 858 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 862 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 868 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 872 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 876 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 881 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 885 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpStb : public Operation
{
public:
	OpStb(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 960 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 964 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 968 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 974 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 978 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 982 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 987 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 991 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 1015 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 1019 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 1023 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 1032 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 1036 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 1040 "cpu/st231_isa.cpp"
	);
private:
};

class OpSth : public Operation
{
public:
	OpSth(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 1100 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 1104 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 1108 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 1114 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 1118 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 1122 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 1127 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 1131 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 1155 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 1159 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 1163 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 1172 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 1176 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 1180 "cpu/st231_isa.cpp"
	);
private:
};

class OpStw : public Operation
{
public:
	OpStw(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 1240 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 1244 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 1248 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 1254 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 1258 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 1262 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 1267 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 1271 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 1295 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 1299 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 1303 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 1312 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 1316 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 1320 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdbu_d : public Operation
{
public:
	OpLdbu_d(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 1380 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 1384 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 1388 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 1394 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 1398 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 1402 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 1407 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 1411 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 1435 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 1439 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 1443 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 1452 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 1456 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 1460 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdbu : public Operation
{
public:
	OpLdbu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 1520 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 1524 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 1528 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 1534 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 1538 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 1542 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 1547 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 1551 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 1575 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 1579 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 1583 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 1592 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 1596 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 1600 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdb_d : public Operation
{
public:
	OpLdb_d(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 1660 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 1664 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 1668 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 1674 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 1678 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 1682 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 1687 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 1691 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 1715 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 1719 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 1723 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 1732 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 1736 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 1740 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdb : public Operation
{
public:
	OpLdb(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 1800 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 1804 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 1808 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 1814 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 1818 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 1822 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 1827 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 1831 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 1855 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 1859 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 1863 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 1872 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 1876 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 1880 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdhu_d : public Operation
{
public:
	OpLdhu_d(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 1940 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 1944 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 1948 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 1954 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 1958 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 1962 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 1967 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 1971 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 1995 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 1999 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 2003 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 2012 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 2016 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 2020 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdhu : public Operation
{
public:
	OpLdhu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 2080 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 2084 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 2088 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 2094 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 2098 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 2102 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 2107 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 2111 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 2135 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 2139 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 2143 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 2152 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 2156 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 2160 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdh_d : public Operation
{
public:
	OpLdh_d(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 2220 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 2224 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 2228 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 2234 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 2238 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 2242 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 2247 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 2251 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 2275 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 2279 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 2283 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 2292 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 2296 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 2300 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdh : public Operation
{
public:
	OpLdh(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 2360 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 2364 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 2368 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 2374 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 2378 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 2382 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 2387 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 2391 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 2415 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 2419 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 2423 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 2432 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 2436 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 2440 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdw_d : public Operation
{
public:
	OpLdw_d(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 2500 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 2504 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 2508 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 2514 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 2518 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 2522 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 2527 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 2531 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 2555 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 2559 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 2563 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 2572 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 2576 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 2580 "cpu/st231_isa.cpp"
	);
private:
};

class OpLdw : public Operation
{
public:
	OpLdw(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t null;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 2640 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 2644 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 2648 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 2654 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 2658 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 2662 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 2667 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 2671 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
	/**
 * Returns the target register of the load instruction.
 */
	virtual

#line 4 "sim_loadstore.isa"
	int
#line 2695 "cpu/st231_isa.cpp"
	loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 2699 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 2703 "cpu/st231_isa.cpp"
	);
	/**
 * Returns the memory acces size in bytes of a load / store instruction
 */
	virtual

#line 23 "sim_loadstore.isa"
	int
#line 2712 "cpu/st231_isa.cpp"
	memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 2716 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 2720 "cpu/st231_isa.cpp"
	);
private:
};

class OpBreak : public Operation
{
public:
	OpBreak(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t sbrknum;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 2777 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 2781 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 2785 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 2791 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 2795 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 2799 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 2804 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 2808 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSyscall : public Operation
{
public:
	OpSyscall(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t sbrknum;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 2880 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 2884 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 2888 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 2894 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 2898 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 2902 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 2907 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 2911 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSbrk : public Operation
{
public:
	OpSbrk(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t sbrknum;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 2983 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 2987 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 2991 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 2997 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3001 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3005 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3010 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3014 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpPrgins : public Operation
{
public:
	OpPrgins(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t sbrknum;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 3086 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 3090 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 3094 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 3100 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3104 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3108 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3113 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3117 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIslctf : public Operation
{
public:
	OpIslctf(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t scond;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 3192 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 3196 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 3200 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 3206 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3210 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3214 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3219 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3223 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIslct : public Operation
{
public:
	OpIslct(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t scond;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 3298 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 3302 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 3306 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 3312 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3316 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3320 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3325 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3329 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImmr : public Operation
{
public:
	OpImmr(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t imm;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 3401 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 3405 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 3409 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 3415 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3419 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3423 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3428 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3432 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImml : public Operation
{
public:
	OpImml(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t imm;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 3504 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 3508 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 3512 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 3518 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3522 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3526 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3531 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3535 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpDivs : public Operation
{
public:
	OpDivs(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t scond;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 3611 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 3615 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 3619 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 3625 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3629 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3633 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3638 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3642 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpAddcg : public Operation
{
public:
	OpAddcg(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t scond;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 3718 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 3722 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 3726 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 3732 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3736 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3740 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3745 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3749 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSlctf : public Operation
{
public:
	OpSlctf(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t scond;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 3825 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 3829 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 3833 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 3839 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3843 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3847 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3852 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3856 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSlct : public Operation
{
public:
	OpSlct(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t scond;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 3932 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 3936 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 3940 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 3946 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 3950 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 3954 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 3959 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 3963 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBrf : public Operation
{
public:
	OpBrf(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bcond;
	uint32_t btarg;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4036 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4040 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4044 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4050 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4054 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4058 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4063 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4067 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBr : public Operation
{
public:
	OpBr(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bcond;
	uint32_t btarg;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4140 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4144 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4148 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4154 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4158 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4162 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4167 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4171 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpRfi : public Operation
{
public:
	OpRfi(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4242 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4246 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4250 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4256 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4260 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4264 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4269 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4273 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpGoto : public Operation
{
public:
	OpGoto(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4344 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4348 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4352 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4358 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4362 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4366 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4371 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4375 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIgoto : public Operation
{
public:
	OpIgoto(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t btarg;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4447 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4451 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4455 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4461 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4465 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4469 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4474 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4478 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCall : public Operation
{
public:
	OpCall(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4549 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4553 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4557 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4563 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4567 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4571 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4576 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4580 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcall : public Operation
{
public:
	OpIcall(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t btarg;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4652 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4656 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4660 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4666 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4670 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4674 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4679 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4683 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpClz : public Operation
{
public:
	OpClz(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4756 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4760 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4764 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4770 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4774 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4778 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4783 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4787 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpZxth : public Operation
{
public:
	OpZxth(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4860 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4864 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4868 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4874 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4878 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4882 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4887 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4891 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBswap : public Operation
{
public:
	OpBswap(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 4964 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 4968 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 4972 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 4978 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 4982 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 4986 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 4991 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 4995 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSxth : public Operation
{
public:
	OpSxth(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 5068 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 5072 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 5076 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 5082 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 5086 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 5090 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 5095 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 5099 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSxtb : public Operation
{
public:
	OpSxtb(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 5172 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 5176 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 5180 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 5186 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 5190 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 5194 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 5199 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 5203 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImulfrac : public Operation
{
public:
	OpImulfrac(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 5277 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 5281 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 5285 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 5291 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 5295 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 5299 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 5304 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 5308 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImul64hu : public Operation
{
public:
	OpImul64hu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 5382 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 5386 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 5390 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 5396 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 5400 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 5404 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 5409 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 5413 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbnorl : public Operation
{
public:
	OpIbnorl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 5488 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 5492 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 5496 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 5502 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 5506 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 5510 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 5515 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 5519 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIborl : public Operation
{
public:
	OpIborl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 5594 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 5598 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 5602 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 5608 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 5612 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 5616 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 5621 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 5625 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbnandl : public Operation
{
public:
	OpIbnandl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 5700 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 5704 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 5708 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 5714 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 5718 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 5722 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 5727 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 5731 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbandl : public Operation
{
public:
	OpIbandl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 5806 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 5810 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 5814 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 5820 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 5824 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 5828 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 5833 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 5837 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmpltu : public Operation
{
public:
	OpIbcmpltu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 5912 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 5916 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 5920 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 5926 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 5930 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 5934 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 5939 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 5943 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmplt : public Operation
{
public:
	OpIbcmplt(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6018 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6022 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6026 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6032 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6036 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6040 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6045 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 6049 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmpleu : public Operation
{
public:
	OpIbcmpleu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6124 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6128 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6132 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6138 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6142 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6146 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6151 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 6155 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmple : public Operation
{
public:
	OpIbcmple(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6230 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6234 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6238 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6244 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6248 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6252 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6257 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 6261 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmpgtu : public Operation
{
public:
	OpIbcmpgtu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6336 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6340 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6344 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6350 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6354 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6358 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6363 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 6367 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmpgt : public Operation
{
public:
	OpIbcmpgt(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6442 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6446 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6450 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6456 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6460 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6464 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6469 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 6473 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmpgeu : public Operation
{
public:
	OpIbcmpgeu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6548 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6552 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6556 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6562 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6566 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6570 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6575 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 6579 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmpge : public Operation
{
public:
	OpIbcmpge(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6654 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6658 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6662 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6668 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6672 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6676 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6681 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 6685 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmpne : public Operation
{
public:
	OpIbcmpne(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6760 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6764 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6768 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6774 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6778 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6782 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6787 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 6791 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIbcmpeq : public Operation
{
public:
	OpIbcmpeq(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t null;
	uint32_t ibdest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6866 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6870 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6874 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6880 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6884 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6888 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6893 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 6897 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImul64h : public Operation
{
public:
	OpImul64h(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 6971 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 6975 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 6979 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 6985 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 6989 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 6993 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 6998 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7002 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImul32 : public Operation
{
public:
	OpImul32(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 7076 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 7080 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 7084 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 7090 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 7094 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 7098 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 7103 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7107 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpInorl : public Operation
{
public:
	OpInorl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 7181 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 7185 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 7189 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 7195 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 7199 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 7203 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 7208 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7212 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIorl : public Operation
{
public:
	OpIorl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 7286 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 7290 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 7294 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 7300 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 7304 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 7308 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 7313 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7317 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpInandl : public Operation
{
public:
	OpInandl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 7391 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 7395 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 7399 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 7405 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 7409 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 7413 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 7418 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7422 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIandl : public Operation
{
public:
	OpIandl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 7496 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 7500 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 7504 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 7510 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 7514 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 7518 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 7523 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7527 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmpltu : public Operation
{
public:
	OpIcmpltu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 7601 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 7605 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 7609 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 7615 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 7619 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 7623 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 7628 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7632 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmplt : public Operation
{
public:
	OpIcmplt(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 7706 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 7710 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 7714 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 7720 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 7724 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 7728 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 7733 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7737 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmpleu : public Operation
{
public:
	OpIcmpleu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 7811 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 7815 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 7819 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 7825 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 7829 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 7833 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 7838 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7842 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmple : public Operation
{
public:
	OpIcmple(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 7916 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 7920 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 7924 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 7930 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 7934 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 7938 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 7943 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 7947 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmpgtu : public Operation
{
public:
	OpIcmpgtu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8021 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8025 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8029 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8035 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8039 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8043 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8048 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8052 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmpgt : public Operation
{
public:
	OpIcmpgt(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8126 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8130 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8134 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8140 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8144 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8148 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8153 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8157 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmpgeu : public Operation
{
public:
	OpIcmpgeu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8231 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8235 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8239 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8245 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8249 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8253 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8258 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8262 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmpge : public Operation
{
public:
	OpIcmpge(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8336 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8340 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8344 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8350 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8354 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8358 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8363 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8367 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmpne : public Operation
{
public:
	OpIcmpne(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8441 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8445 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8449 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8455 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8459 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8463 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8468 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8472 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIcmpeq : public Operation
{
public:
	OpIcmpeq(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8546 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8550 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8554 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8560 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8564 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8568 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8573 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8577 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImulhs : public Operation
{
public:
	OpImulhs(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8651 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8655 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8659 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8665 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8669 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8673 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8678 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8682 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImulhhu : public Operation
{
public:
	OpImulhhu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8756 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8760 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8764 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8770 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8774 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8778 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8783 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8787 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImulhh : public Operation
{
public:
	OpImulhh(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8861 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8865 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8869 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8875 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8879 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8883 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8888 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8892 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImullhu : public Operation
{
public:
	OpImullhu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 8966 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 8970 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 8974 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 8980 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 8984 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 8988 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 8993 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 8997 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImullh : public Operation
{
public:
	OpImullh(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 9071 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 9075 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 9079 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 9085 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 9089 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 9093 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 9098 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 9102 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImulllu : public Operation
{
public:
	OpImulllu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 9176 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 9180 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 9184 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 9190 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 9194 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 9198 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 9203 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 9207 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImulll : public Operation
{
public:
	OpImulll(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 9281 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 9285 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 9289 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 9295 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 9299 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 9303 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 9308 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 9312 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImulhu : public Operation
{
public:
	OpImulhu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 9386 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 9390 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 9394 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 9400 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 9404 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 9408 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 9413 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 9417 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImulh : public Operation
{
public:
	OpImulh(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 9491 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 9495 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 9499 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 9505 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 9509 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 9513 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 9518 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 9522 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImullu : public Operation
{
public:
	OpImullu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 9596 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 9600 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 9604 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 9610 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 9614 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 9618 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 9623 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 9627 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImull : public Operation
{
public:
	OpImull(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 9701 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 9705 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 9709 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 9715 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 9719 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 9723 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 9728 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 9732 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImulhhs : public Operation
{
public:
	OpImulhhs(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 9806 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 9810 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 9814 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 9820 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 9824 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 9828 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 9833 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 9837 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIminu : public Operation
{
public:
	OpIminu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 9911 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 9915 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 9919 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 9925 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 9929 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 9933 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 9938 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 9942 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImin : public Operation
{
public:
	OpImin(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10016 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10020 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10024 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10030 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10034 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10038 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10043 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10047 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImaxu : public Operation
{
public:
	OpImaxu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10121 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10125 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10129 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10135 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10139 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10143 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10148 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10152 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImax : public Operation
{
public:
	OpImax(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10226 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10230 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10234 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10240 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10244 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10248 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10253 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10257 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpImullhus : public Operation
{
public:
	OpImullhus(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t nlidest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10331 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10335 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10339 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10345 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10349 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10353 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10358 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10362 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIxor : public Operation
{
public:
	OpIxor(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10436 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10440 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10444 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10450 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10454 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10458 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10463 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10467 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIorc : public Operation
{
public:
	OpIorc(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10541 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10545 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10549 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10555 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10559 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10563 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10568 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10572 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIor : public Operation
{
public:
	OpIor(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10646 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10650 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10654 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10660 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10664 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10668 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10673 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10677 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIandc : public Operation
{
public:
	OpIandc(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10751 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10755 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10759 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10765 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10769 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10773 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10778 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10782 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIand : public Operation
{
public:
	OpIand(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10856 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10860 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10864 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10870 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10874 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10878 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10883 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10887 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIsh4add : public Operation
{
public:
	OpIsh4add(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 10961 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 10965 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 10969 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 10975 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 10979 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 10983 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 10988 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 10992 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIsh3add : public Operation
{
public:
	OpIsh3add(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 11066 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 11070 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 11074 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 11080 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 11084 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 11088 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 11093 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 11097 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIsh2add : public Operation
{
public:
	OpIsh2add(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 11171 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 11175 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 11179 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 11185 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 11189 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 11193 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 11198 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 11202 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIsh1add : public Operation
{
public:
	OpIsh1add(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 11276 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 11280 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 11284 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 11290 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 11294 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 11298 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 11303 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 11307 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIshru : public Operation
{
public:
	OpIshru(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 11381 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 11385 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 11389 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 11395 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 11399 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 11403 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 11408 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 11412 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIshr : public Operation
{
public:
	OpIshr(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 11486 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 11490 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 11494 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 11500 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 11504 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 11508 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 11513 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 11517 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIshl : public Operation
{
public:
	OpIshl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 11591 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 11595 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 11599 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 11605 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 11609 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 11613 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 11618 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 11622 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIsub : public Operation
{
public:
	OpIsub(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 11696 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 11700 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 11704 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 11710 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 11714 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 11718 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 11723 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 11727 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpIadd : public Operation
{
public:
	OpIadd(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t isrc2;
	uint32_t idest;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 11801 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 11805 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 11809 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 11815 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 11819 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 11823 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 11828 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 11832 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMulfrac : public Operation
{
public:
	OpMulfrac(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 11907 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 11911 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 11915 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 11921 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 11925 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 11929 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 11934 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 11938 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMul64hu : public Operation
{
public:
	OpMul64hu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12013 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12017 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12021 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12027 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12031 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12035 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12040 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12044 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBnorl : public Operation
{
public:
	OpBnorl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12119 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12123 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12127 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12133 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12137 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12141 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12146 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12150 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBorl : public Operation
{
public:
	OpBorl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12225 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12229 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12233 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12239 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12243 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12247 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12252 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12256 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBnandl : public Operation
{
public:
	OpBnandl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12331 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12335 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12339 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12345 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12349 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12353 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12358 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12362 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBandl : public Operation
{
public:
	OpBandl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12437 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12441 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12445 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12451 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12455 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12459 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12464 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12468 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmpltu : public Operation
{
public:
	OpBcmpltu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12543 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12547 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12551 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12557 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12561 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12565 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12570 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12574 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmplt : public Operation
{
public:
	OpBcmplt(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12649 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12653 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12657 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12663 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12667 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12671 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12676 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12680 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmpleu : public Operation
{
public:
	OpBcmpleu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12755 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12759 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12763 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12769 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12773 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12777 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12782 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12786 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmple : public Operation
{
public:
	OpBcmple(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12861 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12865 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12869 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12875 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12879 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12883 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12888 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12892 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmpgtu : public Operation
{
public:
	OpBcmpgtu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 12967 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 12971 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 12975 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 12981 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 12985 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 12989 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 12994 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 12998 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmpgt : public Operation
{
public:
	OpBcmpgt(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 13073 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 13077 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 13081 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 13087 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 13091 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 13095 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 13100 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 13104 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmpgeu : public Operation
{
public:
	OpBcmpgeu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 13179 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 13183 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 13187 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 13193 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 13197 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 13201 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 13206 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 13210 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmpge : public Operation
{
public:
	OpBcmpge(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 13285 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 13289 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 13293 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 13299 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 13303 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 13307 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 13312 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 13316 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmpne : public Operation
{
public:
	OpBcmpne(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 13391 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 13395 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 13399 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 13405 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 13409 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 13413 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 13418 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 13422 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpBcmpeq : public Operation
{
public:
	OpBcmpeq(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 13497 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 13501 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 13505 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 13511 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 13515 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 13519 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 13524 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 13528 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMul64h : public Operation
{
public:
	OpMul64h(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 13603 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 13607 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 13611 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 13617 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 13621 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 13625 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 13630 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 13634 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMul32 : public Operation
{
public:
	OpMul32(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 13709 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 13713 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 13717 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 13723 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 13727 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 13731 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 13736 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 13740 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpNorl : public Operation
{
public:
	OpNorl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 13815 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 13819 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 13823 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 13829 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 13833 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 13837 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 13842 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 13846 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpOrl : public Operation
{
public:
	OpOrl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 13921 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 13925 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 13929 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 13935 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 13939 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 13943 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 13948 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 13952 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpNandl : public Operation
{
public:
	OpNandl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14027 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14031 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14035 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14041 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14045 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 14049 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 14054 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 14058 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpAndl : public Operation
{
public:
	OpAndl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14133 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14137 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14141 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14147 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14151 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 14155 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 14160 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 14164 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmpltu : public Operation
{
public:
	OpCmpltu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14239 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14243 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14247 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14253 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14257 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 14261 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 14266 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 14270 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmplt : public Operation
{
public:
	OpCmplt(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14345 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14349 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14353 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14359 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14363 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 14367 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 14372 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 14376 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmpleu : public Operation
{
public:
	OpCmpleu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14451 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14455 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14459 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14465 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14469 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 14473 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 14478 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 14482 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmple : public Operation
{
public:
	OpCmple(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14557 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14561 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14565 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14571 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14575 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 14579 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 14584 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 14588 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmpgtu : public Operation
{
public:
	OpCmpgtu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14663 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14667 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14671 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14677 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14681 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 14685 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 14690 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 14694 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmpgt : public Operation
{
public:
	OpCmpgt(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14769 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14773 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14777 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14783 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14787 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 14791 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 14796 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 14800 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmpgeu : public Operation
{
public:
	OpCmpgeu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14875 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14879 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14883 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14889 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14893 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 14897 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 14902 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 14906 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmpge : public Operation
{
public:
	OpCmpge(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 14981 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 14985 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 14989 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 14995 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 14999 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15003 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15008 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15012 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmpne : public Operation
{
public:
	OpCmpne(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 15087 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 15091 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 15095 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 15101 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 15105 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15109 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15114 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15118 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpCmpeq : public Operation
{
public:
	OpCmpeq(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 15193 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 15197 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 15201 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 15207 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 15211 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15215 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15220 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15224 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMulhs : public Operation
{
public:
	OpMulhs(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 15299 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 15303 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 15307 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 15313 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 15317 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15321 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15326 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15330 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMulhhu : public Operation
{
public:
	OpMulhhu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 15405 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 15409 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 15413 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 15419 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 15423 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15427 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15432 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15436 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMulhh : public Operation
{
public:
	OpMulhh(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 15511 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 15515 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 15519 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 15525 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 15529 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15533 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15538 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15542 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMullhu : public Operation
{
public:
	OpMullhu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 15617 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 15621 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 15625 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 15631 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 15635 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15639 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15644 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15648 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMullh : public Operation
{
public:
	OpMullh(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 15723 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 15727 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 15731 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 15737 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 15741 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15745 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15750 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15754 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMulllu : public Operation
{
public:
	OpMulllu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 15829 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 15833 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 15837 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 15843 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 15847 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15851 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15856 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15860 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMulll : public Operation
{
public:
	OpMulll(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 15935 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 15939 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 15943 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 15949 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 15953 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 15957 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 15962 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 15966 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMulhu : public Operation
{
public:
	OpMulhu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16041 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16045 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 16049 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 16055 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 16059 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 16063 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 16068 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 16072 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMulh : public Operation
{
public:
	OpMulh(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16147 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16151 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 16155 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 16161 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 16165 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 16169 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 16174 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 16178 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMullu : public Operation
{
public:
	OpMullu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16253 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16257 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 16261 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 16267 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 16271 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 16275 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 16280 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 16284 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMull : public Operation
{
public:
	OpMull(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16359 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16363 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 16367 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 16373 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 16377 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 16381 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 16386 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 16390 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMulhhs : public Operation
{
public:
	OpMulhhs(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16465 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16469 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 16473 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 16479 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 16483 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 16487 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 16492 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 16496 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMinu : public Operation
{
public:
	OpMinu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16571 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16575 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 16579 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 16585 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 16589 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 16593 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 16598 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 16602 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMin : public Operation
{
public:
	OpMin(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16677 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16681 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 16685 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 16691 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 16695 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 16699 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 16704 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 16708 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMaxu : public Operation
{
public:
	OpMaxu(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16783 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16787 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 16791 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 16797 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 16801 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 16805 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 16810 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 16814 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMax : public Operation
{
public:
	OpMax(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16889 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16893 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 16897 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 16903 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 16907 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 16911 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 16916 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 16920 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpMullhus : public Operation
{
public:
	OpMullhus(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t nldest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 16995 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 16999 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17003 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17009 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17013 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17017 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17022 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17026 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpXor : public Operation
{
public:
	OpXor(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 17101 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 17105 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17109 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17115 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17119 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17123 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17128 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17132 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpOrc : public Operation
{
public:
	OpOrc(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 17207 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 17211 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17215 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17221 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17225 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17229 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17234 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17238 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpOr : public Operation
{
public:
	OpOr(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 17313 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 17317 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17321 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17327 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17331 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17335 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17340 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17344 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpAndc : public Operation
{
public:
	OpAndc(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 17419 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 17423 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17427 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17433 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17437 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17441 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17446 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17450 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpAnd : public Operation
{
public:
	OpAnd(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 17525 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 17529 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17533 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17539 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17543 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17547 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17552 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17556 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSh4add : public Operation
{
public:
	OpSh4add(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 17631 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 17635 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17639 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17645 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17649 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17653 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17658 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17662 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSh3add : public Operation
{
public:
	OpSh3add(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 17737 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 17741 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17745 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17751 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17755 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17759 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17764 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17768 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSh2add : public Operation
{
public:
	OpSh2add(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 17843 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 17847 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17851 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17857 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17861 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17865 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17870 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17874 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSh1add : public Operation
{
public:
	OpSh1add(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 17949 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 17953 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 17957 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 17963 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 17967 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 17971 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 17976 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 17980 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpShru : public Operation
{
public:
	OpShru(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 18055 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18059 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18063 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 18069 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18073 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18077 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18082 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18086 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpRshr : public Operation
{
public:
	OpRshr(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 18161 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18165 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18169 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 18175 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18179 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18183 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18188 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18192 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpRshl : public Operation
{
public:
	OpRshl(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 18267 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18271 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18275 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 18281 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18285 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18289 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18294 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18298 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpSub : public Operation
{
public:
	OpSub(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 18373 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18377 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18381 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 18387 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18391 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18395 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18400 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18404 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

class OpAdd : public Operation
{
public:
	OpAdd(st231::CodeType code, uint32_t addr);
	uint32_t stop;
	uint32_t rsv;
	uint32_t bdest;
	uint32_t dest;
	uint32_t rsc2;
	uint32_t rsc1;
	/*******************************************************************************
   st231.isa  -  ST231 instruction set
                 architecture GenISSLib descriptions
                 sim_xxx : the interfaces for Unisim cycle-level simulator

*******************************************************************************/
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
	virtual

#line 53 "isa/st231.isa"
	void
#line 18479 "cpu/st231_isa.cpp"
	execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18483 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18487 "cpu/st231_isa.cpp"
	);
	virtual

#line 58 "isa/st231.isa"
	void
#line 18493 "cpu/st231_isa.cpp"
	disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18497 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18501 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18506 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18510 "cpu/st231_isa.cpp"
	);
	// default constructor for the not difined instructions 
	virtual
	void
	initialize_operands();
	/**
 * Initilize the latency values of each instrucion
 */
	virtual
	void
	initialize_latencies();
	// ??? how to define the latencies before an instruction ???
	/** Functions **/
	virtual
	void
	initialize_function();
private:
};

// op pswclr(stop[1]:rsv[1]:0b10[2]:0b10011[5]:null[11]:src2[6]:0b000000[6])

#line 53 "isa/st231.isa"
void
#line 18534 "cpu/st231_isa.cpp"
OpPswclr::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18538 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18542 "cpu/st231_isa.cpp"
)
{

#line 491 "memory.isa"
{
        int32_t ea = cpu->GetGPR_C(src2);

	if(PSW_USER_MODE)
           cpu->ThrowIllInst();

	cpu->PswClr(ea);
}
#line 18555 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 18560 "cpu/st231_isa.cpp"
OpPswclr::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18564 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18568 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18573 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18577 "cpu/st231_isa.cpp"
)
{

#line 500 "memory.isa"
{
	os << "pswclr $r" << src2 ; 
}
#line 18585 "cpu/st231_isa.cpp"
}
/* === Psw Operation INSTRUCTIONS ====================================== */

void
OpPswclr::initialize_operands()
{

#line 352 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, src2);
}
#line 18598 "cpu/st231_isa.cpp"
}

void
OpPswclr::initialize_latencies()
{

#line 201 "sim_latencies.isa"
{LAT(1);}
#line 18607 "cpu/st231_isa.cpp"
}

void
OpPswclr::initialize_function()
{

#line 41 "sim_functions.isa"
{ function = FnIntBasic; }
#line 18616 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpPswclr(st231::CodeType code, uint32_t addr)
{
	return new OpPswclr(code, addr);
}

// op pswset(stop[1]:rsv[1]:0b10[2]:0b10010[5]:null[11]:src2[6]:0b000000[6])

#line 53 "isa/st231.isa"
void
#line 18628 "cpu/st231_isa.cpp"
OpPswset::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18632 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18636 "cpu/st231_isa.cpp"
)
{

#line 477 "memory.isa"
{
        int32_t ea = cpu->GetGPR_C(src2);

	if(PSW_USER_MODE)
           cpu->ThrowIllInst();

	cpu->PswSet(ea);
}
#line 18649 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 18654 "cpu/st231_isa.cpp"
OpPswset::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18658 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18662 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18667 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18671 "cpu/st231_isa.cpp"
)
{

#line 486 "memory.isa"
{
	os << "pswset $r" << src2 ; 
}
#line 18679 "cpu/st231_isa.cpp"
}
/* === Psw Operation INSTRUCTIONS ====================================== */

void
OpPswset::initialize_operands()
{

#line 352 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, src2);
}
#line 18692 "cpu/st231_isa.cpp"
}

void
OpPswset::initialize_latencies()
{

#line 200 "sim_latencies.isa"
{LAT(1);}
#line 18701 "cpu/st231_isa.cpp"
}

void
OpPswset::initialize_function()
{

#line 41 "sim_functions.isa"
{ function = FnIntBasic; }
#line 18710 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpPswset(st231::CodeType code, uint32_t addr)
{
	return new OpPswset(code, addr);
}

// op sync(stop[1]:rsv[1]:0b10[2]:0b10000[5]:null[2]:0b000000000000000000000[21])

#line 53 "isa/st231.isa"
void
#line 18722 "cpu/st231_isa.cpp"
OpSync::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18726 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18730 "cpu/st231_isa.cpp"
)
{

#line 452 "memory.isa"
{
	sync();
}
#line 18738 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 18743 "cpu/st231_isa.cpp"
OpSync::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18747 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18751 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18756 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18760 "cpu/st231_isa.cpp"
)
{

#line 456 "memory.isa"
{
	os << "sync" ; 
}
#line 18768 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === No dependences Operation INSTRUCTIONS =============================== */
/*****************************************************************************/

void
OpSync::initialize_operands()
{

#line 382 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 18782 "cpu/st231_isa.cpp"
}

void
OpSync::initialize_latencies()
{

#line 208 "sim_latencies.isa"
{LAT(1);}
#line 18791 "cpu/st231_isa.cpp"
}

void
OpSync::initialize_function()
{

#line 42 "sim_functions.isa"
{ function = FnIntBasic; }
#line 18800 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSync(st231::CodeType code, uint32_t addr)
{
	return new OpSync(code, addr);
}

// op prginspg(stop[1]:rsv[1]:0b10[2]:0b10001[5]:null[2]:isrc2[9]:0b000000:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 18812 "cpu/st231_isa.cpp"
OpPrginspg::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18816 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18820 "cpu/st231_isa.cpp"
)
{

#line 461 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;

	if(PSW_USER_MODE)
           cpu->ThrowIllInst();
	cpu->PurgeInsPg(ea);
}
#line 18834 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 18839 "cpu/st231_isa.cpp"
OpPrginspg::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18843 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18847 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18852 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18856 "cpu/st231_isa.cpp"
)
{

#line 471 "memory.isa"
{
        int32_t op3 = SignEx9to32(isrc2);
	os << "prginspg " << op3 <<"[ $r" <<rsc1 << " ]" ; 
}
#line 18865 "cpu/st231_isa.cpp"
}
/* === Mono-register Operation INSTRUCTIONS ============================ */

void
OpPrginspg::initialize_operands()
{

#line 343 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
}
#line 18878 "cpu/st231_isa.cpp"
}

void
OpPrginspg::initialize_latencies()
{

#line 199 "sim_latencies.isa"
{LAT(1);}
#line 18887 "cpu/st231_isa.cpp"
}

void
OpPrginspg::initialize_function()
{

#line 40 "sim_functions.isa"
{ function = FnIntBasic; }
#line 18896 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpPrginspg(st231::CodeType code, uint32_t addr)
{
	return new OpPrginspg(code, addr);
}

// op prgset(stop[1]:rsv[1]:0b10[2]:0b01111[5]:null[2]:isrc2[9]:0b000000[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 18908 "cpu/st231_isa.cpp"
OpPrgset::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 18912 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 18916 "cpu/st231_isa.cpp"
)
{

#line 438 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;

	cpu->PurgeSet(ea);
}
#line 18928 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 18933 "cpu/st231_isa.cpp"
OpPrgset::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 18937 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 18941 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 18946 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 18950 "cpu/st231_isa.cpp"
)
{

#line 446 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "prgset " << op2 <<"[ $r" <<rsc1 << " ]"; 
}
#line 18959 "cpu/st231_isa.cpp"
}
/* === Mono-register Operation INSTRUCTIONS ============================ */

void
OpPrgset::initialize_operands()
{

#line 343 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
}
#line 18972 "cpu/st231_isa.cpp"
}

void
OpPrgset::initialize_latencies()
{

#line 198 "sim_latencies.isa"
{LAT(1);}
#line 18981 "cpu/st231_isa.cpp"
}

void
OpPrgset::initialize_function()
{

#line 40 "sim_functions.isa"
{ function = FnIntBasic; }
#line 18990 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpPrgset(st231::CodeType code, uint32_t addr)
{
	return new OpPrgset(code, addr);
}

// op prgadd(stop[1]:rsv[1]:0b10[2]:0b01110[5]:null[2]:isrc2[9]:0b000000[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 19002 "cpu/st231_isa.cpp"
OpPrgadd::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 19006 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 19010 "cpu/st231_isa.cpp"
)
{

#line 423 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;

	cpu->PurgeAddressCheckMemory(ea);
	cpu->PurgeAddress(ea);
}
#line 19023 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 19028 "cpu/st231_isa.cpp"
OpPrgadd::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 19032 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 19036 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 19041 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 19045 "cpu/st231_isa.cpp"
)
{

#line 432 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "prgadd " << op2 <<"[ $r" <<rsc1 << " ]" ; 
}
#line 19054 "cpu/st231_isa.cpp"
}
/* === Mono-register Operation INSTRUCTIONS ============================ */

void
OpPrgadd::initialize_operands()
{

#line 343 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
}
#line 19067 "cpu/st231_isa.cpp"
}

void
OpPrgadd::initialize_latencies()
{

#line 197 "sim_latencies.isa"
{LAT(1);}
#line 19076 "cpu/st231_isa.cpp"
}

void
OpPrgadd::initialize_function()
{

#line 40 "sim_functions.isa"
{ function = FnIntBasic; }
#line 19085 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpPrgadd(st231::CodeType code, uint32_t addr)
{
	return new OpPrgadd(code, addr);
}

// the following instructions don't really do a load/store operation
// but they use the ld/sw unit, so they are written in this file
// op pft(stop[1]:rsv[1]:0b10[2]:0b01101[5]:null[2]:isrc2[9]:0b000000[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 19099 "cpu/st231_isa.cpp"
OpPft::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 19103 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 19107 "cpu/st231_isa.cpp"
)
{

#line 408 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;

	cpu->PrefetchCheckMemory(ea);
	cpu->PrefetchMemory(ea);
}
#line 19120 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 19125 "cpu/st231_isa.cpp"
OpPft::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 19129 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 19133 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 19138 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 19142 "cpu/st231_isa.cpp"
)
{

#line 417 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "pft " << op2 <<"[ $r" <<rsc1 << " ]" ; 
}
#line 19151 "cpu/st231_isa.cpp"
}
/* === Mono-register Operation INSTRUCTIONS ============================ */

void
OpPft::initialize_operands()
{

#line 343 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
}
#line 19164 "cpu/st231_isa.cpp"
}

void
OpPft::initialize_latencies()
{

#line 196 "sim_latencies.isa"
{LAT(1);}
#line 19173 "cpu/st231_isa.cpp"
}

void
OpPft::initialize_function()
{

#line 40 "sim_functions.isa"
{ function = FnIntBasic; }
#line 19182 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpPft(st231::CodeType code, uint32_t addr)
{
	return new OpPft(code, addr);
}

// op stb(stop[1]:rsv[1]:0b10[2]:0b01100[5]:null[2]:isrc2[9]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 19194 "cpu/st231_isa.cpp"
OpStb::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 19198 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 19202 "cpu/st231_isa.cpp"
)
{

#line 384 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        int32_t op3 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op3;

	if(cpu->IsDBreakHit(ea))
   	    cpu->ThrowDBreak(ea);
	if(cpu->IsCRegSpace(ea))
   	    cpu->ThrowCRegAccessViolation(ea);

	cpu->WriteCheckMemory8(ea);
	cpu->WriteMemory8(ea,op2);

        // compute the effective address
        cpu->SetEA(ea);
}
#line 19224 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 19229 "cpu/st231_isa.cpp"
OpStb::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 19233 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 19237 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 19242 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 19246 "cpu/st231_isa.cpp"
)
{

#line 402 "memory.isa"
{
        int32_t op3 = SignEx9to32(isrc2);
	os << "stb " << op3 <<"[ $r" <<rsc1 << " ] = $r" << rsc2; 
}
#line 19255 "cpu/st231_isa.cpp"
}
/* === Store INSTRUCTIONS ============================================== */

void
OpStb::initialize_operands()
{

#line 331 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
}
#line 19269 "cpu/st231_isa.cpp"
}

void
OpStb::initialize_latencies()
{

#line 185 "sim_latencies.isa"
{LAT(1);}
#line 19278 "cpu/st231_isa.cpp"
}

void
OpStb::initialize_function()
{

#line 49 "sim_functions.isa"
{ function = FnStore; }
#line 19287 "cpu/st231_isa.cpp"
}
//Store word & Store bytes

#line 4 "sim_loadstore.isa"
int
#line 19293 "cpu/st231_isa.cpp"
OpStb::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 19297 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 19301 "cpu/st231_isa.cpp"
)
{

#line 16 "sim_loadstore.isa"
{ return rsc2; }
#line 19307 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 19312 "cpu/st231_isa.cpp"
OpStb::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 19316 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 19320 "cpu/st231_isa.cpp"
)
{

#line 44 "sim_loadstore.isa"
{ return 1; }
#line 19326 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpStb(st231::CodeType code, uint32_t addr)
{
	return new OpStb(code, addr);
}

// op sth(stop[1]:rsv[1]:0b10[2]:0b01011[5]:null[2]:isrc2[9]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 19338 "cpu/st231_isa.cpp"
OpSth::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 19342 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 19346 "cpu/st231_isa.cpp"
)
{

#line 361 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        int32_t op3 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op3;

	if(cpu->IsDBreakHit(ea))
   	    cpu->ThrowDBreak(ea);
	if(cpu->IsCRegSpace(ea))
   	    cpu->ThrowCRegAccessViolation(ea);

	cpu->WriteCheckMemory16(ea);
	cpu->WriteMemory16(ea,op2);

        // compute the effective address
        cpu->SetEA(ea);
}
#line 19368 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 19373 "cpu/st231_isa.cpp"
OpSth::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 19377 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 19381 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 19386 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 19390 "cpu/st231_isa.cpp"
)
{

#line 379 "memory.isa"
{
        int32_t op3 = SignEx9to32(isrc2);
	os << "sth " << op3 <<"[ $r" <<rsc1 << " ] = $r" << rsc2; 
}
#line 19399 "cpu/st231_isa.cpp"
}
/* === Store INSTRUCTIONS ============================================== */

void
OpSth::initialize_operands()
{

#line 331 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
}
#line 19413 "cpu/st231_isa.cpp"
}

void
OpSth::initialize_latencies()
{

#line 184 "sim_latencies.isa"
{LAT(1);}
#line 19422 "cpu/st231_isa.cpp"
}

void
OpSth::initialize_function()
{

#line 49 "sim_functions.isa"
{ function = FnStore; }
#line 19431 "cpu/st231_isa.cpp"
}
//Store word & Store bytes

#line 4 "sim_loadstore.isa"
int
#line 19437 "cpu/st231_isa.cpp"
OpSth::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 19441 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 19445 "cpu/st231_isa.cpp"
)
{

#line 16 "sim_loadstore.isa"
{ return rsc2; }
#line 19451 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 19456 "cpu/st231_isa.cpp"
OpSth::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 19460 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 19464 "cpu/st231_isa.cpp"
)
{

#line 43 "sim_loadstore.isa"
{ return 2; }
#line 19470 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSth(st231::CodeType code, uint32_t addr)
{
	return new OpSth(code, addr);
}

// op stw(stop[1]:rsv[1]:0b10[2]:0b01010[5]:null[2]:isrc2[9]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 19482 "cpu/st231_isa.cpp"
OpStw::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 19486 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 19490 "cpu/st231_isa.cpp"
)
{

#line 331 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        int32_t op3 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op3;

        if(cpu->IsDBreakHit(ea))
            cpu->ThrowDBreak(ea);
        if(cpu->IsCRegSpace(ea))
            cpu->WriteCheckCReg(ea);
        else
            cpu->WriteCheckMemory32(ea);
        
        if(cpu->IsCRegSpace(ea))
            cpu->WrirteCReg(ea,op2);
        else
            cpu->WriteMemory32(ea,op2);

        // compute the effective address
        cpu->SetEA(ea);
}
#line 19516 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 19521 "cpu/st231_isa.cpp"
OpStw::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 19525 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 19529 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 19534 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 19538 "cpu/st231_isa.cpp"
)
{

#line 353 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        int32_t op3 = SignEx9to32(isrc2);
	os << "stw " <<hex<< op3 <<"[ $r" <<dec<< rsc1 << " ] = $r" << rsc2 <<"    //mem[" <<hex <<op3<<" + r" <<dec<<rsc1 <<"("<<hex<<op1<<")" << "] = " <<dec <<op2;
}
#line 19549 "cpu/st231_isa.cpp"
}
/* === Store INSTRUCTIONS ============================================== */

void
OpStw::initialize_operands()
{

#line 331 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
}
#line 19563 "cpu/st231_isa.cpp"
}

void
OpStw::initialize_latencies()
{

#line 183 "sim_latencies.isa"
{LAT(1);}
#line 19572 "cpu/st231_isa.cpp"
}

void
OpStw::initialize_function()
{

#line 49 "sim_functions.isa"
{ function = FnStore; }
#line 19581 "cpu/st231_isa.cpp"
}
//Store word & Store bytes

#line 4 "sim_loadstore.isa"
int
#line 19587 "cpu/st231_isa.cpp"
OpStw::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 19591 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 19595 "cpu/st231_isa.cpp"
)
{

#line 16 "sim_loadstore.isa"
{ return rsc2; }
#line 19601 "cpu/st231_isa.cpp"
}
//Store word & Store bytes

#line 23 "sim_loadstore.isa"
int
#line 19607 "cpu/st231_isa.cpp"
OpStw::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 19611 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 19615 "cpu/st231_isa.cpp"
)
{

#line 42 "sim_loadstore.isa"
{ return 4; }
#line 19621 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpStw(st231::CodeType code, uint32_t addr)
{
	return new OpStw(code, addr);
}

// op ldbu_d(stop[1]:rsv[1]:0b10[2]:0b01001[5]:null[2]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 19633 "cpu/st231_isa.cpp"
OpLdbu_d::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 19637 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 19641 "cpu/st231_isa.cpp"
)
{

#line 301 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        uint8_t result;

	if( cpu->IsDBreakHit(ea) )
   	    cpu->ThrowDBreak(ea);
	if( cpu->IsCRegSpace(ea) == 0 ) 
	    cpu->DisReadCheckMemory8(ea);

   
	if( cpu->IsCRegSpace(ea) )
            result = 0;
        else
	    result = cpu->DisReadMemory8(ea);

        // compute the effective address
        cpu->SetEA(ea);

        cpu->SetGPR_N(nlidest, result);
        
}
#line 19669 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 19674 "cpu/st231_isa.cpp"
OpLdbu_d::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 19678 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 19682 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 19687 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 19691 "cpu/st231_isa.cpp"
)
{

#line 325 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldh_d $r" << nlidest << " = " << op2 <<"[ $r" <<rsc1 << " ]";
}
#line 19700 "cpu/st231_isa.cpp"
}
/* === Load Bytes INSTRUCTIONS ========================================= */

void
OpLdbu_d::initialize_operands()
{

#line 320 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 19714 "cpu/st231_isa.cpp"
}

void
OpLdbu_d::initialize_latencies()
{

#line 181 "sim_latencies.isa"
{LAT(3);}
#line 19723 "cpu/st231_isa.cpp"
}

void
OpLdbu_d::initialize_function()
{

#line 48 "sim_functions.isa"
{ function = FnLoad; }
#line 19732 "cpu/st231_isa.cpp"
}

#line 4 "sim_loadstore.isa"
int
#line 19737 "cpu/st231_isa.cpp"
OpLdbu_d::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 19741 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 19745 "cpu/st231_isa.cpp"
)
{

#line 13 "sim_loadstore.isa"
{ return nlidest; }
#line 19751 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 19756 "cpu/st231_isa.cpp"
OpLdbu_d::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 19760 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 19764 "cpu/st231_isa.cpp"
)
{

#line 39 "sim_loadstore.isa"
{ return 1; }
#line 19770 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdbu_d(st231::CodeType code, uint32_t addr)
{
	return new OpLdbu_d(code, addr);
}

// op ldbu(stop[1]:rsv[1]:0b10[2]:0b01000[5]:null[2]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 19782 "cpu/st231_isa.cpp"
OpLdbu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 19786 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 19790 "cpu/st231_isa.cpp"
)
{

#line 274 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        uint8_t result;

	if(cpu->IsDBreakHit(ea))
   	    cpu->ThrowDBreak(ea);
	if(cpu->IsCRegSpace(ea))
   	    cpu->ThrowCRegAccessViolation(ea);
	else
	    cpu->ReadCheckMemory8(ea);
   
	result = cpu->ReadMemory8(ea);

        // compute the effective address
        cpu->SetEA(ea);

        cpu->SetGPR_N(nlidest, result);
}
#line 19815 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 19820 "cpu/st231_isa.cpp"
OpLdbu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 19824 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 19828 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 19833 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 19837 "cpu/st231_isa.cpp"
)
{

#line 295 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldb $r" << nlidest << " = " << op2 <<"[ $r" <<rsc1 << " ]";
}
#line 19846 "cpu/st231_isa.cpp"
}
/* === Load Bytes INSTRUCTIONS ========================================= */

void
OpLdbu::initialize_operands()
{

#line 320 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 19860 "cpu/st231_isa.cpp"
}

void
OpLdbu::initialize_latencies()
{

#line 180 "sim_latencies.isa"
{LAT(3);}
#line 19869 "cpu/st231_isa.cpp"
}

void
OpLdbu::initialize_function()
{

#line 48 "sim_functions.isa"
{ function = FnLoad; }
#line 19878 "cpu/st231_isa.cpp"
}

#line 4 "sim_loadstore.isa"
int
#line 19883 "cpu/st231_isa.cpp"
OpLdbu::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 19887 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 19891 "cpu/st231_isa.cpp"
)
{

#line 13 "sim_loadstore.isa"
{ return nlidest; }
#line 19897 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 19902 "cpu/st231_isa.cpp"
OpLdbu::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 19906 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 19910 "cpu/st231_isa.cpp"
)
{

#line 38 "sim_loadstore.isa"
{ return 1; }
#line 19916 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdbu(st231::CodeType code, uint32_t addr)
{
	return new OpLdbu(code, addr);
}

// op ldb_d(stop[1]:rsv[1]:0b10[2]:0b00111[5]:null[2]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 19928 "cpu/st231_isa.cpp"
OpLdb_d::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 19932 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 19936 "cpu/st231_isa.cpp"
)
{

#line 244 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        int8_t result;

	if( cpu->IsDBreakHit(ea) )
   	    cpu->ThrowDBreak(ea);
	if( cpu->IsCRegSpace(ea) == 0 ) 
	    cpu->DisReadCheckMemory8(ea);

   
	if( cpu->IsCRegSpace(ea) )
            result = 0;
        else
	    result = cpu->DisReadMemory8(ea);

        // compute the effective address
        cpu->SetEA(ea);

        cpu->SetGPR_N(nlidest, result);
        
}
#line 19964 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 19969 "cpu/st231_isa.cpp"
OpLdb_d::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 19973 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 19977 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 19982 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 19986 "cpu/st231_isa.cpp"
)
{

#line 268 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldb_d $r" << nlidest << " = " << op2 <<"[ $r" <<rsc1 << " ]";
}
#line 19995 "cpu/st231_isa.cpp"
}
/* === Load Bytes INSTRUCTIONS ========================================= */

void
OpLdb_d::initialize_operands()
{

#line 320 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 20009 "cpu/st231_isa.cpp"
}

void
OpLdb_d::initialize_latencies()
{

#line 179 "sim_latencies.isa"
{LAT(3);}
#line 20018 "cpu/st231_isa.cpp"
}

void
OpLdb_d::initialize_function()
{

#line 48 "sim_functions.isa"
{ function = FnLoad; }
#line 20027 "cpu/st231_isa.cpp"
}

#line 4 "sim_loadstore.isa"
int
#line 20032 "cpu/st231_isa.cpp"
OpLdb_d::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 20036 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 20040 "cpu/st231_isa.cpp"
)
{

#line 13 "sim_loadstore.isa"
{ return nlidest; }
#line 20046 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 20051 "cpu/st231_isa.cpp"
OpLdb_d::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 20055 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 20059 "cpu/st231_isa.cpp"
)
{

#line 37 "sim_loadstore.isa"
{ return 1; }
#line 20065 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdb_d(st231::CodeType code, uint32_t addr)
{
	return new OpLdb_d(code, addr);
}

// op ldb(stop[1]:rsv[1]:0b10[2]:0b00110[5]:null[2]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 20077 "cpu/st231_isa.cpp"
OpLdb::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 20081 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 20085 "cpu/st231_isa.cpp"
)
{

#line 217 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        int8_t result;

	if(cpu->IsDBreakHit(ea))
   	    cpu->ThrowDBreak(ea);
	if(cpu->IsCRegSpace(ea))
   	    cpu->ThrowCRegAccessViolation(ea);
	else
	    cpu->ReadCheckMemory8(ea);
   
	result = cpu->ReadMemory8(ea);

        // compute the effective address
        cpu->SetEA(ea);

        cpu->SetGPR_N(nlidest, result);
}
#line 20110 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 20115 "cpu/st231_isa.cpp"
OpLdb::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 20119 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 20123 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 20128 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 20132 "cpu/st231_isa.cpp"
)
{

#line 238 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldb $r" << nlidest << " = " << op2 <<"[ $r" <<rsc1 << " ]";
}
#line 20141 "cpu/st231_isa.cpp"
}
/* === Load Bytes INSTRUCTIONS ========================================= */

void
OpLdb::initialize_operands()
{

#line 320 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 20155 "cpu/st231_isa.cpp"
}

void
OpLdb::initialize_latencies()
{

#line 178 "sim_latencies.isa"
{LAT(3);}
#line 20164 "cpu/st231_isa.cpp"
}

void
OpLdb::initialize_function()
{

#line 48 "sim_functions.isa"
{ function = FnLoad; }
#line 20173 "cpu/st231_isa.cpp"
}

#line 4 "sim_loadstore.isa"
int
#line 20178 "cpu/st231_isa.cpp"
OpLdb::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 20182 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 20186 "cpu/st231_isa.cpp"
)
{

#line 13 "sim_loadstore.isa"
{ return nlidest; }
#line 20192 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 20197 "cpu/st231_isa.cpp"
OpLdb::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 20201 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 20205 "cpu/st231_isa.cpp"
)
{

#line 36 "sim_loadstore.isa"
{ return 1; }
#line 20211 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdb(st231::CodeType code, uint32_t addr)
{
	return new OpLdb(code, addr);
}

// op ldhu_d(stop[1]:rsv[1]:0b10[2]:0b00101[5]:null[2]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 20223 "cpu/st231_isa.cpp"
OpLdhu_d::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 20227 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 20231 "cpu/st231_isa.cpp"
)
{

#line 187 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        uint16_t result;

	if( cpu->IsDBreakHit(ea) )
   	    cpu->ThrowDBreak(ea);
	if( cpu->IsCRegSpace(ea) == 0 ) 
	    cpu->DisReadCheckMemory16(ea);

   
	if( cpu->IsCRegSpace(ea) )
            result = 0;
        else
	    result = cpu->DisReadMemory16(ea);

        // compute the effective address
        cpu->SetEA(ea);

        cpu->SetGPR_N(nlidest, result);
        
}
#line 20259 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 20264 "cpu/st231_isa.cpp"
OpLdhu_d::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 20268 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 20272 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 20277 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 20281 "cpu/st231_isa.cpp"
)
{

#line 211 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldhu_d $r" << nlidest << " = " << op2 <<"[ $r" <<rsc1 << " ]";
}
#line 20290 "cpu/st231_isa.cpp"
}
/* === Load Bytes INSTRUCTIONS ========================================= */

void
OpLdhu_d::initialize_operands()
{

#line 320 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 20304 "cpu/st231_isa.cpp"
}

void
OpLdhu_d::initialize_latencies()
{

#line 177 "sim_latencies.isa"
{LAT(3);}
#line 20313 "cpu/st231_isa.cpp"
}

void
OpLdhu_d::initialize_function()
{

#line 48 "sim_functions.isa"
{ function = FnLoad; }
#line 20322 "cpu/st231_isa.cpp"
}

#line 4 "sim_loadstore.isa"
int
#line 20327 "cpu/st231_isa.cpp"
OpLdhu_d::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 20331 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 20335 "cpu/st231_isa.cpp"
)
{

#line 13 "sim_loadstore.isa"
{ return nlidest; }
#line 20341 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 20346 "cpu/st231_isa.cpp"
OpLdhu_d::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 20350 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 20354 "cpu/st231_isa.cpp"
)
{

#line 35 "sim_loadstore.isa"
{ return 2; }
#line 20360 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdhu_d(st231::CodeType code, uint32_t addr)
{
	return new OpLdhu_d(code, addr);
}

// op ldhu(stop[1]:rsv[1]:0b10[2]:0b00100[5]:null[2]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 20372 "cpu/st231_isa.cpp"
OpLdhu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 20376 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 20380 "cpu/st231_isa.cpp"
)
{

#line 160 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        uint16_t result;

	if(cpu->IsDBreakHit(ea))
   	    cpu->ThrowDBreak(ea);
	if(cpu->IsCRegSpace(ea))
   	    cpu->ThrowCRegAccessViolation(ea);
	else
	    cpu->ReadCheckMemory16(ea);
   
	result = cpu->ReadMemory16(ea);

        // compute the effective address
        cpu->SetEA(ea);

        cpu->SetGPR_N(nlidest, result);
}
#line 20405 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 20410 "cpu/st231_isa.cpp"
OpLdhu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 20414 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 20418 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 20423 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 20427 "cpu/st231_isa.cpp"
)
{

#line 181 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldhu $r" << nlidest << " = " << op2 <<"[ $r" <<rsc1 << " ]";
}
#line 20436 "cpu/st231_isa.cpp"
}
/* === Load Bytes INSTRUCTIONS ========================================= */

void
OpLdhu::initialize_operands()
{

#line 320 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 20450 "cpu/st231_isa.cpp"
}

void
OpLdhu::initialize_latencies()
{

#line 176 "sim_latencies.isa"
{LAT(3);}
#line 20459 "cpu/st231_isa.cpp"
}

void
OpLdhu::initialize_function()
{

#line 48 "sim_functions.isa"
{ function = FnLoad; }
#line 20468 "cpu/st231_isa.cpp"
}

#line 4 "sim_loadstore.isa"
int
#line 20473 "cpu/st231_isa.cpp"
OpLdhu::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 20477 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 20481 "cpu/st231_isa.cpp"
)
{

#line 13 "sim_loadstore.isa"
{ return nlidest; }
#line 20487 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 20492 "cpu/st231_isa.cpp"
OpLdhu::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 20496 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 20500 "cpu/st231_isa.cpp"
)
{

#line 34 "sim_loadstore.isa"
{ return 2; }
#line 20506 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdhu(st231::CodeType code, uint32_t addr)
{
	return new OpLdhu(code, addr);
}

// op ldh_d(stop[1]:rsv[1]:0b10[2]:0b00011[5]:null[2]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 20518 "cpu/st231_isa.cpp"
OpLdh_d::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 20522 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 20526 "cpu/st231_isa.cpp"
)
{

#line 130 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        int16_t result;

	if( cpu->IsDBreakHit(ea) )
   	    cpu->ThrowDBreak(ea);
	if( cpu->IsCRegSpace(ea) == 0 ) 
	    cpu->DisReadCheckMemory16(ea);

   
	if( cpu->IsCRegSpace(ea) )
            result = 0;
        else
	    result = cpu->DisReadMemory16(ea);

        // compute the effective address
        cpu->SetEA(ea);

        cpu->SetGPR_N(nlidest, result);
        
}
#line 20554 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 20559 "cpu/st231_isa.cpp"
OpLdh_d::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 20563 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 20567 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 20572 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 20576 "cpu/st231_isa.cpp"
)
{

#line 154 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldh_d $r" << nlidest << " = " << op2 <<"[ $r" <<rsc1 << " ]";
}
#line 20585 "cpu/st231_isa.cpp"
}
/* === Load Bytes INSTRUCTIONS ========================================= */

void
OpLdh_d::initialize_operands()
{

#line 320 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 20599 "cpu/st231_isa.cpp"
}

void
OpLdh_d::initialize_latencies()
{

#line 175 "sim_latencies.isa"
{LAT(3);}
#line 20608 "cpu/st231_isa.cpp"
}

void
OpLdh_d::initialize_function()
{

#line 48 "sim_functions.isa"
{ function = FnLoad; }
#line 20617 "cpu/st231_isa.cpp"
}

#line 4 "sim_loadstore.isa"
int
#line 20622 "cpu/st231_isa.cpp"
OpLdh_d::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 20626 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 20630 "cpu/st231_isa.cpp"
)
{

#line 13 "sim_loadstore.isa"
{ return nlidest; }
#line 20636 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 20641 "cpu/st231_isa.cpp"
OpLdh_d::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 20645 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 20649 "cpu/st231_isa.cpp"
)
{

#line 33 "sim_loadstore.isa"
{ return 2; }
#line 20655 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdh_d(st231::CodeType code, uint32_t addr)
{
	return new OpLdh_d(code, addr);
}

// op ldh(stop[1]:rsv[1]:0b10[2]:0b00010[5]:null[2]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 20667 "cpu/st231_isa.cpp"
OpLdh::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 20671 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 20675 "cpu/st231_isa.cpp"
)
{

#line 103 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        int16_t result;

	if(cpu->IsDBreakHit(ea))
   	    cpu->ThrowDBreak(ea);
	if(cpu->IsCRegSpace(ea))
   	    cpu->ThrowCRegAccessViolation(ea);
	else
	    cpu->ReadCheckMemory16(ea);
   
	result = cpu->ReadMemory16(ea);

        // compute the effective address
        cpu->SetEA(ea);

        cpu->SetGPR_N(nlidest, result);
}
#line 20700 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 20705 "cpu/st231_isa.cpp"
OpLdh::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 20709 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 20713 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 20718 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 20722 "cpu/st231_isa.cpp"
)
{

#line 124 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldh $r" << nlidest << " = " << op2 <<"[ $r" <<rsc1 << " ]";
}
#line 20731 "cpu/st231_isa.cpp"
}
/* === Load Bytes INSTRUCTIONS ========================================= */

void
OpLdh::initialize_operands()
{

#line 320 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 20745 "cpu/st231_isa.cpp"
}

void
OpLdh::initialize_latencies()
{

#line 174 "sim_latencies.isa"
{LAT(3);}
#line 20754 "cpu/st231_isa.cpp"
}

void
OpLdh::initialize_function()
{

#line 48 "sim_functions.isa"
{ function = FnLoad; }
#line 20763 "cpu/st231_isa.cpp"
}

#line 4 "sim_loadstore.isa"
int
#line 20768 "cpu/st231_isa.cpp"
OpLdh::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 20772 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 20776 "cpu/st231_isa.cpp"
)
{

#line 13 "sim_loadstore.isa"
{ return nlidest; }
#line 20782 "cpu/st231_isa.cpp"
}

#line 23 "sim_loadstore.isa"
int
#line 20787 "cpu/st231_isa.cpp"
OpLdh::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 20791 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 20795 "cpu/st231_isa.cpp"
)
{

#line 32 "sim_loadstore.isa"
{ return 2; }
#line 20801 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdh(st231::CodeType code, uint32_t addr)
{
	return new OpLdh(code, addr);
}

// op ldw_d(stop[1]:rsv[1]:0b10[2]:0b00001[5]:null[2]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 20813 "cpu/st231_isa.cpp"
OpLdw_d::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 20817 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 20821 "cpu/st231_isa.cpp"
)
{

#line 70 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        int32_t result;

	if( cpu->IsDBreakHit(ea) )
   	    cpu->ThrowDBreak(ea);
	if( cpu->IsCRegSpace(ea) == 0 )
	{ 
	    cpu->DisReadCheckMemory32(ea);
	    result = cpu->DisReadMemory32(ea);
        }
        else
            result = 0;
   
//	if( cpu->IsCRegSpace(ea) )
//	    result = 0;
//	else
//	    result = cpu->DisReadMemory32(ea);

        // compute the effective address
        cpu->SetEA(ea);

        cpu->SetGPR_N(idest, result);
}
#line 20852 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 20857 "cpu/st231_isa.cpp"
OpLdw_d::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 20861 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 20865 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 20870 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 20874 "cpu/st231_isa.cpp"
)
{

#line 97 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldw_d $r" << idest << " = " << op2 << "[ $r" <<rsc1 << " ]";
}
#line 20883 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === LOAD/STORE INSTRUCTIONS ============================================= */
/*****************************************************************************/
/* === Load Word INSTRUCTIONS ========================================== */

void
OpLdw_d::initialize_operands()
{

#line 304 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 20900 "cpu/st231_isa.cpp"
}

void
OpLdw_d::initialize_latencies()
{

#line 173 "sim_latencies.isa"
{if(idest==63) LAT(4); else LAT(3);}
#line 20909 "cpu/st231_isa.cpp"
}

void
OpLdw_d::initialize_function()
{

#line 47 "sim_functions.isa"
{ function = FnLoad; }
#line 20918 "cpu/st231_isa.cpp"
}
//Load word & Load bytes

#line 4 "sim_loadstore.isa"
int
#line 20924 "cpu/st231_isa.cpp"
OpLdw_d::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 20928 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 20932 "cpu/st231_isa.cpp"
)
{

#line 12 "sim_loadstore.isa"
{ return idest; }
#line 20938 "cpu/st231_isa.cpp"
}
//Load word & Load bytes

#line 23 "sim_loadstore.isa"
int
#line 20944 "cpu/st231_isa.cpp"
OpLdw_d::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 20948 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 20952 "cpu/st231_isa.cpp"
)
{

#line 31 "sim_loadstore.isa"
{ return 4; }
#line 20958 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdw_d(st231::CodeType code, uint32_t addr)
{
	return new OpLdw_d(code, addr);
}

//*********************************************************************************
// the memory operations : load, store
//*********************************************************************************
// op ldw(stop[1]:rsv[1]:0b10[2]:0b00000[5]:null[2]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 20973 "cpu/st231_isa.cpp"
OpLdw::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 20977 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 20981 "cpu/st231_isa.cpp"
)
{

#line 33 "memory.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        uint32_t ea = op1 + op2;
        int32_t result;

	if(cpu->IsDBreakHit(ea))
   	    cpu->ThrowDBreak(ea);
	if(cpu->IsCRegSpace(ea))
	{
	    cpu->ReadCheckCReg(ea);
	    result = cpu->ReadCReg(ea);
	}
	else
	{
	    cpu->ReadCheckMemory32(ea);
	    result = cpu->ReadMemory32(ea);
	}
   
        //if(cpu->IsCRegSpace(ea))
        //    result = cpu->ReadCReg(ea);
        //else
        //    result = cpu->ReadMemory32(ea);

        // compute the effective address
        cpu->SetEA(ea);
        
        cpu->SetGPR_N(idest, result);
        
}
#line 21016 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21021 "cpu/st231_isa.cpp"
OpLdw::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21025 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21029 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21034 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21038 "cpu/st231_isa.cpp"
)
{

#line 64 "memory.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ldw $r" << idest << " = " <<hex<< op2 <<"[ $r" <<dec<<rsc1 << " ]    // " <<cpu->GetGPR_N(idest) <<"= mem[" <<hex <<op2 <<"+ r" <<dec<<rsc1<<"]" <<"[" <<hex<<op2<<"+"<<cpu->GetGPR_C(rsc1)<<"]"<<dec;
}
#line 21047 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === LOAD/STORE INSTRUCTIONS ============================================= */
/*****************************************************************************/
/* === Load Word INSTRUCTIONS ========================================== */

void
OpLdw::initialize_operands()
{

#line 304 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 21064 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === LOAD/STORE INSTRUCTIONS ============================================= */
/*****************************************************************************/

void
OpLdw::initialize_latencies()
{

#line 172 "sim_latencies.isa"
{if (idest==63) LAT(4); else LAT(3);}
#line 21076 "cpu/st231_isa.cpp"
}

void
OpLdw::initialize_function()
{

#line 47 "sim_functions.isa"
{ function = FnLoad; }
#line 21085 "cpu/st231_isa.cpp"
}
//Load word & Load bytes

#line 4 "sim_loadstore.isa"
int
#line 21091 "cpu/st231_isa.cpp"
OpLdw::loadstore_target_register(
#line 4 "sim_loadstore.isa"
	CPU *
#line 21095 "cpu/st231_isa.cpp"

#line 4 "sim_loadstore.isa"
	cpu
#line 21099 "cpu/st231_isa.cpp"
)
{

#line 12 "sim_loadstore.isa"
{ return idest; }
#line 21105 "cpu/st231_isa.cpp"
}
//Load word & Load bytes

#line 23 "sim_loadstore.isa"
int
#line 21111 "cpu/st231_isa.cpp"
OpLdw::memory_access_size(
#line 23 "sim_loadstore.isa"
	CPU *
#line 21115 "cpu/st231_isa.cpp"

#line 23 "sim_loadstore.isa"
	cpu
#line 21119 "cpu/st231_isa.cpp"
)
{

#line 31 "sim_loadstore.isa"
{ return 4; }
#line 21125 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpLdw(st231::CodeType code, uint32_t addr)
{
	return new OpLdw(code, addr);
}

// op break(stop[1]:rsv[1]:0b01[2]:0b1111111[7]:sbrknum[21])

#line 53 "isa/st231.isa"
void
#line 21137 "cpu/st231_isa.cpp"
OpBreak::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21141 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 21145 "cpu/st231_isa.cpp"
)
{

#line 188 "specific.isa"
{
       cpu->SetException(cpu->GetException()|ILL_INST_EXCEPTION);
       // operand1 = sbrknum ????
}
#line 21154 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21159 "cpu/st231_isa.cpp"
OpBreak::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21163 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21167 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21172 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21176 "cpu/st231_isa.cpp"
)
{

#line 193 "specific.isa"
{
	os << "break "<< sbrknum ;
}
#line 21184 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === No dependences Operation INSTRUCTIONS =============================== */
/*****************************************************************************/

void
OpBreak::initialize_operands()
{

#line 382 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 21198 "cpu/st231_isa.cpp"
}

void
OpBreak::initialize_latencies()
{

#line 207 "sim_latencies.isa"
{LAT(1);}
#line 21207 "cpu/st231_isa.cpp"
}

void
OpBreak::initialize_function()
{

#line 42 "sim_functions.isa"
{ function = FnIntBasic; }
#line 21216 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBreak(st231::CodeType code, uint32_t addr)
{
	return new OpBreak(code, addr);
}

// op syscall(stop[1]:rsv[1]:0b01[2]:0b1111110[7]:sbrknum[21])

#line 53 "isa/st231.isa"
void
#line 21228 "cpu/st231_isa.cpp"
OpSyscall::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21232 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 21236 "cpu/st231_isa.cpp"
)
{

#line 178 "specific.isa"
{
       cpu->SetException(cpu->GetException()|SYSCALL_EXCEPTION);
       // operand1 = sbrknum ????
}
#line 21245 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21250 "cpu/st231_isa.cpp"
OpSyscall::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21254 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21258 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21263 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21267 "cpu/st231_isa.cpp"
)
{

#line 183 "specific.isa"
{
	os << "syscall "<< sbrknum ;
}
#line 21275 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === No dependences Operation INSTRUCTIONS =============================== */
/*****************************************************************************/

void
OpSyscall::initialize_operands()
{

#line 382 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 21289 "cpu/st231_isa.cpp"
}

void
OpSyscall::initialize_latencies()
{

#line 206 "sim_latencies.isa"
{LAT(1);}
#line 21298 "cpu/st231_isa.cpp"
}

void
OpSyscall::initialize_function()
{

#line 42 "sim_functions.isa"
{ function = FnIntBasic; }
#line 21307 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSyscall(st231::CodeType code, uint32_t addr)
{
	return new OpSyscall(code, addr);
}

// op sbrk(stop[1]:rsv[1]:0b01[2]:0b1111101[7]:sbrknum[21])

#line 53 "isa/st231.isa"
void
#line 21319 "cpu/st231_isa.cpp"
OpSbrk::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21323 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 21327 "cpu/st231_isa.cpp"
)
{

#line 168 "specific.isa"
{
       cpu->SetException(cpu->GetException()|SBREAK_EXCEPTION);
       // operand1 = sbrknum ????
}
#line 21336 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21341 "cpu/st231_isa.cpp"
OpSbrk::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21345 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21349 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21354 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21358 "cpu/st231_isa.cpp"
)
{

#line 173 "specific.isa"
{
	os << "sbrk "<< sbrknum ;
}
#line 21366 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === No dependences Operation INSTRUCTIONS =============================== */
/*****************************************************************************/

void
OpSbrk::initialize_operands()
{

#line 382 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 21380 "cpu/st231_isa.cpp"
}

void
OpSbrk::initialize_latencies()
{

#line 205 "sim_latencies.isa"
{LAT(1);}
#line 21389 "cpu/st231_isa.cpp"
}

void
OpSbrk::initialize_function()
{

#line 42 "sim_functions.isa"
{ function = FnIntBasic; }
#line 21398 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSbrk(st231::CodeType code, uint32_t addr)
{
	return new OpSbrk(code, addr);
}

// op prgins(stop[1]:rsv[1]:0b01[2]:0b1111100[7]:sbrknum[21])

#line 53 "isa/st231.isa"
void
#line 21410 "cpu/st231_isa.cpp"
OpPrgins::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21414 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 21418 "cpu/st231_isa.cpp"
)
{

#line 159 "specific.isa"
{
	cpu->invalidate_icache();
}
#line 21426 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21431 "cpu/st231_isa.cpp"
OpPrgins::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21435 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21439 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21444 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21448 "cpu/st231_isa.cpp"
)
{

#line 163 "specific.isa"
{
	os << "prgins";
}
#line 21456 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === No dependences Operation INSTRUCTIONS =============================== */
/*****************************************************************************/

void
OpPrgins::initialize_operands()
{

#line 382 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 21470 "cpu/st231_isa.cpp"
}

void
OpPrgins::initialize_latencies()
{

#line 204 "sim_latencies.isa"
{LAT(1);}
#line 21479 "cpu/st231_isa.cpp"
}

void
OpPrgins::initialize_function()
{

#line 42 "sim_functions.isa"
{ function = FnIntBasic; }
#line 21488 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpPrgins(st231::CodeType code, uint32_t addr)
{
	return new OpPrgins(code, addr);
}

// op islctf(stop[1]:rsv[1]:0b01[2]:0b1001[4]:scond[3]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 21500 "cpu/st231_isa.cpp"
OpIslctf::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21504 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 21508 "cpu/st231_isa.cpp"
)
{

#line 138 "specific.isa"
{
        uint8_t op1 = cpu->GetGPB_C(scond);
        int32_t op2 = cpu->GetGPR_C(rsc1);
        int32_t op3 = cpu->Imm(isrc2);
        int32_t result;

        if(op1)
          result = op3;
        else
          result = op2;

        cpu->SetGPR_N(idest, result);
}
#line 21526 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21531 "cpu/st231_isa.cpp"
OpIslctf::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21535 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21539 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21544 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21548 "cpu/st231_isa.cpp"
)
{

#line 152 "specific.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "slctf $r" << idest << " = $b" << scond <<", $r" <<rsc1 << ", " << op2 ;
}
#line 21557 "cpu/st231_isa.cpp"
}
/* === Immediant Select Operation INSTRUCTIONS ========================= */

void
OpIslctf::initialize_operands()
{

#line 276 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPB_T, scond);
  ST231_OUTPUT(GPR_T, idest);
}
#line 21572 "cpu/st231_isa.cpp"
}

void
OpIslctf::initialize_latencies()
{

#line 194 "sim_latencies.isa"
{LAT(1);}
#line 21581 "cpu/st231_isa.cpp"
}

void
OpIslctf::initialize_function()
{

#line 38 "sim_functions.isa"
{ function = FnIntBasic; }
#line 21590 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIslctf(st231::CodeType code, uint32_t addr)
{
	return new OpIslctf(code, addr);
}

// op islct(stop[1]:rsv[1]:0b01[2]:0b1000[4]:scond[3]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 21602 "cpu/st231_isa.cpp"
OpIslct::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21606 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 21610 "cpu/st231_isa.cpp"
)
{

#line 118 "specific.isa"
{
        uint8_t op1 = cpu->GetGPB_C(scond);
        int32_t op2 = cpu->GetGPR_C(rsc1);
        int32_t op3 = cpu->Imm(isrc2);
        int32_t result;

        if(op1)
          result = op2;
        else
          result = op3;

        cpu->SetGPR_N(idest, result);
}
#line 21628 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21633 "cpu/st231_isa.cpp"
OpIslct::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21637 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21641 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21646 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21650 "cpu/st231_isa.cpp"
)
{

#line 132 "specific.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "slct $r" << idest << " = $b" << scond <<", $r" <<rsc1 << ", " << op2 ;
}
#line 21659 "cpu/st231_isa.cpp"
}
/* === Immediant Select Operation INSTRUCTIONS ========================= */

void
OpIslct::initialize_operands()
{

#line 276 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPB_T, scond);
  ST231_OUTPUT(GPR_T, idest);
}
#line 21674 "cpu/st231_isa.cpp"
}

void
OpIslct::initialize_latencies()
{

#line 193 "sim_latencies.isa"
{LAT(1);}
#line 21683 "cpu/st231_isa.cpp"
}

void
OpIslct::initialize_function()
{

#line 38 "sim_functions.isa"
{ function = FnIntBasic; }
#line 21692 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIslct(st231::CodeType code, uint32_t addr)
{
	return new OpIslct(code, addr);
}

// op immr(stop[1]:rsv[1]:0b01[2]:0b01011[5]:imm[23])

#line 53 "isa/st231.isa"
void
#line 21704 "cpu/st231_isa.cpp"
OpImmr::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21708 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 21712 "cpu/st231_isa.cpp"
)
{

#line 110 "specific.isa"
{
}
#line 21719 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21724 "cpu/st231_isa.cpp"
OpImmr::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21728 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21732 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21737 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21741 "cpu/st231_isa.cpp"
)
{

#line 112 "specific.isa"
{
	os << "immr" << imm ;
}
#line 21749 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === No dependences Operation INSTRUCTIONS =============================== */
/*****************************************************************************/

void
OpImmr::initialize_operands()
{

#line 382 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 21763 "cpu/st231_isa.cpp"
}

void
OpImmr::initialize_latencies()
{

#line 203 "sim_latencies.isa"
{LAT(1);}
#line 21772 "cpu/st231_isa.cpp"
}

void
OpImmr::initialize_function()
{

#line 42 "sim_functions.isa"
{ function = FnIntBasic; }
#line 21781 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImmr(st231::CodeType code, uint32_t addr)
{
	return new OpImmr(code, addr);
}

// immediate extension
// op imml(stop[1]:rsv[1]:0b01[2]:0b01010[5]:imm[23])

#line 53 "isa/st231.isa"
void
#line 21794 "cpu/st231_isa.cpp"
OpImml::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21798 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 21802 "cpu/st231_isa.cpp"
)
{

#line 103 "specific.isa"
{
}
#line 21809 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21814 "cpu/st231_isa.cpp"
OpImml::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21818 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21822 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21827 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21831 "cpu/st231_isa.cpp"
)
{

#line 105 "specific.isa"
{
	os << "imml " << imm ;
}
#line 21839 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === No dependences Operation INSTRUCTIONS =============================== */
/*****************************************************************************/

void
OpImml::initialize_operands()
{

#line 382 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 21853 "cpu/st231_isa.cpp"
}

void
OpImml::initialize_latencies()
{

#line 202 "sim_latencies.isa"
{LAT(1);}
#line 21862 "cpu/st231_isa.cpp"
}

void
OpImml::initialize_function()
{

#line 42 "sim_functions.isa"
{ function = FnIntBasic; }
#line 21871 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImml(st231::CodeType code, uint32_t addr)
{
	return new OpImml(code, addr);
}

// op divs(stop[1]:rsv[1]:0b01[2]:0b0100[4]:scond[3]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 21883 "cpu/st231_isa.cpp"
OpDivs::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21887 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 21891 "cpu/st231_isa.cpp"
)
{

#line 76 "specific.isa"
{
        int32_t result1;
        uint8_t result2;
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        uint8_t op3 = cpu->GetGPB_C(scond);
        uint32_t tmp = (op1*2) | op3;
       
        if(op1<0)
        {
          result1 = tmp + op2;
          result2 = 1;
        }
        else
        {
          result1 = tmp - op2;
          result2 = 0;
        }
        cpu->SetGPR_N(dest, result1);
        cpu->SetGPB_N(bdest, result2);
}
#line 21917 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 21922 "cpu/st231_isa.cpp"
OpDivs::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 21926 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 21930 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 21935 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 21939 "cpu/st231_isa.cpp"
)
{

#line 98 "specific.isa"
{
	os << "divs $r" << dest <<", $b" <<bdest << " = $r" <<rsc1 << ", $r" << rsc2 <<", $b" << scond ;
}
#line 21947 "cpu/st231_isa.cpp"
}
/* === 5 Operands Operation INSTRUCTIONS =============================== */

void
OpDivs::initialize_operands()
{

#line 287 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_INPUT(GPB_T, scond);
  ST231_OUTPUT(GPR_T, dest);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 21964 "cpu/st231_isa.cpp"
}

void
OpDivs::initialize_latencies()
{

#line 119 "sim_latencies.isa"
{LAT(1);}
#line 21973 "cpu/st231_isa.cpp"
}

void
OpDivs::initialize_function()
{

#line 39 "sim_functions.isa"
{ function = FnIntBasic; }
#line 21982 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpDivs(st231::CodeType code, uint32_t addr)
{
	return new OpDivs(code, addr);
}

// op addcg(stop[1]:rsv[1]:0b01[2]:0b0010[4]:scond[3]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 21994 "cpu/st231_isa.cpp"
OpAddcg::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 21998 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22002 "cpu/st231_isa.cpp"
)
{

#line 61 "specific.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        uint8_t op3 = cpu->GetGPB_C(scond);
        uint32_t result = op1+op2+op3;

        cpu->SetGPR_N(dest, result);
        cpu->SetGPB_N(bdest, (result&0x80000000) >> 31);
}
#line 22016 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22021 "cpu/st231_isa.cpp"
OpAddcg::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22025 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22029 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22034 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22038 "cpu/st231_isa.cpp"
)
{

#line 71 "specific.isa"
{
	os << "addcg $r" << dest <<", $b" <<bdest << " = $r" <<rsc1 << ", $r" << rsc2 <<", $b" << scond ;
}
#line 22046 "cpu/st231_isa.cpp"
}
/* === 5 Operands Operation INSTRUCTIONS =============================== */

void
OpAddcg::initialize_operands()
{

#line 287 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_INPUT(GPB_T, scond);
  ST231_OUTPUT(GPR_T, dest);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 22063 "cpu/st231_isa.cpp"
}

void
OpAddcg::initialize_latencies()
{

#line 118 "sim_latencies.isa"
{LAT(1);}
#line 22072 "cpu/st231_isa.cpp"
}

void
OpAddcg::initialize_function()
{

#line 39 "sim_functions.isa"
{ function = FnIntBasic; }
#line 22081 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpAddcg(st231::CodeType code, uint32_t addr)
{
	return new OpAddcg(code, addr);
}

// op slctf(stop[1]:rsv[1]:0b01[2]:0b0001[4]:scond[3]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 22093 "cpu/st231_isa.cpp"
OpSlctf::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22097 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22101 "cpu/st231_isa.cpp"
)
{

#line 42 "specific.isa"
{
        uint8_t op1 = cpu->GetGPB_C(scond);
        int32_t op2 = cpu->GetGPR_C(rsc1);
        int32_t op3 = cpu->GetGPR_C(rsc2);
        int32_t result;

        if(op1)
          result = op3;
        else
          result = op2;

        cpu->SetGPR_N(dest, result);
}
#line 22119 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22124 "cpu/st231_isa.cpp"
OpSlctf::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22128 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22132 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22137 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22141 "cpu/st231_isa.cpp"
)
{

#line 56 "specific.isa"
{
	os << "slctf $r" << dest << " = $b" << scond <<", $r" <<rsc1 << ", $r" << rsc2 ;
}
#line 22149 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* = specific operations : immediate extension,selects,extended arithmetic = */
/*****************************************************************************/
/* === Register Select Operation INSTRUCTIONS ========================== */

void
OpSlctf::initialize_operands()
{

#line 264 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_INPUT(GPB_T, scond);
  ST231_OUTPUT(GPR_T, dest);
}
#line 22168 "cpu/st231_isa.cpp"
}

void
OpSlctf::initialize_latencies()
{

#line 192 "sim_latencies.isa"
{LAT(1);}
#line 22177 "cpu/st231_isa.cpp"
}

void
OpSlctf::initialize_function()
{

#line 37 "sim_functions.isa"
{ function = FnIntBasic; }
#line 22186 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSlctf(st231::CodeType code, uint32_t addr)
{
	return new OpSlctf(code, addr);
}

//*********************************************************************************
// the specific operations : immediate extension, selects, extended arithmetic
//*********************************************************************************
// op slct(stop[1]:rsv[1]:0b01[2]:0b0000[4]:scond[3]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 22201 "cpu/st231_isa.cpp"
OpSlct::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22205 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22209 "cpu/st231_isa.cpp"
)
{

#line 23 "specific.isa"
{
        uint8_t op1 = cpu->GetGPB_C(scond);
        int32_t op2 = cpu->GetGPR_C(rsc1);
        int32_t op3 = cpu->GetGPR_C(rsc2);
        int32_t result;

        if(op1)
          result = op2;
        else
          result = op3;

        cpu->SetGPR_N(dest, result);
}
#line 22227 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22232 "cpu/st231_isa.cpp"
OpSlct::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22236 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22240 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22245 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22249 "cpu/st231_isa.cpp"
)
{

#line 37 "specific.isa"
{
	os << "slct $r" << dest << " = $b" << scond <<", $r" <<rsc1 << ", $r" << rsc2 ;
}
#line 22257 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* = specific operations : immediate extension,selects,extended arithmetic = */
/*****************************************************************************/
/* === Register Select Operation INSTRUCTIONS ========================== */

void
OpSlct::initialize_operands()
{

#line 264 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_INPUT(GPB_T, scond);
  ST231_OUTPUT(GPR_T, dest);
}
#line 22276 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === specific operations & specific control instructions ================= */
/*****************************************************************************/

void
OpSlct::initialize_latencies()
{

#line 191 "sim_latencies.isa"
{LAT(1);}
#line 22288 "cpu/st231_isa.cpp"
}

void
OpSlct::initialize_function()
{

#line 37 "sim_functions.isa"
{ function = FnIntBasic; }
#line 22297 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSlct(st231::CodeType code, uint32_t addr)
{
	return new OpSlct(code, addr);
}

// op brf(stop[1]:rsv[1]:0b1111[4]:bcond[3]:btarg[23])

#line 53 "isa/st231.isa"
void
#line 22309 "cpu/st231_isa.cpp"
OpBrf::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22313 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22317 "cpu/st231_isa.cpp"
)
{

#line 90 "ctrltrans.isa"
{
        uint8_t op1 = cpu->GetGPB_C(bcond);
        uint32_t op2 = (SignEx23to32(btarg))<<2;

        if(op1 == 0)
            cpu->SetNia(op2 + cpu->GetCia());

}
#line 22330 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22335 "cpu/st231_isa.cpp"
OpBrf::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22339 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22343 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22348 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22352 "cpu/st231_isa.cpp"
)
{

#line 99 "ctrltrans.isa"
{
        os <<"brf $b" <<bcond <<", "<< btarg;
}
#line 22360 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === THE CONTROL TRANSFER OPERATIONS : branch, call, rfi, goto =========== */
/*****************************************************************************/
/* === Branch INSTRUCTIONS ============================================= */

void
OpBrf::initialize_operands()
{

#line 365 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPB_T, bcond);
}
#line 22376 "cpu/st231_isa.cpp"
}

void
OpBrf::initialize_latencies()
{

#line 163 "sim_latencies.isa"
{LAT(1);}
#line 22385 "cpu/st231_isa.cpp"
}

void
OpBrf::initialize_function()
{

#line 51 "sim_functions.isa"
{ function = FnBranch; }
#line 22394 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBrf(st231::CodeType code, uint32_t addr)
{
	return new OpBrf(code, addr);
}

// op br(stop[1]:rsv[1]:0b1110[4]:bcond[3]:btarg[23])

#line 53 "isa/st231.isa"
void
#line 22406 "cpu/st231_isa.cpp"
OpBr::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22410 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22414 "cpu/st231_isa.cpp"
)
{

#line 77 "ctrltrans.isa"
{
        uint8_t op1 = cpu->GetGPB_C(bcond);
        uint32_t op2 = (SignEx23to32(btarg))<<2;

        if(op1)
            cpu->SetNia(op2 + cpu->GetCia());
}
#line 22426 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22431 "cpu/st231_isa.cpp"
OpBr::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22435 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22439 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22444 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22448 "cpu/st231_isa.cpp"
)
{

#line 85 "ctrltrans.isa"
{
        os <<"br $b" <<bcond <<", "<<hex<< btarg << "  : bcond = "<< dec<< (int)cpu->GetGPB_C(bcond);
}
#line 22456 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === THE CONTROL TRANSFER OPERATIONS : branch, call, rfi, goto =========== */
/*****************************************************************************/
/* === Branch INSTRUCTIONS ============================================= */

void
OpBr::initialize_operands()
{

#line 365 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPB_T, bcond);
}
#line 22472 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === BRANCH INSTRUCTIONS ================================================= */
/*****************************************************************************/

void
OpBr::initialize_latencies()
{

#line 162 "sim_latencies.isa"
{LAT(1);}
#line 22484 "cpu/st231_isa.cpp"
}

void
OpBr::initialize_function()
{

#line 51 "sim_functions.isa"
{ function = FnBranch; }
#line 22493 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBr(st231::CodeType code, uint32_t addr)
{
	return new OpBr(code, addr);
}

// op rfi(stop[1]:rsv[1]:0b11[2]:  /* Possible super/user transition  */

#line 53 "isa/st231.isa"
void
#line 22505 "cpu/st231_isa.cpp"
OpRfi::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22509 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22513 "cpu/st231_isa.cpp"
)
{

#line 58 "ctrltrans.isa"
{

  cpu->invalidate_last_tlb_cache();
  if(PSW_USER_MODE) {
    cpu->ThrowIllInst();
  }

  cpu->SetNia(cpu->ReadMemory32(SAVED_PC));
  cpu->WriteMemory32(PSW,cpu->ReadMemory32(SAVED_PSW));

  cpu->WriteMemory32(SAVED_PC,cpu->ReadMemory32(SAVED_SAVED_PC));
  cpu->WriteMemory32(SAVED_PSW,cpu->ReadMemory32(SAVED_SAVED_PSW));
}
#line 22531 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22536 "cpu/st231_isa.cpp"
OpRfi::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22540 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22544 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22549 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22553 "cpu/st231_isa.cpp"
)
{

#line 72 "ctrltrans.isa"
{
	os << "rfi " ;
}
#line 22561 "cpu/st231_isa.cpp"
}

void
OpRfi::initialize_operands()
{

#line 390 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 22572 "cpu/st231_isa.cpp"
}

void
OpRfi::initialize_latencies()
{

#line 209 "sim_latencies.isa"
{LAT(1);}
#line 22581 "cpu/st231_isa.cpp"
}

void
OpRfi::initialize_function()
{

#line 52 "sim_functions.isa"
{ function = FnBranch; }
#line 22590 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpRfi(st231::CodeType code, uint32_t addr)
{
	return new OpRfi(code, addr);
}

// op goto(stop[1]:rsv[1]:0b11[2]:0b00011[5]:0b00000000000000000000000[23])

#line 53 "isa/st231.isa"
void
#line 22602 "cpu/st231_isa.cpp"
OpGoto::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22606 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22610 "cpu/st231_isa.cpp"
)
{

#line 49 "ctrltrans.isa"
{
        cpu->SetNia( cpu->GetGPR_C(63));
}
#line 22618 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22623 "cpu/st231_isa.cpp"
OpGoto::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22627 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22631 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22636 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22640 "cpu/st231_isa.cpp"
)
{

#line 53 "ctrltrans.isa"
{
	os << "goto $r63 = " << hex << cpu->GetGPR_C(63) <<dec;
}
#line 22648 "cpu/st231_isa.cpp"
}

void
OpGoto::initialize_operands()
{

#line 397 "sim_dependencies.isa"
{
  ST231_INPUT(GPR_T, 63);
}
#line 22659 "cpu/st231_isa.cpp"
}

void
OpGoto::initialize_latencies()
{

#line 166 "sim_latencies.isa"
{LAT(1);}
#line 22668 "cpu/st231_isa.cpp"
}

void
OpGoto::initialize_function()
{

#line 53 "sim_functions.isa"
{ function = FnBranchLink; }
#line 22677 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpGoto(st231::CodeType code, uint32_t addr)
{
	return new OpGoto(code, addr);
}

// op igoto(stop[1]:rsv[1]:0b11[2]:0b00010[5]:btarg[23])

#line 53 "isa/st231.isa"
void
#line 22689 "cpu/st231_isa.cpp"
OpIgoto::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22693 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22697 "cpu/st231_isa.cpp"
)
{

#line 37 "ctrltrans.isa"
{
        int32_t op1 = (SignEx23to32(btarg))<<2;
        cpu->SetNia(op1 + cpu->GetCia());
}
#line 22706 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22711 "cpu/st231_isa.cpp"
OpIgoto::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22715 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22719 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22724 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22728 "cpu/st231_isa.cpp"
)
{

#line 42 "ctrltrans.isa"
{
        int32_t op1 = (SignEx23to32(btarg))<<2;
	os << "igoto " << hex << (op1 + cpu->GetCia()) <<dec;
}
#line 22737 "cpu/st231_isa.cpp"
}

void
OpIgoto::initialize_operands()
{

#line 390 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 22748 "cpu/st231_isa.cpp"
}

void
OpIgoto::initialize_latencies()
{

#line 167 "sim_latencies.isa"
{LAT(1);}
#line 22757 "cpu/st231_isa.cpp"
}

void
OpIgoto::initialize_function()
{

#line 52 "sim_functions.isa"
{ function = FnBranch; }
#line 22766 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIgoto(st231::CodeType code, uint32_t addr)
{
	return new OpIgoto(code, addr);
}

// op call(stop[1]:rsv[1]:0b11[2]:0b00001[5]:0b00000000000000000000000[23])

#line 53 "isa/st231.isa"
void
#line 22778 "cpu/st231_isa.cpp"
OpCall::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22782 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22786 "cpu/st231_isa.cpp"
)
{

#line 27 "ctrltrans.isa"
{
        cpu->SetGPR_N(63, cpu->GetNia());
        cpu->SetNia( cpu->GetGPR_C(63));
}
#line 22795 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22800 "cpu/st231_isa.cpp"
OpCall::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22804 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22808 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22813 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22817 "cpu/st231_isa.cpp"
)
{

#line 32 "ctrltrans.isa"
{
	os << "call $r63 = " << hex << cpu->GetGPR_C(63) <<dec;
}
#line 22825 "cpu/st231_isa.cpp"
}

void
OpCall::initialize_operands()
{

#line 397 "sim_dependencies.isa"
{
  ST231_INPUT(GPR_T, 63);
}
#line 22836 "cpu/st231_isa.cpp"
}

void
OpCall::initialize_latencies()
{

#line 165 "sim_latencies.isa"
{LAT(1);}
#line 22845 "cpu/st231_isa.cpp"
}

void
OpCall::initialize_function()
{

#line 53 "sim_functions.isa"
{ function = FnBranchLink; }
#line 22854 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCall(st231::CodeType code, uint32_t addr)
{
	return new OpCall(code, addr);
}

//*********************************************************************************
// the control transfer operations : branch, call, rfi, goto 
//*********************************************************************************
// op call(stop[1]:rsv[1]:0b11[2]:0b00000[5]:btarg[23])

#line 53 "isa/st231.isa"
void
#line 22869 "cpu/st231_isa.cpp"
OpIcall::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22873 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22877 "cpu/st231_isa.cpp"
)
{

#line 14 "ctrltrans.isa"
{
        int32_t op1 = (SignEx23to32(btarg))<<2;

        cpu->SetGPR_N(63, cpu->GetNia());
        cpu->SetNia(op1 + cpu->GetCia());
}
#line 22888 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22893 "cpu/st231_isa.cpp"
OpIcall::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22897 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22901 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 22906 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 22910 "cpu/st231_isa.cpp"
)
{

#line 21 "ctrltrans.isa"
{
        //int32_t op1 = SignEx23to32(btarg);
	os << "icall " << hex << cpu->GetNia() <<dec;
}
#line 22919 "cpu/st231_isa.cpp"
}

void
OpIcall::initialize_operands()
{

#line 390 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
}
#line 22930 "cpu/st231_isa.cpp"
}

void
OpIcall::initialize_latencies()
{

#line 164 "sim_latencies.isa"
{LAT(1);}
#line 22939 "cpu/st231_isa.cpp"
}

void
OpIcall::initialize_function()
{

#line 52 "sim_functions.isa"
{ function = FnBranch; }
#line 22948 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcall(st231::CodeType code, uint32_t addr)
{
	return new OpIcall(code, addr);
}

// op clz(stop[1]:rsv[1]:0b00[2]:0b1001110[7]:0b000000100[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 22960 "cpu/st231_isa.cpp"
OpClz::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 22964 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 22968 "cpu/st231_isa.cpp"
)
{

#line 1853 "integer.isa"
{
	int32_t i;
	uint32_t op1 = cpu->GetGPR_C(rsc1);

	for(i=31;i>=0;i--) {
	  if(op1&(1<<i)) break;
	}

        cpu->SetGPR_N(idest, 31-i );
}
#line 22983 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 22988 "cpu/st231_isa.cpp"
OpClz::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 22992 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 22996 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23001 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23005 "cpu/st231_isa.cpp"
)
{

#line 1864 "integer.isa"
{
	os << "clz $r" << idest << " = $r" << rsc1 ;
}
#line 23013 "cpu/st231_isa.cpp"
}
/* === Monadic Operation INSTRUCTIONS ================================== */

void
OpClz::initialize_operands()
{

#line 250 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 23027 "cpu/st231_isa.cpp"
}

void
OpClz::initialize_latencies()
{

#line 117 "sim_latencies.isa"
{LAT(1);}
#line 23036 "cpu/st231_isa.cpp"
}

void
OpClz::initialize_function()
{

#line 36 "sim_functions.isa"
{ function = FnIntBasic; }
#line 23045 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpClz(st231::CodeType code, uint32_t addr)
{
	return new OpClz(code, addr);
}

// op zxth(stop[1]:rsv[1]:0b00[2]:0b1001110[7]:0b000000011[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23057 "cpu/st231_isa.cpp"
OpZxth::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23061 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23065 "cpu/st231_isa.cpp"
)
{

#line 1843 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1) & 0x0000ffff;
        cpu->SetGPR_N(idest, op1 );
}
#line 23074 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23079 "cpu/st231_isa.cpp"
OpZxth::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23083 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23087 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23092 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23096 "cpu/st231_isa.cpp"
)
{

#line 1848 "integer.isa"
{
	os << "zxth $r" << idest << " = $r" << rsc1 ;
}
#line 23104 "cpu/st231_isa.cpp"
}
/* === Monadic Operation INSTRUCTIONS ================================== */

void
OpZxth::initialize_operands()
{

#line 250 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 23118 "cpu/st231_isa.cpp"
}

void
OpZxth::initialize_latencies()
{

#line 116 "sim_latencies.isa"
{LAT(1);}
#line 23127 "cpu/st231_isa.cpp"
}

void
OpZxth::initialize_function()
{

#line 36 "sim_functions.isa"
{ function = FnIntBasic; }
#line 23136 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpZxth(st231::CodeType code, uint32_t addr)
{
	return new OpZxth(code, addr);
}

// op bswap(stop[1]:rsv[1]:0b00[2]:0b1001110[7]:0b000000010[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23148 "cpu/st231_isa.cpp"
OpBswap::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23152 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23156 "cpu/st231_isa.cpp"
)
{

#line 1832 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = ((op1&0x000000ff) <<24 ) | ((op1&0x0000ff00) <<8 ) | ((op1&0x00ff0000) >>8 ) | ((op1&0xff000000) >>24 );
        cpu->SetGPR_N(idest, op2 );
}
#line 23166 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23171 "cpu/st231_isa.cpp"
OpBswap::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23175 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23179 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23184 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23188 "cpu/st231_isa.cpp"
)
{

#line 1838 "integer.isa"
{
	os << "bswap $r" << idest << " = $r" << rsc1 ;
}
#line 23196 "cpu/st231_isa.cpp"
}
/* === Monadic Operation INSTRUCTIONS ================================== */

void
OpBswap::initialize_operands()
{

#line 250 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 23210 "cpu/st231_isa.cpp"
}

void
OpBswap::initialize_latencies()
{

#line 115 "sim_latencies.isa"
{LAT(1);}
#line 23219 "cpu/st231_isa.cpp"
}

void
OpBswap::initialize_function()
{

#line 36 "sim_functions.isa"
{ function = FnIntBasic; }
#line 23228 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBswap(st231::CodeType code, uint32_t addr)
{
	return new OpBswap(code, addr);
}

// op sxth(stop[1]:rsv[1]:0b00[2]:0b1001110[7]:0b000000001[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23240 "cpu/st231_isa.cpp"
OpSxth::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23244 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23248 "cpu/st231_isa.cpp"
)
{

#line 1817 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        if (op1 & (1<<15))
          op1 |= 0xffff0000;
        else
          op1 &= 0x0000ffff;

        cpu->SetGPR_N(idest, op1 );
}
#line 23262 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23267 "cpu/st231_isa.cpp"
OpSxth::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23271 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23275 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23280 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23284 "cpu/st231_isa.cpp"
)
{

#line 1827 "integer.isa"
{
	os << "sxth $r" << idest << " = $r" << rsc1 ;
}
#line 23292 "cpu/st231_isa.cpp"
}
/* === Monadic Operation INSTRUCTIONS ================================== */

void
OpSxth::initialize_operands()
{

#line 250 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 23306 "cpu/st231_isa.cpp"
}

void
OpSxth::initialize_latencies()
{

#line 114 "sim_latencies.isa"
{LAT(1);}
#line 23315 "cpu/st231_isa.cpp"
}

void
OpSxth::initialize_function()
{

#line 36 "sim_functions.isa"
{ function = FnIntBasic; }
#line 23324 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSxth(st231::CodeType code, uint32_t addr)
{
	return new OpSxth(code, addr);
}

//************************************************************************
// the monadic format operations
//************************************************************************
// monadic operations : there is 5 monadic operations (go to line ???)
//************************************************************************
// the monadic format operations
//************************************************************************
// // monadic operations : there is 5 monadic operations 
// op sxtb(stop[1]:rsv[1]:0b00[2]:0b1001110[7]:0b000000000[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23344 "cpu/st231_isa.cpp"
OpSxtb::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23348 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23352 "cpu/st231_isa.cpp"
)
{

#line 1802 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        if (op1 & (1<<7))
          op1 |= 0xffffff00;
        else
          op1 &= 0x000000ff;

        cpu->SetGPR_N(idest, op1 );
}
#line 23366 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23371 "cpu/st231_isa.cpp"
OpSxtb::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23375 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23379 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23384 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23388 "cpu/st231_isa.cpp"
)
{

#line 1812 "integer.isa"
{
	os << "sxtb $r" << idest << " = $r" << rsc1 ;
}
#line 23396 "cpu/st231_isa.cpp"
}
/* === Monadic Operation INSTRUCTIONS ================================== */

void
OpSxtb::initialize_operands()
{

#line 250 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 23410 "cpu/st231_isa.cpp"
}

void
OpSxtb::initialize_latencies()
{

#line 113 "sim_latencies.isa"
{LAT(1);}
#line 23419 "cpu/st231_isa.cpp"
}

void
OpSxtb::initialize_function()
{

#line 36 "sim_functions.isa"
{ function = FnIntBasic; }
#line 23428 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSxtb(st231::CodeType code, uint32_t addr)
{
	return new OpSxtb(code, addr);
}

// op imulfrac(stop[1]:rsv[1]:0b00[2]:0b1111111[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23440 "cpu/st231_isa.cpp"
OpImulfrac::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23444 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23448 "cpu/st231_isa.cpp"
)
{

#line 1775 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        int32_t result;

        if((-op1 == 0x80000000) && (-op2 == 0x80000000))
        {   cpu->SetGPR_N(nlidest, 0x7fffffff );
        }

        else
        {   result = op1 * op2;
            result = op1 + (1 << 30);
            result = result >> 31;
            cpu->SetGPR_N(nlidest, result );
        }
}
#line 23469 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23474 "cpu/st231_isa.cpp"
OpImulfrac::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23478 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23482 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23487 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23491 "cpu/st231_isa.cpp"
)
{

#line 1792 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "imulfrac $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 23500 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImulfrac::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 23514 "cpu/st231_isa.cpp"
}

void
OpImulfrac::initialize_latencies()
{

#line 157 "sim_latencies.isa"
{LAT(3);}
#line 23523 "cpu/st231_isa.cpp"
}

void
OpImulfrac::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 23532 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImulfrac(st231::CodeType code, uint32_t addr)
{
	return new OpImulfrac(code, addr);
}

// inew multiply instructions for ST231 : the destination register is "nldest"
// // inew multiply instructions for ST231 : the destination register is "nldest"
// op imul64hu(stop[1]:rsv[1]:0b00[2]:0b1111110[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23546 "cpu/st231_isa.cpp"
OpImul64hu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23550 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23554 "cpu/st231_isa.cpp"
)
{

#line 1763 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        uint64_t result = op1* op2;
        cpu->SetGPR_N(nlidest, result >> 32 );
}
#line 23565 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23570 "cpu/st231_isa.cpp"
OpImul64hu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23574 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23578 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23583 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23587 "cpu/st231_isa.cpp"
)
{

#line 1770 "integer.isa"
{
	os << "mul64hu $r" << nlidest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 23595 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImul64hu::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 23609 "cpu/st231_isa.cpp"
}

void
OpImul64hu::initialize_latencies()
{

#line 156 "sim_latencies.isa"
{LAT(3);}
#line 23618 "cpu/st231_isa.cpp"
}

void
OpImul64hu::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 23627 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImul64hu(st231::CodeType code, uint32_t addr)
{
	return new OpImul64hu(code, addr);
}

// op ibnorl(stop[1]:rsv[1]:0b00[2]:0b1111101[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23639 "cpu/st231_isa.cpp"
OpIbnorl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23643 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23647 "cpu/st231_isa.cpp"
)
{

#line 1750 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, ~(op1||op2) );
}
#line 23657 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23662 "cpu/st231_isa.cpp"
OpIbnorl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23666 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23670 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23675 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23679 "cpu/st231_isa.cpp"
)
{

#line 1756 "integer.isa"
{
//        int32_t op2 = SignEx9to32(isrc2);
	os << "norl $b" << ibdest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 23688 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbnorl::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 23702 "cpu/st231_isa.cpp"
}

void
OpIbnorl::initialize_latencies()
{

#line 112 "sim_latencies.isa"
{LAT(1);}
#line 23711 "cpu/st231_isa.cpp"
}

void
OpIbnorl::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 23720 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbnorl(st231::CodeType code, uint32_t addr)
{
	return new OpIbnorl(code, addr);
}

// op iborl(stop[1]:rsv[1]:0b00[2]:0b1111100[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23732 "cpu/st231_isa.cpp"
OpIborl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23736 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23740 "cpu/st231_isa.cpp"
)
{

#line 1738 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, op1 || op2 );
}
#line 23750 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23755 "cpu/st231_isa.cpp"
OpIborl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23759 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23763 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23768 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23772 "cpu/st231_isa.cpp"
)
{

#line 1744 "integer.isa"
{
//        int32_t op2 = SignEx9to32(isrc2);
	os << "orl $b" << ibdest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 23781 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIborl::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 23795 "cpu/st231_isa.cpp"
}

void
OpIborl::initialize_latencies()
{

#line 111 "sim_latencies.isa"
{LAT(1);}
#line 23804 "cpu/st231_isa.cpp"
}

void
OpIborl::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 23813 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIborl(st231::CodeType code, uint32_t addr)
{
	return new OpIborl(code, addr);
}

// op ibnandl(stop[1]:rsv[1]:0b00[2]:0b1111011[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23825 "cpu/st231_isa.cpp"
OpIbnandl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23829 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23833 "cpu/st231_isa.cpp"
)
{

#line 1726 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, ~(op1 && op2) );
}
#line 23843 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23848 "cpu/st231_isa.cpp"
OpIbnandl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23852 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23856 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23861 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23865 "cpu/st231_isa.cpp"
)
{

#line 1732 "integer.isa"
{
//        int32_t op2 = SignEx9to32(isrc2);
	os << "nandl $b" << ibdest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 23874 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbnandl::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 23888 "cpu/st231_isa.cpp"
}

void
OpIbnandl::initialize_latencies()
{

#line 110 "sim_latencies.isa"
{LAT(1);}
#line 23897 "cpu/st231_isa.cpp"
}

void
OpIbnandl::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 23906 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbnandl(st231::CodeType code, uint32_t addr)
{
	return new OpIbnandl(code, addr);
}

// op ibandl(stop[1]:rsv[1]:0b00[2]:0b1111010[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 23918 "cpu/st231_isa.cpp"
OpIbandl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 23922 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 23926 "cpu/st231_isa.cpp"
)
{

#line 1714 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, op1 && op2 );
}
#line 23936 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 23941 "cpu/st231_isa.cpp"
OpIbandl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 23945 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 23949 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 23954 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 23958 "cpu/st231_isa.cpp"
)
{

#line 1720 "integer.isa"
{
//        int32_t op2 = SignEx9to32(isrc2);
	os << "andl $b" << ibdest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 23967 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbandl::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 23981 "cpu/st231_isa.cpp"
}

void
OpIbandl::initialize_latencies()
{

#line 109 "sim_latencies.isa"
{LAT(1);}
#line 23990 "cpu/st231_isa.cpp"
}

void
OpIbandl::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 23999 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbandl(st231::CodeType code, uint32_t addr)
{
	return new OpIbandl(code, addr);
}

// op ibcmpltu(stop[1]:rsv[1]:0b00[2]:0b1111001[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24011 "cpu/st231_isa.cpp"
OpIbcmpltu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24015 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24019 "cpu/st231_isa.cpp"
)
{

#line 1703 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, op1 < op2 );
}
#line 24029 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24034 "cpu/st231_isa.cpp"
OpIbcmpltu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24038 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24042 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24047 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24051 "cpu/st231_isa.cpp"
)
{

#line 1709 "integer.isa"
{
	os << "cmpltu $b" << ibdest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 24059 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmpltu::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24073 "cpu/st231_isa.cpp"
}

void
OpIbcmpltu::initialize_latencies()
{

#line 108 "sim_latencies.isa"
{LAT(1);}
#line 24082 "cpu/st231_isa.cpp"
}

void
OpIbcmpltu::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24091 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmpltu(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmpltu(code, addr);
}

// op ibcmplt(stop[1]:rsv[1]:0b00[2]:0b1111000[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24103 "cpu/st231_isa.cpp"
OpIbcmplt::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24107 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24111 "cpu/st231_isa.cpp"
)
{

#line 1691 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, op1 < op2 );
}
#line 24121 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24126 "cpu/st231_isa.cpp"
OpIbcmplt::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24130 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24134 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24139 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24143 "cpu/st231_isa.cpp"
)
{

#line 1697 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "cmplt $b" << ibdest << " = $r" << rsc1 << ", " << op2 ;
}
#line 24152 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmplt::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24166 "cpu/st231_isa.cpp"
}

void
OpIbcmplt::initialize_latencies()
{

#line 107 "sim_latencies.isa"
{LAT(1);}
#line 24175 "cpu/st231_isa.cpp"
}

void
OpIbcmplt::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24184 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmplt(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmplt(code, addr);
}

// op ibcmpleu(stop[1]:rsv[1]:0b00[2]:0b1110111[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24196 "cpu/st231_isa.cpp"
OpIbcmpleu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24200 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24204 "cpu/st231_isa.cpp"
)
{

#line 1680 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, (op1 <= op2) );
}
#line 24214 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24219 "cpu/st231_isa.cpp"
OpIbcmpleu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24223 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24227 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24232 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24236 "cpu/st231_isa.cpp"
)
{

#line 1686 "integer.isa"
{
	os << "ibcmpleu $b" << ibdest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 24244 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmpleu::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24258 "cpu/st231_isa.cpp"
}

void
OpIbcmpleu::initialize_latencies()
{

#line 106 "sim_latencies.isa"
{LAT(1);}
#line 24267 "cpu/st231_isa.cpp"
}

void
OpIbcmpleu::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24276 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmpleu(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmpleu(code, addr);
}

// op ibcmple(stop[1]:rsv[1]:0b00[2]:0b1110110[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24288 "cpu/st231_isa.cpp"
OpIbcmple::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24292 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24296 "cpu/st231_isa.cpp"
)
{

#line 1668 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, (op1 <= op2) );
}
#line 24306 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24311 "cpu/st231_isa.cpp"
OpIbcmple::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24315 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24319 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24324 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24328 "cpu/st231_isa.cpp"
)
{

#line 1674 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "ibcmple $b" << ibdest << " = $r" << rsc1 << ", " << op2 ;
}
#line 24337 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmple::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24351 "cpu/st231_isa.cpp"
}

void
OpIbcmple::initialize_latencies()
{

#line 105 "sim_latencies.isa"
{LAT(1);}
#line 24360 "cpu/st231_isa.cpp"
}

void
OpIbcmple::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24369 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmple(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmple(code, addr);
}

// op ibcmpgtu(stop[1]:rsv[1]:0b00[2]:0b1110101[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24381 "cpu/st231_isa.cpp"
OpIbcmpgtu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24385 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24389 "cpu/st231_isa.cpp"
)
{

#line 1657 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, op1 > op2 );
}
#line 24399 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24404 "cpu/st231_isa.cpp"
OpIbcmpgtu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24408 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24412 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24417 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24421 "cpu/st231_isa.cpp"
)
{

#line 1663 "integer.isa"
{
	os << "cmpgtu $b" << ibdest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 24429 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmpgtu::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24443 "cpu/st231_isa.cpp"
}

void
OpIbcmpgtu::initialize_latencies()
{

#line 104 "sim_latencies.isa"
{LAT(1);}
#line 24452 "cpu/st231_isa.cpp"
}

void
OpIbcmpgtu::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24461 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmpgtu(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmpgtu(code, addr);
}

// op ibcmpgt(stop[1]:rsv[1]:0b00[2]:0b1110100[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24473 "cpu/st231_isa.cpp"
OpIbcmpgt::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24477 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24481 "cpu/st231_isa.cpp"
)
{

#line 1645 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, op1 > op2 );
}
#line 24491 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24496 "cpu/st231_isa.cpp"
OpIbcmpgt::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24500 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24504 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24509 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24513 "cpu/st231_isa.cpp"
)
{

#line 1651 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "cmpgt $b" << ibdest << " = $r" << rsc1 << ", " << op2 ;
}
#line 24522 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmpgt::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24536 "cpu/st231_isa.cpp"
}

void
OpIbcmpgt::initialize_latencies()
{

#line 103 "sim_latencies.isa"
{LAT(1);}
#line 24545 "cpu/st231_isa.cpp"
}

void
OpIbcmpgt::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24554 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmpgt(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmpgt(code, addr);
}

// op ibcmpgeu(stop[1]:rsv[1]:0b00[2]:0b1110011[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24566 "cpu/st231_isa.cpp"
OpIbcmpgeu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24570 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24574 "cpu/st231_isa.cpp"
)
{

#line 1634 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, (op1 >= op2) );
}
#line 24584 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24589 "cpu/st231_isa.cpp"
OpIbcmpgeu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24593 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24597 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24602 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24606 "cpu/st231_isa.cpp"
)
{

#line 1640 "integer.isa"
{
	os << "cmpgeu $b" << ibdest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 24614 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmpgeu::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24628 "cpu/st231_isa.cpp"
}

void
OpIbcmpgeu::initialize_latencies()
{

#line 102 "sim_latencies.isa"
{LAT(1);}
#line 24637 "cpu/st231_isa.cpp"
}

void
OpIbcmpgeu::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24646 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmpgeu(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmpgeu(code, addr);
}

// op ibcmpge(stop[1]:rsv[1]:0b00[2]:0b1110010[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24658 "cpu/st231_isa.cpp"
OpIbcmpge::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24662 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24666 "cpu/st231_isa.cpp"
)
{

#line 1622 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, (op1 >= op2) );
}
#line 24676 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24681 "cpu/st231_isa.cpp"
OpIbcmpge::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24685 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24689 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24694 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24698 "cpu/st231_isa.cpp"
)
{

#line 1628 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "cmpge $b" << ibdest << " = $r" << rsc1 << ", " << op2 ;
}
#line 24707 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmpge::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24721 "cpu/st231_isa.cpp"
}

void
OpIbcmpge::initialize_latencies()
{

#line 101 "sim_latencies.isa"
{LAT(1);}
#line 24730 "cpu/st231_isa.cpp"
}

void
OpIbcmpge::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24739 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmpge(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmpge(code, addr);
}

// op ibcmpne(stop[1]:rsv[1]:0b00[2]:0b1110001[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24751 "cpu/st231_isa.cpp"
OpIbcmpne::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24755 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24759 "cpu/st231_isa.cpp"
)
{

#line 1610 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, (op1 != op2) );
}
#line 24769 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24774 "cpu/st231_isa.cpp"
OpIbcmpne::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24778 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24782 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24787 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24791 "cpu/st231_isa.cpp"
)
{

#line 1616 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "cmpne $b" << ibdest << " = $r" << rsc1 << ", " << op2 ;
}
#line 24800 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmpne::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24814 "cpu/st231_isa.cpp"
}

void
OpIbcmpne::initialize_latencies()
{

#line 100 "sim_latencies.isa"
{LAT(1);}
#line 24823 "cpu/st231_isa.cpp"
}

void
OpIbcmpne::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24832 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmpne(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmpne(code, addr);
}

// immediate format branch operations : destination register is "ibdest"
// // immediate format branch operations : destination register is "ibdest"
// op ibcmpeq(stop[1]:rsv[1]:0b00[2]:0b1110000[7]:isrc2[9]:null[3]:ibdest[3]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24846 "cpu/st231_isa.cpp"
OpIbcmpeq::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24850 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24854 "cpu/st231_isa.cpp"
)
{

#line 1598 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPB_N(ibdest, (op1 == op2) );
}
#line 24864 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24869 "cpu/st231_isa.cpp"
OpIbcmpeq::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24873 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24877 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24882 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24886 "cpu/st231_isa.cpp"
)
{

#line 1604 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "cmpeq $b" << ibdest << " = $r" << rsc1 << ", " << op2 ;
}
#line 24895 "cpu/st231_isa.cpp"
}
/* ---  integer immediate conditional branch instructions -------------- */

void
OpIbcmpeq::initialize_operands()
{

#line 235 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPB_T, ibdest);
}
#line 24909 "cpu/st231_isa.cpp"
}

void
OpIbcmpeq::initialize_latencies()
{

#line 99 "sim_latencies.isa"
{LAT(1);}
#line 24918 "cpu/st231_isa.cpp"
}

void
OpIbcmpeq::initialize_function()
{

#line 35 "sim_functions.isa"
{ function = FnIntBasic; }
#line 24927 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIbcmpeq(st231::CodeType code, uint32_t addr)
{
	return new OpIbcmpeq(code, addr);
}

// op imul64h(stop[1]:rsv[1]:0b00[2]:0b1101111[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 24939 "cpu/st231_isa.cpp"
OpImul64h::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 24943 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 24947 "cpu/st231_isa.cpp"
)
{

#line 1584 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        int64_t result = op1 * op2;
        cpu->SetGPR_N(nlidest, result>>32 );
}
#line 24958 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 24963 "cpu/st231_isa.cpp"
OpImul64h::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 24967 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 24971 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 24976 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 24980 "cpu/st231_isa.cpp"
)
{

#line 1591 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "mul64h $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 24989 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImul64h::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 25003 "cpu/st231_isa.cpp"
}

void
OpImul64h::initialize_latencies()
{

#line 155 "sim_latencies.isa"
{LAT(3);}
#line 25012 "cpu/st231_isa.cpp"
}

void
OpImul64h::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 25021 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImul64h(st231::CodeType code, uint32_t addr)
{
	return new OpImul64h(code, addr);
}

// inew multiply instructions for ST231 : the destination register is "nldest"
// // inew multiply instructions for ST231 : the destination register is "nldest"
// op imul32(stop[1]:rsv[1]:0b00[2]:0b1101110[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25035 "cpu/st231_isa.cpp"
OpImul32::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25039 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25043 "cpu/st231_isa.cpp"
)
{

#line 1572 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(nlidest, op1 * op2 );
}
#line 25053 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25058 "cpu/st231_isa.cpp"
OpImul32::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25062 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25066 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25071 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25075 "cpu/st231_isa.cpp"
)
{

#line 1578 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "mul32 $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 25084 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImul32::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 25098 "cpu/st231_isa.cpp"
}

void
OpImul32::initialize_latencies()
{

#line 154 "sim_latencies.isa"
{LAT(3);}
#line 25107 "cpu/st231_isa.cpp"
}

void
OpImul32::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 25116 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImul32(st231::CodeType code, uint32_t addr)
{
	return new OpImul32(code, addr);
}

// op inorl(stop[1]:rsv[1]:0b00[2]:0b1101101[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25128 "cpu/st231_isa.cpp"
OpInorl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25132 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25136 "cpu/st231_isa.cpp"
)
{

#line 1559 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, ~(op1||op2) );
}
#line 25146 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25151 "cpu/st231_isa.cpp"
OpInorl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25155 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25159 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25164 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25168 "cpu/st231_isa.cpp"
)
{

#line 1565 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "norl $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 25177 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpInorl::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 25192 "cpu/st231_isa.cpp"
}

void
OpInorl::initialize_latencies()
{

#line 98 "sim_latencies.isa"
{LAT(1);}
#line 25201 "cpu/st231_isa.cpp"
}

void
OpInorl::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 25210 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpInorl(st231::CodeType code, uint32_t addr)
{
	return new OpInorl(code, addr);
}

// op iorl(stop[1]:rsv[1]:0b00[2]:0b1101100[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25222 "cpu/st231_isa.cpp"
OpIorl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25226 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25230 "cpu/st231_isa.cpp"
)
{

#line 1547 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, op1 || op2 );
}
#line 25240 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25245 "cpu/st231_isa.cpp"
OpIorl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25249 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25253 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25258 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25262 "cpu/st231_isa.cpp"
)
{

#line 1553 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "orl $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 25271 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIorl::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 25286 "cpu/st231_isa.cpp"
}

void
OpIorl::initialize_latencies()
{

#line 97 "sim_latencies.isa"
{LAT(1);}
#line 25295 "cpu/st231_isa.cpp"
}

void
OpIorl::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 25304 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIorl(st231::CodeType code, uint32_t addr)
{
	return new OpIorl(code, addr);
}

// op inandl(stop[1]:rsv[1]:0b00[2]:0b1101011[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25316 "cpu/st231_isa.cpp"
OpInandl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25320 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25324 "cpu/st231_isa.cpp"
)
{

#line 1535 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, ~(op1&&op2) );
}
#line 25334 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25339 "cpu/st231_isa.cpp"
OpInandl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25343 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25347 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25352 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25356 "cpu/st231_isa.cpp"
)
{

#line 1541 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "nandl $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 25365 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpInandl::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 25380 "cpu/st231_isa.cpp"
}

void
OpInandl::initialize_latencies()
{

#line 96 "sim_latencies.isa"
{LAT(1);}
#line 25389 "cpu/st231_isa.cpp"
}

void
OpInandl::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 25398 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpInandl(st231::CodeType code, uint32_t addr)
{
	return new OpInandl(code, addr);
}

// op iandl(stop[1]:rsv[1]:0b00[2]:0b1101010[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25410 "cpu/st231_isa.cpp"
OpIandl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25414 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25418 "cpu/st231_isa.cpp"
)
{

#line 1523 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, op1 && op2 );
}
#line 25428 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25433 "cpu/st231_isa.cpp"
OpIandl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25437 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25441 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25446 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25450 "cpu/st231_isa.cpp"
)
{

#line 1529 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "andl $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 25459 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIandl::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 25474 "cpu/st231_isa.cpp"
}

void
OpIandl::initialize_latencies()
{

#line 95 "sim_latencies.isa"
{LAT(1);}
#line 25483 "cpu/st231_isa.cpp"
}

void
OpIandl::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 25492 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIandl(st231::CodeType code, uint32_t addr)
{
	return new OpIandl(code, addr);
}

// op icmpltu(stop[1]:rsv[1]:0b00[2]:0b1101001[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25504 "cpu/st231_isa.cpp"
OpIcmpltu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25508 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25512 "cpu/st231_isa.cpp"
)
{

#line 1512 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 < op2) );
}
#line 25522 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25527 "cpu/st231_isa.cpp"
OpIcmpltu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25531 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25535 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25540 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25544 "cpu/st231_isa.cpp"
)
{

#line 1518 "integer.isa"
{
	os << "icmpgltu $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 25552 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmpltu::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 25567 "cpu/st231_isa.cpp"
}

void
OpIcmpltu::initialize_latencies()
{

#line 94 "sim_latencies.isa"
{LAT(1);}
#line 25576 "cpu/st231_isa.cpp"
}

void
OpIcmpltu::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 25585 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmpltu(st231::CodeType code, uint32_t addr)
{
	return new OpIcmpltu(code, addr);
}

// op icmplt(stop[1]:rsv[1]:0b00[2]:0b1101000[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25597 "cpu/st231_isa.cpp"
OpIcmplt::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25601 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25605 "cpu/st231_isa.cpp"
)
{

#line 1500 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 < op2) );
}
#line 25615 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25620 "cpu/st231_isa.cpp"
OpIcmplt::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25624 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25628 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25633 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25637 "cpu/st231_isa.cpp"
)
{

#line 1506 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "icmple $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 25646 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmplt::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 25661 "cpu/st231_isa.cpp"
}

void
OpIcmplt::initialize_latencies()
{

#line 93 "sim_latencies.isa"
{LAT(1);}
#line 25670 "cpu/st231_isa.cpp"
}

void
OpIcmplt::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 25679 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmplt(st231::CodeType code, uint32_t addr)
{
	return new OpIcmplt(code, addr);
}

// op icmpleu(stop[1]:rsv[1]:0b00[2]:0b1100111[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25691 "cpu/st231_isa.cpp"
OpIcmpleu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25695 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25699 "cpu/st231_isa.cpp"
)
{

#line 1489 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 <= op2) );
}
#line 25709 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25714 "cpu/st231_isa.cpp"
OpIcmpleu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25718 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25722 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25727 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25731 "cpu/st231_isa.cpp"
)
{

#line 1495 "integer.isa"
{
	os << "icmpgleu $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 25739 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmpleu::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 25754 "cpu/st231_isa.cpp"
}

void
OpIcmpleu::initialize_latencies()
{

#line 92 "sim_latencies.isa"
{LAT(1);}
#line 25763 "cpu/st231_isa.cpp"
}

void
OpIcmpleu::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 25772 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmpleu(st231::CodeType code, uint32_t addr)
{
	return new OpIcmpleu(code, addr);
}

// op icmple(stop[1]:rsv[1]:0b00[2]:0b1100110[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25784 "cpu/st231_isa.cpp"
OpIcmple::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25788 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25792 "cpu/st231_isa.cpp"
)
{

#line 1477 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 <= op2) );
}
#line 25802 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25807 "cpu/st231_isa.cpp"
OpIcmple::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25811 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25815 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25820 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25824 "cpu/st231_isa.cpp"
)
{

#line 1483 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "icmple $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 25833 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmple::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 25848 "cpu/st231_isa.cpp"
}

void
OpIcmple::initialize_latencies()
{

#line 91 "sim_latencies.isa"
{LAT(1);}
#line 25857 "cpu/st231_isa.cpp"
}

void
OpIcmple::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 25866 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmple(st231::CodeType code, uint32_t addr)
{
	return new OpIcmple(code, addr);
}

// op icmpgtu(stop[1]:rsv[1]:0b00[2]:0b1100101[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25878 "cpu/st231_isa.cpp"
OpIcmpgtu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25882 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25886 "cpu/st231_isa.cpp"
)
{

#line 1466 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 > op2) );
}
#line 25896 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25901 "cpu/st231_isa.cpp"
OpIcmpgtu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25905 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 25909 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 25914 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 25918 "cpu/st231_isa.cpp"
)
{

#line 1472 "integer.isa"
{
	os << "cmpgtu $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 25926 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmpgtu::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 25941 "cpu/st231_isa.cpp"
}

void
OpIcmpgtu::initialize_latencies()
{

#line 90 "sim_latencies.isa"
{LAT(1);}
#line 25950 "cpu/st231_isa.cpp"
}

void
OpIcmpgtu::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 25959 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmpgtu(st231::CodeType code, uint32_t addr)
{
	return new OpIcmpgtu(code, addr);
}

// op icmpgt(stop[1]:rsv[1]:0b00[2]:0b1100100[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 25971 "cpu/st231_isa.cpp"
OpIcmpgt::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 25975 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 25979 "cpu/st231_isa.cpp"
)
{

#line 1454 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 > op2) );
}
#line 25989 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 25994 "cpu/st231_isa.cpp"
OpIcmpgt::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 25998 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26002 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26007 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26011 "cpu/st231_isa.cpp"
)
{

#line 1460 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "cmpgt $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 26020 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmpgt::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 26035 "cpu/st231_isa.cpp"
}

void
OpIcmpgt::initialize_latencies()
{

#line 89 "sim_latencies.isa"
{LAT(1);}
#line 26044 "cpu/st231_isa.cpp"
}

void
OpIcmpgt::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 26053 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmpgt(st231::CodeType code, uint32_t addr)
{
	return new OpIcmpgt(code, addr);
}

// op icmpgeu(stop[1]:rsv[1]:0b00[2]:0b1100011[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26065 "cpu/st231_isa.cpp"
OpIcmpgeu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26069 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26073 "cpu/st231_isa.cpp"
)
{

#line 1443 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 >= op2) );
}
#line 26083 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26088 "cpu/st231_isa.cpp"
OpIcmpgeu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26092 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26096 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26101 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26105 "cpu/st231_isa.cpp"
)
{

#line 1449 "integer.isa"
{
	os << "cmpgeu $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 26113 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmpgeu::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 26128 "cpu/st231_isa.cpp"
}

void
OpIcmpgeu::initialize_latencies()
{

#line 88 "sim_latencies.isa"
{LAT(1);}
#line 26137 "cpu/st231_isa.cpp"
}

void
OpIcmpgeu::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 26146 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmpgeu(st231::CodeType code, uint32_t addr)
{
	return new OpIcmpgeu(code, addr);
}

// op icmpge(stop[1]:rsv[1]:0b00[2]:0b1100010[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26158 "cpu/st231_isa.cpp"
OpIcmpge::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26162 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26166 "cpu/st231_isa.cpp"
)
{

#line 1431 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 >= op2) );
}
#line 26176 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26181 "cpu/st231_isa.cpp"
OpIcmpge::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26185 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26189 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26194 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26198 "cpu/st231_isa.cpp"
)
{

#line 1437 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "cmpge $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 26207 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmpge::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 26222 "cpu/st231_isa.cpp"
}

void
OpIcmpge::initialize_latencies()
{

#line 87 "sim_latencies.isa"
{LAT(1);}
#line 26231 "cpu/st231_isa.cpp"
}

void
OpIcmpge::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 26240 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmpge(st231::CodeType code, uint32_t addr)
{
	return new OpIcmpge(code, addr);
}

// op icmpne(stop[1]:rsv[1]:0b00[2]:0b1100001[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26252 "cpu/st231_isa.cpp"
OpIcmpne::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26256 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26260 "cpu/st231_isa.cpp"
)
{

#line 1419 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 != op2) );
}
#line 26270 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26275 "cpu/st231_isa.cpp"
OpIcmpne::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26279 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26283 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26288 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26292 "cpu/st231_isa.cpp"
)
{

#line 1425 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "cmpne $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 26301 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmpne::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 26316 "cpu/st231_isa.cpp"
}

void
OpIcmpne::initialize_latencies()
{

#line 86 "sim_latencies.isa"
{LAT(1);}
#line 26325 "cpu/st231_isa.cpp"
}

void
OpIcmpne::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 26334 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmpne(st231::CodeType code, uint32_t addr)
{
	return new OpIcmpne(code, addr);
}

// op icmpeq(stop[1]:rsv[1]:0b00[2]:0b1100000[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26346 "cpu/st231_isa.cpp"
OpIcmpeq::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26350 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26354 "cpu/st231_isa.cpp"
)
{

#line 1407 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1 == op2) );
}
#line 26364 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26369 "cpu/st231_isa.cpp"
OpIcmpeq::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26373 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26377 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26382 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26386 "cpu/st231_isa.cpp"
)
{

#line 1413 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "cmpeq $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 26395 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIcmpeq::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 26410 "cpu/st231_isa.cpp"
}

void
OpIcmpeq::initialize_latencies()
{

#line 85 "sim_latencies.isa"
{LAT(1);}
#line 26419 "cpu/st231_isa.cpp"
}

void
OpIcmpeq::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 26428 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIcmpeq(st231::CodeType code, uint32_t addr)
{
	return new OpIcmpeq(code, addr);
}

// op imulhs(stop[1]:rsv[1]:0b00[2]:0b1011111[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26440 "cpu/st231_isa.cpp"
OpImulhs::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26444 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26448 "cpu/st231_isa.cpp"
)
{

#line 1394 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = ((int32_t)cpu->Imm(isrc2))>>16;
        cpu->SetGPR_N(nlidest, (op1 * op2)<<16 );
}
#line 26458 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26463 "cpu/st231_isa.cpp"
OpImulhs::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26467 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26471 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26476 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26480 "cpu/st231_isa.cpp"
)
{

#line 1400 "integer.isa"
{
	os << "mulhs $r" << nlidest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 26488 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImulhs::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 26502 "cpu/st231_isa.cpp"
}

void
OpImulhs::initialize_latencies()
{

#line 153 "sim_latencies.isa"
{LAT(3);}
#line 26511 "cpu/st231_isa.cpp"
}

void
OpImulhs::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 26520 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImulhs(st231::CodeType code, uint32_t addr)
{
	return new OpImulhs(code, addr);
}

// op imulhhu(stop[1]:rsv[1]:0b00[2]:0b1011110[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26532 "cpu/st231_isa.cpp"
OpImulhhu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26536 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26540 "cpu/st231_isa.cpp"
)
{

#line 1383 "integer.isa"
{
        uint16_t op1 = (uint16_t)cpu->GetGPR_C(rsc1)>>16;
        uint16_t op2 = (uint16_t)cpu->Imm(isrc2)>>16;
        cpu->SetGPR_N(nlidest, op1 * op2 );
}
#line 26550 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26555 "cpu/st231_isa.cpp"
OpImulhhu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26559 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26563 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26568 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26572 "cpu/st231_isa.cpp"
)
{

#line 1389 "integer.isa"
{
	os << "mulhhu $r" << nlidest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 26580 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImulhhu::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 26594 "cpu/st231_isa.cpp"
}

void
OpImulhhu::initialize_latencies()
{

#line 152 "sim_latencies.isa"
{LAT(3);}
#line 26603 "cpu/st231_isa.cpp"
}

void
OpImulhhu::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 26612 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImulhhu(st231::CodeType code, uint32_t addr)
{
	return new OpImulhhu(code, addr);
}

// op imulhh(stop[1]:rsv[1]:0b00[2]:0b1011101[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26624 "cpu/st231_isa.cpp"
OpImulhh::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26628 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26632 "cpu/st231_isa.cpp"
)
{

#line 1371 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(nlidest, (op1>>16) * (op2>>16) );
}
#line 26642 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26647 "cpu/st231_isa.cpp"
OpImulhh::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26651 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26655 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26660 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26664 "cpu/st231_isa.cpp"
)
{

#line 1377 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "mulhh $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 26673 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImulhh::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 26687 "cpu/st231_isa.cpp"
}

void
OpImulhh::initialize_latencies()
{

#line 151 "sim_latencies.isa"
{LAT(3);}
#line 26696 "cpu/st231_isa.cpp"
}

void
OpImulhh::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 26705 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImulhh(st231::CodeType code, uint32_t addr)
{
	return new OpImulhh(code, addr);
}

// op imullhu(stop[1]:rsv[1]:0b00[2]:0b1011100[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26717 "cpu/st231_isa.cpp"
OpImullhu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26721 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26725 "cpu/st231_isa.cpp"
)
{

#line 1360 "integer.isa"
{
        uint16_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = (uint16_t)(cpu->Imm(isrc2)>>16);
        cpu->SetGPR_N(nlidest, op1* op2 );
}
#line 26735 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26740 "cpu/st231_isa.cpp"
OpImullhu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26744 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26748 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26753 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26757 "cpu/st231_isa.cpp"
)
{

#line 1366 "integer.isa"
{
	os << "mullhu $r" << nlidest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 26765 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImullhu::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 26779 "cpu/st231_isa.cpp"
}

void
OpImullhu::initialize_latencies()
{

#line 150 "sim_latencies.isa"
{LAT(3);}
#line 26788 "cpu/st231_isa.cpp"
}

void
OpImullhu::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 26797 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImullhu(st231::CodeType code, uint32_t addr)
{
	return new OpImullhu(code, addr);
}

// op imullh(stop[1]:rsv[1]:0b00[2]:0b1011011[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26809 "cpu/st231_isa.cpp"
OpImullh::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26813 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26817 "cpu/st231_isa.cpp"
)
{

#line 1348 "integer.isa"
{
        int16_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(nlidest, op1* (op2>>16) );
}
#line 26827 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26832 "cpu/st231_isa.cpp"
OpImullh::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26836 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26840 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26845 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26849 "cpu/st231_isa.cpp"
)
{

#line 1354 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "mullh $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 26858 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImullh::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 26872 "cpu/st231_isa.cpp"
}

void
OpImullh::initialize_latencies()
{

#line 149 "sim_latencies.isa"
{LAT(3);}
#line 26881 "cpu/st231_isa.cpp"
}

void
OpImullh::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 26890 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImullh(st231::CodeType code, uint32_t addr)
{
	return new OpImullh(code, addr);
}

// op imulllu(stop[1]:rsv[1]:0b00[2]:0b1011010[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26902 "cpu/st231_isa.cpp"
OpImulllu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26906 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 26910 "cpu/st231_isa.cpp"
)
{

#line 1337 "integer.isa"
{
        uint16_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(nlidest, op1* op2 );
}
#line 26920 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 26925 "cpu/st231_isa.cpp"
OpImulllu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 26929 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 26933 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 26938 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 26942 "cpu/st231_isa.cpp"
)
{

#line 1343 "integer.isa"
{
	os << "mulllu $r" << nlidest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 26950 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImulllu::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 26964 "cpu/st231_isa.cpp"
}

void
OpImulllu::initialize_latencies()
{

#line 148 "sim_latencies.isa"
{LAT(3);}
#line 26973 "cpu/st231_isa.cpp"
}

void
OpImulllu::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 26982 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImulllu(st231::CodeType code, uint32_t addr)
{
	return new OpImulllu(code, addr);
}

// op imulll(stop[1]:rsv[1]:0b00[2]:0b1011001[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 26994 "cpu/st231_isa.cpp"
OpImulll::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 26998 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27002 "cpu/st231_isa.cpp"
)
{

#line 1325 "integer.isa"
{
        int16_t op1 = cpu->GetGPR_C(rsc1);
        int16_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(nlidest, op1* op2 );
}
#line 27012 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27017 "cpu/st231_isa.cpp"
OpImulll::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27021 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27025 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27030 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27034 "cpu/st231_isa.cpp"
)
{

#line 1331 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "mulll $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 27043 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImulll::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 27057 "cpu/st231_isa.cpp"
}

void
OpImulll::initialize_latencies()
{

#line 147 "sim_latencies.isa"
{LAT(3);}
#line 27066 "cpu/st231_isa.cpp"
}

void
OpImulll::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 27075 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImulll(st231::CodeType code, uint32_t addr)
{
	return new OpImulll(code, addr);
}

// op imulhu(stop[1]:rsv[1]:0b00[2]:0b1011000[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27087 "cpu/st231_isa.cpp"
OpImulhu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27091 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27095 "cpu/st231_isa.cpp"
)
{

#line 1314 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = (cpu->Imm(isrc2))>>16;
        cpu->SetGPR_N(nlidest, op1* op2 );
}
#line 27105 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27110 "cpu/st231_isa.cpp"
OpImulhu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27114 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27118 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27123 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27127 "cpu/st231_isa.cpp"
)
{

#line 1320 "integer.isa"
{
	os << "mulhu $r" << nlidest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 27135 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImulhu::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 27149 "cpu/st231_isa.cpp"
}

void
OpImulhu::initialize_latencies()
{

#line 146 "sim_latencies.isa"
{LAT(3);}
#line 27158 "cpu/st231_isa.cpp"
}

void
OpImulhu::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 27167 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImulhu(st231::CodeType code, uint32_t addr)
{
	return new OpImulhu(code, addr);
}

// op imulh(stop[1]:rsv[1]:0b00[2]:0b1010111[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27179 "cpu/st231_isa.cpp"
OpImulh::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27183 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27187 "cpu/st231_isa.cpp"
)
{

#line 1302 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(nlidest, op1* (op2>>16) );
}
#line 27197 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27202 "cpu/st231_isa.cpp"
OpImulh::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27206 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27210 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27215 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27219 "cpu/st231_isa.cpp"
)
{

#line 1308 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "mulh $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 27228 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImulh::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 27242 "cpu/st231_isa.cpp"
}

void
OpImulh::initialize_latencies()
{

#line 145 "sim_latencies.isa"
{LAT(3);}
#line 27251 "cpu/st231_isa.cpp"
}

void
OpImulh::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 27260 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImulh(st231::CodeType code, uint32_t addr)
{
	return new OpImulh(code, addr);
}

// op imullu(stop[1]:rsv[1]:0b00[2]:0b1010110[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27272 "cpu/st231_isa.cpp"
OpImullu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27276 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27280 "cpu/st231_isa.cpp"
)
{

#line 1291 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(nlidest, op1* op2 );
}
#line 27290 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27295 "cpu/st231_isa.cpp"
OpImullu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27299 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27303 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27308 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27312 "cpu/st231_isa.cpp"
)
{

#line 1297 "integer.isa"
{
	os << "mullu $r" << nlidest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 27320 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImullu::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 27334 "cpu/st231_isa.cpp"
}

void
OpImullu::initialize_latencies()
{

#line 144 "sim_latencies.isa"
{LAT(3);}
#line 27343 "cpu/st231_isa.cpp"
}

void
OpImullu::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 27352 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImullu(st231::CodeType code, uint32_t addr)
{
	return new OpImullu(code, addr);
}

// op imull(stop[1]:rsv[1]:0b00[2]:0b1010101[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27364 "cpu/st231_isa.cpp"
OpImull::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27368 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27372 "cpu/st231_isa.cpp"
)
{

#line 1279 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int16_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(nlidest, op1* op2 );
}
#line 27382 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27387 "cpu/st231_isa.cpp"
OpImull::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27391 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27395 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27400 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27404 "cpu/st231_isa.cpp"
)
{

#line 1285 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "mull $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 27413 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImull::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 27427 "cpu/st231_isa.cpp"
}

void
OpImull::initialize_latencies()
{

#line 143 "sim_latencies.isa"
{LAT(3);}
#line 27436 "cpu/st231_isa.cpp"
}

void
OpImull::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 27445 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImull(st231::CodeType code, uint32_t addr)
{
	return new OpImull(code, addr);
}

// imultiply instruction: the destination register is "nlidest"
// // imultiply instruction: the destination register is "nlidest"
// op imulhhs(stop[1]:rsv[1]:0b00[2]:0b1010100[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27459 "cpu/st231_isa.cpp"
OpImulhhs::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27463 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27467 "cpu/st231_isa.cpp"
)
{

#line 1267 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(nlidest, (op1*(op2>>16)) >> 16);
}
#line 27477 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27482 "cpu/st231_isa.cpp"
OpImulhhs::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27486 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27490 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27495 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27499 "cpu/st231_isa.cpp"
)
{

#line 1273 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "mulhhs $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 27508 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImulhhs::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 27522 "cpu/st231_isa.cpp"
}

void
OpImulhhs::initialize_latencies()
{

#line 142 "sim_latencies.isa"
{LAT(3);}
#line 27531 "cpu/st231_isa.cpp"
}

void
OpImulhhs::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 27540 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImulhhs(st231::CodeType code, uint32_t addr)
{
	return new OpImulhhs(code, addr);
}

// op iminu(stop[1]:rsv[1]:0b00[2]:0b1010011[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27552 "cpu/st231_isa.cpp"
OpIminu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27556 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27560 "cpu/st231_isa.cpp"
)
{

#line 1254 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        int32_t result = op1 < op2 ? op1 : op2;
        cpu->SetGPR_N(idest, result);
}
#line 27571 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27576 "cpu/st231_isa.cpp"
OpIminu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27580 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27584 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27589 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27593 "cpu/st231_isa.cpp"
)
{

#line 1261 "integer.isa"
{
	os << "minu $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 27601 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIminu::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 27616 "cpu/st231_isa.cpp"
}

void
OpIminu::initialize_latencies()
{

#line 84 "sim_latencies.isa"
{LAT(1);}
#line 27625 "cpu/st231_isa.cpp"
}

void
OpIminu::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 27634 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIminu(st231::CodeType code, uint32_t addr)
{
	return new OpIminu(code, addr);
}

// op imin(stop[1]:rsv[1]:0b00[2]:0b1010010[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27646 "cpu/st231_isa.cpp"
OpImin::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27650 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27654 "cpu/st231_isa.cpp"
)
{

#line 1241 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm( isrc2);
        int32_t result = op1 < op2 ? op1 : op2;
        cpu->SetGPR_N(idest, result);
}
#line 27665 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27670 "cpu/st231_isa.cpp"
OpImin::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27674 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27678 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27683 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27687 "cpu/st231_isa.cpp"
)
{

#line 1248 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "min $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 27696 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpImin::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 27711 "cpu/st231_isa.cpp"
}

void
OpImin::initialize_latencies()
{

#line 83 "sim_latencies.isa"
{LAT(1);}
#line 27720 "cpu/st231_isa.cpp"
}

void
OpImin::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 27729 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImin(st231::CodeType code, uint32_t addr)
{
	return new OpImin(code, addr);
}

// op imaxu(stop[1]:rsv[1]:0b00[2]:0b1010001[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27741 "cpu/st231_isa.cpp"
OpImaxu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27745 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27749 "cpu/st231_isa.cpp"
)
{

#line 1229 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->Imm(isrc2);
        int32_t result = op1 > op2 ? op1 : op2;
        cpu->SetGPR_N(idest, result);
}
#line 27760 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27765 "cpu/st231_isa.cpp"
OpImaxu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27769 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27773 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27778 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27782 "cpu/st231_isa.cpp"
)
{

#line 1236 "integer.isa"
{
	os << "maxu $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 27790 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpImaxu::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 27805 "cpu/st231_isa.cpp"
}

void
OpImaxu::initialize_latencies()
{

#line 82 "sim_latencies.isa"
{LAT(1);}
#line 27814 "cpu/st231_isa.cpp"
}

void
OpImaxu::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 27823 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImaxu(st231::CodeType code, uint32_t addr)
{
	return new OpImaxu(code, addr);
}

// op imax(stop[1]:rsv[1]:0b00[2]:0b1010000[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27835 "cpu/st231_isa.cpp"
OpImax::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27839 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27843 "cpu/st231_isa.cpp"
)
{

#line 1216 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        int32_t result = op1 > op2 ? op1 : op2;
        cpu->SetGPR_N(idest, result);
}
#line 27854 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27859 "cpu/st231_isa.cpp"
OpImax::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27863 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27867 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27872 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27876 "cpu/st231_isa.cpp"
)
{

#line 1223 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "max $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 27885 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpImax::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 27900 "cpu/st231_isa.cpp"
}

void
OpImax::initialize_latencies()
{

#line 81 "sim_latencies.isa"
{LAT(1);}
#line 27909 "cpu/st231_isa.cpp"
}

void
OpImax::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 27918 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImax(st231::CodeType code, uint32_t addr)
{
	return new OpImax(code, addr);
}

// monadic operations : there is 5 monadic operations (go to line ???)
// op monadic  (stop[1]:rsv[1]:0b00[2]:0b1001110[7]:exopcode[9]:idest[6]:rsc1[6])
// multiply instruction: the destination register is "nlidest"
// // multiply instruction: the destination register is "nlidest"
// op imullhus  (stop[1]:rsv[1]:0b00[2]:0b1001111[7]:isrc2[9]:nlidest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 27934 "cpu/st231_isa.cpp"
OpImullhus::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 27938 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 27942 "cpu/st231_isa.cpp"
)
{

#line 1203 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = (uint16_t)cpu->Imm(isrc2);
        int64_t result = op1 * op2;
        cpu->SetGPR_N(nlidest, result >> 32);
}
#line 27953 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 27958 "cpu/st231_isa.cpp"
OpImullhus::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 27962 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 27966 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 27971 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 27975 "cpu/st231_isa.cpp"
)
{

#line 1210 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "mullhus $r" << nlidest << " = $r" << rsc1 << ", " << op2 ;
}
#line 27984 "cpu/st231_isa.cpp"
}
/* ---  integer immediate multiplication instructions ------------------ */

void
OpImullhus::initialize_operands()
{

#line 213 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, nlidest);
}
#line 27998 "cpu/st231_isa.cpp"
}

void
OpImullhus::initialize_latencies()
{

#line 141 "sim_latencies.isa"
{LAT(3);}
#line 28007 "cpu/st231_isa.cpp"
}

void
OpImullhus::initialize_function()
{

#line 45 "sim_functions.isa"
{ function = FnIntExtended; }
#line 28016 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpImullhus(st231::CodeType code, uint32_t addr)
{
	return new OpImullhus(code, addr);
}

// op ixor(stop[1]:rsv[1]:0b00[2]:0b1001101[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28028 "cpu/st231_isa.cpp"
OpIxor::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28032 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28036 "cpu/st231_isa.cpp"
)
{

#line 1190 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, op1 ^ op2);
}
#line 28046 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28051 "cpu/st231_isa.cpp"
OpIxor::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28055 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28059 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28064 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28068 "cpu/st231_isa.cpp"
)
{

#line 1196 "integer.isa"
{
	os << "xor $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 28076 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIxor::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28091 "cpu/st231_isa.cpp"
}

void
OpIxor::initialize_latencies()
{

#line 80 "sim_latencies.isa"
{LAT(1);}
#line 28100 "cpu/st231_isa.cpp"
}

void
OpIxor::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28109 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIxor(st231::CodeType code, uint32_t addr)
{
	return new OpIxor(code, addr);
}

// op iorc(stop[1]:rsv[1]:0b00[2]:0b1001100[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28121 "cpu/st231_isa.cpp"
OpIorc::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28125 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28129 "cpu/st231_isa.cpp"
)
{

#line 1179 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (~op1) | op2);
}
#line 28139 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28144 "cpu/st231_isa.cpp"
OpIorc::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28148 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28152 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28157 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28161 "cpu/st231_isa.cpp"
)
{

#line 1185 "integer.isa"
{
	os << "orc $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 28169 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIorc::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28184 "cpu/st231_isa.cpp"
}

void
OpIorc::initialize_latencies()
{

#line 79 "sim_latencies.isa"
{LAT(1);}
#line 28193 "cpu/st231_isa.cpp"
}

void
OpIorc::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28202 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIorc(st231::CodeType code, uint32_t addr)
{
	return new OpIorc(code, addr);
}

// op ior(stop[1]:rsv[1]:0b00[2]:0b1001011[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28214 "cpu/st231_isa.cpp"
OpIor::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28218 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28222 "cpu/st231_isa.cpp"
)
{

#line 1168 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, op1 | op2);
}
#line 28232 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28237 "cpu/st231_isa.cpp"
OpIor::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28241 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28245 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28250 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28254 "cpu/st231_isa.cpp"
)
{

#line 1174 "integer.isa"
{
	os << "or $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 28262 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIor::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28277 "cpu/st231_isa.cpp"
}

void
OpIor::initialize_latencies()
{

#line 78 "sim_latencies.isa"
{LAT(1);}
#line 28286 "cpu/st231_isa.cpp"
}

void
OpIor::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28295 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIor(st231::CodeType code, uint32_t addr)
{
	return new OpIor(code, addr);
}

// op iandc(stop[1]:rsv[1]:0b00[2]:0b1001010[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28307 "cpu/st231_isa.cpp"
OpIandc::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28311 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28315 "cpu/st231_isa.cpp"
)
{

#line 1157 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (~op1) & op2);
}
#line 28325 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28330 "cpu/st231_isa.cpp"
OpIandc::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28334 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28338 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28343 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28347 "cpu/st231_isa.cpp"
)
{

#line 1163 "integer.isa"
{
	os << "andc $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 28355 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIandc::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28370 "cpu/st231_isa.cpp"
}

void
OpIandc::initialize_latencies()
{

#line 77 "sim_latencies.isa"
{LAT(1);}
#line 28379 "cpu/st231_isa.cpp"
}

void
OpIandc::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28388 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIandc(st231::CodeType code, uint32_t addr)
{
	return new OpIandc(code, addr);
}

// op iand(stop[1]:rsv[1]:0b00[2]:0b1001001[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28400 "cpu/st231_isa.cpp"
OpIand::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28404 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28408 "cpu/st231_isa.cpp"
)
{

#line 1146 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, op1 & op2);
}
#line 28418 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28423 "cpu/st231_isa.cpp"
OpIand::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28427 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28431 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28436 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28440 "cpu/st231_isa.cpp"
)
{

#line 1152 "integer.isa"
{
	os << "and $r" << idest << " = $r" << rsc1 << ", " << isrc2 ;
}
#line 28448 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIand::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28463 "cpu/st231_isa.cpp"
}

void
OpIand::initialize_latencies()
{

#line 76 "sim_latencies.isa"
{LAT(1);}
#line 28472 "cpu/st231_isa.cpp"
}

void
OpIand::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28481 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIand(st231::CodeType code, uint32_t addr)
{
	return new OpIand(code, addr);
}

// op ish4add(stop[1]:rsv[1]:0b00[2]:0b1001000[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28493 "cpu/st231_isa.cpp"
OpIsh4add::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28497 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28501 "cpu/st231_isa.cpp"
)
{

#line 1134 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1<<4)+op2 );
}
#line 28511 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28516 "cpu/st231_isa.cpp"
OpIsh4add::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28520 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28524 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28529 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28533 "cpu/st231_isa.cpp"
)
{

#line 1140 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "sh4add $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 28542 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIsh4add::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28557 "cpu/st231_isa.cpp"
}

void
OpIsh4add::initialize_latencies()
{

#line 75 "sim_latencies.isa"
{LAT(1);}
#line 28566 "cpu/st231_isa.cpp"
}

void
OpIsh4add::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28575 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIsh4add(st231::CodeType code, uint32_t addr)
{
	return new OpIsh4add(code, addr);
}

// op ish3add(stop[1]:rsv[1]:0b00[2]:0b1000111[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28587 "cpu/st231_isa.cpp"
OpIsh3add::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28591 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28595 "cpu/st231_isa.cpp"
)
{

#line 1122 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1<<3)+op2 );
}
#line 28605 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28610 "cpu/st231_isa.cpp"
OpIsh3add::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28614 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28618 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28623 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28627 "cpu/st231_isa.cpp"
)
{

#line 1128 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "sh3add $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 28636 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIsh3add::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28651 "cpu/st231_isa.cpp"
}

void
OpIsh3add::initialize_latencies()
{

#line 74 "sim_latencies.isa"
{LAT(1);}
#line 28660 "cpu/st231_isa.cpp"
}

void
OpIsh3add::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28669 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIsh3add(st231::CodeType code, uint32_t addr)
{
	return new OpIsh3add(code, addr);
}

// op ish2add(stop[1]:rsv[1]:0b00[2]:0b1000110[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28681 "cpu/st231_isa.cpp"
OpIsh2add::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28685 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28689 "cpu/st231_isa.cpp"
)
{

#line 1110 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1<<2)+op2 );
}
#line 28699 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28704 "cpu/st231_isa.cpp"
OpIsh2add::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28708 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28712 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28717 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28721 "cpu/st231_isa.cpp"
)
{

#line 1116 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "sh2add $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 28730 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIsh2add::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28745 "cpu/st231_isa.cpp"
}

void
OpIsh2add::initialize_latencies()
{

#line 73 "sim_latencies.isa"
{LAT(1);}
#line 28754 "cpu/st231_isa.cpp"
}

void
OpIsh2add::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28763 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIsh2add(st231::CodeType code, uint32_t addr)
{
	return new OpIsh2add(code, addr);
}

// op ish1add(stop[1]:rsv[1]:0b00[2]:0b1000101[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28775 "cpu/st231_isa.cpp"
OpIsh1add::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28779 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28783 "cpu/st231_isa.cpp"
)
{

#line 1098 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, (op1<<1)+op2 );
}
#line 28793 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28798 "cpu/st231_isa.cpp"
OpIsh1add::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28802 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28806 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28811 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28815 "cpu/st231_isa.cpp"
)
{

#line 1104 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "sh1add $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 28824 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIsh1add::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28839 "cpu/st231_isa.cpp"
}

void
OpIsh1add::initialize_latencies()
{

#line 72 "sim_latencies.isa"
{LAT(1);}
#line 28848 "cpu/st231_isa.cpp"
}

void
OpIsh1add::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28857 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIsh1add(st231::CodeType code, uint32_t addr)
{
	return new OpIsh1add(code, addr);
}

// op ishru(stop[1]:rsv[1]:0b00[2]:0b1000100[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28869 "cpu/st231_isa.cpp"
OpIshru::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28873 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28877 "cpu/st231_isa.cpp"
)
{

#line 1086 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint8_t op2 = (uint8_t)cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, op1>>op2 );
}
#line 28887 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28892 "cpu/st231_isa.cpp"
OpIshru::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28896 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28900 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28905 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 28909 "cpu/st231_isa.cpp"
)
{

#line 1092 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "shru $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 28918 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIshru::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 28933 "cpu/st231_isa.cpp"
}

void
OpIshru::initialize_latencies()
{

#line 71 "sim_latencies.isa"
{LAT(1);}
#line 28942 "cpu/st231_isa.cpp"
}

void
OpIshru::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 28951 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIshru(st231::CodeType code, uint32_t addr)
{
	return new OpIshru(code, addr);
}

// op ishr(stop[1]:rsv[1]:0b00[2]:0b1000011[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 28963 "cpu/st231_isa.cpp"
OpIshr::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 28967 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 28971 "cpu/st231_isa.cpp"
)
{

#line 1074 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        uint8_t op2 = (uint8_t)cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, op1>>op2 );
}
#line 28981 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 28986 "cpu/st231_isa.cpp"
OpIshr::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 28990 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 28994 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 28999 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29003 "cpu/st231_isa.cpp"
)
{

#line 1080 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "shr $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 29012 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIshr::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 29027 "cpu/st231_isa.cpp"
}

void
OpIshr::initialize_latencies()
{

#line 70 "sim_latencies.isa"
{LAT(1);}
#line 29036 "cpu/st231_isa.cpp"
}

void
OpIshr::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 29045 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIshr(st231::CodeType code, uint32_t addr)
{
	return new OpIshr(code, addr);
}

// op ishl(stop[1]:rsv[1]:0b00[2]:0b1000010[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 29057 "cpu/st231_isa.cpp"
OpIshl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29061 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29065 "cpu/st231_isa.cpp"
)
{

#line 1059 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        uint8_t op2 = (uint8_t)cpu->Imm(isrc2);
        if(op2>31)
          cpu->SetGPR_N(idest, 0 );
        else  
          cpu->SetGPR_N(idest, op1<<op2 );
}
#line 29078 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29083 "cpu/st231_isa.cpp"
OpIshl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29087 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29091 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29096 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29100 "cpu/st231_isa.cpp"
)
{

#line 1068 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "shl $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 29109 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIshl::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 29124 "cpu/st231_isa.cpp"
}

void
OpIshl::initialize_latencies()
{

#line 69 "sim_latencies.isa"
{LAT(1);}
#line 29133 "cpu/st231_isa.cpp"
}

void
OpIshl::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 29142 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIshl(st231::CodeType code, uint32_t addr)
{
	return new OpIshl(code, addr);
}

// op isub(stop[1]:rsv[1]:0b00[2]:0b1000001[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 29154 "cpu/st231_isa.cpp"
OpIsub::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29158 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29162 "cpu/st231_isa.cpp"
)
{

#line 1047 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, op1 - op2 );
}
#line 29172 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29177 "cpu/st231_isa.cpp"
OpIsub::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29181 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29185 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29190 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29194 "cpu/st231_isa.cpp"
)
{

#line 1053 "integer.isa"
{
        int32_t op2 = SignEx9to32(isrc2);
	os << "sub $r" << idest << " = $r" << rsc1 << ", " << op2 ;
}
#line 29203 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIsub::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 29218 "cpu/st231_isa.cpp"
}

void
OpIsub::initialize_latencies()
{

#line 68 "sim_latencies.isa"
{LAT(1);}
#line 29227 "cpu/st231_isa.cpp"
}

void
OpIsub::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 29236 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIsub(st231::CodeType code, uint32_t addr)
{
	return new OpIsub(code, addr);
}

//************************************************************************
// the immediate format operations
//************************************************************************
//************************************************************************
// the immediate format operations
//************************************************************************
// op iadd(stop[1]:rsv[1]:0b00[2]:0b1000000[7]:isrc2[9]:idest[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 29254 "cpu/st231_isa.cpp"
OpIadd::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29258 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29262 "cpu/st231_isa.cpp"
)
{

#line 1031 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->Imm(isrc2);
        cpu->SetGPR_N(idest, op1 + op2 );
}
#line 29272 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29277 "cpu/st231_isa.cpp"
OpIadd::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29281 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29285 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29290 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29294 "cpu/st231_isa.cpp"
)
{

#line 1037 "integer.isa"
{
        //int32_t op2 = SignEx9to32(isrc2);
        int32_t op2 = cpu->Imm(isrc2);
	os << "iadd $r" << idest << " = $r" << rsc1 << ", " << op2 ;
        #if DEBUG
	os << "  //  " << cpu->GetGPR_N(idest) << " = " << cpu->GetGPR_C(rsc1) << " + " << op2 ;
        #endif
}
#line 29307 "cpu/st231_isa.cpp"
}
/* === Integer Immediate INSTRUCTIONS ================================== */
/* ---  integer immediate simple arithmetic instructions --------------- */

void
OpIadd::initialize_operands()
{

#line 188 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_OUTPUT(GPR_T, idest);
}
#line 29322 "cpu/st231_isa.cpp"
}

void
OpIadd::initialize_latencies()
{

#line 67 "sim_latencies.isa"
{LAT(1);}
#line 29331 "cpu/st231_isa.cpp"
}

void
OpIadd::initialize_function()
{

#line 33 "sim_functions.isa"
{ function = FnIntBasic; }
#line 29340 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpIadd(st231::CodeType code, uint32_t addr)
{
	return new OpIadd(code, addr);
}

// op mulfrac(stop[1]:rsv[1]:0b00[2]:0b0111111[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 29352 "cpu/st231_isa.cpp"
OpMulfrac::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29356 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29360 "cpu/st231_isa.cpp"
)
{

#line 1005 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        int32_t result;

        if((-op1 == 0x80000000) && (-op2 == 0x80000000))
        {   cpu->SetGPR_N(nldest, 0x7fffffff );
        }

        else
        {   result = op1 * op2;
            result = op1 + (1 << 30);
            result = result >> 31;
            cpu->SetGPR_N(nldest, result );
        }
}
#line 29381 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29386 "cpu/st231_isa.cpp"
OpMulfrac::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29390 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29394 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29399 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29403 "cpu/st231_isa.cpp"
)
{

#line 1022 "integer.isa"
{
	os << "mulfrac $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 29411 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMulfrac::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 29426 "cpu/st231_isa.cpp"
}

void
OpMulfrac::initialize_latencies()
{

#line 140 "sim_latencies.isa"
{LAT(3);}
#line 29435 "cpu/st231_isa.cpp"
}

void
OpMulfrac::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 29444 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMulfrac(st231::CodeType code, uint32_t addr)
{
	return new OpMulfrac(code, addr);
}

// new multiply instructions for ST231 : the destination register is "nldest"
// // new multiply instructions for ST231 : the destination register is "nldest"
// op mul64hu(stop[1]:rsv[1]:0b00[2]:0b0111110[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 29458 "cpu/st231_isa.cpp"
OpMul64hu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29462 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29466 "cpu/st231_isa.cpp"
)
{

#line 993 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        uint64_t result = op1* op2;
        cpu->SetGPR_N(nldest, result >> 32 );
}
#line 29477 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29482 "cpu/st231_isa.cpp"
OpMul64hu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29486 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29490 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29495 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29499 "cpu/st231_isa.cpp"
)
{

#line 1000 "integer.isa"
{
	os << "mul64hu $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 29507 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMul64hu::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 29522 "cpu/st231_isa.cpp"
}

void
OpMul64hu::initialize_latencies()
{

#line 139 "sim_latencies.isa"
{LAT(3);}
#line 29531 "cpu/st231_isa.cpp"
}

void
OpMul64hu::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 29540 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMul64hu(st231::CodeType code, uint32_t addr)
{
	return new OpMul64hu(code, addr);
}

// op bnorl(stop[1]:rsv[1]:0b00[2]:0b0111101[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 29552 "cpu/st231_isa.cpp"
OpBnorl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29556 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29560 "cpu/st231_isa.cpp"
)
{

#line 980 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, ((op1==0) && (op2==0)) );
}
#line 29570 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29575 "cpu/st231_isa.cpp"
OpBnorl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29579 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29583 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29588 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29592 "cpu/st231_isa.cpp"
)
{

#line 986 "integer.isa"
{
	os << "norl $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 29600 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBnorl::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 29615 "cpu/st231_isa.cpp"
}

void
OpBnorl::initialize_latencies()
{

#line 66 "sim_latencies.isa"
{LAT(1);}
#line 29624 "cpu/st231_isa.cpp"
}

void
OpBnorl::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 29633 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBnorl(st231::CodeType code, uint32_t addr)
{
	return new OpBnorl(code, addr);
}

// op borl(stop[1]:rsv[1]:0b00[2]:0b0111100[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 29645 "cpu/st231_isa.cpp"
OpBorl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29649 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29653 "cpu/st231_isa.cpp"
)
{

#line 969 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 || op2) );
}
#line 29663 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29668 "cpu/st231_isa.cpp"
OpBorl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29672 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29676 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29681 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29685 "cpu/st231_isa.cpp"
)
{

#line 975 "integer.isa"
{
	os << "orl $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 29693 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBorl::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 29708 "cpu/st231_isa.cpp"
}

void
OpBorl::initialize_latencies()
{

#line 65 "sim_latencies.isa"
{LAT(1);}
#line 29717 "cpu/st231_isa.cpp"
}

void
OpBorl::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 29726 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBorl(st231::CodeType code, uint32_t addr)
{
	return new OpBorl(code, addr);
}

// op bnandl(stop[1]:rsv[1]:0b00[2]:0b0111011[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 29738 "cpu/st231_isa.cpp"
OpBnandl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29742 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29746 "cpu/st231_isa.cpp"
)
{

#line 958 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1==0) || (op2==0) );
}
#line 29756 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29761 "cpu/st231_isa.cpp"
OpBnandl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29765 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29769 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29774 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29778 "cpu/st231_isa.cpp"
)
{

#line 964 "integer.isa"
{
	os << "nandl $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 29786 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBnandl::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 29801 "cpu/st231_isa.cpp"
}

void
OpBnandl::initialize_latencies()
{

#line 64 "sim_latencies.isa"
{LAT(1);}
#line 29810 "cpu/st231_isa.cpp"
}

void
OpBnandl::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 29819 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBnandl(st231::CodeType code, uint32_t addr)
{
	return new OpBnandl(code, addr);
}

// op bandl(stop[1]:rsv[1]:0b00[2]:0b0111010[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 29831 "cpu/st231_isa.cpp"
OpBandl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29835 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29839 "cpu/st231_isa.cpp"
)
{

#line 947 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 && op2) );
}
#line 29849 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29854 "cpu/st231_isa.cpp"
OpBandl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29858 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29862 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29867 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29871 "cpu/st231_isa.cpp"
)
{

#line 953 "integer.isa"
{
	os << "andl $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 29879 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBandl::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 29894 "cpu/st231_isa.cpp"
}

void
OpBandl::initialize_latencies()
{

#line 63 "sim_latencies.isa"
{LAT(1);}
#line 29903 "cpu/st231_isa.cpp"
}

void
OpBandl::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 29912 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBandl(st231::CodeType code, uint32_t addr)
{
	return new OpBandl(code, addr);
}

// op bcmpltu(stop[1]:rsv[1]:0b00[2]:0b0111001[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 29924 "cpu/st231_isa.cpp"
OpBcmpltu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 29928 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 29932 "cpu/st231_isa.cpp"
)
{

#line 936 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 < op2) );
}
#line 29942 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 29947 "cpu/st231_isa.cpp"
OpBcmpltu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 29951 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 29955 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 29960 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 29964 "cpu/st231_isa.cpp"
)
{

#line 942 "integer.isa"
{
	os << "cmpltu $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 29972 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmpltu::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 29987 "cpu/st231_isa.cpp"
}

void
OpBcmpltu::initialize_latencies()
{

#line 62 "sim_latencies.isa"
{LAT(1);}
#line 29996 "cpu/st231_isa.cpp"
}

void
OpBcmpltu::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30005 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmpltu(st231::CodeType code, uint32_t addr)
{
	return new OpBcmpltu(code, addr);
}

// op bcmplt(stop[1]:rsv[1]:0b00[2]:0b0111000[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30017 "cpu/st231_isa.cpp"
OpBcmplt::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30021 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30025 "cpu/st231_isa.cpp"
)
{

#line 925 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 < op2) );
}
#line 30035 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30040 "cpu/st231_isa.cpp"
OpBcmplt::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30044 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30048 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30053 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30057 "cpu/st231_isa.cpp"
)
{

#line 931 "integer.isa"
{
	os << "cmplt $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30065 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmplt::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 30080 "cpu/st231_isa.cpp"
}

void
OpBcmplt::initialize_latencies()
{

#line 61 "sim_latencies.isa"
{LAT(1);}
#line 30089 "cpu/st231_isa.cpp"
}

void
OpBcmplt::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30098 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmplt(st231::CodeType code, uint32_t addr)
{
	return new OpBcmplt(code, addr);
}

// op bcmpleu(stop[1]:rsv[1]:0b00[2]:0b0110111[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30110 "cpu/st231_isa.cpp"
OpBcmpleu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30114 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30118 "cpu/st231_isa.cpp"
)
{

#line 914 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 <= op2) );
}
#line 30128 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30133 "cpu/st231_isa.cpp"
OpBcmpleu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30137 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30141 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30146 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30150 "cpu/st231_isa.cpp"
)
{

#line 920 "integer.isa"
{
	os << "bcmpleu $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30158 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmpleu::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 30173 "cpu/st231_isa.cpp"
}

void
OpBcmpleu::initialize_latencies()
{

#line 60 "sim_latencies.isa"
{LAT(1);}
#line 30182 "cpu/st231_isa.cpp"
}

void
OpBcmpleu::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30191 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmpleu(st231::CodeType code, uint32_t addr)
{
	return new OpBcmpleu(code, addr);
}

// op bcmple(stop[1]:rsv[1]:0b00[2]:0b0110110[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30203 "cpu/st231_isa.cpp"
OpBcmple::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30207 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30211 "cpu/st231_isa.cpp"
)
{

#line 903 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 <= op2) );
}
#line 30221 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30226 "cpu/st231_isa.cpp"
OpBcmple::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30230 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30234 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30239 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30243 "cpu/st231_isa.cpp"
)
{

#line 909 "integer.isa"
{
	os << "bcmple $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30251 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmple::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 30266 "cpu/st231_isa.cpp"
}

void
OpBcmple::initialize_latencies()
{

#line 59 "sim_latencies.isa"
{LAT(1);}
#line 30275 "cpu/st231_isa.cpp"
}

void
OpBcmple::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30284 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmple(st231::CodeType code, uint32_t addr)
{
	return new OpBcmple(code, addr);
}

// op bcmpgtu(stop[1]:rsv[1]:0b00[2]:0b0110101[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30296 "cpu/st231_isa.cpp"
OpBcmpgtu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30300 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30304 "cpu/st231_isa.cpp"
)
{

#line 892 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 > op2) );
}
#line 30314 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30319 "cpu/st231_isa.cpp"
OpBcmpgtu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30323 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30327 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30332 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30336 "cpu/st231_isa.cpp"
)
{

#line 898 "integer.isa"
{
	os << "cmpgtu $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30344 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmpgtu::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 30359 "cpu/st231_isa.cpp"
}

void
OpBcmpgtu::initialize_latencies()
{

#line 58 "sim_latencies.isa"
{LAT(1);}
#line 30368 "cpu/st231_isa.cpp"
}

void
OpBcmpgtu::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30377 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmpgtu(st231::CodeType code, uint32_t addr)
{
	return new OpBcmpgtu(code, addr);
}

// op bcmpgt(stop[1]:rsv[1]:0b00[2]:0b0110100[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30389 "cpu/st231_isa.cpp"
OpBcmpgt::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30393 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30397 "cpu/st231_isa.cpp"
)
{

#line 881 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 > op2) );
}
#line 30407 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30412 "cpu/st231_isa.cpp"
OpBcmpgt::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30416 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30420 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30425 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30429 "cpu/st231_isa.cpp"
)
{

#line 887 "integer.isa"
{
	os << "cmpgt $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30437 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmpgt::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 30452 "cpu/st231_isa.cpp"
}

void
OpBcmpgt::initialize_latencies()
{

#line 57 "sim_latencies.isa"
{LAT(1);}
#line 30461 "cpu/st231_isa.cpp"
}

void
OpBcmpgt::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30470 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmpgt(st231::CodeType code, uint32_t addr)
{
	return new OpBcmpgt(code, addr);
}

// op bcmpgeu(stop[1]:rsv[1]:0b00[2]:0b0110011[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30482 "cpu/st231_isa.cpp"
OpBcmpgeu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30486 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30490 "cpu/st231_isa.cpp"
)
{

#line 870 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 >= op2) );
}
#line 30500 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30505 "cpu/st231_isa.cpp"
OpBcmpgeu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30509 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30513 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30518 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30522 "cpu/st231_isa.cpp"
)
{

#line 876 "integer.isa"
{
	os << "cmpgeu $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30530 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmpgeu::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 30545 "cpu/st231_isa.cpp"
}

void
OpBcmpgeu::initialize_latencies()
{

#line 55 "sim_latencies.isa"
{LAT(1);}
#line 30554 "cpu/st231_isa.cpp"
}

void
OpBcmpgeu::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30563 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmpgeu(st231::CodeType code, uint32_t addr)
{
	return new OpBcmpgeu(code, addr);
}

// op bcmpge(stop[1]:rsv[1]:0b00[2]:0b0110010[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30575 "cpu/st231_isa.cpp"
OpBcmpge::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30579 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30583 "cpu/st231_isa.cpp"
)
{

#line 859 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 >= op2) );
}
#line 30593 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30598 "cpu/st231_isa.cpp"
OpBcmpge::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30602 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30606 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30611 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30615 "cpu/st231_isa.cpp"
)
{

#line 865 "integer.isa"
{
	os << "cmpge $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30623 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmpge::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 30638 "cpu/st231_isa.cpp"
}

void
OpBcmpge::initialize_latencies()
{

#line 56 "sim_latencies.isa"
{LAT(1);}
#line 30647 "cpu/st231_isa.cpp"
}

void
OpBcmpge::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30656 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmpge(st231::CodeType code, uint32_t addr)
{
	return new OpBcmpge(code, addr);
}

// op bcmpne(stop[1]:rsv[1]:0b00[2]:0b0110001[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30668 "cpu/st231_isa.cpp"
OpBcmpne::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30672 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30676 "cpu/st231_isa.cpp"
)
{

#line 848 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 != op2) );
}
#line 30686 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30691 "cpu/st231_isa.cpp"
OpBcmpne::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30695 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30699 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30704 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30708 "cpu/st231_isa.cpp"
)
{

#line 854 "integer.isa"
{
	os << "cmpne $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30716 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmpne::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 30731 "cpu/st231_isa.cpp"
}

void
OpBcmpne::initialize_latencies()
{

#line 54 "sim_latencies.isa"
{LAT(1);}
#line 30740 "cpu/st231_isa.cpp"
}

void
OpBcmpne::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30749 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmpne(st231::CodeType code, uint32_t addr)
{
	return new OpBcmpne(code, addr);
}

// op bcmpeq(stop[1]:rsv[1]:0b00[2]:0b0110000[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30761 "cpu/st231_isa.cpp"
OpBcmpeq::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30765 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30769 "cpu/st231_isa.cpp"
)
{

#line 837 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPB_N(bdest, (op1 == op2) );
}
#line 30779 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30784 "cpu/st231_isa.cpp"
OpBcmpeq::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30788 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30792 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30797 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30801 "cpu/st231_isa.cpp"
)
{

#line 843 "integer.isa"
{
	os << "cmpeq $b" << bdest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30809 "cpu/st231_isa.cpp"
}
/* ---  integer register conditional branch instructions --------------- */

void
OpBcmpeq::initialize_operands()
{

#line 146 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPB_T, bdest);
}
#line 30824 "cpu/st231_isa.cpp"
}

void
OpBcmpeq::initialize_latencies()
{

#line 53 "sim_latencies.isa"
{LAT(1);}
#line 30833 "cpu/st231_isa.cpp"
}

void
OpBcmpeq::initialize_function()
{

#line 34 "sim_functions.isa"
{ function = FnIntBasic; }
#line 30842 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpBcmpeq(st231::CodeType code, uint32_t addr)
{
	return new OpBcmpeq(code, addr);
}

// op mul64h(stop[1]:rsv[1]:0b00[2]:0b0101111[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30854 "cpu/st231_isa.cpp"
OpMul64h::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30858 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30862 "cpu/st231_isa.cpp"
)
{

#line 824 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        int64_t result = op1 * op2;
        cpu->SetGPR_N(nldest, result >> 32);
}
#line 30873 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30878 "cpu/st231_isa.cpp"
OpMul64h::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30882 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30886 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30891 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30895 "cpu/st231_isa.cpp"
)
{

#line 831 "integer.isa"
{
	os << "mul64h $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30903 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMul64h::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 30918 "cpu/st231_isa.cpp"
}

void
OpMul64h::initialize_latencies()
{

#line 138 "sim_latencies.isa"
{LAT(3);}
#line 30927 "cpu/st231_isa.cpp"
}

void
OpMul64h::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 30936 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMul64h(st231::CodeType code, uint32_t addr)
{
	return new OpMul64h(code, addr);
}

// new multiply instructions for ST231 : the destination register is "nldest"
// // new multiply instructions for ST231 : the destination register is "nldest"
// op mul32(stop[1]:rsv[1]:0b00[2]:0b0101110[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 30950 "cpu/st231_isa.cpp"
OpMul32::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 30954 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 30958 "cpu/st231_isa.cpp"
)
{

#line 813 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(nldest, op1 * op2 );
}
#line 30968 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 30973 "cpu/st231_isa.cpp"
OpMul32::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 30977 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 30981 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 30986 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 30990 "cpu/st231_isa.cpp"
)
{

#line 819 "integer.isa"
{
	os << "mul32 $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 30998 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMul32::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 31013 "cpu/st231_isa.cpp"
}

void
OpMul32::initialize_latencies()
{

#line 137 "sim_latencies.isa"
{LAT(3);}
#line 31022 "cpu/st231_isa.cpp"
}

void
OpMul32::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 31031 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMul32(st231::CodeType code, uint32_t addr)
{
	return new OpMul32(code, addr);
}

// op norl(stop[1]:rsv[1]:0b00[2]:0b0101101[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31043 "cpu/st231_isa.cpp"
OpNorl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31047 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31051 "cpu/st231_isa.cpp"
)
{

#line 801 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, ((op1==0) && (op2==0)) );
}
#line 31061 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31066 "cpu/st231_isa.cpp"
OpNorl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31070 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31074 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31079 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31083 "cpu/st231_isa.cpp"
)
{

#line 807 "integer.isa"
{
	os << "norl $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31091 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpNorl::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31110 "cpu/st231_isa.cpp"
}

void
OpNorl::initialize_latencies()
{

#line 52 "sim_latencies.isa"
{LAT(1);}
#line 31119 "cpu/st231_isa.cpp"
}

void
OpNorl::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 31128 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpNorl(st231::CodeType code, uint32_t addr)
{
	return new OpNorl(code, addr);
}

// op orl(stop[1]:rsv[1]:0b00[2]:0b0101100[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31140 "cpu/st231_isa.cpp"
OpOrl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31144 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31148 "cpu/st231_isa.cpp"
)
{

#line 790 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 || op2) );
}
#line 31158 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31163 "cpu/st231_isa.cpp"
OpOrl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31167 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31171 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31176 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31180 "cpu/st231_isa.cpp"
)
{

#line 796 "integer.isa"
{
	os << "orl $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31188 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpOrl::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31207 "cpu/st231_isa.cpp"
}

void
OpOrl::initialize_latencies()
{

#line 51 "sim_latencies.isa"
{LAT(1);}
#line 31216 "cpu/st231_isa.cpp"
}

void
OpOrl::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 31225 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpOrl(st231::CodeType code, uint32_t addr)
{
	return new OpOrl(code, addr);
}

// op nandl(stop[1]:rsv[1]:0b00[2]:0b0101011[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31237 "cpu/st231_isa.cpp"
OpNandl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31241 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31245 "cpu/st231_isa.cpp"
)
{

#line 779 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1==0) || (op2==0) );
}
#line 31255 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31260 "cpu/st231_isa.cpp"
OpNandl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31264 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31268 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31273 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31277 "cpu/st231_isa.cpp"
)
{

#line 785 "integer.isa"
{
	os << "nandl $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31285 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpNandl::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31304 "cpu/st231_isa.cpp"
}

void
OpNandl::initialize_latencies()
{

#line 50 "sim_latencies.isa"
{LAT(1);}
#line 31313 "cpu/st231_isa.cpp"
}

void
OpNandl::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 31322 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpNandl(st231::CodeType code, uint32_t addr)
{
	return new OpNandl(code, addr);
}

// op andl(stop[1]:rsv[1]:0b00[2]:0b0101010[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31334 "cpu/st231_isa.cpp"
OpAndl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31338 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31342 "cpu/st231_isa.cpp"
)
{

#line 768 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 && op2) );
}
#line 31352 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31357 "cpu/st231_isa.cpp"
OpAndl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31361 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31365 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31370 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31374 "cpu/st231_isa.cpp"
)
{

#line 774 "integer.isa"
{
	os << "andl $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31382 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpAndl::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31401 "cpu/st231_isa.cpp"
}

void
OpAndl::initialize_latencies()
{

#line 49 "sim_latencies.isa"
{LAT(1);}
#line 31410 "cpu/st231_isa.cpp"
}

void
OpAndl::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 31419 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpAndl(st231::CodeType code, uint32_t addr)
{
	return new OpAndl(code, addr);
}

// op cmpltu(stop[1]:rsv[1]:0b00[2]:0b0101001[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31431 "cpu/st231_isa.cpp"
OpCmpltu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31435 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31439 "cpu/st231_isa.cpp"
)
{

#line 757 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 < op2) );
}
#line 31449 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31454 "cpu/st231_isa.cpp"
OpCmpltu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31458 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31462 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31467 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31471 "cpu/st231_isa.cpp"
)
{

#line 763 "integer.isa"
{
	os << "cmpltu $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31479 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmpltu::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31498 "cpu/st231_isa.cpp"
}

void
OpCmpltu::initialize_latencies()
{

#line 48 "sim_latencies.isa"
{LAT(1);}
#line 31507 "cpu/st231_isa.cpp"
}

void
OpCmpltu::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 31516 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmpltu(st231::CodeType code, uint32_t addr)
{
	return new OpCmpltu(code, addr);
}

// op cmplt(stop[1]:rsv[1]:0b00[2]:0b0101000[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31528 "cpu/st231_isa.cpp"
OpCmplt::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31532 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31536 "cpu/st231_isa.cpp"
)
{

#line 746 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 < op2) );
}
#line 31546 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31551 "cpu/st231_isa.cpp"
OpCmplt::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31555 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31559 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31564 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31568 "cpu/st231_isa.cpp"
)
{

#line 752 "integer.isa"
{
	os << "cmplt $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31576 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmplt::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31595 "cpu/st231_isa.cpp"
}

void
OpCmplt::initialize_latencies()
{

#line 47 "sim_latencies.isa"
{LAT(1);}
#line 31604 "cpu/st231_isa.cpp"
}

void
OpCmplt::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 31613 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmplt(st231::CodeType code, uint32_t addr)
{
	return new OpCmplt(code, addr);
}

// op cmpleu(stop[1]:rsv[1]:0b00[2]:0b0100111[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31625 "cpu/st231_isa.cpp"
OpCmpleu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31629 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31633 "cpu/st231_isa.cpp"
)
{

#line 735 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 <= op2) );
}
#line 31643 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31648 "cpu/st231_isa.cpp"
OpCmpleu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31652 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31656 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31661 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31665 "cpu/st231_isa.cpp"
)
{

#line 741 "integer.isa"
{
	os << "cmpleu $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31673 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmpleu::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31692 "cpu/st231_isa.cpp"
}

void
OpCmpleu::initialize_latencies()
{

#line 46 "sim_latencies.isa"
{LAT(1);}
#line 31701 "cpu/st231_isa.cpp"
}

void
OpCmpleu::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 31710 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmpleu(st231::CodeType code, uint32_t addr)
{
	return new OpCmpleu(code, addr);
}

// op cmple(stop[1]:rsv[1]:0b00[2]:0b0100110[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31722 "cpu/st231_isa.cpp"
OpCmple::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31726 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31730 "cpu/st231_isa.cpp"
)
{

#line 724 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 <= op2) );
}
#line 31740 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31745 "cpu/st231_isa.cpp"
OpCmple::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31749 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31753 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31758 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31762 "cpu/st231_isa.cpp"
)
{

#line 730 "integer.isa"
{
	os << "cmple $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31770 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmple::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31789 "cpu/st231_isa.cpp"
}

void
OpCmple::initialize_latencies()
{

#line 45 "sim_latencies.isa"
{LAT(1);}
#line 31798 "cpu/st231_isa.cpp"
}

void
OpCmple::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 31807 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmple(st231::CodeType code, uint32_t addr)
{
	return new OpCmple(code, addr);
}

// op cmpgtu(stop[1]:rsv[1]:0b00[2]:0b0100101[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31819 "cpu/st231_isa.cpp"
OpCmpgtu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31823 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31827 "cpu/st231_isa.cpp"
)
{

#line 713 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 > op2) );
}
#line 31837 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31842 "cpu/st231_isa.cpp"
OpCmpgtu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31846 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31850 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31855 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31859 "cpu/st231_isa.cpp"
)
{

#line 719 "integer.isa"
{
	os << "cmpgtu $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31867 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmpgtu::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31886 "cpu/st231_isa.cpp"
}

void
OpCmpgtu::initialize_latencies()
{

#line 44 "sim_latencies.isa"
{LAT(1);}
#line 31895 "cpu/st231_isa.cpp"
}

void
OpCmpgtu::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 31904 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmpgtu(st231::CodeType code, uint32_t addr)
{
	return new OpCmpgtu(code, addr);
}

// op cmpgt(stop[1]:rsv[1]:0b00[2]:0b0100100[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 31916 "cpu/st231_isa.cpp"
OpCmpgt::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 31920 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 31924 "cpu/st231_isa.cpp"
)
{

#line 702 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 > op2) );
}
#line 31934 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 31939 "cpu/st231_isa.cpp"
OpCmpgt::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 31943 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 31947 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 31952 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 31956 "cpu/st231_isa.cpp"
)
{

#line 708 "integer.isa"
{
	os << "cmpgt $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 31964 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmpgt::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 31983 "cpu/st231_isa.cpp"
}

void
OpCmpgt::initialize_latencies()
{

#line 43 "sim_latencies.isa"
{LAT(1);}
#line 31992 "cpu/st231_isa.cpp"
}

void
OpCmpgt::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 32001 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmpgt(st231::CodeType code, uint32_t addr)
{
	return new OpCmpgt(code, addr);
}

// op cmpgeu(stop[1]:rsv[1]:0b00[2]:0b0100011[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32013 "cpu/st231_isa.cpp"
OpCmpgeu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32017 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32021 "cpu/st231_isa.cpp"
)
{

#line 691 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 >= op2) );
}
#line 32031 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32036 "cpu/st231_isa.cpp"
OpCmpgeu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32040 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32044 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32049 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32053 "cpu/st231_isa.cpp"
)
{

#line 697 "integer.isa"
{
	os << "cmpgeu $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 32061 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmpgeu::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 32080 "cpu/st231_isa.cpp"
}

void
OpCmpgeu::initialize_latencies()
{

#line 42 "sim_latencies.isa"
{LAT(1);}
#line 32089 "cpu/st231_isa.cpp"
}

void
OpCmpgeu::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 32098 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmpgeu(st231::CodeType code, uint32_t addr)
{
	return new OpCmpgeu(code, addr);
}

// op cmpge(stop[1]:rsv[1]:0b00[2]:0b0100010[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32110 "cpu/st231_isa.cpp"
OpCmpge::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32114 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32118 "cpu/st231_isa.cpp"
)
{

#line 680 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 >= op2) );
}
#line 32128 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32133 "cpu/st231_isa.cpp"
OpCmpge::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32137 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32141 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32146 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32150 "cpu/st231_isa.cpp"
)
{

#line 686 "integer.isa"
{
	os << "cmpge $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 32158 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmpge::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 32177 "cpu/st231_isa.cpp"
}

void
OpCmpge::initialize_latencies()
{

#line 41 "sim_latencies.isa"
{LAT(1);}
#line 32186 "cpu/st231_isa.cpp"
}

void
OpCmpge::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 32195 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmpge(st231::CodeType code, uint32_t addr)
{
	return new OpCmpge(code, addr);
}

// op cmpne(stop[1]:rsv[1]:0b00[2]:0b0100001[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32207 "cpu/st231_isa.cpp"
OpCmpne::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32211 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32215 "cpu/st231_isa.cpp"
)
{

#line 669 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 != op2) );
}
#line 32225 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32230 "cpu/st231_isa.cpp"
OpCmpne::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32234 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32238 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32243 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32247 "cpu/st231_isa.cpp"
)
{

#line 675 "integer.isa"
{
	os << "cmpne $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 32255 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmpne::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 32274 "cpu/st231_isa.cpp"
}

void
OpCmpne::initialize_latencies()
{

#line 40 "sim_latencies.isa"
{LAT(1);}
#line 32283 "cpu/st231_isa.cpp"
}

void
OpCmpne::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 32292 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmpne(st231::CodeType code, uint32_t addr)
{
	return new OpCmpne(code, addr);
}

// op cmpeq(stop[1]:rsv[1]:0b00[2]:0b0100000[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32304 "cpu/st231_isa.cpp"
OpCmpeq::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32308 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32312 "cpu/st231_isa.cpp"
)
{

#line 658 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1 == op2) );
}
#line 32322 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32327 "cpu/st231_isa.cpp"
OpCmpeq::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32331 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32335 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32340 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32344 "cpu/st231_isa.cpp"
)
{

#line 664 "integer.isa"
{
	os << "cmpeq $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
}
#line 32352 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpCmpeq::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 32371 "cpu/st231_isa.cpp"
}

void
OpCmpeq::initialize_latencies()
{

#line 39 "sim_latencies.isa"
{LAT(1);}
#line 32380 "cpu/st231_isa.cpp"
}

void
OpCmpeq::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 32389 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpCmpeq(st231::CodeType code, uint32_t addr)
{
	return new OpCmpeq(code, addr);
}

// op mulhs(stop[1]:rsv[1]:0b00[2]:0b0011111[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32401 "cpu/st231_isa.cpp"
OpMulhs::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32405 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32409 "cpu/st231_isa.cpp"
)
{

#line 643 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = (cpu->GetGPR_C(rsc2))>>16;
        cpu->SetGPR_N(nldest, (op1 * op2)<<16);
}
#line 32419 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32424 "cpu/st231_isa.cpp"
OpMulhs::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32428 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32432 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32437 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32441 "cpu/st231_isa.cpp"
)
{

#line 649 "integer.isa"
{
	os << "mulhs $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(nldest) <<" = (" <<cpu->GetGPR_C(rsc1) <<" * " <<(uint16_t)((cpu->GetGPR_C(rsc2))>>16) <<") <<16"  ;
        #endif
}
#line 32452 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMulhs::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 32467 "cpu/st231_isa.cpp"
}

void
OpMulhs::initialize_latencies()
{

#line 136 "sim_latencies.isa"
{LAT(3);}
#line 32476 "cpu/st231_isa.cpp"
}

void
OpMulhs::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 32485 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMulhs(st231::CodeType code, uint32_t addr)
{
	return new OpMulhs(code, addr);
}

// op mulhhu(stop[1]:rsv[1]:0b00[2]:0b0011110[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32497 "cpu/st231_isa.cpp"
OpMulhhu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32501 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32505 "cpu/st231_isa.cpp"
)
{

#line 629 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1)>>16;
        uint32_t op2 = cpu->GetGPR_C(rsc2)>>16;
        cpu->SetGPR_N(nldest, op1 * op2);
}
#line 32515 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32520 "cpu/st231_isa.cpp"
OpMulhhu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32524 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32528 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32533 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32537 "cpu/st231_isa.cpp"
)
{

#line 635 "integer.isa"
{
	os << "mulhhu $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<(uint32_t)cpu->GetGPR_N(nldest) <<" = " <<(uint16_t)((cpu->GetGPR_C(rsc1))>>16) <<" * " <<(uint16_t)((cpu->GetGPR_C(rsc2))>>16) ;
        #endif
}
#line 32548 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMulhhu::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 32563 "cpu/st231_isa.cpp"
}

void
OpMulhhu::initialize_latencies()
{

#line 135 "sim_latencies.isa"
{LAT(3);}
#line 32572 "cpu/st231_isa.cpp"
}

void
OpMulhhu::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 32581 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMulhhu(st231::CodeType code, uint32_t addr)
{
	return new OpMulhhu(code, addr);
}

// op mulhh(stop[1]:rsv[1]:0b00[2]:0b0011101[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32593 "cpu/st231_isa.cpp"
OpMulhh::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32597 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32601 "cpu/st231_isa.cpp"
)
{

#line 615 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(nldest, (op1>>16) * (op2>>16));
}
#line 32611 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32616 "cpu/st231_isa.cpp"
OpMulhh::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32620 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32624 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32629 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32633 "cpu/st231_isa.cpp"
)
{

#line 621 "integer.isa"
{
	os << "mulhh $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<(int32_t)cpu->GetGPR_N(nldest) <<" = " <<((cpu->GetGPR_C(rsc1))>>16) <<" * " <<((cpu->GetGPR_C(rsc2))>>16) ;
        #endif
}
#line 32644 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMulhh::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 32659 "cpu/st231_isa.cpp"
}

void
OpMulhh::initialize_latencies()
{

#line 134 "sim_latencies.isa"
{LAT(3);}
#line 32668 "cpu/st231_isa.cpp"
}

void
OpMulhh::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 32677 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMulhh(st231::CodeType code, uint32_t addr)
{
	return new OpMulhh(code, addr);
}

// op mullhu(stop[1]:rsv[1]:0b00[2]:0b0011100[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32689 "cpu/st231_isa.cpp"
OpMullhu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32693 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32697 "cpu/st231_isa.cpp"
)
{

#line 601 "integer.isa"
{
        uint16_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = ( (int32_t)(cpu->GetGPR_C(rsc2)) )>>16;
        cpu->SetGPR_N(nldest, op1 * op2);
}
#line 32707 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32712 "cpu/st231_isa.cpp"
OpMullhu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32716 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32720 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32725 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32729 "cpu/st231_isa.cpp"
)
{

#line 607 "integer.isa"
{
	os << "mullhu $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<(uint32_t)cpu->GetGPR_N(nldest) <<" = " <<(uint16_t)(cpu->GetGPR_C(rsc1)) <<" * " <<(uint16_t)((cpu->GetGPR_C(rsc2))>>16) ;
        #endif
}
#line 32740 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMullhu::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 32755 "cpu/st231_isa.cpp"
}

void
OpMullhu::initialize_latencies()
{

#line 133 "sim_latencies.isa"
{LAT(3);}
#line 32764 "cpu/st231_isa.cpp"
}

void
OpMullhu::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 32773 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMullhu(st231::CodeType code, uint32_t addr)
{
	return new OpMullhu(code, addr);
}

// op mullh(stop[1]:rsv[1]:0b00[2]:0b0011011[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]
//Half-word by upper-half-word signed multiply.

#line 53 "isa/st231.isa"
void
#line 32786 "cpu/st231_isa.cpp"
OpMullh::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32790 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32794 "cpu/st231_isa.cpp"
)
{

#line 587 "integer.isa"
{
        int16_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(nldest, op1 * (op2>>16));
}
#line 32804 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32809 "cpu/st231_isa.cpp"
OpMullh::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32813 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32817 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32822 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32826 "cpu/st231_isa.cpp"
)
{

#line 593 "integer.isa"
{
	os << "mullh $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<(int32_t)cpu->GetGPR_N(nldest) <<" = " <<(int16_t)(cpu->GetGPR_C(rsc1)) <<" * " <<(int16_t)((cpu->GetGPR_C(rsc2))>>16) ;
        #endif
}
#line 32837 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMullh::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 32852 "cpu/st231_isa.cpp"
}

void
OpMullh::initialize_latencies()
{

#line 132 "sim_latencies.isa"
{LAT(3);}
#line 32861 "cpu/st231_isa.cpp"
}

void
OpMullh::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 32870 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMullh(st231::CodeType code, uint32_t addr)
{
	return new OpMullh(code, addr);
}

// op mulllu(stop[1]:rsv[1]:0b00[2]:0b0011010[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32882 "cpu/st231_isa.cpp"
OpMulllu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32886 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32890 "cpu/st231_isa.cpp"
)
{

#line 570 "integer.isa"
{
        uint16_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = cpu->GetGPR_C(rsc2);
        uint32_t result = (uint32_t)op1 * (uint32_t)op2;
        cpu->SetGPR_N(nldest, result);
}
#line 32901 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 32906 "cpu/st231_isa.cpp"
OpMulllu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 32910 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 32914 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 32919 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 32923 "cpu/st231_isa.cpp"
)
{

#line 577 "integer.isa"
{
	os << "mulllu $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<(uint32_t)cpu->GetGPR_N(nldest) <<" = " <<(uint16_t)cpu->GetGPR_C(rsc1) <<" * " <<(uint16_t)cpu->GetGPR_C(rsc2) ;
        #endif
}
#line 32934 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMulllu::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 32949 "cpu/st231_isa.cpp"
}

void
OpMulllu::initialize_latencies()
{

#line 131 "sim_latencies.isa"
{LAT(3);}
#line 32958 "cpu/st231_isa.cpp"
}

void
OpMulllu::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 32967 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMulllu(st231::CodeType code, uint32_t addr)
{
	return new OpMulllu(code, addr);
}

// op mulll(stop[1]:rsv[1]:0b00[2]:0b0011001[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 32979 "cpu/st231_isa.cpp"
OpMulll::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 32983 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 32987 "cpu/st231_isa.cpp"
)
{

#line 555 "integer.isa"
{
        int16_t op1 = cpu->GetGPR_C(rsc1);
        int16_t op2 = cpu->GetGPR_C(rsc2);
        int32_t result = (int32_t)op1 * (int32_t)op2;
        cpu->SetGPR_N(nldest, result);
}
#line 32998 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33003 "cpu/st231_isa.cpp"
OpMulll::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33007 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33011 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33016 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33020 "cpu/st231_isa.cpp"
)
{

#line 562 "integer.isa"
{
	os << "mulll $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(nldest) <<" = " <<(int16_t)cpu->GetGPR_C(rsc1) <<" * " <<(int16_t)cpu->GetGPR_C(rsc2) ;
        #endif
}
#line 33031 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMulll::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 33046 "cpu/st231_isa.cpp"
}

void
OpMulll::initialize_latencies()
{

#line 130 "sim_latencies.isa"
{LAT(3);}
#line 33055 "cpu/st231_isa.cpp"
}

void
OpMulll::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 33064 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMulll(st231::CodeType code, uint32_t addr)
{
	return new OpMulll(code, addr);
}

// op mulhu(stop[1]:rsv[1]:0b00[2]:0b0011000[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]
// Half-word by upper-half-word unsigned multiply

#line 53 "isa/st231.isa"
void
#line 33077 "cpu/st231_isa.cpp"
OpMulhu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33081 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33085 "cpu/st231_isa.cpp"
)
{

#line 541 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = ( (int32_t)(cpu->GetGPR_C(rsc2)) )>>16;
        cpu->SetGPR_N(nldest, op1 * op2);
}
#line 33095 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33100 "cpu/st231_isa.cpp"
OpMulhu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33104 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33108 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33113 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33117 "cpu/st231_isa.cpp"
)
{

#line 547 "integer.isa"
{
	os << "mulhu $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<(uint32_t)cpu->GetGPR_N(nldest) <<" = " <<(uint32_t)cpu->GetGPR_C(rsc1) <<" * " <<((cpu->GetGPR_C(rsc2))>>16) ;
        #endif
}
#line 33128 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMulhu::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 33143 "cpu/st231_isa.cpp"
}

void
OpMulhu::initialize_latencies()
{

#line 129 "sim_latencies.isa"
{LAT(3);}
#line 33152 "cpu/st231_isa.cpp"
}

void
OpMulhu::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 33161 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMulhu(st231::CodeType code, uint32_t addr)
{
	return new OpMulhu(code, addr);
}

// op mulh(stop[1]:rsv[1]:0b00[2]:0b0010111[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]
// Half-word by upper-half-word signed multiply

#line 53 "isa/st231.isa"
void
#line 33174 "cpu/st231_isa.cpp"
OpMulh::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33178 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33182 "cpu/st231_isa.cpp"
)
{

#line 526 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(nldest, op1 * (op2>>16));
}
#line 33192 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33197 "cpu/st231_isa.cpp"
OpMulh::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33201 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33205 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33210 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33214 "cpu/st231_isa.cpp"
)
{

#line 532 "integer.isa"
{
	os << "mulh $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(nldest) <<" = " <<cpu->GetGPR_C(rsc1) <<" * " <<((cpu->GetGPR_C(rsc2))>>16) ;
        #endif
}
#line 33225 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMulh::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 33240 "cpu/st231_isa.cpp"
}

void
OpMulh::initialize_latencies()
{

#line 128 "sim_latencies.isa"
{LAT(3);}
#line 33249 "cpu/st231_isa.cpp"
}

void
OpMulh::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 33258 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMulh(st231::CodeType code, uint32_t addr)
{
	return new OpMulh(code, addr);
}

// op mullu(stop[1]:rsv[1]:0b00[2]:0b0010110[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]
//Word by half-word unsigned multiply

#line 53 "isa/st231.isa"
void
#line 33271 "cpu/st231_isa.cpp"
OpMullu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33275 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33279 "cpu/st231_isa.cpp"
)
{

#line 511 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(nldest, op1 * op2);
}
#line 33289 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33294 "cpu/st231_isa.cpp"
OpMullu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33298 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33302 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33307 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33311 "cpu/st231_isa.cpp"
)
{

#line 517 "integer.isa"
{
	os << "mullu $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<(uint32_t)cpu->GetGPR_N(nldest) <<" = " <<cpu->GetGPR_C(rsc1) <<" * " <<(uint16_t)cpu->GetGPR_C(rsc2) ;
        #endif
}
#line 33322 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMullu::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 33337 "cpu/st231_isa.cpp"
}

void
OpMullu::initialize_latencies()
{

#line 127 "sim_latencies.isa"
{LAT(3);}
#line 33346 "cpu/st231_isa.cpp"
}

void
OpMullu::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 33355 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMullu(st231::CodeType code, uint32_t addr)
{
	return new OpMullu(code, addr);
}

// op mull(stop[1]:rsv[1]:0b00[2]:0b0010101[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]
//Word by half-word signed multiply

#line 53 "isa/st231.isa"
void
#line 33368 "cpu/st231_isa.cpp"
OpMull::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33372 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33376 "cpu/st231_isa.cpp"
)
{

#line 496 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int16_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(nldest, op1*op2);
}
#line 33386 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33391 "cpu/st231_isa.cpp"
OpMull::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33395 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33399 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33404 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33408 "cpu/st231_isa.cpp"
)
{

#line 502 "integer.isa"
{
	os << "mull $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(nldest) <<" = " <<cpu->GetGPR_C(rsc1) <<" * " <<((int16_t)cpu->GetGPR_C(rsc2)) ;
        #endif
}
#line 33419 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMull::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 33434 "cpu/st231_isa.cpp"
}

void
OpMull::initialize_latencies()
{

#line 126 "sim_latencies.isa"
{LAT(3);}
#line 33443 "cpu/st231_isa.cpp"
}

void
OpMull::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 33452 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMull(st231::CodeType code, uint32_t addr)
{
	return new OpMull(code, addr);
}

// multiply instruction: the destination register is "nldest"
// op mulhhs(stop[1]:rsv[1]:0b00[2]:0b0010100[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]
// Word by upper-half-word signed multiply, returns top 32 bits of 48 bit result

#line 53 "isa/st231.isa"
void
#line 33466 "cpu/st231_isa.cpp"
OpMulhhs::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33470 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33474 "cpu/st231_isa.cpp"
)
{

#line 480 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        int64_t result = (int64_t)op1 * ((int64_t)(op2>>16));
        cpu->SetGPR_N(nldest, (result >> 16));
}
#line 33485 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33490 "cpu/st231_isa.cpp"
OpMulhhs::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33494 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33498 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33503 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33507 "cpu/st231_isa.cpp"
)
{

#line 487 "integer.isa"
{
	os << "mulhhs $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(nldest) <<" = " <<cpu->GetGPR_C(rsc1) <<" * " <<((cpu->GetGPR_C(rsc2))>>16) <<" >>16" ;
        #endif
}
#line 33518 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMulhhs::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 33533 "cpu/st231_isa.cpp"
}

void
OpMulhhs::initialize_latencies()
{

#line 125 "sim_latencies.isa"
{LAT(3);}
#line 33542 "cpu/st231_isa.cpp"
}

void
OpMulhhs::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 33551 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMulhhs(st231::CodeType code, uint32_t addr)
{
	return new OpMulhhs(code, addr);
}

// op minu(stop[1]:rsv[1]:0b00[2]:0b0010011[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 33563 "cpu/st231_isa.cpp"
OpMinu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33567 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33571 "cpu/st231_isa.cpp"
)
{

#line 443 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        uint32_t result = op1 < op2 ? op1 : op2;
        cpu->SetGPR_N(dest, result);
}
#line 33582 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33587 "cpu/st231_isa.cpp"
OpMinu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33591 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33595 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33600 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33604 "cpu/st231_isa.cpp"
)
{

#line 450 "integer.isa"
{
	os << "minu $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(dest) <<" = umin(" <<cpu->GetGPR_C(rsc1) <<" , " <<cpu->GetGPR_C(rsc2) <<" )" ;
        #endif
}
#line 33615 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpMinu::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 33634 "cpu/st231_isa.cpp"
}

void
OpMinu::initialize_latencies()
{

#line 38 "sim_latencies.isa"
{LAT(1);}
#line 33643 "cpu/st231_isa.cpp"
}

void
OpMinu::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 33652 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMinu(st231::CodeType code, uint32_t addr)
{
	return new OpMinu(code, addr);
}

// op min(stop[1]:rsv[1]:0b00[2]:0b0010010[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 33664 "cpu/st231_isa.cpp"
OpMin::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33668 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33672 "cpu/st231_isa.cpp"
)
{

#line 428 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        int32_t result = op1 < op2 ? op1 : op2;
        cpu->SetGPR_N(dest, result);
}
#line 33683 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33688 "cpu/st231_isa.cpp"
OpMin::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33692 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33696 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33701 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33705 "cpu/st231_isa.cpp"
)
{

#line 435 "integer.isa"
{
	os << "min $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(dest) <<" = min(" <<cpu->GetGPR_C(rsc1) <<" , " <<cpu->GetGPR_C(rsc2) <<" )" ;
        #endif
}
#line 33716 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpMin::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 33735 "cpu/st231_isa.cpp"
}

void
OpMin::initialize_latencies()
{

#line 37 "sim_latencies.isa"
{LAT(1);}
#line 33744 "cpu/st231_isa.cpp"
}

void
OpMin::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 33753 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMin(st231::CodeType code, uint32_t addr)
{
	return new OpMin(code, addr);
}

// op maxu(stop[1]:rsv[1]:0b00[2]:0b0010001[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 33765 "cpu/st231_isa.cpp"
OpMaxu::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33769 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33773 "cpu/st231_isa.cpp"
)
{

#line 413 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint32_t op2 = cpu->GetGPR_C(rsc2);
        uint32_t result = op1 > op2 ? op1 : op2;
        cpu->SetGPR_N(dest, result);
}
#line 33784 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33789 "cpu/st231_isa.cpp"
OpMaxu::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33793 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33797 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33802 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33806 "cpu/st231_isa.cpp"
)
{

#line 420 "integer.isa"
{
	os << "maxu $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(dest) <<" = umax(" <<cpu->GetGPR_C(rsc1) <<" , " <<cpu->GetGPR_C(rsc2) <<" )" ;
        #endif
}
#line 33817 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpMaxu::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 33836 "cpu/st231_isa.cpp"
}

void
OpMaxu::initialize_latencies()
{

#line 36 "sim_latencies.isa"
{LAT(1);}
#line 33845 "cpu/st231_isa.cpp"
}

void
OpMaxu::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 33854 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMaxu(st231::CodeType code, uint32_t addr)
{
	return new OpMaxu(code, addr);
}

// op max(stop[1]:rsv[1]:0b00[2]:0b0010000[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 33866 "cpu/st231_isa.cpp"
OpMax::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33870 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33874 "cpu/st231_isa.cpp"
)
{

#line 398 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        int32_t result = op1 > op2 ? op1 : op2;
        cpu->SetGPR_N(dest, result);
}
#line 33885 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33890 "cpu/st231_isa.cpp"
OpMax::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 33894 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 33898 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 33903 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 33907 "cpu/st231_isa.cpp"
)
{

#line 405 "integer.isa"
{
	os << "max $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(dest) <<" = max(" <<cpu->GetGPR_C(rsc1) <<" , " <<cpu->GetGPR_C(rsc2) <<" )";
        #endif
}
#line 33918 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpMax::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 33937 "cpu/st231_isa.cpp"
}

void
OpMax::initialize_latencies()
{

#line 35 "sim_latencies.isa"
{LAT(1);}
#line 33946 "cpu/st231_isa.cpp"
}

void
OpMax::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 33955 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMax(st231::CodeType code, uint32_t addr)
{
	return new OpMax(code, addr);
}

// this opcode isn't used
// op illegal(stop[1]:rsv[1]:0b00[2]:0b0001110[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])
// multiply instruction: the destination register is "nldest"
// multiply instruction: the destination register is "nldest"
// op mullhus(stop[1]:rsv[1]:0b00[2]:0b0001111[7]:bdest[3]:nldest[6]:rsc2[6]:rsc1[6]
//Word by lower-half-word signed multiply, returns top 16 bits of 48 bit result,
//sign extended.

#line 53 "isa/st231.isa"
void
#line 33973 "cpu/st231_isa.cpp"
OpMullhus::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 33977 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 33981 "cpu/st231_isa.cpp"
)
{

#line 463 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        uint16_t op2 = cpu->GetGPR_C(rsc2);
        int64_t result = (int64_t)op1 *(int64_t)op2;

        cpu->SetGPR_N(nldest, (result>>32));
}
#line 33993 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 33998 "cpu/st231_isa.cpp"
OpMullhus::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34002 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34006 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34011 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34015 "cpu/st231_isa.cpp"
)
{

#line 471 "integer.isa"
{
	os << "mullhus $r" << nldest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<cpu->GetGPR_N(nldest) <<" = " <<cpu->GetGPR_C(rsc1) <<" * " <<((cpu->GetGPR_C(rsc2))&0xffff) <<" >>32" ;
        #endif
}
#line 34026 "cpu/st231_isa.cpp"
}
/* ---  integer register multiplication instructions ------------------- */

void
OpMullhus::initialize_operands()
{

#line 123 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, nldest);
}
#line 34041 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER multipication INSTRUCTIONS ================================== */
/*****************************************************************************/

void
OpMullhus::initialize_latencies()
{

#line 124 "sim_latencies.isa"
{LAT(3);}
#line 34053 "cpu/st231_isa.cpp"
}

void
OpMullhus::initialize_function()
{

#line 44 "sim_functions.isa"
{ function = FnIntExtended; }
#line 34062 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpMullhus(st231::CodeType code, uint32_t addr)
{
	return new OpMullhus(code, addr);
}

// op xor(stop[1]:rsv[1]:0b00[2]:0b0001101[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 34074 "cpu/st231_isa.cpp"
OpXor::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34078 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34082 "cpu/st231_isa.cpp"
)
{

#line 383 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, op1 ^ op2);
}
#line 34092 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 34097 "cpu/st231_isa.cpp"
OpXor::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34101 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34105 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34110 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34114 "cpu/st231_isa.cpp"
)
{

#line 389 "integer.isa"
{
	os << "xor $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<hex <<cpu->GetGPR_N(dest) <<" = " <<hex <<cpu->GetGPR_C(rsc1) <<" ^ " <<hex <<cpu->GetGPR_C(rsc2) <<dec ;
        #endif
}
#line 34125 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpXor::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 34144 "cpu/st231_isa.cpp"
}

void
OpXor::initialize_latencies()
{

#line 34 "sim_latencies.isa"
{LAT(1);}
#line 34153 "cpu/st231_isa.cpp"
}

void
OpXor::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 34162 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpXor(st231::CodeType code, uint32_t addr)
{
	return new OpXor(code, addr);
}

// op orc(stop[1]:rsv[1]:0b00[2]:0b0001100[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 34174 "cpu/st231_isa.cpp"
OpOrc::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34178 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34182 "cpu/st231_isa.cpp"
)
{

#line 369 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (~op1) | op2);
}
#line 34192 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 34197 "cpu/st231_isa.cpp"
OpOrc::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34201 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34205 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34210 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34214 "cpu/st231_isa.cpp"
)
{

#line 375 "integer.isa"
{
	os << "orc $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<hex <<cpu->GetGPR_N(dest) <<" = ~" <<hex <<cpu->GetGPR_C(rsc1) <<" | " <<hex <<cpu->GetGPR_C(rsc2) <<dec ;
        #endif
}
#line 34225 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpOrc::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 34244 "cpu/st231_isa.cpp"
}

void
OpOrc::initialize_latencies()
{

#line 33 "sim_latencies.isa"
{LAT(1);}
#line 34253 "cpu/st231_isa.cpp"
}

void
OpOrc::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 34262 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpOrc(st231::CodeType code, uint32_t addr)
{
	return new OpOrc(code, addr);
}

// op or(stop[1]:rsv[1]:0b00[2]:0b0001011[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 34274 "cpu/st231_isa.cpp"
OpOr::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34278 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34282 "cpu/st231_isa.cpp"
)
{

#line 355 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, op1 | op2);
}
#line 34292 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 34297 "cpu/st231_isa.cpp"
OpOr::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34301 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34305 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34310 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34314 "cpu/st231_isa.cpp"
)
{

#line 361 "integer.isa"
{
	os << "or $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<hex <<cpu->GetGPR_N(dest) <<" = " <<hex <<cpu->GetGPR_C(rsc1) <<" | " <<hex <<cpu->GetGPR_C(rsc2) <<dec ;
        #endif
}
#line 34325 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpOr::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 34344 "cpu/st231_isa.cpp"
}

void
OpOr::initialize_latencies()
{

#line 32 "sim_latencies.isa"
{LAT(1);}
#line 34353 "cpu/st231_isa.cpp"
}

void
OpOr::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 34362 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpOr(st231::CodeType code, uint32_t addr)
{
	return new OpOr(code, addr);
}

// op andc(stop[1]:rsv[1]:0b00[2]:0b0001010[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 34374 "cpu/st231_isa.cpp"
OpAndc::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34378 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34382 "cpu/st231_isa.cpp"
)
{

#line 341 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (~op1) & op2);
}
#line 34392 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 34397 "cpu/st231_isa.cpp"
OpAndc::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34401 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34405 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34410 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34414 "cpu/st231_isa.cpp"
)
{

#line 347 "integer.isa"
{
	os << "andc $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<hex <<cpu->GetGPR_N(dest) <<" = ~" <<hex <<cpu->GetGPR_C(rsc1) <<" & " <<hex <<cpu->GetGPR_C(rsc2) <<dec ;
        #endif
}
#line 34425 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpAndc::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 34444 "cpu/st231_isa.cpp"
}

void
OpAndc::initialize_latencies()
{

#line 31 "sim_latencies.isa"
{LAT(1);}
#line 34453 "cpu/st231_isa.cpp"
}

void
OpAndc::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 34462 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpAndc(st231::CodeType code, uint32_t addr)
{
	return new OpAndc(code, addr);
}

// op and(stop[1]:rsv[1]:0b00[2]:0b0001001[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 34474 "cpu/st231_isa.cpp"
OpAnd::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34478 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34482 "cpu/st231_isa.cpp"
)
{

#line 327 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, op1 & op2);
}
#line 34492 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 34497 "cpu/st231_isa.cpp"
OpAnd::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34501 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34505 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34510 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34514 "cpu/st231_isa.cpp"
)
{

#line 333 "integer.isa"
{
	os << "and $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<hex <<cpu->GetGPR_N(dest) <<" = " <<hex <<cpu->GetGPR_C(rsc1) <<" & " <<hex <<cpu->GetGPR_C(rsc2) <<dec ;
        #endif
}
#line 34525 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpAnd::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 34544 "cpu/st231_isa.cpp"
}

void
OpAnd::initialize_latencies()
{

#line 30 "sim_latencies.isa"
{LAT(1);}
#line 34553 "cpu/st231_isa.cpp"
}

void
OpAnd::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 34562 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpAnd(st231::CodeType code, uint32_t addr)
{
	return new OpAnd(code, addr);
}

// op sh4add(stop[1]:rsv[1]:0b00[2]:0b0001000[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 34574 "cpu/st231_isa.cpp"
OpSh4add::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34578 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34582 "cpu/st231_isa.cpp"
)
{

#line 312 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1<<4) + op2 );
}
#line 34592 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 34597 "cpu/st231_isa.cpp"
OpSh4add::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34601 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34605 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34610 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34614 "cpu/st231_isa.cpp"
)
{

#line 318 "integer.isa"
{
	os << "sh4add $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<hex <<cpu->GetGPR_N(dest) <<" = " <<hex <<cpu->GetGPR_C(rsc1) <<" << 4 + " <<hex <<cpu->GetGPR_C(rsc2) <<dec ;
        #endif
}
#line 34625 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpSh4add::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 34644 "cpu/st231_isa.cpp"
}

void
OpSh4add::initialize_latencies()
{

#line 29 "sim_latencies.isa"
{LAT(1);}
#line 34653 "cpu/st231_isa.cpp"
}

void
OpSh4add::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 34662 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSh4add(st231::CodeType code, uint32_t addr)
{
	return new OpSh4add(code, addr);
}

// op sh3add(stop[1]:rsv[1]:0b00[2]:0b0000111[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 34674 "cpu/st231_isa.cpp"
OpSh3add::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34678 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34682 "cpu/st231_isa.cpp"
)
{

#line 298 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1<<3) + op2 );
}
#line 34692 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 34697 "cpu/st231_isa.cpp"
OpSh3add::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34701 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34705 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34710 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34714 "cpu/st231_isa.cpp"
)
{

#line 304 "integer.isa"
{
	os << "sh3add $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<hex <<cpu->GetGPR_N(dest) <<" = " <<hex <<cpu->GetGPR_C(rsc1) <<" << 3 + " <<hex <<cpu->GetGPR_C(rsc2) <<dec ;
        #endif
}
#line 34725 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpSh3add::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 34744 "cpu/st231_isa.cpp"
}

void
OpSh3add::initialize_latencies()
{

#line 28 "sim_latencies.isa"
{LAT(1);}
#line 34753 "cpu/st231_isa.cpp"
}

void
OpSh3add::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 34762 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSh3add(st231::CodeType code, uint32_t addr)
{
	return new OpSh3add(code, addr);
}

// op sh2add(stop[1]:rsv[1]:0b00[2]:0b0000110[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6]

#line 53 "isa/st231.isa"
void
#line 34774 "cpu/st231_isa.cpp"
OpSh2add::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34778 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34782 "cpu/st231_isa.cpp"
)
{

#line 283 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1<<2) + op2 );
}
#line 34792 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 34797 "cpu/st231_isa.cpp"
OpSh2add::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34801 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34805 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34810 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34814 "cpu/st231_isa.cpp"
)
{

#line 289 "integer.isa"
{
	os << "sh2add $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<hex <<cpu->GetGPR_N(dest) <<" = " <<hex <<cpu->GetGPR_C(rsc1) <<" << 2 + " <<hex <<cpu->GetGPR_C(rsc2) <<dec ;
        #endif
}
#line 34825 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpSh2add::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 34844 "cpu/st231_isa.cpp"
}

void
OpSh2add::initialize_latencies()
{

#line 27 "sim_latencies.isa"
{LAT(1);}
#line 34853 "cpu/st231_isa.cpp"
}

void
OpSh2add::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 34862 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSh2add(st231::CodeType code, uint32_t addr)
{
	return new OpSh2add(code, addr);
}

// op sh1add(stop[1]:rsv[1]:0b00[2]:0b0000101[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 34874 "cpu/st231_isa.cpp"
OpSh1add::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34878 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34882 "cpu/st231_isa.cpp"
)
{

#line 269 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, (op1<<1) + op2 );
}
#line 34892 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 34897 "cpu/st231_isa.cpp"
OpSh1add::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 34901 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 34905 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 34910 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 34914 "cpu/st231_isa.cpp"
)
{

#line 275 "integer.isa"
{
	os << "sh1add $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << " // " <<hex <<cpu->GetGPR_N(dest) <<" = " <<hex <<cpu->GetGPR_C(rsc1) <<" << 1 + " <<hex <<cpu->GetGPR_C(rsc2) <<dec ;
        #endif
}
#line 34925 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpSh1add::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 34944 "cpu/st231_isa.cpp"
}

void
OpSh1add::initialize_latencies()
{

#line 26 "sim_latencies.isa"
{LAT(1);}
#line 34953 "cpu/st231_isa.cpp"
}

void
OpSh1add::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 34962 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSh1add(st231::CodeType code, uint32_t addr)
{
	return new OpSh1add(code, addr);
}

// op shl(stop[1]:rsv[1]:0b00[2]:0b0000010[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])
// op shr(stop[1]:rsv[1]:0b00[2]:0b0000011[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])
// op shru(stop[1]:rsv[1]:0b00[2]:0b0000100[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])
// Logical shift right

#line 53 "isa/st231.isa"
void
#line 34977 "cpu/st231_isa.cpp"
OpShru::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 34981 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 34985 "cpu/st231_isa.cpp"
)
{

#line 251 "integer.isa"
{
        uint32_t op1 = cpu->GetGPR_C(rsc1);
        uint8_t op2 = cpu->GetGPR_C(rsc2);
        if (op2>31)
        	cpu->SetGPR_N(dest, 0);
        else
		cpu->SetGPR_N(dest, op1 << op2);

}
#line 34999 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 35004 "cpu/st231_isa.cpp"
OpShru::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 35008 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 35012 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 35017 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 35021 "cpu/st231_isa.cpp"
)
{

#line 261 "integer.isa"
{
	os << "shru $r" << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << "  //  " << hex << cpu->GetGPR_N(dest) << " = " << hex << cpu->GetGPR_C(rsc1) << " >> " << dec << cpu->GetGPR_C(rsc2) ;
        #endif
}
#line 35032 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpShru::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 35051 "cpu/st231_isa.cpp"
}

void
OpShru::initialize_latencies()
{

#line 25 "sim_latencies.isa"
{LAT(1);}
#line 35060 "cpu/st231_isa.cpp"
}

void
OpShru::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 35069 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpShru(st231::CodeType code, uint32_t addr)
{
	return new OpShru(code, addr);
}

// op shr(stop[1]:rsv[1]:0b00[2]:0b0000011[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])
// Arithmetic shift right

#line 53 "isa/st231.isa"
void
#line 35082 "cpu/st231_isa.cpp"
OpRshr::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 35086 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 35090 "cpu/st231_isa.cpp"
)
{

#line 229 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        uint8_t op2 = cpu->GetGPR_C(rsc2);
        if (op2>31)
		if(op1&0x80000000)
        		cpu->SetGPR_N(dest, 0xffffffff);
		else
        		cpu->SetGPR_N(dest, 0);
        else
		cpu->SetGPR_N(dest, op1 << op2);
}
#line 35106 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 35111 "cpu/st231_isa.cpp"
OpRshr::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 35115 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 35119 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 35124 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 35128 "cpu/st231_isa.cpp"
)
{

#line 241 "integer.isa"
{
	os << "shr $r" << dec << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << "  //  " << hex << cpu->GetGPR_N(dest) << " = " << hex << cpu->GetGPR_C(rsc1) << " >> " << dec << cpu->GetGPR_C(rsc2) ;
        #endif
}
#line 35139 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpRshr::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 35158 "cpu/st231_isa.cpp"
}

void
OpRshr::initialize_latencies()
{

#line 24 "sim_latencies.isa"
{LAT(1);}
#line 35167 "cpu/st231_isa.cpp"
}

void
OpRshr::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 35176 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpRshr(st231::CodeType code, uint32_t addr)
{
	return new OpRshr(code, addr);
}

// op shl(stop[1]:rsv[1]:0b00[2]:0b0000010[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])
// register shift left 

#line 53 "isa/st231.isa"
void
#line 35189 "cpu/st231_isa.cpp"
OpRshl::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 35193 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 35197 "cpu/st231_isa.cpp"
)
{

#line 210 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        uint8_t op2 = (uint8_t)cpu->GetGPR_C(rsc2);
        if (op2>31)
        	cpu->SetGPR_N(dest, 0);
        else
		cpu->SetGPR_N(dest, op1 << op2);
}
#line 35210 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 35215 "cpu/st231_isa.cpp"
OpRshl::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 35219 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 35223 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 35228 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 35232 "cpu/st231_isa.cpp"
)
{

#line 219 "integer.isa"
{
	os << "shl $r" << dec << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << "  //  " << hex << cpu->GetGPR_N(dest) << " = " << hex << cpu->GetGPR_C(rsc1) << " << " << dec << cpu->GetGPR_C(rsc2) ;
        #endif
}
#line 35243 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpRshl::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 35262 "cpu/st231_isa.cpp"
}

void
OpRshl::initialize_latencies()
{

#line 23 "sim_latencies.isa"
{LAT(1);}
#line 35271 "cpu/st231_isa.cpp"
}

void
OpRshl::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 35280 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpRshl(st231::CodeType code, uint32_t addr)
{
	return new OpRshl(code, addr);
}

// op sub(stop[1]:rsv[1]:0b00[2]:0b0000001[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 35292 "cpu/st231_isa.cpp"
OpSub::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 35296 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 35300 "cpu/st231_isa.cpp"
)
{

#line 195 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, op2-op1);
}
#line 35310 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 35315 "cpu/st231_isa.cpp"
OpSub::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 35319 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 35323 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 35328 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 35332 "cpu/st231_isa.cpp"
)
{

#line 201 "integer.isa"
{
	os << "sub $r" << dec << dest << " = $r" << rsc2 << ", $r" << rsc1 ;
        #if DEBUG
	os << "  //  " << cpu->GetGPR_N(dest) << " = " << cpu->GetGPR_C(rsc1) << " - " << cpu-> GetGPR_C(rsc2) ;
        #endif
}
#line 35343 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpSub::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 35362 "cpu/st231_isa.cpp"
}

void
OpSub::initialize_latencies()
{

#line 22 "sim_latencies.isa"
{LAT(1);}
#line 35371 "cpu/st231_isa.cpp"
}

void
OpSub::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 35380 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpSub(st231::CodeType code, uint32_t addr)
{
	return new OpSub(code, addr);
}

//************************************************************************
// the register format operations
//************************************************************************
// op add(stop[1]:rsv[1]:0b00[2]:0b0000000[7]:bdest[3]:dest[6]:rsc2[6]:rsc1[6])

#line 53 "isa/st231.isa"
void
#line 35395 "cpu/st231_isa.cpp"
OpAdd::execute(
#line 53 "isa/st231.isa"
	CPU *
#line 35399 "cpu/st231_isa.cpp"

#line 53 "isa/st231.isa"
	cpu
#line 35403 "cpu/st231_isa.cpp"
)
{

#line 179 "integer.isa"
{
        int32_t op1 = cpu->GetGPR_C(rsc1);
        int32_t op2 = cpu->GetGPR_C(rsc2);
        cpu->SetGPR_N(dest, op1+op2);
}
#line 35413 "cpu/st231_isa.cpp"
}

#line 58 "isa/st231.isa"
void
#line 35418 "cpu/st231_isa.cpp"
OpAdd::disasm(
#line 58 "isa/st231.isa"
	CPU *
#line 35422 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	cpu
#line 35426 "cpu/st231_isa.cpp"
,

#line 58 "isa/st231.isa"
	ostream&
#line 35431 "cpu/st231_isa.cpp"

#line 58 "isa/st231.isa"
	os
#line 35435 "cpu/st231_isa.cpp"
)
{

#line 185 "integer.isa"
{
	os << "add $r" << dec << dest << " = $r" << rsc1 << ", $r" << rsc2 ;
        #if DEBUG
	os << "  //  " <<cpu->GetGPR_N(dest) << " = " << cpu->GetGPR_C(rsc1) << " + " << cpu-> GetGPR_C(rsc2) ;
        #endif

}
#line 35447 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER INSTRUCTIONS ================================================ */
/*****************************************************************************/
/* === Integer Register INSTRUCTIONS =================================== */
/* ---  integer register simple arithmetic instructions ---------------- */

void
OpAdd::initialize_operands()
{

#line 97 "sim_dependencies.isa"
{
  //LOCAL_DEBUG
  ST231_INPUT(GPR_T, rsc1);
  ST231_INPUT(GPR_T, rsc2);
  ST231_OUTPUT(GPR_T, dest);
}
#line 35466 "cpu/st231_isa.cpp"
}
/*****************************************************************************/
/* === INTEGER simple arithmetic INSTRUCTIONS ============================== */
/*****************************************************************************/

void
OpAdd::initialize_latencies()
{

#line 21 "sim_latencies.isa"
{LAT(1);}
#line 35478 "cpu/st231_isa.cpp"
}

void
OpAdd::initialize_function()
{

#line 32 "sim_functions.isa"
{ function = FnIntBasic; }
#line 35487 "cpu/st231_isa.cpp"
}

static st231::Operation *DecodeOpAdd(st231::CodeType code, uint32_t addr)
{
	return new OpAdd(code, addr);
}

OpPswclr::OpPswclr(st231::CodeType code, uint32_t addr) : Operation(code, addr, "pswclr")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35501 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35506 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35511 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 12) & 0x7ff);
	src2 = ((code >> 6) & 0x3f);
}

OpPswset::OpPswset(st231::CodeType code, uint32_t addr) : Operation(code, addr, "pswset")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35524 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35529 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35534 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 12) & 0x7ff);
	src2 = ((code >> 6) & 0x3f);
}

OpSync::OpSync(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sync")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35547 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35552 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35557 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
}

OpPrginspg::OpPrginspg(st231::CodeType code, uint32_t addr) : Operation(code, addr, "prginspg")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35569 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35574 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35579 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	rsc1 = ((code >> 0) & 0x3f);
}

OpPrgset::OpPrgset(st231::CodeType code, uint32_t addr) : Operation(code, addr, "prgset")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35593 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35598 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35603 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	rsc1 = ((code >> 0) & 0x3f);
}

OpPrgadd::OpPrgadd(st231::CodeType code, uint32_t addr) : Operation(code, addr, "prgadd")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35617 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35622 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35627 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	rsc1 = ((code >> 0) & 0x3f);
}

OpPft::OpPft(st231::CodeType code, uint32_t addr) : Operation(code, addr, "pft")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35641 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35646 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35651 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	rsc1 = ((code >> 0) & 0x3f);
}

OpStb::OpStb(st231::CodeType code, uint32_t addr) : Operation(code, addr, "stb")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35665 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35670 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35675 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSth::OpSth(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sth")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35690 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35695 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35700 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpStw::OpStw(st231::CodeType code, uint32_t addr) : Operation(code, addr, "stw")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35715 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35720 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35725 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdbu_d::OpLdbu_d(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldbu_d")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35740 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35745 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35750 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdbu::OpLdbu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldbu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35765 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35770 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35775 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdb_d::OpLdb_d(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldb_d")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35790 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35795 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35800 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdb::OpLdb(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldb")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35815 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35820 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35825 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdhu_d::OpLdhu_d(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldhu_d")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35840 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35845 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35850 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdhu::OpLdhu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldhu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35865 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35870 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35875 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdh_d::OpLdh_d(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldh_d")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35890 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35895 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35900 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdh::OpLdh(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldh")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35915 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35920 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35925 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdw_d::OpLdw_d(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldw_d")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35940 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35945 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35950 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpLdw::OpLdw(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ldw")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35965 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35970 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 35975 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	null = ((code >> 21) & 0x3);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBreak::OpBreak(st231::CodeType code, uint32_t addr) : Operation(code, addr, "break")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 35990 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 35995 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36000 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	sbrknum = ((code >> 0) & 0x1fffff);
}

OpSyscall::OpSyscall(st231::CodeType code, uint32_t addr) : Operation(code, addr, "syscall")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36012 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36017 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36022 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	sbrknum = ((code >> 0) & 0x1fffff);
}

OpSbrk::OpSbrk(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sbrk")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36034 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36039 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36044 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	sbrknum = ((code >> 0) & 0x1fffff);
}

OpPrgins::OpPrgins(st231::CodeType code, uint32_t addr) : Operation(code, addr, "prgins")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36056 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36061 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36066 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	sbrknum = ((code >> 0) & 0x1fffff);
}

OpIslctf::OpIslctf(st231::CodeType code, uint32_t addr) : Operation(code, addr, "islctf")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36078 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36083 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36088 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	scond = ((code >> 21) & 0x7);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIslct::OpIslct(st231::CodeType code, uint32_t addr) : Operation(code, addr, "islct")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36103 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36108 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36113 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	scond = ((code >> 21) & 0x7);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImmr::OpImmr(st231::CodeType code, uint32_t addr) : Operation(code, addr, "immr")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36128 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36133 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36138 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	imm = ((code >> 0) & 0x7fffff);
}

OpImml::OpImml(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imml")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36150 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36155 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36160 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	imm = ((code >> 0) & 0x7fffff);
}

OpDivs::OpDivs(st231::CodeType code, uint32_t addr) : Operation(code, addr, "divs")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36172 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36177 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36182 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	scond = ((code >> 21) & 0x7);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpAddcg::OpAddcg(st231::CodeType code, uint32_t addr) : Operation(code, addr, "addcg")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36198 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36203 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36208 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	scond = ((code >> 21) & 0x7);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSlctf::OpSlctf(st231::CodeType code, uint32_t addr) : Operation(code, addr, "slctf")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36224 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36229 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36234 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	scond = ((code >> 21) & 0x7);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSlct::OpSlct(st231::CodeType code, uint32_t addr) : Operation(code, addr, "slct")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36250 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36255 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36260 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	scond = ((code >> 21) & 0x7);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBrf::OpBrf(st231::CodeType code, uint32_t addr) : Operation(code, addr, "brf")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36276 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36281 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36286 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bcond = ((code >> 23) & 0x7);
	btarg = ((code >> 0) & 0x7fffff);
}

OpBr::OpBr(st231::CodeType code, uint32_t addr) : Operation(code, addr, "br")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36299 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36304 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36309 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bcond = ((code >> 23) & 0x7);
	btarg = ((code >> 0) & 0x7fffff);
}

OpRfi::OpRfi(st231::CodeType code, uint32_t addr) : Operation(code, addr, "rfi")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36322 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36327 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36332 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
}

OpGoto::OpGoto(st231::CodeType code, uint32_t addr) : Operation(code, addr, "goto")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36343 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36348 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36353 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
}

OpIgoto::OpIgoto(st231::CodeType code, uint32_t addr) : Operation(code, addr, "igoto")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36364 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36369 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36374 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	btarg = ((code >> 0) & 0x7fffff);
}

OpCall::OpCall(st231::CodeType code, uint32_t addr) : Operation(code, addr, "call")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36386 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36391 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36396 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
}

OpIcall::OpIcall(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icall")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36407 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36412 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36417 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	btarg = ((code >> 0) & 0x7fffff);
}

OpClz::OpClz(st231::CodeType code, uint32_t addr) : Operation(code, addr, "clz")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36429 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36434 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36439 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpZxth::OpZxth(st231::CodeType code, uint32_t addr) : Operation(code, addr, "zxth")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36452 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36457 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36462 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBswap::OpBswap(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bswap")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36475 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36480 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36485 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSxth::OpSxth(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sxth")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36498 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36503 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36508 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSxtb::OpSxtb(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sxtb")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36521 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36526 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36531 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImulfrac::OpImulfrac(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imulfrac")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36544 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36549 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36554 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImul64hu::OpImul64hu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imul64hu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36568 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36573 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36578 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbnorl::OpIbnorl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibnorl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36592 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36597 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36602 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIborl::OpIborl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "iborl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36617 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36622 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36627 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbnandl::OpIbnandl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibnandl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36642 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36647 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36652 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbandl::OpIbandl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibandl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36667 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36672 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36677 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmpltu::OpIbcmpltu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmpltu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36692 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36697 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36702 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmplt::OpIbcmplt(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmplt")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36717 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36722 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36727 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmpleu::OpIbcmpleu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmpleu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36742 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36747 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36752 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmple::OpIbcmple(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmple")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36767 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36772 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36777 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmpgtu::OpIbcmpgtu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmpgtu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36792 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36797 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36802 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmpgt::OpIbcmpgt(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmpgt")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36817 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36822 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36827 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmpgeu::OpIbcmpgeu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmpgeu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36842 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36847 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36852 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmpge::OpIbcmpge(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmpge")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36867 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36872 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36877 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmpne::OpIbcmpne(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmpne")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36892 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36897 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36902 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIbcmpeq::OpIbcmpeq(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ibcmpeq")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36917 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36922 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36927 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	null = ((code >> 9) & 0x7);
	ibdest = ((code >> 6) & 0x7);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImul64h::OpImul64h(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imul64h")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36942 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36947 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36952 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImul32::OpImul32(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imul32")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36966 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36971 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 36976 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpInorl::OpInorl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "inorl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 36990 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 36995 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37000 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIorl::OpIorl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "iorl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37014 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37019 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37024 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpInandl::OpInandl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "inandl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37038 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37043 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37048 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIandl::OpIandl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "iandl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37062 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37067 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37072 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmpltu::OpIcmpltu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmpltu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37086 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37091 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37096 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmplt::OpIcmplt(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmplt")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37110 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37115 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37120 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmpleu::OpIcmpleu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmpleu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37134 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37139 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37144 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmple::OpIcmple(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmple")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37158 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37163 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37168 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmpgtu::OpIcmpgtu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmpgtu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37182 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37187 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37192 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmpgt::OpIcmpgt(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmpgt")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37206 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37211 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37216 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmpgeu::OpIcmpgeu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmpgeu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37230 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37235 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37240 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmpge::OpIcmpge(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmpge")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37254 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37259 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37264 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmpne::OpIcmpne(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmpne")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37278 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37283 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37288 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIcmpeq::OpIcmpeq(st231::CodeType code, uint32_t addr) : Operation(code, addr, "icmpeq")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37302 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37307 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37312 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImulhs::OpImulhs(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imulhs")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37326 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37331 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37336 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImulhhu::OpImulhhu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imulhhu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37350 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37355 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37360 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImulhh::OpImulhh(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imulhh")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37374 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37379 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37384 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImullhu::OpImullhu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imullhu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37398 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37403 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37408 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImullh::OpImullh(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imullh")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37422 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37427 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37432 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImulllu::OpImulllu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imulllu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37446 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37451 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37456 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImulll::OpImulll(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imulll")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37470 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37475 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37480 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImulhu::OpImulhu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imulhu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37494 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37499 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37504 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImulh::OpImulh(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imulh")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37518 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37523 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37528 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImullu::OpImullu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imullu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37542 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37547 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37552 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImull::OpImull(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imull")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37566 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37571 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37576 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImulhhs::OpImulhhs(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imulhhs")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37590 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37595 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37600 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIminu::OpIminu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "iminu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37614 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37619 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37624 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImin::OpImin(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imin")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37638 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37643 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37648 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImaxu::OpImaxu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imaxu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37662 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37667 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37672 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImax::OpImax(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imax")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37686 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37691 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37696 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpImullhus::OpImullhus(st231::CodeType code, uint32_t addr) : Operation(code, addr, "imullhus")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37710 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37715 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37720 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	nlidest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIxor::OpIxor(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ixor")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37734 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37739 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37744 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIorc::OpIorc(st231::CodeType code, uint32_t addr) : Operation(code, addr, "iorc")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37758 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37763 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37768 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIor::OpIor(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ior")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37782 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37787 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37792 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIandc::OpIandc(st231::CodeType code, uint32_t addr) : Operation(code, addr, "iandc")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37806 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37811 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37816 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIand::OpIand(st231::CodeType code, uint32_t addr) : Operation(code, addr, "iand")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37830 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37835 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37840 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIsh4add::OpIsh4add(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ish4add")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37854 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37859 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37864 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIsh3add::OpIsh3add(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ish3add")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37878 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37883 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37888 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIsh2add::OpIsh2add(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ish2add")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37902 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37907 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37912 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIsh1add::OpIsh1add(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ish1add")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37926 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37931 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37936 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIshru::OpIshru(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ishru")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37950 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37955 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37960 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIshr::OpIshr(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ishr")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37974 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 37979 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 37984 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIshl::OpIshl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "ishl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 37998 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38003 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38008 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIsub::OpIsub(st231::CodeType code, uint32_t addr) : Operation(code, addr, "isub")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38022 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38027 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38032 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpIadd::OpIadd(st231::CodeType code, uint32_t addr) : Operation(code, addr, "iadd")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38046 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38051 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38056 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	isrc2 = ((code >> 12) & 0x1ff);
	idest = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMulfrac::OpMulfrac(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mulfrac")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38070 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38075 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38080 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMul64hu::OpMul64hu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mul64hu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38095 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38100 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38105 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBnorl::OpBnorl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bnorl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38120 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38125 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38130 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBorl::OpBorl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "borl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38145 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38150 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38155 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBnandl::OpBnandl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bnandl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38170 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38175 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38180 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBandl::OpBandl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bandl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38195 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38200 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38205 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmpltu::OpBcmpltu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmpltu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38220 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38225 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38230 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmplt::OpBcmplt(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmplt")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38245 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38250 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38255 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmpleu::OpBcmpleu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmpleu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38270 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38275 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38280 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmple::OpBcmple(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmple")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38295 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38300 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38305 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmpgtu::OpBcmpgtu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmpgtu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38320 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38325 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38330 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmpgt::OpBcmpgt(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmpgt")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38345 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38350 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38355 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmpgeu::OpBcmpgeu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmpgeu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38370 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38375 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38380 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmpge::OpBcmpge(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmpge")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38395 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38400 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38405 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmpne::OpBcmpne(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmpne")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38420 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38425 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38430 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpBcmpeq::OpBcmpeq(st231::CodeType code, uint32_t addr) : Operation(code, addr, "bcmpeq")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38445 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38450 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38455 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMul64h::OpMul64h(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mul64h")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38470 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38475 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38480 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMul32::OpMul32(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mul32")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38495 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38500 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38505 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpNorl::OpNorl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "norl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38520 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38525 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38530 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpOrl::OpOrl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "orl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38545 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38550 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38555 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpNandl::OpNandl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "nandl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38570 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38575 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38580 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpAndl::OpAndl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "andl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38595 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38600 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38605 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmpltu::OpCmpltu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmpltu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38620 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38625 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38630 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmplt::OpCmplt(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmplt")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38645 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38650 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38655 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmpleu::OpCmpleu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmpleu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38670 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38675 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38680 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmple::OpCmple(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmple")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38695 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38700 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38705 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmpgtu::OpCmpgtu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmpgtu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38720 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38725 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38730 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmpgt::OpCmpgt(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmpgt")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38745 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38750 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38755 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmpgeu::OpCmpgeu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmpgeu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38770 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38775 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38780 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmpge::OpCmpge(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmpge")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38795 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38800 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38805 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmpne::OpCmpne(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmpne")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38820 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38825 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38830 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpCmpeq::OpCmpeq(st231::CodeType code, uint32_t addr) : Operation(code, addr, "cmpeq")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38845 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38850 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38855 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMulhs::OpMulhs(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mulhs")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38870 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38875 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38880 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMulhhu::OpMulhhu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mulhhu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38895 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38900 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38905 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMulhh::OpMulhh(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mulhh")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38920 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38925 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38930 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMullhu::OpMullhu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mullhu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38945 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38950 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38955 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMullh::OpMullh(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mullh")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38970 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 38975 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 38980 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMulllu::OpMulllu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mulllu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 38995 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39000 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39005 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMulll::OpMulll(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mulll")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39020 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39025 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39030 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMulhu::OpMulhu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mulhu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39045 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39050 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39055 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMulh::OpMulh(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mulh")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39070 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39075 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39080 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMullu::OpMullu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mullu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39095 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39100 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39105 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMull::OpMull(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mull")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39120 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39125 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39130 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMulhhs::OpMulhhs(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mulhhs")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39145 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39150 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39155 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMinu::OpMinu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "minu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39170 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39175 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39180 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMin::OpMin(st231::CodeType code, uint32_t addr) : Operation(code, addr, "min")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39195 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39200 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39205 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMaxu::OpMaxu(st231::CodeType code, uint32_t addr) : Operation(code, addr, "maxu")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39220 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39225 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39230 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMax::OpMax(st231::CodeType code, uint32_t addr) : Operation(code, addr, "max")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39245 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39250 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39255 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpMullhus::OpMullhus(st231::CodeType code, uint32_t addr) : Operation(code, addr, "mullhus")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39270 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39275 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39280 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	nldest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpXor::OpXor(st231::CodeType code, uint32_t addr) : Operation(code, addr, "xor")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39295 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39300 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39305 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpOrc::OpOrc(st231::CodeType code, uint32_t addr) : Operation(code, addr, "orc")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39320 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39325 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39330 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpOr::OpOr(st231::CodeType code, uint32_t addr) : Operation(code, addr, "or")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39345 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39350 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39355 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpAndc::OpAndc(st231::CodeType code, uint32_t addr) : Operation(code, addr, "andc")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39370 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39375 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39380 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpAnd::OpAnd(st231::CodeType code, uint32_t addr) : Operation(code, addr, "and")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39395 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39400 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39405 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSh4add::OpSh4add(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sh4add")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39420 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39425 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39430 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSh3add::OpSh3add(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sh3add")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39445 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39450 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39455 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSh2add::OpSh2add(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sh2add")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39470 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39475 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39480 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSh1add::OpSh1add(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sh1add")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39495 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39500 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39505 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpShru::OpShru(st231::CodeType code, uint32_t addr) : Operation(code, addr, "shru")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39520 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39525 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39530 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpRshr::OpRshr(st231::CodeType code, uint32_t addr) : Operation(code, addr, "rshr")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39545 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39550 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39555 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpRshl::OpRshl(st231::CodeType code, uint32_t addr) : Operation(code, addr, "rshl")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39570 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39575 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39580 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpSub::OpSub(st231::CodeType code, uint32_t addr) : Operation(code, addr, "sub")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39595 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39600 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39605 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

OpAdd::OpAdd(st231::CodeType code, uint32_t addr) : Operation(code, addr, "add")
{
	noperands = 
#line 46 "sim_dependencies.isa"
0;

#line 39620 "cpu/st231_isa.cpp"
	latency = 
#line 8 "sim_latencies.isa"
0;

#line 39625 "cpu/st231_isa.cpp"
	nstages = 
#line 9 "sim_latencies.isa"
0;

#line 39630 "cpu/st231_isa.cpp"
	stop = ((code >> 31) & 0x1);
	rsv = ((code >> 30) & 0x1);
	bdest = ((code >> 18) & 0x7);
	dest = ((code >> 12) & 0x3f);
	rsc2 = ((code >> 6) & 0x3f);
	rsc1 = ((code >> 0) & 0x3f);
}

DecodeMapPage::DecodeMapPage(uint32_t key)
{
	this->key = key;
	memset(operation, 0, sizeof(operation));
	next = 0;
}

DecodeMapPage::~DecodeMapPage()
{
	unsigned int i;
	for(i = 0; i < NUM_OPERATIONS_PER_PAGE; i++)
		delete operation[i];
}

	DecodeTableEntry::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation *(*decode)(CodeType, uint32_t))
	{
		this->opcode = opcode;
		this->opcode_mask = opcode_mask;
		this->decode = decode;
	}

Decoder::Decoder()
{
	little_endian = true;
	mru_page = 0;
	memset(decode_hash_table, 0, sizeof(decode_hash_table));
	decode_table.push_back(DecodeTableEntry(0x29800000UL, 0x3f80003fUL, DecodeOpPswclr));
	decode_table.push_back(DecodeTableEntry(0x29000000UL, 0x3f80003fUL, DecodeOpPswset));
	decode_table.push_back(DecodeTableEntry(0x28000000UL, 0x3f9fffffUL, DecodeOpSync));
	decode_table.push_back(DecodeTableEntry(0x28800000UL, 0x3f800fc0UL, DecodeOpPrginspg));
	decode_table.push_back(DecodeTableEntry(0x27800000UL, 0x3f800fc0UL, DecodeOpPrgset));
	decode_table.push_back(DecodeTableEntry(0x27000000UL, 0x3f800fc0UL, DecodeOpPrgadd));
	decode_table.push_back(DecodeTableEntry(0x26800000UL, 0x3f800fc0UL, DecodeOpPft));
	decode_table.push_back(DecodeTableEntry(0x26000000UL, 0x3f800000UL, DecodeOpStb));
	decode_table.push_back(DecodeTableEntry(0x25800000UL, 0x3f800000UL, DecodeOpSth));
	decode_table.push_back(DecodeTableEntry(0x25000000UL, 0x3f800000UL, DecodeOpStw));
	decode_table.push_back(DecodeTableEntry(0x24800000UL, 0x3f800000UL, DecodeOpLdbu_d));
	decode_table.push_back(DecodeTableEntry(0x24000000UL, 0x3f800000UL, DecodeOpLdbu));
	decode_table.push_back(DecodeTableEntry(0x23800000UL, 0x3f800000UL, DecodeOpLdb_d));
	decode_table.push_back(DecodeTableEntry(0x23000000UL, 0x3f800000UL, DecodeOpLdb));
	decode_table.push_back(DecodeTableEntry(0x22800000UL, 0x3f800000UL, DecodeOpLdhu_d));
	decode_table.push_back(DecodeTableEntry(0x22000000UL, 0x3f800000UL, DecodeOpLdhu));
	decode_table.push_back(DecodeTableEntry(0x21800000UL, 0x3f800000UL, DecodeOpLdh_d));
	decode_table.push_back(DecodeTableEntry(0x21000000UL, 0x3f800000UL, DecodeOpLdh));
	decode_table.push_back(DecodeTableEntry(0x20800000UL, 0x3f800000UL, DecodeOpLdw_d));
	decode_table.push_back(DecodeTableEntry(0x20000000UL, 0x3f800000UL, DecodeOpLdw));
	decode_table.push_back(DecodeTableEntry(0x1fe00000UL, 0x3fe00000UL, DecodeOpBreak));
	decode_table.push_back(DecodeTableEntry(0x1fc00000UL, 0x3fe00000UL, DecodeOpSyscall));
	decode_table.push_back(DecodeTableEntry(0x1fa00000UL, 0x3fe00000UL, DecodeOpSbrk));
	decode_table.push_back(DecodeTableEntry(0x1f800000UL, 0x3fe00000UL, DecodeOpPrgins));
	decode_table.push_back(DecodeTableEntry(0x19000000UL, 0x3f000000UL, DecodeOpIslctf));
	decode_table.push_back(DecodeTableEntry(0x18000000UL, 0x3f000000UL, DecodeOpIslct));
	decode_table.push_back(DecodeTableEntry(0x15800000UL, 0x3f800000UL, DecodeOpImmr));
	decode_table.push_back(DecodeTableEntry(0x15000000UL, 0x3f800000UL, DecodeOpImml));
	decode_table.push_back(DecodeTableEntry(0x14000000UL, 0x3f000000UL, DecodeOpDivs));
	decode_table.push_back(DecodeTableEntry(0x12000000UL, 0x3f000000UL, DecodeOpAddcg));
	decode_table.push_back(DecodeTableEntry(0x11000000UL, 0x3f000000UL, DecodeOpSlctf));
	decode_table.push_back(DecodeTableEntry(0x10000000UL, 0x3f000000UL, DecodeOpSlct));
	decode_table.push_back(DecodeTableEntry(0x3c000000UL, 0x3c000000UL, DecodeOpBrf));
	decode_table.push_back(DecodeTableEntry(0x38000000UL, 0x3c000000UL, DecodeOpBr));
	decode_table.push_back(DecodeTableEntry(0x32000000UL, 0x3fffffffUL, DecodeOpRfi));
	decode_table.push_back(DecodeTableEntry(0x31800000UL, 0x3fffffffUL, DecodeOpGoto));
	decode_table.push_back(DecodeTableEntry(0x31000000UL, 0x3f800000UL, DecodeOpIgoto));
	decode_table.push_back(DecodeTableEntry(0x30800000UL, 0x3fffffffUL, DecodeOpCall));
	decode_table.push_back(DecodeTableEntry(0x30000000UL, 0x3f800000UL, DecodeOpIcall));
	decode_table.push_back(DecodeTableEntry(0x9c04000UL, 0x3ffff000UL, DecodeOpClz));
	decode_table.push_back(DecodeTableEntry(0x9c03000UL, 0x3ffff000UL, DecodeOpZxth));
	decode_table.push_back(DecodeTableEntry(0x9c02000UL, 0x3ffff000UL, DecodeOpBswap));
	decode_table.push_back(DecodeTableEntry(0x9c01000UL, 0x3ffff000UL, DecodeOpSxth));
	decode_table.push_back(DecodeTableEntry(0x9c00000UL, 0x3ffff000UL, DecodeOpSxtb));
	decode_table.push_back(DecodeTableEntry(0xfe00000UL, 0x3fe00000UL, DecodeOpImulfrac));
	decode_table.push_back(DecodeTableEntry(0xfc00000UL, 0x3fe00000UL, DecodeOpImul64hu));
	decode_table.push_back(DecodeTableEntry(0xfa00000UL, 0x3fe00000UL, DecodeOpIbnorl));
	decode_table.push_back(DecodeTableEntry(0xf800000UL, 0x3fe00000UL, DecodeOpIborl));
	decode_table.push_back(DecodeTableEntry(0xf600000UL, 0x3fe00000UL, DecodeOpIbnandl));
	decode_table.push_back(DecodeTableEntry(0xf400000UL, 0x3fe00000UL, DecodeOpIbandl));
	decode_table.push_back(DecodeTableEntry(0xf200000UL, 0x3fe00000UL, DecodeOpIbcmpltu));
	decode_table.push_back(DecodeTableEntry(0xf000000UL, 0x3fe00000UL, DecodeOpIbcmplt));
	decode_table.push_back(DecodeTableEntry(0xee00000UL, 0x3fe00000UL, DecodeOpIbcmpleu));
	decode_table.push_back(DecodeTableEntry(0xec00000UL, 0x3fe00000UL, DecodeOpIbcmple));
	decode_table.push_back(DecodeTableEntry(0xea00000UL, 0x3fe00000UL, DecodeOpIbcmpgtu));
	decode_table.push_back(DecodeTableEntry(0xe800000UL, 0x3fe00000UL, DecodeOpIbcmpgt));
	decode_table.push_back(DecodeTableEntry(0xe600000UL, 0x3fe00000UL, DecodeOpIbcmpgeu));
	decode_table.push_back(DecodeTableEntry(0xe400000UL, 0x3fe00000UL, DecodeOpIbcmpge));
	decode_table.push_back(DecodeTableEntry(0xe200000UL, 0x3fe00000UL, DecodeOpIbcmpne));
	decode_table.push_back(DecodeTableEntry(0xe000000UL, 0x3fe00000UL, DecodeOpIbcmpeq));
	decode_table.push_back(DecodeTableEntry(0xde00000UL, 0x3fe00000UL, DecodeOpImul64h));
	decode_table.push_back(DecodeTableEntry(0xdc00000UL, 0x3fe00000UL, DecodeOpImul32));
	decode_table.push_back(DecodeTableEntry(0xda00000UL, 0x3fe00000UL, DecodeOpInorl));
	decode_table.push_back(DecodeTableEntry(0xd800000UL, 0x3fe00000UL, DecodeOpIorl));
	decode_table.push_back(DecodeTableEntry(0xd600000UL, 0x3fe00000UL, DecodeOpInandl));
	decode_table.push_back(DecodeTableEntry(0xd400000UL, 0x3fe00000UL, DecodeOpIandl));
	decode_table.push_back(DecodeTableEntry(0xd200000UL, 0x3fe00000UL, DecodeOpIcmpltu));
	decode_table.push_back(DecodeTableEntry(0xd000000UL, 0x3fe00000UL, DecodeOpIcmplt));
	decode_table.push_back(DecodeTableEntry(0xce00000UL, 0x3fe00000UL, DecodeOpIcmpleu));
	decode_table.push_back(DecodeTableEntry(0xcc00000UL, 0x3fe00000UL, DecodeOpIcmple));
	decode_table.push_back(DecodeTableEntry(0xca00000UL, 0x3fe00000UL, DecodeOpIcmpgtu));
	decode_table.push_back(DecodeTableEntry(0xc800000UL, 0x3fe00000UL, DecodeOpIcmpgt));
	decode_table.push_back(DecodeTableEntry(0xc600000UL, 0x3fe00000UL, DecodeOpIcmpgeu));
	decode_table.push_back(DecodeTableEntry(0xc400000UL, 0x3fe00000UL, DecodeOpIcmpge));
	decode_table.push_back(DecodeTableEntry(0xc200000UL, 0x3fe00000UL, DecodeOpIcmpne));
	decode_table.push_back(DecodeTableEntry(0xc000000UL, 0x3fe00000UL, DecodeOpIcmpeq));
	decode_table.push_back(DecodeTableEntry(0xbe00000UL, 0x3fe00000UL, DecodeOpImulhs));
	decode_table.push_back(DecodeTableEntry(0xbc00000UL, 0x3fe00000UL, DecodeOpImulhhu));
	decode_table.push_back(DecodeTableEntry(0xba00000UL, 0x3fe00000UL, DecodeOpImulhh));
	decode_table.push_back(DecodeTableEntry(0xb800000UL, 0x3fe00000UL, DecodeOpImullhu));
	decode_table.push_back(DecodeTableEntry(0xb600000UL, 0x3fe00000UL, DecodeOpImullh));
	decode_table.push_back(DecodeTableEntry(0xb400000UL, 0x3fe00000UL, DecodeOpImulllu));
	decode_table.push_back(DecodeTableEntry(0xb200000UL, 0x3fe00000UL, DecodeOpImulll));
	decode_table.push_back(DecodeTableEntry(0xb000000UL, 0x3fe00000UL, DecodeOpImulhu));
	decode_table.push_back(DecodeTableEntry(0xae00000UL, 0x3fe00000UL, DecodeOpImulh));
	decode_table.push_back(DecodeTableEntry(0xac00000UL, 0x3fe00000UL, DecodeOpImullu));
	decode_table.push_back(DecodeTableEntry(0xaa00000UL, 0x3fe00000UL, DecodeOpImull));
	decode_table.push_back(DecodeTableEntry(0xa800000UL, 0x3fe00000UL, DecodeOpImulhhs));
	decode_table.push_back(DecodeTableEntry(0xa600000UL, 0x3fe00000UL, DecodeOpIminu));
	decode_table.push_back(DecodeTableEntry(0xa400000UL, 0x3fe00000UL, DecodeOpImin));
	decode_table.push_back(DecodeTableEntry(0xa200000UL, 0x3fe00000UL, DecodeOpImaxu));
	decode_table.push_back(DecodeTableEntry(0xa000000UL, 0x3fe00000UL, DecodeOpImax));
	decode_table.push_back(DecodeTableEntry(0x9e00000UL, 0x3fe00000UL, DecodeOpImullhus));
	decode_table.push_back(DecodeTableEntry(0x9a00000UL, 0x3fe00000UL, DecodeOpIxor));
	decode_table.push_back(DecodeTableEntry(0x9800000UL, 0x3fe00000UL, DecodeOpIorc));
	decode_table.push_back(DecodeTableEntry(0x9600000UL, 0x3fe00000UL, DecodeOpIor));
	decode_table.push_back(DecodeTableEntry(0x9400000UL, 0x3fe00000UL, DecodeOpIandc));
	decode_table.push_back(DecodeTableEntry(0x9200000UL, 0x3fe00000UL, DecodeOpIand));
	decode_table.push_back(DecodeTableEntry(0x9000000UL, 0x3fe00000UL, DecodeOpIsh4add));
	decode_table.push_back(DecodeTableEntry(0x8e00000UL, 0x3fe00000UL, DecodeOpIsh3add));
	decode_table.push_back(DecodeTableEntry(0x8c00000UL, 0x3fe00000UL, DecodeOpIsh2add));
	decode_table.push_back(DecodeTableEntry(0x8a00000UL, 0x3fe00000UL, DecodeOpIsh1add));
	decode_table.push_back(DecodeTableEntry(0x8800000UL, 0x3fe00000UL, DecodeOpIshru));
	decode_table.push_back(DecodeTableEntry(0x8600000UL, 0x3fe00000UL, DecodeOpIshr));
	decode_table.push_back(DecodeTableEntry(0x8400000UL, 0x3fe00000UL, DecodeOpIshl));
	decode_table.push_back(DecodeTableEntry(0x8200000UL, 0x3fe00000UL, DecodeOpIsub));
	decode_table.push_back(DecodeTableEntry(0x8000000UL, 0x3fe00000UL, DecodeOpIadd));
	decode_table.push_back(DecodeTableEntry(0x7e00000UL, 0x3fe00000UL, DecodeOpMulfrac));
	decode_table.push_back(DecodeTableEntry(0x7c00000UL, 0x3fe00000UL, DecodeOpMul64hu));
	decode_table.push_back(DecodeTableEntry(0x7a00000UL, 0x3fe00000UL, DecodeOpBnorl));
	decode_table.push_back(DecodeTableEntry(0x7800000UL, 0x3fe00000UL, DecodeOpBorl));
	decode_table.push_back(DecodeTableEntry(0x7600000UL, 0x3fe00000UL, DecodeOpBnandl));
	decode_table.push_back(DecodeTableEntry(0x7400000UL, 0x3fe00000UL, DecodeOpBandl));
	decode_table.push_back(DecodeTableEntry(0x7200000UL, 0x3fe00000UL, DecodeOpBcmpltu));
	decode_table.push_back(DecodeTableEntry(0x7000000UL, 0x3fe00000UL, DecodeOpBcmplt));
	decode_table.push_back(DecodeTableEntry(0x6e00000UL, 0x3fe00000UL, DecodeOpBcmpleu));
	decode_table.push_back(DecodeTableEntry(0x6c00000UL, 0x3fe00000UL, DecodeOpBcmple));
	decode_table.push_back(DecodeTableEntry(0x6a00000UL, 0x3fe00000UL, DecodeOpBcmpgtu));
	decode_table.push_back(DecodeTableEntry(0x6800000UL, 0x3fe00000UL, DecodeOpBcmpgt));
	decode_table.push_back(DecodeTableEntry(0x6600000UL, 0x3fe00000UL, DecodeOpBcmpgeu));
	decode_table.push_back(DecodeTableEntry(0x6400000UL, 0x3fe00000UL, DecodeOpBcmpge));
	decode_table.push_back(DecodeTableEntry(0x6200000UL, 0x3fe00000UL, DecodeOpBcmpne));
	decode_table.push_back(DecodeTableEntry(0x6000000UL, 0x3fe00000UL, DecodeOpBcmpeq));
	decode_table.push_back(DecodeTableEntry(0x5e00000UL, 0x3fe00000UL, DecodeOpMul64h));
	decode_table.push_back(DecodeTableEntry(0x5c00000UL, 0x3fe00000UL, DecodeOpMul32));
	decode_table.push_back(DecodeTableEntry(0x5a00000UL, 0x3fe00000UL, DecodeOpNorl));
	decode_table.push_back(DecodeTableEntry(0x5800000UL, 0x3fe00000UL, DecodeOpOrl));
	decode_table.push_back(DecodeTableEntry(0x5600000UL, 0x3fe00000UL, DecodeOpNandl));
	decode_table.push_back(DecodeTableEntry(0x5400000UL, 0x3fe00000UL, DecodeOpAndl));
	decode_table.push_back(DecodeTableEntry(0x5200000UL, 0x3fe00000UL, DecodeOpCmpltu));
	decode_table.push_back(DecodeTableEntry(0x5000000UL, 0x3fe00000UL, DecodeOpCmplt));
	decode_table.push_back(DecodeTableEntry(0x4e00000UL, 0x3fe00000UL, DecodeOpCmpleu));
	decode_table.push_back(DecodeTableEntry(0x4c00000UL, 0x3fe00000UL, DecodeOpCmple));
	decode_table.push_back(DecodeTableEntry(0x4a00000UL, 0x3fe00000UL, DecodeOpCmpgtu));
	decode_table.push_back(DecodeTableEntry(0x4800000UL, 0x3fe00000UL, DecodeOpCmpgt));
	decode_table.push_back(DecodeTableEntry(0x4600000UL, 0x3fe00000UL, DecodeOpCmpgeu));
	decode_table.push_back(DecodeTableEntry(0x4400000UL, 0x3fe00000UL, DecodeOpCmpge));
	decode_table.push_back(DecodeTableEntry(0x4200000UL, 0x3fe00000UL, DecodeOpCmpne));
	decode_table.push_back(DecodeTableEntry(0x4000000UL, 0x3fe00000UL, DecodeOpCmpeq));
	decode_table.push_back(DecodeTableEntry(0x3e00000UL, 0x3fe00000UL, DecodeOpMulhs));
	decode_table.push_back(DecodeTableEntry(0x3c00000UL, 0x3fe00000UL, DecodeOpMulhhu));
	decode_table.push_back(DecodeTableEntry(0x3a00000UL, 0x3fe00000UL, DecodeOpMulhh));
	decode_table.push_back(DecodeTableEntry(0x3800000UL, 0x3fe00000UL, DecodeOpMullhu));
	decode_table.push_back(DecodeTableEntry(0x3600000UL, 0x3fe00000UL, DecodeOpMullh));
	decode_table.push_back(DecodeTableEntry(0x3400000UL, 0x3fe00000UL, DecodeOpMulllu));
	decode_table.push_back(DecodeTableEntry(0x3200000UL, 0x3fe00000UL, DecodeOpMulll));
	decode_table.push_back(DecodeTableEntry(0x3000000UL, 0x3fe00000UL, DecodeOpMulhu));
	decode_table.push_back(DecodeTableEntry(0x2e00000UL, 0x3fe00000UL, DecodeOpMulh));
	decode_table.push_back(DecodeTableEntry(0x2c00000UL, 0x3fe00000UL, DecodeOpMullu));
	decode_table.push_back(DecodeTableEntry(0x2a00000UL, 0x3fe00000UL, DecodeOpMull));
	decode_table.push_back(DecodeTableEntry(0x2800000UL, 0x3fe00000UL, DecodeOpMulhhs));
	decode_table.push_back(DecodeTableEntry(0x2600000UL, 0x3fe00000UL, DecodeOpMinu));
	decode_table.push_back(DecodeTableEntry(0x2400000UL, 0x3fe00000UL, DecodeOpMin));
	decode_table.push_back(DecodeTableEntry(0x2200000UL, 0x3fe00000UL, DecodeOpMaxu));
	decode_table.push_back(DecodeTableEntry(0x2000000UL, 0x3fe00000UL, DecodeOpMax));
	decode_table.push_back(DecodeTableEntry(0x1e00000UL, 0x3fe00000UL, DecodeOpMullhus));
	decode_table.push_back(DecodeTableEntry(0x1a00000UL, 0x3fe00000UL, DecodeOpXor));
	decode_table.push_back(DecodeTableEntry(0x1800000UL, 0x3fe00000UL, DecodeOpOrc));
	decode_table.push_back(DecodeTableEntry(0x1600000UL, 0x3fe00000UL, DecodeOpOr));
	decode_table.push_back(DecodeTableEntry(0x1400000UL, 0x3fe00000UL, DecodeOpAndc));
	decode_table.push_back(DecodeTableEntry(0x1200000UL, 0x3fe00000UL, DecodeOpAnd));
	decode_table.push_back(DecodeTableEntry(0x1000000UL, 0x3fe00000UL, DecodeOpSh4add));
	decode_table.push_back(DecodeTableEntry(0xe00000UL, 0x3fe00000UL, DecodeOpSh3add));
	decode_table.push_back(DecodeTableEntry(0xc00000UL, 0x3fe00000UL, DecodeOpSh2add));
	decode_table.push_back(DecodeTableEntry(0xa00000UL, 0x3fe00000UL, DecodeOpSh1add));
	decode_table.push_back(DecodeTableEntry(0x800000UL, 0x3fe00000UL, DecodeOpShru));
	decode_table.push_back(DecodeTableEntry(0x600000UL, 0x3fe00000UL, DecodeOpRshr));
	decode_table.push_back(DecodeTableEntry(0x400000UL, 0x3fe00000UL, DecodeOpRshl));
	decode_table.push_back(DecodeTableEntry(0x200000UL, 0x3fe00000UL, DecodeOpSub));
	decode_table.push_back(DecodeTableEntry(0x0UL, 0x3fe00000UL, DecodeOpAdd));
}

Decoder::~Decoder()
{
	InvalidateDecodingCache();
}

Operation *Decoder::NCDecode(uint32_t addr)
{
	Operation *operation;
	vector<DecodeTableEntry >::iterator iter;
	CodeType code;
	Fetch(&code, addr, sizeof(code));
#if BYTE_ORDER == LITTLE_ENDIAN
	if(!little_endian)
#else
	if(little_endian)
#endif
	{
		code = ((code & 0xff000000UL) >> 24) | ((code & 0x00ff0000UL) >> 8) | ((code & 0x0000ff00UL) << 8) | ((code & 0x000000ffUL) << 24);
	}
	unsigned int count = decode_table.size();
	unsigned int i;
	for(i = 0; i < count; i++)
	{
		if((code & decode_table[i].opcode_mask) == decode_table[i].opcode)
		{
			operation = decode_table[i].decode(code, addr);
			operation->initialize_function();
			operation->initialize_latencies();
			operation->initialize_operands();
			return operation;
		}
	}
	operation = new Operation(code, addr, "???");
	operation->initialize_function();
	operation->initialize_latencies();
	operation->initialize_operands();
	return operation;
}

Operation *Decoder::NCDecode(uint32_t addr, CodeType code)
{
	Operation *operation;
	vector<DecodeTableEntry >::iterator iter;
	unsigned int count = decode_table.size();
	unsigned int i;
	for(i = 0; i < count; i++)
	{
		if((code & decode_table[i].opcode_mask) == decode_table[i].opcode)
		{
			operation = decode_table[i].decode(code, addr);
			operation->initialize_function();
			operation->initialize_latencies();
			operation->initialize_operands();
			return operation;
		}
	}
	operation = new Operation(code, addr, "???");
	operation->initialize_function();
	operation->initialize_latencies();
	operation->initialize_operands();
	return operation;
}

void Decoder::InvalidateDecodingCache()
{
	uint32_t index;
	mru_page = 0;
	for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)
	{
		DecodeMapPage *page, *next_page;
		page = decode_hash_table[index];
		if(page)
		{
			do
			{
			next_page = page->next;
			delete page;
			page = next_page;
			} while(page);
		decode_hash_table[index] = 0;
		}
	}
}

void Decoder::InvalidateDecodingCacheEntry(uint32_t addr)
{
	uint32_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	if(mru_page && mru_page->key == page_key) mru_page = 0;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage *prev, *cur;
	cur = decode_hash_table[index];
	if(cur)
	{
		if(cur->key == page_key)
		{
			decode_hash_table[index] = cur->next;
			delete cur;
			return;
		}
		prev = cur;
		cur = cur->next;
		if(cur)
		{
			do
			{
				if(cur->key == page_key)
				{
					prev->next = cur->next;
					cur->next = 0;
					delete cur;
					return;
				}
				prev = cur;
			} while((cur = cur->next) != 0);
		}
	}
}

inline DecodeMapPage *Decoder::FindPage(uint32_t page_key)
{
	if(mru_page && mru_page->key == page_key) return mru_page;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage *prev, *cur;
	cur = decode_hash_table[index];
	if(cur)
	{
		if(cur->key == page_key)
		{
			mru_page = cur;
			return cur;
		}
		prev = cur;
		cur = cur->next;
		if(cur)
		{
			do
			{
				if(cur->key == page_key)
				{
					prev->next = cur->next;
					cur->next= decode_hash_table[index];
					decode_hash_table[index] = cur;
					mru_page = cur;
					return cur;
				}
				prev = cur;
			} while((cur = cur->next) != 0);
		}
	}
	return 0;
}

Operation *Decoder::Decode(uint32_t addr)
{
	Operation *operation;
	uint32_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage(page_key);
		uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
		page->next = decode_hash_table[index];
		decode_hash_table[index] = page;
		mru_page = page;
	}
	operation = page->operation[(addr / 4) & (NUM_OPERATIONS_PER_PAGE - 1)];
	if(operation)
	{
		return operation;
	}
	operation = NCDecode(addr);
	page->operation[(addr / 4) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;
	return operation;
}

Operation *Decoder::Decode(uint32_t addr, CodeType insn)
{
	Operation *operation;
	uint32_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage (page_key);
		uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
		page->next = decode_hash_table[index];
		decode_hash_table[index] = page;
		mru_page = page;
	}
	operation = page->operation[(addr / 4) & (NUM_OPERATIONS_PER_PAGE - 1)];
	if(operation)
	{
		if(operation->GetEncoding() == insn && operation->GetAddr() == addr)
			return operation;
		delete operation;
	}
	operation = NCDecode(addr, insn);
	page->operation[(addr / 4) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;
	return operation;
}

void Decoder::SetLittleEndian()
{
	little_endian = true;
}

void Decoder::SetBigEndian()
{
	little_endian = false;
}

} 

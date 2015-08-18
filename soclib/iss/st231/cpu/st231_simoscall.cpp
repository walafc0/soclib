#include "st231_simoscall.hh"
#include "st231_isa.hh"

using namespace std;

namespace st231{

xiss_bool_t simoscall_mem_read(void *sd,
                                   xiss_addr_t address,
                                   xiss_size_t size,
                                   xiss_byte_t *buffer)
{
	CPU *cpu = (st231::CPU *)sd;
	cpu->ReadMemoryBuffer(address,buffer,size);
	return TRUE;
}

xiss_bool_t simoscall_mem_write(void *sd,
                                    xiss_addr_t address,
                                    xiss_size_t size,
                                    const xiss_byte_t *buffer) 
{
	CPU *cpu = (st231::CPU *)sd;
	cpu->WriteMemoryBuffer(address,buffer,size);
	return TRUE;
}

xiss_bool_t simoscall_reg_read(void * desc,
				    xiss_uint32_t slice_id,
				    xiss_uint32_t thread_id,
				    xiss_uint32_t reg_no,
				    xiss_uint32_t *value)
{
	CPU *cpu = (st231::CPU *)desc;
	if (reg_no >= 64)
		return FALSE;

	*value = cpu->GetGPR_C(reg_no);
	return TRUE;
}

xiss_bool_t simoscall_reg_write(void * desc,
				    xiss_uint32_t slice_id,
				    xiss_uint32_t thread_id,
				    xiss_uint32_t reg_no,
				    xiss_uint32_t value)
{
	CPU *cpu = (st231::CPU *)desc;
	if (reg_no >= 64)
		return FALSE;

	cpu->SetGPR_N(reg_no,value);
	return TRUE;
}

xiss_bool_t simoscall_exit(void * desc,
				xiss_uint32_t slice_id,
				xiss_uint32_t thread_id,
				xiss_int32_t status)
{
	CPU *cpu = (st231::CPU *)desc;
	cpu->sim_exit();
	return FALSE;
}

}

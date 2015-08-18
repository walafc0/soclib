#include "prod_proto.h"
#include <srl.h>

#include "../common/struct.h"

FUNC(prod_func) {
  srl_mwmr_t input  = GET_ARG(input);
  srl_mwmr_t output = GET_ARG(output);
  struct search_tuple_desc desc;
  short ia, ib;
  short in;

  srl_log_printf(TRACE, "PROD WORKING INITIAL_A = %d FINAL_A = %d INIT\
IAL_B = %d FINAL_B = %d\n", initial_a, final_a, initial_b, final_b);

  for (ia=initial_a ; ia < final_a ; ia++){
    for (ib=initial_b ; ib < final_b ; ib++)
      {
	desc.ia=ia;
	desc.ib=ib;
	srl_mwmr_write(output, &desc, sizeof(desc));
      }
  }

  srl_mwmr_read(input, &in, sizeof(in));
}

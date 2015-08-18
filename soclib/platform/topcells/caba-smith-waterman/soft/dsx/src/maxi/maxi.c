#include "maxi_proto.h"
#include <srl.h>

#include "../common/struct.h"
#include "../common/edbseq.h"
#include "../common/eqseq.h"

FUNC(maxi_func) {
  	srl_mwmr_t input = GET_ARG(input);
  	struct result_tuple_desc in_desc;
	short ia, ib;
	short maxv, maxia, maxib;
	short histo[100];

	maxv=0;
	for (ia=0 ; ia < 100 ; ia++)
		histo[ia]=0;

        for (ia=0 ; ia < A ; ia++){
          for (ib=0 ; ib < B ; ib++)
	    {
	      srl_mwmr_read(input, &in_desc, sizeof(in_desc));
	      
	      histo[(int)in_desc.length]++;
	      if (in_desc.length>maxv)
		{
		  maxv=in_desc.length;
		  maxia=in_desc.ia;
		  maxib=in_desc.ib;
		}
              srl_log_printf(NONE, "A=%d B=%d\n",in_desc.ia,in_desc.ib);
	    }
	}
	srl_log_printf(NONE, "MAXI A=%d B=%d LENGTH=%d\n", maxia, maxib, maxv);
	for (ia=0 ; ia <= maxv ; ia++)
	  srl_log_printf(NONE, "histo[%d]=%d\n", ia,histo[ia]);

	int numberCycles = srl_cycle_count();
	srl_log_printf(NONE, "Number of simulation cycles  = %d\n", numberCycles);
        srl_abort();
}

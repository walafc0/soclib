#include "crunch_proto.h"
#include <srl.h>

#include "../common/struct.h"
#include "../common/edbseq.h"
#include "../common/eqseq.h"
#include "../common/aa.h"
#include "../common/const.h"
#include "../common/qseq.h"
#include "../common/dbseq.h"

FUNC(crunch_func) {
  	srl_mwmr_t input = GET_ARG(input);
  	srl_mwmr_t output = GET_ARG(output);

  	struct search_tuple_desc in_desc;
  	struct result_tuple_desc out_desc;

	short DELTA;
	short ia, ib;
	short max, Max;
	short i, j;
        short a[N], b[N];
        short h[N][N];
        short sim[AA][AA];     
	short diag, up, left;

	DELTA=-1;


	//printf("CRUNCH WORKING");
	//srl_log_printf(NONE, "CRUNCH WORKING");
		
        for (i=0;i<AA;i++)
        {
                for (j=0;j<AA;j++)
                        if (i==j)
                                sim[i][j]=2;
                        else
                                sim[i][j]= -1;
        }

  	while (1)
	{
  		srl_mwmr_read(input, &in_desc, sizeof(in_desc));
  
		ia=in_desc.ia;
		ib=in_desc.ib;

		//printf("CRUNCH IA=%d IB=%d\n", ia,ib);
		//srl_log_printf(NONE, "CRUNCH IA=%d IB=%d", ia,ib);
	        a[0]=qseq_lengthes[ia];
        	for (i=1;i<=a[0];i++)
                	a[i]=char2AA(*(qseq_data[ia]+i-1));

 		//srl_log_printf(NONE, "crunch : %s\n", dbseq_names[ib]);
        	b[0]=dbseq_lengthes[ib];
        	for (i=1;i<=b[0];i++)
                	b[i]=char2AA(*(dbseq_data[ib]+i-1));

        	Max=max=0;

        	for (i=0;i<=a[0];i++) h[i][0]=-i;
        	for (j=0;j<=b[0];j++) h[0][j]=-j;

        	for (i=1;i<=a[0];i++)
                	for (j=1;j<=b[0];j++) {
                        	diag    = h[i-1][j-1] + sim[a[i]][b[j]];
                        	up    = h[i-1][j] + DELTA;
                        	left   = h[i][j-1] + DELTA;

                        	max=MAX3(diag,up,left);
                        	if (max <= 0)  {
                                	h[i][j]=0;
                                }
                        	else if (max == diag) {
                                	h[i][j]=diag;
                                }
                        	else if (max == up) {
                                	h[i][j]=up;
                                }
                        	else  {
                                	h[i][j]=left;
                                }
                        	if (max >= Max){
                                	Max=max;
                                }
                        } 

  		out_desc.ia=in_desc.ia;
  		out_desc.ib=in_desc.ib;
  		out_desc.length=Max;

  		srl_mwmr_write(output, &out_desc, sizeof(out_desc));
	}
}

//#include "soclib/timer.h"
#include "system.h"
#include "stdio.h"
#include "stdlib.h"

#include "../segmentation.h"


int main(void) {

        puts("hello from processor ");
        putchar(procnum()+'0');
        putchar('\n');
	int i, samples;
	//printf("Hello from processor %d\n", procnum());
        ///printf("start FIR application excecution\n");
	puts("start FIR application excecution");
	putchar('\n');

	for (i = 0; i <64; i++) { 
		samples = i*2;
		printf("samples = %d\n", samples);
	}
        ////printf("end of FIR testing\n");
	puts("end of FIR testing");
	putchar('\n');

        //int *p=(int*)0x0100A000;
        //*p = -1;

        /*   itoa(*p,s); */
        /*   uputs(s);   */

        while (1)
                ;
        /*   TRAP; */

        return 0;
}


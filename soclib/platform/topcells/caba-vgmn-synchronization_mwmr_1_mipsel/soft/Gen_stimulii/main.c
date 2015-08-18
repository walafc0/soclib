#include <stdio.h>
#include <stdlib.h>
#define N 64

int main() 
{
	int i;
	printf("#define DATA_IN0 {");
	for (i = 0; i<N-1; i++)
	{
		printf("%d,\\\n",i);
	}
	printf("%d }", N-1);
	return 0;
}


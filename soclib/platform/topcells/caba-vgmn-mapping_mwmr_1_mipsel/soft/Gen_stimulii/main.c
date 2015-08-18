#include <stdio.h>
#include <stdlib.h>
#define N 64

int main() 
{
	int i;
	printf("#define DATA_IN0 {");
	for (i = 0; i<N-1; i++)
	{
		printf("%d,\\\n",0);
		i++;
		printf("%d,\\\n",1);
	}
	printf("%d }", 1);
        ///printf("%d", i);
	return 0;
}


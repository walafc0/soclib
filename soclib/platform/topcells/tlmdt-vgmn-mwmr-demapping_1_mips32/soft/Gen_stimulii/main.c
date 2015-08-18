#include <stdio.h>
#include <stdlib.h>
#define N 1023

int main() 
{
	int i;
	printf("#define DATA_IN {");
	for (i = 0; i<N-1; i++)
	{
		printf("%d,\\\n",-256);
		i++;
		printf("%d,\\\n",256);
	}
	printf("%d }", 256);
        ///printf("%d", i);
	return 0;
}


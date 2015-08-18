#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include "crc32.h"

int main(int argc, char **argv)
{
	uint32_t data[128];
	uint32_t crc = 0;
	int fd = open("../test.bin", O_RDONLY);
	printf("static const uint32_t expected_crc[] = {");
	while ( read(fd, data, 512) == 512 ) {
		crc = do_crc32(crc, data, 512);
		printf("0x%x,\n", crc);
	}
	printf("};");
	return 0;
}

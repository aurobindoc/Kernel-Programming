#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BLKSIZE 512
int main(void)
{
	int fDesc;
	int c, len=0;
	char readFile[]= "a.txt";
	char *buffer = (char *)memalign(BLKSIZE, BLKSIZE);
	
    fDesc = open("a.txt", O_DIRECT);
	if (fDesc < 0) {
		printf("Cannot open %s.\n", readFile);
		return 0;
	}

	if(read(fDesc,buffer,BLKSIZE) < 0)	{
		printf("Cannot read %s.\n", readFile);
		return 0;
	}
	puts(buffer);
	close(fDesc);
	
	
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

int main()
{
	getchar();
	printf("I'm waiting for you!!!!\n");
	char *getarea = (char *)malloc(80000);
	int i;
	for(i=0; i<10; i++) {
		*getarea = 'a';
		getarea = getarea + 4096;
		printf("Address = %lx\n",getarea);
	}
	getchar();
	return 0;
}

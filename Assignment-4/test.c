#include <stdio.h>
#include <sys/syscall.h>
#include <linux/kernel.h>

int main()	{
	getchar();
	long pid = syscall(325);
	printf("ashgdjasgd %ld", pid);
	return 0;
}
 

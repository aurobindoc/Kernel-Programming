#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE *file;
	int c, len=0;
	char readFile[]= "a.txt";
	while(1)	{
	   if ((file = fopen(readFile, "r")) == NULL){
			printf("Cannot open %s.\n", readFile);
			return 0;
		}
		while ((c=fgetc(file)) != EOF)	{
			printf("%c\n",c);
			len++;
			system("sudo sh -c 'echo 1 >/proc/sys/vm/drop_caches'");
		}
	}
	fclose(file);
	return len;
}

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

int main()
{
    int fd;
    char *address = NULL;
    
    fd = open("/dev/myCharDev", O_RDWR);
    if(fd < 0) {
        perror("open");
        return -1;
    }

    address = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (address == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    printf("%s\n", address);
    memcpy(address + 16 , "Hi, I am the User", 17);
    printf("%s\n", address);

    close(fd);
    return 0;
}

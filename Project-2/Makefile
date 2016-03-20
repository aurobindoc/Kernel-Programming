obj-m += part2-charDevDrvr.o

all: part2-testerProgram
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

part2-testerProgram: part2-testerProgram.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm part2-testerProgram

obj-m += pfs.o
obj-m += procfs.o
obj-m += procfsMod.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)  clean

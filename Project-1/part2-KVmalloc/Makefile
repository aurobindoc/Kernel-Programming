obj-m += kmalloc.o
obj-m += vmalloc.o
obj-m += check.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

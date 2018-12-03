obj-m += keystats.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
instil:
	sudo insmod keystats.ko
	dmesg | tail -1
remove:
	sudo rmmod keystats.ko
	dmesg | tail -1

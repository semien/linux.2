obj-m += phone_book.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
instil:
	sudo insmod phone_book.ko
	dmesg | tail -1
remove:
	sudo rmmod phone_book.ko
	dmesg | tail -1

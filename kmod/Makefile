MODULENAME=netagg

obj-m += $(MODULENAME).o

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	insmod $(MODULENAME).ko
	chmod 666 /dev/$(MODULENAME)

uninstall:
	rmmod $(MODULENAME).ko

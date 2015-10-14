MODULENAME=netagg
USER=$(shell whoami)

obj-m += $(MODULENAME).o

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install:
ifeq ($(USER), root)
	@insmod $(MODULENAME).ko
	@sleep 1
	@chmod 666 /dev/$(MODULENAME)
else
	@echo "FAILED, root permission needed!"
endif
uninstall:
ifeq ($(USER), root)
	@rmmod $(MODULENAME).ko
else
	@echo "FAILED, root permission needed!"
endif

#ifeq ($(KERNELRELEASE),)
obj-m := pio_interrupt.o
#KERNELDIR ?= /your/target/source/directory/
KERNELDIR ?= /C5S/linux_socfpga/linux-socfpga_3.13/linux-socfpga
PWD := $(shell pwd)
ARM ?=arm
CROSSS_COMPILE ?=arm-none-linux-gnueabi-

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

.PHONY: modules modules_install clean

#else
    
#endif


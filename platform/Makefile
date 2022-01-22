KERN_DIR := /home/jcc/work/linux-xlnx
PROJECT_NAME := led_platform
obj-m := $(PROJECT_NAME).o

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=`pwd` modules
	sudo cp $(PROJECT_NAME).ko /tftpboot/nfsroot/lib/modules/4.19.0-xilinx/
clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=`pwd` clean
	sudo rm -f /tftpboot/nfsroot/lib/modules/4.19.0-xilinx/$(PROJECT_NAME).ko
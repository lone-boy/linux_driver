KERN_DIR := /home/jcc/work/linux-xlnx

obj-m := timer.o

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=`pwd` modules
	#cp leddriver.ko /home/jcc/nfs/lib/modules/my-drivers/
clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=`pwd` clean
	#rm -f /home/jcc/nfs/lib/modules/my-drivers/leddriver.ko
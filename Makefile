#obj-m += testmod.o
#testmodule-objs := ramdisk.o bitmap.o block.o inode.o fdtable.o string_utils.o superblock.o
#testmod-objs := testmodule.o string_utils.o

obj-m += ramdisk_module.o
ramdisk_module-objs := ramdisk_kernel_lib.o ramdisk.o bitmap.o block.o inode.o fdtable.o string_utils.o superblock.o

GCC = g++
CXXFLAGS = -g -DDEBUG -DMODULE -D__KERNEL__
KDIR = /usr/src/kernels/kernel-2.6.18/linux-2.6.18.i686/
PWD = $(shell pwd)

#all: BitmapTest FdtableTest RamdiskTest SuperblockTest kernel

#kernel: 
all: ramdisk_test multi_process_test
	make -C $(KDIR) SUBDIRS=$(PWD) modules	

test_init: test_init.c ramdisk_lib.o
	$(GCC) -o test_init test_init.c ramdisk_lib.o

test_create: test_create.c ramdisk_lib.o
	$(GCC) -o test_create test_create.c ramdisk_lib.o

ramdisk_test: ramdisk_test.c ramdisk_lib.o
	$(GCC) -g -o ramdisk_test ramdisk_test.c ramdisk_lib.o

multi_process_test: multi_process_test.c ramdisk_lib.o
	$(GCC) -g -o multi_process_test multi_process_test.c ramdisk_lib.o

ramdisk_lib.o: ramdisk_lib.c ramdisk_lib.h ramdisk_kernel_lib.h
	$(GCC) -c -g ramdisk_lib.c


#RamdiskTest: ramdisk_test.c ramdisk.o bitmap.o block.o inode.o fdtable.o string_utils.o superblock.o
#	$(GCC) -o RamdiskTest $(CXXFLAGS) ramdisk_test.c ramdisk.o bitmap.o block.o inode.o fdtable.o string_utils.o superblock.o -lpthread


#SuperblockTest: superblock_test.c superblock.o
#	gcc -o fdtableTest $(CXXFLAGS) superblock_test.c superblock.o

#FdtableTest: fdtable_test.c fdtable.o
#	gcc -o fdtableTest $(CXXFLAGS) fdtable_test.c fdtable.o	

#InodeTest: inode_test.c inode.o
#	gcc -o InodeTest $(CXXFLAGS) inode_test.c inode.o

#BitmapTest: bitmap_test.c bitmap.o
#	$(GCC) -o BitmapTest $(CXXFLAGS) bitmap_test.c bitmap.o


#ramdisk.o: ramdisk.h ramdisk.c bitmap.h block.h inode.h string_utils.h
#	gcc -c $(CXXFLAGS) ramdisk.c

#superblock.o: superblock.h superblock.c
#	gcc -c $(CXXFLAGS) superblock.c

#fdtable.o: fdtable.h fdtable.c
#	gcc -c $(CXXFLAGS) fdtable.c

#inode.o: inode.h inode.c block.h ramdisk.h string_utils.h
#	gcc -c $(CXXFLAGS) inode.c

#bitmap.o: bitmap.h bitmap.c
#	gcc -c $(CXXFLAGS) bitmap.c

#block.o: block.h block.c string_utils.h
#	gcc -c $(CXXFLAGS) block.c

#string_utils.o: string_utils.h string_utils.c
#	gcc -c $(CXXFLAGS) string_utils.c
	
#ramdisk_lib.o: ramdisk_lib.h ramdisk_lib.c
#	gcc -c $(CXXFLAGS) ramdisk_lib.c


clean:
#	rm *.o BitmapTest RamdiskTest FdtableTest InodeTest SuperblockTest kernel
	rm ramdisk_test
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

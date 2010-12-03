
GCC = gcc
CXXFLAGS = -g -DDEBUG

all: bitmapTest ramdiskTest

ramdiskTest: ramdisk_test.c ramdisk.o bitmap.o block.o inode.o string_utils.o
	$(GCC) -o ramdiskTest $(CXXFLAGS) ramdisk_test.c ramdisk.o bitmap.o block.o inode.o string_utils.o 

inodeTest: inode_test.c inode.o
	$(GCC) -o inodeTest $(CXXFLAGS) inode_test.c inode.o

bitmapTest: bitmap_test.c bitmap.o
	$(GCC) -o bitmapTest $(CXXFLAGS) bitmap_test.c bitmap.o


ramdisk.o: ramdisk.h ramdisk.c bitmap.h block.h inode.h string_utils.h
	$(GCC) -c $(CXXFLAGS) ramdisk.c

inode.o: inode.h inode.c block.h ramdisk.h string_utils.h
	$(GCC) -c $(CXXFLAGS) inode.c

bitmap.o: bitmap.h bitmap.c
	$(GCC) -c $(CXXFLAGS) bitmap.c

block.o: block.h block.c string_utils.h
	$(GCC) -c $(CXXFLAGS) block.c

string_utils.o: string_utils.h string_utils.c
	$(GCC) -c $(CXXFLAGS) string_utils.c

clean:
	rm *.o bitmapTest ramdiskTest


GCC = gcc
CXXFLAGS = -g -DDEBUG

all: BitmapTest FdtableTest RamdiskTest SuperblockTest

RamdiskTest: ramdisk_test.c ramdisk.o bitmap.o block.o inode.o fdtable.o string_utils.o superblock.o
	$(GCC) -o RamdiskTest $(CXXFLAGS) ramdisk_test.c ramdisk.o bitmap.o block.o inode.o fdtable.o string_utils.o superblock.o


SuperblockTest: superblock_test.c Superblock.o
	gcc -o FdtableTest $(CXXFLAGS) superblock_test.c superblock.o

FdtableTest: fdtable_test.c Fdtable.o
	gcc -o FdtableTest $(CXXFLAGS) fdtable_test.c fdtable.o	

InodeTest: inode_test.c inode.o
	gcc -o InodeTest $(CXXFLAGS) inode_test.c inode.o

BitmapTest: bitmap_test.c bitmap.o
	$(GCC) -o BitmapTest $(CXXFLAGS) bitmap_test.c bitmap.o


ramdisk.o: ramdisk.h ramdisk.c bitmap.h block.h inode.h string_utils.h
	$(GCC) -c $(CXXFLAGS) ramdisk.c


Superblock.o: superblock.h superblock.c
	gcc -c $(CXXFLAGS) superblock.c

Fdtable.o: fdtable.h fdtable.c
	gcc -c $(CXXFLAGS) fdtable.c

inode.o: inode.h inode.c block.h ramdisk.h string_utils.h
	$(GCC) -c $(CXXFLAGS) inode.c

bitmap.o: bitmap.h bitmap.c
	$(GCC) -c $(CXXFLAGS) bitmap.c

block.o: block.h block.c string_utils.h
	$(GCC) -c $(CXXFLAGS) block.c

string_utils.o: string_utils.h string_utils.c
	$(GCC) -c $(CXXFLAGS) string_utils.c

clean:
	rm *.o BitmapTest RamdiskTest FdtableTest InodeTest SuperblockTest

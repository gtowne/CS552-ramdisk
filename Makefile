CXXFLAGS = -g -DDEBUG

all: BitmapTest RamdiskTest

RamdiskTest: ramdisk_test.c Ramdisk.o Bitmap.o Block.o string_utils.o
	gcc -o RamdiskTest $(CXXFLAGS) ramdisk_test.c Ramdisk.o Bitmap.o Block.o string_utils.o

InodeTest: inode_test.c Inode.o
	gcc -o InodeTest $(CXXFLAGS) inode_test.c Inode.o

BitmapTest: bitmap_test.c Bitmap.o
	gcc -o BitmapTest $(CXXFLAGS) bitmap_test.c Bitmap.o


Ramdisk.o: Ramdisk.h Ramdisk.c Bitmap.o Block.o string_utils.o
	gcc -c $(CXXFLAGS) Ramdisk.c

Inode.o: Inode.h Inode.c
	gcc -c $(CXXFLAGS) Inode.c

Bitmap.o: Bitmap.h Bitmap.c
	gcc -c $(CXXFLAGS) Bitmap.c

Block.o: Block.h Block.c string_utils.o
	gcc -c $(CXXFLAGS) Block.c

string_utils.o: string_utils.h string_utils.c
	gcc -c $(CXXFLAGS) string_utils.c

clean:
	rm *.o BitmapTest RamdiskTest

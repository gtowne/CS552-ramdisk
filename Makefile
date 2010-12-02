CXXFLAGS = -g -DDEBUG

all: BitmapTest 

InodeTest: inode_test.c Inode.o
	gcc -o InodeTest $(CXXFLAGS) inode_test.c Inode.o

BitmapTest: bitmap_test.c Bitmap.o
	gcc -o BitmapTest $(CXXFLAGS) bitmap_test.c Bitmap.o

Inode.o: Inode.h Inode.c
	gcc -c $(CXXFLAGS) Inode.c

Bitmap.o: Bitmap.h Bitmap.c
	gcc -c $(CXXFLAGS) Bitmap.c

clean:
	rm *.o BitmapTest InodeTest

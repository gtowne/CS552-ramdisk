CXXFLAGS = -g

all: BitmapTest

BitmapTest: Bitmap.c Bitmap.o
	gcc -o BitmapTest $(CXXFLAGS) Bitmap.c

Bitmap.o: Bitmap.h Bitmap.c
	gcc -c $(CXXFLAGS) Bitmap.c

clean:
	rm *.o BitmapTest

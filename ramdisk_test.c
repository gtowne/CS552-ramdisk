#include "ramdisk.h"

struct Ramdisk ramdisk;

int main()
{
    _ramdisk_initialize();
    
	int foo;
    foo = _ramdisk_parsepath("hello");
	foo = _ramdisk_parsepath("/");
    foo = _ramdisk_parsepath("/foo1/foo2/foo3");
    foo = _ramdisk_parsepath("/foo1/foo2/foo3/somedir");
    foo = _ramdisk_parsepath("/foo1/foo2/foo3/somedir/");
    foo = _ramdisk_parsepath("/foo1/foo2/foo3/somefile.d");

    
    //TODO: insert all other tests here when we implement all the functions
	
	return 0;
}

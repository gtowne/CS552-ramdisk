#include <stdio.h>

#include "ramdisk.h"

struct Ramdisk ramdisk;
struct FdtableArray fdtablea;

int main()
{
    char name[14] = "/aaaa";

    _ramdisk_initialize(&ramdisk);
    
    int i;
    for(i=0; i<17; i++)
      {
	int q = rd_creat(name);
	printf("%d %d %s\n", i, q, name); 
	name[1]++;
      }

    for(i=16; i>=0; i--)
    {  
      name[1]--;
      int q = rd_open(name);
      printf("%d %d %s\n", i, q, name); 
    }

    int z = rd_mkdir("/usr");
    int y = rd_mkdir("/usr/local");
    int x = rd_creat("/usr/local/bin");
    int p = rd_mkdir("/usr/blah/bin");
    printf("%d %d %d %d\n", z, y, x, p);

    /*    
	int foo;
    foo = _ramdisk_parsepath("hello");
	foo = _ramdisk_parsepath("/");
    foo = _ramdisk_parsepath("/foo1/foo2/foo3");
    foo = _ramdisk_parsepath("/foo1/foo2/foo3/somedir");
    foo = _ramdisk_parsepath("/foo1/foo2/foo3/somedir/");
    foo = _ramdisk_parsepath("/foo1/foo2/foo3/somefile.d");
    */
    
    //TODO: insert all other tests here when we implement all the functions
	
	return 0;
}

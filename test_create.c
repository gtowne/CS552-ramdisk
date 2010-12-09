#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef USE_PTHREADS
#include "ramdisk.h"
struct Ramdisk* RAMDISK;
struct FdtableArray fdtablea;
#else
#include "ramdisk_lib.h"
#endif


int ls(char* path);


int main()
{
    int retval, fd;

#ifdef USE_PTHREADS
    RAMDISK = malloc(sizeof(struct Ramdisk));
    _ramdisk_initialize(RAMDISK);
#else
    rd_init();
#endif

    //create a directory that requires more than one block
    //(use all of the direct pointers)
    int i;
    int numdirectories=128;
    char name[100] = "/aaaa";
    for(i=0; i<numdirectories; i++)
    {
        sprintf(name, "/a%d", i);
        int q = rd_creat(name);
    }
    //test readdir
    printf("Created enough directories to fill all the direct pointers in the root directory. Should 128 entries listed\n");
    ls("/");
}

int ls(char* path)
{
    printf("This is LS\n");

    char buffer[16];
    int fd = rd_open(path);
    if(fd < 0)
    {
        return -1;
    }

    while(rd_readdir(fd, buffer) > 0)
    {
        printf("%s/%s\n", path, buffer);
    }
    fd = rd_close(fd);
    return 0;
}

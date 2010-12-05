/*
 _____                     ______            _              
|_   _|                    | ___ \          | |             
  | | ___  __ _ _ __ ___   | |_/ /__ _ _ __ | |_  ___  _ __ 
  | |/ _ \/ _` | '_ ` _ \  |    // _` | '_ \| __|/ _ \| '__|
  | |  __/ (_| | | | | | | | |\ \ (_| | |_) | |_| (_) | |   
  \_/\___|\__,_|_| |_| |_| \_| \_\__,_| .__/ \__|\___/|_|   
                                      | |                   
                                      |_|                              

Luis Carrasco, Diane Theriault, Gordon Towne
Ramdisk - Project 3 - CS552

BITMAP:
This implements the bitmap part of the assignment

*/

#ifndef RAMDISK_H_
#define RAMDISK_H_

#include "superblock.h"
#include "inode.h"
#include "bitmap.h"
#include "block.h"

#include "fdtable.h"

#define MAX_FILENAME_LENGTH 512

struct Ramdisk //Ramdisk struct
{
    struct Superblock   superblock;
    //struct Block      superblock;
    struct IndexNode  inodes[INODES];
    struct Bitmap     bitmap;
    struct Block      blocks[FS_BLOCKS];
};


// Interface Functions

int rd_creat    (char *pathname);
int rd_mkdir    (char *pathname);
int rd_open     (char *pathname);
int rd_close    (int fd);
int rd_read     (int fd, char* address, int num_bytes);
int rd_write    (int fd, char* address, int num_bytes);
int rd_seek     (int fd, int offset);
int rd_unlink   (char *pathname);
int rd_readdir  (int fd, char *address);

// Other Functions

//int _ramdisk_parsepath  (char *pathname); //Parses the pathname to an inode index (parent inode of the requested file)


struct Block* _ramdisk_allocate_block(struct Ramdisk* iRamDisk);
int _ramdisk_deallocate_block(struct Ramdisk* iRamDisk, struct Block* iBlock);

#endif

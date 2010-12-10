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

#ifndef USE_PTHREADS
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>
#endif

#define MAX_FILENAME_LENGTH 512

extern struct FdtableArray fdtablea;
extern struct Ramdisk* RAMDISK;

struct Ramdisk //Ramdisk struct
{
    struct Superblock       superblock;
    //struct Block      superblock;
    struct IndexNode  inodes[INODES];
    struct Bitmap     bitmap;
    struct Block      blocks[FS_BLOCKS];
};


// Interface Functions

int rd_init     (void);
int rd_creat    (char *pathname);
int rd_mkdir    (char *pathname);
int rd_open     (char *pathname);
int rd_close    (int fd);
int rd_read     (int fd, char* address, int num_bytes);
int rd_write    (int fd, char* address, int num_bytes);
int rd_seek     (int fd, int offset);
int rd_unlink   (char *pathname);
int rd_readdir  (int fd, char *address);
int rd_test     (int* free_inodes, int* free_blocks);
// Other Functions

int block_fill(struct Block* block);

int _ramdisk_initialize(struct Ramdisk* ramdisk);
struct Block* _ramdisk_allocate_block(struct Ramdisk* iRamDisk);
int _ramdisk_deallocate_block(struct Ramdisk* iRamDisk, struct Block* iBlock);


int _ramdisk_allocate_inode(struct Ramdisk* ramdisk, enum NodeType type);
int _ramdisk_deallocate_inode(struct Ramdisk* ramdisk, int index);

int _ramdisk_walk_path(struct Ramdisk* ramdisk, char* name);
int _ramdisk_get_parent(struct Ramdisk* ramdisk, char* name, char token[14]);

int _ramdisk_find_directory_entry(struct Ramdisk* ramdisk,
                                  struct IndexNode* parent, char* name,
                                  struct Block** oBlock, int* offset);
int _ramdisk_add_directory_entry(struct Ramdisk* ramdisk, struct IndexNode* parent,
                                 char* name, enum NodeType type);
#endif

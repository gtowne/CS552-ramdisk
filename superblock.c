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

SUPERBLOCK:
This implements the superblock part of the assignment

*/
#ifdef USE_PTHREADS
#include <stdio.h>
#include <stdlib.h>
#endif

#include "superblock.h"

int superblock_initialize(struct Superblock *superblock)
{
    superblock->free_blocks = TOTAL_BLOCKS;
    superblock->free_inodes = INODES;
#ifdef USE_PTHREADS
    pthread_mutex_init(&superblock->mutex, NULL);
#else
    //sema_init(&superblock->mutex, 1);
#endif
    return 0;
}

int superblock_getfreeblocks(struct Superblock *superblock)
{
    return (int)superblock->free_blocks;
}

int superblock_getfreeinodes(struct Superblock *superblock)
{
    return (int)superblock->free_inodes;
}

int superblock_setfreeblocks(short num, struct Superblock *superblock)
{
    superblock->free_blocks = num;
    return 0;
}

int superblock_setfreeinodes(short num, struct Superblock *superblock)
{
    superblock->free_inodes = num;
    return 0;
}

int superblock_lock(struct Superblock *superblock)
{
#ifdef USE_PTHREADS
    //PRINT("LOCK\n");
    return pthread_mutex_lock(&superblock->mutex);
#else
    //down_interruptible(&superblock->mutex);
    return 1;
#endif
    return -1;
}

int superblock_unlock(struct Superblock *superblock)
{
#ifdef USE_PTHREADS
    //PRINT("UNLOCK\n");
    return pthread_mutex_unlock(&superblock->mutex);
#else
    //up(&superblock->mutex);
    return 1;
#endif
    return -1;
}

int _superblock_print(struct Superblock *superblock)
{
    PRINT("\nSUPERBLOCK");
#ifdef USE_PTHREADS
    PRINT(" - using pthreads\n");
#else
    PRINT(" - in the kernel\n");
#endif
    PRINT("-----------------\n");
    PRINT("free blocks: %d\n", superblock->free_blocks);
    PRINT("free inodes: %d\n", superblock->free_inodes);
    return 0;
}

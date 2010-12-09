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

#ifndef SUPERBLOCK_H_
#define SUPERBLOCK_H_

#include "defines.h"

#ifdef USE_PTHREADS
#include <pthread.h>
#else
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

struct Superblock //Bitmap struct
{
    short free_blocks;
    short free_inodes;
#ifdef USE_PTHREADS
    pthread_mutex_t mutex;
    unsigned char filler[BLOCK_BYTES - 2*sizeof(short) - sizeof(pthread_mutex_t)];
#else
    //struct semaphore mutex;
    unsigned char filler[BLOCK_BYTES - 2*sizeof(short)];
#endif
};

int superblock_initialize    (struct Superblock *superblock);            // Initializes superblock with the right values
int superblock_getfreeblocks (struct Superblock *superblock);            // Gets the number of free blocks in the ramdisk
int superblock_getfreeinodes (struct Superblock *superblock);            // Gets the number of free inodes in the ramdisk
int superblock_setfreeblocks (short num, struct Superblock *superblock); // Sets the number of free blocks in the ramdisk
int superblock_setfreeinodes (short num, struct Superblock *superblock); // Sets the number of free inodes in the ramdisk

int superblock_lock          (struct Superblock *superblock);            // Locks the ramdisk
int superblock_unlock        (struct Superblock *superblock);            // Unlocks the ramdisk

int _superblock_print        (struct Superblock *superblock);            // Prints the superblock

#endif

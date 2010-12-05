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

#include <stdio.h>
#include <stdlib.h>

#include "superblock.h"

int superblock_initialize(struct Superblock *superblock)
{
    superblock->free_blocks = TOTAL_BLOCKS;
    superblock->free_inodes = INODES;
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

int _superblock_print(struct Superblock *superblock)
{
    printf("\nSUPERBLOCK\n");
    printf("-----------------\n");
    printf("free blocks: %d\n", superblock->free_blocks);
    printf("free inodes: %d\n", superblock->free_inodes);
    return 0;
}

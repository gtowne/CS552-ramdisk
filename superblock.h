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

typedef struct Superblock //Bitmap struct
{
  short free_blocks;
  short free_inodes;  
}Superblock;

int superblock_initialize    (struct Superblock *superblock);            // Initializes superblock with the right values
int superblock_getfreeblocks (struct Superblock *superblock);            // Gets the number of free blocks in the ramdisk
int superblock_getfreeinodes (struct Superblock *superblock);            // Gets the number of free inodes in the ramdisk
int superblock_setfreeblocks (short num, struct Superblock *superblock); // Sets the number of free blocks in the ramdisk
int superblock_setfreeinodes (short num, struct Superblock *superblock); // Sets the number of free inodes in the ramdisk

int _superblock_print        (struct Superblock *superblock);            // Prints the superblock

#endif

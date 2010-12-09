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

#ifndef BITMAP_H_
#define BITMAP_H_

#include "defines.h"

#ifndef USE_PTHREADS
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

struct Bitmap //Bitmap struct
{
    int size;
    int num_empty;
    unsigned char array[BITMAP_NUM_BYTES];
};

int bitmap_initialize           (struct Bitmap* bitmap);                            //Initializes all bits to 0
int bitmap_get_one_block        (struct Bitmap* bitmap);                            //finds and marks one block


int bitmap_setatindex           (int index, struct Bitmap* bitmap);                 //Sets bit at an index in the bitmap
int bitmap_removeatindex        (int index, struct Bitmap* bitmap);                 //Unsets bit at an index in the bitmap
int bitmap_findemptyblockofsize (int size, struct Bitmap* bitmap);                  //Finds first empty block of given size in bitmap, return block index
int bitmap_setblockofsize       (int size, struct Bitmap* bitmap);                  //Sets the next size bites in the bitmap, return block index
int bitmap_removeblockofsize    (int index, int size, struct Bitmap* bitmap);       //Removes a block of size in the bitmap

int _bitmap_isset               (int block, int bit, struct Bitmap* bitmap);        //Checks if a bit inside a char is set
int _bitmap_isset_b             (int bitnum, struct Bitmap *bitmap);                //Ditto, but with bit num
int _bitmap_set                 (int block, int bit, struct Bitmap* bitmap);        //Sets a bit inside a char
int _bitmap_unset               (int block, int bit, struct Bitmap* bitmap);        //Unsets a bit inside a char
int _bitmap_setalltozero        (int block, struct Bitmap* bitmap);                 //Sets all bits in a char to zero

int _bitmap_printhchar          (char a);                                           //Prints bits in a char
int _bitmap_print_nice          (struct Bitmap* bitmap);                            //Prints bits in bitmap with block num
int _bitmap_print_raw           (struct Bitmap *bitmap);                            //Prints bits in bitmap all at once

#endif

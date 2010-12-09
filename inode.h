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

INODE
This implements the INode part of the assignment

*/

#ifndef INODE_H_
#define INODE_H_

//I think this isn't defined till you include one of the cstdlib headers

#include "defines.h"
#include "block.h"

#ifndef USE_PTHREADS
#include <linux/module.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>
#endif

struct Ramdisk;


//const int BLOCK_PTRS_PER_STORAGE_BLOCK=BLOCK_BYTES/sizeof(struct Block*); //should be 64.

#define IndexNodeDirectPointers 8
#define IndexNodeIndirectPointers 1
#define IndexNodeDoublePointers 1

//const int IndexNodeDirectPointers=8;
//const int IndexNodeIndirectPointers=1;
//const int IndexNodeDoublePointers=1;

enum NodeType { RAMDISK_DIR, RAMDISK_FILE, RAMDISK_UNALLOCATED };

struct IndexNode
{
    enum NodeType type; // @TODO: check that this is 4 bytes
    int size;
    struct Block* directPointer[IndexNodeDirectPointers];
    struct Block* indirectPointer[IndexNodeIndirectPointers];
    struct Block* doublePointer[IndexNodeDoublePointers];
    char freeBytes[16]; //padding to make the struct the right size
};

void r_inode_init(struct IndexNode* iNode);


/**
@param[in]  IndexNode iNode  the index node for the file
@param[in]  int address      the desired offset from the start of the file
@param[out] int oBlockOffset how many bytes into the block you should start
@return     Block*           the block you want to use for that offset
*/
struct Block* inode_get_block_for_byte_index(struct IndexNode* iNode,
        int address, int* oBlockOffset);

/**
@param[in]  IndexNode iNode  the index node for the file
@param[out] int oBlockOffset how many bytes into the block you should start
@return     Block*           the block you want to use for that offset
*/
struct Block* inode_get_last_block(struct IndexNode* iNode, int* oBlockOffset);

/**
@param[in]  IndexNode iNode    the index node for the file
@param[in]  Ramdisk iRamDisk   the Ramdisk structure itself,
                               (in case you need to allocate a new block
                               for an indirect node, etc.)
*/
struct Block* inode_add_block(struct IndexNode* iNode, struct Ramdisk* iRamDisk);


/**
   reduce the iNode's size by "reduce" bytes. (deallocate blocks if necessary)
 */
int inode_reduce_size(struct IndexNode* iNode, struct Ramdisk* iRamDisk,
                      int reduce);

/**
   remove the LAST block
 */
int inode_remove_block(struct IndexNode* iNode, struct Ramdisk* iRamDisk);

/**
@param[in]  IndexNode iNode    the index node for the file
@param[in]  Ramdisk iRamDisk   the Ramdisk structure itself,
                               (to deallocate blocks)
*/
int inode_release(struct IndexNode* iNode,
                  struct Ramdisk* iRamDisk);

#endif

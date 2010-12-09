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

BLOCK
Definition for block data structure for regular or directory files

 */

#ifndef BLOCK_H
#define BLOCK_H
#include "defines.h"
#include "string_utils.h"

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

//const int DIRECTORY_ENTRIES_PER_BLOCK=BLOCK_BYTES/sizeof(DirectoryEntry);
#define DIRECTORY_ENTRIES_PER_BLOCK 16



//#define ENTRIES_PER_DIRECTORY BLOCK_BYTES/DIR_ENTRY_BYTES
//#define BLOCK_PTR_BYTES 4

//const int BLOCK_PTRS_PER_STORAGE_BLOCK=BLOCK_BYTES/sizeof(Block*);
#define BLOCK_PTRS_PER_STORAGE_BLOCK 64

struct Block
{
    char memory [BLOCK_BYTES];
};

struct DirectoryEntry
{
    char name [DIR_ENTRY_NAME_BYTES];
    unsigned short int inode_index;
};

struct IndirectStorageBlock
{
    struct Block *children[BLOCK_PTRS_PER_STORAGE_BLOCK];
};

struct DirectoryBlock
{
    struct DirectoryEntry entries[DIRECTORY_ENTRIES_PER_BLOCK];
};

// FUNCTIONS TO MANIPULATE ARBITRARY BLOCKS
int free_block(struct Block *block);

// FUNCTIONS TO MANIPULATE REGULAR BLOCKS
int allocate_regular_block(struct Block *block);

// FUNCTIONS TO MANIPULATE DIRECTORY BLOCKS

int allocate_directory_block(struct Block *block);

// add a directory entry with specified name and inode index to this directory block
// returns 1 if successful
// returns -1 if this block is full
int add_directory_entry(struct Block *block, char* name, unsigned short inode_index);

// remove the directory entry with specified name in this directory block
// return 1 if successful
// return -1 if there is no such entry in this block
int remove_directory_entry(struct Block *block, char* name);

// takes a pointer to a directory block, and the name of an entry in
// that directory, and a pointer to a short integer that is populated with
// the index into the iNode array of the entry with that name
//
// returns 1 if the entry is in this block
// returns -1 if there is no entry in this block with that name
int get_directory_inode_index(struct Block *block, char* name, unsigned short int *index);

// FUNCTIONS TO MANIPULATE INDIRECT STORAGE BLOCKS

int allocate_indirect_storage_block(struct Block *block);

// add a new block to be pointed to by this indirect storage block
// returns index into the array of blocks stored here if successful
// returns -1 if this block is full
int add_block_to_indirect_storage(struct Block *storage_block, struct Block* new_block);

// return the pointer to the block stored at the specified index
// of an indirect storage block
// return NULL if there is no block pointed to here
struct Block* block_at(struct Block *storage_block, int index);

int set_indirect_storage_block(struct Block *storage_block, int index,
                               struct Block* new_block);

//copy memory from the block to the destination
//start at offset. copy until numBytes or the end of the block.
//return the number of bytes written
int block_copy_out(struct Block* block, char* dst, int offset, int numBytes);
int block_copy_in(struct Block* block, char* src, int offset, int numBytes);


// INTERNAL FUNCTIONS

// overwrite a block with zeroes
int _null_out_block(struct Block *block);

// overwrite a directory entry with zeroes
int _null_out_directory_entry(struct DirectoryEntry *entry);

// returns 1 iff entry is free, -1 otherwise
int _directory_entry_is_free(struct DirectoryEntry *entry);

// returns the index of the first free entry in the directory block
// or -1 if the block is full
int _first_free_directory_entry(struct DirectoryBlock *block);

// returns the index of the first free entry in the indirect storage block
// or -1 if the block is full
int _first_free_block_ptr_loc(struct IndirectStorageBlock *block);

// return index in directory of entry with specified name
// return -1 if entry not in directory
int _index_of_directory_with_name(struct DirectoryBlock *block, char* name);

#endif

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
#define NULL 0x0


#define BLOCK_SIZE 256

//dummy struct so I can just write something
struct Block
{
  char memory[256];
};

struct RAMDisk
{
 int dummy;
};

const int BlockPointersPerBlock=BLOCK_SIZE/sizeof(Block*); //should be 64.

const int IndexNodeDirectPointers=8;
const int IndexNodeIndirectPointers=1;
const int IndexNodeDoublePointers=1;
const int DirectlyIndexedBytes=BLOCK_SIZE*IndexNodeDirectPointers;
const int IndirectlyIndexedBytes=BLOCK_SIZE*(BlockPointersPerBlock);
const int DoubleIndexedBytes=BLOCK_SIZE*(BlockPointersPerBlock*BlockPointersPerBlock);

struct IndexNode
{
  enum blockType{ DIR, FILE, UNALLOCATED } type; // @TODO: check that this is 4 bytes
  int size;
  struct Block* directPointer[IndexNodeDirectPointers];
  struct Block* indirectPointer[IndexNodeIndirectPointers];
  struct Block* doubleIndirectPointer[IndexNodeDoublePointers];
  char freeBytes[16]; //padding to make the struct the right size
};

void inode_init(IndexNode* iNode);


/**
@param[in]  IndexNode iNode  the index node for the file
@param[in]  int address      the desired offset from the start of the file
@param[out] int oBlockOffset how many bytes into the block you should start
*/
Block* inode_get_block_for_byte_index(IndexNode* iNode, 
                                      int address, int* oBlockOffset=NULL);

/**
@param[in]  IndexNode iNode    the index node for the file
@param[in]  Block* iBlock      the block to add
@param[in]  RAMDisk iRamDisk   the Ramdisk structure itself, 
                               (in case you need to allocate a new block
                               for an indirect node, etc.)
*/
void inode_add_block_to_file(IndexNode* iNode, 
			     Block* iBlock, RAMDisk* iRamDisk);

//@TODO: NEED ALLOCATE BLOCK FUNCTION
Block* ramdisk_allocate_block(RAMDisk* iRamDisk);

#endif

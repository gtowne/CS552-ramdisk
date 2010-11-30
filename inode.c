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

INODE:
This implements the iNode part of the assignment

*/

#include "inode.h"

void inode_init(IndexNode* iNode)
{
  iNode->type = IndexNode::UNALLOCATED;
  iNode->size = 0;
  for(int i=0; i<IndexNodeDirectPointers; i++)
  {
    iNode->directPointer[i] = NULL;
  }
  for(int i=0; i<IndexNodeIndirectPointers; i++)
  {
    iNode->indirectPointer[i] = NULL;
  }
  for(int i=0; i<IndexNodeDoublePointers; i++)
  {
    iNode->doubleIndirectPointer[i] = NULL;
  }
}

/**
@param[in]  IndexNode iNode  the index node for the file
@param[in]  int address      the desired offset from the start of the file
@param[out] int oBlockOffset how many bytes into the block you should start
*/
Block* inode_get_block_for_byte_index(IndexNode* iNode, 
                                      int address, int* oBlockOffset)
{
  
}


/**
@param[in]  IndexNode iNode    the index node for the file
@param[in]  Block* iBlock      the block to add
@param[in]  RAMDisk iRamDisk   the Ramdisk structure itself, (in case you need to allocate a new block
                               for an indirect node, etc.)
*/
void inode_add_block_to_file(IndexNode* iNode, Block* iBlock, RAMDisk* iRamDisk)
{
  //find the next place to add a block.
  // if it's in the direct pointers, you are done
  // if it's in the indirect pointer
  //    if the pointer is null, allocate a block for the pointers
  //    if the block has space
  //      add the pointer to the block
  //    if there's no space, resort to the double pointers
  // if it's in the double pointer
  //    if the pointer is null, allocate a block for the indirect pointers
  //       also allocate a block of pointers for the first indirect pointer to point to
  //    walk through the tables for each indirect pointer, looking for space
  //    if you find a slot in a block, you are done
  //    if the table for the last allocated indirect pointer is full, 
  //       allocate a new table for the next indirect pointer
  //
  //Phew!
  //
}





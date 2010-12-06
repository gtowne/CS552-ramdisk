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
#include <stdio.h>

#include "inode.h"
#include "ramdisk.h"

const int DirectlyIndexedBytes=BLOCK_BYTES*IndexNodeDirectPointers;
const int IndirectlyIndexedBytes=BLOCK_BYTES*(BLOCK_PTRS_PER_STORAGE_BLOCK);
const int DoubleIndexedBytes=BLOCK_BYTES*(BLOCK_PTRS_PER_STORAGE_BLOCK*BLOCK_PTRS_PER_STORAGE_BLOCK);

void r_inode_init(struct IndexNode* iNode)
{
  int i;
  iNode->type = RAMDISK_UNALLOCATED;
  iNode->size = 0;
  for(i=0; i<IndexNodeDirectPointers; i++)
  {
    iNode->directPointer[i] = NULL;
  }
  for(i=0; i<IndexNodeIndirectPointers; i++)
  {
    iNode->indirectPointer[i] = NULL;
  }
  for(i=0; i<IndexNodeDoublePointers; i++)
  {
    iNode->doubleIndirectPointer[i] = NULL;
  }
}

/**
@param[in]  IndexNode iNode  the index node for the file
@param[in]  int address      the desired offset from the start of the file
@param[out] int oBlockOffset how many bytes into the block you should start
*/
struct Block* inode_get_block_for_byte_index(struct IndexNode* iNode, 
                                      int address, int* oBlockOffset)
{
  if(oBlockOffset != NULL)
  {
    *oBlockOffset = address % BLOCK_BYTES;
  }

  int blockIdx = address / BLOCK_BYTES;
  if(blockIdx < IndexNodeDirectPointers)
  {
    return iNode->directPointer[blockIdx];
  }

  //now, onto the single indirect pointers
  blockIdx -= IndexNodeDirectPointers;
  
  if(blockIdx < DIRECTORY_ENTRIES_PER_BLOCK * IndexNodeIndirectPointers)
  {
    int indirectIdx = blockIdx / DIRECTORY_ENTRIES_PER_BLOCK;
    blockIdx = blockIdx % DIRECTORY_ENTRIES_PER_BLOCK;
    //does the null check inside the function
    struct Block* block = block_at(iNode->indirectPointer[indirectIdx], 
				   blockIdx);
    return block;
  }

  //@TODO: double Indirect pointers

  return NULL;
}

struct Block* inode_get_last_block(struct IndexNode* iNode, int* oBlockOffset)
{
  return inode_get_block_for_byte_index(iNode, iNode->size, oBlockOffset);
}


/**
@param[in]  IndexNode iNode    the index node for the file
@param[in]  Block* iBlock      the block to add
@param[in]  Ramdisk iRamDisk   the Ramdisk structure itself, 
                               (in case you need to allocate a new block
                               for an indirect node, etc.)
@return     int                -1 if file is full, positive if successful
*/

struct Block* inode_add_block(struct IndexNode* iNode, struct Ramdisk* iRamDisk)
{
  struct Block* block = _ramdisk_allocate_block(iRamDisk);
  if(block == NULL)
  {
    return NULL;
  }

  //find the next place to add a block.
  int i;

  // if it's in the direct pointers, you are done
  for(i=0; i<IndexNodeDirectPointers; i++)
  {
    if(iNode->directPointer[i] == NULL)
    {
      iNode->directPointer[i] = block;
      return block;
    }    
  }

  //now, on to the single indirect pointers
  printf("Indirect pointers\n");
  //first, check to see if we need to add a table
  for(i=0; i<IndexNodeIndirectPointers; i++)
  {
    if(iNode->indirectPointer[i] == NULL)
    {
      iNode->indirectPointer[i] = block;
      block = _ramdisk_allocate_block(iRamDisk);
      if(block == NULL)
      {
	return NULL;
      }  
    }
  
    int retval = add_block_to_indirect_storage(iNode->indirectPointer[i], block);
    if(retval >= 0)
    {
      return block;
    }
  }

  return NULL;

/*
  //double d'oh. no space in the indirect pointer table, go to double pointers

  //first, you need to get the block of indirect pointers.
  //then, check each block in indirect pointers for space.
  
  
  
  for(i=0; i<IndexNodeDoublePointers; i++)
  {
    
    
    
    if(iNode->doubleIndirectPointer[i] == NULL)
    {
      pointerblock = ram_disk_allocate_block(iRamDisk);
      if(pointerblock == NULL)
      {
	//UNABLE TO ALLOCATE A NEW BLOCK
	return -1;
      }
    }

  //    if the pointer is null, allocate a block for the indirect pointers
  //       also allocate a block of pointers for the first indirect pointer to point to
  //    walk through the tables for each indirect pointer, looking for space
  //    if you find a slot in a block, you are done
  //    if the table for the last allocated indirect pointer is full, 
  //       allocate a new table for the next indirect pointer
  }

  //
  //Phew!
  //
  */
}


int inode_reduce_size(struct IndexNode* iNode, struct Ramdisk* iRamDisk, 
		      int reduce)
{
  if(iNode == NULL || iRamDisk == NULL || reduce < 0)
  {
    return -1;
  }
  if(reduce == 0)
  {
    return 0;
  }

  //find the current last block
  int dummy;
  struct Block* currentLastBlock = inode_get_last_block(iNode, &dummy);
  struct Block* hypothetical = 
    inode_get_block_for_byte_index(iNode, iNode->size-reduce-1, &dummy);

  if(currentLastBlock != hypothetical)
  {
    //remove the last block.
    inode_remove_block(iNode, iRamDisk);
  }

  iNode->size -= reduce;

  return 1;
}

int inode_remove_block(struct IndexNode* iNode, struct Ramdisk* iRamDisk)
{
  int blockIdx = iNode->size / BLOCK_BYTES;
  if(blockIdx < IndexNodeDirectPointers)
  {
    if(iNode->directPointer[blockIdx] != NULL)
    {
      _ramdisk_deallocate_block(iRamDisk, iNode->directPointer[blockIdx]);
      iNode->directPointer[blockIdx]=NULL;
      return 1;
    }
  }

  //now, on to single indirect pointers
  if(blockIdx < DIRECTORY_ENTRIES_PER_BLOCK * IndexNodeIndirectPointers)
  { 
    int indirectIdx = blockIdx / DIRECTORY_ENTRIES_PER_BLOCK;
    blockIdx = blockIdx % DIRECTORY_ENTRIES_PER_BLOCK;
    //does the null check inside the function
    struct Block* doomed = block_at(iNode->indirectPointer[indirectIdx], 
				    blockIdx);
    if(doomed != NULL)
    {
      _ramdisk_deallocate_block(iRamDisk, doomed);
      set_indirect_storage_block(iNode->indirectPointer[indirectIdx],
				 blockIdx, NULL);
    }
    if(blockIdx = 0) //this was the first block in the table
    {
      _ramdisk_deallocate_block(iRamDisk, iNode->indirectPointer[indirectIdx]);
      iNode->indirectPointer[indirectIdx]=NULL;
    }
  }
  
  
  
  //@TODO: Double Indirect pointers
  return -1;
}


/**
@param[in]  IndexNode iNode    the index node for the file
@param[in]  Ramdisk iRamDisk   the Ramdisk structure itself, 
                               (to deallocate blocks)
*/
int inode_release(struct IndexNode* iNode,
		   struct Ramdisk* iRamDisk)
{
  if(iNode == NULL || iRamDisk == NULL)
  {
    return -1;
  }

  int i;
  // if it's in the direct pointers, you are done
  for(i=0; i<IndexNodeDirectPointers; i++)
  {
    if(iNode->directPointer[i] != NULL)
    {
      _ramdisk_deallocate_block(iRamDisk, iNode->directPointer[i]);
    }
    else
    {
      return 1;
    }
  }

  for(i=0; i<IndexNodeIndirectPointers; i++)
  {
    if(iNode->indirectPointer[i] == NULL)
    {
      return 1;
    }
    int j;
    for(j=0; j<DIRECTORY_ENTRIES_PER_BLOCK; j++)
    {
      struct Block* block = block_at(iNode->indirectPointer[i], j);
      if(block != NULL)
      {
	_ramdisk_deallocate_block(iRamDisk, block);
	set_indirect_storage_block(iNode->indirectPointer[i], j, NULL);
      }
    }
    _ramdisk_deallocate_block(iRamDisk, iNode->indirectPointer[i]);
  }



  return 1;
}




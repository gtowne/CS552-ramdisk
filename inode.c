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
#include "ramdisk.h"

const int DirectlyIndexedBytes=BLOCK_BYTES*IndexNodeDirectPointers;
const int IndirectlyIndexedBytes=BLOCK_BYTES*(BLOCK_PTRS_PER_STORAGE_BLOCK);
const int DoubleIndexedBytes=BLOCK_BYTES*(BLOCK_PTRS_PER_STORAGE_BLOCK*BLOCK_PTRS_PER_STORAGE_BLOCK);

void inode_init(struct IndexNode* iNode)
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

  //@TODO: Indirect pointers


  return NULL;
}

struct Block* inode_get_last_block(struct IndexNode* iNode, int* oBlockOffset)
{
  return inode_get_block_for_byte_index(iNode, iNode->size, oBlockOffset);
}


/**
Helper function for inode_add_block_to_file

@param[in]  Block* iTable  the block containing all the pointers
@param[in]  Block* iBlock  the block pointer to insert
@return     int    the first free slot where the block pointer was inserted
                     -1 if the table pointer is NULL, 
                     BLOCK_PTRS_PER_STORAGE_BLOCK if the table is full.
 */
/*
int _pointerblock_insert_pointer(Block* iTable, Block* iBlock)
{
  int i;

  if(iTable == NULL)
  {
    return -1;
  }

  //interpret the block as a table of pointers
  Block** table = (Block**)(iTable);

  for(i=0; i<BLOCK_PTRS_PER_STORAGE_BLOCK; i++)
  {
    if(table[i] == NULL)
    {
      table[i] = iBlock;
      return i;
    }
  }
  return i; //will be  BLOCK_PTRS_PER_STORAGE_BLOCK
}
*/

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
  int slot; //for doing the indirect pointers
  struct Block* pointerblock;
  struct Block** table;

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

  return NULL;
/*
  //d'oh. no space in the direct pointer table, go to indirect pointers
  for(i=0; i<IndexNodeIndirectPointers; i++)
  {
    slot = _pointerblock_insert_pointer(iNode->indirectPointer[i], block);

    //check if the block is full
    if(slot == BLOCK_PTRS_PER_STORAGE_BLOCK)
    {
      continue;
    }

    //if the pointer is null, allocate a block for the pointers
    //after allocation, inserting pointer into the table
    if(slot == -1)
    {
      pointerblock = ram_disk_allocate_block(iRamDisk);
      if(pointerblock == NULL)
      {
	//UNABLE TO ALLOCATE A NEW BLOCK
	return -1;
      }
      iNode->indirectPointer[i] = pointerblock;
      _pointerblock_insert_pointer(iNode->indirectPointer[i], block);
    }
    return 1;
  }

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

/**
Helper function for inode_release

@param[in]  Block*  the block containing all the pointers
@return     int     the first free slot. (-1 if full)
 */
/*
void _pointerblock_clear_block(Block* iBlock, Ramdisk* iRamDisk)
{
  int i;

  //interpret the block as a table of pointers
  Block** table = (Block**)(iBlock);

  for(i=0; i<BLOCK_PTRS_PER_STORAGE_BLOCK; i++)
  {
    if(table[i] != NULL)
    {
      ramdisk_deallocate_block(&(table[i]));
      table[i] = NULL;
    }
  }
}
*/

/**
@param[in]  IndexNode iNode    the index node for the file
@param[in]  Ramdisk iRamDisk   the Ramdisk structure itself, 
                               (to deallocate blocks)
*/
void inode_release(struct IndexNode* iNode,
		   struct Ramdisk* iRamDisk)
{

}




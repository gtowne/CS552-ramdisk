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
#ifdef USE_PTHREADS
#include <stdio.h>
#endif

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
        iNode->doublePointer[i] = NULL;
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
    int blockIdx;
    int q;
    int indirectIdx;
    int entries_squared;
    int numDoublePointers;
    int doubleIdx;
    struct Block* block;
    struct Block* doubleIndirect;
    struct Block* indirect;

    if(oBlockOffset != NULL)
    {
        *oBlockOffset = address % BLOCK_BYTES;
    }

    blockIdx = address / BLOCK_BYTES;
    if(blockIdx == 24)
    {
        q=0;
        blockIdx = 24;
    }

    if(blockIdx < IndexNodeDirectPointers)
    {
        return iNode->directPointer[blockIdx];
    }

    //now, onto the single indirect pointers
    blockIdx -= IndexNodeDirectPointers;

    if(blockIdx < BLOCK_PTRS_PER_STORAGE_BLOCK * IndexNodeIndirectPointers)
    {
        indirectIdx = blockIdx / BLOCK_PTRS_PER_STORAGE_BLOCK;
        blockIdx = blockIdx % BLOCK_PTRS_PER_STORAGE_BLOCK;
        //does the null check inside the function
        block = block_at(iNode->indirectPointer[indirectIdx],
                         blockIdx);
        return block;
    }

    //double Indirect pointers
    entries_squared=BLOCK_PTRS_PER_STORAGE_BLOCK*BLOCK_PTRS_PER_STORAGE_BLOCK;
    blockIdx -= BLOCK_PTRS_PER_STORAGE_BLOCK * IndexNodeIndirectPointers;
    numDoublePointers = entries_squared * IndexNodeDoublePointers;
    if(blockIdx < numDoublePointers)
    {
        doubleIdx = blockIdx / (entries_squared);
        blockIdx -= doubleIdx * entries_squared;
        indirectIdx = blockIdx / BLOCK_PTRS_PER_STORAGE_BLOCK;
        blockIdx = blockIdx % BLOCK_PTRS_PER_STORAGE_BLOCK;

        doubleIndirect=iNode->doublePointer[doubleIdx];
        if(doubleIndirect == NULL)
        {
            return NULL;
        }
        indirect=block_at(doubleIndirect, indirectIdx);
        if(indirect == NULL)
        {
            return NULL;
        }
        block = block_at(indirect, blockIdx);
        return block;
    }

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
    int retval;
    int i;
    int blockIdx;
    int indirectIdx;
    int entries_squared;
    int numDoublePointers;
    int doubleIdx;
    struct Block* block;
    struct Block* indirect;

    block = _ramdisk_allocate_block(iRamDisk);
    if(block == NULL)
    {
        return NULL;
    }

    //find the next place to add a block.

    //let's assume that we only add blocks at the end of the file.
    //and let's assume that iNode->size is just after the last block
    //currently in the file

    //direct pointers
    blockIdx = iNode->size / BLOCK_BYTES;
    if(blockIdx < IndexNodeDirectPointers)
    {
        iNode->directPointer[blockIdx] = block;
        return block;
    }

    //now, onto the single indirect pointers
    blockIdx -= IndexNodeDirectPointers;

    if(blockIdx < BLOCK_PTRS_PER_STORAGE_BLOCK * IndexNodeIndirectPointers)
    {
        indirectIdx = blockIdx / BLOCK_PTRS_PER_STORAGE_BLOCK;
        blockIdx = blockIdx % BLOCK_PTRS_PER_STORAGE_BLOCK;

        if(iNode->indirectPointer[indirectIdx] == NULL)
        {
            if(blockIdx != 0)
            {
                PRINT("***ERROR indirect pointer is NULL, but blockIdx > 0\n");
            }

            iNode->indirectPointer[indirectIdx] = block;
            block = _ramdisk_allocate_block(iRamDisk);
            if(block == NULL)
            {
                return NULL;
            }
        }

        retval = add_block_to_indirect_storage(
                     iNode->indirectPointer[indirectIdx],
                     block);
        if(retval != blockIdx)
        {
            PRINT("***ERROR placing block in inode_add_block\n");
            return NULL;
        }
        return block;
    }

    //double indirect pointers
    entries_squared=BLOCK_PTRS_PER_STORAGE_BLOCK*BLOCK_PTRS_PER_STORAGE_BLOCK;
    blockIdx -= BLOCK_PTRS_PER_STORAGE_BLOCK * IndexNodeIndirectPointers;
    numDoublePointers = entries_squared * IndexNodeDoublePointers;
    if(blockIdx < numDoublePointers)
    {
        doubleIdx = blockIdx / (entries_squared);
        blockIdx -= doubleIdx * entries_squared;
        indirectIdx = blockIdx / BLOCK_PTRS_PER_STORAGE_BLOCK;
        blockIdx = blockIdx % BLOCK_PTRS_PER_STORAGE_BLOCK;

        //need to allocate a block for the double pointer
        if(iNode->doublePointer[doubleIdx] == NULL)
        {
            if(indirectIdx != 0 || blockIdx != 0)
            {
                //ERROR
            }
            iNode->doublePointer[doubleIdx] = block;
            block = _ramdisk_allocate_block(iRamDisk);
            if(block == NULL)
            {
                return NULL;
            }
        }

        indirect = block_at(iNode->doublePointer[doubleIdx],
                            indirectIdx);
        //need to allocate a block for the indirect pointer
        if(indirect == NULL)
        {
            retval = add_block_to_indirect_storage(iNode->doublePointer[doubleIdx],
                                                   block);
            indirect = block_at(iNode->doublePointer[doubleIdx],
                                indirectIdx);
            if(retval != indirectIdx || indirect == NULL)
            {
                //ERROR
            }
            block = _ramdisk_allocate_block(iRamDisk);
            if(block == NULL)
            {
                return NULL;
            }
        }

        //add the block to the table of indirect pointers
        retval = add_block_to_indirect_storage(indirect, block);
        if(retval != blockIdx)
        {
            //ERROR
        }
        return block;
    }

    /*
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
      PRINT("Indirect pointers\n");
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
    */
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
    int dummy;
    struct Block* currentLastBlock;
    struct Block* hypothetical;

    if(iNode == NULL || iRamDisk == NULL || reduce < 0)
    {
        return -1;
    }
    if(reduce == 0)
    {
        return 0;
    }

    //find the current last block
    currentLastBlock = inode_get_block_for_byte_index(iNode, iNode->size-1, &dummy);
    hypothetical =
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
    int blockIdx;
    int indirectIdx;
    int entries_squared;
    int numDoublePointers;
    int doubleIdx;
    struct Block* doomed;
    struct Block* doubleIndirect;
    struct Block* indirect;
    struct Block* block;

    //is this minus one the right thing to do?
    blockIdx = (iNode->size -1) / BLOCK_BYTES;
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
    blockIdx -= IndexNodeDirectPointers;
    if(blockIdx < BLOCK_PTRS_PER_STORAGE_BLOCK * IndexNodeIndirectPointers)
    {
        indirectIdx = blockIdx / BLOCK_PTRS_PER_STORAGE_BLOCK;
        blockIdx = blockIdx % BLOCK_PTRS_PER_STORAGE_BLOCK;
        //does the null check inside the function
        doomed = block_at(iNode->indirectPointer[indirectIdx],
                          blockIdx);
        if(doomed != NULL)
        {
            _ramdisk_deallocate_block(iRamDisk, doomed);
            set_indirect_storage_block(iNode->indirectPointer[indirectIdx],
                                       blockIdx, NULL);
        }
        if(blockIdx == 0) //this was the first block in the table
        {
            _ramdisk_deallocate_block(iRamDisk, iNode->indirectPointer[indirectIdx]);
            iNode->indirectPointer[indirectIdx]=NULL;
        }
        return 1;
    }


    //Double Indirect pointers
    entries_squared=BLOCK_PTRS_PER_STORAGE_BLOCK*BLOCK_PTRS_PER_STORAGE_BLOCK;
    blockIdx -= BLOCK_PTRS_PER_STORAGE_BLOCK * IndexNodeIndirectPointers;
    numDoublePointers = entries_squared * IndexNodeDoublePointers;
    if(blockIdx < numDoublePointers)
    {
        doubleIdx = blockIdx / (entries_squared);
        blockIdx -= doubleIdx * entries_squared;
        indirectIdx = blockIdx / BLOCK_PTRS_PER_STORAGE_BLOCK;
        blockIdx = blockIdx % BLOCK_PTRS_PER_STORAGE_BLOCK;

        doubleIndirect=iNode->doublePointer[doubleIdx];
        if(doubleIndirect == NULL)
        {
            return -1;
        }
        indirect=block_at(doubleIndirect, indirectIdx);
        if(indirect == NULL)
        {
            return -1;
        }
        block = block_at(indirect, blockIdx);
        _ramdisk_deallocate_block(iRamDisk, block);
        set_indirect_storage_block(indirect, blockIdx, NULL);

        if(blockIdx == 0)
        {
            _ramdisk_deallocate_block(iRamDisk, indirect);
            set_indirect_storage_block(doubleIndirect, indirectIdx, NULL);
        }

        if(indirectIdx == 0)
        {
            _ramdisk_deallocate_block(iRamDisk, doubleIndirect);
            iNode->doublePointer[doubleIdx]=NULL;
        }

        return 1;
    }

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
    int i;
    int j,k;
    struct Block* block;
    struct Block* doubleIndirect;
    struct Block* indirect;

    if(iNode == NULL || iRamDisk == NULL)
    {
        return -1;
    }

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

    //single indirect pointers
    for(i=0; i<IndexNodeIndirectPointers; i++)
    {
        if(iNode->indirectPointer[i] == NULL)
        {
            return 1;
        }

        for(j=0; j<BLOCK_PTRS_PER_STORAGE_BLOCK; j++)
        {
            block = block_at(iNode->indirectPointer[i], j);
            if(block == NULL)
            {
                break;
            }
            _ramdisk_deallocate_block(iRamDisk, block);
            set_indirect_storage_block(iNode->indirectPointer[i], j, NULL);
        }
        _ramdisk_deallocate_block(iRamDisk, iNode->indirectPointer[i]);
    }

    //double indirect pointers
    for(i=0; i<IndexNodeDoublePointers; i++)
    {
        if(iNode->doublePointer[i] == NULL)
        {
            return 1;
        }

        doubleIndirect=iNode->doublePointer[i];
        for(j=0; j<BLOCK_PTRS_PER_STORAGE_BLOCK; j++)
        {
            indirect=block_at(doubleIndirect, j);
            if(indirect == NULL)
            {
                break;
            }
            for(k=0; k<BLOCK_PTRS_PER_STORAGE_BLOCK; k++)
            {
                block = block_at(indirect, k);
                if(block == NULL)
                {
                    break;
                }
                _ramdisk_deallocate_block(iRamDisk, block);
                set_indirect_storage_block(indirect, k, NULL);
            }
            _ramdisk_deallocate_block(iRamDisk, indirect);
            set_indirect_storage_block(doubleIndirect, j, NULL);
        }
        _ramdisk_deallocate_block(iRamDisk, doubleIndirect);
        iNode->doublePointer[i]=NULL;
    }

    return 1;
}




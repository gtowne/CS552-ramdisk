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

#include "block.h"

int allocate_regular_block(struct Block *block)
{
    // overwrite block with zeroes
    _null_out_block(block);

    return 1;
}

int free_block(struct Block *block)
{
    // overwrite block with zeroes
    _null_out_block(block);

    return 1;
}

int allocate_directory_block(struct Block *block)
{
    // overwrite all directory fields with zeroes
    struct DirectoryBlock *dirBlock;
    int i;

    dirBlock = (struct DirectoryBlock *) block;

    for (i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; i++)
    {
        _null_out_directory_entry(&(dirBlock->entries[i]));
    }

    return 1;
}

int allocate_indirect_storage_block(struct Block *block)
{
    // overwrite block with zeroes
    _null_out_block(block);

    return 1;
}

int get_directory_inode_index(struct Block *block, char* name, unsigned short int *index)
{

    int i;
    struct DirectoryBlock *dirBlock;
    struct DirectoryEntry* entries;

    dirBlock = (struct DirectoryBlock *) block;
    entries = dirBlock->entries;

    // search through all directory entries

    for (i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; i++)
    {

        // if the name of this one matches the target name
        // set its index and return
        if (1 == str_equals((entries[i].name), name, DIR_ENTRY_NAME_BYTES))
        {
            *index = entries[i].inode_index;

            return i;
        }
    }

    // entry is not in this block
    return -1;
}

int add_directory_entry(struct Block *block, char* name, unsigned short inode)
{
    int firstFree;
    struct DirectoryBlock *dirBlock;
    struct DirectoryEntry *thisEntry;

    dirBlock = (struct DirectoryBlock *) block;

    firstFree = _first_free_directory_entry(dirBlock);

    if (firstFree == -1 || firstFree == DIRECTORY_ENTRIES_PER_BLOCK)
    {
        return firstFree;
    }

    thisEntry = &(dirBlock->entries[firstFree]);

    str_copy(name, thisEntry->name, DIR_ENTRY_NAME_BYTES);

    thisEntry->inode_index = inode;

    return firstFree;
}

int remove_directory_entry(struct Block *block, char* name)
{

    int entryIndex;
    struct DirectoryBlock *dirBlock;

    dirBlock = (struct DirectoryBlock *) block;

    entryIndex = _index_of_directory_with_name(dirBlock, name);

    if (entryIndex == -1)
    {
        return -1;
    }

    _null_out_directory_entry(&(dirBlock->entries[entryIndex]));

    return 1;
}

int add_block_to_indirect_storage(struct Block *storage_block, struct Block* new_block)
{

    int firstFree;
    struct IndirectStorageBlock *this_storage_block;

    this_storage_block = (struct IndirectStorageBlock *) storage_block;

    firstFree = _first_free_block_ptr_loc(this_storage_block);

    if (firstFree == -1 || firstFree == BLOCK_PTRS_PER_STORAGE_BLOCK)
    {
        return firstFree;
    }

    this_storage_block->children[firstFree] = new_block;

    return firstFree;
}

struct Block* block_at(struct Block *storage_block, int index)
{

    struct IndirectStorageBlock *this_storage_block;

    this_storage_block = (struct IndirectStorageBlock *) storage_block;
    if(storage_block == NULL)
    {
        return NULL;
    }
    if(index < 0 || index > BLOCK_PTRS_PER_STORAGE_BLOCK)
    {
        return NULL;
    }
    return this_storage_block->children[index];
}

int set_indirect_storage_block(struct Block *storage_block, int index,
                               struct Block* new_block)
{
    struct IndirectStorageBlock *this_storage_block;

    this_storage_block = (struct IndirectStorageBlock *) storage_block;
    if(storage_block == NULL)
    {
        return -1;
    }
    if(index < 0 || index > BLOCK_PTRS_PER_STORAGE_BLOCK)
    {
        return -1;
    }

    this_storage_block->children[index] = new_block;
    return 1;
}


int block_copy_out(struct Block* block, char* dst, int offset, int numBytes)
{
    int i;
    int j;

    if(block == NULL || dst == NULL || offset<0 || offset>=BLOCK_BYTES || numBytes <0)
    {
        return -1;
    }

    //offsets into block and into destination may be different
    j=0;
    for(i=offset; i<numBytes+offset; i++)
    {
        if(i == BLOCK_BYTES) break;
        dst[j]=block->memory[i];
        j++;
    }
    return i-offset;
}

int block_copy_in(struct Block* block, char* src, int offset, int numBytes)
{
    int i;
    int j;


    if(block == NULL || src == NULL || offset<0 || offset>=BLOCK_BYTES || numBytes <0)
    {
        return -1;
    }

    //offsets into block and into destination may be different
    j=0;
    for(i=offset; i<numBytes+offset; i++)
    {
        if(i == BLOCK_BYTES) break;
        block->memory[i]=src[j];
        j++;
    }
    return i-offset;
}



int _null_out_block(struct Block *block)
{
    int i;
    char* blockMemory;

    blockMemory = block->memory;

    for (i = 0; i < BLOCK_BYTES; i++)
    {
        blockMemory[i] = '\0';
    }

    return 1;
}


int _null_out_directory_entry(struct DirectoryEntry *entry)
{
    char* name;
    int i;

    entry->inode_index = 0;

    name = entry->name;

    for (i = 0; i < DIR_ENTRY_NAME_BYTES; i++)
    {
        name[i] = '\0';
    }

    return 1;
}

int _directory_entry_is_free(struct DirectoryEntry *entry)
{
    if (entry->inode_index == 0 && entry->name[0] == '\0')
    {
        return 1;
    }

    return -1;
}

int _first_free_directory_entry(struct DirectoryBlock *block)
{
    int i;
    //struct DirectoryEntry** entries = block->entries;
    if(block == NULL)
    {
        return -1;
    }


    for (i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; i++)
    {
        if (1 == _directory_entry_is_free(&block->entries[i]))
        {
            return i;
        }
    }

    return DIRECTORY_ENTRIES_PER_BLOCK;
}

int _index_of_directory_with_name(struct DirectoryBlock *block, char* name)
{
    int i;
    for (i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; i++)
    {
        if (str_equals(block->entries[i].name, name, DIR_ENTRY_NAME_BYTES))
        {
            return i;
        }
    }

    return -1;
}


int _first_free_block_ptr_loc(struct IndirectStorageBlock *block)
{
    int i;
    if(block == NULL)
    {
        return -1;
    }
    for (i = 0; i < BLOCK_PTRS_PER_STORAGE_BLOCK; i++)
    {
        if (block->children[i] == NULL)
        {
            return i;
        }
    }

    return i; //will be BLOCK_PTRS_PER_STORAGE_BLOCK
}

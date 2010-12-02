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

int allocate_regular_block(struct Block *block) {
	// overwrite block with zeroes
	_null_out_block(block);
	
	return 1;
}

int free_block(struct Block *block) {
	// overwrite block with zeroes
	_null_out_block(block);
	
	return 1;
}

int allocate_directory_block(struct Block *block) {
	// overwrite all directory fields with zeroes
	
	struct DirectoryBlock *dirBlock = (struct DirectoryBlock *) block;
	
	int i = 0;
	for (i = 0; i < ENTRIES_PER_DIRECTORY; i++) {
		_null_out_directory_entry(dirBlock->entries[i]);
	}
	
	return 1;
}

int allocate_indirect_storage_block(struct Block *block) {
	// overwrite block with zeroes
	_null_out_block(block);
	
	return 1;
}

int get_directory_inode_index(struct Block *block, char* name, unsigned short int *index) {
	struct DirectoryBlock *dirBlock = (struct DirectoryBlock *) block;
	struct DirectoryEntry* entries = dirBlock->entries;
	
	// search through all directory entries
	int i;
	for (i = 0; i < ENTRIES_PER_DIRECTORY; i++) {
		
		// if the name of this one matches the target name
		// set its index and return
		if (str_equals(entries[i], name, DIR_ENTRY_NAME_BYTES)) {
			*index = entries[i]->inode_index;
			
			return 1;
		}
	}
	
	// entry is not in this block
	return -1;
}

int add_directory_entry(struct Block *block, char* name, unsigned short inode) {
	struct DirectoryBlock *dirBlock = (struct DirectoryBlock *) block;
	
	int firstFree = _first_free_directory_entry(dirBlock);
	
	if (firstFree == -1) {
		return -1;
	}
	
	struct DirectoryEntry *thisEntry = dirBlock->entries[firstFree];
	
	str_copy(name, thisEntry->name, DIR_ENTRY_NAME_BYTES);
	
	thisEntry->inode_index = inode;
	
	return 1;
}

int remove_directory_entry(struct Block *block, char* name) {
	struct DirectoryBlock *dirBlock = (struct DirectoryBlock *) block;
	
	int entryIndex = _index_of_directory_with_name(*dirBlock, name);
	
	if (entryIndex == -1) {
		return -1;
	}
	
	_null_out_directory_entry(dirBlock, entryIndex);
	
	return 1;
}

int add_block_to_indirect_storage(struct Block *storage_block, struct Block* new_block) {
	struct IndirectStorageBlock *this_storage_block = (struct IndirectStorageBlock *) storage_block;
	
	int firstFree = _first_free_block_ptr_loc(this_storage_block);
	
	if (firstFree == -1) {
		return -1;
	}
	
	this_storage_block->children[firstFree] = new_block;
	
	return firstFree;
}

struct Block* block_at(struct Block *storage_block, int index) {
	struct IndirectStorageBlock *this_storage_block = (struct IndirectStorageBlock *) storage_block;
	
	return this_storage_block->children[i];
}


int _null_out_block(struct Block *block) {
	char* blockMemory = block->memory;
	
	int i;
	for (i = 0; i < BLOCK_BYTES; i++) {
		blockMemory[i] = NULL;
	}
	
	return 1;
}

int _null_out_directory_entry(struct DirectoryEntry *entry) {
	entry->inode_index = 0;
	
	char* name = entry->name;
	int i;
	for (i = 0; i < DIR_ENTRY_NAME_BYTES; i++) {
		name[i] = NULL;
	}
	
	return 1;
}

int _directory_entry_is_free(struct DirectoryEntry *entry) {
	if (entry->inode_index == 0 && entry->name[0] == NULL) {
		return 1;
	}
	
	return -1;
}

int _first_free_directory_entry(struct DirectoryBlock *block) {
	struct DirectoryEntry* entries = block->entries;
	
	int i;
	for (i = 0; i < ENTRIES_PER_DIRECTORY; i++) {
		if (_directory_entry_is_free(entries[i])) {
			return i;
		}
	}
	
	return -1;
}

int _index_of_directory_with_name(struct DirectoryBlock *block, char* name) {
	int i;
	for (i = 0; i < ENTRIES_PER_DIRECTORY; i++) {
		if (str_equals(block->entries[i]->name, name)) {
			return i;
		}
	}
			
	return -1;
}


int _first_free_block_ptr_loc(struct IndirectStorageBlock *block) {
	struct Block *children = block->children;
	
	int i;
	for (i = 0; i < BLOCK_PTRS_PER_STORAGE_BLOCK; i++) {
		if (children[i] == NULL) {
			return i;
		}
	}
	
	return -1;
}

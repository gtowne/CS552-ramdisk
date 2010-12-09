/*
 *  block_test.c
 *
 *
 */


#include <stdio.h>
#include "block.h"

int main()
{
    struct Block block;
    printf("Block size %u\n", sizeof(block));

    struct DirectoryEntry dirEntry;
    printf("DirEntry size %u\n", sizeof(dirEntry));

    struct IndirectStorageBlock ind;
    printf("Indirect Block size %u\n", sizeof(ind));

    struct IndirectStorageBlock dir;
    printf("Directory Block size %u\n", sizeof(dir));

    printf("Size of pointers (make sure we're compiling for 32 bit) %u\n", sizeof(&dir));

    allocate_directory_block(&block);
    add_directory_entry(&block, "hello", 5);
    add_directory_entry(&block, "world", 12);

    unsigned short int* index1;
    unsigned short int* index2;

    get_directory_inode_index(&block, "hello", index1);
    get_directory_inode_index(&block, "world", index2);
    printf("hello at inode %i\nworld at inode %i\n", *index1, *index2);

    remove_directory_entry(&block, "hello");
    add_directory_entry(&block, "blah", 11);

    get_directory_inode_index(&block, "blah", index1);

    printf("blah at inode %i\n", *index1);

}


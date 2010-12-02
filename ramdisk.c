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

RAMDISK:
This implements the ramdisk part of the assignment

*/

#include <stdio.h>
#include <stdlib.h>

#include "ramdisk.h"

int rd_creat(char *pathname)
{
    extern struct Ramdisk ramdisk;

    // Take care of the path
    int inodeofpath = _ramdisk_parsepath(pathname);
    if (inodeofpath < 0)
    {
        printf("ERROR: rd_creat: invalid pathname\n");
    }
    //TODO: check in inodeofpath to see if the file exists

    // Make sure we have empty space
    // Index is the empty block index
    int index = bitmap_setblockofsize(1, &(ramdisk.bitmap));
    if (index < 0)
    {
        printf("ERROR: rd_creat: ramdisk full\n");
        return -1;
    }
    
    //TODO: create entry in the block for the inodeofpath
    //TODO: create inode for file
    
    return 0;
}

int rd_mkdir(char *pathname)
{
    extern struct Ramdisk ramdisk;

    // Take care of the path
    int inodeofpath = _ramdisk_parsepath(pathname);
    if (inodeofpath < 0)
    {
        printf("ERROR: rd_mkdir: invalid pathname\n");
        return -1;
    }
    //TODO: check in inodeofpath to see if the file exists

    // Make sure we have empty space
    // Index is the empty block index
    int index = bitmap_setblockofsize(1, &(ramdisk.bitmap));
    if (index < 0)
    {
        printf("ERROR: rd_mkdir: ramdisk full\n");
        return -1;
    }
    
    //TODO: create entry in the block for the inodeofpath
    //TODO: create inode for file
    
    return 0;
}

int rd_open(char *pathname)
{
    extern struct Ramdisk ramdisk;

    // Take care of the path
    int inodeofpath = _ramdisk_parsepath(pathname);
    if (inodeofpath < 0)
    {
        printf("ERROR: rd_open: invalid pathname\n");
        return -1;
    }

    //TODO: Create file descriptor table for process
    //      create an entry with inodeofpath
    //      and return the index number
    
    return 0;
}

int rd_close(int fd)
{
    extern struct Ramdisk ramdisk;
    //TODO: Everything
    return 0;
}

int rd_read(int fd, char* address, int num_bytes)
{
    extern struct Ramdisk ramdisk;
    //TODO: Everything
    return 0;
}

int rd_write(int fd, char* address, int num_bytes)
{
    extern struct Ramdisk ramdisk;
    //TODO: Check if fd exists   

    //TODO: Open inode and get last block,
    //      Find out number of bytes left in that block
    //      figure out how many extra bytes we need
    //      figure out how many new blocks we need

    int num_blocks_needed = 0;

    int use_block_nums = 0;
    int *block_nums; // Might not be used, but this is the most logical place for it
    
    // Try to find a contiguous block of that size
    int index = bitmap_setblockofsize(num_blocks_needed, &(ramdisk.bitmap));
    if (index < 0)
    {
        // This keeps track of the empty block indeces        
        int *block_nums = malloc(num_blocks_needed*sizeof(int));

        int ii;

        // Look for empty blocks
        for (ii = 0; ii < num_blocks_needed; ii++)
        {
            int index = bitmap_setblockofsize(1, &(ramdisk.bitmap));

            // If there are no empty blocks, then return
            if (index < 0)
            {
                printf("ERROR: rd_write: ramdisk full\n");
                break;
            }

            // If found a block, then set it in the array
            block_nums[ii] = index;
        }

        // If we did not find sufficient blocks, then unset the ones
        // we had set in the bitmap
        if (ii < num_blocks_needed)//TODO: not sure id I need num_blocks_needed or num_blocks_needed-1
        {
            int jj;

            for (jj = 0; jj <= ii; jj++)
            {
                bitmap_removeatindex(block_nums[jj], &(ramdisk.bitmap));
            }
            
            return -1;
        }
        
        use_block_nums = 1;
    } 

    if (use_block_nums == 0)
    {
        //TODO: add new blocks to the inode....
        //      in this case, take them from the use_block_nums array
        //      this is the non contiguous case
    }
    else
    {
        //TODO: add new blocks to the inode....
        //      in this case, use the nodes starting at index ending at index + num_blocks_needed
        //      this is the contiguous case
    }

    //TODO: write to these new found blocks the contents of *address
    
    return 0;
}

int rd_seek(int fd, int offset)
{
    //TODO: everything
    return 0;
}

int rd_unlink(char *pathname)
{
    extern struct Ramdisk ramdisk;

    // Take care of the path
    int inodeofpath = _ramdisk_parsepath(pathname);
    if (inodeofpath < 0)
    {
        printf("ERROR: rd_open: invalid pathname\n");
        return -1;
    }

    //TODO: remove path from that inodeodpath
    //      clear the file's inode
    //      clear blocks for that file
    //      update the bitmap          
    
    return 0;
}

int rd_readdir(int fd, char *address)
{
    //TODO: everything
    return 0;
}





int _ramdisk_initialize()
{
    extern struct Ramdisk ramdisk;
    int ret;
    
    ret = bitmap_initialize(&(ramdisk.bitmap));
    if (ret < 0)
    {
        printf("Ramdisk:: initialize: error initializing bitmap\n");
    }
    
    ret = bitmap_setatindex(0, &(ramdisk.bitmap));
    if (ret < 0)
    {
        printf("Ramdisk:: initialize: setting first bit of bitmap\n");
    }

    //TODO: set first block for "/"
    //TODO: Set Inode for "/"
}


//TODO: decide on the return value for this
int _ramdisk_parsepath(char *pathname)
{
    #ifdef DEBUG
    printf("Ramdisk:: parsepath: path = %s\n", pathname);
    #endif
    
    if (pathname[0] != *("/"))
    {
        printf("Ramdisk:: parsepath: invalid pathname\n");
        return -1;
    }
    
    int index = 1;
    int inodenum = 0;

    int count = 0;

    char path[20] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

    #ifdef DEBUG
    printf("Ramdisk:: parsepath: path so far '/'\n");
    #endif

    while (1==1)
    {
        
        if (pathname[index] == *("/"))
        {
            
            //
            // TODO: get inode number
            //
            //inodenum = getInodeNum inodenum, path;
            //
            //if path does not exist return -1

            #ifdef DEBUG
            printf("Ramdisk:: parsepath: path so far '%s'\n", path);
            #endif

            count = 0;
            index++;
        }
        
        if (pathname[index] == *("\0"))
        {
            if (pathname[index - 1] != *("/"))
            {
                //
                // TODO: get inode number
                //
                //inodenum = getInodeNum inodenum, path;
                //
                //if path does not exist return -1

                #ifdef DEBUG
                printf("Ramdisk:: parsepath: path so far '%s'\n", path);
                #endif
            }
            return inodenum;
        }
        
        path[count] = pathname[index];
        index++;
        count++;
    }

    #ifdef DEBUG
    printf("\n");
    #endif

    return inodenum;
}







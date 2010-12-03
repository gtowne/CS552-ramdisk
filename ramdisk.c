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

extern struct FdtableArray fdtablea;

int rd_creat(char *iPathname)
{
    extern struct Ramdisk ramdisk;

    //struct IndexNode* node = &(ramdisk.inodes[0]);
    if(iPathname[0] != '/')
    {
      return -1;
    }
    int nameLength = str_len(iPathname, MAX_FILENAME_LENGTH);
    int last_delim = str_last_delimiter(iPathname, nameLength-1, '/');
    
    //copy the first part of the path into a buffer
    char root[MAX_FILENAME_LENGTH];
    str_copy(iPathname, root, last_delim+1);
    root[last_delim] = '\0';

    //grab the last token of the path length (this is the thing we want to create)
    char token[14];
    //    str_copy(&(pathname[last_delim+1]), token, DIR_ENTRY_NAME_BYTES);
    str_copy(&(iPathname[last_delim+1]), token, nameLength-last_delim);

    int index=0;
    if(last_delim>0)
    {
      index = _ramdisk_walk_path(&ramdisk, root);
    }
    if(index < 0)
    {
      return -1;
    }

    //@TODO: Make sure the new token doesn't already exist


    int finished = _ramdisk_add_directory_entry(&ramdisk, &(ramdisk.inodes[index]), 
						token, RAMDISK_FILE);
    return finished;
}

int rd_mkdir(char *iPathname)
{
    extern struct Ramdisk ramdisk;

    //struct IndexNode* node = &(ramdisk.inodes[0]);
    if(iPathname[0] != '/')
    {
      return -1;
    }
    int nameLength = str_len(iPathname, MAX_FILENAME_LENGTH);
    int last_delim = str_last_delimiter(iPathname, nameLength-1, '/');
    
    //copy the first part of the path into a buffer
    char root[MAX_FILENAME_LENGTH];
    str_copy(iPathname, root, last_delim+1);
    root[last_delim] = '\0';

    //grab the last token of the path length (this is the thing we want to create)
    char token[14];
    //    str_copy(&(pathname[last_delim+1]), token, DIR_ENTRY_NAME_BYTES);
    str_copy(&(iPathname[last_delim+1]), token, nameLength-last_delim);

    int index=0;
    if(last_delim>0)
    {
      index = _ramdisk_walk_path(&ramdisk, root);
    }
    if(index < 0)
    {
      return -1;
    }

    //@TODO: Make sure the new token doesn't already exist

    int finished = _ramdisk_add_directory_entry(&ramdisk, &(ramdisk.inodes[index]), 
						token, RAMDISK_DIR);

    return finished;
}

int rd_open(char *pathname)
{
    extern struct Ramdisk ramdisk;

    int idx = _ramdisk_walk_path(&ramdisk, pathname);

    int pid = getpid();

    int fd = fdtable_a_createentry(pid, idx, &fdtablea);
    if (fd < 0)
    {
        printf("ERROR::: Ramdisk:: rd_open: Could not create entry in fd table\n");
    }

    #ifdef DEBUG
    printf("Ramdisk:: rd_open: Opened: %s, fd:%d\n", pathname, fd);
    #endif
    
    return fd;
    
    /*
    struct IndexNode* node = &(ramdisk.inodes[0]);
    char token[14];

    str_copy(&(pathname[1]), token, DIR_ENTRY_NAME_BYTES);
    int idx = _ramdisk_find_directory_entry(&ramdisk, node, token);
    */

    /*
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
    */
    
    //return idx;
}

int rd_close(int fd)
{
    extern struct Ramdisk ramdisk;

    int pid = getpid();
    
    int retval = fdtable_a_removeatfd(pid, fd, &fdtablea);
    if (retval < 1)
    {
        printf("ERROR::: Ramdisk:: close: fdtable problem removing entry\n");
    }
    
    //TODO: Everything
    return 0;
}

int rd_read(int fd, char* address, int num_bytes)
{
    extern struct Ramdisk ramdisk;

    int pid = getpid();
    // Get the inode numer for this file descriptor
    int idx = fdtable_a_inodeforfd(pid, fd, &fdtablea);
    if (idx < 1)
    {
        printf("ERROR::: Ramdisk:: read: fdtable problem getting inode number\n");
    }
    // Find the last position we read/wrote from in the file
    int pos_in_file = fdtable_a_positionforfd(pid, fd, &fdtablea);
    if (pos_in_file < 1)
    {
        printf("ERROR::: Ramdisk:: read: fdtable problem getting offset\n");
    }
    
    //TODO: Everything

    // Update the file's offset

    int retval = fdtable_a_seekwithfd(pid, fd, pos_in_file, &fdtablea);
    if (retval < 1)
    {
        printf("ERROR::: Ramdisk:: read: fdtable problem seeking\n");
    }
    
    return 0;
}

int rd_write(int fd, char* address, int num_bytes)
{
    int pid = getpid();
    // Get the inode numer for this file descriptor
    int idx = fdtable_a_inodeforfd(pid, fd, &fdtablea);
    if (idx < 1)
    {
        printf("ERROR::: Ramdisk:: write: fdtable problem getting inode number\n");
    }
    // Find the last position we read/wrote from in the file
    int pos_in_file = fdtable_a_positionforfd(pid, fd, &fdtablea);
    if (pos_in_file < 1)
    {
        printf("ERROR::: Ramdisk:: write: fdtable problem getting offset\n");
    }
    
  /*
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
    */    
    
    // Update the file's offset
    fdtable_a_seekwithfd(pid, fd, pos_in_file, &fdtablea);
    int retval = fdtable_a_seekwithfd(pid, fd, pos_in_file, &fdtablea);
    if (retval < 1)
    {
        printf("ERROR::: Ramdisk:: write: fdtable problem seeking\n");
    }
    
    return 0;
}

int rd_seek(int fd, int offset)
{
    int pid = getpid();
    int retval = fdtable_a_seekwithfd(pid, fd, offset, &fdtablea);
    if (retval < 0)
    {
        printf("ERROR::: Ramdisk:: seek: could not seek\n");
    }

    //TODO: everything
    return 0;
}

int rd_unlink(char *pathname)
{
    extern struct Ramdisk ramdisk;

    int idx = _ramdisk_walk_path(&ramdisk, pathname);

    int pid = getpid();
    
    int retval = fdtable_a_removeatinodenum(pid, idx, &fdtablea);
    if(retval < 0)
    {
        printf("ERROR::: Ramdisk:: unlink: Could not unlink %s\n", pathname);
    }
    
    /*
    // Take care of the path
    int inodeofpath = _ramdisk_parsepath(pathname);
    if (inodeofpath < 0)
    {
        printf("ERROR: rd_open: invalid pathname\n");
        return -1;
    }
    */
    //TODO: remove path from that inodeodpath
    //      clear the file's inode
    //      clear blocks for that file
    //      update the bitmap          
    
    return 0;
}

int rd_readdir(int fd, char *address)
{
    int pid = getpid();
    // Get the inode numer for this file descriptor
    int idx = fdtable_a_inodeforfd(pid, fd, &fdtablea);
    // Find the last position we read/wrote from in the file
    if (idx < 1)
    {
        printf("ERROR::: Ramdisk:: readir: fdtable problem\n");
    }

    
    //TODO: everything
    return 0;
}





int _ramdisk_initialize(struct Ramdisk* ramdisk)
{
  //  extern struct Ramdisk ramdisk;
  
  int i=0; 
  for(i=0; i<INODES; i++)
  {
    inode_init(&(ramdisk->inodes[i]));
  }
  
  int ret;
  ret = bitmap_initialize(&(ramdisk->bitmap));
  if (ret < 0)
  {
      printf("Ramdisk:: initialize: error initializing bitmap\n");
  }
  
  //mark all of the overhead blocks as used 
  for(i=0; i<OVERHEAD_BLOCKS; i++)
  {
    ret = bitmap_setatindex(i, &(ramdisk->bitmap));
    if(ret != 0)
    {
      //ERROR!!
    }
  }

  //@TODO: Initialize Blocks? Lazy evaluation?  


  //now, configure the "/" directory.
  ramdisk->inodes[0].type = RAMDISK_DIR;
  //initially all pointers all null, so everything should be set now.

  //we know we'll have to put something here eventually, so allocate a block now.
  inode_add_block(&(ramdisk->inodes[0]), ramdisk);

  fdtable_a_initialize(&fdtablea);

  return 0;
}

struct Block* _ramdisk_allocate_block(struct Ramdisk* ramdisk)
{
  int ret;
  ret = bitmap_get_one_block(&(ramdisk->bitmap));
  if (ret < 0)
  {
    printf("ERROR: Ramdisk::allocate_block: could not find a free block\n");
    return NULL;
  }
  
  if (ret < OVERHEAD_BLOCKS)
  {
    printf("ERROR: Ramdisk::allocate_block: This block is not free!!\n");
  }
  
  printf("Ramdisk::allocate_block: allocated block # %d\n", ret);

  int index = ret-OVERHEAD_BLOCKS;
  _null_out_block(&(ramdisk->blocks[index]));
  return &(ramdisk->blocks[index]);  
}

int _ramdisk_deallocate_block(struct Ramdisk* ramdisk, struct Block* block)
{
  struct Block *begin, *end;

  begin=&(ramdisk->blocks[0]);
  end  =&(ramdisk->blocks[FS_BLOCKS-1]);
  if(block < begin || block > end)
  {
    printf("Ramdisk:: deallocate_block: pointer value is out of range\n");
    return -1;
  }

  int index = (block-begin)/sizeof(struct Block*);
    
  int ret = bitmap_removeatindex(index, &(ramdisk->bitmap));
  if(ret != 0)
  {
    printf("Ramdisk:: deallocate_block: error resetting bitmap index %d\n", index);
    return -1;
  }
}

int _ramdisk_allocate_inode(struct Ramdisk* ramdisk, enum NodeType type)
{
  int i;
  for(i=0; i<INODES; i++)
  {
    if(ramdisk->inodes[i].type == RAMDISK_UNALLOCATED)
    {
      ramdisk->inodes[i].type = type;
      printf("Ramdisk::allocate_inode: allocated inode # %d\n", i);
      return i;
    }
  }

  #ifdef DEBUG
  printf("Ramdisk::allocate_inode: No empty inodes available\n");
  #endif

  return -1;
}

int _ramdisk_deallocate_inode(struct Ramdisk* ramdisk, int index)
{
  if(index < 0 || index >= INODES)
  {
    #ifdef DEBUG
    printf("Ramdisk::deallocate_inode: node index out of range\n");
    #endif
    return -1;
  }

  if(ramdisk->inodes[index].type == RAMDISK_UNALLOCATED)
#ifdef DEBUG
    printf("Ramdisk::deallocate_inode: tried to deallocate unallocated inode\n");
#endif
  return -1;
  
  ramdisk->inodes[index].type = RAMDISK_UNALLOCATED;
  inode_init(&(ramdisk->inodes[index]));
  return 0;
}

int _ramdisk_add_directory_entry(struct Ramdisk* ramdisk, struct IndexNode* parent,
				  char* name, enum NodeType type)
{
   if(parent == NULL)
   {
     return -1;
   }
   
   //can't add an entry if the parent isn't a directory
   if(parent->type != RAMDISK_DIR)
   {
     return -1;
   }

   //@TODO: check if the thing already exists


   //get a new inode for the thing we are creating
   int childIdx = _ramdisk_allocate_inode(ramdisk, type);
   if(childIdx < 0)
   {
     return -1;
   }

   //get the last memory block of the directory (or allocate a new one)
   int dummyoffset;
   struct Block* block = inode_get_last_block(parent, &dummyoffset);
   if(block == NULL)
   {
     block = inode_add_block(parent, ramdisk);
     if(block == NULL)
     {
       //ERROR!
       return -1;
     }
   }

   //insert the directory entry
   int retval = add_directory_entry(block, name, (unsigned short)(childIdx));

   //shouldn't ever enter this block -- the block above should take care of it
   //if it was full, allocate another block and try again
   if(retval == DIRECTORY_ENTRIES_PER_BLOCK) //full
   {
     block = inode_add_block(parent, ramdisk);
     if(block == NULL)
     {
       //ERROR!
       
       //@TODO: release inode
       return -1;
     }
     retval = add_directory_entry(block, name, (unsigned short)(childIdx));
   }
   
   //increment the size of the block
   parent->size += sizeof(struct DirectoryEntry);

   printf("To iNode %x, added entry \"%s\" at position %d, %x\n", parent, name, childIdx, &(ramdisk->inodes[childIdx]));


   return childIdx;
 }


int _ramdisk_find_directory_entry(struct Ramdisk* ramdisk, 
				  struct IndexNode* parent, char* name)
{
  int offset=0;
  struct Block* block;
  int dummy;
  unsigned short index;
  int retval;

  if(parent->type != RAMDISK_DIR)
  {
    return -1;
  }

  while(offset < parent->size)
  {
    block = inode_get_block_for_byte_index(parent, offset, &dummy);
    retval = get_directory_inode_index(block, name, &index);
    if(retval == 1)
    {
      return index;
    }
    offset+=BLOCK_BYTES;
  }
  return -1;
}

int _ramdisk_walk_path(struct Ramdisk* ramdisk, char* name)
{
  if(name[0] != '/')
  {
    return -1;
  }
  
  int pathLength=str_len(name, MAX_FILENAME_LENGTH); //just picked a number  

  char token[DIR_ENTRY_NAME_BYTES];

  int path_pos=0;
  int token_end, tokenLength;
  int nodeIdx = 0;
  struct IndexNode* node = &(ramdisk->inodes[nodeIdx]);

  printf("Walk path: /");
  
  while(1==1)
  {
    token_end = str_next_token(name, path_pos+1, pathLength, '/');
    tokenLength= token_end-path_pos;
    if(name[path_pos]=='\0' || tokenLength == 0)
    {
      printf("...done\n");
      return nodeIdx;
    }
    if(tokenLength > DIR_ENTRY_NAME_BYTES)
      //strictly greater because "tokenLength" will include the delimiter
    {
      printf("Ramdisk::walk_path: token too long\n");
      return -1;
    }

    str_copy(&(name[path_pos+1]), token, tokenLength);
    token[tokenLength-1]='\0';
    printf("/%s",token);

    nodeIdx = _ramdisk_find_directory_entry(ramdisk, node, token);
    if(nodeIdx < 0)
    {
      //path not found
       printf("\nRamdisk::walk_path: path not found\n");
     return -1;
    }
    node = &(ramdisk->inodes[nodeIdx]);
    path_pos = token_end;
  }

  return -1;
}

/*
//TODO: decide on the return value for this
int _ramdisk_parsepath(char *pathname)
{
    #ifdef DEBUG
    printf("Ramdisk:: parsepath: path = %s\n", pathname);
    #endif
    
    if (pathname[0] != '/')
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
        
        if (pathname[index] == '/')
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
        
        if (pathname[index] == '\0')
        {
            if (pathname[index - 1] != '/')
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

*/





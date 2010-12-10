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

#ifdef USE_PTHREADS
#include <stdio.h>
#include <stdlib.h>
#else

#endif

#include "ramdisk.h"

struct FdtableArray fdtablea;
struct Ramdisk* RAMDISK;

EXPORT_SYMBOL(fdtablea);
EXPORT_SYMBOL(RAMDISK);

//for debugging reading and writing
int block_fill(struct Block* block)
{
    int i;
    for(i=0; i<BLOCK_BYTES; i++)
    {
        block->memory[i]='~';
    }
    return 0;
}

int rd_test(int* free_inodes, int* free_blocks)
{
  *free_inodes = RAMDISK->superblock.free_inodes;
  *free_blocks = RAMDISK->superblock.free_blocks;
  return 1;
}

int rd_init(void)
{
    if(RAMDISK == NULL)
    {
        RAMDISK = vmalloc(RAMDISK_SIZE);
        PRINT("RD_INIT\n");
    }
    _ramdisk_initialize(RAMDISK);
    return 1;
}

int rd_creat(char *iPathname)
{
    char token[14]; //the last part of the path, the thing we want to create
    int parent;
    int finished;

    superblock_lock(&(RAMDISK->superblock));

    parent = _ramdisk_get_parent(RAMDISK, iPathname, token);
    if(parent < 0)
    {
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    finished = _ramdisk_add_directory_entry(RAMDISK, &(RAMDISK->inodes[parent]),
                                            token, RAMDISK_FILE);
    //will return an error if the entry already exists

    if(finished >= 0)
    {
#ifdef DEBUG
        PRINT("RD_CREAT created file %d %s\n", finished, iPathname);
#endif
        superblock_unlock(&(RAMDISK->superblock));
        return 0;
    }

    superblock_unlock(&(RAMDISK->superblock));
    return -1;
}

int rd_mkdir(char *iPathname)
{
    char token[14]; //the last part of the path, the thing we want to create
    int parent;
    int finished;

    superblock_lock(&(RAMDISK->superblock));

    parent = _ramdisk_get_parent(RAMDISK, iPathname, token);
    if(parent < 0)
    {
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }
    finished = _ramdisk_add_directory_entry(RAMDISK,
                                            &(RAMDISK->inodes[parent]),
                                            token, RAMDISK_DIR);

    //will return an error if the entry already exists

    if(finished >= 0)
    {
#ifdef DEBUG
        PRINT("RD_MKDIR Added directory %d %s\n", finished, iPathname);
#endif
        superblock_unlock(&(RAMDISK->superblock));
        return 0;
    }
    superblock_unlock(&(RAMDISK->superblock));
    return -1;
}

int rd_open(char *pathname)
{
    // I hate my life!!!
    int idx;
    int pid;
    int fd;

    superblock_lock(&(RAMDISK->superblock));

    idx = _ramdisk_walk_path(RAMDISK, pathname);
    if(idx < 0)
    {
        PRINT("ERROR::: Ramdisk:: rd_open: Could not find file %s\n", pathname);
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

#ifdef USE_PTHREADS
    pid = getpid();
#else
    pid = current->pid;
#endif

    fd = fdtable_a_createentry(pid, idx, &fdtablea);
    if (fd < 0)
    {
        PRINT("ERROR::: Ramdisk:: rd_open: Could not create entry in fd table\n");
    }

#ifdef DEBUG
    PRINT("RD_OPEN Opened: %s, inode:%d, fd:%d\n", pathname, idx, fd);
#endif

    superblock_unlock(&(RAMDISK->superblock));
    return fd;
}

int rd_close(int fd)
{
    int pid;
    int retval;

    superblock_lock(&(RAMDISK->superblock));

#ifdef USE_PTHREADS
    pid = getpid();
#else
    pid = current->pid;
#endif

    retval = fdtable_a_removeatfd(pid, fd, &fdtablea);
    if (retval < 0)
    {
        PRINT("ERROR::: Ramdisk:: close: fdtable problem removing entry\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

#ifdef DEBUG
    PRINT("RD_CLOSE closed file descriptor %d\n", fd);
#endif

    superblock_unlock(&(RAMDISK->superblock));
    return 0;
}

int rd_read(int fd, char* address, int num_bytes)
{
    int pid;
    int idx;
    struct IndexNode* node;
    int pos_in_file;
    int bytesToRead;
    int block_offset;
    int bytesRead;
    int totalBytes;
    struct Block* block;
    int retval;

    superblock_lock(&(RAMDISK->superblock));

#ifdef USE_PTHREADS
    pid = getpid();
#else
    pid = current->pid;
#endif
    // Get the inode numer for this file descriptor
    idx = fdtable_a_inodeforfd(pid, fd, &fdtablea);
    if (idx < 0)
    {
        PRINT("ERROR::: Ramdisk:: read: fdtable problem getting inode number %d %d\n", pid, fd);
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }
    node = &(RAMDISK->inodes[idx]);
    if(node->type != RAMDISK_FILE)
    {
        PRINT("ERROR:: Ramdisk:: read. can only read from a regular file\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    // Find the last position we read/wrote from in the file
    pos_in_file = fdtable_a_positionforfd(pid, fd, &fdtablea);
    if (pos_in_file < 0)
    {
        PRINT("ERROR::: Ramdisk:: read: fdtable problem getting offset\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    //eh. This is okay -- there is no way for the calling function to
    //know how many bytes are in the file. Just do our best and return
    //the number of bytes we read.;
    //if(pos_in_file+num_bytes > node->size)
    //{
    //  PRINT("ERROR:: Ramdisk:: read. attempt to read past the end of the file\n");
    //  return 0;
    //}

    if(pos_in_file >= node->size)
    {
        superblock_unlock(&(RAMDISK->superblock));
        return 0;
        //End of File
    }

    //TODO: Memory twiddling
    bytesToRead=num_bytes;
    if(pos_in_file + bytesToRead > node->size)
    {
        bytesToRead = node->size - pos_in_file;
    }

    totalBytes=0;

    while(bytesToRead > 0)
    {
        if(pos_in_file >= node->size)
        {
            //EOF
            //return 0;
            break;
        }

        block = inode_get_block_for_byte_index(node, pos_in_file, &block_offset);
        if(block == NULL)
        {
            PRINT("ERROR:: Ramdisk:: read. error retrieving block\n");
            superblock_unlock(&(RAMDISK->superblock));
            return -1;
        }
        bytesRead = block_copy_out(block, &(address[totalBytes]),
                                   block_offset, bytesToRead);

        if(bytesRead < 0)
        {
            PRINT("ERROR: Ramdisk:: read. error reading from block\n");
            superblock_unlock(&(RAMDISK->superblock));
            return -1;
        }
        else
        {
            totalBytes += bytesRead;
            pos_in_file += bytesRead;
            bytesToRead -= bytesRead;
        }
    }
    //PRINT("Ramdisk:: read: %d %d %d %d\n", node->size, pos_in_file, totalBytes, bytesToRead);

    // Update the file's offset
    retval = fdtable_a_seekwithfd(pid, fd, pos_in_file, &fdtablea);
    if (retval < 0)
    {
        PRINT("ERROR::: Ramdisk:: read: fdtable problem seeking\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

#ifdef DEBUG
    PRINT("RD_READ Read %d bytes from file descriptor %d %d %d\n", totalBytes, fd, pos_in_file, node->size);
#endif

    superblock_unlock(&(RAMDISK->superblock));
    return totalBytes;
}

int rd_write(int fd, char* address, int num_bytes)
{
    int pid;
    int idx;
    struct IndexNode* node;
    int pos_in_file;
    int bytesToWrite;
    int block_offset;
    int bytesWritten;
    int totalBytes;
    struct Block* block;
    int retval;

    superblock_lock(&(RAMDISK->superblock));

#ifdef USE_PTHREADS
    pid = getpid();
#else
    pid = current->pid;
#endif
    // Get the inode numer for this file descriptor
    idx = fdtable_a_inodeforfd(pid, fd, &fdtablea);
    if (idx < 0)
    {
        PRINT("ERROR::: Ramdisk:: write: fdtable problem getting inode number\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    node = &(RAMDISK->inodes[idx]);
    if(node->type != RAMDISK_FILE)
    {
        PRINT("ERROR:: Ramdisk:: write. can only write to a regular file\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    // Find the last position we read/wrote from in the file
    pos_in_file = fdtable_a_positionforfd(pid, fd, &fdtablea);
    if (pos_in_file < 0)
    {
        PRINT("ERROR::: Ramdisk:: write: fdtable problem getting offset\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    //Memory twiddling
    bytesToWrite=num_bytes;
    totalBytes=0;
    while(bytesToWrite > 0)
    {
        block = inode_get_block_for_byte_index(node, pos_in_file, &block_offset);
        if(block == NULL)
        {
            block = inode_add_block(node, RAMDISK);
            if(block == NULL)
            {
                PRINT("ERROR: Ramdisk:: write unable to allocate a new block\n");
                superblock_unlock(&(RAMDISK->superblock));
                return -1;
            }
            block_fill(block); //so we can see any errors with offsets
        }
        bytesWritten = block_copy_in(block, &(address[totalBytes]),
                                     block_offset, bytesToWrite);
        if(bytesWritten < 0)
        {
            PRINT("ERROR: Ramdisk:: write error writing to block\n");
            superblock_unlock(&(RAMDISK->superblock));
            return -1;
        }
        else
        {
            totalBytes += bytesWritten;
            pos_in_file += bytesWritten;
            bytesToWrite -= bytesWritten;

            //keep the size updated (important for correct behavior of inode_add_block)
            if(node->size < pos_in_file)
            {
                node->size = pos_in_file;
            }
        }
    }

    //update file's size
    if(node->size < pos_in_file)
    {
        node->size = pos_in_file;
    }

    // Update the file's offset
    //fdtable_a_seekwithfd(pid, fd, pos_in_file, &fdtablea);
    retval = fdtable_a_seekwithfd(pid, fd, pos_in_file, &fdtablea);
    if (retval < 0)
    {
        PRINT("ERROR::: Ramdisk:: write: fdtable problem seeking\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }
#ifdef DEBUG
    PRINT("RD_WRITE Wrote %d bytes to file descriptor %d\n", totalBytes, fd);
#endif

    superblock_unlock(&(RAMDISK->superblock));
    return totalBytes;
}

int rd_seek(int fd, int offset)
{
    int pid;
    int retval;
    int idx;
    struct IndexNode* node;
    int size;

    superblock_lock(&(RAMDISK->superblock));

#ifdef USE_PTHREADS
    pid = getpid();
#else
    pid = current->pid;
#endif

    if (offset == -1)
    {
        idx = fdtable_a_inodeforfd(pid, fd, &fdtablea);
        node = &(RAMDISK->inodes[idx]);
        size = node->size;
        retval = fdtable_a_seekwithfd(pid, fd, size, &fdtablea);
    }
    else
    {
        retval = fdtable_a_seekwithfd(pid, fd, offset, &fdtablea);
    }

    if (retval < 0)
    {
        PRINT("ERROR::: Ramdisk:: seek: could not seek\n");
    }
#ifdef DEBUG
    PRINT("RD_SEEK Seeked to position %d in file descriptor %d\n", offset, fd);
#endif

    superblock_unlock(&(RAMDISK->superblock));
    return 0;
}

int rd_unlink(char *pathname)
{
    int retval;
    int idx;
    int pid;
    char name[14]; //the last part of the path, the thing we want to delete
    int parent;
    struct IndexNode* parentInode;
    struct Block* entryBlock;
    int entryOffset;
    int item;
    struct IndexNode* itemNode;
    struct Block* lastBlock;
    int lastOffset;

    superblock_lock(&(RAMDISK->superblock));

    idx = _ramdisk_walk_path(RAMDISK, pathname);
#ifdef USE_PTHREADS
    pid = getpid();
#else
    pid = current->pid;
#endif

    retval = fdtable_a_checkforinode(pid, idx,&fdtablea);

    if (retval == 1)
    {
        PRINT("ERROR: Ramdisk:: unlink. Someone using file: %s\n", pathname);

        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }


    parent = _ramdisk_get_parent(RAMDISK, pathname, name);
    if(parent < 0)
    {
        PRINT("ERROR: Ramdisk:: unlink. Invalid path %s\n", pathname);

        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }
    parentInode = &(RAMDISK->inodes[parent]);

    item = _ramdisk_find_directory_entry(RAMDISK, &(RAMDISK->inodes[parent]),
                                         name,
                                         &entryBlock, &entryOffset);
    if(item < 0)
    {
        PRINT("ERROR: Ramdisk:: unlink. Invalid path %s\n", pathname);

        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    //free all the blocks in the inode
    itemNode = &(RAMDISK->inodes[item]);
    if(itemNode->type == RAMDISK_DIR && itemNode->size > 0)
    {
        PRINT("ERROR: Ramdisk:: unlink. Attempted to unlink non-empty directory %s\n", pathname);

        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    //release all the blocks
    inode_release(itemNode, RAMDISK);
    _ramdisk_deallocate_inode(RAMDISK, item);

    //remove the entry from the parent
    //we know where this item's entry is, from the call to find the
    //inode. swap it with the last item in the directory, and
    //decrement the parent's size.
    //
    //entryBlock and entryOffset are the block and offset for the
    //doomed entry
    //

    //get the Block* and offset for the last entry

    lastBlock = inode_get_block_for_byte_index(parentInode,
                parentInode->size - DIRECTORY_ENTRY_SIZE,
                &lastOffset);

    //copy the last entry over the doomed entry
    block_copy_in(entryBlock, &(lastBlock->memory[lastOffset]),
                  entryOffset*DIRECTORY_ENTRY_SIZE,
                  DIRECTORY_ENTRY_SIZE);

    //null out the former last entry
    _null_out_directory_entry((struct DirectoryEntry*)(&lastBlock->memory[lastOffset]));

    retval = inode_reduce_size(parentInode, RAMDISK, DIRECTORY_ENTRY_SIZE);

    if(retval < 0)
    {
        PRINT("ERROR: Ramdisk:: unlink. error removing directory entry %s\n", pathname);

        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

#ifdef DEBUG
    PRINT("RD_UNLINK unlinked file %s, %d\n", pathname, item);
#endif

    superblock_unlock(&(RAMDISK->superblock));
    return 0;
}

int rd_readdir(int fd, char *address)
{
    int pid;
    int idx;
    int pos_in_file;
    struct IndexNode* node;
    int offset;
    struct Block* block;
    int bytesWritten;
    int retval;

    superblock_lock(&(RAMDISK->superblock));

#ifdef USE_PTHREADS
    pid = getpid();
#else
    pid = current->pid;
#endif
    // Get the inode numer for this file descriptor
    idx = fdtable_a_inodeforfd(pid, fd, &fdtablea);
    if (idx < 0)
    {
        PRINT("ERROR::: Ramdisk:: readir: could not get inode index from fdtable\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }
    // Find the last position we read/wrote from in the file
    pos_in_file = fdtable_a_positionforfd(pid, fd, &fdtablea);
    if (pos_in_file < 0)
    {
        PRINT("ERROR::: Ramdisk:: readdir: fdtable problem getting offset\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    //get the inode, check the size
    node = &(RAMDISK->inodes[idx]);
    if(node->type != RAMDISK_DIR)
    {
        PRINT("ERROR:: Ramdisk:: readdir. can only read from a directory\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    if(pos_in_file == node->size)
    {
        //EOF
        superblock_unlock(&(RAMDISK->superblock));
        return 0;
    }

    //get the block for the right address

    block = inode_get_block_for_byte_index(node, pos_in_file, &offset);

    //copy the memory

    bytesWritten = block_copy_out(block, address, offset, DIRECTORY_ENTRY_SIZE);
    if(bytesWritten != DIRECTORY_ENTRY_SIZE)
    {
        PRINT("ERROR::: Ramdisk:: readir: error copying directory entry out of block\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

    //move the pointer
    pos_in_file += DIRECTORY_ENTRY_SIZE;
    retval = fdtable_a_seekwithfd(pid, fd, pos_in_file, &fdtablea);
    if (retval < 0)
    {
        PRINT("ERROR::: Ramdisk:: readdir: fdtable problem seeking\n");
        superblock_unlock(&(RAMDISK->superblock));
        return -1;
    }

#ifdef DEBUG
    PRINT("RD_READDIR Read entry from file descriptor %d\n", fd);
#endif

    superblock_unlock(&(RAMDISK->superblock));
    return 1;
}





int _ramdisk_initialize(struct Ramdisk* ramdisk)
{
    int i;
    int ret;
    int root;

#ifdef DEBUG
    PRINT("_ramdisk_initialize\n");
#endif

    superblock_initialize(&(ramdisk->superblock));

    for(i=0; i<INODES; i++)
    {
        r_inode_init(&(ramdisk->inodes[i]));
    }


    ret = bitmap_initialize(&(ramdisk->bitmap));
    if (ret < 0)
    {
        PRINT("Ramdisk:: initialize: error initializing bitmap\n");
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
    ramdisk->superblock.free_blocks -= OVERHEAD_BLOCKS;

    //now, configure the "/" directory.
    root = _ramdisk_allocate_inode(ramdisk, RAMDISK_DIR);
    if(root != 0)
    {
      PRINT("Ramdisk:: initialize: error configuring root directory\n");
    }

    fdtable_a_initialize(&fdtablea);

    return 0;
}

struct Block* _ramdisk_allocate_block(struct Ramdisk* ramdisk)
{
    //@TODO: use the superblock for book-keeping
    int ret;
    int index;
    struct Block* block;

    if(ramdisk->superblock.free_blocks == 0)
    {
        PRINT("Ramdisk::allocate_inode: no more iNodes\n");
        return NULL;
    }
    ret = bitmap_get_one_block(&(ramdisk->bitmap));
    if (ret < 0)
    {
        PRINT("ERROR: Ramdisk::allocate_block: could not find a free block\n");
        return NULL;
    }

    if (ret < OVERHEAD_BLOCKS)
    {
        PRINT("ERROR: Ramdisk::allocate_block: This block is part of the overhead!!\n");
        return NULL;
    }

    index = ret-OVERHEAD_BLOCKS;
    block = &(ramdisk->blocks[index]);
    _null_out_block(block);
    ramdisk->superblock.free_blocks--;

#ifdef DEBUG
    PRINT("Ramdisk::allocate_block: allocated block # %d. %hd blocks free\n", ret, ramdisk->superblock.free_blocks);
#endif
    return block;
}

int _ramdisk_deallocate_block(struct Ramdisk* ramdisk, struct Block* block)
{
    //@TODO: use the superblock for book-keeping
    struct Block *begin, *end;
    int index;
    int ret;

    begin=&(ramdisk->blocks[0]);
    end  =&(ramdisk->blocks[FS_BLOCKS-1]);
    if(block < begin || block > end)
    {
        PRINT("ERROR: Ramdisk:: deallocate_block: pointer value is out of range\n");
        return -1;
    }

    index = (block-begin); //compiler automatically divides by the
    //size of struct Block

    ret = bitmap_removeatindex(index+OVERHEAD_BLOCKS, &(ramdisk->bitmap));
    if(_bitmap_isset_b(index+OVERHEAD_BLOCKS, &(ramdisk->bitmap)) == 1)
    {
        PRINT("Error: Why is this bit still set?\n");
    }
    ramdisk->superblock.free_blocks++;


    if(ret != 0)
    {
        PRINT("Ramdisk:: deallocate_block: error resetting bitmap index %d\n", index);
        return -1;
    }

#ifdef DEBUG
    PRINT("Ramdisk::deallocate_block: deallocated block # %d. %hd blocks free\n", index+OVERHEAD_BLOCKS, ramdisk->superblock.free_blocks);
#endif
    return 1;
}

int _ramdisk_allocate_inode(struct Ramdisk* ramdisk, enum NodeType type)
{
    //@TODO: use the superblock for book-keeping
    int i;
    if(ramdisk->superblock.free_inodes == 0)
    {
        PRINT("Ramdisk::allocate_inode: no more iNodes\n");
        return -1;
    }
    for(i=0; i<INODES; i++)
    {
        if(ramdisk->inodes[i].type == RAMDISK_UNALLOCATED)
        {
            ramdisk->inodes[i].type = type;
	    ramdisk->superblock.free_inodes --;
#ifdef DEBUG
            PRINT("Ramdisk::allocate_inode: allocated inode # %d. %hd inodes free\n", i, ramdisk->superblock.free_inodes);
#endif
            return i;
        }
    }

#ifdef DEBUG
    PRINT("Ramdisk::allocate_inode: No empty inodes available\n");
#endif

    return -1;
}

int _ramdisk_deallocate_inode(struct Ramdisk* ramdisk, int index)
{
    //@TODO: use the superblock for book-keeping
    if(index < 0 || index >= INODES)
    {
#ifdef DEBUG
        PRINT("Ramdisk::deallocate_inode: node index out of range\n");
#endif
        return -1;
    }

    if(ramdisk->inodes[index].type == RAMDISK_UNALLOCATED)
    {
#ifdef DEBUG
        PRINT("Ramdisk::deallocate_inode: tried to deallocate unallocated inode\n");
#endif
        return -1;
    }

    ramdisk->superblock.free_inodes ++;
    ramdisk->inodes[index].type = RAMDISK_UNALLOCATED;
    r_inode_init(&(ramdisk->inodes[index]));
    PRINT("Ramdisk::deallocate_inode: deallocated inode # %d. %hd inodes free\n", index, ramdisk->superblock.free_inodes);
    return 0;
}

int _ramdisk_add_directory_entry(struct Ramdisk* ramdisk, struct IndexNode* parent,
                                 char* name, enum NodeType type)
{
    int len;
    int test;
    int childIdx;
    struct Block* block;
    int offset;
    int dummy;
    int retval;

    if(parent == NULL)
    {
        PRINT("ERROR: Ramdisk:: add_directory_entry. IndexNode pointer is NULL\n");
        return -1;
    }

    //can't add an entry if the parent isn't a directory
    if(parent->type != RAMDISK_DIR)
    {
        PRINT("ERROR: Ramdisk:: add_directory_entry. Parent is not a directory\n");
        return -1;
    }

    len = str_len(name, MAX_FILENAME_LENGTH);

    if(len == 0)
    {
        PRINT("ERROR: Ramdisk:: add_directory_entry. Name of thing to create is empty\n");
        return -1;
    }

    if(len >= DIR_ENTRY_NAME_BYTES)
    {
        PRINT("ERROR: Ramdisk:: add_directory_entry. Name is too long %s\n", name);
        return -1;
    }

    //check if the thing already exists
    test = _ramdisk_find_directory_entry(ramdisk, parent, name, NULL, NULL);
    if(test != -1)
    {
        PRINT("ERROR: Ramdisk:: add_directory_entry. Entry already exists\n");
        return -1;
    }

    //get a new inode for the thing we are creating
    childIdx = _ramdisk_allocate_inode(ramdisk, type);
    if(childIdx < 0)
    {
        PRINT("ERROR: Ramdisk:: add_directory_entry. Unable to allocate an iNode\n");
        return -1;
    }

    offset=0;
    retval=-1;

    //get the last memory block of the directory (or allocate a new one)
    block = inode_get_last_block(parent, &dummy);
    if(block == NULL)
    {
        block = inode_add_block(parent, ramdisk);
        if(block == NULL)
        {
            //ERROR!
            PRINT("ERROR: Ramdisk:: add_directory_entry. Unable to add a block\n");
            return -1;
        }
    }

    //insert the directory entry
    retval = add_directory_entry(block, name, (unsigned short)(childIdx));

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
    parent->size += DIRECTORY_ENTRY_SIZE;

#ifdef DEBUG
    PRINT("Ramdisk:: add_directory_entry. Added entry \"%s\" (%x) to parent %x\n", name, &(ramdisk->inodes[childIdx]), parent);
#endif

    return childIdx;
}


int _ramdisk_find_directory_entry(struct Ramdisk* ramdisk,
                                  struct IndexNode* parent, char* name,
                                  struct Block** oBlock, int* oOffset)
{
    int offset;
    struct Block* block;
    int dummy;
    unsigned short index;
    int retval;

    offset=0;

    if(parent->type != RAMDISK_DIR)
    {
        return -1;
    }

    while(offset < parent->size)
    {
        block = inode_get_block_for_byte_index(parent, offset, &dummy);
        if(block == NULL)
        {
            PRINT("ERROR:  _ramdisk_find_directory_entry. Error getting block pointer in directory\n");
            return -1;
        }

        retval = get_directory_inode_index(block, name, &index);
        if(retval >=0)
        {
            if(oBlock != NULL)
            {
                *oBlock = block;
            }
            if(oOffset != NULL)
            {
                *oOffset = retval;
            }
            return index;
        }
        offset+=BLOCK_BYTES;
    }
    return -1;
}

int _ramdisk_get_parent(struct Ramdisk* ramdisk, char* iPathname, char token[14])
{

    int nameLength;
    int last_delim;
    char root[MAX_FILENAME_LENGTH];

    if(iPathname[0] != '/')
    {
        return -1;
    }

    nameLength = str_len(iPathname, MAX_FILENAME_LENGTH);
    last_delim = str_last_delimiter(iPathname, nameLength-1, '/');

    str_copy(iPathname, root, last_delim+1);
    root[last_delim] = '\0';

    //grab the last token of the path
    str_copy(&(iPathname[last_delim+1]), token, nameLength-last_delim);

    if(last_delim == 0)
    {
        return 0;
    }
    return _ramdisk_walk_path(ramdisk, root);
}


int _ramdisk_walk_path(struct Ramdisk* ramdisk, char* name)
{
    int pathLength;
    char token[DIR_ENTRY_NAME_BYTES];
    int path_pos;
    int token_end, tokenLength;
    int nodeIdx;
    struct IndexNode* node;

    if(name[0] != '/')
    {
        return -1;
    }

    pathLength=str_len(name, MAX_FILENAME_LENGTH); //just picked a number



    path_pos=0;

    nodeIdx = 0;
    node = &(ramdisk->inodes[nodeIdx]);

    if(pathLength == 1)
    {
        //we know this is the root node
        return nodeIdx;
    }

    while(1==1)
    {
        token_end = str_next_token(name, path_pos+1, pathLength, '/');
        tokenLength= token_end-path_pos;
        if(name[path_pos]=='\0' || tokenLength == 0)
        {
            return nodeIdx;
        }
        if(tokenLength > DIR_ENTRY_NAME_BYTES)
            //strictly greater because "tokenLength" will include the delimiter
        {
            PRINT("ERROR Ramdisk::walk_path: token too long\n");
            return -1;
        }

        str_copy(&(name[path_pos+1]), token, tokenLength);
        token[tokenLength-1]='\0';

        nodeIdx = _ramdisk_find_directory_entry(ramdisk, node, token, NULL, NULL);
        if(nodeIdx < 0)
        {
            //path not found
            PRINT("ERROR: Ramdisk::walk_path: path not found (%s)\n", name);
            return -1;
        }
        node = &(ramdisk->inodes[nodeIdx]);
        path_pos = token_end;
    }

    return -1;
}


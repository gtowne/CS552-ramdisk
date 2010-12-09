#include <stdio.h>
#include <stdlib.h>

#include "superblock.h"

int main()
{
    struct Superblock superblock;

    superblock_initialize(&superblock);

    _superblock_print(&superblock);

    printf("blocks: %d, inodes: %d\n", superblock_getfreeblocks(&superblock), superblock_getfreeinodes(&superblock));

    superblock_setfreeblocks(123, &superblock);
    superblock_setfreeinodes(432, &superblock);

    _superblock_print(&superblock);

    printf("blocks: %d, inodes: %d\n", superblock_getfreeblocks(&superblock), superblock_getfreeinodes(&superblock));

    superblock_lock(&superblock);
    printf("locked\n");
    superblock_unlock(&superblock);
    printf("unlocked\n");

    return 0;
}

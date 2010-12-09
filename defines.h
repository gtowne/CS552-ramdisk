#ifndef RAMDISK_DEFINES
#define RAMDISK_DEFINES


#ifndef NULL
#define NULL 0x0
#endif

//2MB
#define RAMDISK_SIZE 2097152
#define BLOCK_BYTES 256

//#define TOTAL_BLOCKS=RAMDISK_SIZE/BLOCK_BYTES
#define TOTAL_BLOCKS 8192
#define INODE_BLOCKS 256
#define BITMAP_BLOCKS 4
//#define OVERHEAD_BLOCKS   1 + INODE_BLOCKS + BITMAP_BLOCKS
#define OVERHEAD_BLOCKS 261
//#define FS_BLOCKS TOTAL_BLOCKS - OVERHEAD_BLOCKS
#define FS_BLOCKS 7931

#define INODES 1024
//#define BITMAP_BYTES TOTAL_BLOCKS/8
#define BITMAP_NUM_BYTES 1024

#define DIR_ENTRY_NAME_BYTES 14
#define DIRECTORY_ENTRY_SIZE 16

#define TABLE_SIZE 64
#define NUM_TABLES 20

//#define USE_PTHREADS

#ifdef USE_PTHREADS
#define PRINT printf
#else
#define PRINT printk
#endif

#define DEBUG

#endif



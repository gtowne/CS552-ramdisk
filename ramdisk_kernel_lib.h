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

ramdisk_kernel_lib.h
 kernel level portion of RAM disk library, receives ioctls and marshals requests
 to the ramdisk

*/

#ifndef RAMDISK_KERNEL_LIB_H
#define RAMDISK_KERNEL_LIB_H

#define DEBUG

struct pathname_args_t
{
    char *pathname;
    int str_len;
    int ret_val;
};

struct close_args_t
{
    int fd;
    int ret_val;
};

struct read_write_args_t
{
    int fd;
    int num_bytes;
    int ret_val;
    char *address;
};

struct seek_args_t
{
    int fd;
    int offset;
    int ret_val;
};

struct test_args_t
{
    int inodes;
    int blocks;
    int ret_val;
};

#define RD_CREAT _IOW(0, 6, struct pathname_args_t)
#define RD_MKDIR _IOW(0, 7, struct pathname_args_t)
#define RD_OPEN _IOW(0, 8, struct pathname_args_t)
#define RD_CLOSE _IOW(0, 9, struct close_args_t)
#define RD_READ _IOW(0, 10, struct read_write_args_t)
#define RD_WRITE _IOW(0, 11, struct read_write_args_t)
#define RD_SEEK _IOW(0, 12, struct seek_args_t)
#define RD_READDIR _IOW(0, 13, struct read_write_args_t)
#define RD_UNLINK _IOW(0, 14, struct pathname_args_t)
#define RD_INIT _IOW(0, 15, struct pathname_args_t)
#define RD_TEST _IOW(0,16, struct test_args_t)

#define PROC_MODULE_NAME "ramdisk_lib"

#endif

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

ramdisk_lib.h
 user level library to interface with RAM disk
*/

#ifndef RAMDISK_LIB_H
#define RAMDISK_LIB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "ramdisk_kernel_lib.h"

#define PROC_ENTRY "/proc/ramdisk_lib"

int rd_init(void);
int rd_creat    (char *pathname);
int rd_mkdir    (char *pathname);
int rd_open     (char *pathname);
int rd_close    (int fd);
int rd_read     (int fd, char* address, int num_bytes);
int rd_write    (int fd, char* address, int num_bytes);
int rd_seek     (int fd, int offset);
int rd_unlink   (char *pathname);
int rd_readdir  (int fd, char *address);

#endif

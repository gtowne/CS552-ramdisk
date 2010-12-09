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

FILE DESCRIPTOR TABLE:
This implements the file descriptor table part of the assignment

*/

/*

Imagine two procesess, each process would have a Fdtable

pid = 1                                     pid = 2

    fd  |   inode_num   |   offset              fd  |   inode_num   |   offset
-------------------------------------       -------------------------------------
    1   |       23      |     0                 2   |       43      |     1
    2   |       21      |     3                 3   |       55      |     0
    4   |       90      |     7                 4   |       23      |     0
    .   |       .       |     .                 .   |       .       |     .
    .   |       .       |     .                 .   |       .       |     .
    .   |       .       |     .                 .   |       .       |     .

The FdtableArray just contains a few of these Fdtable structs
*/

#ifndef FDTABLE_H_
#define FDTABLE_H_

#include "defines.h"

#ifndef USE_PTHREADS
#include <linux/module.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>
#endif

struct FdtableEntry
{
    int     being_used;
    short   fd;
    short   inode_num;
    int     offset;
};

struct Fdtable
{
    int     pid;
    int     t_size;
    struct  FdtableEntry table[TABLE_SIZE];
};

struct FdtableArray
{
    int     a_size;
    struct  Fdtable array[NUM_TABLES];
};

/*METHODS FOR A SINGLE FD TABLE*/

int fdtable_initialize          (int pid, struct Fdtable *fdtable);                     //Initializes the fd table
int fdtable_createentry         (int inodenum, struct Fdtable *fdtable);                //Creates a new entry in the table, returns fd number, -1 if fail
int fdtable_seekwithfd          (int fd, int offset, struct Fdtable *fdtable);          //Seeks to offset given a file descriptor number
int fdtable_seekwithinodenum    (int inodenum, int offset, struct Fdtable *fdtable);    //Seeks to offset given an inode number
int fdtable_removeatfd          (int fd, struct Fdtable *fdtable);                      //Removes entry given a file descriptor number
int fdtable_removeatinodenum    (int inodenum, struct Fdtable *fdtable);                //Removes entry given an inode number

int fdtable_inodeforfd          (int fd, struct Fdtable *fdtable);
int fdtable_positionforfd       (int fd, struct Fdtable *fdtable);                      //Returns the position of the offset

int fdtable_checkforinode       (int inodenum, struct Fdtable *fdtable);

int _fdtable_print              (struct Fdtable *fdtable);                              //Convenience method to print the fd table

/*METHODS FOR ARRAY OF FD TABLES*/

// These do the same thing as the ones above, only on an
// array of *fdtables. This is just for convenience to
// encapsulate the *fdtables once we move to the kernel
// or if we decide to try a multithreaded implementation

int fdtable_a_initialize        (struct FdtableArray *fdtablea);
int fdtable_a_createtable       (int pid, struct FdtableArray *fdtablea);  //Creates a new Fdtable inside the array
int fdtable_a_createentry       (int pid, int inodenum, struct FdtableArray *fdtablea);
int fdtable_a_seekwithfd        (int pid, int fd, int offset, struct FdtableArray *fdtablea);
int fdtable_a_seekwithinodenum  (int pid, int inodenum, int offset, struct FdtableArray *fdtablea);
int fdtable_a_removeatfd        (int pid, int fd, struct FdtableArray *fdtablea);
int fdtable_a_removeatinodenum  (int pid, int inodenum, struct FdtableArray *fdtablea);

int fdtable_a_inodeforfd        (int pid, int fd, struct FdtableArray *fdtablea);
int fdtable_a_positionforfd     (int pid, int fd, struct FdtableArray *fdtablea);

// Checks if someone is using the inode inodenum, return 1 if soomeone else is using it, 0 otherwise
int fdtable_a_checkforinode     (int pid, int inodenum, struct FdtableArray *fdtablea);

int _fdtable_a_print            (struct FdtableArray *fdtablea);

#endif

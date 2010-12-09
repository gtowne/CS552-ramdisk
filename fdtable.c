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
#ifdef USE_PTHREADS
#include <stdio.h>
#include <stdlib.h>
#endif

#include "fdtable.h"

//#define TABLE_SIZE 20
//#define NUM_TABLES 20

/*METHODS FOR A SINGLE FD TABLE*/

int fdtable_initialize(int pid, struct Fdtable *fdtable)
{
    int ii;

    fdtable->pid = pid;
    fdtable->t_size = TABLE_SIZE;
    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        fdtable->table[ii].being_used  = 0;
        fdtable->table[ii].fd          = ii;
        fdtable->table[ii].inode_num   = -1;
        fdtable->table[ii].offset      = -1;
    }

    return 0;
}

int fdtable_createentry(int inodenum, struct Fdtable *fdtable)
{
    int ii;

    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        if (fdtable->table[ii].being_used == 0)
        {
            fdtable->table[ii].being_used = 1;
            fdtable->table[ii].inode_num  = inodenum;
            fdtable->table[ii].offset     = 0;
            return ii;
        }
    }

#ifdef DEBUG
    PRINT("Fdtable:: createentry: table for process %d is full\n", fdtable->pid);
#endif

    return -1;
}

int fdtable_seekwithfd(int fd, int offset, struct Fdtable *fdtable)
{
    int ii;

    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        if (fdtable->table[ii].fd == fd)
        {
            fdtable->table[ii].offset = offset;
            return 0;
        }
    }
    return -1;
}

int fdtable_seekwithinodenum(int inodenum, int offset, struct Fdtable *fdtable)
{
    int ii;

    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        if (fdtable->table[ii].inode_num == inodenum)
        {
            fdtable->table[ii].offset = offset;
            return 0;
        }
    }

    return -1;
}

int fdtable_removeatfd(int fd, struct Fdtable *fdtable)
{
    int ii;

    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        if (fdtable->table[ii].fd == fd)
        {
            if(fdtable->table[ii].being_used != 1)
            {
                break;
            }
            fdtable->table[ii].being_used = 0;
            fdtable->table[ii].inode_num  = -1;
            fdtable->table[ii].offset = -1;
            return 0;
        }
    }

    return -1;
}

int fdtable_removeatinodenum(int inodenum, struct Fdtable *fdtable)
{
    int ii;

    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        if (fdtable->table[ii].inode_num == inodenum)
        {
            fdtable->table[ii].being_used = 0;
            fdtable->table[ii].inode_num  = -1;
            fdtable->table[ii].offset = -1;
            return 0;
        }
    }

    return -1;
}

int fdtable_inodeforfd(int fd, struct Fdtable *fdtable)
{
    int ii;

    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        if (fdtable->table[ii].fd == fd)
        {
            return fdtable->table[ii].inode_num;
        }
    }

    return -1;
}

int fdtable_positionforfd(int fd, struct Fdtable *fdtable)
{
    int ii;

    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        if (fdtable->table[ii].fd == fd)
        {
            return fdtable->table[ii].offset;
        }
    }

    return -1;
}

int fdtable_checkforinode(int inodenum, struct Fdtable *fdtable)
{
    int ii;

    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        if (fdtable->table[ii].inode_num == inodenum)
        {
            return 1;
        }
    }

    return -1;
}

int _fdtable_print(struct Fdtable *fdtable)
{
    int ii;

    PRINT ("%*s    |%*s    |%*s    |%*s     \n",5, "used", 5, "fd", 5, "inode", 5, "offset");
    PRINT ("---------------------------------------\n");

    for (ii = 0; ii < fdtable->t_size; ii++)
    {
        PRINT ("%*d    |%*d    |%*d    |%*d     \n", 5, fdtable->table[ii].being_used, 5, fdtable->table[ii].fd, 5, fdtable->table[ii].inode_num, 5, fdtable->table[ii].offset);
    }

    return 0;
}

/*METHODS FOR ARRAY OF FD TABLES*/

int fdtable_a_initialize(struct FdtableArray *fdtablea)
{
    int ii;

    fdtablea->a_size = NUM_TABLES;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        fdtable_initialize(-1, (struct Fdtable*)&(fdtablea->array[ii]));
    }

    return 0;
}
int fdtable_a_createtable(int pid, struct FdtableArray *fdtablea)
{
    int ii;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtablea->array[ii].pid == -1)
        {
            fdtablea->array[ii].pid = pid;
            return 0;
        }
    }

    return -1;
}
int fdtable_a_createentry(int pid, int inodenum, struct FdtableArray *fdtablea)
{
    int ii;
    int retval;

    // Try to find the table
    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtablea->array[ii].pid == pid)
        {
            return fdtable_createentry(inodenum, (struct Fdtable*)&(fdtablea->array[ii]));
        }
    }

    // If you couldnt' fint an existing table, then create a new one
    retval = fdtable_a_createtable(pid, fdtablea);

    if (retval < 0) return -1;

    // Create entry inside that new table you just created
    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtablea->array[ii].pid == pid)
        {
            return fdtable_createentry(inodenum, (struct Fdtable*)&(fdtablea->array[ii]));
        }
    }

    return -1;
}
int fdtable_a_seekwithfd(int pid, int fd, int offset, struct FdtableArray *fdtablea)
{
    int ii;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtablea->array[ii].pid == pid)
        {
            return fdtable_seekwithfd(fd, offset, (struct Fdtable*)&(fdtablea->array[ii]));
        }
    }

    return -1;
}
int fdtable_a_seekwithinodenum(int pid, int inodenum, int offset, struct FdtableArray *fdtablea)
{
    int ii;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtablea->array[ii].pid == pid)
        {
            return fdtable_seekwithinodenum(inodenum, offset, (struct Fdtable*)&(fdtablea->array[ii]));
        }
    }

    return -1;
}
int fdtable_a_removeatfd(int pid, int fd, struct FdtableArray *fdtablea)
{
    int ii;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtablea->array[ii].pid == pid)
        {
            return fdtable_removeatfd(fd, (struct Fdtable*)&(fdtablea->array[ii]));
        }
    }

    return -1;
}
int fdtable_a_removeatinodenum(int pid, int inodenum, struct FdtableArray *fdtablea)
{
    int ii;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtablea->array[ii].pid == pid)
        {
            return fdtable_removeatinodenum(inodenum, (struct Fdtable*)&(fdtablea->array[ii]));
        }
    }

    return -1;
}

int fdtable_a_inodeforfd(int pid, int fd, struct FdtableArray *fdtablea)
{
    int ii;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtablea->array[ii].pid == pid)
        {
            return fdtable_inodeforfd(fd, (struct Fdtable*)&(fdtablea->array[ii]));
        }
    }

    return -1;
}

int fdtable_a_positionforfd(int pid, int fd, struct FdtableArray *fdtablea)
{
    int ii;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtablea->array[ii].pid == pid)
        {
            return fdtable_positionforfd(fd, (struct Fdtable*)&(fdtablea->array[ii]));
        }
    }

    return -1;
}

int fdtable_a_checkforinode(int pid, int inodenum, struct FdtableArray *fdtablea)
{
    int ii;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        if (fdtable_checkforinode(inodenum, (struct Fdtable*)&(fdtablea->array[ii])) == 1)
        {
            return 1;
        }
    }

    return -1;
}

int _fdtable_a_print(struct FdtableArray *fdtablea)
{
    int ii;

    for (ii = 0; ii < fdtablea->a_size; ii++)
    {
        PRINT("Table For Process: %d\n", fdtablea->array[ii].pid);
        _fdtable_print((struct Fdtable*)&(fdtablea->array[ii]));
        PRINT("\n");
    }
    return 0;
}

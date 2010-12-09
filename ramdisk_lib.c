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

ramdisk_lib.c
 user level library to interface with RAM disk
*/

#include "ramdisk_lib.h"

int ioctl_fd;

/*
 *  @TODO: Pass back appropriate return values
 */

int rd_init()
{
    ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    ioctl(ioctl_fd, RD_INIT, 0);
    return 0;
}

int rd_creat    (char *pathname)
{
    struct pathname_args_t args;

    if (!ioctl_fd)
        ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    args.pathname = pathname;
    args.str_len = strlen(pathname) + 1;

    ioctl (ioctl_fd, RD_CREAT, &args);

    return args.ret_val;
}


int rd_mkdir    (char *pathname)
{
    struct pathname_args_t args;

    if (!ioctl_fd)
        ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    args.pathname = pathname;
    args.str_len = strlen(pathname) + 1;

    ioctl (ioctl_fd, RD_MKDIR, &args);

    return args.ret_val;
}

int rd_open     (char *pathname)
{
    struct pathname_args_t args;

    if (!ioctl_fd)
        ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    args.pathname = pathname;
    args.str_len = strlen(pathname) + 1;

    ioctl (ioctl_fd, RD_OPEN, &args);

    return args.ret_val;
}

int rd_close    (int fd)
{
    struct close_args_t args;

    if (!ioctl_fd)
        ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    args.fd = fd;

    ioctl (ioctl_fd, RD_CLOSE, &args);

    return args.ret_val;
}

int rd_read     (int fd, char* address, int num_bytes)
{
    struct read_write_args_t args;

    if (!ioctl_fd)
        ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    args.fd = fd;
    args.address = address;
    args.num_bytes = num_bytes;

    ioctl (ioctl_fd, RD_READ, &args);

    return args.ret_val;
}

int rd_write    (int fd, char* address, int num_bytes)
{
    struct read_write_args_t args;

    if (!ioctl_fd)
        ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    args.fd = fd;
    args.address = address;
    args.num_bytes = num_bytes;

    ioctl (ioctl_fd, RD_WRITE, &args);

    return args.ret_val;
}

int rd_seek     (int fd, int offset)
{
    struct seek_args_t args;

    if (!ioctl_fd)
        ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    args.fd = fd;
    args.offset = offset;

    ioctl (ioctl_fd, RD_SEEK, &args);

    return args.ret_val;
}

int rd_unlink   (char *pathname)
{
    struct pathname_args_t args;

    if (!ioctl_fd)
        ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    args.pathname = pathname;
    args.str_len = strlen(pathname) + 1;

    ioctl (ioctl_fd, RD_UNLINK, &args);

    return args.ret_val;
}

int rd_readdir  (int fd, char *address)
{
    struct read_write_args_t args;

    if (!ioctl_fd)
        ioctl_fd = open (PROC_ENTRY, O_RDONLY);

    args.fd = fd;
    args.address = address;

    ioctl (ioctl_fd, RD_READDIR, &args);

    return args.ret_val;
}

/*
int main() {

	char a = 'a';
	rd_read(3, &a, 12);

	rd_close(10);

	rd_write(5, &a, 10);
	rd_seek(6, 14);
	rd_readdir(12, &a);
}
*/

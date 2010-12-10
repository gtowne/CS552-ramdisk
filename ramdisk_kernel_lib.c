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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>

#include "ramdisk_kernel_lib.h"

#include "ramdisk.h"

MODULE_LICENSE("GPL");

struct semaphore mutex;

static struct file_operations pseudo_dev_proc_operations;

static struct proc_dir_entry *proc_entry;

int handle_test(unsigned long ioctl_arg)
{
    struct test_args_t test_args;
    if(0 != copy_from_user(&test_args, (struct test_args_t *)ioctl_arg,
			   sizeof(struct test_args_t)))
    {
      printk("Error copying test args from user\n");
      return -1;
    }

    test_args.ret_val = rd_test(&(test_args.inodes), &(test_args.blocks));
    

    if (0 != copy_to_user((void*) ioctl_arg, &test_args, sizeof(struct test_args_t)))
    {
        printk("Error copyting test return value to user level\n");
        return -1;
    }

    printk("RD_CREAT SUCCESSFUL\n");
    return 1;
}

int handle_init(void)
{
    rd_init();
    return 0;
}

int handle_creat_call(unsigned long ioctl_arg)
{

    struct pathname_args_t creat_args;
    char buff[1024];
    //char buff[creat_args.str_len];
    printk("RD_CREAT ioctl\n");

    if (0 != copy_from_user(&creat_args, (struct pathname_args_t *)ioctl_arg,
                            sizeof(struct pathname_args_t)))
    {
        printk("Error copying creat args from user\n");
        return -1;
    }


    if (0 != copy_from_user(&buff, creat_args.pathname, creat_args.str_len))
    {
        printk("Error copying pathname from user\n");
        return -1;
    }

#ifdef DEBUG
    printk("Received creat call with arg %x\n", buff);
#endif

    //	UNCOMMENT TO PASS TO RAMDISK
    //
    creat_args.ret_val = rd_creat(buff);

    if (0 != copy_to_user((void*) ioctl_arg, &creat_args, sizeof(struct pathname_args_t)))
    {
        printk("Error copyting creat return value to user level\n");
        return -1;
    }

    printk("RD_CREAT SUCCESSFUL\n");
    return 1;
}

int handle_mkdir_call(unsigned long ioctl_arg)
{

    struct pathname_args_t mkdir_args;
    char buff[1024];

    if (0 != copy_from_user(&mkdir_args, (struct pathname_args_t *)ioctl_arg,
                            sizeof(struct pathname_args_t)))
    {
        printk("Error copying creat args from user\n");
        return -1;
    }

    //char buff[mkdir_args.str_len];

    if (0 != copy_from_user(&buff, mkdir_args.pathname, mkdir_args.str_len))
    {
        printk("Error copying pathname from user\n");
        return -1;
    }

#ifdef DEBUG
    printk("Received mkdir call with arg %x\n", buff);
#endif

    //	UNCOMMENT TO PASS TO RAMDISK
    //
    mkdir_args.ret_val = rd_mkdir(buff);

    if (0!= copy_to_user((void*) ioctl_arg, (void*) &mkdir_args, sizeof(struct pathname_args_t)))
    {
        printk("Error copyting mkdir return value to user level\n");
        return -1;
    }

    return 1;
}

int handle_open_call(unsigned long ioctl_arg)
{

    struct pathname_args_t open_args;
    char buff[1024];

    if (0 != copy_from_user(&open_args, (struct pathname_args_t *)ioctl_arg,
                            sizeof(struct pathname_args_t)))
    {
        printk("Error copying creat args from user\n");
        return -1;
    }

    //char buff[open_args.str_len];

    if (0 != copy_from_user(&buff, open_args.pathname, open_args.str_len))
    {
        printk("Error copying pathname from user\n");
        return -1;
    }

#ifdef DEBUG
    printk("Received open call with arg %x\n", buff);
#endif

    //	UNCOMMENT TO PASS TO RAMDISK
    //
    open_args.ret_val = rd_open(buff);

    if (0 != copy_to_user((void*) ioctl_arg, (void*) &open_args, sizeof(struct pathname_args_t)))
    {
        printk("Error copying open return value to user level\n");
        return -1;
    }

    return 1;
}

int handle_close_call(unsigned long ioctl_arg)
{

    struct close_args_t close_args;
    int fd;

    if (0 != copy_from_user(&close_args, (struct close_args_t *)ioctl_arg,
                            sizeof(struct close_args_t)))
    {
        printk("Error copying creat args from user\n");
        return -1;
    }

    fd = close_args.fd;

#ifdef DEBUG
    printk("Received close call with arg %i\n", fd);
#endif

    //	UNCOMMENT TO PASS TO RAMDISK
    //
    close_args.ret_val = rd_close(fd);

    if (0 != copy_to_user((void*) ioctl_arg, (void*) &close_args, sizeof(struct close_args_t)))
    {
        printk("Error copyting close return value to user level\n");
        return -1;
    }

    return 1;
}

int handle_read_call(unsigned long ioctl_arg)
{

    struct read_write_args_t read_args;
    int fd;
    char* address;
    int num_bytes;

#
    if (0 != copy_from_user(&read_args, (struct read_write_args_t *)ioctl_arg,
                            sizeof(struct read_write_args_t)))
    {
        printk("Error copying creat args from user\n");
        return -1;
    }

    fd = read_args.fd;
    address = read_args.address;
    num_bytes = read_args.num_bytes;

#ifdef DEBUG
    printk("Received read call with args %i, %i, %i\n", fd, (int) address, num_bytes);
#endif

    //	UNCOMMENT TO PASS TO RAMDISK
    //
    read_args.ret_val = rd_read(fd, address, num_bytes);

    if (0 != copy_to_user((void*) ioctl_arg, (void*) &read_args, sizeof(struct read_write_args_t)))
    {
        printk("Error copyting read return value to user level\n");
        return -1;
    }

    return 1;
}

int handle_write_call(unsigned long ioctl_arg)
{

    struct read_write_args_t write_args;
    int fd;
    char* address;
    int num_bytes;

    if (0 != copy_from_user(&write_args, (struct read_write_args_t *)ioctl_arg,
                            sizeof(struct read_write_args_t)))
    {
        printk("Error copying creat args from user\n");
        return -1;
    }

    fd = write_args.fd;
    address = write_args.address;
    num_bytes = write_args.num_bytes;

#ifdef DEBUG
    printk("Received write call with args %i, %i, %i\n", fd, (int) address, num_bytes);
#endif

    //	UNCOMMENT TO PASS TO RAMDISK
    //
    write_args.ret_val = rd_write(fd, address, num_bytes);

    if (0 != copy_to_user((void*) ioctl_arg, (void*) &write_args, sizeof(struct read_write_args_t)))
    {
        printk("Error copyting write return value to user level\n");
        return -1;
    }

    return 1;
}

int handle_seek_call(unsigned long ioctl_arg)
{

    struct seek_args_t seek_args;
    int fd;
    int offset;

    if (0 != copy_from_user(&seek_args, (struct seek_args_t *)ioctl_arg,
                            sizeof(struct seek_args_t)))
    {
        printk("Error copying creat args from user\n");
        return -1;
    }

    fd = seek_args.fd;
    offset = seek_args.offset;

#ifdef DEBUG
    printk("Received seek call with args %i, %i\n", fd, offset);
#endif

    //	UNCOMMENT TO PASS TO RAMDISK
    //
    seek_args.ret_val = rd_seek(fd, offset);

    if (0 != copy_to_user((void*) ioctl_arg, (void*) &seek_args, sizeof(struct seek_args_t)))
    {
        printk("Error copying seek return value to user level\n");
        return -1;
    }

    return 1;
}

int handle_readdir_call(unsigned long ioctl_arg)
{

    struct read_write_args_t readdir_args;
    int fd;
    char* address;

    if (0 != copy_from_user(&readdir_args, (struct read_write_args_t *)ioctl_arg,
                            sizeof(struct read_write_args_t)))
    {
        printk("Error copying readdir args from user\n");
        return -1;
    }

    fd = readdir_args.fd;
    address = readdir_args.address;

#ifdef DEBUG
    printk("Received readdir call with args %i, %i\n", fd, (int) address);
#endif

    //	UNCOMMENT TO PASS TO RAMDISK
    //
    readdir_args.ret_val = rd_readdir(fd, address);

    if (0 != copy_to_user((void*) ioctl_arg, (void*) &readdir_args, sizeof(struct read_write_args_t)))
    {
        printk("Error copying readdir return value to user level\n");
        return -1;
    }

    return 1;
}

int handle_unlink_call(unsigned long ioctl_arg)
{

    struct pathname_args_t unlink_args;
    char buff[1024];

    if (0 != copy_from_user(&unlink_args, (struct pathname_args_t *)ioctl_arg,
                            sizeof(struct pathname_args_t)))
    {
        printk("Error copying creat args from user\n");
        return -1;
    }

    //char buff[unlink_args.str_len];

    if (0 != copy_from_user(&buff, unlink_args.pathname, unlink_args.str_len))
    {
        printk("Error copying pathname from user\n");
        return -1;
    }

#ifdef DEBUG
    printk("Received unlink call with args %s\n", buff);
#endif

    //	UNCOMMENT TO PASS TO RAMDISK
    //
    unlink_args.ret_val = rd_unlink(buff);

    if (0 != copy_to_user((void*) ioctl_arg, (void*) &unlink_args, sizeof(struct pathname_args_t)))
    {
        printk("Error copyting unlink return value to user level\n");
        return -1;
    }

    return 1;
}

static int pseudo_device_ioctl(struct inode *inode, struct file *file,
                               unsigned int cmd, unsigned long arg)
{

    switch (cmd)
    {

    case RD_INIT:
        down_interruptible(&mutex);
        handle_init();
        up(&mutex);
        break;
    case RD_CREAT:
        down_interruptible(&mutex);
        handle_creat_call(arg);
        up(&mutex);
        break;

    case RD_MKDIR:
        down_interruptible(&mutex);
        handle_mkdir_call(arg);
        up(&mutex);
        break;

    case RD_OPEN:
        down_interruptible(&mutex);
        handle_open_call(arg);
        up(&mutex);
        break;

    case RD_CLOSE:
        down_interruptible(&mutex);
        handle_close_call(arg);
        up(&mutex);
        break;

    case RD_READ:
        down_interruptible(&mutex);
        handle_read_call(arg);
        up(&mutex);
        break;

    case RD_WRITE:
        down_interruptible(&mutex);
        handle_write_call(arg);
        up(&mutex);
        break;

    case RD_SEEK:
        down_interruptible(&mutex);
        handle_seek_call(arg);
        up(&mutex);
        break;

    case RD_READDIR:
        down_interruptible(&mutex);
        handle_readdir_call(arg);
        up(&mutex);
        break;

    case RD_UNLINK:
        down_interruptible(&mutex);
        handle_unlink_call(arg);
        up(&mutex);
        break;

    case RD_TEST:
        down_interruptible(&mutex);
        handle_test(arg);
        up(&mutex);        

    default:
        return -EINVAL;
        break;
    }

    return 0;
}

static int __init initialization_routine(void)
{
    printk("<1> Loading ramdisk_kernel_lib module\n");

    pseudo_dev_proc_operations.ioctl = pseudo_device_ioctl;

    /* Start create proc entry */
    proc_entry = create_proc_entry(PROC_MODULE_NAME, 0444, &proc_root);
    if(!proc_entry)
    {
        printk("<1> Error creating /proc entry.\n");
        return 1;
    }

    proc_entry->owner = THIS_MODULE;
    proc_entry->proc_fops = &pseudo_dev_proc_operations;

    sema_init(&mutex, 1);

    RAMDISK = NULL;

    return 0;
}


static void __exit cleanup_routine(void)
{

    printk("<1> Dumping ramdisk_kernel_lib module\n");
    remove_proc_entry(PROC_MODULE_NAME, &proc_root);
    //@TODO: Deallocate RAMDISK
    vfree(RAMDISK);

    return;
}


module_init(initialization_routine);
module_exit(cleanup_routine);

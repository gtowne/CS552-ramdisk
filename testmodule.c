#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>

#include "string_utils.h"

static int __init initialization_routine(void)
{
    printk("<1> Loading ramdisk_kernel_lib module\n");
    char* hello = "Hello\n";
    str_len(hello, 256);

    /*
    pseudo_dev_proc_operations.ioctl = pseudo_device_ioctl;

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
    */
    return 0;
}


static void __exit cleanup_routine(void)
{

    printk("<1> Dumping ramdisk_kernel_lib module\n");
    /*
      remove_proc_entry(PROC_MODULE_NAME, &proc_root);
            //@TODO: Deallocate RAMDISK
    */
    return;
}


module_init(initialization_routine);
module_exit(cleanup_routine);

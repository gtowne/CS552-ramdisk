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

MODULE_LICENSE("GPL");

static struct file_operations pseudo_dev_proc_operations;

static struct proc_dir_entry *proc_entry;

int handle_creat_call(unsigned long ioctl_arg) {
	struct pathname_args_t creat_args;

	copy_from_user(&creat_args, (struct pathname_args_t *)ioctl_arg, 
		   sizeof(struct pathname_args_t));
		   
	char buff[creat_args.str_len];
	
	copy_from_user(&buff, creat_args.pathname, creat_args.str_len);
	
	printk(buff);

	return 1;
}

static int pseudo_device_ioctl(struct inode *inode, struct file *file,
				unsigned int cmd, unsigned long arg)
{
  
  switch (cmd){

  case RD_CREAT:
    
    break;
    
  case RD_MKDIR:
  
  	break;
  	
  case RD_OPEN:
  
  	break;
  	
  case RD_CLOSE:
  
  	break;
  	
  case RD_READ:
  
  	break;
  	
  case RD_WRITE:
  
  	break;
  	
  case RD_SEEK:
  
  	break;
  	
  case RD_READDIR:
  
  	break;
  
  default:
    return -EINVAL;
    break;
  }
  
  return 0;
}

static int __init initialization_routine(void) {
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
	
	return 0;
}


static void __exit cleanup_routine(void) {
	
	printk("<1> Dumping ramdisk_kernel_lib module\n");
	remove_proc_entry("ioctl_test", &proc_root);
	
	return;
}


module_init(initialization_routine); 
module_exit(cleanup_routine); 

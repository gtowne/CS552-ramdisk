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
 Some convenience functions for handling strings

 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#ifndef NULL
#define NULL 0x0
#endif

#ifndef USE_PTHREADS
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>
#endif

// copy string from src to dst buffer, max_chars is
// maximum number of characters to copy INCLUDING null
// terminator
// returns 1 if successful
int str_copy(char* src, char* dst, int max_chars);

// returns 1 iff str1 and str2 are character-wise
// equivalent, max_chars is maximum number of characters
// to consider INCLUDING null terminator
// returns -1 otherwise
// note - strings considered equal if first max_chars are
// equal
int str_equals(char* str1, char* str2, int max_chars);

int str_len(char* str, int max_chars);

int str_next_token(char* str, int start_pos, int max_chars, char delim);

int str_last_delimiter(char* str, int last_pos, char delim);


// returns the number of "/" in the pathname...not sure if this
// is useful

int str_path_number(char* pathname);

#endif

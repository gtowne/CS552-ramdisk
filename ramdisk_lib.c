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

int rd_creat    (char *pathname) {
	struct pathname_args_t args;
  	int fd = open (PROC_ENTRY, O_RDONLY);

 	args.pathname = pathname;
 	args.str_len = strlen(pathname);

  	ioctl (fd, RD_CREAT, &args);
	return 1;
}


int rd_mkdir    (char *pathname) {
	return 1;
}

int rd_open     (char *pathname) {
	return 1;
}

int rd_close    (int fd) {
	return 1;
}

int rd_read     (int fd, char* address, int num_bytes) {
	return 1;
}

int rd_write    (int fd, char* address, int num_bytes) {
	return 1;
}

int rd_seek     (int fd, int offset) {
	return 1;
}

int rd_unlink   (char *pathname) {
	return 1;
}

int rd_readdir  (int fd, char *address) {
	return 1;
}


int main() {
	rd_creat("Impassing/in/this/path/name");

}


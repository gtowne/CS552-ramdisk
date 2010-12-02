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

#include "string_utils.h"
#include <stdio.h>


int str_copy(char* src, char* dst, int max_chars) {
	int i;
	
	for (i = 0; i < max_chars; i++) {
		dst[i] = src[i];
		
		if (src[i] == NULL) {
			break;
		}
	}
	
	dst[max_chars - 1] = NULL;
	
	return 1;
}

int str_equals(char* str1, char* str2, int max_chars) {
	int i;
	
	for (i = 0; i < max_chars; i++) {
		if (str1[i] != str2[i]) {
			return -1;
		}
		
		if (str1[i] == NULL) { // must both be null if we've gotten here
			break;
		}
	}
	
	return 1;
}

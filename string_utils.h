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

#define NULL 0x0

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

#endif

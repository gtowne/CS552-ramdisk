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


int str_copy(char* src, char* dst, int max_chars)
{
    int i;

    for (i = 0; i < max_chars; i++)
    {
        dst[i] = src[i];

        if (src[i] == '\0')
        {
            break;
        }
    }

    dst[max_chars - 1] = '\0';

    return 1;
}

int str_equals(char* str1, char* str2, int max_chars)
{
    int i;

    for (i = 0; i < max_chars; i++)
    {
        if (str1[i] != str2[i])
        {
            return -1;
        }

        if (str1[i] == '\0')   // must both be null if we've gotten here
        {
            break;
        }
    }

    return 1;
}

int str_len(char* str, int max_chars)
{
    int i;
    if(str == NULL)
    {
        return 0;
    }

    for (i = 0; i < max_chars; i++)
    {
        if(str[i]=='\0')
        {
            return i;
        }
    }
    return -1;
}

int str_next_token(char* str, int start_pos, int max_chars, char delim)
{
    int i;
    for(i=start_pos; i<max_chars; i++)
    {
        if(str[i] == delim || str[i] == '\0')
        {
            return i;
        }
    }
    return max_chars;
}

int str_last_delimiter(char* str, int last_pos, char delim)
{
    int i;
    for(i=last_pos; i>=0; i--)
    {
        if(str[i] == delim)
        {
            return i;
        }
    }
    return -1;
}

int str_path_number(char* pathname)
{
    int count = 0;
    int index = 0;

    while (1==1)
    {
        if (pathname[index] == *("\0"))
        {
            break;
        }

        if (pathname[index] == *("/"))
        {
            count ++;
        }

        index ++;
    }

    return count;
}

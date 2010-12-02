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

BITMAP:
This implements the bitmap part of the assignment

*/

#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

#define BITMAP_NUM_BYTES 	1024
#define BITMAP_NUM_BITS		8192

int bitmap_initialize(struct Bitmap *bitmap)
{
	int ii;

	// Initialize control variables
	bitmap->size 			= BITMAP_NUM_BITS;
	bitmap->num_empty	 	= BITMAP_NUM_BITS;

	// Initialize all bits to zero
	for (ii = 0; ii < BITMAP_NUM_BYTES; ii++)
	{
		_bitmap_setalltozero(ii,bitmap);
	}

	return 0;
}

int bitmap_setatindex(int index, struct Bitmap *bitmap)
{
	if (index < 0 || index > BITMAP_NUM_BITS-1)
	{
		printf("Bitmap:: setatindex: index out of range\n");
		return -1;
	}
	
	int char_index 	= index / 8;
	int shift		= index % 8;
	
	if (_bitmap_isset(char_index, shift, bitmap))
	{
		#ifdef DEBUG
		printf("Bitmap:: could not set bit at: %d, bit already set\n",index);
		#endif
		return -1;
	}
	
	#ifdef DEBUG
	printf("Bitmap:: setatindex: %d char_index = %d  shift = %d\n",index, char_index, shift);
	#endif

	_bitmap_set(char_index, shift, bitmap);
	bitmap->num_empty--;

	return 0;
}

int bitmap_removeatindex(int index, struct Bitmap *bitmap)
{
	if (index < 0 || index > BITMAP_NUM_BITS-1)
	{
		printf("Bitmap:: removeatindex: index out of range\n");
		return -1;
	}

	int char_index 	= index / 8;
	int shift		= index % 8;

	#ifdef DEBUG
	printf("Bitmap:: removeatindex: %d char_index = %d  shift = %d\n",index, char_index, shift);
	#endif

	_bitmap_unset(char_index, shift, bitmap);
	bitmap->num_empty++;
	
	return 0;
}

int bitmap_findemptyblockofsize	(int size, struct Bitmap* bitmap)
{
	if (bitmap->num_empty == 0)
	{
		printf("Bitmap:: findemptyblockofsize: no more empty blocks\n");
		return -1;
	}
	
	//TODO: Decide if we want to start at 0...at random....or keep the last allocated block index
	int index = 0;//random(BITMAP_NUM_BYTES);
	int traveled = 1;
	int consecutive = 0;
	int start = index;
	int first = 1;

	while (1==1)
	{	
		// Handle wrap around case
		if (index == BITMAP_NUM_BITS - 1)
		{
			index = 0;
		}

		// Count number of cinsecutive zeros
		if (!_bitmap_isset_b(index, bitmap))
		{
			consecutive ++;
			if (first)
			{
				start = index;
				first = 0;
			}
		}
		else
		{
			consecutive = 0;
			first = 1;
		}

		// We found the block size, return the start
		if (consecutive == size)
		{
			printf("Bitmap:: findemptyblockofsize: Found block of size: %d at index: %d\n", size, start);
			return start;
		}

		// Traveled the whole thing and did not find anything
		if (traveled == BITMAP_NUM_BITS)
		{
			printf("Bitmap:: findemptyblockofsize: no empty blocks of that size\n");
			return -1;
		}
		traveled++;
		index++;
	}

	return -1;
}

int bitmap_setblockofsize(int size, struct Bitmap* bitmap)
{
	if (bitmap->num_empty == 0)
	{
		printf("Bitmap:: setblockofsize: no more empty blocks\n");
		return -1;
	}

	int index = bitmap_findemptyblockofsize(size, bitmap);
	int start = index;
	int count = 0;
	
	while (1==1)
	{
		// Handle wrap around case
		if (index == BITMAP_NUM_BITS - 1)
		{
			index = 0;
		}

		bitmap_setatindex(index, bitmap);
		
		if (count == size)
		{
			printf("Bitmap:: setblockofsize: Set block of size: %d at index: %d\n", size, start);
			bitmap->num_empty += size;
			return 0;
		}
		count ++;
		index ++;
	}
	
	return -1;
}

int bitmap_removeblockofsize	(int index, int size, struct Bitmap* bitmap)
{
	int start = index;
	int count = 0;
	
	while (1==1)
	{
		// Handle wrap around case
		if (index == BITMAP_NUM_BITS - 1)
		{
			index = 0;
		}

		bitmap_removeatindex(index, bitmap);
		
		if (count == size)
		{
			printf("Bitmap:: setblockofsize: Set block of size: %d at index: %d\n", size, start);
			bitmap->num_empty += size;
			return 0;
		}
		count ++;
		index ++;
	}
	
	return -1;
}



int _bitmap_isset(int block, int bit, struct Bitmap *bitmap)
{	
	return bitmap->array[block] & (1 << bit);
}

int _bitmap_isset_b(int bitnum, struct Bitmap *bitmap)
{
	int block 	= bitnum/8;
	int bit 		= bitnum%8;

	return _bitmap_isset(block, bit, bitmap);
}

int _bitmap_set(int block, int bit, struct Bitmap *bitmap)
{
	bitmap->array[block] = (char)(bitmap->array[block] | (1 << bit));

	return 0;
}

int _bitmap_unset(int block, int bit, struct Bitmap *bitmap)
{	
	bitmap->array[block] = (char)(bitmap->array[block] & ~(1 << bit));

	return 0;
}

int _bitmap_setalltozero(int block, struct Bitmap *bitmap)
{
	bitmap->array[block] = (char)~~0;
	
	return 0;
}




int _bitmap_printhchar(char a)
{
	int ii, kk, mask;

	for (ii = 7 ; ii >= 0 ; ii--)
	{
		mask = 1 << ii;
		kk = a & mask;
		if( kk == 0)
		{
			printf("0");
		}
		else
		{
			printf("1");
		}
	}

	return 0;
}

int _bitmap_print_nice(struct Bitmap *bitmap)
{
	int ii;
	
	for (ii = 0; ii < BITMAP_NUM_BYTES; ii++)
	{
		printf("%d: ", ii);
		
		_bitmap_printhchar(bitmap->array[ii]);
		printf("\n");

	}
    printf("\n");
	return 0;
}

int _bitmap_print_raw(struct Bitmap *bitmap)
{
	int ii;
	
	for (ii = 0; ii < BITMAP_NUM_BYTES; ii++)
	{		
		_bitmap_printhchar(bitmap->array[ii]);

		
		if (ii%13 == 0 && ii != 0)
		{
			printf("\n");
		}
	}
    printf("\n");
	return 0;
}

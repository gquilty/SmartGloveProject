/* S-LZW with Mini-Cache - Functions to perform the S-LZW compression algorithm presented in:
*
*  C. Sadler and M. Martonosi, “Data Compression Algorithms for Energy-Constrained Devices in Delay
*  Tolerant Networks,” Proceedings of the ACM Conference on Embedded Networked Sensor Systems
*  (SenSys) 2006, November 2006.
*
* Note:  I do not include the BWT transform code here because it is based off of code that I
* downloaded from http://www.dogma.net/markn/articles/bwt/bwt.htm and I have not attempted to get
* permission to post it.  If you want a copy, though, email me and I'll send you the original code
* and my amendments so you can see what I did.
*
* Copyright (C) 2005-2006 Princeton University, Christopher M. Sadler (csadler@princeton.edu)
*
*    This program is free software; you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation; either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License along
*    with this program; if not, write to the Free Software Foundation, Inc.,
*    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/***************************************************************************//**
 * Functions for lossless compression of data. This implements the LZW algorithm
 * as described in the paper:
 * @code
 * C.M. Sadler, and M. Martonosi, 2006. Data compression algorithms for energy-
 * constrained devices in delay tolerant networks. In Proc. 4th Int. Conf.
 * Embedded Networked Sensor Systems, 265-278. doi:10.1145/1182807.1182834
 * @endcode
 *
 * There are also functions for the BWT (Burrows-Wheeler Transform), which can
 * help acheive higher compression ratios at the expense of processing time.
 *
 * The original code is available from http://cmsadler.googlepages.com/ under
 * GPL license.
 *
 * The following changes have been made:
 * - Datatypes use fixed length values to avoid errors with different C
 *    compilers.
 * - Added BWT functions
 * - Changed slzw functions so that the data is passed in as an argument and is
 *    not required to be in a specially named extern compBuffer.
 *
 * @todo Fix comments in this file as they are no longer accurate.
 *
 * @file slzw.c
 * @date 18-Jan-2010
 * @author Chris Sadler, Princeton University
 * @author Stefano Rollo and Seán Harte, Tyndall National Institute
 ******************************************************************************/


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slzw.h"


// Technically, if the data is completely random, LZW can increase the size of the data.  If you
// have 10 bit entries (9 bit dictionary plus a 1 bit escape character for the mini-cache), the
// maximum size is (10/8)*size of the buffer.  We use a larger buffer to make sure we don't
// accidentally over run.
#define WRITE_BUFFER_SIZE			((SLZW_FLASH_PAGE_SIZE << 1) + (SLZW_FLASH_PAGE_SIZE >> 2))

#define LZW_BUFFER_SIZE				WRITE_BUFFER_SIZE

// Due to bit alignment issues on the MSP, I has to break part of the hash up into two parts.
//  These masks help the program figure out which part is which.
#define HASH1_MASK					0xFF00
#define HASH2_MASK					0x00FF

#define HIT							1
#define MISS						0

#define get_hash(x) ((x.next_hash1 << 8) + x.next_hash2)

#ifndef __DOXYGEN__
typedef struct dict_node {
	uint16_t longer_string:12;	// When we need to look for a longer string, we head to this index.
	uint16_t next_hash1:4;		// When we need to look for a different string entry of the same
	uint8_t next_hash2;		    //  length, we look here.
	uint8_t entry;
} Dict_node;

typedef struct undict_node {
	int16_t up;						// Signed with plenty of extra bits.
	uint8_t entry;
	uint8_t undefined;	// To make the structure an even number of bytes.
} Undict_node;
#endif

#define MAX(a, b) (((a)>(b))?(a):(b))


// Holds the dictionary (sizeof(Dict_node) = 4)
// must also be 2 * SLZW_BLOCK_SIZE long for BWT.
#define DICT_SIZE ((SLZW_MAX_DICT_ENTRIES+1)*sizeof(Dict_node))
#define BWT_INDICES (SLZW_BLOCK_SIZE * 2)
static uint8_t compression_chars[(SLZW_MAX_DICT_ENTRIES+1)*sizeof(Dict_node)];

// Holds the actual mini cache.
static uint16_t mini_cache_structure[SLZW_MAX_SIZE_OF_MINI_CACHE];



static void init_dictionary(void);
static void init_decomp_dictionary(void);

// Decompression buffer.
#ifdef BWT_USE
static uint16_t compBuffer[256];
#else
static uint8_t compBuffer[256];
#endif


#ifdef BWT_USE
static const uint8_t *circBuffer = NULL;
static uint16_t circBufferSize = 0;

static int circBufferCmp(const void *a, const void *b);


/**
 * Takes in two pointer that are pointing to locations within a single circular
 * buffer, and finds which points to the lowest value string. Can be used with
 * @c qsort().
 *
 * @param a first pointer.
 * @param b second pointer.
 *
 * @return @c 0 if the strings are the same, @c 1 if the string pointed to by
 *     @a a is greater than the string pointed to by @a b, and @c -1 otherwise.
 **/
static int circBufferCmp(const void *a, const void *b)
{
    uint8_t aPtr = *(const uint8_t*)a;
    uint8_t bPtr = *(const uint8_t*)b;
    uint16_t count = 0;
    
    /* Check if pointing to same location */
    if(aPtr == bPtr) {
        return 0;
    }
    
    /* Go through each element until a difference is found */
    while (circBuffer[aPtr] == circBuffer[bPtr]) {
        /* Increment pointers with wrap-around */
        if (++aPtr == circBufferSize) {
            aPtr = 0;
        }
        if (++bPtr == circBufferSize) {
            bPtr = 0;
        }
        
        if (++count == circBufferSize) {
            return 0;       /* Strings are the same */
        }
    }
    
    return (circBuffer[aPtr] > circBuffer[bPtr]) ? 1 : -1;
}


uint16_t bwt_encode(uint16_t size, const uint8_t *bufferIn, uint8_t *bufferOut)
{
    /* Share buffer with SLZW code to save space */
    uint16_t* indices = (uint16_t*)compression_chars;
    uint16_t i;

    /* Initialise indices */
    for(i = 0; i < size; ++i) {
        indices[i] = i;
    }
    
    /* Used in circBufferCmp() */
    circBuffer = bufferIn;
    circBufferSize = size;
    
    /*
     * Sort indices based on string comparison of different start points in
     * circular buffer
     */
    qsort(indices, size, sizeof(uint16_t), circBufferCmp);

    /* Create output data by moving bytes according to indices */
    for (i = 0; i < size; ++i) {
        bufferOut[i] = bufferIn[(indices[i] - 1 + size) % size];
        /* Add index showing starting byte, necessary for reverse BWT */
        if (indices[i] == 1) {
            bufferOut[size] = (uint8_t)(i >> 8);
            bufferOut[size+1] = (uint8_t)i;
        }
    }

    return size + 2;
}


uint16_t bwt_decode(uint16_t size, const uint8_t *bufferIn, uint8_t *bufferOut)
{
    uint16_t i, j;
    uint16_t primaryIndex;
    uint16_t sum = 0;
    uint16_t* indices = (uint16_t*)compression_chars;

    size -= 2;
    primaryIndex = (uint16_t)bufferIn[size] << 8;
    primaryIndex |= (uint16_t)bufferIn[size + 1];

    for (i = 0; i < size; i++) {
        compBuffer[i] = 0;
    }

    for (i = 0; i < size; i++) {
        indices[i] = compBuffer[bufferIn[i]];
        compBuffer[bufferIn[i]]++;
    }

    for (i = 0; i < 256; i++) {
        j = compBuffer[i];
        compBuffer[i] = sum;
        sum += j;
    }

    for (i = 0; i < size; i--) {
        bufferOut[size - i] = bufferIn[primaryIndex];
        primaryIndex = compBuffer[bufferIn[primaryIndex]] + indices[primaryIndex];
    }

    return size;
}
#endif


//************************************************************************//
//  slzw_compress_data                                 		  		      //
//************************************************************************//
//
//  Function Description:
//  Compress data with the S-LZW-MC compression technique.  This differs slightly from the
//	code I used in the paper since it assumes that the data block is in RAM rather than in flash.
//  Also, the code now embeds the size of the mini-cache in the first byte of the compressed stream
//  so that the decompressor does not need to know ahead of time.
//
//	This function looks for the input data in the variable "write_buffer".
//
//  Inputs:
//		- inSize - Size of the file to compress
//		- miniCacheSize - Number of entries in the mini-cache.  Irrelevant if SLZW_USE_MINICACHE is not
//				defined
//
//	Expected Inputs:
//		- The data should be in write_buffer
//
//  Outputs:
//		- Returns the compressed size
//		- Compressed data is in lzw_output_file_buffer
//
//  Revision History:
//		25 Oct 2006		Chris Sadler		Initial Release
//		10 Mar 2007		Dirk Buijsman		Patched bug related to adding the last dictionary
//											entry to the mini-cache
//		10 Mar 2007		Chris Sadler		If the post-compression data size is actually larger
//											than the pre-compression data size, this code now
//											reverts back to the pre-compression data and signifies
//											this by making the mini-cache size 0.
////////////////////////////////////////////////////////////////////////////
uint16_t slzw_compress(uint16_t inSize, uint8_t miniCacheSize,
                                 const uint8_t* inData, uint8_t* outData) {
	uint8_t time_to_add, last_char, new_char, alignment = 0, add_to_mc = 0;
	uint16_t size_of_entry = 9, i, current_dict_entry, temp_dict_entry,
	         lzw_output_file_counter, temp_hash, next_dict_entry;
	uint32_t temp_entry;
	Dict_node *current_node, *node;

	uint8_t mini_dict_hash = miniCacheSize - 1;
	uint8_t hit_bits;

	#ifdef SLZW_STRUCTURED_TRANSFORM
		uint16_t j;
	#endif

	node = (Dict_node *) compression_chars;

	init_dictionary();
	next_dict_entry = 256;

	hit_bits = 0;

	for (i = miniCacheSize; i != 0; i=i>>1) {
		hit_bits++;
	}

	#ifdef SLZW_STRUCTURED_TRANSFORM
		// There is probably a smarter way to do this, but this was fast enough for the block size.
		lzw_output_file_counter = 0;
		for (i=0; i<SLZW_SIZE_OF_READING; i++)
		{
			for (j=i; j < inSize; j+=SLZW_SIZE_OF_READING)
			{
				outData[lzw_output_file_counter++] = inData[j];
			}
		}

		memcpy(inData, outData, inSize);
	#endif

	// Get first byte and start i at 1
	last_char = inData[0];
	i = 1;
	time_to_add = 0;
	current_node = &(node[last_char]);
	current_dict_entry = last_char;

	memset(outData, 0x00, (inSize * 10 )/ 8);

	for (lzw_output_file_counter = 0; lzw_output_file_counter<miniCacheSize; lzw_output_file_counter++) {
		mini_cache_structure[lzw_output_file_counter] = lzw_output_file_counter;
	}

	outData[0] = miniCacheSize;
	lzw_output_file_counter = 1;

	for (; i < inSize; i++) {
		new_char = inData[i];

		if (current_node->longer_string == 0) { // No longer entries in dictionary
			// No strings of this length, so add a deeper entry to the dictionary
			#ifndef SLZW_RESETABLE_DICTIONARY
			if (next_dict_entry < SLZW_MAX_DICT_ENTRIES) {
			#endif
			current_node->longer_string = next_dict_entry;
			#ifndef SLZW_RESETABLE_DICTIONARY
			}
			#endif
			time_to_add = 1;
		}
		else {
			temp_dict_entry = current_node->longer_string;
			current_node = &(node[current_node->longer_string]);

			temp_hash = (current_node->next_hash1 << 8) + current_node->next_hash2;
			while ((current_node->entry != new_char) && (temp_hash != 0)) {
				current_node = &(node[temp_hash]);
				temp_dict_entry = temp_hash;
				temp_hash = (current_node->next_hash1 << 8) + current_node->next_hash2;
			}

			// Did we find the last letter in the string?
			if ((current_node->entry != new_char) && (temp_hash == 0)) {
				// No, so add a parallel entry to the dictionary
				#ifndef SLZW_RESETABLE_DICTIONARY
				if (next_dict_entry < SLZW_MAX_DICT_ENTRIES)
				{
				#endif
				current_node->next_hash1 = ((next_dict_entry) & HASH1_MASK) >> 8;
				current_node->next_hash2 = (next_dict_entry) & HASH2_MASK;
				#ifndef SLZW_RESETABLE_DICTIONARY
				}
				#endif
				time_to_add = 1;
			}
			else {
				// We did find the entry in the list, so now we have to go back to the
				// start to see if there is a longer string that works too
				time_to_add = 0;
				current_dict_entry = temp_dict_entry;
			}
		}

		if (time_to_add == 1) {
			time_to_add = 0;

			// Add to dictionary
			#ifndef SLZW_RESETABLE_DICTIONARY
			if (next_dict_entry < SLZW_MAX_DICT_ENTRIES) {
			#endif
			node[next_dict_entry].longer_string = 0;
			node[next_dict_entry].next_hash1 = 0;
			node[next_dict_entry].next_hash2 = 0;
			node[next_dict_entry++].entry = new_char;
			add_to_mc = 1;
			#ifndef SLZW_RESETABLE_DICTIONARY
			}
			#endif

			#ifdef SLZW_USE_MINICACHE
				if (mini_cache_structure[current_dict_entry & mini_dict_hash] == current_dict_entry) {
					// SUBLIST HIT
					// Part of one byte left or a while byte plus another part of one byte left?
					if (hit_bits > (8 - alignment)) {
						temp_entry = (((current_dict_entry & mini_dict_hash) << 1) | 0x01) << alignment;

						outData[lzw_output_file_counter++] += temp_entry & 0xFF;

						temp_entry = temp_entry >> 8;
						outData[lzw_output_file_counter] = temp_entry & 0xFF;
					}
					else {
						temp_entry = (((current_dict_entry & mini_dict_hash) << 1) | 0x01) << alignment;
						outData[lzw_output_file_counter] += temp_entry & 0xFF;
					}

					alignment += hit_bits;
				}
				else {
					// SUBLIST MISS
					// Add to output buffer
					temp_entry = current_dict_entry << (alignment+1);

					outData[lzw_output_file_counter++] += temp_entry & 0xFF;

					// Part of one byte left or a while byte plus another part of one byte left?
					if (size_of_entry + 1 - (8 - alignment) > 8) {
						temp_entry = current_dict_entry >> (7-alignment);	  // 7=>8-1 from miss tag
						outData[lzw_output_file_counter++] = temp_entry & 0xFF;

						temp_entry = temp_entry >> 8;
					}
					else {
						temp_entry = current_dict_entry >> (7-alignment);
					}

					outData[lzw_output_file_counter] = temp_entry & 0xFF;
					mini_cache_structure[current_dict_entry & mini_dict_hash] = current_dict_entry;
					alignment += size_of_entry+1;
				}

				// Add the newest dictionary entry to the list.
				#ifdef SLZW_ADD_NEWEST
					#ifndef SLZW_RESETABLE_DICTIONARY
						if (add_to_mc) {
					#endif
							mini_cache_structure[(next_dict_entry - 1) & mini_dict_hash] = next_dict_entry - 1;
					#ifndef SLZW_RESETABLE_DICTIONARY
							add_to_mc = 0;
						}
					#endif
				#endif

			#else	// If the program is not using the minicache.
				// Add to output buffer.
				temp_entry = current_dict_entry << alignment;
				outData[lzw_output_file_counter++] += temp_entry & 0xFF;

				// Part of one byte left or a while byte plus another part of one byte left?
				if (size_of_entry - (8 - alignment) > 8) {
					temp_entry = current_dict_entry >> (8-alignment);
					outData[lzw_output_file_counter++] = temp_entry & 0xFF;

					temp_entry = temp_entry >> 8;
				}
				else {
					temp_entry = current_dict_entry >> (8-alignment);
				}

				outData[lzw_output_file_counter] = temp_entry & 0xFF;
				alignment += size_of_entry;
			#endif

			while (alignment >= 8) {
				alignment -= 8;
			}

			if (alignment == 0) {
				lzw_output_file_counter++;
			}

			// Reset variables
			current_node = &(node[new_char]);
			current_dict_entry = new_char;

			if (((next_dict_entry - 1) >> size_of_entry) != 0) {
				size_of_entry++;

				#ifdef SLZW_RESETABLE_DICTIONARY
					if (size_of_entry > SLZW_MAX_ENTRY_BITS)
					{
						size_of_entry = 9;
						init_dictionary();
						next_dict_entry = 256;
					}
				#endif
			}
		}

		last_char = new_char;
	}

	// Add last string to output buffer.  This is pretty much identical to the block of code above,
	// so I really should make this into a function.
	#ifdef SLZW_USE_MINICACHE
		if (mini_cache_structure[current_dict_entry & mini_dict_hash] == current_dict_entry) {
			// SUBLIST HIT
			// Part of one byte left or a while byte plus another part of one byte left?
			if (hit_bits > (8 - alignment)) {
				temp_entry = (((current_dict_entry & mini_dict_hash) << 1) | 0x01) << alignment;
				outData[lzw_output_file_counter++] += temp_entry & 0xFF;

				temp_entry = temp_entry >> 8;
				outData[lzw_output_file_counter] = temp_entry & 0xFF;
			}
			else {
				temp_entry = (((current_dict_entry & mini_dict_hash) << 1) | 0x01) << alignment;
				outData[lzw_output_file_counter] += temp_entry & 0xFF;
			}
		}
		else {
			// SUBLIST MISS
			// Add to output buffer
			temp_entry = current_dict_entry << (alignment+1);
			outData[lzw_output_file_counter++] += temp_entry & 0xFF;

			// Part of one byte left or a while byte plus another part of one byte left?
			if (size_of_entry + 1 - (8 - alignment) > 8) {
				temp_entry = current_dict_entry >> (7-alignment);	  // 7=>8-1 from miss tag
				outData[lzw_output_file_counter++] = temp_entry & 0xFF;

				temp_entry = temp_entry >> 8;
			}
			else {
				temp_entry = current_dict_entry >> (7-alignment);
			}

			outData[lzw_output_file_counter] = temp_entry & 0xFF;
		}
	#else
		temp_entry = current_dict_entry << alignment;
		outData[lzw_output_file_counter++] += temp_entry & 0xFF;

		// Part of one byte left or a while byte plus another part of one byte left?
		if (size_of_entry - (8 - alignment) > 8) {
			temp_entry = current_dict_entry >> (8-alignment);
			outData[lzw_output_file_counter++] = temp_entry & 0xFF;

			temp_entry = temp_entry >> 8;
		}
		else {
			temp_entry = current_dict_entry >> (8-alignment);
		}

		outData[lzw_output_file_counter] = temp_entry & 0xFF;
	#endif

	// In case this actually expands our output.
	if (lzw_output_file_counter + 1 > inSize) {
		outData[0] = 0;
		#ifndef SLZW_STRUCTURED_TRANSFORM
			memcpy(&outData[1], inData, inSize);
		#else
			// Undo the structured transform if necessary.
			lzw_output_file_counter = 1;
			for (i=0; i<SLZW_NUMBER_OF_READINGS; i++) {
				for (j=i; j < inSize; j+=SLZW_NUMBER_OF_READINGS) {
					outData[lzw_output_file_counter++] = inData[j];
				}
			}
		#endif
		return inSize+1;
	}

	return (++lzw_output_file_counter);
}

//************************************************************************//
//  init_dictionary	                                      		  		  //
//************************************************************************//
//
//  Function Description:
//  Initialize the LZW dictionary.
//
//  Revision History:
//		25 Oct 2006		Chris Sadler		Initial Release
////////////////////////////////////////////////////////////////////////////
static void init_dictionary(void) {
	uint16_t j;
	Dict_node* node = (Dict_node *) compression_chars;

	for (j=0; j<256; j++) {
		node[j].longer_string = 0;
		node[j].next_hash1 = ((j+1) & HASH1_MASK) >> 8;
		node[j].next_hash2 = (j+1) & HASH2_MASK;
		node[j].entry = j;
	}
}

//************************************************************************//
//  slzw_decompress                                  		  		      //
//************************************************************************//
//
//  Function Description:
//  Decompress data that we compressed.  The mini-cache size is embedded in the first character of
//  the compressed stream, so we do not need to pass it in as an entry here.  We don't spend much
//  time on this algorithm in the paper, so it has not been tested as thoroughly as the compress
//  function, but it has worked for me so far.
//
//  Inputs:
//		- inSize - Size of the file to compress
//
//	Expected Inputs:
//		- The data should be in lzw_output_file_buffer
//
//  Outputs:
//		- Returns the compressed size
//		- Decompressed data is in write_buffer
//
//  Revision History:
//		25 Oct 2006		Chris Sadler		Initial Release
//		10 Mar 2007		Dirk Buijsman		Patched bug related to adding the last dictionary
//											entry to the mini-cache
//		10 Mar 2007		Chris Sadler		Handles the case where the compressor reverts back to
//											the uncompressed data because the compression had
//											resulted in a data expansion.
////////////////////////////////////////////////////////////////////////////
uint16_t slzw_decompress(uint16_t inSize, const uint8_t* inData, uint8_t* outData) {
	uint16_t i, size_of_entry = 9, next_dict_entry, entry, hit_or_miss = MISS, right_shift = 0, num_bytes = 0;
	int32_t last_entry=-1;
	uint8_t alignment = 0, get_size = size_of_entry+1, temp_letter;
	Undict_node *current_node, *unnode;

	uint8_t array_counter=0;	// I've decided that this is the max string length
	uint8_t alignment_bitmasks[] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

	uint8_t mini_dict_hash, miniCacheSize, hit_bits;

	#ifdef SLZW_STRUCTURED_TRANSFORM
		uint16_t j, lzw_output_file_counter;
	#endif

	unnode = (Undict_node *) compression_chars;

    init_decomp_dictionary();
	next_dict_entry = 256;

	for (i = 0; i < SLZW_MAX_SIZE_OF_MINI_CACHE; i++) {
		mini_cache_structure[i] = i;
	}
	miniCacheSize = inData[0];
	hit_bits = 0;

	// Check to see if the compressor decided it was best to use the data in an uncompressed form.
	if (miniCacheSize == 0) {
		memcpy(outData, &inData[1], inSize-1);
		return inSize-1;
	}

	for (i = miniCacheSize; i != 0; i=i>>1) {
		hit_bits++;
	}

	mini_dict_hash = miniCacheSize - 1;

	if ((inData[1] & 0x01) == HIT) {	// Whats up first?
		get_size = hit_bits;
		hit_or_miss = HIT;
	}

	entry = 0;
	alignment = 0;			// Looked at first bit already

	for (i=1; i<inSize; i++) {
		// Get entry
		if (get_size > 8) {
			entry += (inData[i] << alignment);
			get_size -= 8;
			alignment += 8;
		} else {
			if (get_size > 0) {
				entry += (inData[i] & alignment_bitmasks[get_size]) << alignment;
			}

			entry = entry >> 1;					// Ditch hit or miss bit

			if (right_shift > 0) {
				entry = entry >> right_shift;
				entry = entry >> alignment;
				right_shift = 0;
			}

			if (hit_or_miss == MISS) {
				if (entry < next_dict_entry) {	// its in our dictionary
					current_node = &(unnode[entry]);
					temp_letter = current_node->entry;
					while (current_node->up != -1) {
						compBuffer[array_counter++] = current_node->entry;
						current_node = &(unnode[current_node->up]);
					}

					outData[num_bytes++] = current_node->entry;

					while (array_counter > 0) {
						outData[num_bytes++] = compBuffer[--array_counter];
					}
				} else { // its not in our dictionary yet
					current_node = &(unnode[last_entry]);
					temp_letter = current_node->entry;
					while (current_node->up != -1) {
						compBuffer[array_counter++] = current_node->entry;
						current_node = &(unnode[current_node->up]);
					}

					outData[num_bytes++] = current_node->entry;

					while (array_counter > 0) {
						outData[num_bytes++] = compBuffer[--array_counter];
					}

					// Reprint first character in the string as is the protocol for this case
					outData[num_bytes++] = current_node->entry;
				}

				array_counter = 0;

				// Update subdictionary
				mini_cache_structure[entry & mini_dict_hash] = entry;
			}
			else {			// HIT
				entry = mini_cache_structure[entry];
				if (entry < next_dict_entry) {	// its in our dictionary
					current_node = &(unnode[entry]);
					temp_letter = current_node->entry;
					while (current_node->up != -1) {
						compBuffer[array_counter++] = current_node->entry;
						current_node = &(unnode[current_node->up]);
					}

					outData[num_bytes++] = current_node->entry;

					while (array_counter > 0) {
						outData[num_bytes++] = compBuffer[--array_counter];
					}
				}
				else { // its not in our dictionary yet
					current_node = &(unnode[last_entry]);
					temp_letter = current_node->entry;
					while (current_node->up != -1) {
						compBuffer[array_counter++] = current_node->entry;
						current_node = &(unnode[current_node->up]);
					}

					outData[num_bytes++] = current_node->entry;

					while (array_counter > 0) {
						outData[num_bytes++] = compBuffer[--array_counter];
					}

					// Reprint first character in the string as is the protocol for this case
					outData[num_bytes++] = current_node->entry;
				}

				array_counter = 0;
			}

			// Create dictionary entry
			if (last_entry != -1) {
				if ((uint16_t)last_entry > next_dict_entry) {
					return 0;
				}

				#ifndef SLZW_RESETABLE_DICTIONARY
				if (next_dict_entry < SLZW_MAX_DICT_ENTRIES) {
				#endif
					unnode[next_dict_entry].entry = current_node->entry;
					unnode[next_dict_entry++].up = last_entry;
					#ifdef SLZW_ADD_NEWEST
						if (next_dict_entry < SLZW_MAX_DICT_ENTRIES) {
							mini_cache_structure[(next_dict_entry) & mini_dict_hash] = next_dict_entry;
						}
					#endif
				#ifndef SLZW_RESETABLE_DICTIONARY
				}
				#endif

				if ((next_dict_entry >> size_of_entry) != 0) {
					#ifndef SLZW_RESETABLE_DICTIONARY
					if (size_of_entry < SLZW_MAX_ENTRY_BITS) {
					#endif
					size_of_entry++;
					#ifndef SLZW_RESETABLE_DICTIONARY
					}
					#endif

					#ifdef SLZW_RESETABLE_DICTIONARY
					if (size_of_entry > SLZW_MAX_ENTRY_BITS) {
						size_of_entry = 9;
						init_decomp_dictionary();
						next_dict_entry = 256;
						entry = -1;			// Get ready to build a new dictionary
					}
					#endif
				}
			}
			else {
				#ifndef SLZW_RESETABLE_DICTIONARY
				if (next_dict_entry < SLZW_MAX_DICT_ENTRIES) {
				#endif
					#ifdef SLZW_ADD_NEWEST
						mini_cache_structure[(next_dict_entry) & mini_dict_hash] = next_dict_entry;
					#endif
				#ifndef SLZW_RESETABLE_DICTIONARY
				}
				#endif
			}

			// Clean up
			last_entry = entry;
			if (get_size < 8) {
				entry = inData[i] >> get_size;
				if ((entry & 0x01) == HIT) {
					if ((8-get_size) >= hit_bits) {		// all of the next entry is already here
						i--;		// So when we get the next byte, we get this one again
						entry = 0;
						alignment = get_size;
						right_shift = get_size;
						get_size += hit_bits;
					}
					else {
						alignment = 8-get_size;
						get_size = hit_bits - alignment;
					}

					hit_or_miss = HIT;
				}
				else {
					alignment = 8-get_size;
					get_size = size_of_entry+1-alignment;
					hit_or_miss = MISS;
				}
			}
			else {
				if (i+1 < inSize) {
					if ((inData[i+1] & 0x01) == HIT) {		// Whats up first?
						get_size = hit_bits;
						hit_or_miss = HIT;
					}
					else {
						get_size = size_of_entry+1;
						hit_or_miss = MISS;
					}

					entry = 0;
					alignment = 0;			// Looked at first bit already
				}
				else {
					//fprintf(stderr, "Looking at EOF\n");
				}
			}
		}
	}

	#ifdef SLZW_STRUCTURED_TRANSFORM
		// There is probably a smarter way to do this, but this was fast enough for the block size.
		lzw_output_file_counter = 0;
		for (i=0; i<SLZW_NUMBER_OF_READINGS; i++) {
			for (j=i; j < num_bytes; j+=SLZW_NUMBER_OF_READINGS) {
				inData[lzw_output_file_counter++] = outData[j];
			}
		}

		memcpy(outData, inData, num_bytes);
	#endif

	return num_bytes;
}

//************************************************************************//
//  init_decomp_dictionary	                               		  		  //
//************************************************************************//
//
//  Function Description:
//  Initialize the LZW dictionary for decompression.
//
//  Revision History:
//		25 Oct 2006		Chris Sadler		Initial Release
////////////////////////////////////////////////////////////////////////////
static void init_decomp_dictionary(void) {
	uint16_t i;
	Undict_node* unnode = (Undict_node *) compression_chars;

	for (i=0; i<256; i++) {
		unnode[i].up = -1;
		unnode[i].entry = i;
	}
}

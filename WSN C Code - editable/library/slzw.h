/* Copyright (C) 2005-2006 Princeton University,
*  Christopher M. Sadler (csadler@princeton.edu)
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
//  Revision History:
//		25 Oct 2006		Chris Sadler		Initial Release
//		10 Mar 2007		Chris Sadler		Increased WRITE_BUFFER_SIZE to
//											properly handle the case where the
//											data is very random and compression
//                                          actually causes it to expand.


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
 * Example to compress and decompress without BWT:
 * @code
 * uint8_t* rawData[100];
 * uint8_t* compressedData[100];
 * uint8_t* decompressedData[100];
 * uint16_t compressedSize;
 *
 * // Put some data into rawData array
 * compressedSize = slzw_compress(100, 16, rawData, compressedData);
 * // Delete data in rawData array
 * slzw_decompress(compressedSize, 16, compressedData, rawData);
 * // rawData is restored.
 * @endcode
 *
 * Example to compress and decompress with BWT:
 * @code
 * uint8_t* rawData[100];
 * uint8_t* bwtBuffer[102];
 * uint8_t* compressedData[100];
 * uint16_t compressedSize;
 *
 * // Put some data into rawData array
 * bwt_encode(100, rawData, bwtBuffer);
 * // Delete data in rawData array
 * compressedSize = slzw_compress(102, 16, bwtBuffer, compressedData);
 * slzw_decompress(compressedSize, 16, compressedData, bwtBuffer);
 * bwt_decode(102, bwtBuffer, rawData);
 * // rawData is restored.
 * @endcode
 *
 * Memory and processing requirements of compression, where MC is mini-cache:
@verbatim
Block    , Algorithm     , Compressed  , Flash  , RAM    , Time , Time/byte
size     ,               , Size (bytes), (bytes), (bytes), (ms) , (us)

512 bytes, S-LZW-MC4     , 438         , 1768   , 3348   , 12.27, 28.01
512 bytes, S-LZW-MC8     , 426         , 1744   , 3356   , 13.02, 30.57
512 bytes, S-LZW-MC16    , 415         , 1744   , 3372   , 12.21, 29.42
512 bytes, S-LZW-MC32    , 417         , 1744   , 3404   , 11.87, 28.46
512 bytes, S-LZW-MC4-BWT , 420         , 2116   , 3604   , 21.28, 50.66
512 bytes, S-LZW-MC8-BWT , 415         , 2092   , 3612   , 20.77, 50.05
512 bytes, S-LZW-MC16-BWT, 417         , 2092   , 3628   , 20.97, 50.28
512 bytes, S-LZW-MC32-BWT, 416         , 2092   , 3660   , 20.71, 49.79
256 bytes, S-LZW-MC32    , 213         , 1482   , 2892   ,  6.39, 29.98
256 bytes, S-LZW-MC32-BWT, 215         , 1826   , 3148   , 11.59, 53.9
128 bytes, S-LZW-MC32    ,  98         , 1354   , 2636   ,  3.2 , 32.78
128 bytes, S-LZW-MC32-BWT,  88         , 1698   , 2892   ,  6.5 , 74.14
@endverbatim
 * The original code is available from http://cmsadler.googlepages.com/
 *
 * The following changes have been made:
 * - Changed FLASH_PAGE_SIZE from 264 to 256
 * - Changed MAX_DICT_ENTRIES so that it is calculated automatically based on
 *   the number of bits in dictionary index
 * - Commented out ADD_NEWEST (it had negative affects on compression ratio)
 * - Commented out STRUCTURED_TRANSFORM (not relevant for general data)
 * - Datatypes use fixed length values to avoid errors with different C
 *    compilers.
 * - Added prototypes for BWT functions
 * - Changed prototypes for slzw functions so that the data is passed in as
 *    an argument and is not required to be in a specially named extern array.
 * - Added doxygen format comments. Other comments may no longer be accurate.
 *
 * @file slzw.h
 * @date 18-Jan-2010
 * @author Chris Sadler, Princeton University
 * @author Stefano Rollo and Seán Harte, Tyndall National Institute
 ******************************************************************************/


#ifndef SLZW_H
#define SLZW_H

/**
 * This affects the block size used. @c BLOCK_SIZE is twice @c FLASH_PAGE_SIZE.
 **/
#define SLZW_FLASH_PAGE_SIZE            256

/**
 * This is the maximum amount of data that you can compress.  If you increase
 * it, it will cost you more RAM.
 **/
#define SLZW_BLOCK_SIZE					(SLZW_FLASH_PAGE_SIZE << 1)

/**
 * You can increase the number of dictionary entries to 4096, but there are only
 * 12 bits allocated in each dictionary entry so it won't work for more than
 * that.
 **/
#define SLZW_MAX_ENTRY_BITS				9
#define SLZW_MAX_DICT_ENTRIES			(1 << SLZW_MAX_ENTRY_BITS)

/* Uncomment this line if you want the dictionary to reset when it fills. */
//#define SLZW_RESETABLE_DICTIONARY

/* These definitions are for the mini-cache. */
#define SLZW_USE_MINICACHE

/*
 * If this #define is uncommented, it will add entries to the mini-cache as they
 * are created.
 */
//#define SLZW_ADD_NEWEST

#define SLZW_MAX_SIZE_OF_MINI_CACHE      32

/* Structured Transform Parameters. See referenced paper. */
//#define SLZW_STRUCTURED_TRANSFORM
//#define SLZW_SIZE_OF_READING				10
//#define SLZW_NUMBER_OF_READINGS			50

/**
 * This must be defined to use the BWT functions. If it is not defined, data
 * memory is saved, as a smaller buffer is needed in @e slzw.c
 **/
#define BWT_USE


/**
 * Compress data using the lossless LZW algorithm.
 *
 * @param inSize how many byes are in @a inData. This must be less than @c
 *     SLZW_BLOCK_SIZE.
 * @param miniCacheSize different values (<=32) can affect compression ratio,
 *     but 32 should be a good value.
 * @param inData pointer to data to compress.
 * @param outData where to store compressed data.
 * @return size of compressed data.
 **/
uint16_t slzw_compress(uint16_t inSize, uint8_t miniCacheSize,
                                const uint8_t* inData, uint8_t* outData);
                                
                                
/**
 * Decompress data using the lossless LZW algorithm.
 *
 * @param inSize how many byes are in @a inData.
 * @param inData pointer to data to decompress.
 * @param outData where to store decompressed data. 
 * @return size of decompressed data.
 **/
uint16_t slzw_decompress(uint16_t inSize, const uint8_t* inData,
                         uint8_t* outData);

#if defined (BWT_USE) || defined (__DOXYGEN__)
/**
 * Perform the BWT on some data. Only available if @c BWT_USE is defined.
 *
 * @param size how many byes are in @a buf_in.
 * @param bufferIn pointer to the input data.
 * @param bufferOut data after BWT has been performed. Two bytes are added as an
 *     index for doing the reverse transform.
 * @return @a size + 2.
 **/
uint16_t bwt_encode(uint16_t size, const uint8_t *bufferIn, uint8_t *bufferOut);


/**
 * Perform the reverse BWT on some data. Only available if @c BWT_USE is
 * defined.
 *
 * @param size how many byes are in @a buf_in.
 * @param bufferIn pointer to the input data, with two extra bytes that are used
 *     as index of where original data started.
 * @param bufferOut this contains original data before BWT was performed.
 * @return @a size - 2.
 **/
uint16_t bwt_decode(uint16_t size, const uint8_t *bufferIn, uint8_t *bufferOut);
#endif

#endif

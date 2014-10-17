/******************************************************************************\
 * This source code is released to the public domain.
\******************************************************************************/

/***************************************************************************//**
 * Encryption and decryption using XXTEA algorithm. This is a shared-key block-
 * encrpytion algorithm.
 *
 * @file xxtea.h
 * @date 15-Jan-2010
 ******************************************************************************/


#ifndef XXTEA_H
#define XXTEA_H


/**
 * Performs XXTEA encyption and decryption.
 *
 * @param v data to encrypt/decrypt (in 32bit words).
 * @param n number of 32 bit words in @a data. Positive values do encryption,
 *     negative values do decryption. There must be at least 2 words.
 * @param k the shared key to use. Four 32-bit words.
 **/
void xxtea(uint32_t* v, int8_t n, uint32_t const k[4]);


#endif

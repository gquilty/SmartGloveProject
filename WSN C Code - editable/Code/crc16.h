/******************************************************************************\
 * Copyright (c) 2010, Tyndall National Institute
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Tyndall National Institute nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/***************************************************************************//**
 * Interface for CRC functions.
 *
 * Function for calculating CRC-16 checksum based on the polynomial:
 * x^16 + x^15 + x^2 + 1 (0xa001). CRC-16 is commonly used in disk-drive
 * applications.
 *
 * Example usage to calculate CRC of values in an array:
 *   @code
 *     uint16_t crcValue;
 *     crc16_init();
 *     for (i = 0; i < SIZE; i++) {
 *         crc16_update(array[i]);
 *     }
 *     crcValue = crc16_read();
 *   @endcode
 *
 * @file crc16.h
 * @date 24-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef CRC16_H
#define CRC16_H


/** Resets CRC value for a new calculation. **/
void crc16_init(void);


/**
 * Read current CRC value.
 *
 * @return 16-bit CRC value.
 **/
uint16_t crc16_read(void);


/**
 * Calculates new CRC value based on current value and new data byte.
 *
 * @param data byte to use in updating CRC.
 **/
void crc16_update(uint8_t data);

#endif

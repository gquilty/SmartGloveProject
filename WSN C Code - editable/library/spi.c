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
 * Implementation of SPI function for sending/receiving an array of bytes.
 *
 * @file spi.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "spi.h"


/******************************************************************************\
 * See spi.h for documentation of these functions.
\******************************************************************************/

void spi_readArray(uint8_t* readBytes, uint16_t size)
{
    uint8_t* endPtr = readBytes + size;

    /* Just generate clock in order to read without writing */
    while (readBytes < endPtr) {
        *readBytes++ = spi_readWriteByte(0);
    }

}


void spi_writeArray(const uint8_t* writeBytes, uint16_t size)
{
    const uint8_t* endPtr = writeBytes + size;

    /* Ignore value that is read from SPI */
    while (writeBytes < endPtr) {
        spi_readWriteByte(*writeBytes++);
    }

}


void spi_readWriteArray(uint8_t* readBytes,
                                       const uint8_t* writeBytes, uint16_t size)
{
    uint8_t* endPtr = readBytes + size;

    while (readBytes < endPtr) {
        *readBytes++ = spi_readWriteByte(*writeBytes++);
    }
}

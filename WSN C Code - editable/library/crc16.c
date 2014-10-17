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
 * Implementation of CRC.
 *
 * Function for calculating CRC16 checksum based on the polynomial:
 * x^16 + x^15 + x^2 + 1 (0xa001). The code is based on sample code in avr-libc
 * documentation.
 *
 * An internal CRC value is used. This be reset, updated, and read back.
 *
 * @file crc16.c
 * @date 12-Jan-2010
 * @author Seán Harte
 ******************************************************************************/

#if defined UC_AVR || defined UC_8051
#   include "global.h"      /* This file is also used by USB programmer */
#endif
#include "crc16.h"

#ifdef UC_AVR
#   include <util/crc16.h>
#endif


#define CRC_POLY        0xA001
#define CRC_INIT_VALUE  0xFFFF


/** Current CRC value. **/
static unsigned short crcValue;


/******************************************************************************\
 * See crc.h for documentation of these functions.
\******************************************************************************/


void crc16_init(void)
{
    crcValue = CRC_INIT_VALUE;
}


unsigned short crc16_read(void)
{
    return crcValue;
}


void crc16_update(unsigned char data)
{
#ifdef UC_AVR
    crcValue = _crc16_update(crcValue, data);   /* Assembly implementation */
#else
    crcValue ^= data;
    for (unsigned char i = 0; i < 8; ++i) {
        if (crcValue & 1) crcValue = (crcValue >> 1) ^ CRC_POLY;
        else crcValue = (crcValue >> 1);
    }
#endif
}

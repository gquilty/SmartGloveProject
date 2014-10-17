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
 * Functions for writing and reading the I2C EEPROM on the programming boards.
 * @todo replace 5ms delay with polling, which may be quicker.
 *
 * @file eeprom_i2c.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "eeprom_i2c.h"
#include "i2c.h"
#include "delay.h"


/******************************************************************************\
 * See eeprom_i2c.h for documentation of these functions.
\******************************************************************************/


void eeprom_i2c_write(const uint8_t* data, uint16_t start, uint16_t end)
{
    uint8_t address[2];
    uint16_t pageEnd = start | (EEPROM_I2C_PAGE_SIZE - 1);

    /* Loop through each page that needs to be written to */
    for (; start <= end; pageEnd += EEPROM_I2C_PAGE_SIZE) {
        address[0] = HIGH_BYTE(start);
        address[1] = LOW_BYTE(start);
        pageEnd = (pageEnd <= end) ? pageEnd : end;

        /* Write address */
        i2c_write(EEPROM_I2C_COMM_ADDRESS, address, 2,
                                         I2C_OPTION_ADDRESS | I2C_OPTION_START);

        /* Write data */
        i2c_write(0, data, pageEnd - start + 1, I2C_OPTION_STOP);

        data += pageEnd - start + 1;
        start = pageEnd + 1;
        
        delay_us(5000);
    }
}


void eeprom_i2c_read(uint8_t* data, uint16_t start, uint16_t end)
{
    uint8_t address[2] = {HIGH_BYTE(start), LOW_BYTE(start)};

    if (start >= end) {
        return;
    }

    /* Write address to read from */
    i2c_write(EEPROM_I2C_COMM_ADDRESS, address, 2, I2C_OPTION_FULL);
    
    /* Read back data */
    i2c_read(EEPROM_I2C_COMM_ADDRESS, data, end - start + 1, I2C_OPTION_FULL);
}

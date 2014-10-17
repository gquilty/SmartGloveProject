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
 * Function for reading from ad7997/8 ADC. Look at the datasheet for more.
 *
 * The ADC is read in mode 2 using the convert sequence command. The sequence of
 * channels to read is set in the configuration register. This only needs to be
 * done if the current sequence is different to the previous.
 *
 * @todo: The function below only supports a subset of the features provided by
 *     the ADC. For example it could be configured to give a signal when
 *     values go above or below a specified threshold etc.
 *
 * @file ad7998.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "ad7998.h"
#include "i2c.h"


/** I2C address of ADC. **/
#define AD7998_ADDRESS                  0x20

/******************************************************************************\
 * See ad7998.h for full documentation of this function.
\******************************************************************************/

status_t ad7998_read(uint8_t* data, uint8_t channels)
{
    status_t status = STATUS_OK;
    uint8_t command[3];
    uint8_t i;
    static uint8_t prevChannels = 0;
    static uint8_t numChannels = 0;


    if (prevChannels != channels) {
        /* Write bitmask of channels to configuration register */
        command[0] = 0x02;      /* Config register address */
        command[1] = channels >> 4;
        command[2] = ((channels & 0x0F) << 4) | BIT(3); /* 3 = Filter enable */
        
        if ((status = i2c_write(AD7998_ADDRESS, command, 3, I2C_OPTION_FULL)) !=
                                                                    STATUS_OK) {
            return status;
        }

        /* Count how many channels are being converted */
        numChannels = 0;
        for (i = 0x80; i > 0; i >>= 1) {
            if (i & channels) {
                numChannels++;
            }
        }
        
        prevChannels = channels;
    }

    command[0] = 0x70;          /* Convert sequence command */

    /* Start converting (note that I2C STOP is not sent) */
    if ((status = i2c_write(AD7998_ADDRESS, command, 1,
                         I2C_OPTION_START | I2C_OPTION_ADDRESS)) != STATUS_OK) {
        return status;
    }

    /* Read back results (a repeated START will be used) */
    return i2c_read(AD7998_ADDRESS, data, numChannels * 2, I2C_OPTION_FULL);
}

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
 * Implementation of functions I2C. This is the software implementation. These
 * functions require the following functions (or macros):
 *  - @c i2cSw_initPins()
 *  - @c i2cSw_pullDownSda()
 *  - @c i2cSw_releaseSda()
 *  - @c i2cSw_pullDownScl()
 *  - @c i2cSw_releaseScl()
 *  - @c i2cSw_getSda()       (returns SDA as 0 or 1)
 *  - @c i2cSw_getScl()       (returns SCL as 0 or 1)
 *
 * This file is only for the nRF9E5, and the 2420_revA board, as other boards
 * have hardware support for I2C.
 *
 * @file i2c_sw.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "delay.h"
#include "i2c.h"
#if defined (UC_AVR)
#   include "avr/i2c_sw_avr.h"
#elif defined (UC_8051)
#   include "nrf/i2c_sw_nrf.h"
#endif


/* Internal function prototypes for I2C functions. */
static void delayI2c(void);
static void pulseClock(void);
static void start(void);
static void stop(void);
static uint8_t readBit(void);
static uint8_t putByte(uint8_t byte);


/******************************************************************************\
 * See i2c.h for documentation of these functions.
\******************************************************************************/

uint8_t i2c_write(uint8_t address, const uint8_t* data, uint16_t length,
                                                                 uint8_t option)
{
    if (option & I2C_OPTION_START) {
        start();
    }

    if (option & I2C_OPTION_ADDRESS) {
        address <<= 1;          /* shift 0 into read flag bit */

        if (putByte(address) == STATUS_NO_ACK) {
            stop();
            return STATUS_NO_ACK;
        }
    }

    for (uint16_t i = 0; i < length - 1; ++i) {
        if (putByte(*data++)) {
            stop();
            return STATUS_NO_ACK;
        }
    }
    /* ACK is optional after last byte */
    putByte(*data++);
    if (option & I2C_OPTION_STOP) {
        stop();
    }
    return STATUS_OK;
}


uint8_t i2c_read(uint8_t address, uint8_t* data, uint16_t length, uint8_t option)
{
    if (option & I2C_OPTION_START) {
        start();
    }

    if (option & I2C_OPTION_ADDRESS) {
        address = (address << 1) | 0x01;    /* Shift address and set read flag */
        if (putByte(address) == STATUS_NO_ACK) {
            stop();
            return STATUS_NO_ACK;
        }
    }

    for (uint16_t i = 0; i < length; i++, data++) {
        /* read byte */
        for (uint8_t j = 0x80; j >= 1; j >>= 1) {
            if (readBit()) {
                *data |= j;
            }
            else {
                *data &= ~j;
            }
        }
        /* Give ack if required */
        if (i < length - 1) {
            i2cSw_pullDownSda();
        }
        pulseClock();
        i2cSw_releaseSda();
    }

    if (option & I2C_OPTION_STOP) {
        stop();
    }
    return STATUS_OK;
}


/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/
/**
 * Delay between clock changes.
 **/
static void delayI2c(void)
{
    delay_us(((1e6 / (float)I2C_FREQ) / 2) - 3);
}


/**
 * Pulse clock once. After SCL is released, there is a delay incase the slave
 * is holding SCL low.
 **/
static void pulseClock(void)
{
    delayI2c();

    i2cSw_releaseScl();

    /* Some slave devices hold the clock down until they are ready */
    while (!i2cSw_getScl()) {
        ;
    }

    delayI2c();

    i2cSw_pullDownScl();
}


/**
 * START: Bring SDA low while SCL is high.
 **/
static void start(void)
{
    i2cSw_initPins();
    delayI2c();
    i2cSw_pullDownSda();
    delayI2c();
    i2cSw_pullDownScl();
}


/**
 * STOP: Release SDA while SCL is high.
 **/
static void stop(void)
{
    i2cSw_pullDownSda();
    delayI2c();
    i2cSw_releaseScl();
    delayI2c();
    i2cSw_releaseSda();
}


/**
 * Read and return single bit: release SCL, read SDA and pull down SCL. After
 * SCL is released, there is a delay in case the slave is holding SCL low.
 *
 * @return 0 or non-zero, depending on what bit was read.
 **/
static uint8_t readBit(void)
{
    uint8_t bit;
    i2cSw_releaseScl();
    delayI2c();
    /* Some slave devices hold the clock down until they are ready */
    while (!i2cSw_getScl()) {
        ;
    }
    bit = i2cSw_getSda();
    i2cSw_pullDownScl();
    return bit;
}


/**
 * Send byte MSB first. SCL is pulsed between each bit.
 *
 * @param byte what byte to write.
 *
 * @return @c STATUS_NO_ACK no ack is received, otherwise @c STATUS_ACK.
 **/
static uint8_t putByte(uint8_t byte)
{
    for (uint8_t i = 0x80; i > 0; i >>= 1) {
        if (!(byte & i)) {
            i2cSw_pullDownSda();
        }
        pulseClock();
        i2cSw_releaseSda();
    }
    if (readBit()) {
        return STATUS_NO_ACK;
    }
    else {
        return STATUS_OK;
    }
}

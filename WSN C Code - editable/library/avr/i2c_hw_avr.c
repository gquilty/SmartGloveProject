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
 * Implementation of functions for I2C. This is the hardware implementation. 
 * @note These functions can lock if there is a problem with the I2C hardware.
 *
 * @file i2c_hw_avr.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifdef BOARD_HAS_NO_HARDWARE_I2C
#error "Hardware I2C cannot be used on this board"
#endif


#include <util/twi.h>
#include "global.h"
#include "i2c.h"
#include "delay.h"


/* I2C clock limits from datasheet */
#if I2C_FREQ >= 16 * F_CPU
#error "I2C_FREQ must be less than F_CPU/16"
#endif
#if F_CPU / I2C_FREQ >= 528
#error "I2C freq too low"
#endif


/* Internal function prototypes for I2C functions. */
static void start(void);
static uint8_t putByte(uint8_t byte);
static uint8_t getByte(uint8_t ack);


/******************************************************************************\
 * Inline functions.
\******************************************************************************/

/** Wait for I2C operation to finish. **/
static inline void waitForI2c(void)
{
    while (!(TWCR & BIT(TWINT))) {
        ;
    }
}


/** Send stop condition. **/
static inline void stop(void)
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWSTO);
}


/******************************************************************************\
 * See i2c.h for documentation of these functions.
\******************************************************************************/

status_t i2c_write(uint8_t address, const uint8_t* data,
                                                uint16_t length, uint8_t option)
{
    uint16_t i;

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

    for (i = 0; i < length; ++i) {
        /* ACK is optional after last byte */
        if (putByte(data[i]) == STATUS_NO_ACK && ((i+1) < length) ) {
            stop();
            return STATUS_NO_ACK;
        }
    }
    
    if (option & I2C_OPTION_STOP) {
        stop();
    }

    return STATUS_OK;
}


status_t i2c_read(uint8_t address, uint8_t* data,
                                                uint16_t length, uint8_t option)
{
    uint16_t i;

    if (option & I2C_OPTION_START) {
        start();
    }

    if (option & I2C_OPTION_ADDRESS) {
        address = (address << 1) | 0x01;    /* Shift address and set read flag */
        if (putByte(address) == STATUS_NO_ACK) {
            stop();
            return 0;
        }
    }

    for (i = 0; i < length; ++i) {
        if (i+1 < length) {
            data[i] = getByte(STATUS_ACK);
        }
        else {
            data[i] = getByte(STATUS_NO_ACK);
        }
    }

    if (option & I2C_OPTION_STOP) {
        stop();
    }
    return STATUS_OK;
}


/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/

/** Set up I2C clock to I2C_FREQ and send start condition. **/
static void start(void)
{
    static bool isInit = false;

    if (!isInit) {
        /* From datasheet: I2C_FREQ = F_CPU / (16 + 2(TWBR) * 4^TWPS) */
        TWBR = ((F_CPU / I2C_FREQ) - 16UL) / 2UL;

        /* Set TWPS = 0 */
        TWSR &= ~BIT(TWPS1) & ~BIT(TWPS0);

        /* Reset I2C */
        TWCR &= ~BIT(TWSTO) & ~BIT(TWEN);
        isInit = true;
    }
    
    /* Send start */
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);

    /* Wait for start to finish */
    waitForI2c();
}


/**
 * Read byte and acknowledge it.
 *
 * @param ack if the byte should not be acked, set this to @c STATUS_NO_ACK.
 *
 * @return byte that was read.
 **/
static uint8_t getByte(uint8_t ack)
{
    if (ack == STATUS_NO_ACK) {
        TWCR = BIT(TWINT) | BIT(TWEN);
    }
    else {
        TWCR = BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
    }

    /* Wait to finish receiving byte */
    waitForI2c();

    return TWDR;
}


/**
 * Send byte.
 *
 * @param byte byte to send.
 *
 * @return @c STATUS_NO_ACK if no ack is received, otherwise @c STATUS_OK.
 **/
static uint8_t putByte(uint8_t byte)
{
    /* Byte to send*/
    TWDR = byte; 

    /* Send byte */
    TWCR = BIT(TWINT) | BIT(TWEN);
    
    /* Wait for byte to finish sending */
    waitForI2c();

    /* Check if slave acknowledged */
    if (TW_STATUS == TW_MT_SLA_ACK || TW_STATUS == TW_MT_DATA_ACK ||
            TW_STATUS == TW_MR_SLA_ACK ) {
        return STATUS_OK;
    }
    else {
        return STATUS_NO_ACK;
    }
}
status_t i2c_HMC_send(void)
{
	start();

	//address device
    if (putByte(0x3C) == STATUS_NO_ACK) 
	{
		stop();
		return STATUS_ACK;
    }

	// first word address
	if (putByte(0x02) == STATUS_NO_ACK ) 
	{
		stop();
		return STATUS_ACK;
	}
	
	// second word address
	if (putByte(0x00) == STATUS_NO_ACK ) 
	{
		stop();
		return STATUS_ACK;
	}
	
    stop();

	return STATUS_OK;
}

status_t i2c_HMC_read(uint8_t* data)
{
//	int i;
	start();
	
	//address device
    if (putByte(0x3D) == STATUS_NO_ACK) 
	{
		stop();
		return STATUS_ACK;
    }
	
	// read data now
	*data = getByte(STATUS_NO_ACK);
	
	stop();
	
	return STATUS_OK;
}
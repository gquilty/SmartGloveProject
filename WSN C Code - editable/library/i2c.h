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
 * I2C master implementation.
 *
 * For Atmegas, the hardware I2C unit is used, unless the 2420_revA board is
 * being used. If it is, then software bit-banging is used on the pins defined
 * as SDA and SCL in @e i2cSw_avr.h
 *
 * For nRF905, only software I2C is available. This is on the pins defined in
 * @e i2cSw_nrf.h
 *
 * I2C frequency is set by setting the @c I2C_FREQ variable. If not set, it
 * defaults to 100kHz.
 *
 * @todo Buffered I2C (interrupt based).
 *
 * @file i2c.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef I2C_H
#define I2C_H


#ifndef I2C_FREQ
/** I2C clock frequency. **/
#define I2C_FREQ            400000UL  /* 100kHz. */
#endif


/** Bit controls whether a START is sent (used for @a control parameter). **/
#define I2C_OPTION_START    BIT(0)

/** Bit controls whether a STOP is sent (used for @a control parameter). **/
#define I2C_OPTION_STOP     BIT(1)

/** Bit controls whether an ADDRESS is sent (used for @a control parameter). **/
#define I2C_OPTION_ADDRESS  BIT(2)


/**
 * Use this to send only the data, with no address, or START/STOP
 * (used for @a control parameter).
 **/
#define I2C_OPTION_RAW      0

/**
 * Use this to send START, ADDRESS, the data, and STOP
 * (used for @a control parameter).
 **/
#define I2C_OPTION_FULL     BIT(0) | BIT(1) | BIT(2)

#define I2C_OPTION_NON_STOP     BIT(0) | BIT(2)


/**
 * Write a packet to I2C bus.
 *
 * @param address 7-bit address of device to write to. Unused if
 *     @c I2C_OPTION_ADDRESS is not set.
 * @param data pointer to bytes to be written.
 * @param length number of bytes to write.
 * @param option control if a START, STOP should be used or if the device
 *     address should be sent.
 *
 * @return @c STATUS_NO_ACK if failed, otherwise @c STATUS_OK.
 **/
status_t i2c_write(uint8_t address, const uint8_t* data,
                                               uint16_t length, uint8_t option);


/**
 * Read a packet from the I2C bus.
 *
 * @param address 7-bit address of device to read from. Unused if
 *     @c I2C_OPTION_ADDRESS is not set.
 * @param[out] data pointer to where to store read bytes.
 * @param length number of bytes to read.
 * @param option control if a START, STOP should be used or if the device
 *     address should be sent.
 *
 * @return @c STATUS_NO_ACK if failed, otherwise @c STATUS_OK.
 **/
status_t i2c_read(uint8_t address, uint8_t* data,
                                               uint16_t length, uint8_t option);

status_t i2c_HMC_send(void);
status_t i2c_HMC_read(uint8_t* data);
#endif

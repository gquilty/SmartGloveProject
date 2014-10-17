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
 * Support functions for software I2C on 10mm nRF9E5 board.
 *
 * @file i2c_sw_nrf.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef I2CSW_NRF_H
#define I2CSW_NRF_H


#include "global.h"
#include "delay.h"

/** I2C SCL (clock) pin. **/
#define I2C_SCL_PIN     3

/** I2C SDA (data) pin. **/
#define I2C_SDA_PIN     5


/** Initialise pins. **/
static inline void i2cSw_initPins(void)
{
    /* Set pin values to 0, direction as input, and enable high current sink */
    P0 &= ~BIT(I2C_SCL_PIN) & ~BIT(I2C_SDA_PIN);
    P0_DIR |= BIT(I2C_SCL_PIN) | BIT(I2C_SDA_PIN);
    P0_DRV |= BIT(I2C_SCL_PIN) | BIT(I2C_SDA_PIN);
}


/** Set SDA to output. **/
static inline void i2cSw_pullDownSda(void)
{
    P0_DIR &= ~BIT(I2C_SDA_PIN);
}


/** Set to input. **/
static inline void i2cSw_releaseSda(void)
{
    P0_DIR |= BIT(I2C_SDA_PIN);
}


/** Set SDA to output. **/
static inline void i2cSw_pullDownScl(void)
{
    P0_DIR &= ~BIT(I2C_SCL_PIN);
}


/**  Set to input. **/
static inline void i2cSw_releaseScl(void)
{
    P0_DIR |= BIT(I2C_SCL_PIN);
}


/**
 * Read SDA pin. The pin should be read after the master has released the SDA
 * line.
 *
 * @return 0 or 1.
 **/
static inline uint8_t i2cSw_getSda(void)
{
    return (P0 & BIT(I2C_SDA_PIN));
}


/**
 * Read SCL pin. The pin should be read after the master has released the SCL
 * line.
 *
 * @return 0 or 1.
 **/
static inline uint8_t i2cSw_getScl(void)
{
    return (P0 & BIT(I2C_SCL_PIN));
}


#endif

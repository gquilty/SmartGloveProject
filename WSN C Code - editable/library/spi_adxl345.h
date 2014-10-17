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
 * SPI master implementation.
 * @todo Software control of SPI clock speed. Currently the maximum is used.
 * @todo Software implementation of SPI (bit-banging) for using SPI on different
 *     pins.
 *
 * @file spi_adxl345.h
 * @date 11-Oct-2010
 * @author Michael Walsh
 ******************************************************************************/


#ifndef SPI_ADXL345_H
#define SPI_ADXL345_H

/*
#define CS_ACC  (PORTC,0x03)
#define CS_ACC_HI  PORTC |= (1<<3)
#define CS_ACC_LO  PORTC &= ~(1<<3)
#define CS_ACC_IO (DDRC,3)		//data direction
#define CS_ACC_OUT  DDRC |= (1<<3)

#define MOSI_ACC  (PORTC,0x07)
#define MOSI_HI  PORTC |= (1<<7)
#define MOSI_LO  PORTC &= ~(1<<7)
#define MOSIIO (DDRC,7)		//data direction
#define MOSI_OUT  DDRC |= (1<<7)

//#define MISO_ACC   (PINA & (1<<7)) 
#define MISO_ACC   (PORTA ,0x07) 
#define MISO_HI  PORTA |= (1<<7)
#define MISO_LO  PORTA &= ~(1<<7)
#define MISOIO (DDRA,7)		//data direction
#define MISO_IN  DDRA &= ~(1<<7)	

#define SCK  (PORTA,0x5)
#define SCK_HI  PORTA |= (1<<5)
#define SCK_LO  PORTA &= ~(1<<5)
#define SCKIO (DDRA,5)		//data direction
#define SCK_OUT  DDRA |= (1<<5)
*/
//Ports for Smart Glove

#define uc_sw_cs  (PORTA,0x05)
#define uc_sw_cs_HI  PORTA |= (1<<5)
#define uc_sw_cs_LO  PORTA &= ~(1<<5)
#define uc_sw_csIO (DDRA,5)		//data direction
#define uc_sw_cs_OUT  DDRA |= (1<<5)

#define uc_sw_SCLK  (PORTA,0x06)
#define uc_sw_SCLK_HI  PORTA |= (1<<6)
#define uc_sw_SCLK_LO  PORTA &= ~(1<<6)
#define uc_sw_SCLKIO (DDRA,6)		//data direction
#define uc_sw_SCLK_OUT  DDRA |= (1<<6)

#define uc_sw_SDI  (PORTA,0x07)
#define uc_sw_SDI_HI  PORTA |= (1<<7)
#define uc_sw_SDI_LO  PORTA &= ~(1<<7)
#define uc_sw_SDIKIO (DDRA,7)		//data direction
#define uc_sw_SDI_OUT  DDRA |= (1<<7)
#define uc_sw_SDI_IN  DDRA &= ~(1<<7)


#define uc_sw_SDO  (PORTG,0x02)
#define uc_sw_SDO_HI  PORTG |= (1<<2)
#define uc_sw_SDO_LO  PORTG &= ~(1<<2)
#define uc_sw_SDOKIO (DDRG,2)		//data direction
#define uc_sw_SDO_OUT  DDRG |= (1<<2)
#define uc_sw_SDO_IN  DDRG &= ~(1<<2)


// Ports for WImuRev3_A
/*
#define uc_sw_cs  (PORTC,0x07)
#define uc_sw_cs_HI  PORTC |= (1<<7)
#define uc_sw_cs_LO  PORTC &= ~(1<<7)
#define uc_sw_csIO (DDRC,7)		//data direction
#define uc_sw_cs_OUT  DDRC |= (1<<7)

#define uc_sw_SCLK  (PORTA,0x05)
#define uc_sw_SCLK_HI  PORTA |= (1<<5)
#define uc_sw_SCLK_LO  PORTA &= ~(1<<5)
#define uc_sw_SCLKIO (DDRA,5)		//data direction
#define uc_sw_SCLK_OUT  DDRA |= (1<<5)

#define uc_sw_SDI  (PORTA,0x03)
#define uc_sw_SDI_HI  PORTA |= (1<<3)
#define uc_sw_SDI_LO  PORTA &= ~(1<<3)
#define uc_sw_SDIKIO (DDRA,3)		//data direction
#define uc_sw_SDI_OUT  DDRA |= (1<<3)
#define uc_sw_SDI_IN  DDRA &= ~(1<<3)


#define uc_sw_SDO  (PORTA,0x04)
#define uc_sw_SDO_HI  PORTA |= (1<<4)
#define uc_sw_SDO_LO  PORTA &= ~(1<<4)
#define uc_sw_SDOKIO (DDRA,4)		//data direction
#define uc_sw_SDO_OUT  DDRA |= (1<<4)
#define uc_sw_SDO_IN  DDRA &= ~(1<<4)
*/
void adxl345_spi_init(void);
void adxl345_spi_write(uint8_t address, uint8_t data);
uint8_t adxl345_spi_read(uint8_t data);
char getDOUT(void);

#endif
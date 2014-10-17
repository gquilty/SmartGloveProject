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
 * Header file for using FT232R to do SPI communications by bit-banging.
 *
 * Note that these functions will simply exit the application if there is an
 * error communicating with the FT232R.
 *
 * The calls to these functions take quite some time. To speed up the
 * application try to call these functions as few times as possible (send as
 * much data together as you can).
 *
 * @file ftSpi.h
 * @date 27-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef FT_SPI_H
#define FT_SPI_H


/**
 * FTDI AN232R application note recommends less than 1Mbps. The actual clock
 * rate is 16 times this value.
 **/
#define FTSPI_MAX_BAUDRATE          921600


/**
 * Initialises FT232R device in bit-bang mode for SPI communication. This must
 * called before using the other functions in this header.
 *
 * @param device description of the SPI device that will be communicated
 *     with. This contains a string description which must match the string in
 *     in the FT232R internal EEPROM, and also defines which pins to use for
 *     MISO, MOSI, SCK, and nCS.
 **/
void ftSpi_init(spiDeviceInfo_t* device);


/**
 * Simultaneously reads and writes a number of bytes using the SPI protocol by
 * bit-banging over the FT232R. Note that each call to this function takes
 * a relatively large time to return, so try to call it as few times as
 * possible. The calling code should send as much data as it can with one call
 * to this function.
 *
 * @param[out] misoData where to store bytes that are read. If this is NULL,
 *     then read data will be ignored.
 * @param mosiData pointer to bytes to write.
 * @param count how many bytes to read/write.
 * @param chipSelect if @c true then the chipSelect line is enabled before
 *     communicating, and disabled afterwards; otherwise the chipSelect line is
 *     not changed.
 **/
void ftSpi_readWrite(uint8_t* misoData, const uint8_t* mosiData,
                     uint32_t count, bool chipSelect);


/** Brings chip select low. (Also, note that SCK will be low). **/
void ftSpi_enableChipSelect(void);


/** Brings chip select high. (Also, note that SCK will be low) **/
void ftSpi_disableChipSelect(void);


/** Closes the FT232R device. Should be called before the program exits. **/
void ftSpi_close(void);


#endif

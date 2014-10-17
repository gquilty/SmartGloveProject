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
 * Functions for accessing reading and writing to SPI EEPROM. This is on chip
 * for ATmegas, and on the transceiver board for 10mm modules (upper half).
 *
 * @file eeprom_mcu.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef EEPROM_MCU_H
#define EEPROM_MCU_H


/** The last address of the EEPROM (for 4kbyte EEPROM). **/
#define EEPROM_MAX_ADDRESS  4095
//#define EEPROM_MAX_ADDRESS  8191

/**
 * Writes to the EEPROM.
 *
 * @param data pointer to bytes to write.
 * @param start where to start writing from.
 * @param end where to stop writing.
 **/
void eeprom_mcu_write(const uint8_t* data, uint16_t start, uint16_t end);


/**
 * Reads from the EEPROM.
 *
 * @param[out] data where to put data that is read.
 * @param start where to start reading from.
 * @param end where to stop reading.
 **/
void eeprom_mcu_read(uint8_t* data, uint16_t start, uint16_t end);


#endif

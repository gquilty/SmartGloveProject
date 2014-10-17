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
 * Suport for application reprogramming. This reprogramming system works by
 * keeping a copy of the current application. This copy can be modified while
 * the application is still running. These modifications can be based on
 * commands recevied from UART or RF. When the modifications are complete, the
 * new code is copied over the old code, and the microcontroller is reset. The
 * actual code that does this copying, cannot be part of the application as it
 * would be corrupted by the copying. Therefore for the ATmega, this code is
 * kept in the bootloader section of memory, and cannot be modified. For the
 * nRF9E5, there is no problem, as when it powers on, it copies all the program
 * code into internal RAM, so the external EEPROM can be modified easily.
 *
 * @file reprog.h
 * @date 16-Apr-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef REPROG_H
#define REPROG_H


/**
 * Function for moving sections of code. Code can be moved up or down, and
 * overlapping is not an issue.
 *
 * @param oldAddress curent location of code.
 * @param newAddress new location of code.
 * @param count how many bytes to move.
 **/
void reprog_move(uint16_t oldAddress, uint16_t newAddress, uint16_t count);


/**
 * Reads from a section of code memory.
 *
 * @param address where to read.
 * @param[out] data where to store data that is read.
 * @param count how many bytes to read.
 **/
void reprog_read(uint16_t address, uint8_t* data, uint8_t count);


/**
 * Writes to a section of code memory.
 *
 * @param address where to write.
 * @param data what to write
 * @param count how many bytes to write.
 **/
void reprog_write(uint16_t address, const uint8_t* data, uint8_t count);


#ifndef UC_AVR
uint8_t reprog_restart(void);
#else
/**
 * Restarts microcontroller with new application.
 *
 * @note (AVR only)
 * @verbatim __attribute__ ((section (".bootloader")) @endverbatim
 * puts this function in the @e .bootloader section of memory. The location of 
 * this is controlled in the @e makefile.
 * @verbatim __attribute__((externally_visible))@endverbatim
 * is needed to stop merging this function with main application code when using
 * the @c -fwhole-program compiler option.
 *
 * @return returns only if there is a problem (CRC doesn't match).
 **/
__attribute__ ((section (".bootloader"))) __attribute__((externally_visible))
uint8_t reprog_restart(void);
#endif



#endif

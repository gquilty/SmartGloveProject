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
 * Some defintions that can be used by all files.
 *
 * @file common.h
 * @date 27-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef COMMON_H
#define COMMON_H


#include <stdint.h>
#include <stdbool.h>


#ifdef __WIN32__
#   include <windows.h>
#else
#   include <time.h>
#endif


/** Maximum size of any memory file (1MB). **/
#define MAX_MEMORY_SIZE     1048576

/** Baudrate to use for slow devices **/
#define SLOW_BAUDRATE       9600

/** Num of characters in one row of command window, (for progress bar). **/
#define CMD_WINDOW_WIDTH    80

/**
 * Sets the maximum number of errors that will be printed by the verification
 * process before quitting. 0 = infinite
 **/
#define MAX_VERIFY_ERRORS   10

/** String in FTDI EEPROM on 25mm programming boards. **/
#define DESC_25MM_PROG      "Tyndall 25mm USB Programmer"

/** String in FTDI EEPROM on 10mm programming boards. **/
#define DESC_10MM_PROG      "Tyndall 10mm Interface"

#define STR_HELPER(x)       #x
#define STRING(x)           STR_HELPER(x)
/** Macro to build filename:linenumber string for debugging. **/
#define LOC                 __FILE__":"STRING(__LINE__)

/**
 * Returns bit vector e.g. @c BV(2) is 00000010.
 * @param bit number of places to shift bit left.
 **/
#define BV(bit)    (1 << (bit))


/**
 * Strips the path from a file location string.
 *
 * @param pathString filename including path.
 * @return pointer to filename within @c pathString.
 **/
static inline const char* stripPath(const char* pathString)
{
    const char* filename;

    /* Strip the path (everything upto and including last '/' or '\') */
    for (filename = pathString; *pathString != '\0'; pathString++) {
        if (*pathString == '\\' || *pathString == '/') {
            filename = pathString + 1;
        }
    }
    return filename;
}


/**
 * Returns high byte of a two byte integer.
 *
 * @param word input word.
 * @return high byte.
 **/
static inline uint8_t getHighByte(uint16_t word)
{
    return (uint8_t)(word >> 8);
}

/** Returns low byte of a two byte integer.
 *
 * @param word input word.
 * @return low byte.
 **/
static inline uint8_t getLowByte(uint16_t word)
{
    return (uint8_t)word;
}

/**
 * Delays for a number of milliseconds.
 *
 * @param delay how many milliseconds to delay.
 **/
static inline void sleep_ms(uint32_t delay)
{
#ifdef __WIN32__
    Sleep(delay);
#else
#warning "Delay code for non win32 platforms is not tested!"
    struct timespec nanoDelay, remainingDelay;
    nanoDelay.tv_sec = delay / 1000;
    delay = delay % 1000;
    nanoDelay.tv_nsec = delay * 1e6;
    while (-1 == nanosleep(&nanoDelay, &remainingDelay)) {
        nanoDelay = remainingDelay;
    }
#endif
}


/** Different types of memory that can be programmed. **/
typedef enum {
    MEM_NRF9E5 = 1, MEM_ATMEGA_FLASH, MEM_ATMEGA_EEPROM, MEM_ATMEGA_FUSES
} memType_t;


/**
 * Structure for describing the SPI device with a description string and
 * bitmasks for each of the pins.
 **/
typedef struct {
    char description[64];   /**< Description string. **/
    uint8_t ncs;      /**< Active low Chip Select. **/
    uint8_t sck;      /**< Serial Clock. **/
    uint8_t miso;     /**< data: Master In, Slave Out. **/
    uint8_t mosi;     /**< data: Master Out, Slave In. **/
    uint32_t baudRate;  /**< Baud rate of communications. **/
    uint32_t memorySize;      /**< Total size of memory, in bytes. **/
    uint32_t codeSize;        /**< Size of memory that can be used for code. **/

    /**
     * When supporting reprogramming, there will be large gaps in the code. This
     * array stores addresses, where data should be written. There can be 4
     * different writes, and the start address, and write count is stored.
     **/
    uint32_t writeAdds[4][2];

    /** Function programs bytes to the memory memory (data, address, size). **/
    void (*write)(const uint8_t*, uint32_t, uint32_t);

    /** Function reads bytes from the memory (data, address, size). **/
    void (*read)(uint8_t*, uint32_t, uint32_t);

    /** Function erases all data in the memory. **/
    void (*erase)(void);

    /** Function adds header info to code (data, size, isSupportReprog). **/
    void (*addHeader)(uint8_t*, uint32_t, bool);
} spiDeviceInfo_t;


#endif



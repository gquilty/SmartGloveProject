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
 * Functions to support reprogramming the ATmega while the application is
 * running. The Flash memory is split in half. The lower half is dedicated to
 * the current application and the upper half provides a storage space to store
 * a new application. When the new application is fully received, it is copied
 * into the lower half and the ATmega can be restarted.
 *
 * Only code in the bootloader section of the ATmega can write to flash. All the
 * code in this file is put into that section, and the code in this file, should
 * not call code in the main application section. Also, the @c -mcall-prologues
 * compiler option should not be used as it will generate 'jmp' to application
 * code. In order to check that the application is valid, information on
 * application size and a 16bit CRC are also stored.
 *
 * The ATmega fuses must be set so that the ATmega resets to address 0x00000,
 * and the bootloader size is 1 kByte. This bootloader cannot rewrite itself.
 * The diagram below shows the layout in memory:
 *
 * @code
 *      /--------------------\  0x20000 (128 kB)
 *      | Bootloader Section |
 *      |--------------------|  0x1F800 (127 kB)
 *      |        New         |
 *      |        Code        |
 *      |--------------------|  0x10000 (64 kB)
 *      | App. Info. Header  |
 *      |--------------------|  0x0F800 (63 kB)
 *      |       Current      |
 *      |        Code        |
 *      \--------------------/  0x00000 (0 kB)
 * @endcode
 *
 * @todo should optimise @c reprog_move() so that copies to take place on page
 * boundaries
 *
 * @file reprog_avr.c
 * @date 16-Apr-2010
 * @author Seán Harte
 ******************************************************************************/


#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/crc16.h>
#include "stdio.h"
#include "reprog.h"


#ifndef _AVR_IOM128_H_
#warning "Reprogramming may not work on non ATmega128 chips"
#endif

/* Locations in memory */
#define MEM_SIZE        0x20000UL
#define CODE_SIZE       0xF800UL
#define CODE_OFFSET     MEM_SIZE / 2
#define HEADER_OFFSET   CODE_SIZE

/* Macros for converting between words and bytes */
#define BYTE_LOW(x)             ((uint8_t)(x))
#define BYTE_HIGH(x)            ((uint8_t)((x) >> 8))
#define TO_UINT16(b1, b0)       (((uint16_t)(0xFF & b1) << 8) | \
                                ((uint16_t)(0xFF & b0) << 0))

/* Buffer used when copying memory */
uint8_t buffer[SPM_PAGESIZE];


/******************************************************************************\
 * Local function prototypes.
\******************************************************************************/

BOOTLOADER_SECTION
static uint8_t readFlashByte(uint32_t address);
BOOTLOADER_SECTION
static void readFlash(uint8_t* data, uint32_t start, uint32_t end);
BOOTLOADER_SECTION
static void writeFlash(const uint8_t* data, uint32_t start, uint32_t end);
BOOTLOADER_SECTION
static void writeFlashPage(const uint8_t* data, uint32_t start, uint32_t end);


/******************************************************************************\
 * See reprog.h for documentation of these functions.
\******************************************************************************/


void reprog_move(uint16_t oldAddress, uint16_t newAddress, uint16_t count)
{
    uint32_t oldAddr32, newAddr32;
    uint16_t tempCount;
    
    oldAddr32 = (uint32_t)oldAddress + CODE_OFFSET;
    newAddr32 = (uint32_t)newAddress + CODE_OFFSET;

    if (oldAddress > newAddress) {
        /* Copy SPM_PAGESIZE bytes at a time. Start with lower bytes */
        while (count > 0) {
            tempCount = (count <= SPM_PAGESIZE) ? count : SPM_PAGESIZE;
            readFlash(buffer, oldAddr32, oldAddr32 + tempCount - 1);
            writeFlash(buffer, newAddr32, newAddr32 + tempCount - 1);
            newAddr32 += tempCount;
            oldAddr32 += tempCount;
            count -= tempCount;
        }
    }
    else {
        oldAddr32 += count;
        newAddr32 += count;
        /* Copy SPM_PAGESIZE bytes at a time. Start with higher bytes */
        while (count > 0) {
            tempCount = (count <= SPM_PAGESIZE) ? count : SPM_PAGESIZE;
            readFlash(buffer, oldAddr32 - tempCount, oldAddr32 - 1);
            writeFlash(buffer, newAddr32 - tempCount, newAddr32 - 1);
            newAddr32 -= tempCount;
            oldAddr32 -= tempCount;
            count -= tempCount;
        }
    }
}


void reprog_read(uint16_t address, uint8_t* data, uint8_t count)
{
    uint32_t addr32 = address;

    /* If writing to code section */
    if (addr32 + count < HEADER_OFFSET) {
        addr32 += CODE_OFFSET;
    }
    /* If writing to application info header */
    else {
        addr32 += 4;
    }
    readFlash(data, addr32, addr32 + count - 1);
}


void reprog_write(uint16_t address, const uint8_t* data, uint8_t count)
{
    uint32_t addr32 = address;

    /* If writing to code section */
    if (addr32 + count < HEADER_OFFSET) {
        addr32 += CODE_OFFSET;
    }
    /* If writing to application info header */
    else {
        addr32 += 4;
    }
    writeFlash(data, addr32, addr32 + count - 1);
}


uint8_t reprog_restart(void)
{
    uint8_t sreg;
    uint16_t crcValue;
    uint16_t newLength, newCrc;

    /* Disable interrupts */
    sreg = SREG;
    cli();

    /* Read code length and crc */
    readFlash(buffer, HEADER_OFFSET + 4, HEADER_OFFSET + 7);
    newLength = TO_UINT16(buffer[1], buffer[0]);
    newCrc = TO_UINT16(buffer[3], buffer[2]);

    /* Calculate CRC and compare to stored value */
    crcValue = 0xFFFF;
    for (uint32_t i = 0; i < newLength ; i++) {
        crcValue = _crc16_update(crcValue, readFlashByte(i + CODE_OFFSET));
    }

    /* Only restart if CRC matches */
    if (newCrc != crcValue) {
        /* Re-enable interrupts, if they were enabled */
        SREG = sreg;
        return 0;
    }

    /* Copy CRC and length for new application */
    writeFlash(buffer, HEADER_OFFSET, HEADER_OFFSET + 3);

    /* Copy from upper half of memory to lower half */
    for (uint32_t addr = 0; addr < newLength; addr += SPM_PAGESIZE) {
        readFlash(buffer, CODE_OFFSET + addr,
                  CODE_OFFSET + addr + SPM_PAGESIZE - 1);
        writeFlash(buffer, addr, addr + SPM_PAGESIZE - 1);
    }

    /* Start watchdog timer */
    wdt_enable(WDTO_15MS);

    /* Wait for WDT to timeout */
    for(;;) {
        ;
    }
    return 0;
}


/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/

/**
 * Reads a single byte of flash
 *
 * @param address byte address of location to read.
 * @return byte that was read.
 **/
static uint8_t readFlashByte(uint32_t address)
{
        if (address <= 0xFFFF) {
            return pgm_read_byte_near((uint16_t)address);
        }
        else {
            return pgm_read_byte_far(address);
        }
}


/**
 * Writes to the flash memory.
 *
 * @param data what to write.
 * @param start where to start writing.
 * @param end where to stop writing.
 **/
static void writeFlash(const uint8_t* data, uint32_t start, uint32_t end)
{
    for (uint32_t pageEnd = start | (SPM_PAGESIZE - 1); start <= end;
                                                      pageEnd += SPM_PAGESIZE) {
        pageEnd = (pageEnd <= end) ? pageEnd : end;
        writeFlashPage(data, start, pageEnd);
        data += pageEnd - start + 1;
        start = pageEnd + 1;
    }
}


/**
 * Reads from the flash memory.
 *
 * @param data where to store data that is read.
 * @param start where to start reading.
 * @param end where to stop reading.
 **/
static void readFlash(uint8_t* data, uint32_t start, uint32_t end)
{
    for (uint32_t i = start; i <= end; i++) {
        data[i - start] = readFlashByte(i);
    }
}


/**
 * Writes within a single flash page.
 *
 * @param data what to write.
 * @param start where to start writing.
 * @param end where to stop writing.
 **/
static void writeFlashPage(const uint8_t* data, uint32_t start, uint32_t end)
{
    uint8_t sreg;
    uint16_t i = 0;
    uint32_t page = start & ~(SPM_PAGESIZE - 1);
    uint16_t pageStart = start & (SPM_PAGESIZE - 1);
    uint16_t pageEnd = end & (SPM_PAGESIZE - 1);
    uint16_t temp16;
    
    /* Disable interrupts */
    sreg = SREG;
    cli();

    /*
     * Fill page buffer with new data, If no new data is given for an address,
     * then the existing data is used.
     */
    while (i < SPM_PAGESIZE) {
        if ((i < pageStart) || (i > pageEnd)) {
            temp16 = readFlashByte(page + i);
        }
        else {
            temp16 = data[i - pageStart];
        }
        i++;
        if ((i < pageStart) || (i > pageEnd)) {
            temp16 |= (uint16_t)readFlashByte(page + i) << 8;
        }
        else {
            temp16 |= (uint16_t)data[i - pageStart] << 8;
        }
        boot_page_fill_safe(page + i, temp16);
        i++;
    }

    /* Erase page */
    boot_page_erase_safe(page);

    /* Write new data */
    boot_page_write_safe(page);

    boot_rww_enable_safe();

    /* Re-enable interrupts, if they were enabled */
    SREG = sreg;
}

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
 * Functions for writing and reading ATmega memory.
 *
 * @file atmega.c
 * @date 27-Jan-2010
 * @author Seán Harte
 *
 * See atmega.h for more documentation on this library and functions.
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "atmega.h"
#include "ftSpi.h"
#include "atmegaDevices.h"
#include "crc16.h"


/** Find number of chips specified in atmegaChips.h **/
#define NUM_SUPPORTED_CHIPS  ((sizeof(atmega_infoTable)/sizeof(uint32_t))/5)

/* Used as index when accessing atmega_infoTable */
#define IND_SIGBYTE                 0
#define IND_FLASHSIZE               1
#define IND_FLASHPAGESIZE           2
#define IND_EEPROMSIZE              3
#define IND_EEPROMPAGESIZE          4

/* SPI instructions. */
#define PROG_EN                     0xAC53
#define CHIP_ERASE                  0xAC80
#define POLL_RDY                    0xF000
#define READ_PROG_MEM_LOW           0x2000
#define READ_PROG_MEM_HIGH          0x2800
#define LOAD_PROG_MEM_PAGE_LOW      0x4000
#define LOAD_PROG_MEM_PAGE_HIGH     0x4800
#define WRITE_PROG_MEM_PAGE         0x4C00
#define READ_EEPROM_MEM             0xA000
#define WRITE_EEPROM_MEM            0xC000
#define READ_LOCK_BITS              0x5800
#define WRITE_LOCK_BITS             0xACE0
#define READ_SIG_BYTE               0x3000
#define WRITE_FUSE_BITS             0xACA0
#define WRITE_FUSE_HIGH_BITS        0xACA8
#define WRITE_FUSE_EXT_BITS         0xACA4
#define READ_FUSE_BITS              0x5000
#define READ_FUSE_EXT_BITS          0x5008
#define READ_FUSE_HIGH_BITS         0x5808
#define READ_CALIB_BYTE             0x3800

/* Addresses in Memory */
#define REPROG_CODE_SIZE            0xF800
#define REPROG_CODE_OFFSET          info.memorySize / 2
#define REPROG_HEADER_OFFSET        REPROG_CODE_SIZE
#define REPROG_HEADER_SIZE          0x08
#define REPROG_BOOTLOADER_OFFSET    0x1F800UL

/* Use these to index three fuse bytes */
#define FUSE_LOW                    2
#define FUSE_HIGH                   1
#define FUSE_EXT                    0

/* Delay values in ms (about 200% of values in the datasheet)*/
#define DELAY_INIT                  40
#define DELAY_FLASH                 10
#define DELAY_EEPROM                20
#define DELAY_ERASE                 20

/** Bit mask for pin that is connected to the chip select. **/
#define PIN_NCS_25MM                0x08

/** Bit mask for pin that is connected to the SPI clock. **/
#define PIN_SCK_25MM                0x04

/** Bit mask for pin that is connected to UART0 RX. **/
#define PIN_RX_25MM                 0x01

/** Bit mask for pin that is connected to UART0 TX. **/
#define PIN_TX_25MM                 0x02

/** Bit mask for pin that is connected to MISO. **/
#define PIN_MISO_25MM               0x40

/** Bit mask for pin that is connected to MOSI. **/
#define PIN_MOSI_25MM               0x10


/** Page size of memory. This is set by @c atmega_init() **/
static uint32_t pageSize;

/**
 * Structure storing details of this programmer.
 *
 * Note about clock edge: Atmel documentation says that when data is written to
 * the ATmega, it is clocked on the rising edge. When data is being read from
 * the ATmega, it is clocked on the falling edge. However it seems to work ok,
 * clocking on the rising edge in both directions.
 **/
static spiDeviceInfo_t info = {
    DESC_25MM_PROG,                 /* Description. */
    PIN_NCS_25MM,                   /* nCS pin. */
    PIN_SCK_25MM,                   /* SCK pin. */
    0x00,                           /* MISO pin (set later). */
    0x00,                           /* MOSI pin (set later). */
    FTSPI_MAX_BAUDRATE,             /* Baud rate. */
    0,                              /* Memory Size (set later). */
    0,                              /* Code Size (set later). */
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}},   /* Write addresses (set later). */
    atmega_write,
    atmega_read,
    atmega_erase,
    atmega_addHeader,
};

static memType_t memType;
static uint32_t sigBytes = 0;

/* Prototypes for local functions. */
static void writeFlashPage(const uint8_t* data, uint32_t start, uint32_t end);
static void readFlash(uint8_t* data, uint32_t address, uint32_t size);
static void writeEeprom(const uint8_t* data, uint32_t address, uint32_t size);
static void writeFlash(const uint8_t* data, uint32_t address, uint32_t size);
static void readEeprom(uint8_t* data, uint32_t address, uint32_t size);
static void writeFuses(const uint8_t data[3]);
static void readFuses(uint8_t data[3]);
static bool initAtmega(void);
static void eraseFlash(void);
static void eraseEeprom(void);
static uint32_t readSigBytes(void);
static void spiInstruction(uint8_t bytes[4]);


/******************************************************************************\
 * Functions that be used externally. Documentation in atmega.h.
 */


spiDeviceInfo_t* atmega_init(memType_t memory, bool isSlowClock)
{
    /* Remember what type of memory is being programmed */
    memType = memory;

    uint32_t i;

    /* ATmega128 uses UART pins for SPI programming. Try this first */
    info.miso = PIN_TX_25MM;
    info.mosi = PIN_RX_25MM;

    if (isSlowClock) {
        info.baudRate = SLOW_BAUDRATE;
    }

    /* Initialise SPI comms with these pins */
    ftSpi_init(&info);

    if (!initAtmega()) {
        /* If first selection of pins didn't work, try with SPI pins: */
        info.miso = PIN_MISO_25MM;
        info.mosi = PIN_MOSI_25MM;

        ftSpi_close();
        ftSpi_init(&info);
        if (!initAtmega()) {
            fprintf(stderr,
                    "ERROR: Could not communicate with ATmega. Check connections and power\n");
            exit(EXIT_FAILURE);
        }
    }

    sigBytes = readSigBytes();

    printf(" ATmega signature bytes: %.6X", sigBytes);

    /* Set memory and page sizes */
    for (i = 0; i < NUM_SUPPORTED_CHIPS; ++i) {
        if (sigBytes == atmega_infoTable[i][IND_SIGBYTE]) {
            printf(" = %s.\n", atmega_strings[i]);
            if (memType == MEM_ATMEGA_FLASH) {
                info.memorySize = atmega_infoTable[i][IND_FLASHSIZE];
                info.codeSize = atmega_infoTable[i][IND_FLASHSIZE];
                pageSize = atmega_infoTable[i][IND_FLASHPAGESIZE];
            }
            else if (memType == MEM_ATMEGA_EEPROM) {
                info.memorySize = atmega_infoTable[i][IND_EEPROMSIZE];
                info.codeSize = atmega_infoTable[i][IND_EEPROMSIZE];
                pageSize = atmega_infoTable[i][IND_EEPROMPAGESIZE];
            }
            else if (memType == MEM_ATMEGA_FUSES) {
                info.memorySize = 3;
                info.codeSize = 3;
                pageSize = 3;
            }
            else {
                fprintf(stderr, "\nERROR: Invalid memType: %X.\n", memType);
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    if (info.memorySize == 0) {
        fprintf(stderr,
                "\nERROR: Unrecognised Atmel signature bytes. Supported chips are:\n");
        for (i = 0; i < NUM_SUPPORTED_CHIPS; i++) {
            fprintf(stderr, "       %s, 0x%X\n",
                    atmega_strings[i], atmega_infoTable[i][IND_SIGBYTE]);
        }
        exit(EXIT_FAILURE);
    }

    return &info;
}


void atmega_write(const uint8_t* data, uint32_t address, uint32_t size)
{
    static bool isCalled = false;

    /* Check if address is valid */
    if (address + size > info.memorySize) {
        fprintf(stderr, "ERROR: data size too big (%u) at %s.\n", size,
                stripPath(LOC));
        exit(EXIT_FAILURE);
    }

    switch(memType) {
    case MEM_ATMEGA_FLASH:  if (isCalled == false) {
                                eraseFlash();       /* Only erase once */
                                isCalled = true;
                            }
                            writeFlash(data, address, size);
                            break;

    case MEM_ATMEGA_EEPROM: writeEeprom(data, address, size);
                            break;

    case MEM_ATMEGA_FUSES:  writeFuses(data);
                            break;

    default:                fprintf(stderr, "ERROR: Invalid memType: %X\n",
                                    memType);
                            exit(EXIT_FAILURE);
    }
}


void atmega_read(uint8_t* data, uint32_t address, uint32_t size)
{
    /* Check if address is valid */
    if (address + size > info.memorySize) {
        fprintf(stderr, "ERROR: data size too big (%u) at %s.\n", size,
                stripPath(LOC));
        exit(EXIT_FAILURE);
    }

    switch(memType) {
    case MEM_ATMEGA_FLASH:  readFlash(data, address, size);
                            break;

    case MEM_ATMEGA_EEPROM: readEeprom(data, address, size);
                            break;

    case MEM_ATMEGA_FUSES:  readFuses(data);
                            break;

    default:                fprintf(stderr, "ERROR: Invalid memType: %X\n",
                            memType);
                            exit(EXIT_FAILURE);
    }
}


void atmega_erase(void)
{
    switch(memType) {
    case MEM_ATMEGA_FLASH:
        eraseFlash();
        break;

    case MEM_ATMEGA_EEPROM:
        eraseEeprom();
        break;

    case MEM_ATMEGA_FUSES:
        fprintf(stderr, "WARNING: Fuses cannot be erased.");
        break;

    default:
        fprintf(stderr, "ERROR: Invalid memType: %X\n", memType);
        exit(EXIT_FAILURE);
    }
}


void atmega_addHeader(uint8_t* data, uint32_t rawCodeSize,
                      bool isSupportReprog)
{
    uint32_t i;
    uint32_t codeEnd = 0;

    /* Do nothing if reprogramming support is not needed */
    if (!isSupportReprog) {
        info.writeAdds[0][0] = 0;
        info.writeAdds[0][1] = rawCodeSize;
        return;
    }

    /* Set actual size available for code */
    info.codeSize = REPROG_CODE_SIZE;

    /* Find actual location of code end and bootloader end */
    for (i = info.codeSize; i > 0; i--) {
        if (data[i] == 0x00 || data[i] == 0xFF) codeEnd = i;
        else break;
    }

    /* Location of last byte with data */
    codeEnd--;

    /* Set up locations that need to be written to */
    info.writeAdds[0][0] = 0;
    info.writeAdds[0][1] = codeEnd;
    info.writeAdds[1][0] = REPROG_HEADER_OFFSET;
    info.writeAdds[1][1] = REPROG_HEADER_SIZE;
    info.writeAdds[2][0] = info.memorySize / 2;
    info.writeAdds[2][1] = codeEnd;
    info.writeAdds[3][0] = REPROG_BOOTLOADER_OFFSET;
    info.writeAdds[3][1] = rawCodeSize - REPROG_BOOTLOADER_OFFSET;

    /* Copy code data to upper section */
    for (i = 0; i < codeEnd; i++) {
        data[i + (info.memorySize / 2)] = data[i];
    }

    /* Calculate CRC-16 value for program code */
    crc16_init();
    for (i = 0; i < codeEnd; i++) {
        crc16_update(data[i]);
    }

    /* Write application header */
    data[REPROG_HEADER_OFFSET] = data[REPROG_HEADER_OFFSET + 4] =
                                                            getLowByte(codeEnd);

    data[REPROG_HEADER_OFFSET + 1] = data[REPROG_HEADER_OFFSET + 5] =
                                                           getHighByte(codeEnd);

    data[REPROG_HEADER_OFFSET + 2] = data[REPROG_HEADER_OFFSET + 6] =
                                                       getLowByte(crc16_read());

    data[REPROG_HEADER_OFFSET + 3] = data[REPROG_HEADER_OFFSET + 7] =
                                                      getHighByte(crc16_read());

}


void atmega_reset(void)
{
    ftSpi_enableChipSelect();
    sleep_ms(20);
    ftSpi_disableChipSelect();
}


void atmega_parseFuseValues(const char* fuseString, uint8_t data[3])
{
    bool isError = false;
    int values[3];   /* sscanf wants to write to integers */

    /* Check string length */
    if (strlen(fuseString) != 6) {
        isError = true;
    }
    else {
        /* Convert and test if conversion was successful */
        if (3 != sscanf(fuseString, "%2X%2X%2X",
                    &values[FUSE_EXT], &values[FUSE_HIGH], &values[FUSE_LOW])) {
            isError = true;
        }
    }
    if (isError) {
        fprintf(stderr,
                "ERROR: Fuse must be a 6 hexadecimal digits, e.g. FF91E4.\n\n");
        exit(EXIT_FAILURE);
    }

    /* Prevent user from disabling serial interface */
    if ((values[1] & 0x20) != 0) {
        printf("WARNING: Cannot disable SPI programming interface."
               "         Use a JTAG programmer if you really want to do this.\n");
        values[1] |= 0x20;
    }

    data[FUSE_EXT] = values[FUSE_EXT];
    data[FUSE_HIGH] = values[FUSE_HIGH];
    data[FUSE_LOW] = values[FUSE_LOW];
}


void atmega_printFuseInfo(uint8_t fuses[3])
{
    printf(" Fuses: %.2X%.2X%.2X\n\n", fuses[0], fuses[1], fuses[2]);

    if (sigBytes == ATMEGA_ATMEGA128_SIG) {
        printf("  Brown-out detection level at VCC = ");
        printf((fuses[FUSE_LOW] & 0x80) ? "2.7V\n" : "4.0V \n");

        printf("  Brown out detection ");
        printf((fuses[FUSE_LOW] & 0x40) ? "disabled\n" : "enabled\n");

        switch (fuses[FUSE_LOW] & 0x3F) {
            case 0x00: printf("  Ext. clock. Startup: 6CK + 0ms"); break;
            case 0x10: printf("  Ext. clock. Startup: 6CK + 4ms"); break;
            case 0x20: printf("  Ext. clock. Startup: 6CK + 64ms"); break;
            case 0x01: printf("  Int. RC osc. 1MHz. Startup: 6K + 0ms"); break;
            case 0x11: printf("  Int. RC osc. 1MHz. Startup: 6CK + 4ms"); break;
            case 0x21: printf("  Int. RC osc. 1MHz. Startup: 6CK + 64ms"); break;
            case 0x02: printf("  Int. RC osc. 2MHz. Startup: 6CK + 0ms"); break;
            case 0x12: printf("  Int. RC osc. 2MHz. Startup: 6CK + 4ms"); break;
            case 0x22: printf("  Int. RC osc. 2MHz. Startup: 6CK + 64ms"); break;
            case 0x03: printf("  Int. RC osc. 4MHz. Startup: 6CK + 0ms"); break;
            case 0x13: printf("  Int. RC osc. 4MHz. Startup: 6CK + 4ms"); break;
            case 0x23: printf("  Int. RC osc. 4MHz. Startup: 6CK + 64ms"); break;
            case 0x04: printf("  Int. RC osc. 8MHz. Startup: 6CK + 0ms"); break;
            case 0x14: printf("  Int. RC osc. 8MHz. Startup: 6CK + 4ms"); break;
            case 0x24: printf("  Int. RC osc. 8MHz. Startup: 6CK + 64ms"); break;
            case 0x05: printf("  Ext. RC osc. < 0.9MHz. Startup: 18CK + 0ms"); break;
            case 0x15: printf("  Ext. RC osc. < 0.9MHz. Startup: 18CK + 4ms"); break;
            case 0x25: printf("  Ext. RC osc. < 0.9MHz. Startup: 18CK + 64ms"); break;
            case 0x35: printf("  Ext. RC osc. < 0.9MHz. Startup: 6CK + 4ms"); break;
            case 0x06: printf("  Ext. RC osc. 0.9 - 3.0MHz. Startup: 18CK + 0ms"); break;
            case 0x16: printf("  Ext. RC osc. 0.9 - 3.0MHz. Startup: 18CK + 4ms"); break;
            case 0x26: printf("  Ext. RC osc. 0.9 - 3.0MHz. Startup: 18CK + 64ms"); break;
            case 0x36: printf("  Ext. RC osc. 0.9 - 3.0MHz. Startup: 6CK + 4ms"); break;
            case 0x07: printf("  Ext. RC osc. 3.0 - 8.0MHz. Startup: 18CK + 0ms"); break;
            case 0x17: printf("  Ext. RC osc. 3.0 - 8.0MHz. Startup: 18CK + 4ms"); break;
            case 0x27: printf("  Ext. RC osc. 3.0 - 8.0MHz. Startup: 18CK + 64ms"); break;
            case 0x37: printf("  Ext. RC osc. 3.0 - 8.0MHz. Startup: 6CK + 4ms"); break;
            case 0x08: printf("  Ext. RC osc. 8.0 - 12.0MHz. Startup: 18CK + 0ms"); break;
            case 0x18: printf("  Ext. RC osc. 8.0 - 12.0MHz. Startup: 18CK + 4ms"); break;
            case 0x28: printf("  Ext. RC osc. 8.0 - 12.0MHz. Startup: 18CK + 64ms"); break;
            case 0x38: printf("  Ext. RC osc. 8.0 - 12.0MHz. Startup: 6CK + 4ms;"); break;
            case 0x09: printf("  Ext. low-freq. crystal. startup. 1000CK + 4ms"); break;
            case 0x19: printf("  Ext. low-freq. crystal. startup. 1000CK + 64ms"); break;
            case 0x29: printf("  Ext. low-freq. crystal. startup. 32000CK + 64ms"); break;
            case 0x0A: printf("  Ext. crystal/resonator low freq. Startup: 258CK + 4ms"); break;
            case 0x1A: printf("  Ext. crystal/resonator low freq. Startup: 258CK + 64ms"); break;
            case 0x2A: printf("  Ext. crystal/resonator low freq. Startup: 1000CK + 0ms"); break;
            case 0x3A: printf("  Ext. crystal/resonator low freq. Startup: 1000CK + 4ms"); break;
            case 0x0B: printf("  Ext. crystal/resonator low freq. Startup: 1000CK + 64ms"); break;
            case 0x1B: printf("  Ext. crystal/resonator low freq. Startup: 16000CK + 0ms"); break;
            case 0x2B: printf("  Ext. crystal/resonator low freq. Startup: 16000CK + 4ms"); break;
            case 0x3B: printf("  Ext. crystal/resonator low freq. Startup: 16000CK + 64ms"); break;
            case 0x0C: printf("  Ext. crystal/resonator medium freq. Startup: 258CK + 4ms"); break;
            case 0x1C: printf("  Ext. crystal/resonator medium freq. Startup: 258CK + 64ms"); break;
            case 0x2C: printf("  Ext. crystal/resonator medium freq. Startup: 1000CK + 0ms"); break;
            case 0x3C: printf("  Ext. crystal/resonator medium freq. Startup: 1000CK + 4ms"); break;
            case 0x0D: printf("  Ext. crystal/resonator medium freq. Startup: 1000CK + 64ms"); break;
            case 0x1D: printf("  Ext. crystal/resonator medium freq. Startup: 16000CK + 0ms"); break;
            case 0x2D: printf("  Ext. crystal/resonator medium freq. Startup: 16000CK + 4ms"); break;
            case 0x3D: printf("  Ext. crystal/resonator medium freq. Startup: 16000CK + 64ms"); break;
            case 0x0E: printf("  Ext. crystal/resonator high freq. Startup: 258CK + 4ms"); break;
            case 0x1E: printf("  Ext. crystal/resonator high freq. Startup: 258CK + 64ms"); break;
            case 0x2E: printf("  Ext. crystal/resonator high freq. Startup: 1000CK + 0ms"); break;
            case 0x3E: printf("  Ext. crystal/resonator high freq. Startup: 1000CK + 4ms"); break;
            case 0x0F: printf("  Ext. crystal/resonator high freq. Startup: 1000CK + 64ms"); break;
            case 0x1F: printf("  Ext. crystal/resonator high freq. Startup: 16000CK + 0ms"); break;
            case 0x2F: printf("  Ext. crystal/resonator high freq. Startup: 16000CK + 4ms"); break;
            case 0x3F: printf("  Ext. crystal/resonator high freq. Startup: 16000CK + 64ms"); break;
            default: printf("  Clock Source = 0x%.6X", fuses[FUSE_LOW] & 0x3F);
        }
        printf("\n");

        printf("  On-Chip Debug ");
        printf((fuses[FUSE_HIGH] & 0x80) ? "disabled\n" : "enabled\n");

        printf("  JTAG " );
        printf((fuses[FUSE_HIGH] & 0x40) ? "disabled\n" : "enabled\n");

        printf("  Serial program downloading (SPI) ");
        printf((fuses[FUSE_HIGH] & 0x20) ? "disabled\n" : "enabled\n");

        printf("  Preserve EEPROM memory through the Chip Erase cycle: ");
        printf((fuses[FUSE_HIGH] & 0x08) ? "no\n" : "yes\n");

        printf("  Boot Flash section size: ");
        switch (fuses[FUSE_HIGH] & 0x06) {
            case 0x00: printf("8192"); break;
            case 0x02: printf("4096"); break;
            case 0x04: printf("2048"); break;
            case 0x06: printf("1024");  break;
        }

        printf("bytes. Boot start address: ");
        switch (fuses[FUSE_HIGH] & 0x06) {
            case 0x00: printf("0xF000\n"); break;
            case 0x02: printf("0xF800\n"); break;
            case 0x04: printf("0xFC00\n"); break;
            case 0x06: printf("0xFE00\n");  break;
        }

        printf("  Boot reset vector ");
        printf((fuses[FUSE_HIGH] & 0x01) ? "disabled\n" : "enabled\n");

        printf("  CKOPT = ");
        printf((fuses[FUSE_HIGH] & 0x08) ? "disabled\n" : "enabled\n");

        printf("  ATmega103 compatibility mode ");
        printf((fuses[FUSE_EXT] & 0x02) ? "disabled\n" : "enabled\n");

        printf("  Watchdog timer always on = ");
        printf((fuses[FUSE_EXT] & 0x01) ? "no\n" : "yes\n");
    }
    else if (sigBytes == ATMEGA_ATMEGA1281_SIG) {
        printf("  Divide clock by 8 internally: ");
        printf((fuses[FUSE_LOW] & 0x80) ? "no\n" : "yes\n");

        printf("  Clock output on PORTE7: ");
        printf((fuses[FUSE_LOW] & 0x40) ? "no\n" : "yes\n");

        switch (fuses[FUSE_LOW] & 0x3F) {
            case 0x00: printf("  Ext. clock. Startup: 6CK + 0ms"); break;
            case 0x10: printf("  Ext. clock. Startup: 6CK + 4.1ms"); break;
            case 0x20: printf("  Ext. clock. Startup: 6CK + 65ms"); break;
            case 0x02: printf("  Int. RC osc. Startup: 6CK + 0ms"); break;
            case 0x12: printf("  Int. RC osc. Startup: 6CK + 4.1ms"); break;
            case 0x22: printf("  Int. RC osc. Startup: 6CK + 65ms"); break;
            case 0x03: printf("  Int. 128kHz RC osc. Startup: 6CK + 0ms"); break;
            case 0x13: printf("  Int. 128kHz RC osc. Startup: 6CK + 4ms"); break;
            case 0x23: printf("  Int. 128kHz RC osc. Startup: 6CK + 64ms"); break;
            case 0x04: printf("  Ext. low-freq. crystal. Startup: 1000CK + 0ms"); break;
            case 0x14: printf("  Ext. low-freq. crystal. Startup: 1000CK + 4.1ms"); break;
            case 0x24: printf("  Ext. low-freq. crystal. Startup: 1000CK + 65ms"); break;
            case 0x05: printf("  Ext. low-freq. crystal. Startup: 32000CK + 0ms"); break;
            case 0x15: printf("  Ext. low-freq. crystal. Startup: 32000CK + 4.1ms"); break;
            case 0x25: printf("  Ext. low-freq. crystal. Startup: 32000CK + 65ms"); break;
            case 0x06: printf("  Full swing osc. Startup: 258CK + 4.1ms"); break;
            case 0x16: printf("  Full swing osc. Startup: 258CK + 65ms"); break;
            case 0x26: printf("  Full swing osc. Startup: 1000CK + 0ms"); break;
            case 0x36: printf("  Full swing osc. Startup: 1000CK + 4.1ms"); break;
            case 0x07: printf("  Full swing osc. Startup: 1000CK + 65ms"); break;
            case 0x17: printf("  Full swing osc. Startup: 16000CK + 0ms"); break;
            case 0x27: printf("  Full swing osc. Startup: 16000CK + 4.1ms"); break;
            case 0x37: printf("  Full swing osc. Startup: 16000CK + 65ms"); break;
            case 0x08: printf("  Ext. crystal osc. 0.4 - 0.9MHz. Startup: 258CK + 4.1ms"); break;
            case 0x18: printf("  Ext. crystal osc. 0.4 - 0.9MHz. Startup: 258CK + 65ms"); break;
            case 0x28: printf("  Ext. crystal osc. 0.4 - 0.9MHz. Startup: 1000CK + 0ms"); break;
            case 0x38: printf("  Ext. crystal osc. 0.4 - 0.9MHz. Startup: 1000CK + 4.1ms"); break;
            case 0x09: printf("  Ext. crystal osc. 0.4 - 0.9MHz. Startup: 1000CK + 65ms"); break;
            case 0x19: printf("  Ext. crystal osc. 0.4 - 0.9MHz. Startup: 16000CK + 0ms"); break;
            case 0x29: printf("  Ext. crystal osc. 0.4 - 0.9MHz. Startup: 16000CK + 4.1ms"); break;
            case 0x39: printf("  Ext. crystal osc. 0.4 - 0.9MHz. Startup: 16000CK + 65ms"); break;
            case 0x0A: printf("  Ext. crystal osc. 0.9 - 3.0MHz. Startup: 258CK + 4.1ms"); break;
            case 0x1A: printf("  Ext. crystal osc. 0.9 - 3.0MHz. Startup: 258CK + 65ms"); break;
            case 0x2A: printf("  Ext. crystal osc. 0.9 - 3.0MHz. Startup: 1000CK + 0ms"); break;
            case 0x3A: printf("  Ext. crystal osc. 0.9 - 3.0MHz. Startup: 1000CK + 4.1ms"); break;
            case 0x0B: printf("  Ext. crystal osc. 0.9 - 3.0MHz. Startup: 1000CK + 65ms"); break;
            case 0x1B: printf("  Ext. crystal osc. 0.9 - 3.0MHz. Startup: 16000CK + 0ms"); break;
            case 0x2B: printf("  Ext. crystal osc. 0.9 - 3.0MHz. Startup: 16000CK + 4.1ms"); break;
            case 0x3B: printf("  Ext. crystal osc. 0.9 - 3.0MHz. Startup: 16000CK + 65ms"); break;
            case 0x0C: printf("  Ext. crystal osc. 3.0 - 8.0MHz. Startup: 258CK + 4.1ms"); break;
            case 0x1C: printf("  Ext. crystal osc. 3.0 - 8.0MHz. Startup: 258CK + 65ms"); break;
            case 0x2C: printf("  Ext. crystal osc. 3.0 - 8.0MHz. Startup: 1000CK + 0ms"); break;
            case 0x3C: printf("  Ext. crystal osc. 3.0 - 8.0MHz. Startup: 1000CK + 4.1ms"); break;
            case 0x0D: printf("  Ext. crystal osc. 3.0 - 8.0MHz. Startup: 1000CK + 65ms"); break;
            case 0x1D: printf("  Ext. crystal osc. 3.0 - 8.0MHz. Startup: 16000CK + 0ms"); break;
            case 0x2D: printf("  Ext. crystal osc. 3.0 - 8.0MHz. Startup: 16000CK + 4.1ms"); break;
            case 0x3D: printf("  Ext. crystal osc. 3.0 - 8.0MHz. Startup: 16000CK + 65ms"); break;
            case 0x0E: printf("  Ext. crystal osc. > 8.0MHz. Startup: 258CK + 4.1ms"); break;
            case 0x1E: printf("  Ext. crystal osc. > 8.0MHz. Startup: 258CK + 65ms"); break;
            case 0x2E: printf("  Ext. crystal osc. > 8.0MHz. Startup: 1000CK + 0ms"); break;
            case 0x3E: printf("  Ext. crystal osc. > 8.0MHz. Startup: 1000CK + 4.1ms"); break;
            case 0x0F: printf("  Ext. crystal osc. > 8.0MHz. Startup: 1000CK + 65ms"); break;
            case 0x1F: printf("  Ext. crystal osc. > 8.0MHz. Startup: 16000CK + 0ms"); break;
            case 0x2F: printf("  Ext. crystal osc. > 8.0MHz. Startup: 16000CK + 4.1ms"); break;
            case 0x3F: printf("  Ext. crystal osc. > 8.0MHz. Startup: 16000CK + 65ms"); break;
            default: printf("  Clock Source = 0x%.6X", fuses[FUSE_LOW] & 0x3F);
        }
        printf("\n");

        printf("  On-Chip Debug ");
        printf((fuses[FUSE_HIGH] & 0x80) ? "disabled\n" : "enabled\n");

        printf("  JTAG " );
        printf((fuses[FUSE_HIGH] & 0x40) ? "disabled\n" : "enabled\n");

        printf("  Serial program downloading (SPI) ");
        printf((fuses[FUSE_HIGH] & 0x20) ? "disabled\n" : "enabled\n");

        printf("  Watchdog timer always on = ");
        printf((fuses[FUSE_EXT] & 0x10) ? "no\n" : "yes\n");

        printf("  Preserve EEPROM memory through the Chip Erase cycle: ");
        printf((fuses[FUSE_HIGH] & 0x08) ? "no\n" : "yes\n");

        printf("  Boot Flash section size: ");
        switch (fuses[FUSE_HIGH] & 0x06) {
            case 0x00: printf("8192"); break;
            case 0x02: printf("4096"); break;
            case 0x04: printf("2048"); break;
            case 0x06: printf("1024");  break;
        }

        printf("bytes. Boot start address: ");
        switch (fuses[FUSE_HIGH] & 0x06) {
            case 0x00: printf("0xF000\n"); break;
            case 0x02: printf("0xF800\n"); break;
            case 0x04: printf("0xFC00\n"); break;
            case 0x06: printf("0xFE00\n");  break;
        }

        printf("  Boot reset vector ");
        printf((fuses[FUSE_HIGH] & 0x01) ? "disabled\n" : "enabled\n");

        switch (fuses[FUSE_EXT] & 0x07) {
            case 0x07: printf("  Brown-out detection disabled\n"); break;
            case 0x06: printf("  Brown-out detection level at VCC = 1.8V\n"); break;
            case 0x05: printf("  Brown-out detection level at VCC = 2.7V\n"); break;
            case 0x04: printf("  Brown-out detection level at VCC = 4.3V\n");  break;
        }
    }
}


/******************************************************************************\
 * Functions that can only be used within this file
 */


/**
 * Initialises the atmega into programming mode.
 *
 * @return @c true if successful, @c false if not.
 **/
static bool initAtmega(void)
{
    uint8_t command[4];
    uint8_t readBytes[4];
    
    ftSpi_enableChipSelect();
    sleep_ms(DELAY_INIT);
    ftSpi_disableChipSelect();

    /* Wait at least 2 clock cycles */
    sleep_ms(DELAY_INIT);
    ftSpi_enableChipSelect();

    /* Wait at least 20ms */
    sleep_ms(DELAY_INIT);
    
    command[0] = getHighByte(PROG_EN);
    command[1] = getLowByte(PROG_EN);
    command[2] = 0;
    command[3] = 0;

    ftSpi_readWrite(readBytes, command, 4, false);

    if (readBytes[2] == getLowByte(PROG_EN)) {
        return true;
    }
    else {
        return false;
    }
}


/** Erases flash memory. **/
static void eraseFlash(void)
{
    uint8_t command[4];

    /* Erase current contents of flash */
    command[0] = getHighByte(CHIP_ERASE);
    command[1] = getLowByte(CHIP_ERASE);
    spiInstruction(command);

    sleep_ms(DELAY_ERASE);
}


/**
 * Reads and returns signature bytes of ATmega.
 *
 * @return signature bytes as 32bit integer.
 **/
static uint32_t readSigBytes(void)
{
    uint8_t command[4];
    uint32_t sigBytesOut = 0;

    command[0] = getHighByte(READ_SIG_BYTE);
    command[1] = getLowByte(READ_SIG_BYTE);
    command[3] = 0;

    command[2] = 0;
    spiInstruction(command);
    sigBytesOut |= ((uint32_t)command[3]) << 16;

    command[2] = 1;
    spiInstruction(command);
    sigBytesOut |= ((uint32_t)command[3]) << 8;

    command[2] = 2;
    spiInstruction(command);
    sigBytesOut |= (uint32_t)command[3];

    return sigBytesOut;
}


/**
 * Write to ATmega's flash memory.
 *
 * @param data what to write to flash.
 * @param address where to start writing
 * @param size how many bytes to write.
 **/
static void writeFlash(const uint8_t* data, uint32_t address, uint32_t size)
{
    uint32_t end = address + size - 1;
    uint32_t pageEnd = address | (pageSize - 1);

    /* Check parameters are within limits of EEPROM */
    if ((address + size) > info.memorySize) {
        fprintf(stderr, "ERROR: Cannot write to address %u of %u byte memory\n",
                end, info.memorySize);
        exit(EXIT_FAILURE);
    }

    /* Write to 1 page at a time */
    for (; address <= end; pageEnd += pageSize) {
        pageEnd = (pageEnd <= end) ? pageEnd : end;
        writeFlashPage(data, address, pageEnd);
        data += pageEnd - address + 1;
        address = pageEnd + 1;
    }
}


/**
 * Write to a single memory page in ATmega's memory.
 *
 * @param data the data to store in the page.
 * @param start where to start writing
 * @param end where to stop writing.
 **/
static void writeFlashPage(const uint8_t* data, uint32_t start, uint32_t end)
{
    uint8_t* loadPage;
    uint8_t* loadPagePtr;
    uint32_t i;

    if (NULL == (loadPage = (uint8_t*)malloc((pageSize * 4) + 4))) {
        fprintf(stderr, "ERROR: could not allocate %u bytes at %s.\n",
                (pageSize * 4) + 4, LOC);
        exit(EXIT_FAILURE);
    }
    memset(loadPage, 0xFF, (pageSize * 4) + 4);
    loadPagePtr = loadPage;

    /* Firstly, create all commands to fill page in local memory */
    for (i = start; i <= end; ++i) {

        /* Low byte must be loaded first */
        if ((i & 1) == 0) {
            *loadPagePtr++ = getHighByte(LOAD_PROG_MEM_PAGE_LOW);
            *loadPagePtr++ = getLowByte(LOAD_PROG_MEM_PAGE_LOW);

        }
        else {
            *loadPagePtr++ = getHighByte(LOAD_PROG_MEM_PAGE_HIGH);
            *loadPagePtr++ = getLowByte(LOAD_PROG_MEM_PAGE_HIGH);
        }

        /* Word address (increments every second loop) */
        *loadPagePtr++ = (i >> 1) & 0x007F;

        /* Data */
        *loadPagePtr++ = *data++;
    }

    /*
     * Bytes must be written in words. If the last byte is the low byte, then a
     * dummy byte must be added so that the last byte is written.
     */
    if ((i & 1) == 1) {
        *loadPagePtr++ = getHighByte(LOAD_PROG_MEM_PAGE_HIGH);
        *loadPagePtr++ = getLowByte(LOAD_PROG_MEM_PAGE_HIGH);
        *loadPagePtr++ = (i >> 1) & 0x007F;
        *loadPagePtr++ = 0xFF;
    }

    /* Add command to write the program page to the flash */
    *loadPagePtr++ = getHighByte(WRITE_PROG_MEM_PAGE);
    *loadPagePtr++ = getHighByte((start >> 1) & ~0x007F);
    *loadPagePtr++ = getLowByte((start >> 1) & ~0x007F);
    *loadPagePtr++ = 0x00;

    /* Send commands to ATmega */
    ftSpi_readWrite(NULL, loadPage, (pageSize * 4) + 4, false);

    /* Wait for write to complete */
    sleep_ms(DELAY_FLASH);

    free(loadPage);
}


/**
 * Reads from the flash (program memory).
 *
 * @param[out] data where to store read data.
 * @param address where to start reading.
 * @param size how many bytes to read.
 **/
static void readFlash(uint8_t* data, uint32_t address, uint32_t size)
{
    uint8_t* outBytes;
    uint8_t* outBytePtr;
    uint8_t* inBytes;
    uint32_t i;
    
    outBytes = (uint8_t*)malloc(size * 4);
    inBytes = (uint8_t*)malloc(size * 4);
    if (outBytes == NULL || inBytes == NULL) {
        fprintf(stderr, "ERROR: could not allocate %u bytes at %s.\n",
                size * 4, LOC);
        exit(EXIT_FAILURE);
    }
    
    /* Create commands to read chosen memory area */
    outBytePtr = outBytes;
    for (i = 0; i < size; ++i) {
        if (((address + i) & 1) == 0) {
            *outBytePtr++ = getHighByte(READ_PROG_MEM_LOW);
        }
        else {
            *outBytePtr++ = getHighByte(READ_PROG_MEM_HIGH);
        }
        *outBytePtr++ = getHighByte((address + i) / 2);
        *outBytePtr++ = getLowByte((address + i) / 2);
        *outBytePtr++ = 0;
    }

    /* Send commands and receive memory data */
    ftSpi_readWrite(inBytes, outBytes, size * 4, false);

    /* Copy memory data to "data" */
    for (i = 0; i < size; ++i) {
        /* (((i+1) * 4) - 1) = 3, 7, 11, ... */
        data[i] = inBytes[((i+1) * 4) - 1];
    }

    free(outBytes);
    free(inBytes);
}


/**
 * Writes to the EEPROM (data memory).
 *
 * @param data the data to be written.
 * @param address where to start writing.
 * @param size how many bytes to write.
 **/
static void writeEeprom(const uint8_t* data, uint32_t address, uint32_t size)
{
    uint8_t command[4];
    uint32_t i;

    command[0] = getHighByte(WRITE_EEPROM_MEM);

    for (i = address; i < size; ++i) {
        command[1] = getHighByte(i);
        command[2] = getLowByte(i);
        command[3] = data[i];
        spiInstruction(command);

        /* Wait for write to complete */
        sleep_ms(DELAY_EEPROM);
    }
}


/**
 * Writes 0xFF to every location in the EEPROM.
 **/
static void eraseEeprom(void)
{
    uint8_t data[info.memorySize];
	
	memset(data, 0xFF, info.memorySize);
	writeEeprom(data, 0, info.memorySize);
}


/**
 * Reads from the EEPROM (data memory).
 *
 * @param[out] data where to store read data.
 * @param address where to start reading.
 * @param size how many bytes to read.
 **/
static void readEeprom(uint8_t* data, uint32_t address, uint32_t size)
{
    uint8_t* outBytes;
    uint8_t* outBytePtr;
    uint8_t* inBytes;
    uint32_t i;

    outBytes = (uint8_t*)malloc(size * 4);
    inBytes = (uint8_t*)malloc(size * 4);
    if (outBytes == NULL || inBytes == NULL) {
        fprintf(stderr, "ERROR: could not allocate %u bytes at %s.\n",
                size * 4, LOC);
        exit(EXIT_FAILURE);
    }

    /* Create commands to read chosen memory area */
    outBytePtr = outBytes;
    for (i = 0; i < size; ++i) {
        *outBytePtr++ = getHighByte(READ_EEPROM_MEM);
        *outBytePtr++ = getHighByte((address + i));
        *outBytePtr++ = getLowByte((address + i));
        *outBytePtr++ = 0;
    }

    /* Send commands and receive memory data */
    ftSpi_readWrite(inBytes, outBytes, size * 4, false);

    /* Copy memory data to "data" */
    for (i = 0; i < size; ++i) {
        /* (((i+1) * 4) - 1) = 3, 7, 11, ... */
        data[i] = inBytes[((i+1) * 4) - 1];
    }
    
    free(outBytes);
    free(inBytes);
}


/**
 * Writes the three fuse bytes
 *
 * @param fuses the three fuse bytes: @c "<ext><high><low>".
 **/
static void writeFuses(const uint8_t fuses[3])
{
    uint8_t command[4];

    command[0] = getHighByte(WRITE_FUSE_EXT_BITS);
    command[1] = getLowByte(WRITE_FUSE_EXT_BITS);
    command[3] = fuses[FUSE_EXT];
    spiInstruction(command);
    /* Wait for write to complete */
    sleep_ms(5);

    command[0] = getHighByte(WRITE_FUSE_HIGH_BITS);
    command[1] = getLowByte(WRITE_FUSE_HIGH_BITS);
    command[3] = fuses[FUSE_HIGH];
    spiInstruction(command);
    /* Wait for write to complete */
    sleep_ms(5);

    command[0] = getHighByte(WRITE_FUSE_BITS);
    command[1] = getLowByte(WRITE_FUSE_BITS);
    command[3] = fuses[FUSE_LOW];
    spiInstruction(command);
    /* Wait for write to complete */
    sleep_ms(DELAY_FLASH);
}


/**
 * Reads the three fuse bytes
 *
 * @param[out] fuses the three fuse bytes: @c "<ext><high><low>".
 **/
static void readFuses(uint8_t fuses[3])
{
    uint8_t command[4];

    command[0] = getHighByte(READ_FUSE_EXT_BITS);
    command[1] = getLowByte(READ_FUSE_EXT_BITS);
    spiInstruction(command);
    fuses[FUSE_EXT] = command[3];

    command[0] = getHighByte(READ_FUSE_HIGH_BITS);
    command[1] = getLowByte(READ_FUSE_HIGH_BITS);
    spiInstruction(command);
    fuses[FUSE_HIGH] = command[3];

    command[0] = getHighByte(READ_FUSE_BITS);
    command[1] = getLowByte(READ_FUSE_BITS);
    spiInstruction(command);
    fuses[FUSE_LOW] = command[3];
}


/**
 * Does a single SPI instruction.
 *
 * @param[in,out] bytes the 4 bytes to send to the ATmega. Data returned from
 *     the ATmega will be put into the last byte.
 **/
static void spiInstruction(uint8_t bytes[4])
{
    uint8_t readBytes[4];

    ftSpi_readWrite(readBytes, bytes, 4, false);

    /* Check if atmega is echoing bytes correctly */
    if ((readBytes[1] != bytes[0]) || (readBytes[2] != bytes[1])) {
        fprintf(stderr,
                "ERROR: ATmega did not echo bytes correctly.\n");
        fprintf(stderr, " out:    %.2X %.2X %.2X %.2X\n",
               bytes[0], bytes[1], bytes[2], bytes[3]);
        fprintf(stderr, " in:  %.2X %.2X %.2X %.2X\n",
               readBytes[0], readBytes[1], readBytes[2], readBytes[3]);
        exit(EXIT_FAILURE);
    }

    /* Copy last byte which can be output from microcontroller */
    bytes[3] = readBytes[3];
}

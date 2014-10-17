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
 * Functions for writing and reading EEPROM on the 10mm nRF9E5 layer.
 * See nrf9e5.h for documentation on this library and functions.
 *
 * @file nrf9e5.c
 * @date 27-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "nrf9e5.h"
#include "ftSpi.h"
#include "crc16.h"


/* Instructions for 25320 EEPROM */
#define WREN_INSTR          0x06        /* 0000X110 */
#define WRDI_INSTR          0x04        /* 0000X100 */
#define RDSR_INSTR          0x05        /* 0000X101 */
#define WRSR_INSTR          0x01        /* 0000X001 */
#define READ_INSTR          0x03        /* 0000X011 */
#define WRITE_INSTR         0x02        /* 0000X010 */

/* Status reg bits in 25320 EEPROM */
#define nRDY                0x01        /* 00000001 */

/* Bitmasks for 1st byte the nRF9E5 expects in EEPROM */
#define NRF_SLOW_EEPROM     0x00        /* xxxx0xxx */
#define NRF_FAST_EEPROM     0x08        /* xxxx1xxx */
#define NRF_XO_FREQ_4MHZ    0x00        /* xxxxx000 */
#define NRF_XO_FREQ_8MHZ    0x01        /* xxxxx001 */
#define NRF_XO_FREQ_12MHZ   0x02        /* xxxxx010 */
#define NRF_XO_FREQ_16MHZ   0x03        /* xxxxx011 */
#define NRF_XO_FREQ_20MHZ   0x04        /* xxxxx100 */

#define MEMORY_HEADER_SIZE  7
#define MEMORY_SIZE         8192
#define MEMORY_PAGE_MASK    0x1F
#define MEMORY_PAGE_SIZE    0x20
#define MEMORY_CODE_SIZE    4096 - MEMORY_HEADER_SIZE

/** Delay after writing to memory **/
#define DELAY_WRITE         10

/** Bit mask for pin that is connected to the chip select. **/
#define PIN_NCS_10MM        0x40

/** Bit mask for pin that is connected to the SPI clock. **/
#define PIN_SCK_10MM        0x04

/** Bit mask for pin that is connected to MISO. **/
#define PIN_MISO_10MM       0x08

/** Bit mask for pin that is connected to MOSI. **/
#define PIN_MOSI_10MM       0x10


/** Frequency of crystal oscillator in Mhz. **/
static uint32_t crystalFreq;

/** Structure storing details of this EEPROM programmer. **/
static spiDeviceInfo_t info = {
    DESC_10MM_PROG,                     /* Description. */
    PIN_NCS_10MM,                       /* nCS pin. */
    PIN_SCK_10MM,                       /* SCK pin. */
    PIN_MISO_10MM,                      /* MISO pin. */
    PIN_MOSI_10MM,                      /* MOSI pin. */
    FTSPI_MAX_BAUDRATE,                 /* Baud rate.  */
    MEMORY_SIZE,                        /* Memory Size. */
    MEMORY_CODE_SIZE,                   /* Code Size. */
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}},   /* Write addresses (set later). */
    nrf_write,
    nrf_read,
    nrf_erase,
    nrf_addHeader,
};


/* Local function prototypes. */
static void writeToEepromPage(const uint8_t* data, uint32_t start,
                              uint32_t end);


/*******************************************************************************
 * Functions that can be used externally. Documentation in nrf9e5.h.
 */


spiDeviceInfo_t* nrf_init(uint32_t freq)
{
    crystalFreq = freq;
    ftSpi_init(&info);

    return &info;
}


void nrf_write(const uint8_t* data, uint32_t address, uint32_t size)
{
    static bool isCalled = false;
    uint32_t end = address + size - 1;
    uint32_t pageEnd = address | (MEMORY_PAGE_SIZE - 1);
    uint8_t lastPageData[MEMORY_PAGE_SIZE];

    /* Check parameters are within limits of EEPROM */
    if ((address + size) > MEMORY_SIZE) {
        fprintf(stderr, "ERROR: Cannot write to address %u of %u byte memory\n",
                end, MEMORY_SIZE);
        exit(EXIT_FAILURE);
    }

    memset(lastPageData, 0xFF, MEMORY_PAGE_SIZE);
    
    /*
     * Write to 1 page at a time. Sometimes there is a problem with writing to
     * the last page. So always a full page write is done, twice...
     */
    for (; address <= end; pageEnd += MEMORY_PAGE_SIZE) {
        if (pageEnd >= end) {
            memmove(lastPageData, data, (end & MEMORY_PAGE_MASK) + 1);
            writeToEepromPage(lastPageData, address, pageEnd);
            writeToEepromPage(lastPageData, address, pageEnd);
        }
        else {
            writeToEepromPage(data, address, pageEnd);
        }
        data += pageEnd - address + 1;
        address = pageEnd + 1;
    }
    if (!isCalled) {
        printf("\nWARNING: The board must be manually reset for the new program to run\n\n");
    }
    isCalled = true;
}


void nrf_erase(void)
{
    uint8_t buffer[MEMORY_PAGE_SIZE];
    uint32_t i;

    memset(buffer, 0xFF, MEMORY_PAGE_SIZE);

    for (i = 0; i < (MEMORY_SIZE / MEMORY_PAGE_SIZE); ++i) {
        writeToEepromPage(buffer,
                          i * MEMORY_PAGE_SIZE,
                          ((i + 1) * MEMORY_PAGE_SIZE) - 1);
    }
}


void nrf_read(uint8_t* data, uint32_t address, uint32_t size)
{
    uint8_t outData[size + 3];
    uint8_t inData[size + 3];

    /* Check for invalid parameters */
    if ((address + size) > MEMORY_SIZE) {
        fprintf(stderr, "ERROR: EEPROM size too big (%u) at %s.\n",
                size, stripPath(LOC));
        exit(EXIT_FAILURE);
    }

    memset(outData, 0xFF, size + 3);

    /* Setup read instruction and 2-byte address */
    outData[0] = READ_INSTR;
    outData[1] = getHighByte(address);
    outData[2] = getLowByte(address);

    ftSpi_readWrite(inData, outData, size + 3, true);
    memmove(data, inData + 3, size);
}


void nrf_addHeader(uint8_t* data, uint32_t rawCodeSize, bool isSupportReprog)
{
    uint32_t numBlocks;
    uint32_t freq;
    uint32_t size = rawCodeSize + MEMORY_HEADER_SIZE;
    uint32_t i;
    FILE* file;

    /* Check there's enough room to add the header */
    if (rawCodeSize > MEMORY_CODE_SIZE) {
        fprintf(stderr,
                "ERROR: EEPROM size too big (%u bytes). Max is %u bytes.\n",
                rawCodeSize, MEMORY_CODE_SIZE);
        exit(EXIT_FAILURE);
    }

    freq = (crystalFreq / 4);
    if (freq == 0 || freq > 5) {
        fprintf(stderr, "ERROR: Invalid crystal frequency(%u)\n", crystalFreq);
        exit(EXIT_FAILURE);
    }
    freq -= 1;

    /* How many 256 byte blocks in program */
    numBlocks = size / 256;
    if (size % 256 != 0) {
        numBlocks++;
    }

    /* Calculate CRC-16 value for program code */
    crc16_init();
    for (i = 0; i < rawCodeSize; i++) {
        crc16_update(data[i]);
    }

    /* Shift bytes */
    memmove(data + MEMORY_HEADER_SIZE, data, rawCodeSize);

    /* Add configuration bytes */
    data[0] = NRF_FAST_EEPROM | freq;
    data[1] = MEMORY_HEADER_SIZE;
    data[2] = numBlocks - 1;        /* Don't include 1st block */
    data[3] = getLowByte(rawCodeSize);
    data[4] = getHighByte(rawCodeSize);
    data[5] = getLowByte(crc16_read());
    data[6] = getHighByte(crc16_read());

    info.writeAdds[0][0] = 0;
    info.writeAdds[0][1] = size;

    /* Create a file and write the data with header to it. */
    if ((file = fopen("nrf9e5_out.bin", "wb")) == NULL) {
        fprintf(stderr, "ERROR: could not open file nrf9e5_out.bin.\n");
        exit(EXIT_FAILURE);
    }
    if (size != fwrite(data, 1, size, file)) {
        fprintf(stderr,
                "ERROR: could not write %i bytes to nrf9e5_out.bin at %s.\n",
                size, stripPath(LOC));
        exit(EXIT_FAILURE);
    }
    fclose(file);

    if (isSupportReprog) {
        /* Copy to upper half of memory */
        memmove(data + (MEMORY_SIZE / 2), data,
                                              rawCodeSize + MEMORY_HEADER_SIZE);
        info.writeAdds[1][0] = (MEMORY_SIZE / 2);
        info.writeAdds[1][1] = info.writeAdds[0][1];
    }
}


/*******************************************************************************
 * Functions that be only used within file.
 */


/**
 * Writes a number of bytes to the EEPROM. Can only write within a single page.
 * There is a pause after writing to make sure the data is written before
 * sending the next command.
 *
 * @param data pointer to bytes to write.
 * @param start where to start writing to.
 * @param end where to finish writing to.
 **/
static void writeToEepromPage(const uint8_t* data, uint32_t start, uint32_t end)
{
    uint32_t size = end - start + 1;
    uint8_t* outData;

    if (NULL == (outData = (uint8_t*)malloc(end - start + 1 + 3))) {
        fprintf(stderr, "ERROR: could not allocate %u bytes at %s.\n",
                end - start + 1 + 3, LOC);
        exit(EXIT_FAILURE);
    }

    /* Send WREN instruction */
    outData[0] = WREN_INSTR;
    ftSpi_readWrite(NULL, outData, 1, true);

    /* Setup data to send: instruction + address + data */
    outData[0] = WRITE_INSTR;
    outData[1] = getHighByte(start);
    outData[2] = getLowByte(start);
    memmove(outData + 3, data, size);

    /* Send data plus 3 bytes for intruction and address */
    ftSpi_readWrite(NULL, outData, size + 3, true);

    /* Faster to wait than to check the status bit. */
    sleep_ms(DELAY_WRITE);
    
    free(outData);
}

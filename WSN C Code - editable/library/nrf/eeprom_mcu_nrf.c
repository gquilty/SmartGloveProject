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
 * Functions for accessing the EEPROM on the 10mm transceiver board. This EEPROM
 * has 8kbytes. The lower 4 kbytes are for the program code. The upper 4 kbytes
 * can be used for data storage. If the application support reprogramming
 * ("reprog.h" is included), do not use these functions, as upper 4kBytes are
 * used for code storage.
 *
 * @file eeprom_mcu_nrf.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "spi.h"
#include "eeprom_mcu.h"
#include "reprog_nrf.h"
#include "delay.h"
#include "simpleIo.h"


/* Instructions for EEPROM. */
#define WRSR_INSTR          0x01        /* 0000X001 */
#define WRITE_INSTR         0x02        /* 0000X010 */
#define READ_INSTR          0x03        /* 0000X011 */
#define WRDI_INSTR          0x04        /* 0000X100 */
#define RDSR_INSTR          0x05        /* 0000X101 */
#define WREN_INSTR          0x06        /* 0000X110 */

/* Status reg bit in EEPROM. */
#define nRDY                0x01        /* 00000001 */

/** Page Size. **/
#define PAGE_SIZE           32

//#define PAGE_SIZE           64
/** Change this to 0 to write to lower 4 bytes, (for reprogramming). **/
static uint16_t addressOffset = 4096;
//static uint16_t addressOffset = 0;
//static uint16_t addressOffset =8192;

static void writePage(const uint8_t* data, uint16_t start, uint16_t end);


/******************************************************************************\
 * See eeprom_mcu.h for documentation of these functions.
\******************************************************************************/


void eeprom_mcu_write(const uint8_t* data, uint16_t start, uint16_t end)
{
    start += addressOffset;
    end += addressOffset;

    for (uint16_t pageEnd = start | (PAGE_SIZE - 1); start <= end; pageEnd += PAGE_SIZE) {
        pageEnd = (pageEnd <= end) ? pageEnd : end;
        writePage(data, start, pageEnd);
        data += pageEnd - start + 1;
        start = pageEnd + 1;
    }
}


void eeprom_mcu_read(uint8_t* data, uint16_t start, uint16_t end)
{
    start += addressOffset;
    end += addressOffset;

    disableInterrupts();

    /* Slow down SPI clock, and connect SPI to pins */
    SPICLK = 2;
    spi_selectExternal();

    P1_EECSN = 0;

    /* Send read instruction and 2-byte address */
    spi_enableCsn();
    spi_readWriteByte(READ_INSTR);
    spi_readWriteByte(HIGH_BYTE(start));
    spi_readWriteByte(LOW_BYTE(start));

    /* Read in data */
    spi_readArray(data, end - start + 1);
    P1_EECSN = 1;

    spi_selectInternal();
    SPICLK = 0;

    enableInterrupts();
}


/******************************************************************************\
 * See reprog_nrf.h for documentation of this functions.
\******************************************************************************/

void mem_eeprom_CodeSection(bool mode)
{
    if (mode == true) {
        addressOffset = 0;
    }
    else {
      addressOffset = 4096;
 //addressOffset = 8192;
    }
}


/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/

/**
 * Writes to the EEPROM. Can only write within a single page.
 *
 * @param data pointer to bytes to write.
 * @param start where to start writing from.
 * @param end where to stop writing.
 **/
static void writePage(const uint8_t* data, uint16_t start, uint16_t end)
{
    uint8_t status;

    /* Connect CSN to external pin */
    disableInterrupts();

    spi_selectExternal();
    SPICLK = 2;

    /* Send WREN instruction */
    P1_EECSN = 0;
    spi_readWriteByte(WREN_INSTR);
    P1_EECSN = 1;

    /* Send address */
    P1_EECSN = 0;
    spi_readWriteByte(WRITE_INSTR);
    spi_readWriteByte(HIGH_BYTE(start));
    spi_readWriteByte(LOW_BYTE(start));
    spi_writeArray(data, end - start + 1);
    P1_EECSN = 1;

    /* Wait for EEPROM to finish writing */
    do {
        P1_EECSN = 0;
        spi_readWriteByte(RDSR_INSTR);
        status = spi_readWriteByte(0);
        P1_EECSN = 1;
    } while (status & BIT(nRDY));

    spi_selectInternal();
    SPICLK = 0;

    enableInterrupts();
}

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
 * Implementation of functions for in-system re-programming of nRF905.
 *
 * @file reprog_nrf.c
 * @date 16-Apr-2010
 * @author Seán Harte
 ******************************************************************************/

#include "global.h"
#include "eeprom_mcu.h"
#include "reprog.h"
#include "reprog_nrf.h"
#include "nrf_regx.h"
#include "crc16.h"


#define BUFFER_SIZE         32   /**< This is the page size of the memory! **/
//#define BUFFER_SIZE         64
#define MEM_SIZE            4096 /**< Code memory size. **/
//#define MEM_SIZE            8192 /**< Code memory size. **/
#define CODE_HEADER_SIZE    7    /**< How many meta-data bytes. **/


/** Buffer to temporatily store bytes when copying. **/
static uint8_t buffer[BUFFER_SIZE];


/******************************************************************************\
 * See reprog.h for documentation of these functions.
\******************************************************************************/


void reprog_move(uint16_t oldAddress, uint16_t newAddress, uint16_t count)
{
    uint16_t tempCount;

    if (oldAddress > newAddress) {
        /* Copy BUFFER_SIZE bytes at a time. Start with lower bytes */
        while (count > 0) {
            tempCount = (count <= BUFFER_SIZE) ? count : BUFFER_SIZE;
            eeprom_mcu_read(buffer, oldAddress, oldAddress + tempCount - 1);
            eeprom_mcu_write(buffer, newAddress, newAddress + tempCount - 1);
            newAddress += tempCount;
            oldAddress += tempCount;
            count -= tempCount;
        }
    }
    else {
        oldAddress += count;
        newAddress += count;
        /* Copy BUFFER_SIZE bytes at a time. Start with higher bytes */
        while (count > 0) {
            tempCount = (count <= BUFFER_SIZE) ? count : BUFFER_SIZE;
            eeprom_mcu_read(buffer, oldAddress - tempCount, oldAddress - 1);
            eeprom_mcu_write(buffer, newAddress - tempCount, newAddress - 1);
            newAddress -= tempCount;
            oldAddress -= tempCount;
            count -= tempCount;
        }
    }
}


void reprog_read(uint16_t address, uint8_t* data, uint8_t count)
{
    eeprom_mcu_read(data, address, address + count - 1);
}


void reprog_write(uint16_t address, const uint8_t* data, uint8_t count)
{
    eeprom_mcu_write(data, address, address + count - 1);
}


uint8_t reprog_restart(void)
{
    uint16_t address, length, crc;

    /* Read code length and crc */
    eeprom_mcu_read((uint8_t*)&length, 3, 4);
    eeprom_mcu_read((uint8_t*)&crc, 5, 6);

    /* Calculate CRC and compare to stored value */
    crc16_init();
    for (uint16_t i = CODE_HEADER_SIZE; i < length + CODE_HEADER_SIZE; i++) {
        eeprom_mcu_read(buffer, address, address);
        crc16_update(buffer[0]);
    }

    /* Only restart if CRC matches */
    if (crc != crc16_read()) {
        return false;
    }

    /* Copy from upper half of memory to lower half */
    for (uint8_t j = 0; j < (MEM_SIZE / BUFFER_SIZE); j++) {
        address = j * BUFFER_SIZE;
        eeprom_mcu_read(buffer, address, address + (BUFFER_SIZE - 1));
        mem_eeprom_CodeSection(true);
        eeprom_mcu_write(buffer, address, address + (BUFFER_SIZE - 1));
        mem_eeprom_CodeSection(false);
    }

    /* Reset address = 0x8000 (bootloader) */
    RSTREAS |= BIT(RFLR1);
    
    /* Start watchdog timer */
    regx_write(WD, 1);
    
    /* Wait for WDT to timeout */
    for(;;) {
        ;
    }
}

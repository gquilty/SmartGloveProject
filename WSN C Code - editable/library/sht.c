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
 * Functions for communicating with SHT71 sensor.
 *
 * @file sht.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "sht.h"
#include "delay.h"


/* Length of time to wait for SHT to respond (ms) */
#if SHT_LOW_RES_ADC == 1
    #define TEMPHUM_TIMEOUT     75
#else
    #define TEMPHUM_TIMEOUT     255
#endif


/* Set up initial values to be put into status register on SHT */
#if SHT_NO_RELOAD_FROM_OTP == 0 && SHT_LOW_RES_ADC == 0
    #define STATUS_REG      0x0
#elif SHT_NO_RELOAD_FROM_OTP == 0 && SHT_LOW_RES_ADC == 1
    #define STATUS_REG      0x1
#elif SHT_NO_RELOAD_FROM_OTP == 1 && SHT_LOW_RES_ADC == 0
    #define STATUS_REG      0x2
#elif SHT_NO_RELOAD_FROM_OTP == 1 && SHT_LOW_RES_ADC == 1
    #define STATUS_REG      0x3
#endif

/* Commands that are sent to SHT */
#define WRITE_STATUS_CMD    0x06
#define SOFT_RESET_CMD      0x1E
#define READ_STATUS_CMD     0x07


#if SHT_CHECK_CRC == 1
/* Lookup table used when calculating CRC */
const uint8_t CRC_TABLE[256] = {
    0, 49, 98, 83, 196, 245, 166, 151, 185, 136, 219, 234, 125, 76, 31, 46,
    67, 114, 33, 16, 135, 182, 229, 212, 250, 203, 152, 169, 62, 15, 92, 109,
    134, 183, 228, 213, 66, 115, 32, 17, 63, 14, 93, 108, 251, 202, 153, 168,
    197, 244, 167, 150, 1, 48, 99, 82, 124, 77, 30, 47, 184, 137, 218, 235,
    61, 12, 95, 110, 249, 200, 155, 170, 132, 181, 230, 215, 64, 113, 34, 19,
    126, 79, 28, 45, 186, 139, 216, 233, 199, 246, 165, 148, 3, 50, 97, 80,
    187, 138, 217, 232, 127, 78, 29, 44, 2, 51, 96, 81, 198, 247, 164, 149,
    248, 201, 154, 171, 60, 13, 94, 111, 65, 112, 35, 18, 133, 180, 231, 214,
    122, 75, 24, 41, 190, 143, 220, 237, 195, 242, 161, 144, 7, 54, 101, 84,
    57, 8, 91, 106, 253, 204, 159, 174, 128, 177, 226, 211, 68, 117, 38, 23,
    252, 205, 158, 175, 56, 9, 90, 107, 69, 116, 39, 22, 129, 176, 227, 210,
    191, 142, 221, 236, 123, 74, 25, 40, 6, 55, 100, 85, 194, 243, 160, 145,
    71, 118, 37, 20, 131, 178, 225, 208, 254, 207, 156, 173, 58, 11, 88, 105,
    4, 53, 102, 87, 192, 241, 162, 147, 189, 140, 223, 238, 121, 72, 27, 42,
    193, 240, 163, 146, 5, 52, 103, 86, 120, 73, 26, 43, 188, 141, 222, 239,
    130, 179, 224, 209, 70, 119, 36, 21, 59, 10, 89, 104, 255, 206, 157, 172
};
#endif  /* CHECK_CRC */


static void start(void);
static void reset(void);
static status_t writeByte(unsigned char value);
static uint8_t readByte(bool ack);
static status_t startSensor(void);


/** Clock pin is driven high. **/
FORCE_INLINE static inline void clockHigh(void)
{
    SHT_CLOCK_PORT |= BIT(SHT_CLOCK);
}

/** Clock pin is driven low. **/
FORCE_INLINE static inline void clockLow(void)
{
    SHT_CLOCK_PORT &= ~BIT(SHT_CLOCK);
}


/**
 * Data pin has a pullup, so the pin is set to high-impedance state to go high.
 **/
FORCE_INLINE static inline void dataRelease(void)
{
#if defined (UC_AVR)
    SHT_DATA_DDR &= ~BIT(SHT_DATA);
    SHT_DATA_PORT |= BIT(SHT_DATA);
#else
    SHT_DATA_DDR |= BIT(SHT_DATA);
#endif
}

/**
 * Data pin has a pullup, is driven low.
 **/
FORCE_INLINE static inline void dataLow(void)
{
#if defined (UC_AVR)
    SHT_DATA_PORT &= ~BIT(SHT_DATA);
    SHT_DATA_DDR |= BIT(SHT_DATA);
#else
    SHT_DATA_DDR &= ~BIT(SHT_DATA);
#endif
}

/**
 * Reat state of data pin (1 or 0).
 **/
FORCE_INLINE static inline uint8_t readData(void)
{
    return SHT_DATA_PIN & BIT(SHT_DATA);
}


/**
 * Delays for 100ns. (just do nothing).
 **/
FORCE_INLINE static inline void delay(void)
{
    ;
}


/**
 * Setup pins.
 **/
FORCE_INLINE static inline void initPins(void)
{
#if defined (UC_AVR)
    SHT_CLOCK_DDR |= BIT(SHT_CLOCK);
#else
    P0_DRV |= BIT(SHT_DATA) | BIT(SHT_CLOCK);
    P0_DIR &= ~BIT(SHT_CLOCK);
    P0 &= ~BIT(SHT_DATA);
#endif
    clockLow();
    dataRelease();
}


/******************************************************************************\
 * See sht.h for documentation of this function.
\******************************************************************************/

status_t sht_read(uint8_t cmd, uint16_t* value)
{
    static bool isInit = 0;
    uint8_t i;
    uint8_t data[3];
    status_t status = STATUS_OK;

    /* Set up config register if necessary */
    if (!isInit) {
        status = startSensor();
        if (status == STATUS_OK) isInit = 1;
        else return status;
    }

    start();
    if ((status = writeByte(cmd)) != STATUS_OK) {
        isInit = false;
        return status;
    }

    /* Wait for conversion to finish */
    for (i = 0; i < TEMPHUM_TIMEOUT; ++i) {
        if (!readData()) break;
        delay_us(1000);
    }

    /* If sensor didn't complete conversion in allowed time */
    if (readData()) {
        isInit = false;
        return STATUS_NO_REPSONSE;
    }

    data[0] = readByte(true);        /* Read most significant byte */
    data[1] = readByte(true);        /* Read least significant byte */
    data[2] = readByte(false);       /* Read reversed CRC byte */

#if SHT_CHECK_CRC == 1
    uint8_t receivedCRC = 0, CRC = 0;

    /* Reverse byte to get CRC */
    for (i = 0; i < 8; ++i) {
        receivedCRC |= ((data[2] >> (i)) & 1) << (7 - i);
    }
    for (i = 0; i < 8; ++i) {
        CRC |= (((STATUS_REG & 0xF) >> (i)) & 1) << (7 - i);
    }
    CRC = CRC_TABLE[cmd ^ CRC];
    CRC = CRC_TABLE[data[0] ^ CRC];
    CRC = CRC_TABLE[data[1] ^ CRC];
    if (CRC != receivedCRC) {
        isInit = false;
        return STATUS_COMM_ERROR;
    }
#endif   // End CHECK_CRC

    *value = TO_UINT16(data[0], data[1]);

    if (SHT_LOW_RES_ADC) {
        if (cmd == SHT_TEMPERATURE) *value <<= 2;      /* 12bit to 14bit */
        else if (cmd == SHT_HUMIDITY) *value <<= 4;  /* 8bit to 12bit */
    }
    return STATUS_OK;
}


#if SHT_CONV_ACCURACY_HIGH == 1
float sht_convert(uint8_t cmd, uint16_t value)
{
    if (cmd == SHT_TEMPERATURE) {
        /* From datasheet */
        return (value * 0.01) - 39.6;
    }
    else {
        /* From application note "Non-linearity compensation" */
        if (value <= 1712) return (138.0 * value - 3680) / 4096;
        else return (122.0 * value - 20896) / 4096;
    }
}
#else
int8_t sht_convert(uint8_t cmd, uint16_t value)
{
    if (cmd == SHT_TEMPERATURE) {
        /*
         * value/100 would be rounded down, when doing integer division.
         * Therefore 0.5 must be added, which gives
         *     value/100 - 39.1
         *     value/100 - 10/100 - 39
         */
        return ((value - 10) / 100) - 39;
    }
    else {
        /* From application note "Non-linearity compensation" */
        return 1 + ((value+16) >> 5);
    }
}
#endif

/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/


/**
 * Generates transmission start sequence.
 * @code
 *        _____         _____
 * DATA:       \_______/
 *            ___     ___
 * CLOCK: ___/   \___/   \___
 * @endcode
 **/
static void start(void)
{
    initPins();
    delay();
    clockHigh();
    delay();
    dataLow();
    delay();
    clockLow();
    delay();
    clockHigh();
    delay();
    dataRelease();
    delay();
    clockLow();
    delay();
}


/**
 * Resets connection, by toggling CLOCK 9 times while DATA is high, and
 * sending transmission start sequence.
 * @code
 *        __________________________________________         _____
 * DATA:                                            \_______/
 *           _   _   _   _   _   _   _   _   _     ___     ___
 * CLOCK: __/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \___/   \___/   \___
 * @endcode
 **/
static void reset(void)
{
    uint8_t i;

    initPins();
    for (i = 0; i < 9; ++i) {
        delay();
        clockHigh();
        delay();
        clockLow();
    }
    start();
}


/**
 * Writes a byte on the bus and checks the acknowledge.
 *
 * @param value byte to write.
 * @return @c STATUS_NO_ACK if no ack from sensor, otherwise @c STATUS_OK.
 **/
static status_t writeByte(unsigned char value)
{ 
    status_t status = STATUS_OK;
    uint8_t i;

    /* Send byte, starting with MSB */
    for (i = 0x80; i > 0 ; i >>= 1) {
        if (i & value) dataRelease();
        else dataLow();
        clockHigh();
        delay();
        clockLow();
        delay();
    }
    dataRelease();
    clockHigh();                    /* Clock 9 for ack */
    delay();

    if (readData()) status = STATUS_NO_ACK;

    clockLow();
    return status;
}


/**
 * Reads a byte from the bus.
 *
 * @param ack if 1, the byte is acknowledged.
 * @return byte read from bus.
 **/
static uint8_t readByte(bool ack)
{
    uint8_t val = 0;
    uint8_t i;

    dataRelease();
    for (i = 0x80; i > 0; i >>= 1) {
        clockHigh();
        if (readData()) val |= i;              /* Set bit */
        clockLow();
        delay();
    }

    if (ack == true) dataLow();
    clockHigh();
    delay();;
    clockLow();
    dataRelease();

    return val;
}


/**
 * Configure the sensor.
 *
 * @return @c STATUS_OK or @c STATUS_NO_ACK.
 **/
static status_t startSensor(void)
{
    uint8_t errors = 0;

    reset();

    errors += writeByte(WRITE_STATUS_CMD);
    errors += writeByte(STATUS_REG);

    if (errors == 0) return STATUS_OK;
    else return STATUS_NO_ACK;
}

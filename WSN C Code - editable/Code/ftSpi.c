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
 * Functions for bit-banged SPI protocol.
 *
 * This code must be linked with either the FTDI D2XX library (@e FTD2XX.lib)
 * from http://www.ftdichip.com or the libusb library (@e libusb.a) from
 * http://libusb-win32.sourceforge.net/
 *
 * To use D2XX, define @c FTDI_USE_D2XX.
 *
 * To use libusb, define @c FTDI_USE_LIBUSB. The libftdi API is used to access
 * libusb, and the file @e ftdi.c should be compiled and linked also.
 *
 * References:
 *  - FTDI, FT232R Datasheet.
 *  - FTDI, D2XX Programmer's Guide.
 *  - FTDI, AN232R-01 Bit Bang Modes for the FT232R and FT245R.
 *  - libftdi API docs:
 *    http://www.intra2net.com/en/developer/libftdi/documentation/
 *
 * @file ftSpi.c
 * @date 27-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>


#if defined (FTDI_USE_D2XX)
#   include <windows.h>
#   include "ftd2xx.h"
#elif defined (FTDI_USE_LIBUSB)
#   include "ftdi.h"
#endif
#include "common.h"
#include "ftSpi.h"
#include "usb.h"


/** Active low pin to enable analogue switch for SPI. **/
#define PROG_EN             0x80

/** Bit bang mode to use on FT232R (4 = synchronous, 1 = asynchronous). **/
#define BIT_BANG_MODE       0x04

/** Vendor ID for Tyndall programing boards (FTDI default) */
#define VENDOR_ID           0x0403

/** Product ID for Tyndall programing boards (FTDI default) */
#define PRODUCT_ID          0x6001

#if defined (FTDI_USE_D2XX)
    /**
     * Largest value that can be used in FT_Write() and FT_Read() functions.
     * Writes and reads were tested and found to fail at around 38000 bytes. Not
     * sure why this is...
     **/
#   define MAX_FT_COMM_SIZE    15000
#elif defined (FTDI_USE_LIBUSB)
    /**
     * Largest value that can be used in the ftdi_write_data() function. Larger
     * values fail with error: "LIBUSB_DLL: error: usb_reap: timeout error"
     * Not sure why this is...
     **/
#   define MAX_FT_COMM_SIZE    384
#endif

/** How long to wait if something is not responding (in seconds). **/
#define TIMEOUT             1

#if defined (FTDI_USE_D2XX)
/** Used to convert error codes to strings. **/
static const char* ftErrorStrings[] = {"FT_OK", "FT_INVALID_HANDLE",
        "FT_DEVICE_NOT_FOUND", "FT_DEVICE_NOT_OPENED", "FT_IO_ERROR",
        "FT_INSUFFICIENT_RESOURCES", "FT_INVALID_PARAMETER",
        "FT_INVALID_BAUD_RATE", "FT_DEVICE_NOT_OPENED_FOR_ERASE",
        "FT_DEVICE_NOT_OPENED_FOR_WRITE", "FT_FAILED_TO_WRITE_DEVICE",
        "FT_EEPROM_READ_FAILED", "FT_EEPROM_WRITE_FAILED",
        "FT_EEPROM_ERASE_FAILED", "FT_EEPROM_NOT_PRESENT",
        "FT_EEPROM_NOT_PROGRAMMED", "FT_INVALID_ARGS", "FT_NOT_SUPPORTED",
        "FT_OTHER_ERROR"};
#endif

/** Handle used to access FT device by all functions in this file. **/
static void* ftHandle = NULL;

/** Only close connection if it has been opened */
static bool isFtInit = false;


/* Pins used for SPI communications. **/
static uint8_t ncs;
static uint8_t sck;
static uint8_t miso;
static uint8_t mosi;

/* Local function prototypes. **/
static void serialiseMosi(const uint8_t* bytes, uint8_t* serialData,
                          uint32_t size);
static bool deserialiseMiso(const uint8_t* serialData, uint8_t* bytes,
                            uint32_t size);
static void bitBang(const uint8_t* writeData, uint8_t* readData, uint32_t size);
static void bitBangBlock(const uint8_t* writeData, uint8_t* readData,
                         uint32_t size);
static void checkFtError(int32_t code, const char* location);


/*******************************************************************************
 * Functions that be used externally. Documentation in ft_spi.h.
 */


void ftSpi_init(spiDeviceInfo_t* device)
{
    int32_t status;
    uint8_t direction;

    /* Store pins to use for SPI communication */
    ncs = device->ncs;      /* active low chip select */
    sck = device->sck;      /* clock */
    miso = device->miso;    /* Master In, Slave Out */
    mosi = device->mosi;    /* Master Out, Slave In */

    /* These pins will be outputs */
    direction = PROG_EN | ncs | sck | mosi;

#if defined (FTDI_USE_D2XX)
    /* Compares descriptions and opens device if description matches */
    if (FT_OK != FT_OpenEx((PVOID)device->description,
                           FT_OPEN_BY_DESCRIPTION,
                           &ftHandle)) {
        fprintf(stderr, "ERROR: Could not connect to %s Board.\n"
                "       Check USB connection, power switch, and if another program is \n"
                "       communicating with the board through the virtual COM Port.\n",
                device->description);
        exit(EXIT_FAILURE);
    }
    printf(" Board Found (%s).\n", device->description);

    isFtInit = true;

    /* Set speed that data changes at */
    status = FT_SetBaudRate(ftHandle, device->baudRate);
    checkFtError(status, LOC);

    /* Set synchronous bit-bang mode */
    status = FT_SetBitMode(ftHandle, direction, BIT_BANG_MODE);
    checkFtError(status, LOC);

    /* Purge RX and TX buffers, just in case there's some data there */
    status = FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);
    checkFtError(status, LOC);

    /* Set timeouts */
    status = FT_SetTimeouts(ftHandle, TIMEOUT * 1000, TIMEOUT * 1000);
    checkFtError(status, LOC);

    /* Set lower latency for faster operation */
//    status = FT_SetLatencyTimer(ftHandle, 2);
//    checkFtError(status, LOC);

#elif defined (FTDI_USE_LIBUSB)
    if (NULL == (ftHandle = ftdi_new())) {
        fprintf(stderr, "ERROR: Could not connect to %s Board.\n"
                "       Check USB connection, power switch, and if another program is \n"
                "       communicating with the board through the virtual COM Port.\n",
                device->description);
        exit(EXIT_FAILURE);
    }

    if (0 > ftdi_usb_open_desc(ftHandle, VENDOR_ID, PRODUCT_ID,
                                                   device->description, NULL)) {
        fprintf(stderr, "ERROR: Could not connect to %s Board.\n"
                "       Check USB connection, power switch, and if another program is \n"
                "       communicating with the board through the virtual COM Port.\n",
                device->description);
        exit(EXIT_FAILURE);
    }

    printf(" Board Found (%s).\n", device->description);

    /* Set speed that data changes at */
    status = ftdi_set_baudrate(ftHandle, device->baudRate);
    checkFtError(status, LOC);

    /* Set synchronous bit-bang mode TODO */
    status = ftdi_set_bitmode(ftHandle, direction, BITMODE_SYNCBB);
    checkFtError(status, LOC);

    /* Purge RX and TX buffers, just in case there's some data there */
    status = ftdi_usb_purge_buffers(ftHandle);
    checkFtError(status, LOC);

    /* Set lower latency for faster operation */
    status = ftdi_set_latency_timer(ftHandle, 2);
    checkFtError(status, LOC);

    /* Enable debugging messages for libusb */
    usb_set_debug(255);
#endif
}


void ftSpi_readWrite(uint8_t* misoData, const uint8_t* mosiData,
                     uint32_t count, bool chipSelect)
{
    uint8_t* outSpi;
    uint8_t* inSpi;
    uint32_t bufferSize = count * 16 + 4;

    if (count == 0) {
        return;                  /* Nothing to do */
    }

    outSpi = (uint8_t*)malloc(bufferSize);
    inSpi = (uint8_t*)malloc(bufferSize);
    if (outSpi == NULL || inSpi == NULL) {
        fprintf(stderr, "ERROR: Could not allocate %u bytes at %s.\n",
                bufferSize, LOC);
        exit(EXIT_FAILURE);
    }

    if (chipSelect) {
        outSpi[0] = ncs;
        outSpi[bufferSize - 1] = ncs;
    }
    else {
        outSpi[0] = 0;
        outSpi[bufferSize - 1] = 0;
    }
    outSpi[1] = 0;
    outSpi[bufferSize - 2] = 0;

    /* Generate SPI stream to send out */
    serialiseMosi(mosiData, outSpi + 2, count);

    /* Send and receive the data */
    bitBang(outSpi, inSpi, bufferSize);

    if (misoData != NULL) {
        /* Decode data that was read back from EEPROM */
        if (!deserialiseMiso(inSpi + 2, misoData, bufferSize - 4)) {
            fprintf(stderr,
                    "ERROR: SPI communications failed when sending %u bytes\n"
                    "       Check USB connection and power switch.\n", count);
            free(inSpi);
            free(outSpi);
            exit(EXIT_FAILURE);
        }
        free(inSpi);
    }
    free(outSpi);
}


void ftSpi_enableChipSelect(void)
{
    uint8_t outSpi[2];
    uint8_t inSpi[2];

    /* Bring nCS low at start of SPI data */
    outSpi[0] = ncs;
    outSpi[1] = 0;

    bitBang(outSpi, inSpi, 2);
}


void ftSpi_disableChipSelect(void)
{
    uint8_t outSpi[2];
    uint8_t inSpi[2];

    /* Bring nCS high at end of SPI data */
    outSpi[0] = 0;
    outSpi[1] = ncs;

    bitBang(outSpi, inSpi, 2);
}


void ftSpi_close(void)
{
    if (isFtInit) {
#if defined (FTDI_USE_D2XX)
        FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);
        FT_SetBitMode(ftHandle, 0, 0);
        FT_Close(ftHandle);
#elif defined (FTDI_USE_LIBUSB)
        ftdi_usb_purge_buffers(ftHandle);
        ftdi_disable_bitbang(ftHandle);
        ftdi_free(ftHandle);
#endif
    }
}


/*******************************************************************************
 * Functions that can only be used within this file.
 */


/**
 * Converts bytes of data to serial form to send over SPI. Each bit is converted
 * to two bytes, 1 to set the data, and the second to toggle SCK. The bit is set
 * on the rising edge of the clock.
 *
 * @param bytes bytes to convert to serial. If this is NULL, then MOSI is set to
 *     0 for all bits.
 * @param[out] serialData where to store serial SPI data. Must be at least
 *     size * 16 bytes in size.
 * @param size how many bytes to convert to serial form.
 **/
void serialiseMosi(const uint8_t* bytes, uint8_t* serialData, uint32_t size)
{
    uint8_t mosiBit;
    uint8_t singleByte;
    uint32_t i;
    uint8_t j;

    /* Loop through all bytes */
    for (i = 0; i < size; i++) {
        /* Use 0 for the value if bytes is not present */
        singleByte = (bytes != NULL) ? *bytes++ : 0;

        /* Loop through each bit in the byte */
        for (j = 0x80; j > 0; j >>= 1) {
            /* Set MOSI high, if the bit is set in the byte, and clear SCK */
            mosiBit = (singleByte & j) ? mosi : 0;

            *serialData++ = mosiBit;

            /* Set SCK high */
            *serialData++ = mosiBit | sck;

        }
    }
}


/**
 * Converts data read from SPI into bytes of data. Two serial bytes are
 * converted to a single MISO bit. The bit is read on the rising edge of SCK.
 *
 * @param serialData serial SPI data to convert to bytes.
 * @param[out] bytes where to store bytes. Must be at least count / 16 bytes in
 *     size.
 * @param size size of serialData, multiple of 16.
 * @return @c true if no errors, @c false if SCK is not toggling
 *     correctly in serialData.
 **/
bool deserialiseMiso(const uint8_t* serialData, uint8_t* bytes, uint32_t size)
{
    uint8_t* endPtr = bytes + (size / 16);
    uint8_t j;
    bool isError = false;

    /* Loop through all bytes */
    for ( ; bytes < endPtr; bytes++) {
        /* Initialise byte to 0 so binary OR can be used to set bits */
        *bytes = 0;

        /* Loop through each bit in the byte */
        for (j = 0x80; j > 0; j >>= 1) {
            /* SCK should be 0 here */
            if (*serialData++ & sck) {
                isError = true;
            }

            /* Set bit in byte */
            *bytes |= (*serialData & miso) ? j : 0;

            /* SCK should be 1 here */
            if (!(*serialData++ & sck)) {
                isError = true;
            }
        }
    }

    return !isError;
}


/**
 * Writes and reads back a number of bytes from FT232R. This is not limited by
 * maximum communication size.
 *
 * @param writeData pointer to bytes to write.
 * @param[out] readData where to store read bytes.
 * @param size how many bytes to write and optionally read.
 **/
void bitBang(const uint8_t* writeData, uint8_t* readData, uint32_t size)
{
    /* Split data up if it is too big and call bitBangBlock() */
    while (size > MAX_FT_COMM_SIZE - 1) {
        bitBangBlock(writeData, readData, MAX_FT_COMM_SIZE - 1);
        writeData += MAX_FT_COMM_SIZE - 1;
        readData += MAX_FT_COMM_SIZE - 1;
        size -= MAX_FT_COMM_SIZE - 1;
    }
    bitBangBlock(writeData, readData, size);
}


/**
 * Writes and reads back a number of bytes from FT232R. This function should
 * only be called by @c bitBang().
 *
 * Every time a byte is written the previous values are read from the pins
 * and placed in the RX buffer. Therefore this buffer should be cleared, if
 * it is not going to be read, to avoid messing up later communication.
 * If the RX buffer is going to be read, and aligned with the data that has
 * been written, a dummy byte is written so that the last byte of
 * real data can be read back. And the first byte read back is
 * ignored as it will have pin values from an earlier operation.
 *
 * @param writeData pointer to bytes to write.
 * @param[out] readData where to store read bytes.
 * @param size how many bytes to write and optionally read. MUST be less than
 *     @c MAX_FT_COMM_SIZE - 1
 **/
void bitBangBlock(const uint8_t* writeData, uint8_t* readData, uint32_t size)
{
    uint8_t* outData;
    uint8_t* inData;
    uint32_t writeSize = size + 1;
    int32_t status = 0;
#if defined (FTDI_USE_D2XX)
    uint32_t bytesWritten;
    uint32_t bytesRead = 0;
#endif

    outData = (uint8_t*)malloc(size + 1);
    inData = (uint8_t*)malloc(size + 1);
    if (outData == NULL || inData == NULL) {
        fprintf(stderr, "ERROR: Could not allocate %u bytes at %s.\n",
                size + 1, LOC);
        exit(EXIT_FAILURE);
    }

    /*
     * Create data that will be written, with added "dummy" byte (same pin
     * values as the last "real" byte).
     */
    memmove(outData, writeData, size);
    outData[size] = outData[size - 1];

    /* Write all bytes and wait for write to finish */
#if defined (FTDI_USE_D2XX)
    status = FT_Write(ftHandle, (LPVOID)outData , writeSize,
                                                         (DWORD*)&bytesWritten);
    if (bytesWritten != writeSize) {
            fprintf(stderr, "ERROR: Only wrote %u bytes of %u (%s).\n",
                    bytesWritten, writeSize, stripPath(LOC));
        free(inData);
        free(outData);
        exit(EXIT_FAILURE);
    }
#elif defined (FTDI_USE_LIBUSB)
    status = ftdi_write_data(ftHandle, outData, writeSize);
#endif
    checkFtError(status, LOC);

    /* Read back bytes */
#if defined (FTDI_USE_D2XX)
    status = FT_Read(ftHandle, (LPVOID)inData, writeSize, (DWORD*)&bytesRead);
    if (bytesRead != writeSize) {
        fprintf(stderr, "ERROR: Only read %u bytes of %u (%s).\n",
                bytesRead, writeSize, stripPath(LOC));
        free(outData);
        free(inData);
        exit(EXIT_FAILURE);
    }

#elif defined (FTDI_USE_LIBUSB)
    status = ftdi_read_data(ftHandle, inData, writeSize);
#endif
    checkFtError(status, LOC);

    memmove(readData, inData + 1, size);

    free(inData);
    free(outData);
}


/**
 * This function should be called with the return code from every Ft function.
 * If there's an error, it's printed out and the program exits.
 *
 * @param code error code.
 * @param location filename and line number from where function is called.
 **/
void checkFtError(int32_t code, const char* location)
{
#if defined (FTDI_USE_D2XX)
    if (code == FT_OK) return;      /* No error */
    if (code <= FT_OTHER_ERROR) {
        fprintf(stderr, "ERROR: %s at %s\n",
                ftErrorStrings[code], stripPath(location));
        exit(EXIT_FAILURE);
    }
    else {
        fprintf(stderr, "ERROR: FT:%i at %s\n", code, stripPath(location));
        exit(EXIT_FAILURE);
    }
#elif defined (FTDI_USE_LIBUSB)
    if (code < 0) {
        fprintf(stderr, "ERROR %i: %s at %s\n", code,
                ftdi_get_error_string(ftHandle), stripPath(location));
        exit(EXIT_FAILURE);
    }
#endif
}

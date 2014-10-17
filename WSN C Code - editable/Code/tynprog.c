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
 * Main application file.
 *
 * Application for programming Tyndall wireless sensor nodes. This application
 * is able to program nodes with the ATmega128 and the nRF9E5. In each case, the
 * EEPROM/Flash is programmed through the FT232R chip. The FT232R is set to
 * bit-bang mode to use the SPI protocol to read from and write to the
 * EEPROM/Flash. This application works with binary files. External tools may be
 * needed to convert between binary and Intel hex formats.
 *
 * @file tynprog.c
 * @date 27-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "nrf9e5.h"
#include "atmega.h"
#include "ftSpi.h"


/* Different types of memory that can be programmed */
#define STR_MEM_NRF9E5          "nrf9e5"
#define STR_MEM_ATMEGA_FLASH    "atmega_flash"
#define STR_MEM_ATMEGA_EEPROM   "atmega_eeprom"
#define STR_MEM_ATMEGA_FUSES    "atmega_fuses"


/** Structure to hold parsed command line options. **/
typedef struct {
    memType_t memType;              /**< description of memory. **/
    bool isReading;                 /**< 1 = reading from memory. **/
    bool isWriting;                 /**< 1 = writing to memory. **/
    bool isErasing;                 /**< 1 = writing to memory. **/
    char* filename;                 /**< Name of file to read/write. **/
    bool isReprogram;               /**< 1 = support wireless reprogramming. **/
    bool isSlowClock;               /**< 1 = uses slow clock speed for SPI. **/
    uint32_t nrf_crystalFrequency;  /**< Frequency of crystal in Mhz. **/
} args_t;


/* Function prototypes. */
static void parseCommandLine(int argc, char *argv[], args_t* args);
static void printHelpMessage(const char* programName);
static void printTitle(void);
static void writeAndVerify(uint8_t* data, spiDeviceInfo_t* device);


/**
 * Main function.
 *
 * @param argc number of command line arguments.
 * @param argv strings containing command line arguments.
 * @return @c EXIT_SUCCESS or @c EXIT_FAILURE.
 **/
int main(int argc, char* argv[])
{
    args_t args;
    uint8_t* data;
    FILE* file = NULL;
    spiDeviceInfo_t* device;

    atexit(ftSpi_close);

    printTitle();

    parseCommandLine(argc, argv, &args);

    if (args.memType == MEM_NRF9E5) {
        device = nrf_init(args.nrf_crystalFrequency);
    }
    else {
        device = atmega_init(args.memType, args.isSlowClock);
    }

    if (NULL == (data = (uint8_t*)malloc(device->memorySize))) {
        fprintf(stderr, "ERROR: could not allocate %u bytes at %s.\n",
                device->memorySize, LOC);
        exit(EXIT_FAILURE);
    }
    
    if (args.isErasing) {
        printf(" Erasing...\n");
        device->erase();
        printf(" Memory erased.\n\n");
        return EXIT_SUCCESS;
    }

    memset(data, 0xFF, device->memorySize);

    /* Fuse values are handled differently to other memory */
    if (args.memType == MEM_ATMEGA_FUSES) {
        if (args.isWriting) {
            /* Parse fuse values on command line */
            atmega_parseFuseValues(args.filename, data);

            /* Write fuse values */
            device->write(data, 0, device->memorySize);
            printf(" Writing fuses...\n");
        }

        /* Read fuses and print to screen */
        device->read(data, 0, device->memorySize);
        atmega_printFuseInfo(data);

        return EXIT_SUCCESS;
    }

    /* Open file on disk */
    if ((file = fopen(args.filename, args.isReading ? "wb" : "rb")) == NULL) {
        fprintf(stderr, "ERROR: could not open file %s.\n", args.filename);
        return EXIT_FAILURE;
    }

    /* Read from device and write to file */
    if (args.isReading) {
        printf(" Reading...\n");
        device->read(data, 0, device->memorySize);

        /* Write data to binary file */
        if (device->memorySize != fwrite(data, 1, device->memorySize, file)) {
            fprintf(stderr,
                    "ERROR: could not write %i bytes to %s at %s.\n",
                    device->memorySize, args.filename, stripPath(LOC));
            fclose(file);
            return EXIT_FAILURE;
        }

        printf(" Completed reading from memory.\n\n");
    }
    else if (args.isWriting) {

        /* Read file into RAM */
        uint32_t fileSize = fread(data, 1, device->memorySize, file);

        /* Check that there is no more data in the file */
        if (getc(file) != EOF) {
            fprintf(stderr, "ERROR: %s must be less than %u bytes in size.\n",
                   args.filename, device->memorySize);
            return EXIT_FAILURE;
        }

        device->addHeader(data, fileSize, args.isReprogram);

        /* Write data to memory */
        printf(" Programming %u bytes to %u byte memory (%.2f%% full)...\n",
               device->writeAdds[0][1], device->codeSize,
               100.0*((double)(device->writeAdds[0][1])/device->codeSize));

        writeAndVerify(data, device);
        printf(" Completed writing and verification successfully.\n\n");
    }
    free(data);
    fclose(file);
    return EXIT_SUCCESS;
}


/**
 * Parses command line options.
 *
 * @param argc number of arguments.
 * @param argv argument strings.
 * @param args structure where arguments will be stored.
 **/
static void parseCommandLine(int argc, char* argv[], args_t* args)
{
    char* memTypeString = NULL;
    bool isMemTypeSet = false;
    bool isFreqSet = false;
    char* opt = NULL;
    int i;

    /* Initialise options */
    args->isReading = false;
    args->isWriting = false;
    args->isErasing = false;
    args->isReprogram = false;
    args->nrf_crystalFrequency = NRF_DEFAULT_CRYSTAL_FREQUENCY;
    args->isSlowClock = false;
    args->memType = 0;
    args->filename = NULL;


    /* Loop through each command line argument, ignoring executable name */
    i = 1;
    while (i < argc) {
        if (argv[i][0] != '-') {
            fprintf(stderr, "ERROR: Arguments must begin with '-'.\n\n");
            printHelpMessage(argv[0]);
        }
        if (argv[i][2] != '\0') {
            fprintf(stderr,
                    "ERROR: Arguments must be seperated by a space.\n\n");
            printHelpMessage(argv[0]);
        }
        if (((i + 1) < argc) && (argv[i + 1][0] != '-')) {
            opt = argv[i + 1];
        }
        else {
            opt = NULL;
        }
        switch (argv[i][1]) {
        case 'c':   args->nrf_crystalFrequency = atoi(opt);
                    isFreqSet = true;
                    break;
        case 'e':   args->isErasing = true;
                    break;
        case 'h':   printHelpMessage(argv[0]);
                    break;
        case 'i':   args->isReprogram = true;
                    break;
        case 'm':   isMemTypeSet = true;
                    memTypeString = opt;
                    break;
        case 'r':   args->isReading = true;
                    args->filename = opt;
                    break;
        case 's':   args->isSlowClock = true;
                    break;
        case 'w':   args->isWriting = true;
                    args->filename = opt;
                    break;
        default:    printf("ERROR: Invalid argument ('%c').\n\n", argv[i][1]);
                    printHelpMessage(argv[0]);
        }
        if (opt != NULL) {
            i++;
        }
        i++;
    }

    /* Check that memory type is valid */
    if (isMemTypeSet) {
        if (strcmp(memTypeString, STR_MEM_NRF9E5) == 0) {
            args->memType = MEM_NRF9E5;
        }
        else if (strcmp(memTypeString, STR_MEM_ATMEGA_FLASH) == 0) {
            args->memType = MEM_ATMEGA_FLASH;
        }
        else if (strcmp(memTypeString, STR_MEM_ATMEGA_EEPROM) == 0) {
            args->memType = MEM_ATMEGA_EEPROM;
        }
        else if (strcmp(memTypeString, STR_MEM_ATMEGA_FUSES) == 0) {
            args->memType = MEM_ATMEGA_FUSES;
        }
        else {
            fprintf(stderr, "ERROR: Memory type is not recognised.\n\n");
            printHelpMessage(argv[0]);
        }
    }
    else {
        fprintf(stderr, "ERROR: A memory type must be specified.\n\n");
        printHelpMessage(argv[0]);
    }

    /* Check that a filename has been entered if reading from memory */
    if ((args->memType != MEM_ATMEGA_FUSES) && args->isReading &&
                                                 (args->filename == NULL)) {
        fprintf(stderr,
                "ERROR: Must specify filename for saving memory to.\n\n");
        printHelpMessage(argv[0]);
    }

    /* Check crystal oscillator frequency is valid */
    if ((args->nrf_crystalFrequency != 4) &&
       (args->nrf_crystalFrequency != 8) &&
       (args->nrf_crystalFrequency != 12) &&
       (args->nrf_crystalFrequency != 16) &&
       (args->nrf_crystalFrequency != 20)) {
        fprintf(stderr, "ERROR: Invalid crystal frequency.\n\n");
        printHelpMessage(argv[0]);
    }
}


/**
 * Prints help message. Exits program when finished.
 *
 * @param programName name of program executable.
 **/
static void printHelpMessage(const char* programName)
{
    const char* filename;

    filename = stripPath(programName);

    /* Print help message */
    fprintf(stderr,
" Usage: %s [options]\n\n"
" Options:\n"
"   -c <frequency>     Value in MHz for crystal oscillator frequency. This is\n"
"                      only for the nRF9E5. Valid values are 4, 8, 12, 16 or 20.\n"
"                      If this option is not used, 20 will be assumed.\n\n"
"   -e                 Erases memory.\n\n"
"   -h                 Print this help message.\n\n"
"   -i                 To allow support for in-system reprogramming.\n\n"
"   -m <memory type>   Select memory that will be written to. Valid options are:\n"
"                      "STR_MEM_NRF9E5", "STR_MEM_ATMEGA_FLASH", "
STR_MEM_ATMEGA_EEPROM", or "STR_MEM_ATMEGA_FUSES".\n\n"
"   -r <file>          Read memory and store it in <file> (in binary format).\n"
"                      Overwrites any existing file of the same name.\n\n"
"   -s                 Use very slow clock speed for programming (%u bits/s).\n\n"
"   -w <file>          Write <file> to memory with verification.\n\n"
" Only one operation (erase, read, or write) can be performed in a single\n"
" execution. \n\n"
" For fuses, replace <file> with <ext_byte><high_byte><low_byte>. Example fuse\n"
" settings to use the internal 8MHz clock are FF91E4 for Atmega128, and FF91E2\n"
" for ATmega1281.\n\n"
" The format of the file type is raw binary. External tools should be used to\n"
" convert to/from other formats (e.g. Intel-Hex format).\n\n"
" Only one device can be programmed at a time.\n\n"
, filename, SLOW_BAUDRATE);

    exit(EXIT_FAILURE);
}


/**
 * Prints title message.
 **/
static void printTitle(void)
{
    uint32_t i;

    putchar(' ');
    for (i = 0; i < CMD_WINDOW_WIDTH - 2; ++i) {
        putchar('-');
    }
    putchar('\n');

    printf(" Programmer for 25mm and 10mm modular Tyndall nodes\n"
        " Build time: "__DATE__  " / "__TIME__ "\n");
#if defined (FTDI_USE_D2XX)
    printf(" FTDI D2XX version\n");
#elif defined (FTDI_USE_LIBUSB)
    printf(" libftdi / libusb version\n");
#endif

    putchar(' ');
    for (i = 0; i < CMD_WINDOW_WIDTH - 2; ++i) {
        putchar('-');
    }
    printf("\n");
}


/**
 * Writes data to the device, then reads it back compares with data that has
 * been written.
 *
 * @param data the data to write.
 * @param device what memeory to use, and what address to write to.
 **/
static void writeAndVerify(uint8_t* data, spiDeviceInfo_t* device)
{
    uint8_t readData[device->memorySize];
    uint32_t i, j, address, size;
    uint32_t errorCount = 0;

    /* Write */
    for (i = 0; i < 4; i++) {
        if (device->writeAdds[i][1] == 0) continue;
        device->write(data + device->writeAdds[i][0], device->writeAdds[i][0],
                      device->writeAdds[i][1]);
    }

    /* Read back and verify */
    printf(" Verifying...\n");
    for (i = 0; i < 4; i++) {
        if (device->writeAdds[i][1] == 0) continue;

        address = device->writeAdds[i][0];
        size = device->writeAdds[i][1];
        
        /* Read back data in order to verify */
        device->read(readData, address, size);
        for (j = 0; j < size; j++) {
            if (readData[j] != data[j + address]) {
                /* If it fails, try to rewrite and read back again */
                device->write(data + address + j, address + j, 1);
                device->read(readData + j, address + j, 1);
                if (readData[j] == data[j + address]) {
                    continue;
                }
                fprintf(stderr, "\nERROR: at address %6u (0x%.4X): "
                        "written = %.2X, read = %.2X",
                        j + address, j + address,
                        data[j + address], readData[j]);
                errorCount++;
                /* Stop if there are more than MAX_VERIFY_ERRORS errors */
                if (errorCount > MAX_VERIFY_ERRORS && MAX_VERIFY_ERRORS != 0) {
                    fprintf(stderr,
                            "\n\nERROR: Check connections and power switch.\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    if (errorCount > 0) {
        fprintf(stderr, "\n\nERROR: Check connections and power switch.\n");
        exit(EXIT_FAILURE);
    }
}

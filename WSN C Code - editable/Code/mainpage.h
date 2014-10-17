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

/**
Main documentation information. Contains no code!
@file mainpage.h
@date 27-Jan-2010
@author Seán Harte
@mainpage

Instructions for using the Tyndall USB Programmer

@par Installation

    The executable file @e tynprog.exe should be placed in a directory that is
    in the PATH system variable. An easy way to do this is to copy it to the
    @e utils/bin directory of WinAVR.

    The latest FTDI drivers should be installed. These can be found here:
    http://www.ftdichip.com/FTDrivers.htm. Version 2.06.00 is known to work, but
    other versions should be ok too.

    If there are problems with @e tynprog.exe, tynprog_oss.exe can be used
    instead. This version does not require the FTDI drivers, but requires the
    open source software @e libusb filter driver to be installed:
    http://libusb-win32.sourceforge.net/#installation
    In order to use this version from within Programmer's Notepad, it should be
    renamed to @e tynprog.exe, replacing the other version.

    The EEPROM in the FT232R chip must be programmed before @e tynprog.exe can
    recognise the board. This should be done when the board is first assembled,
    and will only need to be done once. The file @e Tyndall25mmProg.xml (for
    25mm USB boards) or @e Tyndall10mmProg.xml must be programmed using FTProg:
    http://www.ftdichip.com/Resources/Utilities.htm


@par Usage

    Connect the Tyndall USB programming board, with the transceiver layer that
    you want to program, to the PC/laptop. Note that it is not possible to
    program the 25mm Nordic nRF2401 board, or the 25mm ZigBee revA board using
    USB. This is because the SPI clock pin is not available on the connector.

    Make sure the power switch on the programming board is in the correct
    position.

    @e tynprog.exe can be called from Programmer's Notepad using the @e [WinAVR]
    Program command in the tools menu. To do this the @c PROG_PORT variable in
    @e apps/makerules should be set to @c USB, e.g.:
    @code PROG_PORT = USB @endcode

    @e tynprog.exe can also be called from the command line. Some examples for
    setting fuses, writing program code to Flash memory, and reading back the
    EEPROM:
    @code    tynprog.exe -m atmega_fuses -w FF91EF @endcode
    @code    tynprog.exe -w atmega_flash -w adcToRf.bin @endcode
    @code    tynprog.exe -m atmega_eeprom -r eeprom.bin @endcode
    @code    tynprog.exe -m nrf9e5 -i -w app.bin @endcode
    @code    tynprog.exe -m nrf9e5 -e @endcode

    @e tynprog.exe works with only with raw binary files. Some compilers may
    only generate intel-hex formatted files. The tool @e srec_cat that is
    installed with WinAVR can be used to convert between the formats.

    To convert from hex to binary (replace file1.* with real filenames):
    @code    srec_cat file1.hex --intel -o file1.bin -binary @endcode

    To convert from binary to hex (replace file1.* with real filenames):
    @code    srec_cat file1.bin --binary -o file1.hex -intel @endcode

    If a 25mm board is being programmed for the first time, the @c '-s' option
    must be used, as the default clock frequency is 1MHz which is not fast
    enough for full speed communication.

    A header is always added when programming a 10mm board. This header is 
    necessary for the nRF9E5 to be able to read the memory correctly. A file
    which contains the actual content that is programmed to the board is output
    called @e nrf9e5_out.bin.

    Note that only one device can be programmed at a time, and therefore only
    one programming board should be connected before programming.

@par Command-line Options

@verbatim
 Usage: tynprog [options]

 Options:
   -c <frequency>     Value in MHz for crystal oscillator frequency. This is
                      only for the nRF9E5. Valid values are 4, 8, 12, 16 or 20.
                      If this option is not used, 20 will be assumed.

   -e                 Erases memory.

   -h                 Print this help message.

   -i                 To allow support for in-system reprogramming.

   -m <memory type>   Select memory that will be written to. Valid options are:
                      nrf9e5, atmega_flash, atmega_eeprom, or atmega_fuses.

   -r <file>          Read memory and store it in <file> (in binary format).
                      Overwrites any existing file of the same name.

   -s                 Use very slow clock speed for programming (9600 bits/s).

   -w <file>          Write <file> to memory with verification.

 Only one operation (erase, read, or write) can be performed in a single
 execution.

 For fuses, replace <file> with <ext_byte><high_byte><low_byte>. Example fuse
 settings to use the internal 8MHz clock are FF91E4 for Atmega128, and FF91E2
 for ATmega1281.

 The format of the file type is raw binary. External tools should be used to
 convert to/from other formats (e.g. Intel-Hex format).

 Only one device can be programmed at a time.
@endverbatim


@par Information for Developers

    This software works by interfacing with the FT232R chip, which has eight
    pins that can be used for bit-banging. This software communicates with the
    FT232R through the API provided by the D2XX device drivers. An array of
    bytes is passed to the FT232R and appears as parallel data on the eight
    pins, each of which can be set to be an input (high-impedance) or an output.
    The logic value of each pin can is also read back every time the pins are
    written to. This allows bi-directional communication.

    In order to access the API, the files @e ftd2xx.h and @e ftd2xx.lib
    (available with the drivers) must be put in the same folder as the source
    files, and @e ftd2xx.lib must be linked to when compiling. 

    Alternatively, the FT232R can be accessed by the functions in libftdi. These
    functions are described and implemented in @e ftdi.h and @e ftdi.c, which
    are available from http://www.intra2net.com/en/developer/libftdi/. libftdi 
    uses libusb to perform the actual interaction with the chip. This must be
    installed on the PC where the programmer will run. The functions in libusb
    are detailed in @e usb.h, and the library @e libusb.a must be linked to when
    compiling. The libusb driver, and other files are available from
    http://libusb-win32.sourceforge.net/
    
    To build with the D2XX driver, @c FTDI_USE_D2XX must be defined when
    compiling, @e ftd2xx.lib must be linked to, and @e ftd2xx.h should be in the
    same folder as other files.

    To build with libftdi and libusb, @c FTDI_USE_LIBUSB must be defined when
    compiling, @e libusb.a must be linked to, @e ftdi.c should be compiled and
    linked to, and @e ftdi.h and @e usb.h should be in the same folder as other
    files.
    
    The CodeLite IDE (http://codelite.org/) has been successfully used to
    compile this, and a workspace file is included with the sources files
    (@e tynprog.workspace). This workspace contains two projects for linking
    with D2XX or libusb / libftdi.

    The bit-banging mechanism is used to communicate with the target
    microcontroller using the SPI protocol. The functions for this are in
    @e ft_spi.h. Each microcontroller (AVR ATmegaXXX, and nRF9E5) uses different
    commands to control programming, so requires their own implementation of a
    programming interface. This is done in the @e atmega.c and @e nrf9e5.c
    files. The interface itself is defined by the @c spiDeviceInfo_t structure
    in @e common.h. Any new microcontrollers for which suppport is added should
    provide the same set of commands to allow them to be easily integrated, and
    @e tynprog.c shouldn't need to be modified.

    There is a significant delay every time the FT232R is accessed. In order to
    speed-up the operation of the application, the number of calls to the
    functions in @e ft_spi.h should be reduced as much as possible. This may
    require buffering commands and sending them all at once.

@par Copyright and License

    @e ftdi.c and @e ftdi.h are copyrighted to Intra2net AG, and are distributed
    under the GPL license. @e usb.h and @e libusb.a are copyrighted to Johannes
    Erdfelt and are also distributed under the GPL license. @e ftd2xx.h and
    @e ftd2xx.lib are copyrighted to FTDI Ltd. and can be distributed royalty
    free.

    All other files are copyrighted to the Tyndall National Institute and are
    distributed under the BSD license:

@verbatim
Copyright (c) 2010, Tyndall National Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the Tyndall National Institute nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
@endverbatim
**/

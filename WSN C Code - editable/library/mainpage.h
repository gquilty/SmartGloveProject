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
Main documentation information.
@file mainpage.h
@date 16-Apr-2010
@author Seán Harte
@mainpage


@section intro Information about C code for Tyndall 25mm and 10mm sensor nodes

This page gives short instructions on using the source code provided. The source
code is for using the CC/EM2420 transceiver or the nRF905 transceiver with the
Atmega128 microcontroller. It also supports the 8051-based microcontroller in
the Nordic nRF9E5 transceiver.


<br>
@section Contents

 - @ref changelog
 - @ref winavr
 - @ref iar
 - @ref pn
 - @ref layout
 - @ref makefiles
 - @ref programming
 - @ref adding
 - @ref tyn25
 - @ref tyn10
 - @ref tips
 - @ref troubleshooting
 - @ref further
 - @ref license


<br>
@section changelog Changelog

 - 2009-06-03
     - Initial Release.


 - 2009-06-05
     - Added support for SHTx1 humidity/temperature sensor (@e sht.h, @e sht.c).


 - 2009-06-18
     - Made it possible to access rf packet header information from main
       context, not just in the callback. This means that
       @c rf_setReceiveBuffer() now takes a pointer to a struct, and not a
       pointer to an array. See @e rfToUart.c to see an example of this, and of
       accessing address data from main context.
     - Added example app showing use of @e sht.h.


 - 2009-06-26
     - Changed @c board_init() in the board header files to give low-power sleep
       modes when using the USB programming board.
     - Added @c adc_disable() and @c uart_disable() functions


 - 2009-07-07
     - Added support for timers on Atmega1281 (see @e delay_avr.c)

 - 2010-01-17
     - Added @c adc_readMultiple() function to @e adc.h.
     - Added CRC functions.
     - Generated Doxygen documentation.
     - Combined @c putHexWord() and @c putHexByte() into @c putHex().
     - Split @c putNumber() into @c putNum() and @c putUNum() (unsigned).
     - Added lossless compression support (see @e slzw.h).
     - Added data encryption (see @e xxtea.h).
     - Added self-programming support (see @e reprog.h).
     - Networking/MAC encapsulation (see @e msg.h).
     
 - 2010-01-27
     - Fixed bug in @c adc_readMultiple() that prevented reading from ADC
       Channel 0.
       
 - 2010-02-12
     - Added support for 1.1V internal reference in ATmega1281. See
       @c ADC_REF_INTERNAL_1_1V in @e adc_avr.h.

 @todo nRF2401 support.
 @todo MSP430 and ADF7020 support.
 @todo Dynamic clock scaling?
 @todo Interface to all sensors on Tyndall boards (IMU etc.).
 @todo Comms: dynamic routing algorithms / freq. hopping etc.
 @todo Cross platform watchdog.

<br>
@section winavr WinAVR

WinAVR is a collection of software for programming Atmel AVR based
microcontrolers such as the ATmega128. It includs a C library (avr-libc), a C
compiler (avr-gcc), and a device programmer (avrdude). It is all open-source
software and can be downloaded for free from
http://sourceforge.net/projects/winavr/. Version 20090313 is known to work.

If WinAVR is not installed with administrator access, then the WinAVR paths will
not be added to the environmental variable @e "%PATH%". This must be done
manually as follows:

 -# Right-click on "My Computer". Then go to the "Advanced" tab and click on
    "Environment Variables".
 -# In the User variables section look for a variable called @e PATH. If it
    doesn't exist click new and enter a variable name PATH. If it exists, select
    it and click edit.
 -# Two paths from WinAVR must be added:
     - @e "[winavr]\bin"
     - @e "[winavr]\utils\bin"
     .
    These should be separated by a semicolon from each other and any previous
       paths. For example:
       @code "C:\Windows;C:\WinAVR-20090313\bin;C:\WinAVR-20090313\utils\bin"
       @endcode
 -# Click "OK" multiple times to save the new variable value and close the
       window

The WinAVR user manual should be read. It gives an overview of the components
that make up WinAVR, and how they are used.


<br>
@section iar IAR MCS-51

IAR MCS-51 is a compiler for 8051 microcontrollers. It is only necessary to
install this if you will be using Tyndall 10mm modular nodes. It is available in
a "KickStart Edition". This is limited in that the output binary file must be
less than 4kBytes. As the nRF9E5 only has 4kBytes, this is not a problem. It can
be downloaded from http://supp.iar.com/Download/SW/?item=EW8051-KS4. Version
7.51A is known to work.

As above for WinAVR, the path should be added to @e "%PATH%". For this compiler
the path will be something similar to:
@code
"C:\Program Files\IAR Systems\Embedded Workbench 5.3\8051\bin"
@endcode

If not installed to the default path, the path must also be added to the
@e "makerules" file in the @e "apps" directory. On line 29:
@code
IAR_DIR = C:/Program Files/IAR Systems/Embedded Workbench 5.3/8051
@endcode


<br>
@section pn Using Programmer's Notepad

WinAVR comes with a text editor called Programmer's Notepad. This can be used as
an IDE for projects. It can keep track of a group of files that make up a
project. It can also call command line programs through the Tools menu. Three
options are available after installation:

 - @e "[WinAVR] Make All": compiles the current application and generates
   output files.
 - @e "[WinAVR] Make Clean": clears directory of output files.
 - @e "[WinAVR] Program": compiles current application and programs the
   microcontroller.

To select which application will be compiled, the main Programmer's Notepad
window must be open with a file in the application folder.

The three commands above all call the program @e make.exe with different command
line parameters to achieve different behaviour. The behaviour is controlled by
makefiles, which specify which further commands to use to compile files etc. The
makefiles have been modified to support both 25mm, and 10mm nodes.


<br>
@section layout Layout of code library

The provided source code is split into two directories: @e apps/ and @e library/

@e library/ contain libraries that can be used by all the applications. In the
root of the library folder are some header files that provide a generic
interface to some common features, e.g. the radio (CC2420 or nRF905) can be
accessed using the functions in @e rf.h

To use one these functions just include the header file. The implementation
files that are specific to the AVR microcontroller are in the @e avr/ sub-
directory. Files that are specific to 10mm nRF9E5 boards are in the @e nrf/
sub-directory.

The @e board/ directory contains header files that describe each Tyndall
transciever board. The use of these is described below.

The @e apps/ directory contains a number of different applications e.g.
@e adcToRf/ and @e rfToUart/. Each application has a source file with the
@c main() function, and also a @e makefile.


<br>
@section makefiles Makefiles

The @e makefile specifies a number of application variables:

 - TARGET:       the name of the file with the main() function
 - BOARD_HEADER: header file listing pins used on atmega. This is the only
   thing that has to be changed to use different boards. The boards that can
   be selected are:
     - @e "board/25_nrf905_revA_433.h"
     - @e "board/25_nrf905_revA_868.h"
     - @e "board/25_nrf905_revB_433.h"
     - @e "board/25_nrf905_revB_868.h"
     - @e "board/25_2420_revA.h"
     - @e "board/25_2420_revB.h"
     - @e "board/10_nrf9e5_433.h"
     - @e "board/10_nrf9e5_433.h"

The makefile also provides the ability to override some default settings for the
libraries, for example UART baudrate.

Additionally, some libraries support an optional callback function, where the
library can call a function in the application, to signal that an event has
happened, e.g. the ADC has finished a conversion. To use these functions,
uncomment (by removing the @c # at the start of the line) one of the lines.

Variables selecting which libraries to use for @c printf() and @c scanf() are
also usable.

All the options are explained within the makeiles that are with the example
applications.

The makefile then includes the makerules file in the apps directory. This file
lists all the source files that will be included in the build. This is not
wasteful as, in the final output file, the compiler only includes code for
functions that are called. The makerules file then includes either the
@e 10mm_makerules or @e 25_makerules. This selection is made automatically.
These files contains all the rules for building and programming.


<br>
@section programming Programming flash and debugging

For programming the microcontroller: the @e "*.hex" (when using JTAG programmer)
or the @e "*.bin" (when using USB board) file is used. This can be programmed to
the microcontroller's memory by using the @e "[WinAVR] Program" command from
Programmer's Notepad.

If using a JTAG programmer, the COM port that the JTAG programmer is connected
to should be set in @e makerules, using the @c PROG_PORT variable e.g.:
@code
PROG_PORT = com
@endcode

The type of JTAG programmer must also be set appropriately in @e makerules. This
can be @c jtagmkI or @c jtagmkII

Alternatively, this should be set to @c USB if using the Tyndall USB board.
Look at the tynprog documentation for more information about this method.

For debugging (in AVR Studio), the @e "*.elf" file should be used.


<br>
@section adding Adding library modules

If you have written some code that may be useful for other people, e.g. routing
algorithm, hardware interface. It is better to create a library module and not
just keep the code in your application file. To create a library module, create
two new files in the @e library/ directory. For example if you have code that
reads from a digital light sensor, you would create two file:
@e digitalLightSensor.h and d@e igitalLightSensor.c. In the @e "*.h" file, you
should put the function prototypes e.g.
@code
void digitalLightSensor_init(void);
uint16_t digitalLightSensor_read(void);
@endcode
In the @e "*.c" file will be the actual implementation of these functions. The
module must be added to the @e makerules file so that the compiler can find it.
A line such as this is used:
@code
SRC += $(LIB_PATH)/digitalLightSensor.c
@endcode
Then just include the file in you application:
 @code
#include "digitalLightSensor.h"
...
value = digitalLightSensor_read();
@endcode


<br>
@section tyn25 Tyndall 25mm node characterisation

Two radios are supported by 25mm nodes: CC2420/EM2420 and nRF905

They have different power consumption, throughput, and range. Due to operating
in the 433MHz band, at similiar energy consumption levels, the nRF905 wil have
significantly longer range compared to the EM2420.

@par Power Consumption

The readings below were tken with fuse values of @c FFD9FF. This is using
external 8MHz crystal, and JTAG is disabled. Only 1 board of each type was
measured. Variations of +-10% could easily be possible.
@verbatim
                                 , EM2420 , nRF905 , nRF905@16MHz, nRF905/USB
SLEEP_POWERDOWN + RF_MODE_OFF    ,  2.1 uA,  2.3 uA,             ,     6.0 uA
SLEEP_POWERDOWN + RF_MODE_SLEEP  , 57.9 uA,  2.4 uA,             ,
SLEEP_POWERDOWN + RF_MODE_STANDBY,  469 uA,  164 uA,             ,
delay_s(time) + RF_MODE_OFF      ,  9.2 uA, 10.7 uA,             ,    14.3 uA
SLEEP_IDLE + RF_MODE_OFF         ,  4.4 mA, 4.25 mA,      8.91 mA,
active + RF_MODE_OFF             , 8.32 mA, 7.86 mA,     16.48 mA,
active + RF_MODE_SLEEP           , 8.37 mA, 7.91 mA,             ,
active + RF_MODE_STANDBY         ,  8.8 mA, 8.05 mA,             ,
active + RF TX max.              , 24.9 mA, 34.8 mA,             ,
active + RF TX min.              , 17.3 mA, 16.9 mA,             ,
active + RF_MODE_RECEIVING       , 26.8 mA, 20.3 mA,             ,@endverbatim

@par Throughput

Throughput for EM2420 without CCA (all values are bits/sec)
@verbatim
                       , RF_MODE_STANDBY, RF_MODE_SLEEP
Sending 115 bytes (max),          195040,        188839
Sending 30 bytes       ,          132888,        124041
Sending 2 bytes        ,           18904,         17442@endverbatim

Max Throughput for EM2420 with CCA (clear channel assesment)
@verbatim
                         RF_MODE_STANDBY, RF_MODE_SLEEP
Sending 115 bytes (max),          172353,        171874
Sending 30 bytes       ,          107952,        106075
Sending 2 bytes        ,           13630,         13366@endverbatim

Throughput for nRF905
@verbatim
                        RF_MODE_STANDBY, RF_MODE_SLEEP
Sending 30 bytes (max),           36120,         24778
Sending 2 bytes       ,            7412,          3112@endverbatim


<br>
@section tyn10 Tyndall 10mm node characterisation

@par Power Consumption

Note that these values are for a full node, not just a transceiver layer (as in
25mm measurements). So current in power down mode is made up of leakage current
into the battery charger and through I/O switches, sleep current of I2C eeprom,
SPI eeprom, nRF9e5, and sensor's regulator, and supply current of transceiver's
regulator.
@verbatim
                       , 500kHz , 4MHz   , 20MHz
powerdown              ,   5.4uA,   5.4uA,   5.4uA
delay_s() low power    ,   5.4uA,   5.4uA,   5.4uA
delay_s() accurate     , 170.2uA, 192.4uA,   292uA
uC active              , 0.709mA, 1.053mA,  2.63mA
uc active + RF rx      , 12.93mA, 13.23mA, 14.86mA
uC active + RF tx (min),  9.50mA,  9.80mA,  11.4mA
uC active + RF tx (max), 27.60mA,  27.9mA,  29.5mA@endverbatim

@par Throughput

Throughput (bits/second)
@verbatim
      , 2 Bytes, 30 Bytes
500kHz,    2986,    12336
4MHz  ,    5701,    28272
20MHz ,    6357,    33168@endverbatim


<br>
@section tips Tips

Microcontroller programming has some differences when compared with writing C
applications that run on a PC. They often operate only on 8 bit values. For this
reason the smallest possible size shold be used for variables. If you know a
variable will never have a value greater than 255, then it should be declared as
@c uint8_t, and not @c int. The @c 'u' in @c uint8_t stands for unsigned.

Also it is common that you want change only a single bit in a value.
 - To set a bit use code like this: @c "value |= BIT(3);"
 - To clear a bit use code like this: @c "value &= ~BIT(3);"
 - To set multiple bits do this: @c "value |= BIT(3) | BIT(5);"
 - To clear multiple bits do this: @c "value &= ~(BIT(3) | BIT(5));"
 - To flip bits do this: @c "value ^= BIT(3) | BIT(5);"


<br>
@section troubleshooting Troubleshooting

If code is not working like you would expect it to, here are some things that
may have gone wrong:

 - Are the fuses on the microcontroller set correctly? If ATmega103 mode is
   selected, strange things will happen, including the LEDS always being on.
   Also if the OCDEN fuse is set, the power consumption will be higher. But be
   careful not to disable JTAGEN and SPIEN, as then it will be very difficult
   to reprogram the board...

 - If two nodes are not communicating using their radios, make sure that they
   are trying to communicate on the same channel and know each other's address
   address. Also, make sure that there is no major interence on the selected
   channel. If the nodes are too close, this can be another source of
   communication problems.

 - If programming fails make sure the correct option for @c PROG_PORT in
   @e makerules is set. To find the COM port that the JTAG programmer is using,
   follow these steps: right-click "my computer", then go to "Device Manager"
   in the "Hardware" tab. Look for "USB Serial Port" under "Ports (COM & LPT)".

 - Check everything is connected correctly and powered properly.

 - Use the version of WinAVR recommended above (newer may also work). Use
   the latest version of the FTDI driver.


<br>
@section further Further information

AVR Libc documentation contains information on the standard libraries and
provides example code: http://www.nongnu.org/avr-libc/user-manual/index.html

The webforum at http://www.avrfreaks.net/ has lots of stuff about general Atmel
AVR programming

The Make manual fully describes the operation of Make and the format of
makefiles: http://www.gnu.org/software/make/manual/

The files are documented using Doxygen formatted comments, from which this
documentation is created. Information on this can be found at
http://www.doxygen.org

The IAR MCS-51 compiler contains documentation on its libraries and compiler
options.

Datasheets for the components and schematics for the boards being used should be
examined.

A good reference for C programming is 2nd edition of
"The C Programming Language" by Kernighan and Ritchie. ISBN: 0-13-110362-8.

<br>
@section license License

Code is distributed under the BSD license:

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

Exceptions to this are @e slzw.h and @e slzw.c which are GPL code, and
@e xxtea.c and xxtea.h which are public domain.
**/

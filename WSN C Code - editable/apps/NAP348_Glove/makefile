# Application makefile.
# This is used to configure the compilation of an application.
# Lines beginning with '#' are commented out, and do not affect the output.

# Filename of the file with the "main()" function. This must not include the
# file path, or the file extension
TARGET = NAP348_Glove_v1_0

# The board header selects what type of board is being targetted
#BOARD_HEADER = "board/10_nrf9e5_433.h"
#BOARD_HEADER = "board/25_nrf905_revB_433.h"
BOARD_HEADER = "board/25_2420_revB.h"

# Processer speed of the microcontroller in Hz. The 25mm are commonly at 8MHz
# (8000000), and the 10mm are commonly at 20MHz)
F_CPU = 8000000
#F_CPU = 20000000

# MCU name (only used for 25mm boards).
MCU = atmega128

CDEFS += -DRF_LOCAL_ADDRESS=0x7979
CDEFS += -DRF_NETWORK_ID=0x31B5

# Uncomment to override the default configuration values. Note that currently
# the 10mm boards have a fixed UART baudrate of F_CPU/32.
CDEFS += -DUART_BAUDRATE=1000000
#115200UL
#CDEFS += -DUART_BAUD_TOL=5
#CDEFS += -DUART_PARITY=0
#CDEFS += -DUART_DO_NOT_INSERT_RETURN
#CDEFS += -DI2C_FREQ=100000
#CDEFS += -DRF_MAX_PAYLOAD_SIZE=28
#CDEFS += -DRF_CARRIER_DETECT
#CDEFS += -DLED_NOT_USED
#CDEFS += -DSHT_LOW_RES_ADC=1

# Uncomment if a callback function is needed
#CDEFS += -DDELAY_USE_CALLBACK_S
#CDEFS += -DDELAY_USE_CALLBACK_MS
#CDEFS += -DADC_USE_CALLBACK
#CDEFS += -DUART_USE_CALLBACK

# Comment these lines if a library module is not needed to save space.
ADC_USE = TRUE
UART_USE = TRUE
DELAY_USE = TRUE
#BWT_USE = TRUE
#EXTERNINT_USE = TRUE

# Uncomment if you want floating point support in printf() or scanf(). This is
# only for the 25mm boards.
PRINTF = FLOAT_SUPPORT
SCANF = FLOAT_SUPPORT

# Remove printf() from the 10mm boards.
CDEFS += -DPRINTF_NOT_USED

# Make simple_io library a bit smaller by removing some features.
CDEFS += -DSIMPLEIO_SMALLER

# Include general rules for compiling, linking and programming.
include ../makerules

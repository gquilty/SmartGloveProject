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
 * Functions for using radio on nRF9E5 or nRF905.
 *
 * @file rf_nrf9x5.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "spi.h"
#include "delay.h"
#include "rf.h"
#include "spi_nrfCommands.h"


/*------------------------------------------------------------------------------
 *  Macro functions.
 */
#if defined (UC_AVR)
#   define enablePwrUp()            RF_PWR_UP_PORT |= BIT(RF_PWR_UP)
#   define disablePwrUp()           RF_PWR_UP_PORT &= ~BIT(RF_PWR_UP)
#   define enableTrxCe()            RF_TRX_CE_PORT |= BIT(RF_TRX_CE)
#   define disableTrxCe()           RF_TRX_CE_PORT &= ~BIT(RF_TRX_CE)
#   define enableTxEn()             RF_TX_EN_PORT |= BIT(RF_TX_EN)
#   define disableTxEn()            RF_TX_EN_PORT &= ~BIT(RF_TX_EN)

#   define isAm()                   (RF_AM_PIN & BIT(RF_AM))
#   define isDr()                   (RF_DR_PIN & BIT(RF_DR))
#   define isCd()                   (RF_CD_PIN & BIT(RF_CD))

#   define rf_clearIntFlag()
#else
#   define enablePwrUp()
#   define disablePwrUp()
#   define enableTxEn()             P2_TX_EN = 1
#   define enableTrxCe()            P2_TRX_CE = 1
#   define disableTxEn()            P2_TX_EN = 0
#   define disableTrxCe()           P2_TRX_CE = 0

#   define isCd()                   P2_CD

#   define rf_initInterrupt()       (PX4 = 1)           /* High priority */
#   define rf_clearIntFlag()        EXIF &= ~BIT(IE4);
#   define rf_enableInterrupt()     (EX4 = 1)
#   define rf_disableInterrupt()    (EX4 = 0)
#endif


/*------------------------------------------------------------------------------
 *  Module variables.
 */


/** Variable to keep track of the current state. **/
static volatile uint8_t rf_mode = RF_MODE_SLEEP;

/** Needed to restore mode after it has been temporarily modified. **/
static volatile uint8_t rf_oldMode;

/** Is the transmitter turned on. **/
static volatile bool isTransmitting;

/** Recieved bytes will be stored here. **/
static volatile rf_msgType* rxMsg;

/** Store current value of channel. **/
static uint16_t rf_channel;

/** Store current value of power. **/
static uint16_t rf_power;


/*------------------------------------------------------------------------------
 *  Function prototypes.
 */
static void setChannelConfig(void);
static void setTxAddress(uint32_t address);
FORCE_INLINE static void endTransmission(void);


/******************************************************************************\
 * See rf.h and rf_nrf9x5.h for documentation of these functions
\******************************************************************************/

void rf_init(uint16_t channel, uint8_t power)
{
    /* Check parameters */
    channel &= 0x01FF;      /* Only 9 bits */
    power &= RF_PWR_10;

    /* Initialise global variables */
    isTransmitting = false;
    rf_channel = channel;
    rf_power = power;

#if defined (UC_AVR)
    /* Init pins to communicate with radio */
    disablePwrUp();
    disableTrxCe();
    disableTxEn();
    RF_TRX_CE_DDR |= BIT(RF_TRX_CE);
    RF_PWR_UP_DDR |= BIT(RF_PWR_UP);
    RF_TX_EN_DDR |= BIT(RF_TX_EN);
    RF_CD_DDR &= ~BIT(RF_CD);
    RF_AM_DDR &= ~BIT(RF_AM);
    RF_DR_DDR &= ~BIT(RF_DR);
    RF_CD_PORT &= ~BIT(RF_CD);
    RF_AM_PORT &= ~BIT(RF_AM);
    RF_DR_PORT &= ~BIT(RF_DR);

    spi_init();     /* For nrf9E5, this is done in board_init() */
#endif

    disableInterrupts();

    /* Set chip select low */
    spi_enableCsn();

    /* Write bytes to the config register */
    spi_readWriteByte(W_RF_CONFIG);
    spi_readWriteByte(LOW_BYTE(channel));
    spi_readWriteByte(power | (HFREQ_PLL_VALUE << HFREQ_PLL) |
                                          ((HIGH_BYTE(channel)) << CH_NO_BIT8));
    spi_readWriteByte((4 << TX_AFW) | (4 << RX_AFW));
    spi_readWriteByte(RF_MAX_PAYLOAD_SIZE+2);
    spi_readWriteByte(RF_MAX_PAYLOAD_SIZE+2);
    spi_readWriteByte(LOW_BYTE(RF_LOCAL_ADDRESS));
    spi_readWriteByte(HIGH_BYTE(RF_LOCAL_ADDRESS));
    spi_readWriteByte(LOW_BYTE(RF_NETWORK_ID));
    spi_readWriteByte(HIGH_BYTE(RF_NETWORK_ID));
#if defined (UC_AVR)
    /* This is done in board_init() for nrf9E5 boards */
    spi_readWriteByte((CRC_MODE_16BIT << CRC_MODE) | (1 << CRC_EN) |
                                                            (XOF_20MHZ << XOF));
#endif

    /* Finish SPI communication */
    spi_disableCsn();
    enableInterrupts();
    rf_initInterrupt();
}


void rf_send(uint16_t address, const uint8_t* msg, uint8_t length)
{
    static uint16_t prevAddress = 0;

    /* Check that packet isn't too big */
    if (length > RF_MAX_PAYLOAD_SIZE) {
        length = RF_MAX_PAYLOAD_SIZE;
    }

    while (isTransmitting) {
        ;
    }

    /* Set destination address, if this address is different to previous */
    if (address != prevAddress) {
        setTxAddress(address);
        prevAddress = address;
    }

    /* Turn on radio */
    rf_oldMode = rf_mode;
    rf_setMode(RF_MODE_STANDBY);
    isTransmitting = true;

    /* Write data using SPI */
    disableInterrupts();
    spi_enableCsn();
    spi_readWriteByte(W_TX_PAYLOAD);
    spi_readWriteByte(LOW_BYTE(RF_LOCAL_ADDRESS));         /* Low byte */
    spi_readWriteByte(HIGH_BYTE(RF_LOCAL_ADDRESS));        /* High byte */
    for (uint8_t i = length; i > 0; --i) {
        spi_readWriteByte(*msg++);
    }
    spi_disableCsn();
    enableInterrupts();

    enableTrxCe();                  /* Turn on radio */
#ifdef RF_CARRIER_DETECT
    delay_us(650);
    while (isCd()) {
        ;                           /* Wait until channel is free */
    }
#endif
    enableTxEn();                   /* Start transmission */
    rf_enableInterrupt();
#ifdef RF_INTERRUPT_AM  /* revA of 25mm nRF905 board */
    /* Wait until transmission is finished */
    while (! isDr()) {
        ;
    }
    endTransmission();
#endif
}


void rf_sendCarrier(void)
{
    rf_setMode(RF_MODE_STANDBY);

    /* Start a transmission, but leave TRX_CE and TX_EN high */
    rf_disableInterrupt();
    enableTrxCe();
    enableTxEn();
    /* An unmodulated carrier is now being transmitted */
}


void rf_setMode(uint8_t mode)
{

    if (mode == rf_mode) {
        return;
    }

    /* Wait for active transmission to finish before changing */
    while (isTransmitting) {
        ;
    }

    if (mode != RF_MODE_RECEIVING) {
        /* Turn off reception */
        disableTrxCe();
        rf_disableInterrupt();
    }
#if defined (UC_AVR)
    /* Go to sleep mode */
    if (mode == RF_MODE_SLEEP) {
        disablePwrUp();
    }
    /* Disable radio completely */
    if (mode == RF_MODE_OFF) {
        disablePwrUp();

        /* Bring pins to 0 */
        RF_CD_DDR |= BIT(RF_CD);
        RF_AM_DDR |= BIT(RF_AM);
        RF_DR_DDR |= BIT(RF_DR);

        RF_CD_PORT &= ~BIT(RF_CD);
        RF_AM_PORT &= ~BIT(RF_AM);
        RF_DR_PORT &= ~BIT(RF_DR);
    }
    /* Go to standy mode, if not already there */
    else if ((mode == RF_MODE_RECEIVING && rf_mode != RF_MODE_STANDBY) ||
                    (mode == RF_MODE_STANDBY && rf_mode != RF_MODE_RECEIVING)) {
        disableTxEn();
        enablePwrUp();
        delay_us(3000);
    }
#endif
    /* Turn on receiver */
    if (mode == RF_MODE_RECEIVING) {
        enableTrxCe();
        rf_enableInterrupt();
    }

    rf_mode = mode;
}


void rf_setReceiveBuffer(volatile rf_msgType* receiveBuffer)
{
    disableInterrupts();
    rxMsg = receiveBuffer;
    rxMsg->length = RF_MAX_PAYLOAD_SIZE;        /* Constant packet size */
    enableInterrupts();
}


void rf_setChannel(uint16_t channel)
{
    channel &= 0x01FF;      /* Only 9 bits */
    rf_channel = channel;
    setChannelConfig();
}


void rf_setPower(uint8_t power)
{
    power &= RF_PWR_10;
    rf_power = power;
    setChannelConfig();
}


void rf_increasePower(void)
{
    if (rf_power != RF_PWR_MAX) {
        rf_power += 4;
        setChannelConfig();
    }
}


void rf_decreasePower(void)
{
    if (rf_power != RF_PWR_MIN) {
        rf_power -= 4;
        setChannelConfig();
    }
}


void rf_setMulticastAddress(void)
{
    disableInterrupts();
    spi_enableCsn();
    spi_readWriteByte(W_RF_CONFIG | 5);
    spi_readWriteByte(0xFF);
    spi_readWriteByte(0xFF);
    spi_disableCsn();
    enableInterrupts();
}


void rf_setUnicastAddress(void)
{
    disableInterrupts();
    spi_enableCsn();
    spi_readWriteByte(W_RF_CONFIG | 5);
    spi_readWriteByte(LOW_BYTE(RF_LOCAL_ADDRESS));
    spi_readWriteByte(HIGH_BYTE(RF_LOCAL_ADDRESS));
    spi_disableCsn();
    enableInterrupts();
}


/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/


/**
 * Load new settings using the channel config command. Uses globally declared
 * channelConfig.
 **/
static void setChannelConfig(void)
{
    disableInterrupts();
    spi_enableCsn();
    spi_readWriteByte(CHANNEL_CONFIG | (rf_power << CC_PA_PWR) |
                           (HFREQ_PLL_VALUE << CC_HFREQ_PLL) | rf_channel >> 8);
    spi_readWriteByte((uint8_t)rf_channel);
    spi_disableCsn();
    enableInterrupts();
}


/**
 * Set address to send data to.
 *
 * @param address 32 bit address to send data to.
 **/
static void setTxAddress(uint32_t address)
{
    disableInterrupts();
    spi_enableCsn();

    /* Write TX address to register */
    spi_readWriteByte(W_TX_ADDRESS);
    spi_readWriteByte(LOW_BYTE(address));
    spi_readWriteByte(HIGH_BYTE(address));
    spi_readWriteByte(LOW_BYTE(RF_NETWORK_ID));
    spi_readWriteByte(HIGH_BYTE(RF_NETWORK_ID));

    spi_disableCsn();
    enableInterrupts();
}


/** Returns radio to state it was in before transmission started. **/
FORCE_INLINE static void endTransmission(void)
{
    disableTxEn();
    if (rf_oldMode != RF_MODE_RECEIVING) {
        /* Turn off reception */
        disableTrxCe();
        rf_disableInterrupt();
    }
#if defined (UC_AVR)
    if (rf_oldMode == RF_MODE_SLEEP) {
        /* Turn off transceiver */
        disablePwrUp();
    }
#endif
    rf_mode = rf_oldMode;
    isTransmitting = false;
}


#if defined (UC_8051)
#    define RF_INT_VECTOR IE4_int
#endif
/** Interrupt triggered when finished transmitting or data is recieved. **/
ISR(RF_INT_VECTOR)
{
    disableInterrupts();
    rf_clearIntFlag();
#ifdef RF_INTERRUPT_AM  /* revA of 25mm nRF905 board */
    /* Wait for data to be ready */
    while (! isDr()) {
        /* If address match goes low it means the message was not decoded */
        /* Could do a NACK here */
        if (! isAm()) {
            enableInterrupts();
            return;
        }
    }
#else
    /* Finished transmitting */
    if (isTransmitting) {
        endTransmission();
        enableInterrupts();
        return;
    }
#endif

    /*
     * SPI functions are not used, as it causes a lot of bloat when calling
     * them from within an interrupt.
     */
    spi_enableCsn();

    SPI_WRITE_BYTE(R_RX_PAYLOAD);
    rxMsg->srcAddress = SPI_READ_WRITE_BYTE(0);                 /* Low byte */
    rxMsg->srcAddress |= (uint16_t)SPI_READ_WRITE_BYTE(0) << 8; /* high byte */
    for (uint8_t i = 0; i < RF_MAX_PAYLOAD_SIZE; ++i) {
        /* Read received bytes into memory */
        rxMsg->data[i] = SPI_READ_WRITE_BYTE(0);
    }
    spi_disableCsn();

    /* Pass data to application */
    rf_callback(rxMsg);

    enableInterrupts();
}

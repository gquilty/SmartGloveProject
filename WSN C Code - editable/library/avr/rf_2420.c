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
 * Code for using Texas Instruments CC2420 / Ember EM2420.
 * @todo make use of encryption and auto-ack features of chip.
 *
 * @file rf_2420.c
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#include "global.h"
#include "delay.h"
#include "rf.h"
#include "spi.h"


/*------------------------------------------------------------------------------
 * Addresses of registers.
 */
#define SNOP            0x00
#define SXOSCON         0x01
#define STXCAL          0x02
#define SRXON           0x03
#define STXON           0x04
#define STXONCCA        0x05
#define SRFOFF          0x06
#define SXOSCOFF        0x07
#define SFLUSHRX        0x08
#define SFLUSHTX        0x09
#define SACK            0x0A
#define SACKPEND        0x0B
#define SRXDEC          0x0C
#define STXENC          0x0D
#define SAES            0x0E
#define MAIN            0x10
#define MDMCTRL0        0x11
#define MDMCTRL1        0x12
#define RSSI            0x13
#define SYNCWORD        0x14
#define TXCTRL          0x15
#define RXCTRL0         0x16
#define RXCTRL1         0x17
#define FSCTRL          0x18
#define SECCTRL0        0x19
#define SECCTRL1        0x1A
#define BATTMON         0x1B
#define IOCFG0          0x1C
#define IOCFG1          0x1D
#define MANFIDL         0x1E
#define MANFIDH         0x1F
#define FSMTC           0x20
#define MANAND          0x21
#define MANOR           0x22
#define AGCCTRL         0x23
#define AGCTST0         0x24
#define AGCTST1         0x25
#define AGCTST2         0x26
#define FSTST0          0x27
#define FSTST1          0x28
#define FSTST2          0x29
#define FSTST3          0x2A
#define RXBPFTST        0x2B
#define FSMSTATE        0x2C
#define ADCTST          0x2D
#define DACTST          0x2E
#define TOPTST          0x2F
#define RESERVED        0x30
#define TXFIFO          0x3E
#define RXFIFO          0x3F

/* Bitwise-Or register address with this when reading them */
#define REG_READ        0x40

/* Status byte bits. */
#define XOSC16M_STABLE  6
#define TX_UNDERFLOW    5
#define ENC_BUSY        4
#define TX_ACTIVE       3
#define LOCK            2
#define RSSI_VALID      1

/* RAM Addresses. */
#define RAM_TXFIFO      0x000
#define RAM_RXFIFO      0x080
#define RAM_KEY0        0x100
#define RAM_RXNONCE     0x110
#define RAM_SABUF       0x120
#define RAM_KEY1        0x130
#define RAM_TXNONCE     0x140
#define RAM_CBCSTATE    0x150
#define RAM_IEEEADR     0x160
#define RAM_PANID       0x168
#define RAM_SHORTADR    0x16A


/* FCF, seq. num., PAN ID, dest. address, srcs address, footer */
#define RF_PACKET_OVERHEAD_SIZE     (2 + 1 + 2 + 2 + 2 + 2)

/* FCF (Frame control field) */
#define RF_FCF_NOACK                0x8861

#define RF_LENGTH_MASK              0x7F
#define RF_CRC_OK_MASK              0x80


/* Pin Access Macros */
#define isFifo()        (RF_FIFO_PIN & BIT(RF_FIFO))
#define isFifop()       (RF_FIFOP_PIN & BIT(RF_FIFOP))
#define isSfd()         (RF_SFD_PIN & BIT(RF_SFD))

#define enableReset()   (RF_RESETN_PORT &= ~BIT(RF_RESETN))
#define disableReset()  (RF_RESETN_PORT |= BIT(RF_RESETN))

#define enableVreg()    (RF_VREG_PORT |= BIT(RF_VREG_EN))
#define disableVreg()   (RF_VREG_PORT &= ~BIT(RF_VREG_EN))


/******************************************************************************\
 * Inline functions.
\******************************************************************************/

/** Set pins for communicating with radio. **/
static inline void initPins(void)
{
    RF_RESETN_DDR |= BIT(RF_RESETN);
    RF_RESETN_PORT |= BIT(RF_RESETN);
    RF_VREG_DDR |= BIT(RF_VREG_EN);
    RF_FIFOP_DDR &= ~BIT(RF_FIFOP);
    RF_FIFO_DDR &= ~BIT(RF_FIFO);
    RF_SFD_DDR &= ~BIT(RF_SFD);
    RF_CCA_DDR &= ~BIT(RF_CCA);
}


/** Set pins to allow Atmega to go to low power sleep. **/
static inline void unsetPins(void)
{
    RF_FIFOP_DDR |= BIT(RF_FIFOP);
    RF_FIFO_DDR |= BIT(RF_FIFO);
    RF_SFD_DDR |= BIT(RF_SFD);
    RF_CCA_DDR |= BIT(RF_CCA);
}

static uint8_t getByte(void);
static void sendByte(uint8_t byte);
static void setRegister(uint8_t address, uint16_t value);
static void sendRamAddress(uint16_t address);
static void writeFifo(const uint8_t* data, uint8_t size);
static void getFifo(volatile uint8_t* data, uint8_t size);
static void discardFifo(uint8_t size);


/******************************************************************************\
 * Global variables.
\******************************************************************************/

/** Current sequence number, incremented every time a message is sent. **/
static uint8_t txSeqNumber;

static uint16_t shortAddress = RF_LOCAL_ADDRESS;
static uint16_t panId = RF_NETWORK_ID;

/** Where to put bytes received over the radio. **/
static volatile rf_msgType* buffer;

/** Mode that radio is currently in. **/
static uint8_t rf_mode = RF_MODE_SLEEP;

/** Current RF output power. **/
static uint8_t rf_power;


/******************************************************************************\
 * See rf.h for documentation of these functions.
\******************************************************************************/

void rf_init(uint16_t channel, uint8_t power)
{
    uint8_t statusByte;

    initPins();
    spi_init();

    /* Turn on regulator and reset chip */
    enableVreg();
    delay_us(600);
    enableReset();
    delay_us(1000);
    disableReset();
    delay_us(500);

    /* Set the RF channel */
    rf_setChannel(channel);

    /* Set the output power */
    rf_setPower(power);

    /* Initialize the FIFOP external interrupt */
    rf_initInterrupt();

    /* Disable interrupts while accessing SPI */
    disableInterrupts();

    /* Enable oscillator */
    sendByte(SXOSCON);

    /* Set correlation threshold to 20 */
    setRegister(MDMCTRL1, 0x0500);

    /* Set RXBPF to 1 as recommended in datasheet */
    setRegister(RXCTRL1, 0x2A56);

    /* Set the FIFOP threshold to maximum (127 bytes) */
    setRegister(IOCFG0, 0x007F);

    /* Disable RXFIFO_PROTECTION */
    setRegister(SECCTRL0, 0x01C4);

    txSeqNumber = 0;

    /* Wait for the crystal oscillator to become stable */
    do {
        statusByte = getByte();
    } while (!(statusByte & BIT(XOSC16M_STABLE)));

    /* Write addresses to RAM. The oscillator must be running for this. */
    spi_enableCsn();
    sendRamAddress(RAM_SHORTADR);
    spi_readWriteByte(RF_LOCAL_ADDRESS & 0xFF);  /* LSB first */
    spi_readWriteByte(RF_LOCAL_ADDRESS >> 8);
    spi_disableCsn();

    spi_enableCsn();
    sendRamAddress(RAM_PANID);
    spi_readWriteByte(RF_NETWORK_ID & 0xFF);
    spi_readWriteByte(RF_NETWORK_ID >> 8);
    spi_disableCsn();

    /* Disable oscillator to save power */
    sendByte(SXOSCOFF);

    enableInterrupts();
}


void rf_send(uint16_t address, const uint8_t* msg, uint8_t length)
{
    uint16_t frameControlField = RF_FCF_NOACK;
    uint8_t packetLength = length + RF_PACKET_OVERHEAD_SIZE;
    uint8_t oldMode;

    if (length > RF_MAX_PAYLOAD_SIZE) {
        return;
    }

    /* Turn radio to standby mode, and save original mode */
    oldMode = rf_mode;
    rf_setMode(RF_MODE_STANDBY);

    /* Wait until the transceiver is idle */
    while (isFifop() || isSfd()) {
        ;
    }

    /* Disable interrupts while accessing SPI */
    disableInterrupts();

    /* Flush the TX FIFO buffer */
    sendByte(SFLUSHTX);

#ifdef RF_CARRIER_DETECT
    uint8_t statusByte;
    /* Turn on receiver and wait for RSSI values to be valid */
    if (rf_mode != RF_MODE_RECEIVING) {
        sendByte(SRXON);
    }
    do {
        statusByte = getByte();
    } while (!(statusByte & BIT(RSSI_VALID)));
#endif

    /* Write packet to the TX FIFO (footer is appended automatically) */
    writeFifo(&packetLength, 1);                    /* Packet Length */
    writeFifo((uint8_t*)&frameControlField, 2);     /* Frame control field */
    writeFifo(&txSeqNumber, 1);                     /* Sequence Number */
    writeFifo((uint8_t*)&panId, 2);                 /* PAN ID */
    writeFifo((uint8_t*)&address, 2);               /* Destination address */
    writeFifo((uint8_t*)&shortAddress, 2);          /* Source address */
    writeFifo(msg, length);                         /* Payload */

    /* Send packet */
#ifdef RF_CARRIER_DETECT
    sendByte(STXONCCA);
#else
    sendByte(STXON);
#endif

    /* Wait until transmission starts (SFD field has been sent) */
    while (!isSfd()) {
        ;
    }

    /* Increment the sequence number*/
    txSeqNumber++;
    
#ifdef RF_CARRIER_DETECT
    if (rf_mode != RF_MODE_RECEIVING) {
        /* Wait until transmission is finished, then disable receiver */
        while (isSfd()) {
            ;
        }
        sendByte(SRFOFF);
    }
#endif

    /* Return to original mode */
    rf_setMode(oldMode);

    /* Turn interrupts back on */
    enableInterrupts();
}


void rf_sendCarrier(void)
{
    rf_setMode(RF_MODE_STANDBY);
    setRegister(MDMCTRL1, 0x0008);    /* MDMCTRL1.TX_MODE = 2 */
    setRegister(DACTST, 0x1800);
    sendByte(STXON);
}


void rf_setReceiveBuffer(volatile rf_msgType* receiveBuffer)
{
    buffer = receiveBuffer;
}


void rf_setMode(uint8_t mode)
{
    uint8_t statusByte;

    if (mode == rf_mode) {
        return;
    }

    disableInterrupts();

    /* Wait until any active transmission is finished before changing */
    do {
        statusByte = getByte();
    } while (statusByte & BIT(TX_ACTIVE));

    /* Turn off RX */
    if (mode != RF_MODE_RECEIVING && rf_mode == RF_MODE_RECEIVING) {
        sendByte(SRFOFF);
        rf_disableInterrupt();
    }

    /* Go to sleep mode (oscillator off) */
    if (mode == RF_MODE_SLEEP) {
        sendByte(SXOSCOFF);
    }

    /* Disable radio completely, ans configure pins for sleep */
    if (mode == RF_MODE_OFF) {
        disableVreg();
        unsetPins();
    }

    /* Go to standby mode (oscillator on), if not already there */
    if ((mode == RF_MODE_RECEIVING && rf_mode != RF_MODE_STANDBY) ||
                    (mode == RF_MODE_STANDBY && rf_mode != RF_MODE_RECEIVING)) {
        sendByte(SXOSCON);
        /* Wait for the crystal oscillator to become stable */
        do {
            statusByte = getByte();
        } while (!(statusByte & BIT(XOSC16M_STABLE)));
    }

    /* Turn on receiver */
    if (mode == RF_MODE_RECEIVING) {
        sendByte(SRXON);
        sendByte(SFLUSHRX);
        rf_enableInterrupt();
    }

    enableInterrupts();

    rf_mode = mode;
}


void rf_setChannel(uint16_t channel)
{
    uint16_t f;

    /* Check input parameter */
    if ((channel > 26) || (channel < 11)) {
        channel = RF_CHANNEL_CENTRE;
    }

    /* Calculate frequency from channel number (see datasheet) */
    f = ((uint16_t)(channel - 11) * 5) + 357;
    f |= 0x4000;        /* To set bits in rest of register */

    /* Write it to the CC2420 */
    disableInterrupts();
    setRegister(FSCTRL, f);
    enableInterrupts();
}


void rf_setPower(uint8_t power)
{
    if ((power != RF_PWR_0) &&
            (power != RF_PWR_NEG1) &&
            (power != RF_PWR_NEG3) &&
            (power != RF_PWR_NEG5) &&
            (power != RF_PWR_NEG7) &&
            (power != RF_PWR_NEG10) &&
            (power != RF_PWR_NEG15) &&
            (power != RF_PWR_NEG25)) {
        power = RF_PWR_MAX;
    }
    
    rf_power = power;

    disableInterrupts();
    setRegister(TXCTRL, 0xA000 | (uint16_t)rf_power);
    enableInterrupts();
}


void rf_increasePower(void)
{
    if (rf_power != RF_PWR_MAX) {
        rf_power += 4;
        rf_setPower(rf_power);
    }
}


void rf_decreasePower(void)
{
    if (rf_power != RF_PWR_MIN) {
        rf_power -= 4;
        rf_setPower(rf_power);
    }
}


void rf_setMulticastAddress(void)
{
    /* Not needed for this chip */
}


void rf_setUnicastAddress(void)
{
    /* Not needed for this chip */
}


/******************************************************************************\
 * Functions local to this file.
\******************************************************************************/

/**
 * This interrupt is triggered when a full packet is received.
 **/
ISR(RF_INT_VECTOR)
{
    uint16_t frameControlField;
    uint8_t length;
    uint8_t footer[2];

    /* Check if FIFO overflow has happened */
    if((isFifop()) && (!(isFifo()))) {
        sendByte(SFLUSHRX);
        return;
    }

    /* Payload length */
    getFifo(&length, 1);
    length &= RF_LENGTH_MASK;

    /* Ignore the packet if the length is too short */
    if (length < RF_PACKET_OVERHEAD_SIZE) {
        discardFifo(length);
        return;
    }

    /* Store payload length */
    buffer->length = length - RF_PACKET_OVERHEAD_SIZE;

    /* Start reading the rest of the data */
    getFifo((uint8_t*) &frameControlField, 2);
    getFifo(&buffer->seqNumber, 1);

    /* Ignore destination address */
    discardFifo(4);

    /* Read the source address */
    getFifo((volatile uint8_t*) &buffer->srcAddress, 2);

    /* Read the packet payload */
    getFifo(buffer->data, length - RF_PACKET_OVERHEAD_SIZE);

    /* Read the footer to get the RSSI value */
    getFifo(footer, 2);
    
    /* Subtracting 45 as recommended by datasheet */
    buffer->rssi = footer[0] - 45;

    /* Check CRC, and call upper layer if ok */
    if (footer[1] & RF_CRC_OK_MASK) {
        rf_callback(buffer);
    }
}


/**
 * Read single SPI byte (status byte).
 *
 * @return byte read from SPI.
 **/
static uint8_t getByte(void)
{
    uint8_t byte;
    spi_enableCsn();
    byte = spi_readWriteByte(0);
    spi_disableCsn();
    return byte;
}


/**
 * Send single SPI byte (e.g. for strobe command).
 *
 * @param byte what byte to send.
 **/
static void sendByte(uint8_t byte)
{
    spi_enableCsn();
    spi_readWriteByte(byte);
    spi_disableCsn();
}


/**
 * Write a value to a register.
 *
 * @param address which register to write to.
 * @param value the 16-bit value to write to the register.
 **/
static void setRegister(uint8_t address, uint16_t value)
{
    spi_enableCsn();
    spi_readWriteByte(address);
    spi_readWriteByte((uint8_t) ((value) >> 8));
    spi_readWriteByte((uint8_t) (value));
    spi_disableCsn();
}


/**
 * Convert to format expected by radio (see datasheet) and send RAM address.
 *
 * @param address where to start writing to RAM within the CC2420.
 **/
static void sendRamAddress(uint16_t address)
{
    spi_readWriteByte(0x80 | (address & 0x7F));
    spi_readWriteByte((address >> 1) & 0xC0);
}


/**
 * Write data to the TX FIFO.
 *
 * @param data a pointer to the data that will be written.
 * @param size how many bytes of @a data to write.
 **/
static void writeFifo(const uint8_t* data, uint8_t size)
{
    spi_enableCsn();
    spi_readWriteByte(TXFIFO);
    for (uint8_t i = 0; i < size; i++) {
        spi_readWriteByte(data[i]);
    }
    spi_disableCsn();
}


/**
 * Read data from the RX FIFO.
 *
 * @param data a pointer to where to save the data that is read.
 * @param size how many bytes to read.
 **/
static void getFifo(volatile uint8_t* data, uint8_t size)
{
    spi_enableCsn();
    spi_readWriteByte(RXFIFO | REG_READ);
    for (uint8_t i = 0; i < size; i++) {
        data[i] = spi_readWriteByte(0);
    }
    spi_disableCsn();
}


/**
 * Read and ignore data from the RX FIFO.
 *
 * @param size how many bytes to read and discard.
 **/
static void discardFifo(uint8_t size)
{
    spi_enableCsn();
    spi_readWriteByte(RXFIFO | REG_READ);
    for (uint8_t i = 0; ((i < size) && isFifo()); i++){
        spi_readWriteByte(0);
    }
    spi_disableCsn();
}

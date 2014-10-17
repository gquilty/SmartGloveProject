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
 * Header file for accessing SPI on ATmega128.
 *
 * @file spi_avr.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef SPI_AVR_H
#define SPI_AVR_H


/* SPI pins. */
#define SPI_PORT            PORTB
#define SPI_DDR             DDRB
#define SPI_CSN             0           /* SPI Chip Select */
#define SPI_SCLK            1           /* SPI Serial Clock */
#define SPI_MOSI            2           /* SPI Master Out Slave In (SI) */
#define SPI_MISO            3           /* SPI Master In - Slave Out (SO) */


/** Enable chip select connected to radio. **/
FORCE_INLINE
static inline void spi_enableCsn(void)
{
    SPI_PORT &= ~BIT(SPI_CSN);
}

/** Disable chip select connected to radio. **/
FORCE_INLINE
static inline void spi_disableCsn(void)
{
    SPI_PORT |= BIT(SPI_CSN);
}


/** For the AVR, the spi byte function is always inlined anyway... **/
#define SPI_READ_WRITE_BYTE(byte) spi_readWriteByte(byte)

/** For the AVR, the spi byte function is always inlined anyway... **/
#define SPI_WRITE_BYTE(byte) spi_readWriteByte(byte)


/******************************************************************************\
 * See spi.h for documentation of these functions.
\******************************************************************************/

/** Enables SPI, selects master mode, clock rate F_CPU / 2, and SPI mode 0. **/
static inline void spi_init(void)
{
    SPI_PORT |= BIT(SPI_MOSI) | BIT(SPI_SCLK) | BIT(SPI_CSN) | BIT(SPI_MISO);

    SPCR = BIT(SPE) | BIT(MSTR);
   // SPSR = BIT(SPI2X);
    spi_disableCsn();
}


FORCE_INLINE
static inline void spi_disable(void)
{
    SPI_PORT &= ~(BIT(SPI_MOSI) | BIT(SPI_SCLK) | BIT(SPI_MISO));
    SPCR &= ~BIT(SPE);
}


FORCE_INLINE
static inline void spi_enable(void)
{
    SPCR |= BIT(SPE);
}


FORCE_INLINE
static inline uint8_t spi_readWriteByte(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & BIT(SPIF))) {
        ;
    }
    return SPDR;
}


#endif

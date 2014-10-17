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
 * Header file for accessing SPI on nRF9E5.
 *
 * @file spi_nrf.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef SPI_NRF_H
#define SPI_NRF_H

/* SPI pins. */
#define SPI_SCLK                0   /* SPI Serial Clock */
#define SPI_MOSI                1   /* SPI Master Out Slave In (SI) */
#define SPI_MISO                2   /* SPI Master In - Slave Out (SO) */
#define SPI_CSN                 3   /* SPI Chip Select */


/**
 * Always inlined SPI write function. This is provided so that interrupts can
 * communicate using SPI, without calling functions. This allows the interrupt
 * handler code to be much smaller.
 *
 * @param byte what value to write.
 * @return byte that was read.
 **/
FORCE_INLINE
static inline uint8_t SPI_READ_WRITE_BYTE(uint8_t byte)
{
    EXIF &= ~BIT(IE3);
    SPI_DATA = byte;
    while (!(EXIF & BIT(IE3))) {
        ;
    }
    return SPI_DATA;
}


/**
 * Always inlined SPI read function. This is provided so that interrupts can
 * communicate using SPI, without calling functions. This allows the interrupt
 * handler code to be much smaller.
 *
 * @param byte what value to write.
 **/
FORCE_INLINE
static inline void SPI_WRITE_BYTE(uint8_t byte)
{
    EXIF &= ~BIT(IE3);
    SPI_DATA = byte;
    while (!(EXIF & BIT(IE3))) {
        ;
    }
}


/** SPI can be connected internally to radio/ADC, or to external port pins. **/
FORCE_INLINE
static inline void spi_selectInternal(void)
{
    SPI_CTRL = BIT(SPI_RADIO);
}

/** SPI can be connected internally to radio/ADC, or to external port pins. **/
FORCE_INLINE
static inline void spi_selectExternal(void)
{
    SPI_CTRL = BIT(SPI_P1);
}


/** Enable chip select. Disables interrupts for communication period. **/
FORCE_INLINE
static inline void spi_enableCsn(void)
{
    P2_RACSN = 0;
}


/** Disable chip select. Re-enables interrupts. **/
FORCE_INLINE
static inline void spi_disableCsn(void)
{
    P2_RACSN = 1;
}


/******************************************************************************\
 * See spi.h for documentation of these functions.
\******************************************************************************/


FORCE_INLINE
static inline void spi_init(void)
{
    /* This is done in board_init(). */
}


FORCE_INLINE
static inline void spi_disable(void)
{
    SPI_CTRL = 0;
}


FORCE_INLINE
static inline void spi_enable(void)
{
    SPI_CTRL = BIT(SPI_RADIO);
}


#endif

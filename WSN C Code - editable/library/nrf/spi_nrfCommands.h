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
 * Some defines for communicating with the nrf9x5 radio, and also the ADC on the
 * nRF9E5. See datasheet for more details.
 *
 * @file spi_nrfCommands.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef SPI_NRFCOMMANDS_H
#define SPI_NRFCOMMANDS_H


/*------------------------------------------------------------------------------
 * SPI Commands
 */
#define W_RF_CONFIG         0x00    /* Low 4bits = start address */
#define R_RF_CONFIG         0x10    /* Low 4bits = start address */
#define W_TX_PAYLOAD        0x20
#define R_TX_PAYLOAD        0x21
#define W_TX_ADDRESS        0x22
#define R_TX_ADDRESS        0x23
#define R_RX_PAYLOAD        0x24
#define CHANNEL_CONFIG      0x80    /* 1000 pphc cccc cccc: p = PA_PWR,
                                               h = HFREQ_PLL, c = CH_NO */
                                               
#define R_ADC_DATA          0x40    /* LSB = which byte to read */
#define W_ADC_CONFIG        0x44
#define R_ADC_CONFIG        0x46
#define START_ADC_CONV      0xC0    /* Low 4bits = CHSEL */

/*------------------------------------------------------------------------------
 * RF Configuration Register Bit Positions
 */
/* Byte 0 = CH_NO bit[7-0] */

/* Byte 1 */
#define CH_NO_BIT8          0
#define HFREQ_PLL           1
#define PA_PWR              2
#define RX_RED_PWR          4
#define AUTO_RETRAN         5

/* Byte 2 */
#define RX_AFW              0
#define TX_AFW              4

/* Byte 3 = RX_PW bit[5-0] */
/* Byte 4 = TX_PW bit[5-0] */
/* Byte 5-8 = RX_ADDRESS */

/* Byte 9 */
#define UP_CLK_FREQ         0
#define XO_DIRECT           2
#define XOF                 3
#define CRC_EN              6
#define CRC_MODE            7

/*------------------------------------------------------------------------------
 * RF Configuration Register Bit Values (shift these using values above)
 */
#define HFREQ_PLL_433       0x00
#define HFREQ_PLL_868       0x01

#define PA_PWR_NEG10        0x00
#define PA_PWR_NEG2         0x01
#define PA_PWR_6            0x02
#define PA_PWR_10           0x03

#define UP_CLK_FREQ_4MHZ    0x00
#define UP_CLK_FREQ_2MHZ    0x01
#define UP_CLK_FREQ_1MHZ    0x02
#define UP_CLK_FREQ_500KHZ  0x03

#define XO_DIRECT_UP_CLK    0x00
#define XO_DIRECT_XOF       0x01

#define XOF_4MHZ            0x00
#define XOF_8MHZ            0x01
#define XOF_12MHZ           0x02
#define XOF_16MHZ           0x03
#define XOF_20MHZ           0x04

#define CRC_MODE_8BIT       0x00
#define CRC_MODE_16BIT      0x01

/*------------------------------------------------------------------------------
 * CHANNEL_CONFIG Bit Positions and Bit Masks
 */
#define CC_PA_PWR               8       /* Value is already shifted by PA_PWR */
#define CC_PA_PWR_BM            0x0C00  /* xxxx11xx xxxxxxxx */
#define CC_HFREQ_PLL            9
#define CC_HFREQ_PLL_BM         0x0200  /* xxxxxx1x xxxxxxxx */
#define CC_CH_NO                0
#define CC_CH_NO_BM             0x01FF  /* xxxxxxx1 11111111 */

/*------------------------------------------------------------------------------
 * ADC Configuration Register Bit Positions
 */
/* Byte 0 */
#define CSTARTN             0
#define ADCRUN              1
#define ADC_PWR_UP          2
#define VFSSEL              3
#define CHSEL               4

/* Byte 1 */
#define RESCTRL             0
#define DIFFMODE            2
#define ADC_RL_JUST         3


/*------------------------------------------------------------------------------
 * ADC Configuration Register Bit Values (shift these using values above)
 */
#define VFFSEL_INTERN       0x00
#define VFFSEL_EXTERN       0x01

#define CHSEL_1_3_VDD       0x10

#define RESCTRL_6_BIT       0x00
#define RESCTRL_8_BIT       0x01
#define RESCTRL_10_BIT      0x02
#define RESCTRL_12_BIT      0x03

#define ADC_RL_JUST_LEFT    0x00
#define ADC_RL_JUST_RIGHT   0x01

#endif

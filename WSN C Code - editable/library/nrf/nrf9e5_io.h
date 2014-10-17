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
 * SFR, interrupt vector and internal SPI definitions for Nordic nRF9E5. SFRs
 * are delcared as in compiler.h from SDCC (http://sdcc.sourceforge.net)
 *
 * @file nrf9e5_io.h
 * @date 15-Jan-2010
 * @author Seán Harte
 ******************************************************************************/


#ifndef NRF9E5_H
#define NRF9E5_H


#define SBIT(name, addr, bit)   __bit __no_init volatile bool name @ (addr+bit)
#define SFR(name, addr)         __sfr __no_init volatile unsigned char name @ addr


/*------------------------------------------------------------------------------
 * SFR definitions
 */
SFR(P0,         0x80);  /**< SFR: Port 0 **/
    SBIT(P0_0,          0x80,   0); /**< Port 0 bit 0.**/
    SBIT(P0_1,          0x80,   1); /**< Port 0 bit 1.**/
    SBIT(P0_2,          0x80,   2); /**< Port 0 bit 2.**/
    SBIT(P0_3,          0x80,   3); /**< Port 0 bit 3.**/
    SBIT(P0_4,          0x80,   4); /**< Port 0 bit 4.**/
    SBIT(P0_5,          0x80,   5); /**< Port 0 bit 5.**/
    SBIT(P0_6,          0x80,   6); /**< Port 0 bit 6.**/
    SBIT(P0_7,          0x80,   7); /**< Port 0 bit 7.**/
    SBIT(P0_GTIMER,     0x80,   0); /**< GTIMER.**/
    SBIT(P0_RXD,        0x80,   1); /**< UART RX.**/
    SBIT(P0_TXD,        0x80,   2); /**< UART TX.**/
    SBIT(P0_INT0_N,     0x80,   3); /**< External interrupt 0.**/
    SBIT(P0_INT1_N,     0x80,   4); /**< External interrupt 1.**/
    SBIT(P0_T0,         0x80,   5); /**< Timer 0 input.**/
    SBIT(P0_T1,         0x80,   6); /**< Timer 1 input.**/
    SBIT(P0_PWM,        0x80,   7); /**< PWM.**/
    #define P_GTIMER            0
    #define RXD                 1
    #define TXD                 2
    #define INT0_N              3
    #define INT1_N              4
    #define T0                  5
    #define T1                  6
    #define PWM                 7

SFR(DPL0,       0x82);  /**< SFR: Data Pointer 0 Low byte.**/
SFR(DPH0,       0x83);  /**< SFR: Data Pointer 0 High byte.**/
SFR(DPL1,       0x84);  /**< SFR: Data Pointer 1 Low byte.**/
SFR(DPH1,       0x85);  /**< SFR: Data Pointer 1 High byte.**/
SFR(DPS,        0x86);  /**< SFR: Data Pointer Select.**/
    #define DPS_SEL             0

SFR(PCON,       0x87);  /**< SFR: Power Control.**/
    #define IDLE                0   /**< Idle mode.**/
    #define STOP                1   /**< Stop mode.**/
    #define GF0                 2   /**< General purpose flag 0.**/
    #define GF1                 3   /**< General purpose flag 1.**/
    #define SMOD                7   /**< Serial port baud rate doubler.**/

SFR(TCON,       0x88);  /**< SFR: Interrupt Flag.**/
    SBIT(IT0,           0x88,   0); /**< Ext. Int. 0 type (1=falling, 0=low).**/
    SBIT(IE0,           0x88,   1); /**< Ext. Int. 0 flag.**/
    SBIT(IT1,           0x88,   2); /**< Ext. Int 1 type (1=falling, 0=low).**/
    SBIT(IE1,           0x88,   3); /**< Ext. Int. 0 flag.**/
    SBIT(TR0,           0x88,   4); /**< Timer0 run control.**/
    SBIT(TF0,           0x88,   5); /**< Timer0 overflow flag.**/
    SBIT(TR1,           0x88,   6); /**< Timer1 run control.**/
    SBIT(TF1,           0x88,   7); /**< Timer1 overflow flag.**/
    #define IT0_b               0
    #define IE0_b               1
    #define IT1_b               2
    #define IE1_b               3
    #define TR0_b               4
    #define TF0_b               5
    #define TR1_b               6
    #define TF1_b               7

SFR(TMOD,       0x89);  /**< SFR: Timer Mode.**/
    #define M00                 0   /**< Timer0 [M0 - M1].**/
    #define M10                 1   /**<  0:13bit, 1:16bit, 2:8bitreload, 3:2x8bit.**/
    #define C_T0                2   /**< Timer0 Counter/Timer, 1 = clock by T0 pin.**/
    #define GATE0               3   /**< Timer0 Gate, 1 = clock when INT0_N is 1.**/
    #define M01                 4   /**< Timer1 [M0 - M1].**/
    #define M11                 5   /**<  0:13bit, 1:16bit, 2:8bitreload, 3:2x8bit.**/
    #define C_T1                6   /**< Timer1 Counter/Timer, 1 = clock by T1 pin.**/
    #define GATE1               7   /**< Timer1 Gate, 1 = clock when INT1_N is 1.**/

SFR(TL0,        0x8A);  /**< SFR: Timer0 Low byte.**/
SFR(TH0,        0x8C);  /**< SFR: Timer0 High byte.**/
SFR(TL1,        0x8B);  /**< SFR: Timer1 Low byte.**/
SFR(TH1,        0x8D);  /**< SFR: Timer1 High byte.**/

SFR(CKCON,      0x8E);  /**< SFR: Timer rate control.**/
    #define MD0                 0   /**< MD[0-2].**/
    #define MD1                 1   /**<  Number of cycles for external MOVX.**/
    #define MD2                 2   /**<  = 2 + MD[0-2].**/
    #define T0M                 3   /**< Timer0 clock. 0=cpu/12, 1=cpu/4.**/
    #define T1M                 4   /**< Timer1 clock. 0=cpu/12, 1=cpu/4.**/
    #define T2M                 5   /**< Timer2 clock. 0=cpu/12, 1=cpu/4.**/

SFR(SPC_FNC,    0x8F);  /**< SFR: Do not use.**/

SFR(P1,         0x90);  /**< SFR: Port 1.**/
    SBIT(P1_0,          0x90,   0); /**< Port 1 bit 0.**/
    SBIT(P1_1,          0x90,   1); /**< Port 1 bit 1.**/
    SBIT(P1_2,          0x90,   2); /**< Port 1 bit 2.**/
    SBIT(P1_3,          0x90,   3); /**< Port 1 bit 3.**/
    SBIT(P1_SPI_SCK,    0x90,   0); /**< SPI clock.**/
    SBIT(P1_MOSI,       0x90,   1); /**< SPI Master Out Slave In.**/
    SBIT(P1_MISO,       0x90,   2); /**< SPI Master In Slave Out.**/
    SBIT(P1_EECSN,      0x90,   3); /**< SPI Chip Select (active low).**/
    SBIT(P1_T2,         0x90,   0); /**< Timer 2 input.**/
    #define SPI_SCK             0
    #define SPI_MOSI            1
    #define SPI_MISO            2
    #define SPI_CSN             3
    #define T2                  0

SFR(EXIF,       0x91);  /**< SFR: Extended Interrupt Flags.**/
    #define IE2                 4   /**< ADC EOC (end of conversion) interrupt.**/
    #define IE3                 5   /**< SPI interrupt.**/
    #define IE4                 6   /**< DR (data ready) interrupt.**/
    #define IE5                 7   /**< AM (address match) interrupt.**/

SFR(MPAGE,      0x92);  /**< SFR: Do not use.**/

SFR(P0_DRV,     0x93);  /**< SFR: Drive strength of port 0.**/
SFR(P0_DIR,     0x94);  /**< SFR: Direction of Port 0 (1 = input).**/
SFR(P0_ALT,     0x95);  /**< SFR: Alternate functions of Port 0.**/
SFR(P1_DIR,     0x96);  /**< SFR: Direction of Port 1 (1 = input).**/
SFR(P1_ALT,     0x97);  /**< SFR: Alternate functions of Port 1.**/

SFR(SCON,       0x98);  /**< SFR: Serial port Control.**/
    SBIT(RI,            0x98,   0); /**< Receive Interrupt flag.**/
    SBIT(TI,            0x98,   1); /**< Transmit Interrupt flag.**/
    SBIT(RB8,           0x98,   2); /**< Ninth Receive Bit.**/
    SBIT(TB8,           0x98,   3); /**< Ninth Transmission Bit.**/
    SBIT(REN,           0x98,   4); /**< Receive Enable.**/
    SBIT(SM2,           0x98,   5); /**< Multiprocessor communication enable.**/
    SBIT(SM1,           0x98,   6); /**< Serial port operation mode.**/
    SBIT(SM0,           0x98,   7); /**< Serial port operation mode.**/
    #define RI_b                0
    #define TI_b                1
    #define RB8_b               2
    #define TB8_b               3
    #define REN_b               4
    #define SM2_b               5
    #define SM1_b               6
    #define SM0_b               7
SFR(SBUF,       0x99);  /**< SFR: Serial port data Buffer.**/

SFR(P2,         0xA0);  /**< SFR: Port 2 (radio).**/
    SBIT(P2_SBSCK,      0xA0,   0); /**< (write) SPI clock.**/
    SBIT(P2_SBMOSI,     0xA0,   1); /**< (write) SPI Master Out Slave In.**/
    SBIT(P2_RACSN,      0xA0,   3); /**< (write) SPI Chip Select (active low).**/
    SBIT(P2_TX_EN,      0xA0,   4); /**< (write) TX mode.**/
    SBIT(P2_TRX_CE,     0xA0,   5); /**< (write) Turn on transceiver.**/
    SBIT(P2_SBMISO,     0xA0,   2); /**< (read) SPI Master In Slave Out.**/
    SBIT(P2_EOC,        0xA0,   4); /**< (read) ADC End Of Conversion.**/
    SBIT(P2_DR,         0xA0,   5); /**< (read) Transceiver Data Ready.**/
    SBIT(P2_CD,         0xA0,   6); /**< (read) Transceiver Carrier Detect.**/
    SBIT(P2_AM,         0xA0,   7); /**< (read) Transceiver Address Match.**/
    #define SBSCK               0
    #define SBMOSI              1
    #define RACSN               3
    #define TX_EN               4
    #define TRX_CE              5
    #define SBMISO              2
    #define EOC                 4
    #define DR                  5
    #define CD                  6
    #define AM                  7

SFR(IE,         0xA8);  /**< SFR: Interrupt Enable.**/
    SBIT(EX0,           0xA8,   0); /**< Enable External interrupt 0.**/
    SBIT(ET0,           0xA8,   1); /**< Enable Timer0 interrupt.**/
    SBIT(EX1,           0xA8,   2); /**< Enable External interrupt 1.**/
    SBIT(ET1,           0xA8,   3); /**< Enable Timer1 interrupt.**/
    SBIT(ES,            0xA8,   4); /**< Enable Serial port interrupt.**/
    SBIT(ET2,           0xA8,   5); /**< Enable Timer2 interrupt.**/
    SBIT(EA,            0xA8,   7); /**< Global interrupt enable.**/
    #define EX0_b               0
    #define ET0_b               1
    #define EX1_b               2
    #define ET1_b               3
    #define ES_b                4
    #define ET2_b               5
    #define EA_b                7

SFR(PWMCON,     0xA9);  /**< SFR: PWM Control.**/
    #define PWM_PRESCALE        0   /**< PWM frequency prescale factor (6 bits).**/
    #define PWM_LENGTH          6   /**< Number of bits to use for period length.**/

SFR(PWMDUTY,    0xAA);  /**< SFR: PWM Duty cycle.**/

SFR(REGX_MSB,   0xAB);  /**< SFR: High byte of Watchdog/RTC register.**/
SFR(REGX_LSB,   0xAC);  /**< SFR: Low byte of Watchdog/RTC register.**/
SFR(REGX_CTRL,  0xAD);  /**< SFR: Control of REGX_MSB and REGX_LSB.**/
    #define WD_ADDRESS          0   /**< Address to access (3 bits).**/
    #define WD_WRITE            3   /**< 1 = write, 0 = read.**/
    #define WD_REG_BUSY         4   /**< 1 if Watchdog register is busy.**/

SFR(RSTREAS,    0xB1);  /**< SFR: Program Reset Address.**/
    #define RFLR0               0   /**< Reason For Last Reset (0= Power on).**/
    #define RFLR1               1   /**< Use IROM (0x8000) For Reset.**/

SFR(SPI_DATA,   0xB2);  /**< SFR: SPI data byte.**/
SFR(SPI_CTRL,   0xB3);  /**< SFR: SPI mux control.**/
    #define SPI_P1              0   /**< Connect SPI to P1.**/
    #define SPI_RADIO           1   /**< Connect SPI to radio.**/
SFR(SPICLK,     0xB4);  /**< SFR: SPI clock frequency.**/

SFR(TICK_DV,    0xB5);  /**< SFR: Divider for TICK clock.**/

SFR(CK_CTRL,    0xB6);  /**< SFR: Power-down mode control.**/

SFR(IP,         0xB8);  /**< SFR: Interrupt Priority.**/
    SBIT(PX0,           0xB8,   0); /**< External interrupt 0 priority.**/
    SBIT(PT0,           0xB8,   1); /**< Timer0 interrupt priority.**/
    SBIT(PX1,           0xB8,   2); /**< External interrupt 1 priority.**/
    SBIT(PT1,           0xB8,   3); /**< Timer1 interrupt priority.**/
    SBIT(PS,            0xB8,   4); /**< Serial port interrupt priority.**/
    SBIT(PT2,           0xB8,   5); /**< Timer2 interrupt priority.**/
    #define PX0_b               0
    #define PT0_b               1
    #define PX1_b               2
    #define PT1_b               3
    #define PS_b                4
    #define PT2_b               5

SFR(CKLFCON,    0xBF);  /**< SFR: Low-freq clock generation.**/
    #define UP_CLK_FREQ         0   /**< CPU clock frequency.**/
    #define XO_DIRECT           2   /**< CPU clock select.**/
    #define XOF                 3   /**< Crystal frequency.**/

SFR(T2CON,      0xC8);  /**< Timer2 control.**/
    SBIT(CP_RL2,        0xC8,   0); /**< Timer2 Capture/Reload flag.**/
    SBIT(C_T2,          0xC8,   1); /**< Timer2 Counter/Timer select.**/
    SBIT(TR2,           0xC8,   2); /**< Timer2 Run control.**/
    SBIT(EXEN2,         0xC8,   3); /**< Timer2 External Enable.**/
    SBIT(TCLK,          0xC8,   4); /**< Transmit Clock flag.**/
    SBIT(RCLK,          0xC8,   5); /**< Receive Clock flag.**/
    SBIT(EXF2,          0xC8,   6); /**< Timer2 External flag.**/
    SBIT(TF2,           0xC8,   7); /**< Timer2 overflow flag.**/
    #define CP_RL2_b            0
    #define C_T2_b              1
    #define TR2_b               2
    #define EXEN2_b             3
    #define TCLK_b              4
    #define RCLK_b              5
    #define EXF2_b              6
    #define TF2_b               7
SFR(RCAP2L,     0xCA);  /**< SFR: Timer2 capture or reload, low byte.**/
SFR(RCAP2H,     0xCB);  /**< SFR: Timer2 capture or reload, high byte.**/
SFR(TL2,        0xCC);  /**< SFR: Timer2 value, low byte.**/
SFR(TH2,        0xCD);  /**< SFR: Timer2 value, high byte.**/

SFR(PSW,        0xD0);  /**< SFR: Program Status Word.**/
    SBIT(P,             0xD0,   0); /**< Parity flag.**/
    SBIT(F1,            0xD0,   1); /**< User flag 1.**/
    SBIT(OV,            0xD0,   2); /**< Overflow flag.**/
    SBIT(RS0,           0xD0,   3); /**< Register Bank Select 0.**/
    SBIT(RS1,           0xD0,   4); /**< register Bank Select 1.**/
    SBIT(F0,            0xD0,   5); /**< User flag 0.**/
    SBIT(AC,            0xD0,   6); /**< Auxiliary Carry flag.**/
    SBIT(CY,            0xD0,   7); /**< Carry flag.**/
    #define P_b                 0
    #define F1_b                1
    #define OV_b                2
    #define RS0_b               3
    #define RS1_b               4
    #define F0_b                5
    #define AC_b                6
    #define CY_b                7

SFR(EICON,      0xD8);  /**< SFR: External interrupt control.**/
    SBIT(EICON_WDTI,    0xD8,   3); /**< GPIO and RTC wakeup interrupt flag.**/
    #define WDTI                3

SFR(ACC,        0xE0);  /**< SFR: Accumulator register.**/

SFR(EIE,        0xE8);  /**< SFR: Extended interrupt enable.**/
    SBIT(EX2,           0xE8,   0); /**< Enable ADC EOC interrupt.**/
    SBIT(EX3,           0xE8,   1); /**< Enable SPI ready interrupt.**/
    SBIT(EX4,           0xE8,   2); /**< Enable radio DR interrupt.**/
    SBIT(EX5,           0xE8,   3); /**< Enable raidio AM interrupt.**/
    SBIT(EWDI,          0xE8,   4); /**< Enable RTC wakeup interrupt.**/
    #define EX2_b               0
    #define EX3_b               1
    #define EX4_b               2
    #define EX5_b               3
    #define EWDI_b              4

SFR(B,          0xF0);  /**<SFR:  B register.**/

SFR(EIP,        0xF8);  /**< SFR: Extended interrupt priority.**/
    SBIT(PX2,           0xF8,   0); /**< ADC EOC interrupt priority.**/
    SBIT(PX3,           0xF8,   1); /**< SPI ready interrupt priority.**/
    SBIT(PX4,           0xF8,   2); /**< Radio DR interrupt priority.**/
    SBIT(PX5,           0xF8,   3); /**< Raidio AM interrupt priority.**/
    SBIT(PWDI,          0xF8,   4); /**< RTC wakeup interrupt priority.**/
    #define PX2_b               0
    #define PX3_b               1
    #define PX4_b               2
    #define PX5_b               3
    #define PWDI_b              4

SFR(HWREV,              0xFE);  /**< SFR: Do not use.**/


/*------------------------------------------------------------------------------
 * Interrupt Vectors
 */
#define IE0_int             0x03    /**< External interrupt 0.**/
#define TF0_int             0x0B    /**< Timer 0 Interrupt.**/
#define IE1_int             0x13    /**< External interrupt 1.**/
#define TF1_int             0x1B    /**< Timer 1 Interrupt.**/
#define RI_int              0x23    /**< Serial Port Interrupt.**/
#define TI_int              0x23    /**< Serial Port Interrupt.**/
#define TF2_int             0x2B    /**< Timer 2 Interrupt.**/
#define EXF2_int            0x2B    /**< Timer 2 Interrupt.**/
#define IE2_int             0x43    /**< ADC_EOC interrupt.**/
#define IE3_int             0x4B    /**< SPI_READY interrupt.**/
#define IE4_int             0x53    /**< Radio Data Ready interrupt.**/
#define IE5_int             0x5B    /**< Radio Address Match interrupt.**/
#define WDTI_int            0x63    /**< RTC wakeup timer interrupt.**/


#endif

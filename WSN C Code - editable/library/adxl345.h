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
 * Interface for Analog Devices ADXL345 accelerometer sensor when 
 * configured to I2C interface
 * 
 * @file adxl345.c
 * @date 03-March-2011
 * @author Javier Torres
 ******************************************************************************/


#ifndef ADXL345_H
#define ADXL345_H



/** I2C SCL (clock) pin. **/
#define ADXL345_INT1_PIN     4

/** I2C SDA (data) pin. **/
#define ADXL345_INT2_PIN     7

/** 7 bits I2C address if ALT_ADDRESS pin is tied to Vddi/o,
* 	change to 0x53 if ALT_ADDRESS is tied to GND. 
**/
#define	ADXL345_I2C_ADDRESS			0x1D 

/** Pins used to communicated with sensor. **/
#if defined (UC_AVR)
#   define ADXL345_CLOCK_PORT       PORTD
#   define ADXL345_CLOCK_DDR        DDRD
#   define ADXL345_CLOCK_PIN        PIND
#   define ADXL345_CLOCK            0

#   define ADXL345_DATA_PORT        PORTD
#   define ADXL345_DATA_DDR         DDRD
#   define ADXL345_DATA_PIN         PIND
#   define ADXL345_DATA             1
#else
#   define ADXL345_CLOCK_PORT       P0
#   define ADXL345_CLOCK_DDR        P0_DIR
#   define ADXL345_CLOCK_PIN        P0
#   define ADXL345_CLOCK            3

#   define ADXL345_DATA_PORT        P0
#   define ADXL345_DATA_DDR         P0_DIR
#   define ADXL345_DATA_PIN         P0
#   define ADXL345_DATA             5
#endif

/** ADXL345 Register Map, Device ID. **/

#define REG_ADDR_ADXL345_DEVID			0

/** ADXL345 Register Map, Offsets. **/

#define REG_ADDR_ADXL345_OFSX			30
#define REG_ADDR_ADXL345_OFSY			31
#define REG_ADDR_ADXL345_OFSZ			32

/** ADXL345 Register Map, Tap Detection. **/
		
#define REG_ADDR_ADXL345_THERSH_TAP		29		
#define REG_ADDR_ADXL345_TAP_DUR		33
#define REG_ADDR_ADXL345_TAP_LATENT		34
#define REG_ADDR_ADXL345_TAP_WINDOW		35
#define REG_ADDR_ADXL345_TAP_AXES		42
#define REG_ADDR_ADXL345_ACT_TAP_STATUS	43

/** ADXL345 Register Map, Activity Detection. **/

#define REG_ADDR_ADXL345_THERSH_ACT		36

/** ADXL345 Register Map, Inactivity Detection. **/

#define REG_ADDR_ADXL345_THERSH_INACT	37
#define REG_ADDR_ADXL345_TIME_INACT		38
#define REG_ADDR_ADXL345_ACT_INACT_CTL	39

/** ADXL345 Register Map, Free Fall Detection. **/

#define REG_ADDR_ADXL345_THERSH_FF		40
#define REG_ADDR_ADXL345_TIME_FF		41

/** ADXL345 Register Map, Data Rate and Power Mode Control. **/

#define REG_ADDR_ADXL345_BW_RATE		44
#define REG_ADDR_ADXL345_POWER_CTL		45

/** ADXL345 Register Map, Interrupts Control. **/

#define REG_ADDR_ADXL345_INT_ENABLE		46
#define REG_ADDR_ADXL345_INT_MAP		47
#define REG_ADDR_ADXL345_INT_SOURCE		48

/** ADXL345 Register Map, Data Format Control and data registers. **/

#define REG_ADDR_ADXL345_DATA_FORMAT	49

#define REG_ADDR_ADXL345_DATA_X0		50
#define REG_ADDR_ADXL345_DATA_X1		51
#define REG_ADDR_ADXL345_DATA_Y0		52
#define REG_ADDR_ADXL345_DATA_Y1		53
#define REG_ADDR_ADXL345_DATA_Z0		54
#define REG_ADDR_ADXL345_DATA_Z1		55

/** ADXL345 Register Map, FIFO Control. **/

#define REG_ADDR_ADXL345_FIFO_CTL		56
#define REG_ADDR_ADXL345_FIFO_STATUS	57


/** ADXL345 Data Format Settings. **/

#define  ACTIVE_LOW		BIT(5)
#define  FULL_RESOL		BIT(3)
#define  RANGE_2g		0x00
#define  RANGE_4g		BIT(0)
#define  RANGE_8g		BIT(1)
#define  RANGE_16g		BIT(1)|BIT(0)
#define  RANGE2g_INtActLOW_FULLRES BIT(3)|BIT(5)
#define  RANGE2g_INtActHIGH_FULLRES BIT(3)

/** ADXL345 Activity Control Settings. **/

#define  ACT_AC_COMP	BIT(7)
#define  INACT_AC_COMP	BIT(3)

#define  ZAxIS_DETEC	BIT(0)|BIT(4)
#define  YAxIS_DETEC	BIT(1)|BIT(5)
#define  XAxIS_DETEC	BIT(2)|BIT(6)

#define AC_ZAXIS_DETEC	BIT(0)|BIT(4)|BIT(7)|BIT(3)
#define AC_3AXIS_DETEC	0xF0

/** ADXL345 POWER Control Settings. **/

#define  ADXL345_STANDBY_MODE	0x04
#define  ADXL345_SLEEP_MODE		0x0C
#define  ADXL345_MEASURE_MODE	0x08

void Init_ADXL345_pins(void);

void Conf_ADXL345_ActDetec(uint8_t *DataFormat,uint8_t *Threshold_act,uint8_t *Time_inact,uint8_t *act_ctrl);


/**
 * Starts an ADC conversion and records the result. Note that this function
 * blocks while waiting for the result, which could take 210ms.
 *
 * @param cmd should be either @c SHT_TEMPERATURE or @c SHT_HUMIDITY.
 * @param value where to store the reading.
 *
 * @return @c STATUS_OK or error value.
 **/
status_t ADXL345_read(uint8_t cmd, uint16_t* value);



#endif
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
 * Functions for communicating with ADXL345 sensor when configured to I2C interface.
 *
 * @file adxl345.c
 * @date 03-March-2011
 * @author Javier Torres
 ******************************************************************************/


#include "global.h"
#include "adxl345.h"
#include "delay.h"
#include "i2c.h"
#if defined (UC_AVR)
#   include "avr/i2c_sw_avr.h"
#elif defined (UC_8051)
#   include "nrf/i2c_sw_nrf.h"
#endif

//FORCE_INLINE static inline void Init_ADXL345_pins(void);



 void Conf_ADXL345_ActDetec(uint8_t *DataFormat,uint8_t *Threshold_act,uint8_t *Time_inact,uint8_t *act_ctrl)
{
	uint8_t RegAddress[4];
	uint8_t temp[4];
	uint8_t tempda[4];
	status_t ack;
	ack=STATUS_NO_ACK;
	

			/* Configure device in standby mode*/
		*RegAddress=REG_ADDR_ADXL345_POWER_CTL;
		*temp=ADXL345_STANDBY_MODE;
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	ack=i2c_write(ADXL345_I2C_ADDRESS, temp, 1,I2C_OPTION_STOP);
		//delay_ms(20);
	
	

	
		*RegAddress=REG_ADDR_ADXL345_INT_ENABLE;
		*temp=0x00;
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, temp, 1,I2C_OPTION_STOP);

	//Init_ADXL345_pins();
			/* Disable Act/Inact interrupts for motion detection */

	

			//delay_ms(20);
		/* Configure device in standby mode*/
		*RegAddress=REG_ADDR_ADXL345_POWER_CTL;
		*temp=ADXL345_STANDBY_MODE;
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, temp, 1,I2C_OPTION_STOP);
	


	/* Configure the range */
	
	*RegAddress=REG_ADDR_ADXL345_DATA_FORMAT;
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, DataFormat, 1,I2C_OPTION_STOP);
	/* Configure the Inactivity threshold */
	/*	*RegAddress=REG_ADDR_ADXL345_THERSH_INACT;
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, Threshold_act, 1,I2C_OPTION_STOP);*/
	
	/* Configure the Time Inactive threshold */
	/*	*RegAddress=REG_ADDR_ADXL345_TIME_INACT;
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, Time_inact, 1,I2C_OPTION_STOP);*/
	
	/* Configure the Activity threshold */

		*RegAddress=REG_ADDR_ADXL345_THERSH_ACT;
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, Threshold_act, 1,I2C_OPTION_STOP);
	
	/* Configure Axe for motion detection */
		*RegAddress=REG_ADDR_ADXL345_ACT_INACT_CTL;
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, act_ctrl, 1,I2C_OPTION_STOP);
			/* Map Act/Inact interrupts lines for motion detection */
		*RegAddress=REG_ADDR_ADXL345_INT_MAP;
		*temp=BIT(3);
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, temp, 1,I2C_OPTION_STOP);
	
	/* Enable Act/Inact interrupts for motion detection */
		*RegAddress=REG_ADDR_ADXL345_INT_ENABLE;
		*temp=BIT(4);
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, temp, 1,I2C_OPTION_STOP);
			  *RegAddress=REG_ADDR_ADXL345_INT_SOURCE;
					i2c_write(ADXL345_I2C_ADDRESS,RegAddress , 1,I2C_OPTION_NON_STOP);
	
			i2c_read(ADXL345_I2C_ADDRESS, tempda, 1, I2C_OPTION_FULL);
	
			/* Configure device in standby mode*/
		*RegAddress=REG_ADDR_ADXL345_POWER_CTL;
		*temp=ADXL345_SLEEP_MODE;
	i2c_write(ADXL345_I2C_ADDRESS, RegAddress, 1,I2C_OPTION_NON_STOP);
	i2c_write(ADXL345_I2C_ADDRESS, temp, 1,I2C_OPTION_STOP);
	

}



/******************************************************************************\
 * See adxl345.h for documentation of this function.
\******************************************************************************/

status_t ADXL345_read(uint8_t cmd, uint16_t* value)
{


    return STATUS_OK;
}




/******************************************************************************\
 * Functions used only within this file.
\******************************************************************************/

 void Init_ADXL345_pins(void)
{

//i2cSw_initPins();
						/* Configure INT1 and INT2 */
     /* Set pin values to 0, direction as input, and enable high current sink */
    P0 &= ~BIT(ADXL345_INT1_PIN) & ~BIT(ADXL345_INT1_PIN);
    P0_DIR |= BIT(ADXL345_INT1_PIN) | BIT(ADXL345_INT1_PIN);
    P0_DRV |= BIT(ADXL345_INT1_PIN) | BIT(ADXL345_INT1_PIN);
	    P0 &= ~BIT(ADXL345_INT2_PIN) & ~BIT(ADXL345_INT2_PIN);
    P0_DIR |= BIT(ADXL345_INT2_PIN) | BIT(ADXL345_INT2_PIN);
    P0_DRV |= BIT(ADXL345_INT2_PIN) | BIT(ADXL345_INT2_PIN);
}





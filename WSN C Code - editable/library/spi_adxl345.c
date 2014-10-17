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
 * Function for reading from adxl345 accelerometer. Look at the datasheet for more.
 *
 *
 * @todo: 
 *
 * @file spi_adxl345.c
 * @date 11-Oct-2010
 * @author Michael Walsh
 ******************************************************************************/
  #include "global.h"
#include "spi_adxl345.h" 
#include "delay.h"
void adxl345_spi_init(void)
{

	uc_sw_cs_LO;
	uc_sw_cs_OUT;
 
	uc_sw_SCLK_HI;
	uc_sw_SCLK_OUT;
	
    uc_sw_SDI_LO;
    uc_sw_SDI_OUT;
 
    uc_sw_SDO_LO; 
    uc_sw_SDO_IN;
}

char getDOUT(void)
{
	if (PING & (1<<2)) return 1; 	// read PA7
	else return 0; 
}

void adxl345_spi_write(uint8_t address, uint8_t data)
{
	uint8_t data_in = 0;
	uint8_t i,temp_set,temp_clr;		
	
	temp_set = 1;		// 10
	temp_clr = 1;		// 10
	// want to change this variable, please use oscilloscope
	
	uc_sw_SCLK_HI;
	uc_sw_cs_LO;		// enable /CS_ADC
	
	//for(j=0;j<temp_clr;j++) delay_us(1);		//=12us
			
	// Send command
	
	for (i=8;i>0;i--)
	{
		uc_sw_SCLK_LO; 
		data_in = address >> (i-1);
		if(data_in & 0x01) uc_sw_SDI_HI;
		else uc_sw_SDI_LO;
		//for(j=0;j<temp_clr;j++) delay_us(1);
		uc_sw_SCLK_HI; //for(j=0;j<temp_set;j++) delay_us(1);
	}
	
	
	// send data
	for (i=8;i>0;i--)
	{
		uc_sw_SCLK_LO; 
		data_in = data >> (i-1);
		if(data_in & 0x01) uc_sw_SDI_HI;
		else uc_sw_SDI_LO;
		//for(j=0;j<temp_clr;j++) delay_us(1);
		uc_sw_SCLK_HI; //for(j=0;j<temp_set;j++) delay_us(1);
	}
	
	
	// end of transmission
	
	uc_sw_cs_HI;		// disable /CS_ADC
}
	
uint8_t adxl345_spi_read(uint8_t data)
{
	uint8_t data_out = 0;
	uint8_t data_in = 0;
	uint8_t i,temp_set,temp_clr;		
	
	temp_set = 1;		// 10
	temp_clr = 1;		// 10

	// want to change this variable, please use oscilloscope
	
	uc_sw_SCLK_HI;
	uc_sw_cs_LO;		// enable /uc_sw_cs
	
//	for(j=0;j<temp_clr;j++) ;
	
	// Send command
	for (i=8;i>0;i--)
	{
		uc_sw_SCLK_LO; 
		data_in = data >> (i-1);
		if(i == 8) data_in = 0x01; // read byte
		if(data_in & 0x01) uc_sw_SDI_HI;
		else uc_sw_SDI_LO;
		//for(j=0;j<temp_clr;j++) delay_us(1);
		uc_sw_SCLK_HI; //for(j=0;j<temp_set;j++) delay_us(1);
	}
	
	// Read data
	for (i=0;i<8;i++)
	{
		uc_sw_SCLK_LO; //for(j=0;j<temp_clr;j++) delay_us(1);
		data_out |= getDOUT();
		if(i != 7) data_out <<= 1;  // do not shift data if this is the late bit
		uc_sw_SCLK_HI; //for(j=0;j<temp_set;j++) delay_us(1);

	} 
	// end of transmission
	
	uc_sw_cs_HI;		// disable /uc_sw_cs
	return data_out;
}
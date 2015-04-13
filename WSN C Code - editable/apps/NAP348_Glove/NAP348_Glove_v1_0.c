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
 * This application reads from the microcontroller's ADC. It prints the value to
 * the UART and also transmits it using the radio. Everytime a packet is sent an
 * LED is toggled.
 *
 * @file adcToRf.c
 * @date 17-Jan-2010
 * @author Seán Harte
 ******************************************************************************/

#include "global.h"
#include "sleep.h"
#include "adc.h"
#include "delay.h"
#include "led.h"
#include "rf.h"
#include "uart.h"
#include "simpleIo.h"
//#include "externInt.h"

#include "spi_adxl345.c"

/* Address to send to (see rfToUart application makefile) */
#define DEST_ADDR       0x8480

/** ADC channel to read **/
#define ADC_CHANNEL     ADC_CH_ADC0

/*
 * This variable signals when the timer delay is over. It is declared "volatile"
 * because it is modified in a callback function
 */
#define sensor  (PORTA,0x04)
#define sensor_on  PORTA |= (1<<4)
#define sensor_off  PORTA &= ~(1<<4)
#define sensorIO (DDRA,4)		//data direction
#define sensor_OUT  DDRA |= (1<<4)



#define uc_sw_MUX_A0  (PORTC,0x07)
#define uc_sw_MUX_A0_HI  PORTC |= (1<<7)
#define uc_sw_MUX_A0_LO  PORTC &= ~(1<<7)
#define uc_sw_MUX_A0KIO (DDRC,7)		//data direction
#define uc_sw_MUX_A0_OUT  DDRC |= (1<<7)
#define uc_sw_MUX_A0_IN  DDRC &= ~(1<<7)

#define uc_sw_MUX_A1  (PORTC,0x06)
#define uc_sw_MUX_A1_HI  PORTC |= (1<<6)
#define uc_sw_MUX_A1_LO  PORTC &= ~(1<<6)
#define uc_sw_MUX_A1KIO (DDRC,6)		//data direction
#define uc_sw_MUX_A1_OUT  DDRC |= (1<<6)
#define uc_sw_MUX_A1_IN  DDRC &= ~(1<<6)

#define uc_sw_MUX_A2  (PORTC,0x05)
#define uc_sw_MUX_A2_HI  PORTC |= (1<<5)
#define uc_sw_MUX_A2_LO  PORTC &= ~(1<<5)
#define uc_sw_MUX_A2KIO (DDRC,5)		//data direction
#define uc_sw_MUX_A2_OUT  DDRC |= (1<<5)
#define uc_sw_MUX_A2_IN  DDRC &= ~(1<<5)

#define uc_sw_MUX_A3  (PORTC,0x04)
#define uc_sw_MUX_A3_HI  PORTC |= (1<<4)
#define uc_sw_MUX_A3_LO  PORTC &= ~(1<<4)
#define uc_sw_MUX_A3KIO (DDRC,4)		//data direction
#define uc_sw_MUX_A3_OUT  DDRC |= (1<<4)
#define uc_sw_MUX_A3_IN  DDRC &= ~(1<<4)

#define uc_sw_MUX_ACC_EN  (PORTC,0x03)
#define uc_sw_MUX_ACC_EN_HI  PORTC |= (1<<3)
#define uc_sw_MUX_ACC_EN_LO  PORTC &= ~(1<<3)
#define uc_sw_MUX_ACC_ENKIO (DDRC,3)		//data direction
#define uc_sw_MUX_ACC_EN_OUT  DDRC |= (1<<3)
#define uc_sw_MUX_ACC_EN_IN  DDRC &= ~(1<<3)

#define uc_sw_MUX_BEND_EN  (PORTC,0x02)
#define uc_sw_MUX_BEND_EN_HI  PORTC |= (1<<2)
#define uc_sw_MUX_BEND_EN_LO  PORTC &= ~(1<<2)
#define uc_sw_MUX_BEND_ENKIO (DDRC,2)		//data direction
#define uc_sw_MUX_BEND_EN_OUT  DDRC |= (1<<2)
#define uc_sw_MUX_BEND_EN_IN  DDRC &= ~(1<<2)


#define uc_sw_MUX_WR  (PORTC,0x00)
#define uc_sw_MUX_MUX_WR_HI  PORTC |= (1<<0)
#define uc_sw_MUX_MUX_WR_LO  PORTC &= ~(1<<0)
#define uc_sw_MUX_MUX_WRKIO (DDRC,0)		//data direction
#define uc_sw_MUX_MUX_WR_OUT  DDRC |= (1<<0)
#define uc_sw_MUX_MUX_WR_IN  DDRC &= ~(1<<0)

#define uc_sw_MUX_cSA_CSB  (PORTC,0x01)
#define uc_sw_MUX_MUX_cSA_CSB_HI  PORTC |= (1<<1)
#define uc_sw_MUX_MUX_cSA_CSB_LO  PORTC &= ~(1<<1)
#define uc_sw_MUX_MUX_cSA_CSBKIO (DDRC,1)		//data direction
#define uc_sw_MUX_MUX_cSA_CSB_OUT  DDRC |= (1<<1)
#define uc_sw_MUX_MUX_cSA_CSB_IN  DDRC &= ~(1<<1)

#define DEBUGGING_ON  1

static volatile rf_msgType receivedMsg;

static uint8_t txBuffer[RF_MAX_PAYLOAD_SIZE];
//static uint8_t i,j;
static unsigned char i,j;
static double tempresult;
  uint8_t Bdata[25];
  uint16_t adcValue;
  
  //---------*********TIMER**********-----------
  
// these values are changed in an ISR, so they must be declared as volatile
volatile uint8_t timer0_ticks;
volatile uint16_t overflowCount;
  

/********************************************************************************
Interrupt Routines
********************************************************************************/

// timer1 overflow
ISR(TIMER1_OVF_vect) {
   // printf("***** Timer1 Overflow *****\n");
}
// timer0 overflow
ISR(TIMER0_OVF_vect) {
    // 61 ticks = 2 seconds @ 8.0MHz
    timer0_ticks++;
	//printf("Its been %u ticks.\n",timer0_ticks);
    if(timer0_ticks < 10){
        //fake it 
    }
    if(timer0_ticks==61){
        timer0_ticks = 0;
        // till you make it, now we increment seconds twice
		if(overflowCount >= 100){
			overflowCount = 0;
		}else{
			overflowCount++;
		}
       // printf("Its been %u seconds.\n",seconds);
    }
}



  //---------*********TIMER**********-----------

int main(void)
{
 // uint8_t i;

    /* Initalise everything */
    board_init();

	//Ensure adc port is input
	PORTF = 0x00; 
    DDRF  = 0x00;
	
	//Turn off sensors and disable mux 
	sensor_OUT;
    sensor_off;
	
	  //---------*********TIMER**********-----------
    // enable timer overflow interrupt for both Timer0 and Timer1
    TIMSK=(1<<TOIE0) | (1<<TOIE1);

    // set timer0 counter initial value to 0
    TCNT0=0x00;
    // start timer0 with /1024 prescaler
    TCCR0 = (1<<CS02) | (1<<CS00);
    // lets turn on 16 bit timer1 also
    TCCR1B |= (1 << CS10) | (1 << CS12);

    // enable interrupts
    sei(); 
	
	  //---------*********TIMER**********-----------

	
	
	uc_sw_MUX_BEND_EN_OUT;
	uc_sw_MUX_BEND_EN_HI;//Interface Disabled
	uc_sw_MUX_ACC_EN_OUT;
	uc_sw_MUX_ACC_EN_HI;// Interface Disabled
	
	uc_sw_MUX_MUX_cSA_CSB_OUT;
	uc_sw_MUX_MUX_cSA_CSB_LO;
	
	uc_sw_MUX_MUX_WR_OUT;
	uc_sw_MUX_MUX_WR_LO;
	
	adxl345_spi_init();
	uc_sw_MUX_A0_OUT;
	uc_sw_MUX_A0_LO;
	uc_sw_MUX_A1_OUT;
	uc_sw_MUX_A1_LO;
	uc_sw_MUX_A2_OUT;
	uc_sw_MUX_A2_LO;
	uc_sw_MUX_A3_OUT;
	uc_sw_MUX_A3_LO;

	
	
	PORTF = 0x00; 
    DDRF  = 0x00;
	rf_init(RF_CHANNEL_CENTRE, RF_PWR_MAX);
	rf_setReceiveBuffer(&receivedMsg);
	//rf_setMode(RF_MODE_RECEIVING);
	sensor_on;
	uart_init();


	uc_sw_MUX_ACC_EN_LO;
	delay_us(500);
	
for(j=0;j<16;j++)
{


	if(j&0x01)uc_sw_MUX_A0_HI;
	else uc_sw_MUX_A0_LO;
	if(j&0x02)uc_sw_MUX_A1_HI;
	else uc_sw_MUX_A1_LO;
	if(j&0x04)uc_sw_MUX_A2_HI;
	else uc_sw_MUX_A2_LO;
	if(j&0x08)uc_sw_MUX_A3_HI;
	else uc_sw_MUX_A3_LO;
	delay_us(100);
	adxl345_spi_write(0x31,0x08);
	adxl345_spi_write(0x31,0x08);	
	
	adxl345_spi_write(0x2D,0x08);		// start measurement
	adxl345_spi_write(0x31,0x08);
	}
    for (;;) {
	
		uc_sw_MUX_BEND_EN_HI;//Interface Disabled
		uc_sw_MUX_ACC_EN_HI;//Disconnect interface
	
		
	//read accelerometers s
	printf("%u.%.2u,",overflowCount,timer0_ticks);
		for(j=0;j<16;j++)
		{
			if(j&0x01)uc_sw_MUX_A0_HI;
			else uc_sw_MUX_A0_LO;
			if(j&0x02)uc_sw_MUX_A1_HI;
			else uc_sw_MUX_A1_LO;
			if(j&0x04)uc_sw_MUX_A2_HI;
			else uc_sw_MUX_A2_LO;
			if(j&0x08)uc_sw_MUX_A3_HI;
			else uc_sw_MUX_A3_LO;
			delay_us(2);
			uc_sw_MUX_ACC_EN_LO;//Connect interface
			delay_us(2);
			
			
		txBuffer[0+6*j+3] = adxl345_spi_read(0xB2);
		txBuffer[1+6*j+3] = adxl345_spi_read(0xB3);	
        txBuffer[2+6*j+3] = adxl345_spi_read(0xB4);
        txBuffer[3+6*j+3] = adxl345_spi_read(0xB5);
        txBuffer[4+6*j+3] = adxl345_spi_read(0xB6);
        txBuffer[5+6*j+3] = adxl345_spi_read(0xB7);
		uc_sw_MUX_ACC_EN_HI;//Disconnect interface
		if(DEBUGGING_ON)
		{
				for (i=0;i<3;i++)
		{
		
		if(0x80&txBuffer[1+2*i+6*j+3])tempresult=((((txBuffer[0+2*i+6*j+3])^0xFF)+1)+256*((((txBuffer[2*i+1+6*j+3])^0xFF))))*(-1.0/256);
		else tempresult=(txBuffer[2*i+6*j+3]+256*(txBuffer[2*i+1+6*j+3]))*(1.0/256);
		snprintf(&Bdata[0],9,"%f",tempresult);
			//putchar(' ');
			//putchar('A');
			//putchar('C');
			//putchar('C');
			//putHex_JT(j);
			//putchar(' ');
			//putchar(' ');
			putchar(Bdata[0]);
			putchar(Bdata[1]);
			putchar(Bdata[2]);
			putchar(Bdata[3]);
			putchar(Bdata[4]);	
			putchar(Bdata[5]);
			putchar(Bdata[6]);
			putchar(Bdata[7]);
			putchar(Bdata[8]);
			putchar(',');


					
		}
		}
		}
				if(DEBUGGING_ON)
		{
		}
		rf_send(DEST_ADDR, txBuffer, 99);
		//delay_ms(7);
		//rf_send(DEST_ADDR, txBuffer, 99);
		
	
			
		for(j=0;j<16;j++)
		{
			if(j&0x01)uc_sw_MUX_A0_HI;
			else uc_sw_MUX_A0_LO;
			if(j&0x02)uc_sw_MUX_A1_HI;
			else uc_sw_MUX_A1_LO;
			if(j&0x04)uc_sw_MUX_A2_HI;
			else uc_sw_MUX_A2_LO;
			if(j&0x08)uc_sw_MUX_A3_HI;
			else uc_sw_MUX_A3_LO;
			delay_us(10);
			uc_sw_MUX_BEND_EN_LO;//Interface Enabled
			delay_us(90);
			delay_us(250);
			delay_us(250);
			
			adc_init(2, 10, ADC_REF_VCC);
			
			adcValue=adc_singleConversion();
			//adcValue=adc_singleConversion();
			txBuffer[0+6*j+3] = HIGH_BYTE(adcValue);
			txBuffer[1+6*j+3]= LOW_BYTE(adcValue);
			
			
			adc_init(1, 10,ADC_REF_VCC);
			//delay_us(10);
			adcValue=adc_singleConversion();
			//adcValue=adc_singleConversion();
			txBuffer[2+6*j+3] = HIGH_BYTE(adcValue);
			txBuffer[3+6*j+3] = LOW_BYTE(adcValue);
			
			adc_init(3, 10,ADC_REF_VCC);
			//adc_init(3, 10,ADC_REF_INTERNAL);
			//delay_us(100);
			adcValue=adc_singleConversion();

			//adcValue=adc_singleConversion();
			txBuffer[4+6*j+3] = HIGH_BYTE(adcValue);
			txBuffer[5+6*j+3]  = LOW_BYTE(adcValue);

			
		uc_sw_MUX_BEND_EN_HI;//Disconnect interface
		if(DEBUGGING_ON)
		{
			for (i=0;i<1;i++)
		{
		

		tempresult=(txBuffer[2*i+1+6*j+3]+256*((txBuffer[2*i+6*j+3])&0x03))*(3.3/1024);
		snprintf(&Bdata[0],9,"%f",tempresult);
			
			
			
			
			if(i==0 && j < 5)
			{
				
				putchar(Bdata[0]);
				putchar(Bdata[1]);
				putchar(Bdata[2]);
				putchar(Bdata[3]);
				putchar(Bdata[4]);
				putchar(Bdata[5]);
				putchar(Bdata[6]);
				putchar(Bdata[7]);
				putchar(Bdata[8]);
				putchar(',');
				
			
			} else if(i==0 && j == 5){
			
				putchar(Bdata[0]);
				putchar(Bdata[1]);
				putchar(Bdata[2]);
				putchar(Bdata[3]);
				putchar(Bdata[4]);
				putchar(Bdata[5]);
				putchar(Bdata[6]);
				putchar(Bdata[7]);
				putchar(Bdata[8]);
				putchar('*');
				
			}
					
		}
		
		}
			
			}
			//		if(DEBUGGING_ON)
		//{
			

		//}
		//		rf_send(DEST_ADDR, txBuffer, 99);
			//delay_ms(7);
			
			}
}

/*------------------------------------------------------------------------------
 * Ignore any received packets.
 */
void rf_callback(volatile rf_msgType* msg)
{

	UNUSED(msg);
 
				//adc_init(1, 10, ADC_REF_INTERNAL);

		
}

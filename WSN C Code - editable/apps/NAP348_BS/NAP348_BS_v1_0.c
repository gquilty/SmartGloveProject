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
 * This application writes any data it receives over to radio to the UART. In
 * between receiving packets, it enters sleep mode. LED_0 is flashed everytime a
 * packet is received.
 *
 * @file rfToUart.c
 * @date 17-Jan-2010
 * @author Seán Harte
 ******************************************************************************/

#include "global.h"
#include "rf.h"
#include "led.h"
#include "uart.h"
#include "sleep.h"
#include "simpleIo.h"
#include "delay.h"

#define DEST_ADDR       0x0100

static volatile bool isReceived = false;
static volatile rf_msgType receivedMsg;
static volatile uint8_t buffer[RF_MAX_PAYLOAD_SIZE];

//static volatile uint8_t txBuffer[RF_MAX_PAYLOAD_SIZE];

uint8_t Bdata[25];
static uint8_t i,j;
static double tempresult;
int main(void)
{
    /* Initialise everything */
    board_init();
    //led_init();
    uart_init();
    rf_init(RF_CHANNEL_CENTRE, RF_PWR_MAX);
    receivedMsg.data = buffer;
   // printf("\nrfToUart\n");

    /* Turn radio on to RX mode */
    rf_setReceiveBuffer(&receivedMsg);
    rf_setMode(RF_MODE_RECEIVING);

    /* Go to sleep now, and after each packet is received */
    for(;;) {

        while (!isReceived) {
            delay_us(250);
        }
        isReceived = false;
		

		
		if((receivedMsg.data[0] =='A')&&(receivedMsg.data[1] =='C'))
		{
		
    
		for(j=0;j<16;j++)
		{
				for (i=0;i<3;i++)
		{
		
		if(0x80&receivedMsg.data[1+2*i+6*j+3])tempresult=((((receivedMsg.data[0+2*i+6*j+3])^0xFF)+1)+256*((((receivedMsg.data[2*i+1+6*j+3])^0xFF))))*(-1.0/256);
		else tempresult=(receivedMsg.data[2*i+6*j+3]+256*(receivedMsg.data[2*i+1+6*j+3]))*(1.0/256);
		snprintf(&Bdata[0],9,"%f",tempresult);
	
			//putchar(' ');
			putchar('A');
			putchar('C');
			putchar('C');
			putHex_JT(j);
			putchar(' ');
			if(i==0)putchar('X');
			else if (i==1)putchar('Y');
			else putchar('Z');
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
					
		}
		}
					putchar(' ');
			putchar('E');
			putchar('N');
			putchar('D');
			putchar('O');
			putchar('F');
			putchar('D');
			putchar('A');
			putchar('T');
			putchar('A');

}else{
for(j=0;j<16;j++)
		{
			for (i=0;i<3;i++)
		{
		
		tempresult=(receivedMsg.data[2*i+1+6*j+3]+256*((receivedMsg.data[2*i+6*j+3])&0x03))*(3.3/1024);
		
		snprintf(&Bdata[0],9,"%f",tempresult);

			
			if(j<4)
			{
			
				if(i==0)
				{
				putchar('B');
				putchar('E');
				putchar('N');
				putchar('D');
				putHex_JT(j);
				putchar(' ');
				//putchar(' ');
			
				}else if (i==1)
			
				{
				putchar('S');
				putchar('P');
				putchar('L');
				putchar('A');
				putHex_JT(j);
				putchar(' ');
				//putchar(' ');
						
			
				} else 
				{
		
				putchar('B');
				putchar('I');
				putchar('A');
				putchar('S');
				putHex_JT(j);
				putchar(' ');
				//putchar(' ');
		
					}
			
			}else{
							if(i==0)
				{
				putchar('B');
				putchar('E');
				putchar('N');
				putchar('D');
				putHex_JT(j);
				putchar(' ');
				//putchar(' ');
		
				}else if (i==1)
			
				{
				putchar('F');
				putchar('O');
				putchar('R');
				putchar('C');
				putHex_JT((j-4));
				putchar(' ');
				//putchar(' ');
				
			
				} else 
				{
		
				putchar('B');
				putchar('I');
				putchar('A');
				putchar('S');
				putHex_JT(j);
				putchar(' ');
				//putchar(' ');
			
					}
			
			}
			putchar(Bdata[0]);
			putchar(Bdata[1]);
			putchar(Bdata[2]);
			putchar(Bdata[3]);
			putchar(Bdata[4]);
			putchar(Bdata[5]);
			putchar(Bdata[6]);
			putchar(Bdata[7]);
			putchar(Bdata[8]);

		//	putchar(0x0A);
		//	putchar(0x0D);
					
		}
			
			
			}
			putchar(' ');
			putchar('E');
			putchar('N');
			putchar('D');
			putchar('O');
			putchar('F');
			putchar('D');
			putchar('A');
			putchar('T');
			putchar('A');

}
/*
  	if(receivedMsg.data[0] =='A'){
		for(j=0;j<16;j++)
		{
				for (i=0;i<3;i++)
		{
		
		if(0x80&receivedMsg.data[1+2*i+6*j+3])tempresult=((((receivedMsg.data[0+2*i+6*j+3])^0xFF)+1)+256*((((receivedMsg.data[2*i+1+6*j+3])^0xFF))))*(-1.0/256);
		else tempresult=(receivedMsg.data[2*i+6*j+3]+256*(receivedMsg.data[2*i+1+6*j+3]))*(1.0/256);
		snprintf(&Bdata[0],9,"%f",tempresult);
	
			//putchar(' ');
			putchar('A');
			putchar('C');
			putchar('C');
			putHex_JT(j);
			putchar(' ');
			if(i==0)putchar('X');
			else if (i==1)putchar('Y');
			else putchar('Z');
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
					
		}
		}
					putchar(' ');
			putchar('E');
			putchar('N');
			putchar('D');
			putchar('O');
			putchar('F');
			putchar('D');
			putchar('A');
			putchar('T');
			putchar('A');
		
		}else{
		for(j=0;j<16;j++)
		{
			for (i=0;i<3;i++)
		{
		

		tempresult=(receivedMsg.data[2*i+1+6*j+3]+256*(receivedMsg.data[2*i+6*j+3]))*(3.3/1024);
		snprintf(&Bdata[0],9,"%f",tempresult);

			
			if(j<4)
			{
			
				if(i==0)
				{
				putchar('B');
				putchar('E');
				putchar('N');
				putchar('D');
				putHex(j);
				putchar(' ');
				putchar(' ');
			
				}else if (i==1)
			
				{
				putchar('S');
				putchar('P');
				putchar('L');
				putchar('A');
				putHex(j);
				putchar(' ');
				putchar(' ');
						
			
				} else 
				{
		
				putchar('B');
				putchar('I');
				putchar('A');
				putchar('S');
				putHex(j);
				putchar(' ');
				putchar(' ');
		
					}
			
			}else{
							if(i==0)
				{
				putchar('B');
				putchar('E');
				putchar('N');
				putchar('D');
				putHex(j);
				putchar(' ');
				putchar(' ');
		
				}else if (i==1)
			
				{
				putchar('F');
				putchar('O');
				putchar('R');
				putchar('C');
				putHex((j-4));
				putchar(' ');
				putchar(' ');
				
			
				} else 
				{
		
				putchar('B');
				putchar('I');
				putchar('A');
				putchar('S');
				putHex(j);
				putchar(' ');
				putchar(' ');
			
					}
			
			}
			putchar(Bdata[0]);
			putchar(Bdata[1]);
			putchar(Bdata[2]);
			putchar(Bdata[3]);
			putchar(Bdata[4]);
			putchar(Bdata[5]);
			putchar(Bdata[6]);
			putchar(Bdata[7]);
			putchar(Bdata[8]);

		//	putchar(0x0A);
		//	putchar(0x0D);
					
		}
			
			
			}
			putchar(' ');
			putchar('E');
			putchar('N');
			putchar('D');
			putchar('O');
			putchar('F');
			putchar('D');
			putchar('A');
			putchar('T');
			putchar('A');
		
		}*/
	
			
			}
}

/*------------------------------------------------------------------------------
 * Handler for received packets. Flashes LED_0.
 */
void rf_callback(volatile rf_msgType* msg)
{
    UNUSED(msg);
    led_toggle(LED_0);

    isReceived = 1;
}

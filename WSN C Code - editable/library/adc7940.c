/*********************************************
Compile using ICC
Project : test
Version : 1.0
Date    : 17/05/2005
Author  : Philip Angove
Company : Tyndall
Comments: 
Attempt to implement a software SPI interface from ADI AD7490 ADC  
to 25mm3 module uC transceiver layer

DESCRIPTION
;*	This is a collection of 8/16-bit word, Mode 0, Master SPI routines.
;*	It simultaneously transmits and receives SPI data in 8- or 16-bit
;*	word format.  Data is sent and received MSB-first.  One pair of
;*	registers is used both to send and to receive; i.e., when one bit
;*	is shifted out (transmitted), the vacated bit position is used to
;*	store the new received bit.  These routines are low-level
;*	interface routines, and do not inherently contain a command
;*	structure; that is dictated by the connected SPI peripheral(s).
;*
;*	Due to having separate Enable/Disable and Read/Write-Word
;*	routines, larger blocks of data can be sent simply by calling
;*	the RW_SPI routine multiple times before disabling /SS.
;*
;* MAJOR ROUTINES:
;*	init_spi: initializes the port lines used for SPI.
;*			No calling requirements, returns nothing.
;*	ena_spi:  forces SCK low, and activates /SS signal.
;*			No calling requirements, returns nothing.
;*	disa_spi: brings /SS signal hi (inactive).
;*			No calling requirements, returns nothing.
;*	rw_spi:   sends/receives a an 8-bit or 16-bit data word.
;*			Must set up data to be sent in (spi_hi,spi_lo)
;*			prior to calling; it returns received data in
;*			the same register pair (if 8-bit, uses only
;*			the spi_lo register).
;*
;* VARIABLES:
;*	The spi_hi and spi_lo variables are the high and low data bytes.
;*	They can be located anywhere in the register file.
;*	
;*	The temp variable holds the bit count, and  also can be used in 
;*  timing the high/low minimum pulse width.  This must be located in 
;*	an upper register due to the use of an IMMEDIATE-mode instruction.
;*	(not required for ATmega 128)


Chip type           : ATmega128L
Program type        : Application
Clock frequency     : 4.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 1024
*********************************************/
/*********************************************
Program type        : Application
Clock frequency     : 4.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 1024
*********************************************/





/***************************************************************************
;*
;* PORT DEFINITIONS
;*
/***************************************************************************

MISO = 0		    ;PG0 pin
MOSI = 1		    ;PG1 pin
SCK  = 2			;PG2 pin
NSS  = 3			;PG3 pin            

**************************************************************************/
/*
#include <iom128v.h>
#include <macros.h>
#include <stdio.h>
#include <string.h>
#include "global.h"		// include our global settings
#include "delay.h"
*/

#include "global.h"
//#define MISO  (PORTG,0x00)//
#define MOSI  (PORTG,0x00)
#define MOSI_HI  PORTG |= (1<<0)
#define MOSI_LO  PORTG &= ~(1<<0)
#define MOSIIO (DDRG,0)		//data direction
#define MOSI_OUT  DDRG |= (1<<0)


#define MISO   (PING & (1<<1)) 
#define MISO_HI  PORTG |= (1<<1)
#define MISO_LO  PORTG &= ~(1<<1)
#define MISOIO (DDRG,1)		//data direction
#define MISO_IN  DDRG &= ~(1<<1)					 

#define SCK  (PORTG,0x02)
#define SCK_HI  PORTG |= (1<<2)
#define SCK_LO  PORTG &= ~(1<<2)
#define SCKIO (DDRG,2)		//data direction
#define SCK_OUT  DDRG |= (1<<2)


#define NSS  (PORTG,0x03)
#define NSS_inactive  PORTG |= (1<<3)
#define NSS_active  PORTG &= ~(1<<3)
#define NSSIO (DDRG,3)		//data direction
#define NSS_OUT  DDRG |= (1<<3)




#define MAG_EN  (PORTC,0x07)
#define MAG_EN_HI  PORTC |= (1<<7)
#define MAG_EN_LO  PORTC &= ~(1<<7)
#define MAG_ENIO (DDRC,7)		//data direction
#define MAG_EN_OUT  DDRC |= (1<<7)


#define MAG_SET  (PORTC,0x05)
#define MAG_SET_HI  PORTC |= (1<<5)
#define MAG_SET_LO  PORTC &= ~(1<<5)
#define MAG_SETIO (DDRC,5)		//data direction
#define MAG_SET_OUT  DDRC |= (1<<5)


#define MAG_RESET  (PORTC,0x06)
#define MAG_RESET_HI  PORTC |= (1<<6)
#define MAG_RESET_LO  PORTC &= ~(1<<6)
#define MAG_RESETIO (DDRC,6)		//data direction
#define MAG_RESET_OUT  DDRC |= (1<<6)

//#define SD_5V_CNTRL  (PORTA,0x04)//			//Turn on 5V IMU power - turns off in sleep mode (npn base pulls high)//
#define SD_5V_CNTRL  (PORTA,0x04)
//#define SD_5V_CNTRL_OFF  PORTA |= (1<<4)	//not needed (auto)//
#define SD_5V_CNTRL_ON  PORTA &= ~(1<<4)
#define SD_5V_CNTRL_ENIO (DDRA,4)		//data direction
#define SD_5V_CNTRL_OUT  DDRA |= (1<<4)





	
/***************************************************************************
;*
;* FUNCTION
;*	init_spi
;*
;* DESCRIPTION
;*	Initialize our port pins for use as SPI master.
;*
;* CODE SIZE:
;*	8 words
;*

init_spi:
	ss_inactive		;set latch bit hi (inactive)
	sbi	ddrg,nss	;make it an output
	;
	sck_lo			;set clk line lo
	sbi	ddrg,sck	;make it an output
	;
	mosi_lo			;set data-out lo
	sbi	ddrg,mosi	;make it an output
	;
	cbi	ddrg,miso	;not really required, it powers up clr'd!
	ret
	
;***************************************************************************/
	

void port_init_spi(void)
{
 NSS_inactive;
 NSS_OUT;
 //SCK_LO;
 SCK_HI;
 SCK_OUT;
 MOSI_LO;
 MOSI_OUT;
 MISO_IN;
}







/***************************************************************************
;*
;* FUNCTION
;*	ena_spi
;*
;* DESCRIPTION
;*	Init data & clock lines, then assert /SS.  Note that if more than
;*	one slave is used, copies of this could be made that would each
;*	reference a different /SS port pin (use SS_ACTIVE0, SS_ACTIVE1, ...)
;*	Ensure SCK is inactive prior to enabling /SS.
;*
;* CODE SIZE:
;*	4 words
;*
***************************************************************************/


void ena_spi(void)
{
//SCK_LO;			  			 //depends on SPI peripheral, SCK inactive
SCK_HI;							 //Ensure SCK inactive, should be already set
//MOSI_LO;
NSS_active;						 //SCK inactive, enable /ss (slave select)
}
	
	
	
	
	

/***************************************************************************
;*
;* FUNCTION
;*	disa_spi
;*
;* DESCRIPTION
;*	De-assert /SS.  Since this routine is so short, it might be better
;*	to use the SS_INACTIVE statement directly in higher level code.
;*	Again, if multiple slaves exist, additional copies of this could
;*	be created; or ONE routine that disabled ALL /ss signals could be
;*	used instead to make the code less error-prone due to calling the
;*	wrong Disable routine.
;*
;* CODE SIZE:
;*	2 words
;*
;***************************************************************************/


void disa_spi(void)
{
NSS_inactive;
//Delay_1us(10);
}


	
/***************************************************************************
;*
;* FUNCTION
;*	dummy_spi
;*
;* DESCRIPTION
;*	Required to write FF to AD7490 ADC, on initial power up.
;*	Write a word out on SPI while simultaneously reading in a word.
;*	Data is sent MSB-first, and info read from SPI goes into
;*	the same buffer that the write data is going out from.
;*	Make sure data, clock and /SS are init'd before coming here.
;*	SCK high time is ((delay * 3) + 1) AVR clock cycles.
;*
;*	If 8-bit use is needed, change  LDI TEMP,16  to ,8  and also
;*	eliminate the ROL SPI_HI statement.
;*
;* CODE SIZE:
;*	21 words (assembly)
;* NUMBER OF CYCLES (assembler):
;*	Overhead = 8, loop = 16 * (16 + (2* (delay_value*3)))
;	(With call + return + delay=4, it is about 648 cycles.)
;*
;***************************************************************************/

void dummy_spi(void)
{
unsigned char temp;
temp = 16;						//init loop counter to 16 bits (not required), number 
	   						    //of bits of data / word to be sent (AD7490 ADC)					
ena_spi();							

	for (temp=0;temp<16;temp++)
	{
//		SCK_LO;				   	//note normally for SPI MOSI is clocked on SCK
//		if (spi_hi&0x80)		//rising edge, note: AD7490 data write (control register) 
//		   MOSI_HI;				//& read is valid on the SCK falling edge
//		else
//		   MOSI_LO;			
//		Delay_1us(1);	//not really needed
		SCK_HI;			// clock HI, delay, then pull LO, should be already be high
		Delay_1us(1);   //time the SCK_HI pulse - depends on peripheral slave / AD7490
		
		MOSI_HI;	
 
		SCK_LO;			//drop clock line low, AD7490 clocked on falling edge 
		
		Delay_1us(1); 	//must now delay before reading in SPI data on MISO 
						//(following SCK_LO, i.e. falling edge)
					    //after delay, read in SPI bit & put into spi_lo lsb
	  	
	}

disa_spi();
				// returns with received byte & data transmitted	 		  								
}




	
/***************************************************************************
;*
;* FUNCTION
;*	rw_spi
;*
;* DESCRIPTION
;*	Write a word out on SPI while simultaneously reading in a word.
;*	Data is sent MSB-first, and info read from SPI goes into
;*	the same buffer that the write data is going out from.
;*	Make sure data, clock and /SS are init'd before coming here.
;*	SCK high time is ((delay * 3) + 1) AVR clock cycles.
;*
;*	If 8-bit use is needed, change  LDI TEMP,16  to ,8  and also
;*	eliminate the ROL SPI_HI statement.
;*
;* CODE SIZE:
;*	21 words (assembly)
;* NUMBER OF CYCLES (assembler):
;*	Overhead = 8, loop = 16 * (16 + (2* (delay_value*3)))
;	(With call + return + delay=4, it is about 648 cycles.)
;*
;***************************************************************************/

void rw_spi(char *data)
{
unsigned char din;
unsigned char spi_hi;
unsigned char spi_lo;
unsigned char temp;
char *temp_ptr;
unsigned char spi_lo_msb;
unsigned char spi_hi_msb;

temp = 16;						//init loop counter to 16 bits (not required), number 
	   						    //of bits of data / word to be sent (AD7490 ADC)					

temp_ptr = data;				//creates a pointer to the data passed in with array data[2)


spi_lo=*data;					//initially assign spi_lo & spi_hi to the initial passed in 
data++;							//config word.
spi_hi=*data;		
					

	for (temp=0;temp<16;temp++)
	{
//		SCK_LO;				   	//note normally for SPI MOSI is clocked on SCK
//		if (spi_hi&0x80)		//rising edge, note: AD7490 data write (control register) 
//		   MOSI_HI;				//& read is valid on the SCK falling edge
//		else
//		   MOSI_LO;			

		
		Delay_1us(1); 	//must now delay before reading in SPI data on MISO 
						//(following SCK_LO, i.e. falling edge)
					    //after delay, read in SPI bit & put into spi_lo lsb
						//only following SPI word left shift (MOSI transmisssion). 
						//Note: AD3 is read out from ADC following the NSS falling 
						//edge, the remaining bits (AD2, ...) are read on SCK falling edge.
						//(for example: AD2 is read following the first SCK falling edge,
						//& including a 1uS delay.		
		
		din=MISO;		//reads portG (pinG), bit 3 (MISO)


		
	
		spi_hi_msb = 0x80 & spi_hi;     //store spi_hi msb prior to left shift 

		spi_lo_msb = 0x80 & spi_lo;     //store spi_lo msb prior to left shift 
		spi_lo <<= 1;					//move 0 into D0, all other bits UP one slot,
		spi_hi <<= 1;					//spi_lo msb must be moved to spi_hi lsb   
		spi_lo_msb = spi_lo_msb >> 7;
		spi_hi = spi_hi | spi_lo_msb;


		if(din)
		spi_lo = spi_lo |0x01;
		else
		spi_lo = spi_lo & 0xFE;

		SCK_HI;			// clock HI, delay, then pull LO, should be already be high
		Delay_1us(1);   //time the SCK_HI pulse - depends on peripheral slave / AD7490

		if (spi_hi_msb & 0x80)
		   MOSI_HI;	
		else
		   MOSI_LO;		
		
		
		SCK_LO;			//drop clock line low, AD7490 clocked on falling edge 
		 
	
		
	}


data=temp_ptr;			//assign the shifted conversion result to the array data[2]
*data=spi_hi;			//data[2] is returned & transmitted by RF (Tx).
data++;
*data=spi_lo;

	
			// returns with received byte & data to be transmitted	 		  								
}




void MAG_DOMAIN_RESET(void)
{


	  

	// returns with magnetometer reset	 		  								
}

void rw_spiJ(unsigned char *data)
{
unsigned char temp;
*data=0x00;
*(data+1)=0x00;				
	
	for (temp=0;temp<8;temp++)
	{
	
		if(MISO)*data|=(1<<(7-temp));

		SCK_HI;			// clock HI, delay, then pull LO, should be already be high
		//delay_us(1);   //time the SCK_HI pulse - depends on peripheral slave / AD7490
	
		SCK_LO;			//drop clock line low, AD7490 clocked on falling edge 
		
	}

for (temp=0;temp<8;temp++)
{

		if(MISO)*(data+1)|=(1<<(7-temp));

		SCK_HI;			// clock HI, delay, then pull LO, should be already be high
		//delay_us(1);   //time the SCK_HI pulse - depends on peripheral slave / AD7490
		SCK_LO;			//drop clock line low, AD7490 clocked on falling edge 
	

}

		  								
}

//*********************************************************************//			
/*Function that reads from the ADC,does a simple mathematical operation
and puts the result into an array*/
//*********************************************************************//

void Get_Data(unsigned char data[]) 		//get spi AD7490 ADC conversion result
{
					 
	ena_spi();					 //activate /SS
	rw_spi(data);				 //send/receive 16 bits for AD7490 (or 8 bits peripheral)	 
	disa_spi();					 //deactivate /SS
	
}

void Get_DataJ(unsigned char *data) 		//get spi AD7490 ADC conversion result
{
					 
	ena_spi();					 //activate /SS
	rw_spiJ(data);				 //send/receive 16 bits for AD7490 (or 8 bits peripheral)	 
	disa_spi();					 //deactivate /SS
}
void Init_ADC(void)
{
unsigned char tempdata[2];
//SD_5V_CNTRL_OUT;
//SD_5V_CNTRL_ON; 			  //Turn ON 5V IMU power by pulling npn base low//
Delay_1ms(10);							//Ading a delay for startup (5V charge pump soft start)


//MAG_DOMAIN_RESET();						//initially reset the magnetomer


dummy_spi();							//required to write FF to the ADC on inital					  
dummy_spi();							//power up (mode dependant).

tempdata[0]=0x90;					 	 	 //ADD3 ... ADD0 (channel Vin), PM1, PM0 (mode 1 - powered)
tempdata[1]=0x83;							 //shadow (0, no seq), WEAK (pullup enabled, 1), range (low, 1), 
								         //CODING (binary, 1), data[0] = spi low byte, data[1] = spi high byte,
Get_Data(tempdata);							 //write ADC config word


tempdata[0]=0xB4;						 	 //write to the shadow register, to enable sequenced 
tempdata[1]=0x1F;						     //conversion on 9 i/p channels (Vin0, ... , Vin8)
								         
Get_Data(tempdata);							 //write ADC shadow word
Delay_1us(10);
}
void read_ADC(unsigned char *values)
{
	unsigned char i;
	unsigned char data[2];

	for (i=0;i<9;i++)
			{
	
			*data=0xB4;					 	 //ADD3 ... ADD0 (channel Vin3), PM1, PM0 (mode 1 - powered)
			*(data+1)=0x0F;						 //shadow (0, no seq), WEAK (pullup enabled, 1), range (low, 1), 
												 //CODING (binary, 1)
	
			Get_DataJ(data);						 //Continuously gets data from the ADC and writes to shadow register
			
			*(values+2*i) = data[0];
			
			*(values+2*i+1) = data[1];
		

			}		//end for

}
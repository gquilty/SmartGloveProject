


#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "delay.h"
#include "TMP102.h"



void TMP102_Port_Init(void)
{
	TMP102_SCL_OUT;
	TMP102_SCL_LO;
	TMP102_SDA_OUT;
	TMP102_SDA_HI;
}

void TMP102_Start_Command(void)
{

	TMP102_SCL_HI;
	TMP102_SDA_HI;
	delay_us(1);
	TMP102_SDA_LO;
	delay_us(1);
}

void TMP102_Stop_Command(void)
{
	TMP102_SDA_LO;
	delay_us(1);
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SDA_HI;
	delay_us(1);
}

void TMP102_Read_Configure(uint8_t* Status)
{
	unsigned char j;
	TMP102_Start_Command();
	TMP102_SCL_LO;
	delay_us(1);
	TMP102_SDA_HI;
	delay_us(1);
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;//1
	delay_us(1);
	TMP102_SDA_LO;
	delay_us(1);
	for(j=0;j<2;j++)
	{
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;
	delay_us(1);
	}//100
	TMP102_SDA_HI;
	delay_us(1);
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;
	TMP102_SDA_LO;
	delay_us(1);//1001
	for(j=0;j<3;j++)
	{
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;
	delay_us(1);
	}//1001000
	TMP102_SDA_LO;
	delay_us(1);
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;//10010001
	TMP102_SDA_LO;
	TMP102_SDA_IN;
	delay_us(1);
	TMP102_SCL_HI;//ACK by TMP102
	delay_us(1);
	if(PIND&0x02)
	{
		*Status=0x00;
		//return;
	}else *Status=0x01;
	TMP102_SCL_LO;
	delay_us(1);
	TMP102_SDA_OUT;
	TMP102_SDA_LO;
	delay_us(1);
	for(j=0;j<8;j++)
	{
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;
	delay_us(1);
	}//0000000
	
	
	//TMP102_SDA_LO;
	TMP102_SDA_IN;
	delay_us(1);
	TMP102_SCL_HI;//ACK by TMP102
	delay_us(1);
	if(PIND&0x02)
	{
		*Status=0x00;
		//return;
	}else *Status=0x01;
	TMP102_SCL_LO;
	delay_us(1);
	TMP102_SDA_OUT;
//	TMP102_SDA_LO;
	TMP102_Stop_Command();
}

void TMP102_Read_Temperature(uint8_t* Temperature,uint8_t* Status)
{
	unsigned char j;

	*Temperature=0x00;
	*(Temperature + 1)=0x00;

	TMP102_Start_Command();
	TMP102_SCL_LO;
	delay_us(1);
	TMP102_SDA_HI;
	delay_us(1);
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;//1
	delay_us(1);
	TMP102_SDA_LO;
	delay_us(1);
	for(j=0;j<2;j++)
	{
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;
	delay_us(1);
	}//100
	TMP102_SDA_HI;
	delay_us(1);
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;
	TMP102_SDA_LO;
	delay_us(1);//1001
	for(j=0;j<3;j++)
	{
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;
	delay_us(1);
	}//1001000
	TMP102_SDA_HI;
	delay_us(1);
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;//10010001
	TMP102_SDA_LO;
	TMP102_SDA_IN;
	delay_us(1);
	TMP102_SCL_HI;//ACK by TMP102
	delay_us(1);
	if(PIND&0x02)
	{
		*Status=0x00;
		//return;
	}else *Status=0x01;
	TMP102_SCL_LO;
	delay_us(1);
	
	for(j=0;j<4;j++)
	{
	TMP102_SCL_HI;
	delay_us(1);
	//if((PORTA&0x10))Temperature[0] |= (1<<(3-j));
	//if(PORTA&0x10) Temperature[0] |= (1<<(3-j));
	if(PIND&0x02)Temperature[0] = Temperature[0] |= (1<<(3-j));
	//if(j==0)*Temperature= PORTA;
	//if((TMP102_SDA)&&j==1) (*Temperature) |= 0x04;
	//if((TMP102_SDA)&&j==2) (*Temperature) |= (1<<1);
	//if((TMP102_SDA)&&j==3) (*Temperature) |= (1<<0);
	//if(j==3)*(Temperature+1)= PORTA;
	//Temperature[0] |= (1<<(3-j));
	//if(PORTA&0x04) *Temperature |= (1<<(3-j));
	
	//if(TMP102_SDA)Temperature[1] = TMP102_SDA;
	
	TMP102_SCL_LO;
	delay_us(1);
	}
	for(j=0;j<4;j++)
	{
	TMP102_SCL_HI;
	delay_us(1);
	//if(!(TMP102_SDA&0x04)) *Temperature= 0x01;
	//if(TMP102_SDA) Temperature [1] |= (1<<(7-j));
	//if(PORTA&0x10) Temperature [1] |= (1<<(7-j));
	if(PIND&0x02)Temperature[1] = Temperature [1] |= (1<<(7-j));
	//if(PORTA&0xF7) Temperature[1]|= (1<<(7-j));
	//if(PORTA&0x04) Temperature[1] = Temperature[1]|(1<<(7-j));
	TMP102_SCL_LO;
	delay_us(1);
	}
	TMP102_SDA_OUT;//Master ACK
	TMP102_SDA_LO;
	delay_us(1);
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;
	TMP102_SDA_IN;
	delay_us(1);
	for(j=4;j<8;j++)
	{
	TMP102_SCL_HI;
	delay_us(1);
	//if(TMP102_SDA) Temperature[1] |= (1<<(7-j));
	//if(PORTA&0x10) *(Temperature + 1) |= (1<<(7-j));
	if(PIND&0x02) Temperature[1] = Temperature[1]|(1<<(7-j));
	TMP102_SCL_LO;
	delay_us(1);
	}
	for(j=0;j<4;j++)
	{
	TMP102_SCL_HI;
	delay_us(1);
	//if(TMP102_SDA) *(Temperature + 1) |= (1<<(7-j));
	TMP102_SCL_LO;
	delay_us(1);
	}
	TMP102_SDA_OUT;//Master ACK
	TMP102_SDA_LO;
	delay_us(1);
	TMP102_SCL_HI;
	delay_us(1);
	TMP102_SCL_LO;
	delay_us(1);
	TMP102_Stop_Command();
	//*Temperature=0x00;
	//*(Temperature + 1)=0x00;

}











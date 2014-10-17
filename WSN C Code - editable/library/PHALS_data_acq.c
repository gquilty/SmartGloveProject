#include <avr/io.h>
#include <stdio.h>
#include "delay.h"




//Eoin//
#define   bit_set(...)      bit_set_(__VA_ARGS__)
#define   bit_set_(x,y)      x |= 1<<y         // set a bit
#define   bit_clear(...)      bit_clear_(__VA_ARGS__)
#define   bit_clear_(x,y)      x &= ~(1<<y)         // clear a bit
#define   bit_test(...)      bit_test_(__VA_ARGS__)
#define   bit_test_(x,y)      (!!(x & (1<<y)))      // test a bit
#define   bit_dir_outp(...)   bit_dir_outp_(__VA_ARGS__)
#define   bit_dir_outp_(x,y)   *(&x-1) |= 1<<y         // access DDRx of PORTx !
#define   bit_dir_inp(...)   bit_dir_inp_(__VA_ARGS__)
#define   bit_dir_inp_(x,y)   *(&x-1) &= ~(1<<y)      // access DDRx of PORTx !
#define   bit_test_in(...)   bit_test_in_(__VA_ARGS__)
#define   bit_test_in_(x,y)   (!!(*(&x-2) & (1<<y)))

#define RTC_ENABLE ASSR,2
#define RTCIF TIFR2,0
#define RTCIE TIMSK2,0

//Eoin//

//unsigned char data[3];
//uint16_t msec, sec, min, hour;
//
//void time_init(void)
//{
//	
//	//************************Eoin*****************************///
//	bit_set(RTC_ENABLE);
//  
//    sei();
//    bit_set(RTCIF); // clear IFLAG
//    TCCR2A=0;
//    TCCR2B=0x04;// use 4 for ms, use 5 for secDIV BY 128 = 1 sec
//    bit_set(RTCIE); // Enables timer 2 overflow interrupt
//	
//	//***********************Eoin******************************//
//}

//Eoin//
 
 //ISR(TIMER2_OVF_vect)
//{////
 //   time_init();
 //   
 //   //bit_clear( RTCIF);
 //   msec++;
	//if(msec == 1000)
	//{
	//
	///*msec4++;
	//
	//}
	//
 //   if (msec4 == 2 && msec == 231)
	//{
	//msec = 0;
	//msec4 = 0;
	//*/
	//msec =0;
	//sec++;
	//
	//	if(sec==60)
	//	{
	//		sec=0;
	//		min++;
	//		if(min==60)
	//		{
	//			
	//			hour++;
	//			min=0;
	//			
	//		}
	//	}
 //           
 //   }
 //   
	//return;
 //   
//}////

//
//
//void ecg_data_acq(unsigned char *edata)
// {
//	uint16_t temp_data[3], temp[3];
//	 
//	uint16_t te_sec;
//	uint16_t te_msec;
//	
//	ADC_Get_Data(temp);	
//	
//	edata = temp[0];
//	edata++;
//	
//	edata = temp[1];
//	edata++;
//	
//	edata = sec;
//	edata++;
//	
//	edata = msec;
//	
//}
//
//
//
//
//
//
//
//
//
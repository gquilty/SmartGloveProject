

/***************************************************************************
;*
;* PORT DEFINITIONS
;*
***************************************************************************

SCL = 0		    ;PA2 pin   
SDA = 1		    ;PA4 pin



**************************************************************************/

#define TMP102_Read_Address	0x49					 
#define TMP102_Write_Address	0x48

#define TMP102_SCL  (PORTD,0x00)
#define TMP102_SCL_HI  PORTD |= (1<<0)
#define TMP102_SCL_LO  PORTD &= ~(1<<0)
#define TMP102_SCLIO (DDRD,0)		//data direction
#define TMP102_SCL_OUT  DDRD |= (1<<0)

#define TMP102_SDA  (PORTD,0x01)
#define TMP102_SDA_HI  PORTD |= (1<<1)
#define TMP102_SDA_LO  PORTD &= ~(1<<1)
#define TMP102_SDAIO (DDRD,1)		//data direction
#define TMP102_SDA_OUT  DDRD |= (1<<1)
#define TMP102_SDA_IN  DDRD &= ~(1<<1)





void TMP102_Port_Init(void);
void TMP102_Start_Command(void);
void TMP102_Stop_Command(void);
void TMP102_Read_Configure(uint8_t* Status);
void TMP102_Read_Temperature(uint8_t* Temperature,uint8_t* Status);


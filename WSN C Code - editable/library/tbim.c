/****************************************************************************\
 * tbim.c
 *
 * @date 02-Feb-2007
 *
 * Functions for dealing with TEDS data, and reading data from PSoC.
 *
 * printSensorData Modified to interface with Intelligent Data Systems GPRS
 *
\****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "global.h"
//#include <avr/pgmspace.h>
//#include "hal_atmega.h"
#include "i2c.h"
#include "delay.h"
#include "pnpSensors.h"
#include "tbim.h"




extern char DataBuffer[2][DATA_BUFFER_SIZE+100];
extern char* DataPtr[2];


/**-------------------------------------------------------------------------*\
 * Initialises TBIM or individual sensor. If a command is sent to a sensor 
 * that does not exist, the TBIM should ignore the command.
 *
 * @param address   I2C address of TBIM.
 * @param cmd       Initialisation command to send.
 *
 * @return Error status.
\**-------------------------------------------------------------------------*/
uint8_t initTbim(uint8_t address, uint8_t cmd)
{
    /* Check command is valid */
    if (cmd != CMD_INIT_TBIM && cmd < CMD_INIT_SENSOR_01 && 
                                cmd > CMD_INIT_SENSOR_01 + MAX_NUM_SENSORS) {
        return TBIM_INVALID_COMMAND;
    }

    /* Write an init command */
    if (! i2c_write(address, &cmd, 1,I2C_OPTION_FULL)) return TBIM_FAILED_I2C;
//delay_ms(1);
    return TBIM_SUCCESS;
}


/**-------------------------------------------------------------------------*\
 * Reads TEDS data at a specific address. Verifies TEDS checksum.
 *
 * @param address   I2C address to read from.
 * @param cmd       Command gives which TEDS to read (meta or sensor_xx).
 * @param buffer    Buffer to store the TEDS data in. This array must be long 
 *                  enough to store the TEDS.
 *
 * @return error status.
\**-------------------------------------------------------------------------*/
uint8_t readTeds(uint8_t address,  uint8_t cmd, uint8_t* buffer)
{
    uint8_t i;
    uint8_t length;
    uint16_t chk = 0;
    uint16_t receivedChk;

    /* Check command is valid */
    if (cmd != CMD_READ_META_TEDS && cmd < CMD_READ_TEDS_01 &&
                                   cmd > CMD_READ_TEDS_01 + MAX_NUM_SENSORS) {
				        //printf_P(PSTR("log: Wrong tedsVersion from %#x (%i)\n"),
                // address, buffer[1]);				   
        //return TBIM_INVALID_COMMAND;
		
    }

    /* Find how many bytes to read */
    if (cmd == CMD_READ_META_TEDS) length = META_TEDS_LENGTH;
    else length = SENSOR_TEDS_LENGTH;

    /* Attempt to write a read TEDS command */
    if (! i2c_write(address, &cmd, 1,I2C_OPTION_FULL)) 
	{
		    //    printf_P(PSTR("log: Wrong write from %#x (%i)\n"),//
             //    address, buffer[1]);//
		//return TBIM_FAILED_I2C;
		delay_us(200);
		//delay_ms(1);
	}
	/* To allow the PSoC to write the data on the buffer */
	//delay_s(1);
    /* Attempt to read back TEDS data */
    if ( ! i2c_read(address, buffer, length,I2C_OPTION_FULL)) 
	{
	delay_us(200);
	//delay_ms(1);
		   //   printf_P(PSTR("log: Wrong read from %#x (%i)\n"),//
            //    address, buffer[1]);//
		//return TBIM_FAILED_I2C;

	}
    /* If reading meta-TEDS, check version is equal to TEDS_VERSION */
    if (cmd == CMD_READ_META_TEDS && buffer[1] != TEDS_VERSION) {
       // printf_P(PSTR("log: Wrong tedsVersion from %#x (%i)\n"),
        //         address, buffer[1]);
        //return TBIM_INVALID_TEDS;
		delay_us(200);
		//delay_ms(1);
    }

    /* Calculate checksum */
    for (i = 0; i < length - 2; ++i) chk += buffer[i];
    chk = ~chk;

    receivedChk = ((uint16_t)buffer[length-2] << 8) |
                   (uint16_t)buffer[length-1];

    /* Compare calculated checksum against recieved checksum */
    if (chk != receivedChk) {
       // printf_P(PSTR("log: Checksum from %#x (%#.4x) should be %#.4x\n"),
        //        address, receivedChk, chk);
        //return TBIM_INVALID_TEDS;
		
    }

    return TBIM_SUCCESS;
}


/**-------------------------------------------------------------------------*\
 * Reads sensor data at a specific address.
 *
 * @param address       I2C address to read from.
 * @param cmd           Command gives which sensor to read (CMD_READ_DATA_xx).
 * @param buffer        Buffer to store the sensor data in. Must be at least
 *                      dataLength bytes long.
 * @param dataLength    Number of bytes to read.
 * @param delay         (0.1ms * delay) between start sampling and reading 
 *                      data commands.
 *
 * @return Error status.
\**-------------------------------------------------------------------------*/
uint8_t readSensorData(uint8_t address, uint8_t cmd, uint8_t* buffer,
                       uint8_t dataLength, uint32_t sampleDelay)
{
    uint8_t startSampCmd;

    /* Check if command is valid */
    if (cmd < CMD_READ_DATA_01 || cmd > CMD_READ_DATA_01 + MAX_NUM_SENSORS) ;
	     // return TBIM_INVALID_COMMAND;  
    

    /* Get start sampling command for this sensor */
    startSampCmd = cmd + CMD_START_SAMP_01 - CMD_READ_DATA_01;

    /* Attempt to write start sampling command */
    if (! i2c_write(address, &startSampCmd, 1,I2C_OPTION_FULL));// return TBIM_FAILED_I2C;

    /* Delay */
  // delay_s(sampleDelay / 1000);
   // delay_ms(sampleDelay % 1000);
delay_ms(2);
    /* Attempt to write read sensor command */
    if (! i2c_write(address, &cmd, 1,I2C_OPTION_FULL));
        //return TBIM_FAILED_I2C;
	
	/* To allow time to the PSoC to copy data to the buffer */
	//delay(1,MILLISECOND);
	delay_ms(2);
    /* Attempt to read sensor data from TBIM */
    if (! i2c_read(address, buffer, dataLength,I2C_OPTION_FULL));// return TBIM_FAILED_I2C;
    //delay_ms(1);
    return TBIM_SUCCESS;
}


/**-------------------------------------------------------------------------*\
 * Use this to put TBIM to sleep or wakeup to active mode.
 *
 * @param address   I2C address of TBIM to put to sleep.
 * @param cmd       CMD_SLEEP_TBIM or CMD_WAKEUP_TBIM
 *
 * @return Error status.
\**-------------------------------------------------------------------------*/
uint8_t tbimPowerSave(uint8_t address, uint8_t cmd)
{
	if (cmd == CMD_WAKEUP_TBIM) delay_ms(1);//i2c_dummy(20); // generates a dummy to wake up i2c and waits 15ms
    if (! i2c_write(address, &cmd, 1,I2C_OPTION_FULL)) ;//return TBIM_FAILED_I2C;
    return TBIM_SUCCESS;
}


/**-------------------------------------------------------------------------*\
 * Copy byte array into metaTedsStruct.
 *
 * @param array     Pointer to source.
 * @param teds      Pointer to destination.
\**-------------------------------------------------------------------------*/
void copyArrayToMetaTeds(const uint8_t* array, metaTedsStruct* teds)
{
    teds->tedsVersion = array[1];
    teds->uuid = ((uint16_t)array[2] << 8) | (uint16_t)array[3];
    teds->numSensors =
            (array[4] <= MAX_NUM_SENSORS) ? array[4]: MAX_NUM_SENSORS;
}
void copyArrayToMetaTeds2(const uint8_t* array, metaTedsStruct* teds)
{
    teds->tedsVersion = array[0];
    teds->uuid = ((uint16_t)array[1] << 8) | (uint16_t)array[2];
    teds->numSensors =
            (array[3] <= MAX_NUM_SENSORS) ? array[3]: MAX_NUM_SENSORS;
}

/**-------------------------------------------------------------------------*\
 * Copy byte array into sensorTedsStruct.
 *
 * @param array     Pointer to source.
 * @param teds      Pointer to destination.
\**-------------------------------------------------------------------------*/
void copyArrayToSensorTeds(const uint8_t* array, sensorTedsStruct* teds)
{
    teds->sensorType = array[1];
    teds->dataType = array[2];
    teds->dataLength =
            (array[3] <= MAX_DATA_LENGTH) ? array[3]: MAX_DATA_LENGTH;
    teds->warmUpTime = ((uint16_t)array[4] << 8) | (uint16_t)array[5];
    teds->samplingPeriod = ((uint16_t)array[6] << 8) | (uint16_t)array[7];
}
void copyArrayToSensorTeds2(const uint8_t* array, sensorTedsStruct* teds)
{
    teds->sensorType = array[0];
    teds->dataType = array[1];
    teds->dataLength =
            (array[2] <= MAX_DATA_LENGTH) ? array[2]: MAX_DATA_LENGTH;
    teds->warmUpTime = ((uint16_t)array[3] << 8) | (uint16_t)array[4];
    teds->samplingPeriod = ((uint16_t)array[5] << 8) | (uint16_t)array[6];
}

/**-------------------------------------------------------------------------*\
 * Print out formatted meta-TEDS data.
 *
 * @param teds  pointer to meta-TEDS data.
\**-------------------------------------------------------------------------*/
void printMetaTeds(const metaTedsStruct* teds)
{
  //  puts_P(PSTR("log:\tMeta-TEDS:"));
   // printf_P(PSTR("log:\t\ttedsVersion = %u\n"), teds->tedsVersion);
   // printf_P(PSTR("log:\t\tuuid = %#.4x\n"), teds->uuid);
   // printf_P(PSTR("log:\t\tnumSensors = %u\n"), teds->numSensors);
}


/**-------------------------------------------------------------------------*\
 * Print out formatted sensor-TEDS data.
 *
 * @param teds  pointer to sensor-TEDS data.
\**-------------------------------------------------------------------------*/
void printSensorTeds(const sensorTedsStruct* teds)
{
    static const char* dataStrings[12] = {"U8", "S8", "U16", "S16", "U32",
                    "S32", "U64", "S64", "float", "double", "bool", "string"};

    static const char* sensorStrings[16] = {"Thermistor_Depth_1", "Thermistor_Depth_2", "Thermistor_Depth_3", 
                                      "DO_Temperature", "DO_O2_level(%)","DO_O2_level(ppm)","Fluorometer","PH","Battery(V)","NCSR_DO_O2(mg/L)","NCSR_DO_Temp(°C)"};

  //  printf_P(PSTR("log:\tSensor-TEDS:"));

   // printf_P(PSTR("\nlog:\t\tsensorType = %#x"), teds->sensorType);
    if (teds->sensorType <= 0x05) {
     //   printf_P(PSTR(" (%s)"), sensorStrings[teds->sensorType-1]);
    }

    //printf_P(PSTR("\nlog:\t\tdataType = %#x"), teds->dataType);
    if (teds->dataType <= 0x0C) {
      //  printf_P(PSTR(" (%s)"), dataStrings[teds->dataType-1]);
    }

    //printf_P(PSTR("\nlog:\t\tdataLength = %u bytes"), teds->dataLength);

    //printf_P(PSTR("\nlog:\t\twarmUpTime = %u * 1ms"), teds->warmUpTime);

    //printf_P(PSTR("\nlog:\t\tsamplingPeriod = %u * 1ms\n"),
                //  teds->samplingPeriod);
}

/**-------------------------------------------------------------------------*\
 * Print out data, converting from raw bytes into human readable form.
 *
 * @param data      Pointer to data bytes. Data is big-endian.
 * @param dataType  Description of databytes.
\**-------------------------------------------------------------------------*/
double PrintSensorData(uint8_t type, const uint8_t* data, uint8_t length)
{
    uint8_t i, j;
	

    /* Union used to convert data to a different type */
	
    union {
        uint8_t u8[8];
        int8_t s8[8];
        uint16_t u16[4];
        int16_t s16[4];
        uint32_t u32[2];
        int32_t s32[2];
        float f[2];
        double d[1];
    } dataUnion;
	typedef union {
	float fe;
	uint8_t b[4];
	} fbsplit;


    /* Copy from raw data to Union. Bytes are reversed to be little Endian */
   for (i = 0, j = 7; i < length && i < 8; ++i, --j) {
        dataUnion.u8[j] = data[i];
    }
	

    switch (type) {
        case DATATYPE_U8:
            printf_P(PSTR("%u"), dataUnion.u8[7]);
            break;
        case DATATYPE_S8:
            printf_P(PSTR("%i"), dataUnion.s8[7]);
            break;
        case DATATYPE_U16:
            printf_P(PSTR("%u"), dataUnion.u16[3]);
            break;
        case DATATYPE_S16:
            printf_P(PSTR("%i"), dataUnion.s16[3]);
            break;
        case DATATYPE_U32:
            printf_P(PSTR("%lu"), dataUnion.u32[1]);
            break;
        case DATATYPE_S32:
            printf_P(PSTR("%li"), dataUnion.s32[1]);
            break;
        case DATATYPE_F32:
		
			//printf("%f\n", *da);
		// printf_P(PSTR("%u\n"), dataUnion.u8[7]);
           // printf_P(PSTR("%G"), (double)dataUnion.f[1]);
			
			// printf("%G\n", (double)dataUnion.f[1]);
			//printf_P(PSTR("%G\n"), (double)dataUnion.f[0]);
			 //printf_P(PSTR("%f\n"), data);
			// printf("%d\n", length);
			 
			/* for (i = 0; i < 4; ++i) {
			 
			  dat.b[i]=data[i];
			  //printf("%.2x", data[i]);
                
            }*/
			
		
            break;
        case DATATYPE_F64:
         // printf_P(PSTR("%G"), dataUnion.d[0]);
			//printf_P(PSTR("%f"), dataUnion.d[0]);
            break;
        case DATATYPE_BOOL:
            printf_P(PSTR("%u"), dataUnion.u8[7] != 0);
            break;
        case DATATYPE_STRING:
            putchar('"');
            for (i = 0; i < length; ++i) {
                putchar(data[i]);
            }
            putchar('"');
            break;
        case DATATYPE_RAW_DO_DATA:
            printf_P(PSTR("%u %u %u"), (data[0] << 8) | data[1],
                                       (data[2] << 8) | data[3],
                                       (data[4] << 8) | data[5]);
            break;
        /* No conversion specifier for 64bit ints, so print them as hex */
        case DATATYPE_U64:
        case DATATYPE_S64:
        default:
            for (i = 0; i < length; ++i) {
                printf_P(PSTR("%.2x"), data[i]);
				
            }
            break;
    }
	return (double)dataUnion.f[1];
}

#ifdef INTELLIGENT_DATA_SYSTEMS_INTERFACE
/**-------------------------------------------------------------------------*\
 * Print out data, converting from raw bytes into human readable form.
 *
 * @param data      Pointer to data bytes. Data is big-endian.
 * @param dataType  Description of databytes.
\**-------------------------------------------------------------------------*/
void PrintSensorDataIDSformat(uint8_t sensortype, const uint8_t* data, uint8_t node,uint8_t index, uint8_t sensor)
{
    uint8_t i, j;

 									  
    /* Union used to convert data to a different type */
    union {
        uint8_t u8[8];
        int8_t s8[8];
        uint16_t u16[4];
        int16_t s16[4];
        uint32_t u32[2];
        int32_t s32[2];
        float f[2];
        double d[1];
    } dataUnion;

/* To be agreed regarding number of sensors for deployment with Javi and John W */
   /* static const char* sensorStrings[14] = {
											"Thermistor   ", 
											"Temp1(°C)", //1
											"Temp2(°C)", //2
											"Temp3(°C)", //3				
											"DO_Temp(°C)", //4
											"DO_O2(ppm)",//5
                                            "DO_O2(%)",//6
											"Fluoro(V)",//7
											"pH(pH)",//8
											"Battery(V)",//9
											"NCSR_DO_O2(mg/L)",//10	
											"NCSR_DO_Temp(°C)",//
											"SHT71 H"//12											
											};*/
	static const char* sensorStrings[14] = {
											"Thermistor   ", 
											"Temp1(°C)", //1
											"Water_Level", //2
											"BatteryVoltage", //3				
											"ZDO(°C)", //4
											"ZDO(ppm)",//5
                                            "ZDO(%)",//6
											"Trilux_1",//7
											"Conductivity",//8
											"Trilux_2",//9
											"Trilux_3",//10	
											"NCSRDO(°C)",//
											"SHT71 H"//12											
											};

	DataPtr[node] += sprintf_P(DataPtr[node],PSTR("%s,"), sensorStrings[sensortype]); 

    /* Copy from raw data to Union. Bytes are reversed to be little Endian */
    /*for (i = 0, j = 7; i < 8 && i < 8; ++i, --j) {
        dataUnion.u8[j] = data[i];
    }*/
     for (i = 0, j = 7; i < 8 && i < 8; ++i, --j) {
        dataUnion.u8[j] = data[i];
    }
	/* Handling only float type data */
		
		DataPtr[node] +=sprintf_P(DataPtr[node],PSTR(" %5.4G ,"),(float)dataUnion.f[1]);
		//if(last==0)DataPtr[node] +=sprintf_P(DataPtr[node],PSTR("%6.3f,"),(double)dataUnion.f[1]);
		//if(last==1)DataPtr[node] +=sprintf_P(DataPtr[node],PSTR("%6.3f"),(double)dataUnion.f[1]);
		
}

#endif

#ifdef SIMULATE_TBIM
/* Code for simulating a single TBIM with 2 sensors with I2C address 8 */
uint8_t lastCmd;
uint8_t TEST_writeI2C(uint8_t address, uint8_t* data,
                      uint16_t __attribute__((unused)) unused)
{
    if (address != 8) return 0;
    lastCmd = data[0];
    return 1;
}

uint8_t TEST_readI2C(uint8_t address, uint8_t* data, uint16_t length)
{
    if (address != 8) return 0;
    switch (length) {
    case META_TEDS_LENGTH:
        data[0] = 7; data[1] = 3; data[2] = 0; data[3] = 1; data[4] = 2;
        data[5] = 0xFF; data[6] = 0xF2;
        break;
    case SENSOR_TEDS_LENGTH:
        if (lastCmd == CMD_READ_TEDS_01) {
            data[0] = 10; data[1] = THERMISTOR; data[2] = DATATYPE_U8;
            data[3] = 1; data[4] = 0; data[5] = 5; data[6] = 0; data[7] = 5;
            data[8] = 0xFF; data[9] = 0xE8;
        }
        else if (lastCmd == CMD_READ_TEDS_02) {
            data[0] = 10; data[1] = LDR; data[2] = DATATYPE_F32;
            data[3] = 4; data[4] = 0; data[5] = 10; data[6] = 0; data[7] = 10;
            data[8] = 0xFF; data[9] = 0xD1;
        }
        break;
    case 1:
        data[0] = 17;
        break;
    case 4:
        data[0] = 0x42; data[1] = 0xF6; data[2] = 0xE9; data[3] = 0x79;
        break;
    default: return 0;
    }
    return 1;
}
#endif

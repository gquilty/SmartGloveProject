/****************************************************************************\
 * pnpSensors.h
 *
 * @date 19-May-2006
 *
 * Definitions for communicating with plug-ang-play (PNP) sensors. Further
 * information is in "PlugAndPlay Sensors" document.
\****************************************************************************/


#ifndef PNPSENSORS_H
#define PNPSENSORS_H


/* Length of TEDSs */
#define META_TEDS_LENGTH   7
#define SENSOR_TEDS_LENGTH 10

/* The maximum number of bytes needed by a sensor reading */
#define MAX_DATA_LENGTH         16

/* Maximum number of TBIMs connected to an NCAP at once */
#define MAX_NUM_TBIMS           8

/* Maximum number of sensors connected to a TBIM at once */
#define MAX_NUM_SENSORS         8

/* I2C Addresses */
#define LOWEST_I2C_ADDRESS      0x08
#define HIGHEST_I2C_ADDRESS     LOWEST_I2C_ADDRESS + MAX_NUM_TBIMS


/* Command definitions */
#define CMD_INIT_TBIM           0x11
#define CMD_WAKEUP_TBIM         0x12
#define CMD_SLEEP_TBIM          0x13
#define CMD_READ_META_TEDS      0x14

#define CMD_READ_TEDS_01        0x20
#define CMD_READ_TEDS_02        0x21
#define CMD_READ_TEDS_03        0x22
#define CMD_READ_TEDS_04        0x23
#define CMD_READ_TEDS_05        0x24
#define CMD_READ_TEDS_06        0x25
#define CMD_READ_TEDS_07        0x26
#define CMD_READ_TEDS_08        0x27

#define CMD_READ_DATA_01        0x30
#define CMD_READ_DATA_02        0x31
#define CMD_READ_DATA_03        0x32
#define CMD_READ_DATA_04        0x33
#define CMD_READ_DATA_05        0x34
#define CMD_READ_DATA_06        0x35
#define CMD_READ_DATA_07        0x36
#define CMD_READ_DATA_08        0x37

#define CMD_INIT_SENSOR_01      0x40
#define CMD_INIT_SENSOR_02      0x41
#define CMD_INIT_SENSOR_03      0x42
#define CMD_INIT_SENSOR_04      0x43
#define CMD_INIT_SENSOR_05      0x44
#define CMD_INIT_SENSOR_06      0x45
#define CMD_INIT_SENSOR_07      0x46
#define CMD_INIT_SENSOR_08      0x47

#define CMD_START_SAMP_01       0x50
#define CMD_START_SAMP_02       0x51
#define CMD_START_SAMP_03       0x52
#define CMD_START_SAMP_04       0x53
#define CMD_START_SAMP_05       0x54
#define CMD_START_SAMP_06       0x55
#define CMD_START_SAMP_07       0x56
#define CMD_START_SAMP_08       0x57

#define CMD_SLEEP_SENSOR_01     0x60
#define CMD_SLEEP_SENSOR_02     0x61
#define CMD_SLEEP_SENSOR_03     0x62
#define CMD_SLEEP_SENSOR_04     0x63
#define CMD_SLEEP_SENSOR_05     0x64
#define CMD_SLEEP_SENSOR_06     0x65
#define CMD_SLEEP_SENSOR_07     0x66
#define CMD_SLEEP_SENSOR_08     0x67

/**
 * Structure used to store meta-TEDS data.
 * The length and the checksum are not stored!
 **/
typedef struct {
/*  uint8_t tedsLength; */
    uint8_t tedsVersion;        /* 0x03 for this version */
    uint16_t uuid;              /* Universal unique ID */
    uint8_t numSensors;         /* Number of sensors connected */
/*  uint16_t tedsChk; */
} metaTedsStruct;

#define TEDS_VERSION            3

/**
 * Structure used to store Sensor-TEDS data.
 * The length and the checksum are not stored!
 **/
typedef struct {
/*  uint8_t tedsLength; */
    uint8_t sensorType;         /* Type of sensor */
    uint8_t dataType;           /* Type of data sensor gives */
    uint8_t dataLength;         /* Num bytes in data */
    uint16_t warmUpTime;        /* Unit = 1ms */
    uint16_t samplingPeriod;    /* Unit = 1ms */
/*  uint16_t tedsChk; */
} sensorTedsStruct;


/* Types of sensor */
#define THERMISTOR              0x01
#define NCSR_DO                 0x02
#define LDR                     0x03  /* Light Dependent resistor */
#define SHT71_TEMPERATURE       0x04
#define SHT71_HUMIDITY          0x05
#define GW_WQ710_TURB			0x06  /* Global water turbidity sensor */
#define GW_WQ301_COND			0x07  /* Global water conductivity sensor */
#define GW_WQ201_PH  			0x08  /* Global water pH sensor */
#define GW_WQ101_TEMP			0x09  /* Global water temperature sensor */
#define GW_WL400_LEVEL			0x0A  /* Global water water level sensor */
#define NCSR_PO					0x0B  


/* Types of data (U = unsigned, S = signed, F = float) */
#define DATATYPE_U8             0x01
#define DATATYPE_S8             0x02
#define DATATYPE_U16            0x03
#define DATATYPE_S16            0x04
#define DATATYPE_U32            0x05
#define DATATYPE_S32            0x06
#define DATATYPE_U64            0x07
#define DATATYPE_S64            0x08
#define DATATYPE_F32            0x09
#define DATATYPE_F64            0x0A
#define DATATYPE_BOOL           0x0B
#define DATATYPE_STRING         0x0C
#define DATATYPE_RAW_DO_DATA    0xFE
#define DATATYPE_CUSTOM         0xFF


#endif /* PNPSENSORS_H */

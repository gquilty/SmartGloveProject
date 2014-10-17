/****************************************************************************\
 * tbim.h
 *
 * @date 19-May-2006
 *
 * Functions for communicating with TBIM, and handling TEDS data.
\****************************************************************************/

#ifndef TBIM_H
#define TBIM_H


/* Possible return values for functions declared in this file */
#define TBIM_SUCCESS                0
#define TBIM_FAILED_I2C             1
#define TBIM_INVALID_TEDS           2
#define TBIM_INVALID_COMMAND        3

#define NUM_CHAR_SENSOR_DEF			13
#define NUM_CHAR_SENSOR_DATA		7
#define DATA_BUFFER_SIZE			512


/**
 * Structure to store information about a single TBIM.
 **/
typedef struct {
    uint8_t isConnected;
    metaTedsStruct metaTeds;
    sensorTedsStruct sensorTeds[MAX_NUM_SENSORS];
} tbimStruct;


/**-------------------------------------------------------------------------*\
 * Initialises TBIM or individual sensor. If a command is sent to a sensor 
 * that does not exist, the TBIM should ignore the command.
 *
 * @param address   I2C address of TBIM.
 * @param cmd       Initialisation command to send.
 *
 * @return Error status.
\**-------------------------------------------------------------------------*/
uint8_t initTbim(uint8_t address, uint8_t cmd);


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
uint8_t readTeds(uint8_t address, uint8_t cmd, uint8_t* buffer);


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
                       uint8_t dataLength, uint32_t sampleDelay);


/**-------------------------------------------------------------------------*\
 * Use this to put TBIM to sleep or wakeup to active mode.
 *
 * @param address   I2C address of TBIM to put to sleep.
 * @param cmd       CMD_SLEEP_TBIM or CMD_WAKEUP_TBIM
 *
 * @return Error status.
\**-------------------------------------------------------------------------*/
uint8_t tbimPowerSave(uint8_t address, uint8_t cmd);


/**-------------------------------------------------------------------------*\
 * Copy byte array into metaTedsStruct.
 *
 * @param array     Pointer to source.
 * @param teds      Pointer to destination.
\**-------------------------------------------------------------------------*/
void copyArrayToMetaTeds(const uint8_t* array, metaTedsStruct* teds);

void copyArrayToMetaTeds2(const uint8_t* array, metaTedsStruct* teds);
/**-------------------------------------------------------------------------*\
 * Copy byte array into sensorTedsStruct.
 *
 * @param array     Pointer to source.
 * @param teds      Pointer to destination.
\**-------------------------------------------------------------------------*/
void copyArrayToSensorTeds(const uint8_t* array, sensorTedsStruct* teds);
void copyArrayToSensorTeds2(const uint8_t* array, sensorTedsStruct* teds);

/**-------------------------------------------------------------------------*\
 * Print out formatted meta-TEDS data.
 *
 * @param teds  pointer to meta-TEDS data.
\**-------------------------------------------------------------------------*/
void printMetaTeds(const metaTedsStruct* teds);


/**-------------------------------------------------------------------------*\
 * Print out formatted sensor-TEDS data.
 *
 * @param teds  pointer to sensor-TEDS data.
\**-------------------------------------------------------------------------*/
void printSensorTeds(const sensorTedsStruct* teds);

/**-------------------------------------------------------------------------*\
 * Print out data, converting from raw bytes into human readable form.
 *
 * @param data      Pointer to data bytes. Data is big-endian.
 * @param dataType  Description of databytes.
\**-------------------------------------------------------------------------*/
double PrintSensorData(uint8_t datatype, const uint8_t* data, uint8_t length);

//#ifdef INTELLIGENT_DATA_SYSTEMS_INTERFACE
/**-------------------------------------------------------------------------*\
 * Print out data, converting from raw bytes into human readable form.
 *
 * @param data      Pointer to data bytes. Data is big-endian.
 * @param dataType  Description of databytes.
\**-------------------------------------------------------------------------*/

void PrintSensorDataIDSformat(uint8_t sensortype, const uint8_t* data, uint8_t node,uint8_t index, uint8_t sensor);

//#endif

#ifdef SIMULATE_TBIM
/* Functions for simulating TBIMs */
uint8_t TEST_writeI2C(uint8_t address, uint8_t* data, uint16_t length);
uint8_t TEST_readI2C(uint8_t address, uint8_t* data, uint16_t length);
#endif

#endif

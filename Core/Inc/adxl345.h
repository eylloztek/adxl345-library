/*
 * adxl345.h
 *
 *  Created on: Jun 1, 2026
 *      Author: Eylül Öztek
 */

#ifndef INC_ADXL345_H_
#define INC_ADXL345_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define ADXL345_ADDRESS_LOW        			(0x53 << 1)
#define ADXL345_ADDRESS_HIGH       			(0x1D << 1)
#define ADXL345_DEVICE_ID          			0xE5

#define ADXL345_DEVID 						0x00
#define ADXL345_THRESH_TAP 					0x1D
#define ADXL345_OFSX						0x1E
#define ADXL345_OFSY						0x1F
#define ADXL345_OFSZ						0x20
#define ADXL345_DUR							0x21
#define ADXL345_LATENT						0x22
#define ADXL345_WINDOW						0x23
#define ADXL345_THRESH_ACT					0x24
#define ADXL345_THRESH_INACT				0x25
#define ADXL345_TIME_INACT					0x26
#define ADXL345_ACT_INACT_CTL				0x27
#define ADXL345_THRESH_FF					0x28
#define ADXL345_TIME_FF 					0x29
#define ADXL345_TAP_AXES					0x2A
#define ADXL345_ACT_TAP_STATUS				0x2B
#define ADXL345_BW_RATE						0x2C
#define ADXL345_POWER_CTL					0x2D
#define ADXL345_INT_ENABLE 					0x2E
#define ADXL345_INT_MAP						0x2F
#define ADXL345_INT_SOURCE					0x30
#define ADXL345_DATA_FORMAT					0x31
#define ADXL345_DATAX0 						0x32
#define ADXL345_DATAX1						0x33
#define ADXL345_DATAY0						0x34
#define ADXL345_DATAY1 						0x35
#define ADXL345_DATAZ0						0x36
#define ADXL345_DATAZ1						0x37
#define ADXL345_FIFO_CTL					0x38
#define ADXL345_FIFO_STATUS					0x39

#define ADXL345_TIMEOUT						1000

#define ADXL345_SCALE_FACTOR_FULL_RES   	(float)1/256.0
#define ADXL345_SCALE_FACTOR_2G 			(float)1/256.0
#define ADXL345_SCALE_FACTOR_4G 			(float)1/128.0
#define ADXL345_SCALE_FACTOR_8G 			(float)1/64.0
#define ADXL345_SCALE_FACTOR_16G 			(float)1/32.0

#define ADXL345_WAKEUP_8HZ					0x00
#define ADXL345_WAKEUP_4HZ					0x01
#define ADXL345_WAKEUP_2HZ					0x02
#define ADXL345_WAKEUP_1HZ					0x03

#define ADXL345_RANGE_2G					0x00
#define ADXL345_RANGE_4G					0x01
#define ADXL345_RANGE_8G					0x02
#define ADXL345_RANGE_16G					0x03

#define ADXL345_DATARATE_3200HZ  			0x0F
#define ADXL345_DATARATE_1600HZ  			0x0E
#define ADXL345_DATARATE_800HZ   			0x0D
#define ADXL345_DATARATE_400HZ   			0x0C
#define ADXL345_DATARATE_200HZ   			0x0B
#define ADXL345_DATARATE_100HZ   			0x0A
#define ADXL345_DATARATE_50HZ    			0x09
#define ADXL345_DATARATE_25HZ    			0x08
#define ADXL345_DATARATE_12_5HZ  			0x07
#define ADXL345_DATARATE_6_25HZ  			0x06
#define ADXL345_DATARATE_3_13HZ  			0x05
#define ADXL345_DATARATE_1_56HZ  			0x04
#define ADXL345_DATARATE_0_78HZ  			0x03
#define ADXL345_DATARATE_0_39HZ  			0x02
#define ADXL345_DATARATE_0_20HZ  			0x01
#define ADXL345_DATARATE_0_10HZ  			0x00

/* POWER_CTL Register Bits */
#define ADXL345_POWER_CTL_WAKEUP_MASK       0x03
#define ADXL345_POWER_CTL_SLEEP             (1 << 2)
#define ADXL345_POWER_CTL_MEASURE           (1 << 3)
#define ADXL345_POWER_CTL_AUTO_SLEEP        (1 << 4)
#define ADXL345_POWER_CTL_LINK              (1 << 5)

/* DATA_FORMAT Register Bits */
#define ADXL345_DATA_FORMAT_RANGE_MASK      0x03
#define ADXL345_DATA_FORMAT_JUSTIFY         (1 << 2)
#define ADXL345_DATA_FORMAT_FULL_RES        (1 << 3)
#define ADXL345_DATA_FORMAT_INT_INVERT      (1 << 5)
#define ADXL345_DATA_FORMAT_SPI             (1 << 6)
#define ADXL345_DATA_FORMAT_SELF_TEST       (1 << 7)

/* BW_RATE Register Bits */
#define ADXL345_BW_RATE_RATE_MASK           0x0F
#define ADXL345_BW_RATE_LOW_POWER           (1 << 4)

typedef enum {
	ADXL345_OK = 0, ADXL345_ERROR, ADXL345_INVALID_ID, ADXL345_INVALID_PARAM
} ADXL345_Status_t;

typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint16_t address;
	uint8_t range;
	float scaleFactor;
} ADXL345_Handle_t;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} ADXL345_RawData_t;

typedef struct {
	float x;
	float y;
	float z;
} ADXL345_GData_t;

typedef struct {
	float roll;
	float pitch;
} ADXL345_Angle_t;

ADXL345_Status_t ADXL345_Init(ADXL345_Handle_t *dev, I2C_HandleTypeDef *hi2c, uint16_t address);
ADXL345_Status_t ADXL345_ReadDeviceID(ADXL345_Handle_t *dev, uint8_t *deviceID);
ADXL345_Status_t ADXL345_IsDeviceReady(ADXL345_Handle_t *dev);
ADXL345_Status_t ADXL345_SetStandbyMode(ADXL345_Handle_t *dev);
ADXL345_Status_t ADXL345_SetMeasureMode(ADXL345_Handle_t *dev);
ADXL345_Status_t ADXL345_SetRange(ADXL345_Handle_t *dev, uint8_t range, uint8_t fullResolution);
ADXL345_Status_t ADXL345_SetDataRate(ADXL345_Handle_t *dev, uint8_t dataRate, uint8_t lowPower);
ADXL345_Status_t ADXL345_ReadRawXYZ(ADXL345_Handle_t *dev, ADXL345_RawData_t *rawData);
ADXL345_Status_t ADXL345_ReadGXYZ(ADXL345_Handle_t *dev, ADXL345_GData_t *gData);
ADXL345_Status_t ADXL345_CalculateRollPitch(ADXL345_Handle_t *dev, ADXL345_Angle_t *angle);

#endif /* INC_ADXL345_H_ */

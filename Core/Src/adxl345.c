/*
 * adxl345.c
 *
 *  Created on: Jun 1, 2026
 *      Author: Eylül Öztek
 */

#include "adxl345.h"
#include <math.h>

static ADXL345_Status_t ADXL345_ReadRegister(ADXL345_Handle_t *dev, uint8_t reg,
		uint8_t *data, uint16_t length) {
	if (dev == NULL || dev->hi2c == NULL || data == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (HAL_I2C_Mem_Read(dev->hi2c, dev->address, reg,
	I2C_MEMADD_SIZE_8BIT, data, length, ADXL345_TIMEOUT) != HAL_OK) {
		return ADXL345_ERROR;
	}

	return ADXL345_OK;
}

static ADXL345_Status_t ADXL345_WriteRegister(ADXL345_Handle_t *dev,
		uint8_t reg, uint8_t value) {
	if (dev == NULL || dev->hi2c == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (HAL_I2C_Mem_Write(dev->hi2c, dev->address, reg,
	I2C_MEMADD_SIZE_8BIT, &value, 1, ADXL345_TIMEOUT) != HAL_OK) {
		return ADXL345_ERROR;
	}

	return ADXL345_OK;
}

ADXL345_Status_t ADXL345_IsDeviceReady(ADXL345_Handle_t *dev) {
	if (dev == NULL || dev->hi2c == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (HAL_I2C_IsDeviceReady(dev->hi2c, dev->address, 3, ADXL345_TIMEOUT)
			!= HAL_OK) {
		return ADXL345_ERROR;
	}

	return ADXL345_OK;
}

ADXL345_Status_t ADXL345_ReadDeviceID(ADXL345_Handle_t *dev, uint8_t *deviceID) {
	if (dev == NULL || deviceID == NULL) {
		return ADXL345_INVALID_PARAM;
	}
	return ADXL345_ReadRegister(dev, ADXL345_DEVID, deviceID, 1);
}

ADXL345_Status_t ADXL345_Init(ADXL345_Handle_t *dev, I2C_HandleTypeDef *hi2c,
		uint16_t address) {
	uint8_t deviceID = 0;

	if (dev == NULL || hi2c == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	dev->hi2c = hi2c;
	dev->address = address;
	dev->range = ADXL345_RANGE_2G;
	dev->scaleFactor = ADXL345_SCALE_FACTOR_2G;

	if (ADXL345_IsDeviceReady(dev) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	if (ADXL345_ReadDeviceID(dev, &deviceID) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	if (deviceID != ADXL345_DEVICE_ID) {
		return ADXL345_INVALID_ID;
	}

	if (ADXL345_SetStandbyMode(dev) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	if (ADXL345_SetRange(dev, ADXL345_RANGE_4G, 0) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	if (ADXL345_SetDataRate(dev, ADXL345_DATARATE_100HZ, 0) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	if (ADXL345_SetMeasureMode(dev) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	return ADXL345_OK;
}

ADXL345_Status_t ADXL345_SetStandbyMode(ADXL345_Handle_t *dev) {
	uint8_t powerCtl = 0;

	if (ADXL345_ReadRegister(dev, ADXL345_POWER_CTL, &powerCtl, 1)
			!= ADXL345_OK) {
		return ADXL345_ERROR;
	}

	powerCtl &= ~ADXL345_POWER_CTL_MEASURE;

	if (ADXL345_WriteRegister(dev, ADXL345_POWER_CTL, powerCtl) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	return ADXL345_OK;
}

ADXL345_Status_t ADXL345_SetMeasureMode(ADXL345_Handle_t *dev) {
	uint8_t powerCtl = 0;

	if (ADXL345_ReadRegister(dev, ADXL345_POWER_CTL, &powerCtl, 1)
			!= ADXL345_OK) {
		return ADXL345_ERROR;
	}

	powerCtl |= ADXL345_POWER_CTL_MEASURE;

	if (ADXL345_WriteRegister(dev, ADXL345_POWER_CTL, powerCtl) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	return ADXL345_OK;
}

ADXL345_Status_t ADXL345_SetRange(ADXL345_Handle_t *dev, uint8_t range,
		uint8_t fullResolution) {
	uint8_t dataFormat = 0;
	float newScaleFactor = dev->scaleFactor;

	if (dev == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (range > ADXL345_RANGE_16G) {
		return ADXL345_INVALID_PARAM;
	}

	if (ADXL345_ReadRegister(dev, ADXL345_DATA_FORMAT, &dataFormat, 1)
			!= ADXL345_OK) {
		return ADXL345_ERROR;
	}

	dataFormat &= ~ADXL345_DATA_FORMAT_RANGE_MASK;
	dataFormat |= range;

	if (fullResolution) {
		dataFormat |= ADXL345_DATA_FORMAT_FULL_RES;
		dev->scaleFactor = ADXL345_SCALE_FACTOR_FULL_RES;
	} else {
		dataFormat &= ~ADXL345_DATA_FORMAT_FULL_RES;

		switch (range) {
		case ADXL345_RANGE_2G:
			dev->scaleFactor = ADXL345_SCALE_FACTOR_2G;
			break;

		case ADXL345_RANGE_4G:
			dev->scaleFactor = ADXL345_SCALE_FACTOR_4G;
			break;

		case ADXL345_RANGE_8G:
			dev->scaleFactor = ADXL345_SCALE_FACTOR_8G;
			break;

		case ADXL345_RANGE_16G:
			dev->scaleFactor = ADXL345_SCALE_FACTOR_16G;
			break;

		default:
			return ADXL345_INVALID_PARAM;
		}
	}

	if (ADXL345_WriteRegister(dev, ADXL345_DATA_FORMAT, dataFormat)
			!= ADXL345_OK) {
		return ADXL345_ERROR;
	}

	dev->range = range;
	dev->scaleFactor = newScaleFactor;

	return ADXL345_OK;
}

ADXL345_Status_t ADXL345_SetDataRate(ADXL345_Handle_t *dev, uint8_t dataRate,
		uint8_t lowPower) {
	uint8_t bwRate = 0;

	if (dev == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (dataRate > ADXL345_DATARATE_3200HZ) {
		return ADXL345_INVALID_PARAM;
	}

	bwRate = dataRate & ADXL345_BW_RATE_RATE_MASK;

	if (lowPower) {
		bwRate |= ADXL345_BW_RATE_LOW_POWER;
	}

	if (ADXL345_WriteRegister(dev, ADXL345_BW_RATE, bwRate) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	return ADXL345_OK;
}

ADXL345_Status_t ADXL345_ReadRawXYZ(ADXL345_Handle_t *dev,
		ADXL345_RawData_t *rawData) {
	uint8_t data[6] = { 0 };

	if (dev == NULL || rawData == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (ADXL345_ReadRegister(dev, ADXL345_DATAX0, data, 6) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	rawData->x = (int16_t) ((data[1] << 8) | data[0]);
	rawData->y = (int16_t) ((data[3] << 8) | data[2]);
	rawData->z = (int16_t) ((data[5] << 8) | data[4]);

	return ADXL345_OK;
}

ADXL345_Status_t ADXL345_ReadGXYZ(ADXL345_Handle_t *dev, ADXL345_GData_t *gData) {
	ADXL345_RawData_t rawData = { 0 };

	if (dev == NULL || gData == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (ADXL345_ReadRawXYZ(dev, &rawData) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	gData->x = rawData.x * dev->scaleFactor;
	gData->y = rawData.y * dev->scaleFactor;
	gData->z = rawData.z * dev->scaleFactor;

	return ADXL345_OK;
}

ADXL345_Status_t ADXL345_CalculateRollPitch(ADXL345_Handle_t *dev,
		ADXL345_Angle_t *angle) {
	ADXL345_GData_t gData = { 0 };

	if (dev == NULL || angle == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (ADXL345_ReadGXYZ(dev, &gData) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	angle->roll = atan2f(gData.y, gData.z) * 180.0f / 3.14159265f;

	angle->pitch = atan2f(-gData.x,
			sqrtf((gData.y * gData.y) + (gData.z * gData.z))) * 180.0f
			/ 3.14159265f;

	return ADXL345_OK;
}

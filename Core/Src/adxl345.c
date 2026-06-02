/*
 * adxl345.c
 *
 *  Created on: Jun 1, 2026
 *      Author: Eylül Öztek
 */

#include "adxl345.h"
#include <math.h>

/**
 * @file adxl345.c
 * @brief Source file for STM32 HAL based ADXL345 accelerometer driver.
 */

/**
 * @brief Read one or more bytes from an ADXL345 register.
 *
 * This is a private helper function used by the public driver API.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param reg Register address to read from.
 * @param data Pointer to receive buffer.
 * @param length Number of bytes to read.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Write one byte to an ADXL345 register.
 *
 * This is a private helper function used by the public driver API.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param reg Register address to write to.
 * @param value Value to write.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Check whether the ADXL345 responds on the I2C bus.
 *
 * @param dev Pointer to ADXL345 handle.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Read the ADXL345 device ID register.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param deviceID Pointer to variable where the device ID will be stored.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_ReadDeviceID(ADXL345_Handle_t *dev, uint8_t *deviceID) {
	if (dev == NULL || deviceID == NULL) {
		return ADXL345_INVALID_PARAM;
	}
	return ADXL345_ReadRegister(dev, ADXL345_DEVID, deviceID, 1);
}

/**
 * @brief Initialize the ADXL345 device.
 *
 * This function verifies communication with the sensor, checks the fixed
 * device ID, configures default settings, and starts measurement mode.
 *
 * Default configuration:
 * - Range: +/-4g
 * - Full-resolution mode: disabled
 * - Output data rate: 100 Hz
 * - Low-power mode: disabled
 *
 * @param dev Pointer to ADXL345 handle.
 * @param hi2c Pointer to STM32 HAL I2C handle.
 * @param address Shifted I2C address of the ADXL345.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Put the ADXL345 into standby mode.
 *
 * This function clears the MEASURE bit in the POWER_CTL register.
 *
 * @param dev Pointer to ADXL345 handle.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Put the ADXL345 into measurement mode.
 *
 * This function sets the MEASURE bit in the POWER_CTL register.
 *
 * @param dev Pointer to ADXL345 handle.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Configure measurement range and full-resolution mode.
 *
 * This function updates the DATA_FORMAT register and stores the matching
 * scale factor in the ADXL345 handle.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param range Measurement range.
 * @param fullResolution Set to 1 to enable full-resolution mode, 0 to disable.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_SetRange(ADXL345_Handle_t *dev, uint8_t range,
		uint8_t fullResolution) {
	uint8_t dataFormat = 0;
	float newScaleFactor = 0.0f;

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
		newScaleFactor = ADXL345_SCALE_FACTOR_FULL_RES;
	} else {
		dataFormat &= ~ADXL345_DATA_FORMAT_FULL_RES;

		switch (range) {
		case ADXL345_RANGE_2G:
			newScaleFactor = ADXL345_SCALE_FACTOR_2G;
			break;

		case ADXL345_RANGE_4G:
			newScaleFactor = ADXL345_SCALE_FACTOR_4G;
			break;

		case ADXL345_RANGE_8G:
			newScaleFactor = ADXL345_SCALE_FACTOR_8G;
			break;

		case ADXL345_RANGE_16G:
			newScaleFactor = ADXL345_SCALE_FACTOR_16G;
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

/**
 * @brief Configure output data rate and low-power mode.
 *
 * This function writes the selected output data rate code to the BW_RATE
 * register. Low-power mode can also be enabled with the lowPower parameter.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param dataRate Output data rate code.
 * @param lowPower Set to 1 to enable low-power mode, 0 to disable.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Read raw X, Y, and Z acceleration values.
 *
 * This function reads 6 consecutive bytes starting from DATAX0.
 * The ADXL345 stores each axis value as a 16-bit two's complement value.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param rawData Pointer to raw data structure.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Read X, Y, and Z acceleration values in g units.
 *
 * This function reads raw acceleration data and converts it to g units
 * using the active scale factor stored in the ADXL345 handle.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param gData Pointer to g data structure.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Calculate roll and pitch angles from acceleration data.
 *
 * The calculation is based on gravity direction and is suitable for static
 * or slow-moving tilt sensing applications.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param angle Pointer to angle data structure.
 * @return ADXL345_Status_t Operation status.
 */
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

/**
 * @brief Write offset calibration values to the offset registers.
 *
 * Offset values are written to OFSX, OFSY, and OFSZ registers.
 * Negative values are stored in two's complement format.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param xOffset X-axis offset value.
 * @param yOffset Y-axis offset value.
 * @param zOffset Z-axis offset value.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_SetOffset(ADXL345_Handle_t *dev, int8_t xOffset,
		int8_t yOffset, int8_t zOffset) {
	if (dev == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (ADXL345_WriteRegister(dev, ADXL345_OFSX, (uint8_t) xOffset)
			!= ADXL345_OK) {
		return ADXL345_ERROR;
	}

	if (ADXL345_WriteRegister(dev, ADXL345_OFSY, (uint8_t) yOffset)
			!= ADXL345_OK) {
		return ADXL345_ERROR;
	}

	if (ADXL345_WriteRegister(dev, ADXL345_OFSZ, (uint8_t) zOffset)
			!= ADXL345_OK) {
		return ADXL345_ERROR;
	}

	return ADXL345_OK;
}

/**
 * @brief Read offset calibration values from the offset registers.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param xOffset Pointer to variable where X-axis offset will be stored.
 * @param yOffset Pointer to variable where Y-axis offset will be stored.
 * @param zOffset Pointer to variable where Z-axis offset will be stored.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_GetOffset(ADXL345_Handle_t *dev, int8_t *xOffset,
		int8_t *yOffset, int8_t *zOffset) {
	uint8_t data[3] = { 0 };

	if (dev == NULL || xOffset == NULL || yOffset == NULL || zOffset == NULL) {
		return ADXL345_INVALID_PARAM;
	}

	if (ADXL345_ReadRegister(dev, ADXL345_OFSX, data, 3) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	*xOffset = (int8_t) data[0];
	*yOffset = (int8_t) data[1];
	*zOffset = (int8_t) data[2];

	return ADXL345_OK;
}

/**
 * @brief Perform basic offset calibration.
 *
 * This function assumes that the sensor is placed flat and stationary.
 * The expected acceleration is X = 0g, Y = 0g, and Z = +1g.
 *
 * The calibration process:
 * 1. Clears previous offset values.
 * 2. Collects multiple raw acceleration samples.
 * 3. Computes average raw values.
 * 4. Calculates correction values.
 * 5. Writes the calculated offsets to OFSX, OFSY, and OFSZ.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param sampleCount Number of samples to average.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_Calibrate(ADXL345_Handle_t *dev, uint16_t sampleCount) {
	ADXL345_RawData_t rawData = { 0 };

	int32_t sumX = 0;
	int32_t sumY = 0;
	int32_t sumZ = 0;

	int16_t avgX = 0;
	int16_t avgY = 0;
	int16_t avgZ = 0;

	int16_t expectedZ = 0;

	int8_t xOffset = 0;
	int8_t yOffset = 0;
	int8_t zOffset = 0;

	if (dev == NULL || sampleCount == 0) {
		return ADXL345_INVALID_PARAM;
	}

	/*
	 * Offset calibration should start from zero offset.
	 * Otherwise old offset values affect the new calibration result.
	 */
	if (ADXL345_SetOffset(dev, 0, 0, 0) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	HAL_Delay(100);

	for (uint16_t i = 0; i < sampleCount; i++) {
		if (ADXL345_ReadRawXYZ(dev, &rawData) != ADXL345_OK) {
			return ADXL345_ERROR;
		}

		sumX += rawData.x;
		sumY += rawData.y;
		sumZ += rawData.z;

		HAL_Delay(10);
	}

	avgX = (int16_t) (sumX / sampleCount);
	avgY = (int16_t) (sumY / sampleCount);
	avgZ = (int16_t) (sumZ / sampleCount);

	/*
	 * Expected Z value depends on selected scale factor.
	 * For example:
	 * ±2g 10-bit:  1g ≈ 256 raw LSB
	 * ±4g 10-bit:  1g ≈ 128 raw LSB
	 * ±8g 10-bit:  1g ≈ 64 raw LSB
	 * ±16g 10-bit: 1g ≈ 32 raw LSB
	 */
	expectedZ = (int16_t) (1.0f / dev->scaleFactor);

	xOffset = (int8_t) (-avgX / ADXL345_RAW_TO_OFFSET_DIVIDER);
	yOffset = (int8_t) (-avgY / ADXL345_RAW_TO_OFFSET_DIVIDER);
	zOffset = (int8_t) (-(avgZ - expectedZ) / ADXL345_RAW_TO_OFFSET_DIVIDER);

	if (ADXL345_SetOffset(dev, xOffset, yOffset, zOffset) != ADXL345_OK) {
		return ADXL345_ERROR;
	}

	return ADXL345_OK;
}

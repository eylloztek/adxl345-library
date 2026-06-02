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

/**
 * @file adxl345.h
 * @brief STM32 HAL based ADXL345 accelerometer driver.
 *
 * This driver provides basic configuration, raw acceleration reading,
 * g-unit conversion, roll/pitch calculation, and offset calibration
 * functions for the ADXL345 3-axis digital accelerometer over I2C.
 */

/* I2C addresses -------------------------------------------------------------*/

/**
 * @brief ADXL345 I2C address when SDO/ALT ADDRESS pin is connected to GND.
 *
 * STM32 HAL expects the 7-bit I2C address to be shifted left by 1 bit.
 */
#define ADXL345_ADDRESS_LOW        			(0x53 << 1)

/**
 * @brief ADXL345 I2C address when SDO/ALT ADDRESS pin is connected to VDD.
 *
 * STM32 HAL expects the 7-bit I2C address to be shifted left by 1 bit.
 */
#define ADXL345_ADDRESS_HIGH       			(0x1D << 1)

/**
 * @brief Expected fixed device ID value of the ADXL345.
 */
#define ADXL345_DEVICE_ID          			0xE5

/* Register map --------------------------------------------------------------*/

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

/**
 * @brief Default HAL timeout value in milliseconds.
 */
#define ADXL345_TIMEOUT						1000

/* Scale factors -------------------------------------------------------------*/

/**
 * @brief Scale factor used in full-resolution mode.
 *
 * In full-resolution mode, the scale factor remains approximately
 * 3.9 mg/LSB, which is equivalent to 1/256 g per LSB.
 */
#define ADXL345_SCALE_FACTOR_FULL_RES   	(float)1/256.0
#define ADXL345_SCALE_FACTOR_2G 			(float)1/256.0
#define ADXL345_SCALE_FACTOR_4G 			(float)1/128.0
#define ADXL345_SCALE_FACTOR_8G 			(float)1/64.0
#define ADXL345_SCALE_FACTOR_16G 			(float)1/32.0

/**
 * @brief Offset register scale in g/LSB.
 *
 * Each offset register LSB corresponds to approximately 15.6 mg.
 */
#define ADXL345_OFFSET_LSB_SCALE_G      	0.0156f

/**
 * @brief Raw-to-offset conversion divider.
 *
 * One offset LSB corresponds to approximately 4 raw LSB
 * in 10-bit mode / full-resolution 2g-equivalent scale.
 */
#define ADXL345_RAW_TO_OFFSET_DIVIDER   	4.0f

/* Wake-up frequency options -------------------------------------------------*/

#define ADXL345_WAKEUP_8HZ					0x00
#define ADXL345_WAKEUP_4HZ					0x01
#define ADXL345_WAKEUP_2HZ					0x02
#define ADXL345_WAKEUP_1HZ					0x03

/* Measurement range options -------------------------------------------------*/

#define ADXL345_RANGE_2G					0x00
#define ADXL345_RANGE_4G					0x01
#define ADXL345_RANGE_8G					0x02
#define ADXL345_RANGE_16G					0x03

/* Output data rate options --------------------------------------------------*/

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

/* POWER_CTL register bits ---------------------------------------------------*/

#define ADXL345_POWER_CTL_WAKEUP_MASK       0x03
#define ADXL345_POWER_CTL_SLEEP             (1 << 2)
#define ADXL345_POWER_CTL_MEASURE           (1 << 3)
#define ADXL345_POWER_CTL_AUTO_SLEEP        (1 << 4)
#define ADXL345_POWER_CTL_LINK              (1 << 5)

/* DATA_FORMAT register bits -------------------------------------------------*/

#define ADXL345_DATA_FORMAT_RANGE_MASK      0x03
#define ADXL345_DATA_FORMAT_JUSTIFY         (1 << 2)
#define ADXL345_DATA_FORMAT_FULL_RES        (1 << 3)
#define ADXL345_DATA_FORMAT_INT_INVERT      (1 << 5)
#define ADXL345_DATA_FORMAT_SPI             (1 << 6)
#define ADXL345_DATA_FORMAT_SELF_TEST       (1 << 7)

/* BW_RATE register bits -----------------------------------------------------*/

#define ADXL345_BW_RATE_RATE_MASK           0x0F
#define ADXL345_BW_RATE_LOW_POWER           (1 << 4)

/**
 * @brief Driver status return values.
 */
typedef enum {
    ADXL345_OK = 0,             /**< Operation completed successfully. */
    ADXL345_ERROR,              /**< HAL communication or generic driver error. */
    ADXL345_INVALID_ID,         /**< Device ID does not match ADXL345_DEVICE_ID. */
    ADXL345_INVALID_PARAM       /**< Invalid function parameter. */
} ADXL345_Status_t;

/**
 * @brief ADXL345 driver handle structure.
 *
 * This structure stores the I2C handle, device address, selected range,
 * and active scale factor used for raw-to-g conversion.
 */
typedef struct {
    I2C_HandleTypeDef *hi2c;    /**< Pointer to STM32 HAL I2C handle. */
    uint16_t address;           /**< Shifted I2C device address. */
    uint8_t range;              /**< Current measurement range. */
    float scaleFactor;          /**< Active raw-to-g scale factor. */
} ADXL345_Handle_t;

/**
 * @brief Raw acceleration data container.
 */
typedef struct {
    int16_t x;                  /**< Raw X-axis acceleration value. */
    int16_t y;                  /**< Raw Y-axis acceleration value. */
    int16_t z;                  /**< Raw Z-axis acceleration value. */
} ADXL345_RawData_t;

/**
 * @brief Acceleration data in g units.
 */
typedef struct {
    float x;                    /**< X-axis acceleration in g. */
    float y;                    /**< Y-axis acceleration in g. */
    float z;                    /**< Z-axis acceleration in g. */
} ADXL345_GData_t;

/**
 * @brief Roll and pitch angle container.
 */
typedef struct {
    float roll;                 /**< Roll angle in degrees. */
    float pitch;                /**< Pitch angle in degrees. */
} ADXL345_Angle_t;

/**
 * @brief Initialize the ADXL345 device.
 *
 * This function assigns the I2C handle and device address, checks device
 * readiness, verifies the device ID, configures default range and data rate,
 * and enables measurement mode.
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
        uint16_t address);

/**
 * @brief Read the ADXL345 device ID register.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param deviceID Pointer to variable where the device ID will be stored.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_ReadDeviceID(ADXL345_Handle_t *dev, uint8_t *deviceID);

/**
 * @brief Check whether the ADXL345 responds on the I2C bus.
 *
 * @param dev Pointer to ADXL345 handle.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_IsDeviceReady(ADXL345_Handle_t *dev);

/**
 * @brief Put the ADXL345 into standby mode.
 *
 * In standby mode, the device stops measurement but register access remains
 * available. It is recommended to configure device registers in standby mode.
 *
 * @param dev Pointer to ADXL345 handle.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_SetStandbyMode(ADXL345_Handle_t *dev);

/**
 * @brief Put the ADXL345 into measurement mode.
 *
 * @param dev Pointer to ADXL345 handle.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_SetMeasureMode(ADXL345_Handle_t *dev);

/**
 * @brief Configure measurement range and resolution mode.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param range Measurement range. Use ADXL345_RANGE_2G,
 *              ADXL345_RANGE_4G, ADXL345_RANGE_8G, or ADXL345_RANGE_16G.
 * @param fullResolution Set to 1 to enable full-resolution mode, 0 to disable.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_SetRange(ADXL345_Handle_t *dev, uint8_t range,
        uint8_t fullResolution);

/**
 * @brief Configure output data rate and low-power mode.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param dataRate Output data rate code. Use ADXL345_DATARATE_xxx macros.
 * @param lowPower Set to 1 to enable low-power mode, 0 to disable.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_SetDataRate(ADXL345_Handle_t *dev, uint8_t dataRate,
        uint8_t lowPower);

/**
 * @brief Read raw X, Y, and Z acceleration values.
 *
 * This function performs a 6-byte burst read starting from DATAX0.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param rawData Pointer to raw data structure.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_ReadRawXYZ(ADXL345_Handle_t *dev,
        ADXL345_RawData_t *rawData);

/**
 * @brief Read X, Y, and Z acceleration values in g units.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param gData Pointer to g data structure.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_ReadGXYZ(ADXL345_Handle_t *dev, ADXL345_GData_t *gData);

/**
 * @brief Calculate roll and pitch angles from acceleration data.
 *
 * This calculation is suitable for static or slow-moving tilt sensing.
 * Fast motion, vibration, or shock may distort the calculated angles.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param angle Pointer to angle data structure.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_CalculateRollPitch(ADXL345_Handle_t *dev,
        ADXL345_Angle_t *angle);

/**
 * @brief Write offset calibration values to OFSX, OFSY, and OFSZ registers.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param xOffset X-axis offset value.
 * @param yOffset Y-axis offset value.
 * @param zOffset Z-axis offset value.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_SetOffset(ADXL345_Handle_t *dev, int8_t xOffset,
        int8_t yOffset, int8_t zOffset);

/**
 * @brief Read offset calibration values from OFSX, OFSY, and OFSZ registers.
 *
 * @param dev Pointer to ADXL345 handle.
 * @param xOffset Pointer to variable where X-axis offset will be stored.
 * @param yOffset Pointer to variable where Y-axis offset will be stored.
 * @param zOffset Pointer to variable where Z-axis offset will be stored.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_GetOffset(ADXL345_Handle_t *dev, int8_t *xOffset,
        int8_t *yOffset, int8_t *zOffset);

/**
 * @brief Perform basic offset calibration.
 *
 * The sensor must remain flat and stationary during calibration.
 * This function assumes the expected acceleration is:
 * - X = 0g
 * - Y = 0g
 * - Z = +1g
 *
 * @param dev Pointer to ADXL345 handle.
 * @param sampleCount Number of samples used for averaging.
 * @return ADXL345_Status_t Operation status.
 */
ADXL345_Status_t ADXL345_Calibrate(ADXL345_Handle_t *dev, uint16_t sampleCount);

#endif /* INC_ADXL345_H_ */

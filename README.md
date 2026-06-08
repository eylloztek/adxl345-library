# ADXL345 STM32 HAL Driver

A lightweight STM32 HAL-based driver for the **ADXL345 3-axis digital accelerometer** using the I2C interface.

This library provides basic device initialization, range and data rate configuration, raw acceleration reading, g-unit conversion, roll/pitch angle calculation, and basic offset calibration. You can access the datasheet from [here.](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl345.pdf)

## Features

* STM32 HAL I2C support
* Device ID verification
* Standby and measurement mode control
* Selectable measurement range:

  * ±2g
  * ±4g
  * ±8g
  * ±16g
* Selectable output data rate
* Raw X/Y/Z acceleration reading
* Acceleration conversion to g units
* Roll and pitch angle calculation
* Manual offset read/write
* Basic automatic offset calibration
* Simple handle-based driver structure

## Hardware

This library is designed for the **ADXL345 digital accelerometer** connected to an STM32 microcontroller over I2C.

### ADXL345 I2C Address Options

The ADXL345 has two possible I2C addresses depending on the state of the `SDO/ALT ADDRESS` pin.

| SDO/ALT ADDRESS Pin | 7-bit I2C Address | STM32 HAL Address |
| ------------------- | ----------------: | ----------------: |
| GND                 |            `0x53` |            `0xA6` |
| VDD                 |            `0x1D` |            `0x3A` |

In this library, the STM32 HAL-compatible shifted addresses are defined as:

```c
#define ADXL345_ADDRESS_LOW   (0x53 << 1)
#define ADXL345_ADDRESS_HIGH  (0x1D << 1)
```

## Pin Connection

Example wiring for STM32 I2C usage:

| ADXL345 Pin     | STM32 Connection |
| --------------- | ---------------- |
| VCC             | 3.3V             |
| GND             | GND              |
| SDA             | I2C SDA          |
| SCL             | I2C SCL          |
| CS              | 3.3V             |
| SDO/ALT ADDRESS | GND or 3.3V      |

> `CS` must be connected to 3.3V for I2C mode.

## Files

```text
adxl345.h
adxl345.c
```

## Public API

### Initialization and Device Check

```c
ADXL345_Status_t ADXL345_Init(
    ADXL345_Handle_t *dev,
    I2C_HandleTypeDef *hi2c,
    uint16_t address
);
```

Initializes the ADXL345 driver, verifies the device ID, configures the default settings, and enables measurement mode.

Default configuration:

| Setting              | Default Value    |
| -------------------- | ---------------- |
| Range                | ±4g              |
| Full-resolution mode | Disabled         |
| Output data rate     | 100 Hz           |
| Low-power mode       | Disabled         |
| Mode                 | Measurement mode |

```c
ADXL345_Status_t ADXL345_IsDeviceReady(ADXL345_Handle_t *dev);
ADXL345_Status_t ADXL345_ReadDeviceID(ADXL345_Handle_t *dev, uint8_t *deviceID);
```

---

### Power Mode Control

```c
ADXL345_Status_t ADXL345_SetStandbyMode(ADXL345_Handle_t *dev);
ADXL345_Status_t ADXL345_SetMeasureMode(ADXL345_Handle_t *dev);
```

The device can be placed into standby mode before configuration and then switched back to measurement mode.

---

### Range Configuration

```c
ADXL345_Status_t ADXL345_SetRange(
    ADXL345_Handle_t *dev,
    uint8_t range,
    uint8_t fullResolution
);
```

Supported range values:

```c
ADXL345_RANGE_2G
ADXL345_RANGE_4G
ADXL345_RANGE_8G
ADXL345_RANGE_16G
```

Example:

```c
ADXL345_SetRange(&adxl345, ADXL345_RANGE_4G, 0);
```

If `fullResolution` is set to `1`, full-resolution mode is enabled.

---

### Output Data Rate Configuration

```c
ADXL345_Status_t ADXL345_SetDataRate(
    ADXL345_Handle_t *dev,
    uint8_t dataRate,
    uint8_t lowPower
);
```

Example:

```c
ADXL345_SetDataRate(&adxl345, ADXL345_DATARATE_100HZ, 0);
```

Available data rate macros:

```c
ADXL345_DATARATE_3200HZ
ADXL345_DATARATE_1600HZ
ADXL345_DATARATE_800HZ
ADXL345_DATARATE_400HZ
ADXL345_DATARATE_200HZ
ADXL345_DATARATE_100HZ
ADXL345_DATARATE_50HZ
ADXL345_DATARATE_25HZ
ADXL345_DATARATE_12_5HZ
ADXL345_DATARATE_6_25HZ
ADXL345_DATARATE_3_13HZ
ADXL345_DATARATE_1_56HZ
ADXL345_DATARATE_0_78HZ
ADXL345_DATARATE_0_39HZ
ADXL345_DATARATE_0_20HZ
ADXL345_DATARATE_0_10HZ
```

---

### Raw Acceleration Reading

```c
ADXL345_Status_t ADXL345_ReadRawXYZ(
    ADXL345_Handle_t *dev,
    ADXL345_RawData_t *rawData
);
```

Example:

```c
ADXL345_RawData_t rawData;

if (ADXL345_ReadRawXYZ(&adxl345, &rawData) == ADXL345_OK) {
    // rawData.x
    // rawData.y
    // rawData.z
}
```

---

### Acceleration Reading in g

```c
ADXL345_Status_t ADXL345_ReadGXYZ(
    ADXL345_Handle_t *dev,
    ADXL345_GData_t *gData
);
```

Example:

```c
ADXL345_GData_t gData;

if (ADXL345_ReadGXYZ(&adxl345, &gData) == ADXL345_OK) {
    // gData.x
    // gData.y
    // gData.z
}
```

---

### Roll and Pitch Calculation

```c
ADXL345_Status_t ADXL345_CalculateRollPitch(
    ADXL345_Handle_t *dev,
    ADXL345_Angle_t *angle
);
```

Example:

```c
ADXL345_Angle_t angle;

if (ADXL345_CalculateRollPitch(&adxl345, &angle) == ADXL345_OK) {
    // angle.roll
    // angle.pitch
}
```

> Roll and pitch calculation is based on gravity direction. It is suitable for static or slow-moving tilt sensing. Fast motion, vibration, or shock may distort the calculated angle values.

---

### Offset Calibration

Manual offset write:

```c
ADXL345_Status_t ADXL345_SetOffset(
    ADXL345_Handle_t *dev,
    int8_t xOffset,
    int8_t yOffset,
    int8_t zOffset
);
```

Offset read:

```c
ADXL345_Status_t ADXL345_GetOffset(
    ADXL345_Handle_t *dev,
    int8_t *xOffset,
    int8_t *yOffset,
    int8_t *zOffset
);
```

Basic automatic calibration:

```c
ADXL345_Status_t ADXL345_Calibrate(
    ADXL345_Handle_t *dev,
    uint16_t sampleCount
);
```

The calibration function assumes that the sensor is placed flat and stationary.

Expected acceleration during calibration:

```text
X = 0g
Y = 0g
Z = +1g
```

Example:

```c
ADXL345_Calibrate(&adxl345, 100);
```

## Basic Usage Example

```c
#include "adxl345.h"

ADXL345_Handle_t adxl345;
ADXL345_Status_t adxlStatus;

ADXL345_RawData_t rawData;
ADXL345_GData_t gData;
ADXL345_Angle_t angle;

int8_t offsetX = 0;
int8_t offsetY = 0;
int8_t offsetZ = 0;

int main(void)
{
    HAL_Init();

    /*
     * SystemClock_Config();
     * MX_GPIO_Init();
     * MX_I2C1_Init();
     */

    adxlStatus = ADXL345_Init(&adxl345, &hi2c1, ADXL345_ADDRESS_LOW);

    if (adxlStatus == ADXL345_OK) {
        ADXL345_Calibrate(&adxl345, 100);
        ADXL345_GetOffset(&adxl345, &offsetX, &offsetY, &offsetZ);
    }

    while (1) {
        if (ADXL345_ReadRawXYZ(&adxl345, &rawData) == ADXL345_OK) {
            /*
             * rawData.x
             * rawData.y
             * rawData.z
             */
        }

        if (ADXL345_ReadGXYZ(&adxl345, &gData) == ADXL345_OK) {
            /*
             * gData.x
             * gData.y
             * gData.z
             */
        }

        if (ADXL345_CalculateRollPitch(&adxl345, &angle) == ADXL345_OK) {
            /*
             * angle.roll
             * angle.pitch
             */
        }

        HAL_Delay(100);
    }
}
```

## Data Structures

### ADXL345 Handle

```c
typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint16_t address;
    uint8_t range;
    float scaleFactor;
} ADXL345_Handle_t;
```

### Raw Data

```c
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} ADXL345_RawData_t;
```

### g Data

```c
typedef struct {
    float x;
    float y;
    float z;
} ADXL345_GData_t;
```

### Angle Data

```c
typedef struct {
    float roll;
    float pitch;
} ADXL345_Angle_t;
```

## Status Codes

```c
typedef enum {
    ADXL345_OK = 0,
    ADXL345_ERROR,
    ADXL345_INVALID_ID,
    ADXL345_INVALID_PARAM
} ADXL345_Status_t;
```

| Status                  | Description                                      |
| ----------------------- | ------------------------------------------------ |
| `ADXL345_OK`            | Operation completed successfully                 |
| `ADXL345_ERROR`         | HAL communication or generic driver error        |
| `ADXL345_INVALID_ID`    | Device ID does not match the expected ADXL345 ID |
| `ADXL345_INVALID_PARAM` | Invalid function parameter                       |

## Notes

* This library currently supports I2C communication.
* STM32 HAL expects the ADXL345 I2C address to be shifted left by 1 bit.
* The default initialization uses ±4g range and 100 Hz output data rate.
* The calibration function assumes that the sensor is flat and stationary.
* Roll and pitch values are reliable mainly for static or slow-moving tilt sensing.

## Math Library Note

`ADXL345_CalculateRollPitch()` uses functions from `math.h`.

If you get a linker error such as:

```text
undefined reference to `sqrt`
```

link the math library.

For GCC-based toolchains, add:

```text
-lm
```

In STM32CubeIDE, this can usually be added from:

```text
Project Properties
C/C++ Build
Settings
MCU GCC Linker
Libraries
```

Add:

```text
m
```

## Possible Future Improvements

* Single tap and double tap detection
* Activity and inactivity detection
* Free-fall detection
* Interrupt configuration
* FIFO support
* SPI support
* Self-test function

## License

This project is licensed under the MIT License. See the [LICENSE](https://github.com/eylloztek/adxl345-library/blob/master/LICENSE.txt) file for details.

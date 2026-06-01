/*
 * adxl345.c
 *
 *  Created on: Jun 1, 2026
 *      Author: Eylül Öztek
 */

#include "adxl345.h"

extern I2C_HandleTypeDef hi2c1;

int ADXL345_ScanDeviceID(){
    for (uint8_t address = 0; address < 255; address++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, address, 1, TIMEOUT) == HAL_OK){
            return address;
        }
    }

    return -1;
}

ADXL345ReadStatus ADXL345_ReadRegisterData(uint16_t registerAddress, uint16_t sizeofData, uint8_t *dataBuffer){
	if (HAL_I2C_Mem_Read(&hi2c1, ADXL345_DEVICE_ADDRESS, registerAddress, 1, dataBuffer, sizeofData, TIMEOUT) == HAL_OK) {
		return READ_SUCCESS;
	}

	return READ_FAIL;

}

ADXL345InitStatus ADXL345_Init(void){
	uint8_t dataBuffer = 0;
	ADXL345_ReadRegisterData(DEVID, 1, &dataBuffer);

	if (dataBuffer != 0xE5) {
		return INIT_FAIL;
	}

	return INIT_SUCCESS;
}

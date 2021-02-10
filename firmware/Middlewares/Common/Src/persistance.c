//
// Created by Olukelo on 02.02.2021.
//
#include "common_resources.h"
#include "global_settings.h"
#include "string.h"

#define PROFILE_DATA_ADDRESS        0x80C0000
#define CALIBRATION_DATA_ADDRESS    0x80E0000

extern CalibrationCurveTypeDef calibrationCurve;

void _writeData(uint32_t sector, uint64_t address, uint8_t *data, uint16_t size) {
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
    FLASH_Erase_Sector(sector, VOLTAGE_RANGE_3);
    for (int i = 0; i < size; ++i) {
        HAL_FLASH_Program(TYPEPROGRAM_BYTE, address, data[i]);
        address++;
    }
    HAL_FLASH_Lock();
}

void loadCalibrationCurve() {
    memcpy((uint8_t *) &calibrationCurve, (uint8_t *) CALIBRATION_DATA_ADDRESS, sizeof(CalibrationCurveTypeDef));
}

void saveCalibrationCurve() {
    _writeData(FLASH_SECTOR_11, CALIBRATION_DATA_ADDRESS, (uint8_t *) &calibrationCurve, sizeof(CalibrationCurveTypeDef));
}

void loadSettings() {
    memcpy((uint8_t *) &globalSettings, (uint8_t *) PROFILE_DATA_ADDRESS, sizeof(GlobalSettingsTypeDef));
}

void saveSettings() {
    _writeData(FLASH_SECTOR_10, PROFILE_DATA_ADDRESS, (uint8_t *) &globalSettings, sizeof(GlobalSettingsTypeDef));
}
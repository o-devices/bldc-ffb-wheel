//
// Created by Olukelo on 01.08.2020.
//

#include "as504x.h"
#include "circular_buffer.h"
#include "common_math.h"
#include "dwt_delay.h"
#include "global_settings.h"

#define VALUE_MASK          0b0011111111111111
#define ANGLE_COMMAND       (uint16_t)(0xFFFF)
#define SENSOR_SMOOTHING    50

const uint16_t _angleCommand = ANGLE_COMMAND;

void InitPositionSensor(As504xPositionSensorTypeDef *sensor) {
    HAL_GPIO_WritePin(sensor->Port, sensor->Pin, GPIO_PIN_SET);
    HAL_Delay(10);
    for (int i = 0; i < POSITION_SENSOR_READINGS_BUFFER_SIZE; ++i) {
        UpdatePositionSensor(sensor);
    }
    sensor->RotationCount = 0;
}

void UpdatePositionSensor(As504xPositionSensorTypeDef *sensor) {
    uint16_t value = 0;
    HAL_GPIO_WritePin(sensor->Port, sensor->Pin, GPIO_PIN_RESET);
    DWT_Delay(1);
    HAL_SPI_TransmitReceive(sensor->Spi, (uint8_t *) (&_angleCommand), (uint8_t *) (&value), 1, 1);
    HAL_GPIO_WritePin(sensor->Port, sensor->Pin, GPIO_PIN_SET);
    DWT_Delay(1);
    value = value & VALUE_MASK;
    if (value != 0 && value != VALUE_MASK) {
        if ((int16_t) value - sensor->Position > ((int32_t) VALUE_MASK) / 2) {
            sensor->RotationCount = sensor->RotationCount - 1;
        } else if ((int16_t) value - sensor->Position < -((int32_t) VALUE_MASK) / 2) {
            sensor->RotationCount = sensor->RotationCount + 1;
        }
        sensor->Position = (int16_t) value;
        AddBufferValue(sensor->Buffer, sensor->Position + (int16_t) VALUE_MASK * sensor->RotationCount);
    }
    sensor->AbsolutePosition = Lerp(sensor->AbsolutePosition, GetBufferMedian(sensor->Buffer), SENSOR_SMOOTHING);
}
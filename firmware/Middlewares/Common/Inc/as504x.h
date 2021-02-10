//
// Created by Olukelo on 31.07.2020.
//

#ifndef H_AS504x_H
#define H_AS504x_H

#include "circular_buffer.h"
#include "spi.h"
#include "stdint.h"

typedef struct {
    SPI_HandleTypeDef *Spi;
    GPIO_TypeDef *Port;
    uint16_t Pin;
    CircularBufferTypeDef *Buffer;
    int32_t Position;
    int32_t RotationCount;
    int32_t AbsolutePosition;
} As504xPositionSensorTypeDef;

void InitPositionSensor(As504xPositionSensorTypeDef *sensor);

void UpdatePositionSensor(As504xPositionSensorTypeDef *sensor);

#endif //H_AS504x_H

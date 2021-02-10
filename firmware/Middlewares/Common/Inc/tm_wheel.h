//
// Created by Olukelo on 02.11.2020.
//

#ifndef H_TM_WHEEL_H
#define H_TM_WHEEL_H

#include "spi.h"
#include "stdint.h"

typedef struct {
    SPI_HandleTypeDef *Spi;
    GPIO_TypeDef *Port;
    uint16_t Pin;
    uint32_t Value;
} TmWheelTypeDef;

void InitTmWheel(TmWheelTypeDef *wheel);

void UpdateTmWheel(TmWheelTypeDef *wheel);

#endif //H_TM_WHEEL_H

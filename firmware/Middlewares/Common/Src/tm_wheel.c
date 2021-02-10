//
// Created by Olukelo on 02.11.2020.
//

#include "dwt_delay.h"
#include "tm_wheel.h"

#define VALUE_MASK      0b11111111111111111111111111111000

void InitTmWheel(TmWheelTypeDef *wheel){
    HAL_GPIO_WritePin(wheel->Port, wheel->Pin, GPIO_PIN_SET);
}

void UpdateTmWheel(TmWheelTypeDef *wheel){
    uint32_t value;
    HAL_GPIO_WritePin(wheel->Port, wheel->Pin, GPIO_PIN_RESET);
    DWT_Delay(1);
    HAL_SPI_Receive(wheel->Spi, (uint8_t *) (&value), 2, 1000);
    wheel->Value = (~value) & VALUE_MASK;
    HAL_GPIO_WritePin(wheel->Port, wheel->Pin, GPIO_PIN_SET);
    DWT_Delay(1);
}

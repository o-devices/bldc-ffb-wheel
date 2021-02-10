//
// Created by Olukelo on 12.09.2020.
//

#ifndef H_CIRCULAR_BUFFER_H
#define H_CIRCULAR_BUFFER_H

#include "stdint.h"

typedef struct {
    int32_t *Values;
    uint8_t Size;
    uint8_t Index;
} CircularBufferTypeDef;

void AddBufferValue(CircularBufferTypeDef *buffer, int32_t value);

int32_t GetBufferMedian(CircularBufferTypeDef *buffer);

#endif //H_CIRCULAR_BUFFER_H

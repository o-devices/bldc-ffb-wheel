//
// Created by Olukelo on 12.09.2020.
//

#include "circular_buffer.h"
#include "common_math.h"
#include "string.h"

void AddBufferValue(CircularBufferTypeDef *buffer, int32_t value) {
    buffer->Index = buffer->Index + 1;
    if (buffer->Index > buffer->Size) {
        buffer->Index = 0;
    }
    buffer->Values[buffer->Index] = value;
}

int32_t GetBufferMedian(CircularBufferTypeDef *buffer) {
    int32_t temporary[buffer->Size];
    memcpy(temporary, buffer->Values, buffer->Size * 4);
    for (int offset = 0; offset < buffer->Size; ++offset) {
        uint8_t stopIndex = buffer->Size - offset - 1;
        uint8_t maxIndex = 0;
        for (int i = 0; i < stopIndex; ++i) {
            if (temporary[i] >= temporary[maxIndex]) { maxIndex = i; }
        }
        Swap(&temporary[maxIndex], &temporary[stopIndex]);
    }
    return (temporary[buffer->Size / 2] + temporary[buffer->Size / 2 + 1]) / 2;
}
//
// Created by olukelo on 12.06.2020.
//

#ifndef H_COMMON_MATH_H
#define H_COMMON_MATH_H

#include "stdint.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define Clamp(value,min,max) ((value) > (max) ? (max) : ((value) < (min) ? (min) : (value)))

uint16_t NormalizeAngle(int32_t angle);

int32_t Sinus(int32_t angle);

int32_t  Lerp(int32_t previous, int32_t next, uint8_t smoothing);

int16_t SinusPhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum);

int16_t SquarePhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum);

int16_t TrianglePhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum);

int16_t SawtoothUpPhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum);

int16_t SawtoothDownPhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum);

void Swap(int32_t *a, int32_t *b);

#endif //H_COMMON_MATH_H

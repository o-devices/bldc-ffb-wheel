//
// Created by olukelo on 12.06.2020.
//

#include "common_math.h"
#include "generic_commands.h"

const int16_t sinusCurve[37] = {
    0, 1736, 3420, 5000, 6428, 7660,
    8660, 9397, 9848, 10000, 9848, 9397,
    8660, 7660, 6428, 5000, 3420, 1736,
    0, -1736, -3420, -5000, -6428, -7660,
    -8660, -9397, -9848, -10000, -9848, -9397,
    -8660, -7660, -6428, -5000, -3420, -1736,
    0,
};

uint16_t NormalizeAngle(int32_t angle) {
    angle = angle % 360;
    if (angle < 0) {
        angle = angle + 360;
    }
    return angle;
}

int32_t Sinus(int32_t angle) {
    angle = NormalizeAngle(angle);
    int16_t index = (angle / 10) % 36;
    int16_t delta = angle % 10;
    return sinusCurve[index] + (sinusCurve[index + 1] - sinusCurve[index]) * delta / 10;
}

int32_t Lerp(int32_t previous, int32_t next, uint8_t smoothing) {
    return next * (100 - smoothing) / 100 + previous * smoothing / 100;
}

int16_t SinusPhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum) {
    return magnitude * (Sinus(360 * phase / phaseMaximum) * magnitudeMaximum / LOGICAL_MAXIMUM) / magnitudeMaximum;
}

int16_t SquarePhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum) {
    return magnitude * (phase < (phaseMaximum / 2 + 1) ? magnitudeMaximum : -magnitudeMaximum) / magnitudeMaximum;
}

int16_t TrianglePhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum) {
    return magnitude * (-magnitudeMaximum + (phase <= (phaseMaximum / 2 + 1) ? phase : (phaseMaximum - phase)) * magnitudeMaximum * 4 / phaseMaximum) / magnitudeMaximum;
}

int16_t SawtoothUpPhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum) {
    return magnitude * (-magnitudeMaximum + phase * magnitudeMaximum * 2 / phaseMaximum) / magnitudeMaximum;
}

int16_t SawtoothDownPhaseMagnitude(uint16_t phase, uint16_t phaseMaximum, int16_t magnitude, int16_t magnitudeMaximum) {
    return magnitude * (magnitudeMaximum - phase * magnitudeMaximum * 2 / phaseMaximum) / magnitudeMaximum;
}

void Swap(int32_t *a, int32_t *b) {
    int32_t temp = *a;
    *a = *b;
    *b = temp;
}
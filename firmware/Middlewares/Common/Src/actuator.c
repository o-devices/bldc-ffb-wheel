//
// Created by Olukelo on 25.07.2020.
//

#include "actuator.h"
#include "common_resources.h"
#include "common_math.h"
#include "global_settings.h"
#include "stdlib.h"

#define PHASE_A 0
#define PHASE_B 1
#define PHASE_C 2

void _safetyCritical(MotorTypeDef *motor, void (*volatile operation)(MotorTypeDef *)) {
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    (operation)(motor);
    __set_PRIMASK(mask);
}

void _applyMotorTimings(MotorTypeDef *motor) {
    motor->Timer->Instance->CCR1 = motor->State->Timings[PHASE_A];
    motor->Timer->Instance->CCR2 = motor->State->Timings[PHASE_B];
    motor->Timer->Instance->CCR3 = motor->State->Timings[PHASE_C];
}

void ApplyMotorAngleAndMagnitude(MotorTypeDef *motor, uint16_t angle, uint16_t magnitude, uint16_t maximumMagnitude) {
    magnitude = Clamp(magnitude, 0, maximumMagnitude);
    int32_t alpha = Sinus(angle) * magnitude / LOGICAL_MAXIMUM;
    int32_t beta = Sinus(90 - angle) * magnitude / LOGICAL_MAXIMUM;
    ApplySvm(motor, alpha, beta);
}

uint8_t _inSegment(int16_t startValue, int16_t endValue, int16_t position, int16_t direction) {
    if (direction == 1) {
        return position > startValue && position < endValue;
    }
    if (direction == -1) {
        return position < startValue && position > endValue;
    }
    return false;
}

void ApplyMotorTorque(MotorTypeDef *motor, int32_t torque, uint16_t maximumMagnitude) {
    int32_t _magnitude = abs(torque);
    if (_magnitude > 0) {
        int8_t direction = torque > 0 ? 1 : -1;
        uint8_t index = 0;
        do {
            int16_t startValue = calibrationCurve.Values[index];
            int16_t endValue = calibrationCurve.Values[index + 1];
            if (_inSegment(startValue, endValue, motor->State->Position, calibrationCurve.Direction)) {
                int16_t delta = (startValue - endValue) * calibrationCurve.Direction;
                int16_t phaseAngle = index * 90 + (startValue - motor->State->Position) * 90 / delta;
                int i = phaseAngle + 90 * direction;
                uint16_t angle = NormalizeAngle(i);
                ApplyMotorAngleAndMagnitude(motor, angle, _magnitude, maximumMagnitude);
                return;
            }
            index++;
        } while (index < calibrationCurve.Size - 1);
    }
}

uint8_t _getSvmSector(int32_t alpha, int32_t beta) {
    if (beta >= 0) {
        if (alpha >= 0) {
            if (ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM > alpha) {
                return 2;
            } else {
                return 1;
            }

        } else {
            if (-ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM > alpha) {
                return 3;
            } else {
                return 2;
            }
        }
    } else {
        if (alpha >= 0) {
            if (-ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM > alpha) {
                return 5;
            } else {
                return 6;
            }
        } else {
            if (ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM > alpha) {
                return 4;
            } else {
                return 5;
            }
        }
    }
}

void ApplySvm(MotorTypeDef *motor, int32_t alpha, int32_t beta) {
    int32_t t1;
    int32_t t2;
    uint32_t A = 0;
    uint32_t B = 0;
    uint32_t C = 0;
    uint8_t sector = _getSvmSector(alpha, beta);
    alpha = alpha * SQRT3_BY_2 / LOGICAL_MAXIMUM;
    beta = beta * SQRT3_BY_2 / LOGICAL_MAXIMUM;
    switch (sector) {
        case 1: {
            t1 = alpha - ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            t2 = TWO_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            A = (LOGICAL_MAXIMUM - t1 - t2) / 2;
            B = A + t1;
            C = B + t2;
        }
            break;
        case 2: {
            t1 = alpha + ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            t2 = -alpha + ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            B = (LOGICAL_MAXIMUM - t1 - t2) / 2;
            A = B + t2;
            C = A + t1;
        }
            break;
        case 3: {
            t1 = TWO_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            t2 = -alpha - ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            B = (LOGICAL_MAXIMUM - t1 - t2) / 2;
            C = B + t1;
            A = C + t2;
        }
            break;
        case 4: {
            t1 = -alpha + ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            t2 = -TWO_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            C = (LOGICAL_MAXIMUM - t1 - t2) / 2;
            B = C + t2;
            A = B + t1;
        }
            break;
        case 5: {
            t1 = -alpha - ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            t2 = alpha - ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            C = (LOGICAL_MAXIMUM - t1 - t2) / 2;
            A = C + t1;
            B = A + t2;
        }
            break;
        case 6: {
            t1 = -TWO_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            t2 = alpha + ONE_BY_SQRT3 * beta / LOGICAL_MAXIMUM;
            A = (LOGICAL_MAXIMUM - t1 - t2) / 2;
            C = A + t2;
            B = C + t1;
        }
            break;
    }
    TIM_HandleTypeDef *timer = motor->Timer;
    motor->State->Timings[0] = Clamp(A, 0, LOGICAL_MAXIMUM) * timer->Init.Period / LOGICAL_MAXIMUM;
    motor->State->Timings[1] = Clamp(B, 0, LOGICAL_MAXIMUM) * timer->Init.Period / LOGICAL_MAXIMUM;
    motor->State->Timings[2] = Clamp(C, 0, LOGICAL_MAXIMUM) * timer->Init.Period / LOGICAL_MAXIMUM;
    _safetyCritical(motor, &_applyMotorTimings);
}

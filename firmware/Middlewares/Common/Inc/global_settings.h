//
// Created by olukelo on 16.07.2020.
//

#ifndef FFB_JOYSTICK_GLOBAL_SETTINGS_H
#define FFB_JOYSTICK_GLOBAL_SETTINGS_H

#include "stdint.h"
#include "effects_constants.h"

#define POSITION_SENSOR_READINGS_BUFFER_SIZE    6       // With value 8 it is enough time for 2 axis smooth reading in fast update

typedef struct __attribute__((packed)) {
    uint8_t EnableConstantViaSine;           // Enables possibility to handle zero magnitude sine effect as constant force. Used in R3E. Disable for other games
    uint8_t UpdateDurationOnEffectReport;    // Do not stop effect if time elapsed. Update started at on every packet. R3E does not provide proper duration for effects. // TODO: Investigate R3E
    uint16_t SteeringRange;                  // Encoder ticks. Limits manipulator output and soft stops.
    uint8_t SteeringSmoothing;               // 0..100. Output smoothing.
    int16_t TorqueGain;                      // 0..100. Scales total output. Negative values to invert torque direction
    uint8_t TorqueSmoothing;                 // 0..100. Combined torque smoothing
    int16_t EffectGains[MAX_EFFECTS + 1];    // Use negative values to invert individual Common effect torque. 0 to disable. value 100 will scale by 1.
    int8_t Direction;                       // -1/+1. Stands if positive direction of rotation corresponds to increasing encoder values
    uint16_t MechanicalCenter;               // Offset from encoder Z index to mechanical center in encoder ticks.
    uint16_t EncoderCPR;
    uint16_t CalibrationMagnitude;
} GlobalSettingsTypeDef;

typedef struct {
    int8_t Direction;
    uint16_t Size;
    int16_t Values[128];
} CalibrationCurveTypeDef;

#endif //FFB_JOYSTICK_GLOBAL_SETTINGS_H

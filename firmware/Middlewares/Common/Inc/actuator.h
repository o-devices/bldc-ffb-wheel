//
// Created by olukelo on 18.06.2020.
//

#ifndef H_ACTUATOR_H
#define H_ACTUATOR_H

#include "effects_constants.h"
#include "global_settings.h"
#include "stdint.h"
#include "stdbool.h"
#include "tim.h"

typedef struct {
    uint16_t Position;
    uint32_t Timings[3];
} MotorStateTypeDef;

typedef struct {
    TIM_HandleTypeDef *Timer;
    MotorStateTypeDef *State;
} MotorTypeDef;

typedef struct {
    uint32_t LastUpdateTick;
    int32_t CurrentSpeed;
    int32_t CurrentPosition;
    int32_t TargetTorque;
    int16_t AdcPhaseReadings[3];
} ActuatorStateTypeDef;

typedef struct _ActuatorTypeDef {
    ActuatorStateTypeDef *State;
    MotorTypeDef *Motor;
} ActuatorTypeDef;

void EnableActuator(ActuatorTypeDef *actuator);

void DisableActuator(ActuatorTypeDef *actuator);

void ApplyMotorTorque(MotorTypeDef *motor, int32_t torque, uint16_t maximumMagnitude);

void ApplyMotorAngleAndMagnitude(MotorTypeDef *motor, uint16_t angle, uint16_t magnitude, uint16_t maximumMagnitude);

void ApplySvm(MotorTypeDef *motor, int32_t alpha, int32_t beta);

#endif //H_ACTUATOR_H

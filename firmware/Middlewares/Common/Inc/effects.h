//
// Created by olukelo on 17.04.2020.
//

#ifndef H_EFFECTS_H
#define H_EFFECTS_H

#include "actuator.h"
#include "effects_constants.h"
#include "stdint.h"

typedef struct {
    volatile uint8_t Type;
    volatile uint8_t State;
    volatile uint8_t Gain;
    volatile uint16_t Duration;
    volatile uint16_t LoopCount;
    volatile uint32_t StartedAt;
    volatile int32_t Torque;
} EffectTypeDef;

typedef struct {
    EffectTypeDef Effect;
    volatile int16_t Magnitude;
    volatile int16_t Offset;
    volatile uint16_t Period;
    volatile uint16_t CurrentPhase;
    volatile uint32_t LastUpdateTick;
} PeriodicForceEffectTypeDef;

typedef struct {
    EffectTypeDef Effect;
    volatile int16_t Magnitude;
} ConstantForceEffectTypeDef;

typedef struct {
    EffectTypeDef Effect;
    volatile int16_t MagnitudeStart;
    volatile int16_t MagnitudeEnd;
} RampForceEffectTypeDef;

typedef struct {
    EffectTypeDef Effect;
    volatile int16_t CenterOffset;
    volatile int16_t PositiveCoefficient;
    volatile int16_t NegativeCoefficient;
} ConditionForceEffectTypeDef;

uint8_t *Effects[MAX_EFFECTS + 1];

int32_t CalculateEffectsTorque();

void UpdateEffects(uint32_t tick);

#endif //H_EFFECTS_H

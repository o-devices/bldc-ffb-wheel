//
// Created by olukelo on 18.04.2020.
//

#include "common_math.h"
#include "common_resources.h"
#include "effects.h"
#include "effects_constants.h"

// All effects are allocated at program start not to deal with allocation/reallocation in runtime
ConstantForceEffectTypeDef constantViaSineEffect = {.Effect.Type = EFFECT_CONSTANT_VIA_SINE};
ConstantForceEffectTypeDef constantEffect = {.Effect.Type = EFFECT_CONSTANT};
RampForceEffectTypeDef rampEffect = {.Effect.Type = EFFECT_RAMP};
PeriodicForceEffectTypeDef squareEffect = {.Effect.Type = EFFECT_SQUARE};
PeriodicForceEffectTypeDef sineEffect = {.Effect.Type = EFFECT_SINE};
PeriodicForceEffectTypeDef triangleEffect = {.Effect.Type = EFFECT_TRIANGLE};
PeriodicForceEffectTypeDef sawtoothUpEffect = {.Effect.Type = EFFECT_SAWTOOTH_UP};
PeriodicForceEffectTypeDef sawtoothDownEffect = {.Effect.Type = EFFECT_SAWTOOTH_DOWN};
ConditionForceEffectTypeDef springEffect = {.Effect.Type = EFFECT_SPRING};
ConditionForceEffectTypeDef damperEffect = {.Effect.Type = EFFECT_DAMPER};

uint8_t *Effects[MAX_EFFECTS + 1] = {
    (uint8_t *) &constantViaSineEffect,
    (uint8_t *) &constantEffect,
    (uint8_t *) &rampEffect,
    (uint8_t *) &squareEffect,
    (uint8_t *) &sineEffect,
    (uint8_t *) &triangleEffect,
    (uint8_t *) &sawtoothUpEffect,
    (uint8_t *) &sawtoothDownEffect,
    (uint8_t *) &springEffect,
    (uint8_t *) &damperEffect,
};

uint8_t _effectIsPlaying(uint32_t elapsedTime, EffectTypeDef *e) {
    return e->State == STATE_PLAYING && ((elapsedTime < e->Duration * e->LoopCount) || (e->Duration >= DURATION_INFINITE) || (e->Duration == 0));
}

void UpdatePeriodic(uint32_t tick, PeriodicForceEffectTypeDef *e, int16_t (*periodicMagnitude)(uint16_t, uint16_t, int16_t, int16_t)) {
    if (tick > e->LastUpdateTick) {
        if (_effectIsPlaying(tick - e->Effect.StartedAt, &e->Effect) && e->Period != 0) {
            e->CurrentPhase = (e->CurrentPhase + INT16_MAX * ((tick - e->LastUpdateTick) % (e->Period + 1)) / e->Period) % INT16_MAX;
            e->Effect.Torque = periodicMagnitude(e->CurrentPhase, INT16_MAX, e->Magnitude, LOGICAL_MAXIMUM) + e->Offset;
        } else {
            e->CurrentPhase = 0;
            e->Effect.Torque = 0;
        }
        e->LastUpdateTick = tick;
    }
}

void _updateConstantForce(uint32_t tick, ConstantForceEffectTypeDef *e) {
    if (_effectIsPlaying(tick - e->Effect.StartedAt, &e->Effect)) {
        e->Effect.Torque = e->Magnitude;
    } else {
        e->Effect.Torque = 0;
    }
}

void _updateRamp(uint32_t tick, RampForceEffectTypeDef *e) {
    uint32_t elapsed = tick - e->Effect.StartedAt;
    if (_effectIsPlaying(elapsed, &e->Effect) && e->Effect.Duration != 0) {
        e->Effect.Torque = e->MagnitudeStart + (e->MagnitudeEnd - e->MagnitudeStart) * (elapsed % (e->Effect.Duration + 1)) / e->Effect.Duration;
    } else {
        e->Effect.Torque = 0;
    }
}

void _updateSpring(uint32_t tick, ConditionForceEffectTypeDef *e) {
    if (_effectIsPlaying(tick - e->Effect.StartedAt, &e->Effect)) {
        int32_t steeringRange = globalSettings.SteeringRange * globalSettings.EncoderCPR / 360 / 2;
        int32_t input = actuator.State->CurrentPosition * LOGICAL_MAXIMUM / steeringRange - e->CenterOffset;
        int32_t coefficient = input > 0 ? e->PositiveCoefficient : e->NegativeCoefficient;
        e->Effect.Torque = -(input * coefficient / LOGICAL_MAXIMUM);
    } else {
        e->Effect.Torque = 0;
    }
}

void _updateIndividual(uint32_t tick, uint8_t *e) {
    EffectTypeDef *effect = (EffectTypeDef *) e;
    switch (effect->Type) {
        case EFFECT_CONSTANT_VIA_SINE:
        case EFFECT_CONSTANT:
            _updateConstantForce(tick, (ConstantForceEffectTypeDef *) e);
            break;
        case EFFECT_RAMP:
            _updateRamp(tick, (RampForceEffectTypeDef *) e);
            break;
        case EFFECT_SQUARE:
            UpdatePeriodic(tick, (PeriodicForceEffectTypeDef *) e, SquarePhaseMagnitude);
            break;
        case EFFECT_SINE:
            UpdatePeriodic(tick, (PeriodicForceEffectTypeDef *) e, SinusPhaseMagnitude);
            break;
        case EFFECT_TRIANGLE:
            UpdatePeriodic(tick, (PeriodicForceEffectTypeDef *) e, TrianglePhaseMagnitude);
            break;
        case EFFECT_SAWTOOTH_UP:
            UpdatePeriodic(tick, (PeriodicForceEffectTypeDef *) e, SawtoothUpPhaseMagnitude);
            break;
        case EFFECT_SAWTOOTH_DOWN:
            UpdatePeriodic(tick, (PeriodicForceEffectTypeDef *) e, SawtoothDownPhaseMagnitude);
            break;
        case EFFECT_SPRING:
            _updateSpring(tick, (ConditionForceEffectTypeDef *) e);
            break;
    }
}

void UpdateEffects(uint32_t tick) {
    for (int i = 0; i <= MAX_EFFECTS; i++) {
        _updateIndividual(tick, Effects[i]);
    }
}

int32_t CalculateEffectsTorque() {
    int32_t effectsTorque = 0;
    for (int effectId = 0; effectId <= MAX_EFFECTS; effectId++) {
        EffectTypeDef *effect = (EffectTypeDef *) Effects[effectId];
        if (effect->State != STATE_NONE) {
            effectsTorque += effect->Torque * effect->Gain / UINT8_MAX * globalSettings.EffectGains[effect->Type] / 100;
        }
    }
    return effectsTorque;
}

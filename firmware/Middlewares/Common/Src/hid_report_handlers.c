//
// Created by olukelo on 18.04.2020.
//

#include "common_resources.h"
#include "effects.h"
#include "generic_commands.h"
#include "hid_report_handlers.h"
#include "usbd_ioreq.h"
#include "usb_reports.h"

extern PidBlockLoadReportTypeDef pidBlockLoadReport;
extern PidPoolReportTypeDef pidPoolReport;

void HandleDeviceGainReport(PidDeviceGainReportTypeDef *report) {
    //IGNORE FOR NOW. Relay on custom gains.
}

void HandleSetEnvelopeReport(uint8_t *buffer) {
    //IGNORE FOR NOW
}

void HandleSetEffectReport(uint32_t tick, SetEffectReportTypeDef *report) {
    EffectTypeDef *effect = (EffectTypeDef *) Effects[report->EffectBlockIndex];
    effect->Duration = report->Duration;
    effect->Gain = report->Gain;
    effect->StartedAt = tick;
    if (effect->Type == EFFECT_SINE && globalSettings.EnableConstantViaSine) {
        EffectTypeDef *constantViaSineEffect = (EffectTypeDef *) Effects[EFFECT_CONSTANT_VIA_SINE];
        constantViaSineEffect->Duration = report->Duration;
        constantViaSineEffect->Gain = report->Gain;
        constantViaSineEffect->StartedAt = tick;
    }
}

void HandleSetPeriodicReport(uint32_t tick, SetPeriodicReportTypeDef *report) {
    PeriodicForceEffectTypeDef *effect = (PeriodicForceEffectTypeDef *) Effects[report->EffectBlockIndex];
    if (effect->Effect.Type == EFFECT_SINE && globalSettings.EnableConstantViaSine && report->Magnitude == 0) {
        ConstantForceEffectTypeDef *constantViaSineEffect = (ConstantForceEffectTypeDef *) Effects[EFFECT_CONSTANT_VIA_SINE];
        constantViaSineEffect->Magnitude = report->CenterOffset;
        if (globalSettings.UpdateDurationOnEffectReport) {
            constantViaSineEffect->Effect.StartedAt = tick;
        }
    } else {
        effect->Magnitude = report->Magnitude;
        effect->Offset = report->CenterOffset;
        effect->Period = report->Period;
        if (globalSettings.UpdateDurationOnEffectReport) {
            effect->Effect.StartedAt = tick;
        }
    }
}

void HandleSetConditionReport(uint32_t tick, SetConditionReportTypeDef *report) {
    ConditionForceEffectTypeDef *effect = (ConditionForceEffectTypeDef *) Effects[report->EffectBlockIndex];
    if (report->ParameterBlockOffset == 0) {
        effect->CenterOffset = report->CenterOffset;
        effect->PositiveCoefficient = report->PositiveCoefficient;
        effect->NegativeCoefficient = report->NegativeCoefficient;
    }
    if (globalSettings.UpdateDurationOnEffectReport) {
        effect->Effect.StartedAt = tick;
    }
}

void HandleSetConstantForceReport(uint32_t tick, SetConstantForceReportTypeDef *report) {
    ConstantForceEffectTypeDef *effect = (ConstantForceEffectTypeDef *) Effects[report->EffectBlockIndex];
    effect->Magnitude = report->Magnitude;
    if (globalSettings.UpdateDurationOnEffectReport) {
        effect->Effect.StartedAt = tick;
    }
}

void HandleSetRampForceReport(uint32_t tick, SetRampForceReportTypeDef *report) {
    RampForceEffectTypeDef *effect = (RampForceEffectTypeDef *) Effects[report->EffectBlockIndex];
    effect->MagnitudeStart = report->EffectStart;
    effect->MagnitudeEnd = report->EffectEnd;
    if (globalSettings.UpdateDurationOnEffectReport) {
        effect->Effect.StartedAt = tick;
    }
}

void AllocateEffect(uint8_t effectId) {
    EffectTypeDef *effect = (EffectTypeDef *) Effects[effectId];
    effect->State = STATE_ALLOCATED;
    effect->Gain = 0;
    effect->Duration = 0;
    effect->LoopCount = 0;
    effect->StartedAt = 0;
    effect->Torque = 0;
}

void StartEffectWithLoopCount(uint32_t tick, uint8_t effectId, uint8_t loopCount) {
    if (effectId <= MAX_EFFECTS) {
        EffectTypeDef *effect = (EffectTypeDef *) Effects[effectId];
        effect->State = STATE_PLAYING;
        effect->LoopCount = loopCount;
        effect->StartedAt = tick;
        if (effectId == EFFECT_SINE && (globalSettings.EnableConstantViaSine)) {
            EffectTypeDef *constantViaSineEffect = (EffectTypeDef *) Effects[EFFECT_CONSTANT_VIA_SINE];
            constantViaSineEffect->State = STATE_PLAYING;
            constantViaSineEffect->LoopCount = loopCount;
            constantViaSineEffect->StartedAt = tick;
        }
    }
}

void StopEffect(uint8_t effectId) {
    if (effectId <= MAX_EFFECTS) {
        EffectTypeDef *effect = (EffectTypeDef *) Effects[effectId];
        effect->State = STATE_STOPPED;
        effect->Torque = 0;
        if (effectId == EFFECT_SINE && globalSettings.EnableConstantViaSine) {
            EffectTypeDef *constantViaSineEffect = (EffectTypeDef *) Effects[EFFECT_CONSTANT_VIA_SINE];
            constantViaSineEffect->State = STATE_STOPPED;
            constantViaSineEffect->Torque = 0;
        }
    }
}

void StopAllEffect() {
    for (uint8_t i = 1; i <= MAX_EFFECTS; i++) {
        StopEffect(i);
    }
}

void FreeEffect(uint8_t effectId) {
    if (effectId <= MAX_EFFECTS) {
        EffectTypeDef *effect = (EffectTypeDef *) Effects[effectId];
        effect->State = STATE_NONE;
        effect->Gain = 0;
        effect->Duration = 0;
        effect->LoopCount = 0;
        effect->StartedAt = 0;
        effect->Torque = 0;
    }
}

void FreeAllEffects() {
    for (uint8_t i = 1; i <= MAX_EFFECTS; i++) {
        FreeEffect(i);
    }
}

void PidControlEnableActuators() {
    // TODO:implement
}

void PidControlDisableActuators() {
    // TODO:implement
}

void PidControlStopAll() {
    StopAllEffect();
}

void PidControlReset() {
    FreeAllEffects();
}

void PidControlPause() {
    // TODO:implement
}

void PidControlContinue() {
    // TODO:implement
}

void HandleEffectOperationReport(EffectOperationReportTypeDef *report) {
    switch (report->EffectOperation) {
        case OPERATION_START:
            StartEffectWithLoopCount(HAL_GetTick(), report->EffectBlockIndex, report->LoopCount);
            break;
        case OPERATION_START_SOLO:
            StopAllEffect();
            StartEffectWithLoopCount(HAL_GetTick(), report->EffectBlockIndex, report->LoopCount);
            break;
        case OPERATION_STOP:
            StopEffect(report->EffectBlockIndex);
            break;
        default:
            break;
    }
}

void HandlePidDeviceControlReport(PidDeviceControlReportTypeDef *report) {
    switch (report->Control) {
        case CONTROL_ENABLE_ACTUATORS:
            PidControlEnableActuators();
            break;
        case CONTROL_DISABLE_ACTUATORS:
            PidControlDisableActuators();
            break;
        case CONTROL_STOP_ALL:
            PidControlStopAll();
            break;
        case CONTROL_RESET:
            PidControlReset();
            break;
        case CONTROL_PAUSE:
            PidControlPause();
            break;
        case CONTROL_CONTINUE:
            PidControlContinue();
            break;
        default:
            break;
    }
}

void HandlePidBlockFreeReport(PidBlockFreeReportTypeDef *report) {
    if (report->EffectBlockIndex == 0xFF) {
        FreeAllEffects();
    } else {
        FreeEffect(report->EffectBlockIndex);
    }
}

void HandleCreateNewEffectReport(CreateNewEffectReportTypeDef *report) {
    uint8_t effectType = report->EffectType;
    switch (effectType) {
        case EFFECT_CONSTANT:
        case EFFECT_RAMP:
        case EFFECT_SQUARE:
        case EFFECT_SINE:
        case EFFECT_TRIANGLE:
        case EFFECT_SAWTOOTH_UP:
        case EFFECT_SAWTOOTH_DOWN:
        case EFFECT_SPRING:
        case EFFECT_DAMPER:
            AllocateEffect(effectType);
            pidBlockLoadReport.EffectBlockIndex = effectType;
            pidBlockLoadReport.BlockLoadStatus = BLOCK_LOAD_STATUS_SUCCESS;
            break;
        default:
            pidBlockLoadReport.EffectBlockIndex = EFFECT_UNSUPPORTED;
            pidBlockLoadReport.BlockLoadStatus = BLOCK_LOAD_STATUS_ERROR;
            break;
    }
    if (effectType == EFFECT_SINE && globalSettings.EnableConstantViaSine) {
        AllocateEffect(EFFECT_CONSTANT_VIA_SINE);
    }
}

void CustomEP0RxReady(USBD_HandleTypeDef *pDef) {
    GenericReportTypeDef *report = (GenericReportTypeDef *) pDef->pUserData;
    switch (report->ReportId) {
        case REPORT_CREATE_NEW_EFFECT:
            HandleCreateNewEffectReport((CreateNewEffectReportTypeDef *) report->ReportData);
            break;
        case REPORT_PID_POOL:
            HandlePidPoolReportRequest(pDef);
            break;
        case REPORT_GENERIC:
            HandleGenericReport((GenericReportTypeDef *) report->ReportData);
            break;
        default:
            break;
    }
}

void CustomUsbDataOut(USBD_HandleTypeDef *pDef) {
    GenericReportTypeDef *report = (GenericReportTypeDef *) pDef->pUserData;
    switch (report->ReportId) {
        case REPORT_SET_EFFECT:
            HandleSetEffectReport(HAL_GetTick(), (SetEffectReportTypeDef *) report->ReportData);
            break;
        case REPORT_SET_ENVELOPE:
            HandleSetEnvelopeReport(report->ReportData);
            break;
        case REPORT_SET_PERIODIC:
            HandleSetPeriodicReport(HAL_GetTick(), (SetPeriodicReportTypeDef *) report->ReportData);
            break;
        case REPORT_SET_CONDITION:
            HandleSetConditionReport(HAL_GetTick(), (SetConditionReportTypeDef *) report->ReportData);
            break;
        case REPORT_SET_CONSTANT_FORCE:
            HandleSetConstantForceReport(HAL_GetTick(), (SetConstantForceReportTypeDef *) report->ReportData);
            break;
        case REPORT_SET_RAMP_FORCE:
            HandleSetRampForceReport(HAL_GetTick(), (SetRampForceReportTypeDef *) report->ReportData);
            break;
        case REPORT_EFFECT_OPERATION:
            HandleEffectOperationReport((EffectOperationReportTypeDef *) report->ReportData);
            break;
        case REPORT_PID_DEVICE_CONTROL:
            HandlePidDeviceControlReport((PidDeviceControlReportTypeDef *) report->ReportData);
            break;
        case REPORT_PID_BLOCK_FREE:
            HandlePidBlockFreeReport((PidBlockFreeReportTypeDef *) report->ReportData);
            break;
        case REPORT_DEVICE_GAIN:
            HandleDeviceGainReport((PidDeviceGainReportTypeDef *) report->ReportData);
            break;
        default:
            break;
    }
}

void HandlePidBlockLoadReportRequest(USBD_HandleTypeDef *pDef) {
    USBD_CtlSendData(pDef, (uint8_t *) &pidBlockLoadReport, sizeof(pidBlockLoadReport));
}

void HandlePidPoolReportRequest(USBD_HandleTypeDef *pDef) {
    USBD_CtlSendData(pDef, (uint8_t *) &pidPoolReport, sizeof(pidPoolReport));
}

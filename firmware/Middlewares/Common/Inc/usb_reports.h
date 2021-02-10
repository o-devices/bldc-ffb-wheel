//
// Created by olukelo on 17.04.2020.
//

#ifndef FFB_JOYSTICK_USB_REPORTS_H
#define FFB_JOYSTICK_USB_REPORTS_H

#include "stdint.h"

typedef struct __attribute__((packed)) {
    uint8_t ReportId;
    uint8_t ReportData[63];
} GenericReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t EffectType;             // 1..20
    uint16_t ByteCount;             // 0..511
} CreateNewEffectReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t EffectBlockIndex;       // 1..20
    uint8_t EffectOperation;        // 1..3
    uint8_t LoopCount;              // 0..255
} EffectOperationReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t EffectBlockIndex;       // 1..20
} PidBlockFreeReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t ReportId;
    uint8_t EffectBlockIndex;
    uint8_t BlockLoadStatus;
    uint16_t RamPoolAvailable;
} PidBlockLoadReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t Control;                // 1..6
} PidDeviceControlReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t Gain;                   // 0..255
} PidDeviceGainReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t ReportId;
    uint16_t RamPoolSize;           // 0..65535
    uint8_t MaxEffects;             // 0..255
    uint8_t MemoryManagement;       // Bits: 0=DeviceManagedPool, 1=SharedParameterBlocks
} PidPoolReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t ReportId;
    uint8_t Status;                 // Bits: 0=ODrive Paused, 1=Actuators Enabled, 2=Safety Switch, 3=Actuator Override Switch, 4=Actuator Power
    uint8_t EffectBlockIndex;       // Bit7=Effect Playing, Bit0..7=EffectId
} PidStateReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t EffectBlockIndex;       // 1..20
    uint8_t ParameterBlockOffset;   // 0..1
    int16_t CenterOffset;           // -10000..10000
    int16_t PositiveCoefficient;    // -10000..10000
    int16_t NegativeCoefficient;    // -10000..10000
    uint16_t PositiveSaturation;    // 0..10000
    uint16_t NegativeSaturation;    // 0..10000
    int16_t DeadBand;               // -10000..10000
} SetConditionReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t EffectBlockIndex;       // 1..20
    int16_t Magnitude;              // -10000..10000
} SetConstantForceReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t EffectBlockIndex;       // 1..20
    uint8_t EffectType;             // 1..12
    uint16_t Duration;              // 0..32767 ms
    uint16_t TriggerRepeatInterval; // 0..32767 ms
    uint16_t SamplePeriod;          // 0..32767 ms
    uint8_t Gain;                   // 0..255 (physical 0..10000)
    uint8_t TriggerButton;          // button ID (0..8)
    uint8_t AxisEnable;             // bits: 0=X, 1=Y, 2=DirectionEnable
    uint8_t Direction[2];           // angle (0=0 .. 255=360deg)
} SetEffectReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t EffectBlockIndex;       // 1..20
    uint16_t AttackLevel;           // 0..10000
    uint16_t FadeLevel;             // 0..10000
    uint16_t AttackTime;            // 0..32767 ms
    uint16_t FadeTime;              // 0..32767 ms
} SetEnvelopeReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t EffectBlockIndex;       // 1..20
    uint16_t Magnitude;             // 0..10000
    int16_t CenterOffset;           // -10000..10000
    uint16_t Phase;                 // 0..180
    uint16_t Period;                // 0..32767
} SetPeriodicReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t EffectBlockIndex;       // 1..20
    int16_t EffectStart;            // -10000..10000
    int16_t EffectEnd;              // -10000..10000
} SetRampForceReportTypeDef;

typedef struct __attribute__((packed)) {
    uint8_t ReportId;
    uint8_t Buttons[4];
    uint16_t X;
    uint16_t Y;
} JoystickReportTypeDef;

#endif //FFB_JOYSTICK_USB_REPORTS_H

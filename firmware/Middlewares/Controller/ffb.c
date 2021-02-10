//
// Created by olukelo on 21.03.2020.
//

#include "adc.h"
#include "as504x.h"
#include "common_math.h"
#include "common_resources.h"
#include "effects.h"
#include "dwt_delay.h"
#include "generic_commands.h"
#include "persistance.h"
#include "usbd_customhid.h"
#include "tm_wheel.h"

volatile uint16_t adcRegular[3] = {0, 0, 0};

volatile bool _enterFastUpdateRate = false;
volatile bool _enterSlowUpdateRate = false;

volatile uint32_t _lastTick;

void (*volatile UpdateOperation)(uint32_t tick);

void UpdateDevice() {
    if (UpdateOperation != NULL) {
        (UpdateOperation)(HAL_GetTick());
    }
}

void _doFastUpdate() {
    UpdatePositionSensor(&positionSensor);
    actuator.Motor->State->Position = positionSensor.Position;
    ApplyMotorTorque(actuator.Motor, actuator.State->TargetTorque, LOGICAL_MAXIMUM);
}

int32_t _getTorque() {
    return Clamp(CalculateEffectsTorque(actuator), -LOGICAL_MAXIMUM, LOGICAL_MAXIMUM) * globalSettings.TorqueGain / 100;
}

void _doTickUpdate(uint32_t tick) {
    UpdateEffects(tick);
    int32_t input = (positionSensor.AbsolutePosition - globalSettings.MechanicalCenter) * globalSettings.Direction;
    actuator.State->CurrentSpeed = (input - actuator.State->CurrentPosition) / (tick - _lastTick);
    actuator.State->CurrentPosition = input;
    actuator.State->TargetTorque = Lerp(actuator.State->TargetTorque, _getTorque(), globalSettings.TorqueSmoothing);
}

int32_t _positionInput() {
    int32_t steeringRange = globalSettings.SteeringRange * (int32_t )globalSettings.EncoderCPR / 360 / 2;
    return (Clamp(actuator.State->CurrentPosition, -steeringRange, steeringRange) + steeringRange) * INT16_MAX / steeringRange / 2;
}

void _sendJoystickReport() {
    joystickReport.X = Lerp(joystickReport.X, _positionInput(), globalSettings.SteeringSmoothing);
    joystickReport.Y = INT16_MAX / 2;
    memcpy(joystickReport.Buttons, &tmWheel.Value, 4);
    UsbSendReport(usbDevice, (uint8_t *) &joystickReport, 9);
}

void _doNormalUpdateOperation(uint32_t tick) {
    if (_enterFastUpdateRate) {
        _doFastUpdate();
        _enterFastUpdateRate = false;
    }
    if (tick > _lastTick) {
        _doTickUpdate(tick);
        _lastTick = tick;
    }
    if (_enterSlowUpdateRate) {
        UpdateTmWheel(&tmWheel);
        _sendJoystickReport();
        _enterSlowUpdateRate = false;
    }
}

void _startTimers() {
    // Set to neutral.
    htim1.Instance->CCR1 = TIM_BASE_PERIOD / 2;
    htim1.Instance->CCR2 = TIM_BASE_PERIOD / 2;
    htim1.Instance->CCR3 = TIM_BASE_PERIOD / 2;

    // Set ADC trigger channels duty cycles back to normal.
    htim1.Instance->CCR4 = ADC_TRIGGER_DUTY_CYCLE;

    // Start PWM timers.
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    // Start timer for update flag triggering.
    HAL_TIM_Base_Start_IT(fastUpdateRateTimer);
    HAL_TIM_Base_Start_IT(sowUpdateRateTimer);
}

void _start_Sensors() {
    InitPositionSensor(&positionSensor);
}

void _startAdc() {
    //Start ADC in triple mode.
    HAL_ADCEx_InjectedStart(&hadc3);
    HAL_ADCEx_InjectedStart(&hadc2);
    HAL_ADCEx_InjectedStart_IT(&hadc1);
    HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t *) &adcRegular, 3);
}

void _startActuator() {
    EnableActuator(&actuator);
    HAL_GPIO_WritePin(MOTOR_EN_GPIO_Port, MOTOR_EN_Pin, GPIO_PIN_SET);
}

void StartDevice() {
    loadSettings();
    loadCalibrationCurve();
    InitTmWheel(&tmWheel);
    DWT_Init();
    _start_Sensors();
    _startAdc();
    _startTimers();
    _startActuator();

    _lastTick = HAL_GetTick();
    UpdateOperation = &_doNormalUpdateOperation;
}

void PeriodElapsedCallback(TIM_HandleTypeDef *timer) {
    if (timer->Instance == fastUpdateRateTimer->Instance) {
        _enterFastUpdateRate = true;
    }
    if (timer->Instance == sowUpdateRateTimer->Instance) {
        _enterSlowUpdateRate = true;
    }
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc) {
    ActuatorStateTypeDef *state = actuator.State;
    state->AdcPhaseReadings[0] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
    state->AdcPhaseReadings[1] = HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1);
    state->AdcPhaseReadings[2] = HAL_ADCEx_InjectedGetValue(&hadc3, ADC_INJECTED_RANK_1);
}

void EnableActuator(ActuatorTypeDef *actuator) {
    HAL_GPIO_WritePin(MOTOR_EN_GPIO_Port, MOTOR_EN_Pin, GPIO_PIN_SET);
}

void DisableActuator(ActuatorTypeDef *actuator) {
    HAL_GPIO_WritePin(MOTOR_EN_GPIO_Port, MOTOR_EN_Pin, GPIO_PIN_RESET);
}

uint8_t condition(uint8_t zeroPassed, uint16_t start, uint16_t value) {
    return zeroPassed && value > start;
}

void _updatePosition(uint8_t samples) {
    for (int i = 0; i < samples; ++i) {
        UpdatePositionSensor(&positionSensor);
        HAL_Delay(10);
    }
}

void _calibrateMechanicalCenter(uint32_t tick) {
    for (int i = 0; i < 16; ++i) {
            UpdatePositionSensor(&positionSensor);
            HAL_Delay(1);
    }
    UpdateOperation = &_doNormalUpdateOperation;
}

void StartMechanicalCenterCalibration() {
    UpdateOperation = &_calibrateMechanicalCenter;
}

void _calibrateCurve(uint32_t tick) {
    int16_t curve[128] = {0};
    uint16_t start;
    uint16_t end;

    for (int magnitude = 0; magnitude < globalSettings.CalibrationMagnitude; ++magnitude) {
        ApplyMotorAngleAndMagnitude(actuator.Motor, 0, magnitude, LOGICAL_MAXIMUM);
        HAL_Delay(1);
    }
    HAL_Delay(100);
    _updatePosition(POSITION_SENSOR_READINGS_BUFFER_SIZE);
    do {
        start = positionSensor.Position;
        for (int i = 0; i >= -360; --i) {
            ApplyMotorAngleAndMagnitude(actuator.Motor, i, globalSettings.CalibrationMagnitude, LOGICAL_MAXIMUM);
            HAL_Delay(10);
        }
        _updatePosition(POSITION_SENSOR_READINGS_BUFFER_SIZE);
        end = positionSensor.Position;
    } while (start > end);

    for (int i1 = 0; i1 >= -45; --i1) {
        ApplyMotorAngleAndMagnitude(actuator.Motor, i1, globalSettings.CalibrationMagnitude, LOGICAL_MAXIMUM);
        HAL_Delay(10);
    }
    _updatePosition(POSITION_SENSOR_READINGS_BUFFER_SIZE);
    start = positionSensor.Position;

    for (int i = -45; i <= 0; ++i) {
        ApplyMotorAngleAndMagnitude(actuator.Motor, i, globalSettings.CalibrationMagnitude, LOGICAL_MAXIMUM);
        HAL_Delay(10);
    }

    uint8_t zeroPassed = 0;
    uint8_t fullRotation = 0;
    uint8_t index = 0;

    do {
        _updatePosition(POSITION_SENSOR_READINGS_BUFFER_SIZE);
        curve[index] = positionSensor.Position;
        if (index > 0 && curve[index] < curve[0]) {
            zeroPassed = 1;
        }
        for (int a = 0; a <= 90; ++a) {
            ApplyMotorAngleAndMagnitude(actuator.Motor, a + a * index, globalSettings.CalibrationMagnitude, LOGICAL_MAXIMUM);
            HAL_Delay(10);
        }
        if (zeroPassed && curve[index] > start) {
            fullRotation = 1;
            curve[index] = curve[0];
            curve[index + 1] = curve[1];
            curve[0] = curve[0] - globalSettings.EncoderCPR;
            calibrationCurve.Size = index + 1;
            memcpy(calibrationCurve.Values, curve, sizeof(curve));
        }
        index++;
    } while (!fullRotation);
    UpdateOperation = &_doNormalUpdateOperation;
}

void StartPolesCurveCalibration() {
    UpdateOperation = &_calibrateCurve;
}

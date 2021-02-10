//
// Created by olukelo on 26.05.2020.
//

#include "actuator.h"
#include "as504x.h"
#include "global_settings.h"
#include "tim.h"
#include "tm_wheel.h"
#include "usbd_def.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

USBD_HandleTypeDef *usbDevice = &hUsbDeviceFS;
TIM_HandleTypeDef *fastUpdateRateTimer = &htim6;
TIM_HandleTypeDef *sowUpdateRateTimer = &htim7;

GlobalSettingsTypeDef globalSettings = {};
CalibrationCurveTypeDef calibrationCurve = {};

MotorStateTypeDef _motorState = {};

MotorTypeDef _motor1 = {
    .Timer = &htim1,
    .State =&_motorState,
};

ActuatorStateTypeDef _actuatorState = {};

ActuatorTypeDef actuator = {
    .Motor=&_motor1,
    .State = &_actuatorState,
};

int32_t _sensorBuffer1[POSITION_SENSOR_READINGS_BUFFER_SIZE];

CircularBufferTypeDef _circularBuffer1 = {
    .Values=_sensorBuffer1,
    .Size=POSITION_SENSOR_READINGS_BUFFER_SIZE,
    .Index=0,
};

As504xPositionSensorTypeDef positionSensor = {
    .Spi=&hspi1,
    .Port = SPI1_nCS_GPIO_Port,
    .Pin = SPI1_nCS_Pin,
    .Buffer = &_circularBuffer1,
};

TmWheelTypeDef tmWheel = {
    .Port = SPI3_nCS_GPIO_Port,
    .Pin=SPI3_nCS_Pin,
    .Spi=&hspi3,
};

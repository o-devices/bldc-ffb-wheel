//
// Created by Olukelo on 19.08.2020.
//

#ifndef H_COMMON_RESOURCES_H
#define H_COMMON_RESOURCES_H

#include "actuator.h"
#include "as504x.h"
#include "global_settings.h"
#include "tm_wheel.h"
#include "usbd_def.h"
#include "usb_reports.h"

ActuatorTypeDef actuator;
GlobalSettingsTypeDef globalSettings;
USBD_HandleTypeDef *usbDevice;
TIM_HandleTypeDef *fastUpdateRateTimer;
TIM_HandleTypeDef *sowUpdateRateTimer;
CalibrationCurveTypeDef calibrationCurve;
JoystickReportTypeDef joystickReport;
As504xPositionSensorTypeDef positionSensor;
TmWheelTypeDef tmWheel;

void StartDevice();

void UpdateDevice();

void PeriodElapsedCallback(TIM_HandleTypeDef *timer);

#endif //H_COMMON_RESOURCES_H

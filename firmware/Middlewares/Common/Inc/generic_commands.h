//
// Created by olukelo on 20.06.2020.
//

#ifndef FFB_JOYSTICK_GENERIC_COMMANDS_H
#define FFB_JOYSTICK_GENERIC_COMMANDS_H

#define REPORT_GET_COMMON_SETTINGS          0x01
#define REPORT_SET_COMMON_SETTINGS          0x02
#define REPORT_CALIBRATE_POLES_CURVE   0x04
#define REPORT_CALIBRATE_MECHANICAL_CENTER  0x05
#define REPORT_SAVE_CALIBRATION_CURVE        0x06
#define REPORT_SAVE_SETTINGS                0x07

#include "stdint.h"
#include "effects_constants.h"
#include "usb_reports.h"

void PrepareSettingsReport();

void ApplySettings(GenericReportTypeDef *report);

void StartPolesCurveCalibration();

void StartMechanicalCenterCalibration();

#endif //FFB_JOYSTICK_GENERIC_COMMANDS_H

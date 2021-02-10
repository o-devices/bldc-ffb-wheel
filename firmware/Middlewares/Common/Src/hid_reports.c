//
// Created by Olukelo on 24.10.2020.
//

#include "common_resources.h"

PidBlockLoadReportTypeDef pidBlockLoadReport = {
    .ReportId = REPORT_PID_BLOCK_LOAD,
    .RamPoolAvailable=0xFFFF
};

PidPoolReportTypeDef pidPoolReport = {
    .ReportId = REPORT_PID_POOL,
    .RamPoolSize = 0xFFFF,
    .MaxEffects = MAX_EFFECTS,
    .MemoryManagement = 0x1
};

GenericReportTypeDef genericReport = {
};

JoystickReportTypeDef joystickReport = {
    .ReportId=REPORT_JOYSTICK,
    .Buttons[0]=0x00,
    .Buttons[1]=0x00,
    .Buttons[2]=0x00,
    .Buttons[3]=0x00,
    .X=0x0000,
    .Y=0x0000
};
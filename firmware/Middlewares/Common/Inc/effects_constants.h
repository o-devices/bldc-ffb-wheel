//
// Created by olukelo on 18.04.2020.
//

#ifndef H_CONSTANTS_H
#define H_CONSTANTS_H

#define REPORT_JOYSTICK                 0x01
#define REPORT_GENERIC                  0x1F
#define REPORT_CREATE_NEW_EFFECT        0x11
#define REPORT_PID_BLOCK_LOAD           0x12
#define REPORT_PID_POOL                 0x13
#define REPORT_SET_EFFECT               0x11
#define REPORT_SET_ENVELOPE             0x12
#define REPORT_SET_CONDITION            0x13
#define REPORT_SET_PERIODIC             0x14
#define REPORT_SET_CONSTANT_FORCE       0x15
#define REPORT_SET_RAMP_FORCE           0x16
#define REPORT_EFFECT_OPERATION         0x1A
#define REPORT_PID_STATE                0x12
#define REPORT_PID_BLOCK_FREE           0x1B
#define REPORT_PID_DEVICE_CONTROL       0x1C
#define REPORT_DEVICE_GAIN              0x1D

#define BLOCK_LOAD_STATUS_SUCCESS       0x01
#define BLOCK_LOAD_STATUS_ERROR         0x03

#define EFFECT_UNSUPPORTED              0x00
#define EFFECT_CONSTANT_VIA_SINE        0x00
#define EFFECT_CONSTANT                 0x01
#define EFFECT_RAMP                     0x02
#define EFFECT_SQUARE                   0x03
#define EFFECT_SINE                     0x04
#define EFFECT_TRIANGLE                 0x05
#define EFFECT_SAWTOOTH_UP              0x06
#define EFFECT_SAWTOOTH_DOWN            0x07
#define EFFECT_SPRING                   0x08
#define EFFECT_DAMPER                   0x09

#define STATE_NONE                      0x00
#define STATE_ALLOCATED                 0x01
#define STATE_PLAYING                   0x02
#define STATE_STOPPED                   0x03

#define OPERATION_START                 0x01
#define OPERATION_START_SOLO            0x02
#define OPERATION_STOP                  0x03

#define CONTROL_ENABLE_ACTUATORS        0x01
#define CONTROL_DISABLE_ACTUATORS       0x02
#define CONTROL_STOP_ALL                0x03
#define CONTROL_RESET                   0x04
#define CONTROL_PAUSE                   0x05
#define CONTROL_CONTINUE                0x06

#define DURATION_INFINITE               0x7FFF

#define MAX_EFFECTS                     9

#define LOGICAL_MAXIMUM                 10000
#define ONE_BY_SQRT3                    5773
#define TWO_BY_SQRT3                    5773*2
#define SQRT3_BY_2                      8660

#endif //H_CONSTANTS_H

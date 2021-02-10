//
// Created by olukelo on 12.06.2020.
//

#ifndef FFB_JOYSTICK_HID_REPORT_HANDLERS_H
#define FFB_JOYSTICK_HID_REPORT_HANDLERS_H

#include "usbd_def.h"
#include "usb_reports.h"

void CustomEP0RxReady(USBD_HandleTypeDef *pDef);

void CustomUsbDataOut(USBD_HandleTypeDef *pDef);

void HandlePidBlockLoadReportRequest(USBD_HandleTypeDef *pDef);

void HandlePidPoolReportRequest(USBD_HandleTypeDef *pDef);

void HandleGenericReportRequest(USBD_HandleTypeDef *pDef);

void HandleGenericReport(GenericReportTypeDef *genericReport);

#endif //FFB_JOYSTICK_HID_REPORT_HANDLERS_H

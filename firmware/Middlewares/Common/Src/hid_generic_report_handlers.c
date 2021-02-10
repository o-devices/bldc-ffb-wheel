//
// Created by olukelo on 29.06.2020.
//

#include "common_resources.h"
#include "generic_commands.h"
#include "global_settings.h"
#include "persistance.h"
#include "usbd_customhid.h"
#include "usb_reports.h"

extern GenericReportTypeDef genericReport;

void PrepareSettingsReport() {
    genericReport.ReportId = REPORT_GET_COMMON_SETTINGS;
    memcpy(genericReport.ReportData, &globalSettings, sizeof(GlobalSettingsTypeDef));
}

void ApplySettings(GenericReportTypeDef *report) {
    memcpy(&globalSettings, report->ReportData, sizeof(GlobalSettingsTypeDef));
}

void HandleGenericReport(GenericReportTypeDef *report) {
    switch (report->ReportId) {
        case REPORT_GET_COMMON_SETTINGS:
            PrepareSettingsReport();
            break;
        case REPORT_SET_COMMON_SETTINGS:
            ApplySettings(report);
            break;
        case REPORT_CALIBRATE_MECHANICAL_CENTER:
            StartMechanicalCenterCalibration();
            break;
        case REPORT_CALIBRATE_POLES_CURVE:
            StartPolesCurveCalibration();
            break;
        case REPORT_SAVE_CALIBRATION_CURVE:
            saveCalibrationCurve();
            break;
        case REPORT_SAVE_SETTINGS:
            saveSettings();
            break;
    }
}

void HandleGenericReportRequest(USBD_HandleTypeDef *pDef) {
    USBD_CtlSendData(pDef, (uint8_t *) &genericReport, sizeof(GenericReportTypeDef));
}


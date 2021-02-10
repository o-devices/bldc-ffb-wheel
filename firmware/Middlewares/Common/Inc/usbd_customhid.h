/**
  ******************************************************************************
  * @file    usbd_customhid.h
  * @author  MCD Application Team
  * @brief   header file for the usbd_customhid.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CUSTOMHID_H
#define __USB_CUSTOMHID_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

#define USB_CUSTOM_HID_CONFIG_DESC_SIZ                  41U

#define CUSTOM_HID_EPIN_ADDR                            0x81U
#define CUSTOM_HID_EPIN_SIZE                            0x40U
#define CUSTOM_HID_EPOUT_ADDR                           0x01U
#define CUSTOM_HID_EPOUT_SIZE                           0x40U
#define CUSTOM_HID_FS_BINTERVAL                         0x05U
#define CUSTOM_HID_DESCRIPTOR_TYPE                      0x21U
#define CUSTOM_HID_REPORT_DESC                          0x22U
#define CUSTOM_HID_REQ_SET_PROTOCOL                     0x0BU
#define CUSTOM_HID_REQ_GET_PROTOCOL                     0x03U
#define CUSTOM_HID_REQ_SET_IDLE                         0x0AU
#define CUSTOM_HID_REQ_GET_IDLE                         0x02U
#define CUSTOM_HID_REQ_SET_REPORT                       0x09U
#define CUSTOM_HID_REQ_GET_REPORT                       0x01U

#define HID_REQ_FEATURE                                 0x03

/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
typedef enum {
    CUSTOM_HID_IDLE = 0U,
    CUSTOM_HID_BUSY,
} CUSTOM_HID_StateTypeDef;

typedef struct {
    uint32_t Protocol;
    uint32_t IdleState;
    uint32_t AltSetting;
    CUSTOM_HID_StateTypeDef state;
} USBD_CUSTOM_HID_HandleTypeDef;

extern USBD_ClassTypeDef USBD_CUSTOM_HID;

uint8_t UsbSendReport(USBD_HandleTypeDef *pDef, uint8_t *report, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __USB_CUSTOMHID_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

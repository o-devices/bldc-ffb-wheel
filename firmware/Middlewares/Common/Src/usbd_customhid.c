/**
  ******************************************************************************
  * @file    usbd_customhid.c
  * @author  MCD Application Team
  * @brief   This file provides the CUSTOM_HID core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                CUSTOM_HID Class  Description
  *          ===================================================================
  *           This module manages the CUSTOM_HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (CUSTOM_HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - Usage Page : Generic Desktop
  *             - Usage : Vendor
  *             - Collection : Application
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
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

/* Includes ------------------------------------------------------------------*/
#include "hid_report_handlers.h"
#include "effects_constants.h"
#include "usbd_customhid.h"
#include "usbd_ctlreq.h"

extern uint8_t HidReportDescriptor[];

static uint8_t USBD_CUSTOM_HID_Init(USBD_HandleTypeDef *pDef, uint8_t configuration);

static uint8_t USBD_CUSTOM_HID_DeInit(USBD_HandleTypeDef *pDef, uint8_t configuration);

static uint8_t USBD_CUSTOM_HID_Setup(USBD_HandleTypeDef *pDef, USBD_SetupReqTypedef *pReq);

static uint8_t USBD_CUSTOM_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_CUSTOM_HID_DataOut(USBD_HandleTypeDef *pDef, uint8_t epnum);

static uint8_t USBD_CUSTOM_HID_EP0_RxReady(USBD_HandleTypeDef *pDef);

static uint8_t *USBD_CUSTOM_HID_GetFSCfgDesc(uint16_t *length);

static uint8_t *USBD_CUSTOM_HID_GetDeviceQualifierDesc(uint16_t *length);

USBD_ClassTypeDef USBD_CUSTOM_HID = {
    USBD_CUSTOM_HID_Init,           /*Init*/
    USBD_CUSTOM_HID_DeInit,         /*DeInit*/
    USBD_CUSTOM_HID_Setup,          /*Setup*/
    NULL,                           /*EP0_TxSent*/
    USBD_CUSTOM_HID_EP0_RxReady,    /*EP0_RxReady*/ /* STATUS STAGE IN */
    USBD_CUSTOM_HID_DataIn,         /*DataIn*/
    USBD_CUSTOM_HID_DataOut,        /*DataOut*/
    NULL,                           /*SOF */
    NULL,                           /*IsoINIncomplete*/
    NULL,                           /*IsoINIncomplete*/
    USBD_CUSTOM_HID_GetFSCfgDesc,   /*GetHSConfigDescriptor*/
    USBD_CUSTOM_HID_GetFSCfgDesc,   /*GetHSConfigDescriptor*/
    USBD_CUSTOM_HID_GetFSCfgDesc,   /*GetHSConfigDescriptor*/
    USBD_CUSTOM_HID_GetDeviceQualifierDesc,
};

/* USB CUSTOM_HID device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_CfgFSDesc[USB_CUSTOM_HID_CONFIG_DESC_SIZ] __ALIGN_END = {
    0x09,                                               /* bLength: Configuration Descriptor size */
    0x02,                                               /* bDescriptorType: Configuration */
    LOBYTE(USB_CUSTOM_HID_CONFIG_DESC_SIZ),             /* wTotalLength: Bytes returned */
    HIBYTE(USB_CUSTOM_HID_CONFIG_DESC_SIZ),
    0x01,                                               /* bNumInterfaces: 1 interface */
    0x01,                                               /* bConfigurationValue: Configuration value */
    0x00,                                               /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,                                               /* bmAttributes: bus powered */
    0x32,                                               /* MaxPower 100 mA: this current is used for detecting Vbus */

    /************** Descriptor of CUSTOM HID interface ****************/

    /* 09 */
    0x09,                                               /* bLength: Interface Descriptor size*/
    0x04,                                               /* bDescriptorType: Interface descriptor type */
    0x00,                                               /* bInterfaceNumber: Number of Interface */
    0x00,                                               /* bAlternateSetting: Alternate setting */
    0x02,                                               /* bNumEndpoints*/
    0x03,                                               /* bInterfaceClass: CUSTOM_HID */
    0x00,                                               /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,                                               /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0x00,                                               /* iInterface: Index of string descriptor */

    /******************** Descriptor of CUSTOM_HID *************************/

    /* 18 */
    0x09,                                               /* bLength: CUSTOM_HID Descriptor size */
    0x21,                                               /* bDescriptorType: HID */
    0x00,                                               /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                                               /* bCountryCode: Hardware target country */
    0x01,                                               /* bNumDescriptors: Number of CUSTOM_HID class descriptors to follow */
    0x22,                                               /* bDescriptorType */
    LOBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE),           /* wItemLength: Total length of Report descriptor */
    HIBYTE(USBD_CUSTOM_HID_REPORT_DESC_SIZE),           /* wItemLength: Total length of Report descriptor */

    /******************** Descriptor of Custom HID endpoints ********************/

    /* 27 */
    0x07,                                               /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */
    CUSTOM_HID_EPIN_ADDR,                               /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                                               /* bmAttributes: Interrupt endpoint */
    CUSTOM_HID_EPIN_SIZE,                               /* wMaxPacketSize: 2 Byte max */
    0x00,
    CUSTOM_HID_FS_BINTERVAL,                            /* bInterval: Polling Interval */

    /* 34 */
    0x07,                                               /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */
    CUSTOM_HID_EPOUT_ADDR,                              /* bEndpointAddress: Endpoint Address (OUT) */
    0x03,                                               /* bmAttributes: Interrupt endpoint */
    CUSTOM_HID_EPOUT_SIZE,                              /* wMaxPacketSize: 2 Bytes max  */
    0x00,
    CUSTOM_HID_FS_BINTERVAL,                            /* bInterval: Polling Interval */
    /* 41 */
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
};

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         Initialize the CUSTOM_HID interface
  * @param  pDef: device instance
  * @param  configuration: Configuration index
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_Init(USBD_HandleTypeDef *pDef, uint8_t configuration) {
    UNUSED(configuration);
    USBD_CUSTOM_HID_HandleTypeDef *handleTypeDef;

    handleTypeDef = USBD_malloc(sizeof(USBD_CUSTOM_HID_HandleTypeDef));

    if (handleTypeDef == NULL) {
        pDef->pClassData = NULL;
        return (uint8_t) USBD_EMEM;
    }

    pDef->pClassData = (void *) handleTypeDef;

    pDef->pUserData = USBD_malloc(CUSTOM_HID_EPOUT_SIZE);

    pDef->ep_in[CUSTOM_HID_EPIN_ADDR & 0xFU].bInterval = CUSTOM_HID_FS_BINTERVAL;
    pDef->ep_out[CUSTOM_HID_EPOUT_ADDR & 0xFU].bInterval = CUSTOM_HID_FS_BINTERVAL;

    /* Open EP IN */
    (void) USBD_LL_OpenEP(pDef, CUSTOM_HID_EPIN_ADDR, USBD_EP_TYPE_INTR, CUSTOM_HID_EPIN_SIZE);

    pDef->ep_in[CUSTOM_HID_EPIN_ADDR & 0xFU].is_used = 1U;

    /* Open EP OUT */
    (void) USBD_LL_OpenEP(pDef, CUSTOM_HID_EPOUT_ADDR, USBD_EP_TYPE_INTR, CUSTOM_HID_EPOUT_SIZE);

    pDef->ep_out[CUSTOM_HID_EPOUT_ADDR & 0xFU].is_used = 1U;

    handleTypeDef->state = CUSTOM_HID_IDLE;

    /* Prepare Out endpoint to receive 1st packet */
    (void) USBD_LL_PrepareReceive(pDef, CUSTOM_HID_EPOUT_ADDR, pDef->pUserData, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);

    return (uint8_t) USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         DeInitialize the CUSTOM_HID layer
  * @param  pDef: device instance
  * @param  configuration: Configuration index
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_DeInit(USBD_HandleTypeDef *pDef, uint8_t configuration) {
    UNUSED(configuration);

    /* Close CUSTOM_HID EP IN */
    (void) USBD_LL_CloseEP(pDef, CUSTOM_HID_EPIN_ADDR);
    pDef->ep_in[CUSTOM_HID_EPIN_ADDR & 0xFU].is_used = 0U;
    pDef->ep_in[CUSTOM_HID_EPIN_ADDR & 0xFU].bInterval = 0U;

    /* Close CUSTOM_HID EP OUT */
    (void) USBD_LL_CloseEP(pDef, CUSTOM_HID_EPOUT_ADDR);
    pDef->ep_out[CUSTOM_HID_EPOUT_ADDR & 0xFU].is_used = 0U;
    pDef->ep_out[CUSTOM_HID_EPOUT_ADDR & 0xFU].bInterval = 0U;

    /* FRee allocated memory */
    if (pDef->pClassData != NULL) {
        USBD_free(pDef->pClassData);
        pDef->pUserData = NULL;
    }

    if (pDef->pUserData != NULL) {
        USBD_free(pDef->pUserData);
        pDef->pUserData = NULL;
    }

    return (uint8_t) USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Setup
  *         Handle the CUSTOM_HID specific requests
  * @param  pDef: instance
  * @param  pReq: usb requests
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_Setup(USBD_HandleTypeDef *pDef, USBD_SetupReqTypedef *pReq) {
    USBD_CUSTOM_HID_HandleTypeDef *handleTypeDef = (USBD_CUSTOM_HID_HandleTypeDef *) pDef->pClassData;

    switch (pReq->bmRequest & USB_REQ_TYPE_MASK) {

        case USB_REQ_TYPE_CLASS:
            switch (pReq->bRequest) {

                case CUSTOM_HID_REQ_SET_PROTOCOL:
                    handleTypeDef->Protocol = (uint8_t) (pReq->wValue);
                    break;

                case CUSTOM_HID_REQ_GET_PROTOCOL:
                    (void) USBD_CtlSendData(pDef, (uint8_t *) &handleTypeDef->Protocol, 1U);
                    break;

                case CUSTOM_HID_REQ_SET_IDLE:
                    handleTypeDef->IdleState = (uint8_t) (pReq->wValue >> 8);
                    break;

                case CUSTOM_HID_REQ_GET_IDLE:
                    (void) USBD_CtlSendData(pDef, (uint8_t *) &handleTypeDef->IdleState, 1U);
                    break;

                case CUSTOM_HID_REQ_SET_REPORT:
                    switch (HIBYTE(pReq->wValue))
                        case HID_REQ_FEATURE:
                            switch (LOBYTE(pReq->wValue)) {
                                case REPORT_CREATE_NEW_EFFECT:
                                case REPORT_GENERIC:
                                    (void) USBD_CtlPrepareRx(pDef, pDef->pUserData, pReq->wLength);
                                    break;
                            }
                    break;

                case CUSTOM_HID_REQ_GET_REPORT:
                    switch (HIBYTE(pReq->wValue)) {
                        case (HID_REQ_FEATURE):
                            switch (LOBYTE(pReq->wValue)) {
                                case REPORT_PID_POOL:
                                    HandlePidPoolReportRequest(pDef);
                                    break;
                                case REPORT_PID_BLOCK_LOAD:
                                    HandlePidBlockLoadReportRequest(pDef);
                                    break;
                                case REPORT_GENERIC:
                                    HandleGenericReportRequest(pDef);
                                    break;
                            }
                    }
                    break;

                default:
                    USBD_CtlError(pDef, pReq);
                    return USBD_FAIL;
            }
            break;

        case USB_REQ_TYPE_STANDARD:
            switch (pReq->bRequest) {

                case USB_REQ_GET_DESCRIPTOR:
                    if ((pReq->wValue >> 8) == CUSTOM_HID_REPORT_DESC) {
                        USBD_CtlSendData(pDef, HidReportDescriptor, MIN(USBD_CUSTOM_HID_REPORT_DESC_SIZE, pReq->wLength));
                    } else {
                        if ((pReq->wValue >> 8) == CUSTOM_HID_DESCRIPTOR_TYPE) {
                            USBD_CtlSendData(pDef, USBD_CUSTOM_HID_CfgFSDesc + 18, MIN(9, pReq->wLength));
                        }
                    }
                    break;

                case USB_REQ_GET_INTERFACE:
                    (void) USBD_CtlSendData(pDef, (uint8_t *) &handleTypeDef->AltSetting, 1U);
                    break;

                case USB_REQ_SET_INTERFACE:
                    handleTypeDef->AltSetting = (uint8_t) (pReq->wValue);
                    break;

                default:
                    USBD_CtlError(pDef, pReq);
                    return USBD_FAIL;
            }
            break;

        default:
            USBD_CtlError(pDef, pReq);
            return USBD_FAIL;
    }
    return (uint8_t) USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_SendReport
  *         Send CUSTOM_HID Report
  * @param  pDef: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t UsbSendReport(USBD_HandleTypeDef *pDef, uint8_t *report, uint16_t len) {
    USBD_CUSTOM_HID_HandleTypeDef *handleTypeDef;

    if (pDef->pClassData == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    handleTypeDef = (USBD_CUSTOM_HID_HandleTypeDef *) pDef->pClassData;

    if (pDef->dev_state == USBD_STATE_CONFIGURED) {
        if (handleTypeDef->state == CUSTOM_HID_IDLE) {
            handleTypeDef->state = CUSTOM_HID_BUSY;
            (void) USBD_LL_Transmit(pDef, CUSTOM_HID_EPIN_ADDR, report, len);
        } else {
            return (uint8_t) USBD_BUSY;
        }
    }
    return (uint8_t) USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_GetFSCfgDesc
  *         return FS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_CUSTOM_HID_GetFSCfgDesc(uint16_t *length) {
    *length = (uint16_t) sizeof(USBD_CUSTOM_HID_CfgFSDesc);

    return USBD_CUSTOM_HID_CfgFSDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    UNUSED(epnum);

    /* Ensure that the FIFO is empty before a new transfer, this condition could
    be caused by  a new transfer before the end of the previous transfer */
    ((USBD_CUSTOM_HID_HandleTypeDef *) pdev->pClassData)->state = CUSTOM_HID_IDLE;

    return (uint8_t) USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_DataOut
  *         handle data OUT Stage
  * @param  pDef: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_DataOut(USBD_HandleTypeDef *pDef, uint8_t epnum) {
    UNUSED(epnum);

    if (pDef->pClassData == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    CustomUsbDataOut(pDef);

    USBD_LL_PrepareReceive(pDef, CUSTOM_HID_EPOUT_ADDR, pDef->pUserData, USB_FS_MAX_PACKET_SIZE);

    return (uint8_t) USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_EP0_RxReady
  *         Handles control request data.
  * @param  pDef: device instance
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_EP0_RxReady(USBD_HandleTypeDef *pDef) {
    CustomEP0RxReady(pDef);
    return (uint8_t) USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t *USBD_CUSTOM_HID_GetDeviceQualifierDesc(uint16_t *length) {
    *length = (uint16_t) sizeof(USBD_CUSTOM_HID_DeviceQualifierDesc);
    return USBD_CUSTOM_HID_DeviceQualifierDesc;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/*******************************************************************************
  OTA service Transport Header File

  File Name:
    ota_service_transport_ble.h

  Summary:
    This file contains OTA service Transport definitions and functions.

  Description:
    This file contains OTA service Transport definitions and functions.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

#ifndef OTA_SERVICE_TRANSPORT_H    /* Guard against multiple inclusion */
#define OTA_SERVICE_TRANSPORT_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
//#include "system/system_module.h"
#include "ota_service.h"
/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


bool OTA_SERVICE_Transport_initialize(char* pBuffer, uint8_t len);
bool OTA_SERVICE_Transport_Open(void * param);
bool OTA_SERVICE_Transport_Deinitialize(void);
bool OTA_SERVICE_Transport_Complete(void);
uint32_t OTA_SERVICE_Transport_MsgRecv(void* handle, uint8_t* buf, uint32_t bufSize);
uint32_t OTA_SERVICE_Transport_MsgSend(void* handle, char* buf, uint32_t bufSize);
uint32_t OTA_SERVICE_Transport_RspRecv(void* handle, char* buf, uint32_t bufSize);
bool OTA_SERVICE_Transport_SendCmd_RecvRsp(uint8_t *cmdMsg, uint8_t cmdLen,  uint8_t *responsemsg, uint8_t responseLen);
bool OTA_SERVICE_Transport_ackResp(void);
bool OTA_SERVICE_Transport_CloseOta(int state);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* OTA_SERVICE_TRANSPORT_H */

/* *****************************************************************************
 End of File
 */

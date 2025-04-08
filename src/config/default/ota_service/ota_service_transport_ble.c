/*******************************************************************************
  OTA service Transport Source File

  File Name:
    ota_service_transport_ble.c

  Summary:
    This file contains source code for OTA Transport service.

  Description:
    This file contains source code for OTA Transport service.
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

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include<stdio.h>
#include<string.h>
#include "ota_service.h"
#include "rnbd/rnbd.h"
#include "ota_service_transport_ble.h"
#include "rnbd/rnbd_interface.h"
#include "rnbd/rnbd.h"
static char otaBuff[10] = {'%', 'O', 'T', 'A', '_', 'D', 'A', 'T', 'A', ','};
static char OTAV_0[9] = {'O', 'T', 'A', 'V', ',', '0', '0', '\r', '\n'};
static char OTAV_1[9] = {'O', 'T', 'A', 'V', ',', '0', '1', '\r', '\n'};
static char OTAV_2[9] = {'O', 'T', 'A', 'V', ',', '0', '2', '\r', '\n'};

static char OTAA_RESP[10] = {'A', 'O', 'K', '\r', '\n', 'D', 'F', 'U', '>', ' '};
static char OTAA2_ACK[9] = {'O', 'T', 'A', 'A', ',', '0', '2', '\r', '\n'};

static char OTAA_RESP2[9] = {'A', 'O', 'K', '\r', '\n', 'D', 'F', 'U', '>'};
static uint8_t respBuff[20];

#define NIBBLE_TO_HEX(nibble) (((nibble) >= (uint8_t)'0' && (nibble) <= (uint8_t)'9') ? ((nibble) - (uint8_t)'0') : (((nibble) >= (uint8_t)'a' && (nibble) <= (uint8_t)'f') ? ((nibble) - (uint8_t)'a' + 10U) : (((nibble) >= (uint8_t)'A' && (nibble) <= (uint8_t)'F') ? ((nibble) - (uint8_t)'A' + 10U) : 0U)))

bool OTA_SERVICE_Transport_initialize(char* pBuffer, uint8_t len) {

    return (RNBD_SetAsyncMessageHandler(pBuffer, len));

}

bool OTA_SERVICE_Transport_Open(void * param) {

    bool ret = true;

    return ret;
}

bool OTA_SERVICE_Transport_Deinitialize(void) {

    bool ret = false;

    //RNBD_EnterDataMode();
    return ret;
}

bool OTA_SERVICE_Transport_Complete(void) {
    bool ret = true;
    int index = 0;
    char buf[14];
    char respBuffer[14] = {'%', 'O', 'T', 'A', '_', 'C', 'O', 'M', 'P', 'L', 'E', 'T', 'E', '%'};
    for (index = 0; index < 14; index++) {
        buf[index] = (char)RNBD_Read();
        if (buf[index] != respBuffer[index]) {
            return false; //Error condition
        }
    }
    (void)OTA_SERVICE_Transport_MsgSend(NULL, OTAV_1, sizeof (OTAV_1));
    if (OTA_SERVICE_Transport_RspRecv(NULL, OTAA_RESP, sizeof (OTAA_RESP)) != sizeof (OTAA_RESP))
    {
        return false;
    }

    return ret;
}

bool OTA_SERVICE_Transport_CloseOta(int state) {
    bool ret = true;
    if (state == 1) {
        (void)OTA_SERVICE_Transport_MsgSend(NULL, OTAV_0, sizeof (OTAV_0));

        if (OTA_SERVICE_Transport_RspRecv(NULL, OTAA_RESP2, sizeof (OTAA_RESP2)) != sizeof (OTAA_RESP2)) {
            return false;
        }
    } else {
        (void)OTA_SERVICE_Transport_MsgSend(NULL, OTAV_2, sizeof (OTAV_2));

        if (OTA_SERVICE_Transport_RspRecv(NULL, OTAA_RESP2, sizeof (OTAA_RESP2)) != sizeof (OTAA_RESP2)) {
            return false;
        }

    }
    return ret;
}

bool OTA_SERVICE_Transport_SendCmd_RecvRsp(uint8_t *cmdMsg, uint8_t cmdLen, uint8_t *responsemsg, uint8_t responseLen) {
    bool ret = false;
    ret = RNBD_SendCommand_ReceiveResponse((const char *)cmdMsg, cmdLen, (const char *)responsemsg, responseLen);
    return ret;
}

uint32_t OTA_SERVICE_Transport_RspRecv(void* handle, char* buf, uint32_t bufSize) {
    uint32_t index;

    for (index = 0; index < bufSize; index++) {
        respBuff[index] = RNBD_Read();
    }
    for (index = 0; index < bufSize; index++) {
        if ((char)respBuff[index] != buf[index])
        {
            return 0;
        }
    }
    return index;
}

uint32_t OTA_SERVICE_Transport_MsgRecv(void* handle, uint8_t* buf, uint32_t bufSize) {
    uint32_t index;
    uint8_t data_header[19];
    uint32_t payloadSize;
    uint8_t temp1;
    uint8_t temp2;
    uint32_t nibble1;
    uint32_t nibble2;
    //uint16_t seq_num;

    //Read first 0x12 data from here to verify header %OTA_DATA,
    for (index = 0U; index < 10U; index++) {
        data_header[index] = RNBD_Read();
        if (data_header[index] != (uint8_t)otaBuff[index]) {
            return 0; //Error condition
        }
    }
    for (; index < 18U; index++) {
        data_header[index] = RNBD_Read();
    }
    /* extract seq number */
    //user can extract seq number from data_header[10] and data_header[11]
    //seq_num = ((uint16_t) (NIBBLE_TO_HEX(data_header[10]) << 4) | (uint16_t) (NIBBLE_TO_HEX(data_header[11])));

    // Fixed MISRA C violation
    temp2 = NIBBLE_TO_HEX(data_header[13]);
    temp1 = NIBBLE_TO_HEX(data_header[14]);
    nibble2 = temp2;
    nibble1 = temp1;
    /* extract payload size from read buffer */
    payloadSize = (((nibble2 & 0xFU) << 12U) | ((nibble1 & 0xFU) << 8U) | ((NIBBLE_TO_HEX(data_header[15]) & 0xFU) << 4U) | (NIBBLE_TO_HEX(data_header[16]) & 0xFU));

    for (index = 0; index < payloadSize; index++) {
        buf[index] = RNBD_Read();
    }
    data_header[18] = RNBD_Read();

    return payloadSize;
}

bool OTA_SERVICE_Transport_ackResp(void) {
    bool ret = true;
    (void)OTA_SERVICE_Transport_MsgSend(NULL, OTAA2_ACK, sizeof (OTAA2_ACK));
    if (OTA_SERVICE_Transport_RspRecv(NULL, OTAA_RESP, sizeof (OTAA_RESP)) != sizeof (OTAA_RESP)) {

        return false;
    }
    return ret;
}

uint32_t OTA_SERVICE_Transport_MsgSend(void* handle, char* buf, uint32_t bufSize) {
    uint32_t index = 1U;

    RNBD_SendCmd((const char *) buf, (uint8_t)bufSize);

    return index;
}


/* *****************************************************************************
 End of File
 */

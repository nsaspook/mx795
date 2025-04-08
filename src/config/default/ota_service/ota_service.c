/*******************************************************************************
  OTA service Source File

  File Name:
    ota_service.c

  Summary:
    This file contains source code for OTA service.

  Description:
    This file contains source code for OTA service.
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

#include <stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include "ota_service.h"
#include "ota_service_transport_ble.h"
#include "ota_service_file_handler.h"
#include "rnbd/rnbd_interface.h"
#include "rnbd/rnbd.h"

/* temporary place holder. Will change during integration */

#define MAX_BUFFER_SIZE                 (80)
#define TRANSPORT_BUFFER_SIZE           MAX_PKT_LEN
#define FILE_IMAGE_HEADER_1_SIZE        sizeof(OTA_FILE_HEADER)
#define FILE_IMAGE_HEADER_1             "IMAGESTART0"
#define FILE_IMAGE_END_MARK             "IMAGEEND"
#define OTA_SUCCESS                     1
#define OTA_FAIL                        0

static uint8_t image_data[TRANSPORT_BUFFER_SIZE];
static OTA_DATA ota;

static OTA_DOWNLOAD_TASK_CONTEXT ctx1;
static bool transportIsInitialized = false;
static char statusBuffer[MAX_BUFFER_SIZE];

// *****************************************************************************
// *****************************************************************************
// Section: To check if OTA state is idle
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  bool OTA_IsIdle(void)

  Description:
    To check if OTA state is idle

  Task Parameters:
    None

  Return:
    True- if state is idle
    False- if state is not idle
 */
//---------------------------------------------------------------------------

bool OTA_IsIdle(void) {

    return ota.ota_idle;
}


// *****************************************************************************
// *****************************************************************************
// Section: To Initialize OTA related parameters
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_SERVICE_OTA_Initialize(void)

  Description:
    To Initialize OTA related parameters

  Task Parameters:
    None

  Return:
    None
 */
//---------------------------------------------------------------------------

void OTA_SERVICE_OTA_Initialize(void) {
    (void)memset(&ota, 0, sizeof (ota));
    /*Initializing downloader status*/

    ota.status = SYS_STATUS_UNINITIALIZED;
    ota.current_task = OTA_TASK_INIT;
    ota.callback = NULL;
}


//---------------------------------------------------------------------------

// *****************************************************************************
// *****************************************************************************
// Section: API for upper layer to initiate Roll back
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_SERVICE_OTA_Rollback(void)

  Description:
    API for upper layer to initiate Roll back

  Task Parameters:
    None

  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

/*update this function */
SYS_STATUS OTA_SERVICE_OTA_Rollback(void) {

    if (ota.current_task != OTA_TASK_IDLE) {
        return SYS_STATUS_ERROR;
    }
    /* Implement rollback logic here*/

    ota.ota_rollback_initiated = true;
    return SYS_STATUS_READY;
}

//---------------------------------------------------------------------------

// *****************************************************************************
// *****************************************************************************
// Section: API for upper layer to initiate Firmware switch
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_SERVICE_OTA_FirmwareSwitch(void)

  Description:
    API for upper layer to initiate Firmware switch

  Task Parameters:
    None

  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

SYS_STATUS OTA_SERVICE_OTA_FirmwareSwitch(void) {
    if (ota.current_task != OTA_TASK_IDLE) {
        return SYS_STATUS_ERROR;
    }
    /* Implement firmware switch logic here*/


    ota.ota_result = OTA_RESULT_SWITCH_FIRMWARE_SUCCESS;
    return SYS_STATUS_READY;
}

// *****************************************************************************
// *****************************************************************************
// Section: Starting OTA process
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_SERVICE_OTA_Start(void)
  Description:
    Starting OTA process

  Task Parameters:
    None..

  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

SYS_STATUS OTA_SERVICE_OTA_Start(void) {

    /*Add OTA PARAM structure during integration if needed to init file handler
     related parameters later                       */
    ota.current_task = OTA_TASK_DOWNLOAD_IMAGE;
    ota.status = SYS_STATUS_BUSY;

    return SYS_STATUS_READY;
}

//---------------------------------------------------------------------------
/*
  OTA_RegCB(void)

  Description:
    To register user call back function

  Task Parameters:
    None

  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

static inline SYS_STATUS OTA_RegCB(OTA_COMPLETION_CALLBACK callback) {
    SYS_STATUS ret = SYS_STATUS_ERROR;
    if (ota.callback == NULL) {
        /* Copy the client function pointer */
        ota.callback = callback;
        ret = SYS_STATUS_READY;
    }
    return ret;
}

//---------------------------------------------------------------------------
/*

  Description:
 Registering OTA callback function

  Task Parameters:
    buffer - callback function name
    length - function pointer length
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

SYS_STATUS OTA_CallBackReg(OTA_COMPLETION_CALLBACK buffer, uint32_t length) {
    SYS_STATUS status = SYS_STATUS_ERROR;

    OTA_COMPLETION_CALLBACK g_otaFunPtr = buffer;
    if ((g_otaFunPtr != NULL) && (length == sizeof (g_otaFunPtr))) {
        /* Register the client callback function */
        status = OTA_RegCB(g_otaFunPtr);
    }
    return status;
}
//---------------------------------------------------------------------------
/*
  void OTA_SERVIC_Task_UpdateUser(void)

  Description:
    To update user about OTA status

  Task Parameters:
    None

  Return:
    None
 */
//---------------------------------------------------------------------------

void OTA_SERVIC_Task_UpdateUser(void) {
    OTA_COMPLETION_CALLBACK callback = ota.callback;

    /*if callback is for image download start */

    if ((ota.ota_result != OTA_RESULT_IMAGE_DOWNLOAD_START)) {

        ota.status = SYS_STATUS_ERROR;
    }

    ota.status = SYS_STATUS_READY;
    if (callback != NULL) {
        callback(ota.ota_result, NULL, NULL);
    }
}
// *****************************************************************************

uint32_t OTA_SERVICE_Transport_FHMsgReceive(OTA_FILE_HANDLER_CONTEXT *otaFileHandlerCtx) {
    OTA_DOWNLOAD_TASK_CONTEXT *ctx = &ctx1;
    uint32_t bytes_written = 0U;

    if (ctx->read_index < ctx->write_index) {
        if (otaFileHandlerCtx->size < (ctx->write_index - ctx->read_index)) {
            bytes_written = otaFileHandlerCtx->size;
        } else {
            bytes_written = (ctx->write_index - ctx->read_index);
        }

        if (ctx->file_header == false) {
            if (strncmp((char *) &image_data[ctx->read_index], FILE_IMAGE_HEADER_1, strlen(FILE_IMAGE_HEADER_1)) == 0) {
                ctx->file_header = true;

                /* Store File Header */
                (void)memcpy((uint8_t *)otaFileHandlerCtx->fileHeader, &image_data[ctx->read_index], FILE_IMAGE_HEADER_1_SIZE);

                /* Store remaining data */
                (void)memcpy(otaFileHandlerCtx->buffer, (void *)&image_data[ctx->read_index + FILE_IMAGE_HEADER_1_SIZE], (bytes_written - FILE_IMAGE_HEADER_1_SIZE));

                /* Save File Header fields into transport task context */
                ctx->total_len = otaFileHandlerCtx->fileHeader->imageSize + FILE_IMAGE_HEADER_1_SIZE + strlen(FILE_IMAGE_END_MARK);
                ctx->read_index += bytes_written;
                bytes_written = bytes_written - FILE_IMAGE_HEADER_1_SIZE;
            } else {
                bytes_written = 0U;
            }
        } else {
            (void)memcpy(otaFileHandlerCtx->buffer, (void *)&image_data[ctx->read_index], bytes_written);
            ctx->read_index += bytes_written;
        }
    }

    return bytes_written;
}

SYS_STATUS OTA_SERVICE_Transport_Tasks(OTA_DOWNLOAD_TASK_CONTEXT *ctx) {
    static SYS_STATUS status = SYS_STATUS_BUSY;


    switch (ota.task.state) {
        case TASK_STATE_D_INIT:
        {
            ota.ota_result = OTA_RESULT_IMAGE_DOWNLOAD_START;

            OTA_SERVIC_Task_UpdateUser();

            if (transportIsInitialized == false) {
                transportIsInitialized = OTA_SERVICE_Transport_initialize(statusBuffer, (uint8_t)sizeof (statusBuffer));
            }
            ctx->total_len = 0;
            ctx->total_received = 0;
            ctx->readPktlen = MAX_PKT_LEN;
            ctx->read_index = 0;
            ctx->write_index = 0;
            ctx->file_header = false;
            ota.task.state = TASK_STATE_D_GET_IMAGE_READ;
            break;
        }

        case TASK_STATE_D_GET_IMAGE_READ:
        {
            uint32_t rx_len = 0;

            rx_len = OTA_SERVICE_Transport_MsgRecv(NULL, &image_data[ctx->write_index], ctx->readPktlen + DATA_HEADER_LEN);

            ctx->write_index += rx_len;
            ctx->total_received += rx_len;

            if (rx_len == 0U) {

                ota.task.state = TASK_STATE_D_IMAGE_END;
                status = SYS_STATUS_ERROR;
                break;
            }

            ota.task.state = TASK_STATE_D_CHECK_IMAGE_READ;


            break;
        }

        case TASK_STATE_D_CHECK_IMAGE_READ:
        {
            if (ctx->file_header == true) {
                if ((ctx->total_len - ctx->total_received) < MAX_PKT_LEN) {
                    ctx->readPktlen = (ctx->total_len - ctx->total_received);
                }
            }

            if ((ctx->write_index >= TRANSPORT_BUFFER_SIZE) || ((ctx->total_received % BUFFER_MAX_SIZE) == 0U) || ((ctx->file_header == true) && (ctx->total_len == ctx->total_received))) {
                ota.task.state = TASK_STATE_D_SEND_ACK;
            } else {
                ota.task.state = TASK_STATE_D_GET_IMAGE_READ;
            }
            break;
        }
        case TASK_STATE_D_SEND_ACK:
        {
            if ((ctx->write_index == ctx->read_index) || ((ctx->total_received % BUFFER_MAX_SIZE) == 0U) || ((ctx->file_header == true) && (ctx->total_len == ctx->total_received))) {
                if (ctx->write_index == ctx->read_index) {
                    ctx->write_index = 0;
                    ctx->read_index = 0;
                }
                if (OTA_SERVICE_Transport_ackResp() == false) {
                    ota.task.state = TASK_STATE_D_IMAGE_END;
                    status = SYS_STATUS_ERROR;
                    break;
                }

                if (ctx->total_len == ctx->total_received) {
                    ota.task.state = TASK_STATE_D_IMAGE_END;
                } else {
                    ota.task.state = TASK_STATE_D_GET_IMAGE_READ;
                }
            }
            break;
        }
        case TASK_STATE_D_IMAGE_END:
        {
            if ((OTA_SERVICE_FH_StateGet() == OTA_SERVICE_FH_STATE_IDLE) && (status != SYS_STATUS_ERROR)) {
                if (ota.task.state == TASK_STATE_D_IMAGE_END) {
                    if (OTA_SERVICE_Transport_Complete() == false) {
                        status = SYS_STATUS_ERROR;
                    }

                    if (status != SYS_STATUS_ERROR) {
                        if (OTA_SERVICE_Transport_CloseOta(OTA_SUCCESS) == false) {
                            ctx = NULL;
                            status = SYS_STATUS_ERROR;
                        }

                    }
                    return status == SYS_STATUS_ERROR ? SYS_STATUS_ERROR : SYS_STATUS_READY;
                }
            } else if ((OTA_SERVICE_FH_StateGet() == OTA_SERVICE_FH_STATE_ERROR) || (status == SYS_STATUS_ERROR)) {
                if (OTA_SERVICE_Transport_CloseOta(OTA_FAIL) == false) {
                    ctx = NULL;
                    status = SYS_STATUS_ERROR;
                }
            } else {

            }

            break;
        }
        default:
        {
            /* Do nothing */
            break;
        }
    }

    return SYS_STATUS_BUSY;
}

// *****************************************************************************
// *****************************************************************************
// Section: To maintain OTA task state machine
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_SERVICE_OTA_Tasks(void)

  Description:
    To maintain OTA task state machine

  Task Parameters:
    None

  Return:
    None
 */
//---------------------------------------------------------------------------

static void OTA_SERVICE_OTA_Tasks(void) {

    switch (ota.current_task) {
        case OTA_TASK_INIT:
        {
            ota.ota_idle = false;
            ota.current_task = OTA_TASK_UPDATE_USER;
            ota.task.state = TASK_STATE_D_INIT;
            break;
        }
        case OTA_TASK_UPDATE_USER:
        {
            ota.ota_idle = false;
            ota.current_task = OTA_TASK_IDLE;
            if (ota.ota_result == OTA_RESULT_IMAGE_DOWNLOAD_SUCCESS) {
                ota.current_task = OTA_TASK_INIT;
                ota.task.state = TASK_STATE_D_INIT;
            }

            if (ota.ota_result == OTA_RESULT_IMAGE_DOWNLOAD_START) {
                ota.current_task = OTA_TASK_DOWNLOAD_IMAGE;
                ota.task.state = TASK_STATE_D_INIT;
            }
            OTA_SERVIC_Task_UpdateUser();
            break;
        }
        case OTA_TASK_IDLE:
        {
            ota.ota_idle = true;
            break;
        }

        case OTA_TASK_DOWNLOAD_IMAGE:
        {
            ota.ota_idle = false;
            ota.status = OTA_SERVICE_Transport_Tasks(& ctx1);
            if (ota.status == SYS_STATUS_READY) {

                ota.ota_result = OTA_RESULT_IMAGE_DOWNLOAD_SUCCESS;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = TASK_STATE_D_INIT;

            }

            if (ota.status == SYS_STATUS_ERROR) {

                ota.ota_result = OTA_RESULT_IMAGE_DOWNLOAD_FAILED;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = TASK_STATE_D_INIT;
            }

            break;
        }
        case OTA_TASK_IMAGE_END:
        {
            if (ota.status == SYS_STATUS_READY) {

                ota.ota_result = OTA_RESULT_IMAGE_DOWNLOAD_SUCCESS;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = TASK_STATE_D_INIT;
                ota.new_downloaded_img = true;
            }

            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }
}

void OTA_SERVICE_Tasks(void) {

    OTA_SERVICE_OTA_Tasks();

    OTA_SERVICE_FH_Tasks();
}

/* *****************************************************************************
 End of File
 */

/*******************************************************************************
  OTA service Header File

  File Name:
    ota_service.h

  Summary:
    This file contains OTA service definitions and functions.

  Description:
    This file contains OTA service definitions and functions.
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

#ifndef OTA_SERVICE_H    /* Guard against multiple inclusion */
#define OTA_SERVICE_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/* This section lists the other files that are included in this file.
 */
#include "ota_config.h"
#include "system/system.h"
#include "ota_service_file_handler.h"
/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


    /* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */

/******************************************************************************/
/*                     provides ota system status                             */
/******************************************************************************/

typedef enum {

    /*Image OTA update  available*/
    OTA_RESULT_OTA_UPDATE_AVAILABLE = 0,

    /*Image OTA update  available*/
    OTA_RESULT_OTA_UPDATE_NOT_AVAILABLE,

    /*Roll back completed*/
    OTA_RESULT_ROLLBACK_SUCCESS,

    /*Roll back failed*/
    OTA_RESULT_ROLLBACK_FAILED,

    /*OTA update completed*/
    OTA_RESULT_OTA_UPDATE_SUCCESS,

    /*OTA update failed*/
    OTA_RESULT_OTA_UPDATE_FAILED,

    /* Image downloading started*/
    OTA_RESULT_IMAGE_DOWNLOAD_START,

    /*Image downloaded successfully*/
    OTA_RESULT_IMAGE_DOWNLOAD_SUCCESS ,

    /*Image download failed*/
    OTA_RESULT_IMAGE_DOWNLOAD_FAILED,

    /*firmware switch successful*/
    OTA_RESULT_SWITCH_FIRMWARE_SUCCESS,

    /*firmware switch successful*/
    OTA_RESULT_SWITCH_FIRMWARE_FAILED,

    /*No update for user*/
    OTA_RESULT_NONE

}OTA_RESULT;

/******************************************************************************/
/*                         OTA TASK state                                     */
/******************************************************************************/

typedef enum {
    OTA_TASK_INIT = 0,
    OTA_TASK_IDLE,
    OTA_TASK_DOWNLOAD_IMAGE,
    OTA_TASK_IMAGE_END,
    OTA_TASK_SET_IMAGE_STATUS,
    OTA_TASK_UPDATE_USER
} OTA_TASK_ID;

typedef enum {
    TASK_STATE_D_INIT = 0,
    TASK_STATE_D_GET_IMAGE_READ,
    TASK_STATE_D_CHECK_IMAGE_READ,
    TASK_STATE_D_SEND_ACK,
    TASK_STATE_D_IMAGE_END,
} OTA_DOWNLOAD_TASK_STATE;

typedef void (*OTA_COMPLETION_CALLBACK)(uint32_t event, void * data,void *cookie );
typedef struct {

    struct {
        uint8_t context[16]; //256 + 64 + 1024];
        OTA_DOWNLOAD_TASK_STATE state;
    } task;

    OTA_TASK_ID current_task;
    SYS_STATUS status;
    OTA_RESULT ota_result;
    OTA_COMPLETION_CALLBACK callback;
    bool new_downloaded_img;
    bool ota_rollback_initiated;
    bool ota_idle;
    bool ota_start;
} OTA_DATA;

typedef struct {
    uint32_t total_len;
    uint32_t total_received;
    uint32_t readPktlen;
    uint32_t read_index;
    uint32_t write_index;
    bool     file_header;
} OTA_DOWNLOAD_TASK_CONTEXT;

// *****************************************************************************
/* Function:
    void OTA_SERVICE_Tasks(void)

  Summary:
    Maintains the OTA module state machine.

  Description:
    This function maintains the OTA module state machine and manages the
    OTA Module structure items and responds to OTA Module events.

  Precondition:
    None.

  Parameters:
 * None.
 *
  Returns:
    None.

  Example:
    <code>

    while (true)
    {
        OTA_SERVICE_Tasks();
    }
    </code>

  Remarks:
*/
// *****************************************************************************

    void OTA_SERVICE_Tasks(void );

// *****************************************************************************
/*
  Function:
    bool OTA_IsIdle(void);

  Summary:
    To check if OTA Task is in idle state.

  Description:
    To check if OTA Task is in idle state.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    True - Idle
    False - Not Idle
*/
// *****************************************************************************

    bool OTA_IsIdle(void);

    // *****************************************************************************
/*
  Function:
    SYS_STATUS OTA_CallBackReg(OTA_COMPLETION_CALLBACK buffer, uint32_t length);

  Summary:
    Registering callback.

  Description:
    Registering callback.

  Precondition:
    None.

  Parameters:
    buffer - pointer to callback function name.
    length - length of callback function name.

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************

SYS_STATUS OTA_CallBackReg(OTA_COMPLETION_CALLBACK buffer, uint32_t length);
void OTA_SERVICE_OTA_Initialize(void);
SYS_STATUS OTA_SERVICE_OTA_Start(void);
SYS_STATUS OTA_SERVICE_OTA_FirmwareSwitch(void);
SYS_STATUS OTA_SERVICE_OTA_Rollback(void);
void OTA_SERVIC_Task_UpdateUser(void);
SYS_STATUS OTA_SERVICE_Transport_Tasks(OTA_DOWNLOAD_TASK_CONTEXT  *ctx);
uint32_t OTA_SERVICE_Transport_FHMsgReceive(OTA_FILE_HANDLER_CONTEXT *otaFileHandlerCtx);
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* OTA_SERVICE_H */

/* *****************************************************************************
 End of File
 */

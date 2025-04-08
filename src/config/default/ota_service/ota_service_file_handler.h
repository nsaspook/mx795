/*******************************************************************************
  OTA service File Handler Header File

  File Name:
    ota_service_file_handler.h

  Summary:
    This file contains OTA service File Handler definitions and functions.

  Description:
    This file contains OTA service File Handler definitions and functions.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#ifndef OTA_SERVICE_FILE_HANDLER_H
#define OTA_SERVICE_FILE_HANDLER_H

#include "ota_service_control_block.h"
#include "configuration.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

#define FLASH_START                      (0x9d000000UL)
#define FLASH_LENGTH                     (0x80000UL)
#define PAGE_SIZE                        (512UL)
#define ERASE_BLOCK_SIZE                 (4096UL)



#define DATA_SIZE                        PAGE_SIZE

#define BUFFER_SIZE(x, y)                ((y) > (x)? ((((y) % (x)) != 0U)?((((y) / (x)) + 1U) * (x)) : (((y) / (x)) * (x))) : (x))

#define OTA_CONTROL_BLOCK_SIZE           sizeof(OTA_CONTROL_BLOCK)
#define OTA_CONTROL_BLOCK_PAGE_SIZE      PAGE_SIZE

#define OTA_CONTROL_BLOCK_BUFFER_SIZE    BUFFER_SIZE(OTA_CONTROL_BLOCK_PAGE_SIZE, OTA_CONTROL_BLOCK_SIZE)


typedef enum
{
    OTA_SERVICE_FH_STATE_INIT = 0,

    OTA_SERVICE_FH_STATE_MSG_RECV,

    OTA_SERVICE_FH_STATE_ERASE,

    OTA_SERVICE_FH_STATE_ERASE_WAIT,

    OTA_SERVICE_FH_STATE_WRITE,

    OTA_SERVICE_FH_STATE_WRITE_WAIT,

    OTA_SERVICE_FH_STATE_VERIFY,

    OTA_SERVICE_FH_STATE_CB_WRITE,

    OTA_SERVICE_FH_STATE_IDLE,

    OTA_SERVICE_FH_STATE_ERROR

} OTA_SERVICE_FH_STATE;

typedef struct __attribute__((packed))
{
    uint8_t  imageIdentity[11];
    uint8_t  headerVersion;
    uint8_t  signaturePresent;
    uint8_t  status;
    uint8_t  imageStorage;
    uint8_t  imageType;
    uint32_t programAddress;
    uint32_t jumpAddress;
    uint32_t imageSize;
    uint32_t loadAddress;
    uint8_t  versionNumber;
    uint8_t  signature[8];
    uint8_t  signatureType;
} OTA_FILE_HEADER;

typedef struct
{
    OTA_SERVICE_FH_STATE state;

    uint32_t nFlashBytesWritten;

    uint32_t totalBytesWritten;

    uint32_t nFlashBytesFreeSpace;

    uint32_t memoryAddress;

    bool     initData;

    OTA_FILE_HEADER fileHeader;

    OTA_CONTROL_BLOCK *controlBlock;
} OTA_FILE_HANDLER_DATA;

typedef struct
{
    void *buffer;

    uint32_t size;

    OTA_FILE_HEADER *fileHeader;
} OTA_FILE_HANDLER_CONTEXT;

void OTA_SERVICE_FH_TriggerReset(void);
bool OTA_SERVICE_FH_CtrlBlkRead(OTA_CONTROL_BLOCK *controlBlock, uint32_t length);
bool OTA_SERVICE_FH_CtrlBlkWrite(OTA_CONTROL_BLOCK *controlBlock, uint32_t length);
OTA_SERVICE_FH_STATE OTA_SERVICE_FH_StateGet(void);
void OTA_SERVICE_FH_Tasks(void);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif      //OTA_SERVICE_FILE_HANDLER_H

/*******************************************************************************
  OTA service File Handler Source File

  File Name:
    ota_service_file_handler.c

  Summary:
    This file contains source code necessary to handle the image and it's metadata.

  Description:
    This file contains source code necessary to handle the image and it's metadata.
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

#include "ota_service_file_handler.h"
#include "ota_service.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

static uint8_t CACHE_ALIGN controlBlockBuffer[OTA_CONTROL_BLOCK_BUFFER_SIZE];
static uint8_t CACHE_ALIGN flash_data[DATA_SIZE];
static OTA_FILE_HANDLER_CONTEXT otaFileHandlerCtx;
static uint32_t ctrlBlkSize = OTA_CONTROL_BLOCK_BUFFER_SIZE;
static OTA_FILE_HANDLER_DATA otaFileHandlerData =
{
    .state = OTA_SERVICE_FH_STATE_INIT,
    .controlBlock = (OTA_CONTROL_BLOCK *)controlBlockBuffer
};

// *****************************************************************************
// *****************************************************************************
// Section: File Handler Local Functions
// *****************************************************************************
// *****************************************************************************

static uint32_t OTA_SERVICE_FH_CRCGenerate(uint8_t *start_addr, uint32_t size, uint32_t crc)
{
    uint32_t   i, j, value;
    uint32_t   crc_tab[256];
    uint8_t    data;

    for (i = 0U; i < 256U; i++)
    {
        value = i;

        for (j = 0U; j < 8U; j++)
        {
            if ((value & 1U) != 0U)
            {
                value = (value >> 1U) ^ 0xEDB88320U;
            }
            else
            {
                value >>= 1U;
            }
        }
        crc_tab[i] = value;
    }

    for (i = 0U; i < size; i++)
    {
        data = start_addr[i];
        crc = crc_tab[(crc ^ data) & 0xffU] ^ (crc >> 8U);
    }

    return crc;
}

static bool OTA_SERVICE_FH_WaitForXferComplete(void)
{
    bool status = false;

    OTA_MEM_TRANSFER_STATUS transferStatus = OTA_MEM_TRANSFER_ERROR_UNKNOWN;

    do
    {
        transferStatus = (OTA_MEM_TRANSFER_STATUS)RAM_TransferStatusGet(otaFileHandlerData.handle);

    } while (transferStatus == OTA_MEM_TRANSFER_BUSY);

    if(transferStatus == OTA_MEM_TRANSFER_COMPLETED)
    {
        status = true;
    }

    return status;
}

// *****************************************************************************
// *****************************************************************************
// Section: File Handler Global Functions
// *****************************************************************************
// *****************************************************************************

/* Trigger a reset */
void OTA_SERVICE_FH_TriggerReset(void)
{
    /* Perform system unlock sequence */
    SYSKEY = 0x00000000U;
    SYSKEY = 0xAA996655U;
    SYSKEY = 0x556699AAU;

    RSWRSTSET = _RSWRST_SWRST_MASK;
    (void)RSWRST;
}

bool OTA_SERVICE_FH_CtrlBlkRead(OTA_CONTROL_BLOCK *controlBlock, uint32_t length)
{
    uint32_t count;
    uint32_t blockSize;
    uint32_t otaMemoryStart;
    uint32_t otaMemorySize;
    uint32_t appMetaDataAddress;
    bool     status = false;
    uint8_t  *ptrBuffer = (uint8_t *)controlBlock;

    if ((ptrBuffer != NULL) && (length == ctrlBlkSize))
    {
        otaMemoryStart    = otaFileHandlerData.geometry.blockStartAddress;

        otaMemorySize     = (otaFileHandlerData.geometry.read_blockSize * otaFileHandlerData.geometry.read_numBlocks);

        blockSize = otaFileHandlerData.geometry.write_blockSize;

        appMetaDataAddress  = ((otaMemoryStart + otaMemorySize) - BUFFER_SIZE(blockSize, ctrlBlkSize));

        for (count = 0U; count < length; count += OTA_CONTROL_BLOCK_PAGE_SIZE)
        {
            if (RAM_Read(otaFileHandlerData.handle, ptrBuffer, OTA_CONTROL_BLOCK_PAGE_SIZE, appMetaDataAddress) != true)
            {
                status = false;
                break;
            }

            status = OTA_SERVICE_FH_WaitForXferComplete();
            appMetaDataAddress += OTA_CONTROL_BLOCK_PAGE_SIZE;
            ptrBuffer += OTA_CONTROL_BLOCK_PAGE_SIZE;
        }
    }

    return status;
}

bool OTA_SERVICE_FH_CtrlBlkWrite(OTA_CONTROL_BLOCK *controlBlock, uint32_t length)
{
    uint32_t count;
    uint32_t blockSize;
    uint32_t otaMemoryStart;
    uint32_t otaMemorySize;
    uint32_t appMetaDataAddress;
    bool     status = false;
    uint8_t  *ptrBuffer = (uint8_t *)controlBlock;

    if ((ptrBuffer != NULL) && (length == ctrlBlkSize))
    {
        otaMemoryStart    = otaFileHandlerData.geometry.blockStartAddress;

        otaMemorySize     = (otaFileHandlerData.geometry.read_blockSize * otaFileHandlerData.geometry.read_numBlocks);

        blockSize = otaFileHandlerData.geometry.write_blockSize;

        appMetaDataAddress  = ((otaMemoryStart + otaMemorySize) - BUFFER_SIZE(blockSize, ctrlBlkSize));


        for (count = 0U; count < length; count += OTA_CONTROL_BLOCK_PAGE_SIZE)
        {
            if (RAM_PageWrite(otaFileHandlerData.handle, ptrBuffer, appMetaDataAddress) != true)
            {
                status = false;
                break;
            }

            status = OTA_SERVICE_FH_WaitForXferComplete();
            appMetaDataAddress += OTA_CONTROL_BLOCK_PAGE_SIZE;
            ptrBuffer += OTA_CONTROL_BLOCK_PAGE_SIZE;
        }
    }

    return status;
}

OTA_SERVICE_FH_STATE OTA_SERVICE_FH_StateGet(void)
{
    return otaFileHandlerData.state;
}

void OTA_SERVICE_FH_Tasks(void)
{
    /* Check the application's current state. */
    switch (otaFileHandlerData.state)
    {
        case OTA_SERVICE_FH_STATE_INIT:
        {
            otaFileHandlerData.nFlashBytesWritten = 0U;
            otaFileHandlerData.nFlashBytesFreeSpace = DATA_SIZE;
            otaFileHandlerData.totalBytesWritten = 0U;
            otaFileHandlerData.controlBlock->ActiveImageNum = 0U;
            otaFileHandlerCtx.fileHeader = &otaFileHandlerData.fileHeader;
            if (RAM_Status(RAM_INDEX) == SYS_STATUS_READY)
            {
                otaFileHandlerData.handle = RAM_Open(RAM_INDEX, DRV_IO_INTENT_READWRITE);

                if (otaFileHandlerData.handle != DRV_HANDLE_INVALID)
                {
                    if (RAM_GeometryGet(otaFileHandlerData.handle, (void *)&otaFileHandlerData.geometry) != true)
                    {
                        otaFileHandlerData.state = OTA_SERVICE_FH_STATE_ERROR;
                        break;
                    }
                    otaFileHandlerData.state = OTA_SERVICE_FH_STATE_MSG_RECV;
                }
            }
            break;
        }

        case OTA_SERVICE_FH_STATE_MSG_RECV:
        {
            uint32_t receivedBytes;

            otaFileHandlerCtx.buffer = &flash_data[otaFileHandlerData.nFlashBytesWritten];
            otaFileHandlerCtx.size = otaFileHandlerData.nFlashBytesFreeSpace;
            receivedBytes = OTA_SERVICE_Transport_FHMsgReceive(&otaFileHandlerCtx);

            if (receivedBytes != 0U)
            {
                otaFileHandlerData.nFlashBytesWritten += receivedBytes;
                otaFileHandlerData.nFlashBytesFreeSpace -= receivedBytes;

                if ((otaFileHandlerData.nFlashBytesWritten >= DATA_SIZE)
                ||  ((otaFileHandlerData.initData == true) && ((otaFileHandlerData.nFlashBytesWritten + otaFileHandlerData.totalBytesWritten) >= otaFileHandlerData.fileHeader.imageSize)))
                {
                    if (otaFileHandlerData.initData == false)
                    {
                        otaFileHandlerData.initData = true;
                        otaFileHandlerData.memoryAddress = otaFileHandlerData.fileHeader.loadAddress;
                    }

                    otaFileHandlerData.state = OTA_SERVICE_FH_STATE_WRITE;
                }
            }
            break;
        }


        case OTA_SERVICE_FH_STATE_WRITE:
        {
            (void)RAM_PageWrite(otaFileHandlerData.handle, &flash_data[0], otaFileHandlerData.memoryAddress);
            otaFileHandlerData.nFlashBytesWritten = 0U;
            otaFileHandlerData.nFlashBytesFreeSpace = DATA_SIZE;
            otaFileHandlerData.memoryAddress += DATA_SIZE;
            otaFileHandlerData.totalBytesWritten += DATA_SIZE;
            otaFileHandlerData.state = OTA_SERVICE_FH_STATE_WRITE_WAIT;
            break;
        }

        case OTA_SERVICE_FH_STATE_WRITE_WAIT:
        {
            OTA_MEM_TRANSFER_STATUS transferStatus;
            transferStatus = (OTA_MEM_TRANSFER_STATUS)RAM_TransferStatusGet(otaFileHandlerData.handle);
            if (transferStatus == OTA_MEM_TRANSFER_COMPLETED)
            {
                if (otaFileHandlerData.totalBytesWritten >= otaFileHandlerData.fileHeader.imageSize)
                {
                    otaFileHandlerData.state = OTA_SERVICE_FH_STATE_VERIFY;
                }
                else
                {
                    otaFileHandlerData.state = OTA_SERVICE_FH_STATE_MSG_RECV;
                }
            }
            break;
        }

        case OTA_SERVICE_FH_STATE_VERIFY:
        {
            uint32_t crc32;
            uint32_t *received_crc32 = (void *)otaFileHandlerData.fileHeader.signature;
            uint32_t dataSize = DATA_SIZE;
            uint32_t imageSize = 0U;

            otaFileHandlerData.memoryAddress = otaFileHandlerData.fileHeader.loadAddress;
            crc32 = 0xFFFFFFFFU;
            while (imageSize < otaFileHandlerData.fileHeader.imageSize)
            {

                (void)RAM_Read(otaFileHandlerData.handle, &flash_data[0], dataSize, otaFileHandlerData.memoryAddress);

                (void)OTA_SERVICE_FH_WaitForXferComplete();

                crc32 = OTA_SERVICE_FH_CRCGenerate(&flash_data[0], dataSize, crc32);

                otaFileHandlerData.memoryAddress += dataSize;
                imageSize += dataSize;

                if ((otaFileHandlerData.fileHeader.imageSize - imageSize) < dataSize)
                {
                    dataSize = (otaFileHandlerData.fileHeader.imageSize - imageSize);
                }
            }

            if (crc32 == *received_crc32)
            {
                otaFileHandlerData.state = OTA_SERVICE_FH_STATE_CB_WRITE;
            }
            else
            {
                otaFileHandlerData.state = OTA_SERVICE_FH_STATE_IDLE;
            }
            break;
        }

        case OTA_SERVICE_FH_STATE_CB_WRITE:
        {
            (void)OTA_SERVICE_FH_CtrlBlkRead(otaFileHandlerData.controlBlock, ctrlBlkSize);

            otaFileHandlerData.controlBlock->ActiveImageNum++;
            if (otaFileHandlerData.controlBlock->ActiveImageNum >= 2U)
            {
                otaFileHandlerData.controlBlock->ActiveImageNum = 0U;
            }
            otaFileHandlerData.controlBlock->versionNum = 0U;
            otaFileHandlerData.controlBlock->blockUpdated = 1U;
            otaFileHandlerData.controlBlock->appImageInfo[otaFileHandlerData.controlBlock->ActiveImageNum].imageType = otaFileHandlerData.fileHeader.imageType;
            otaFileHandlerData.controlBlock->appImageInfo[otaFileHandlerData.controlBlock->ActiveImageNum].status = otaFileHandlerData.fileHeader.status;
            otaFileHandlerData.controlBlock->appImageInfo[otaFileHandlerData.controlBlock->ActiveImageNum].programAddress = otaFileHandlerData.fileHeader.programAddress;
            otaFileHandlerData.controlBlock->appImageInfo[otaFileHandlerData.controlBlock->ActiveImageNum].jumpAddress = otaFileHandlerData.fileHeader.jumpAddress;
            otaFileHandlerData.controlBlock->appImageInfo[otaFileHandlerData.controlBlock->ActiveImageNum].loadAddress = otaFileHandlerData.fileHeader.loadAddress;
            otaFileHandlerData.controlBlock->appImageInfo[otaFileHandlerData.controlBlock->ActiveImageNum].imageSize = otaFileHandlerData.fileHeader.imageSize;
            otaFileHandlerData.controlBlock->appImageInfo[otaFileHandlerData.controlBlock->ActiveImageNum].signaturePresent = 1U;
            (void)memcpy(otaFileHandlerData.controlBlock->appImageInfo[otaFileHandlerData.controlBlock->ActiveImageNum].signature, otaFileHandlerData.fileHeader.signature, sizeof(otaFileHandlerData.fileHeader.signature));

            if (OTA_SERVICE_FH_CtrlBlkWrite(otaFileHandlerData.controlBlock, ctrlBlkSize) == false)
            {
                otaFileHandlerData.state = OTA_SERVICE_FH_STATE_ERROR;
            }
            else
            {
                otaFileHandlerData.state = OTA_SERVICE_FH_STATE_IDLE;
            }
            break;
        }

        case OTA_SERVICE_FH_STATE_IDLE:
        {
            break;
        }

        case OTA_SERVICE_FH_STATE_ERROR:
        default:
        {
            // Do nothing
            break;
        }
    }
}

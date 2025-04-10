 /*******************************************************************************
  USB CDC class definitions

  Company:
    Microchip Technology Inc.

  File Name:
    usb_host_cdc_local.h

  Summary:
    USB CDC class definitions

  Description:
    This file describes the CDC class specific definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef USB_HOST_CDC_LOCAL_H
#define USB_HOST_CDC_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "usb/usb_host.h"
#include "usb/src/usb_host_local.h"
#include "usb/usb_host_cdc.h"


/* Configures DTE rate, stop-bits, parity, and number-of-character bits.*/
#define USB_CDC_GET_LINE_CODING             0x21
/* Number of bytes Line Coding transfer */
#define USB_CDC_LINE_CODING_LENGTH          0x07
/* Configures DTE rate, stop-bits, parity, and number-of-character bits. */
#define USB_CDC_SET_LINE_CODING             0x20
/* [V24] signal used to tell the DCE device the DTE device is now present. */
#define USB_CDC_SET_CONTROL_LINE_STATE      0x22
 /* Number of bytes Control line transfer */
#define USB_CDC_CONTROL_LINE_LENGTH         0x00
 /* Sends special carrier modulation used to specify [V24] style break. */
#define USB_CDC_SEND_BREAK                  0x23   
/* Reset the CDC device*/
#define USB_CDC_RESET           (0xFF)
#define MARK_RESET_RECOVERY     (0x0E)



/*****************************************
 * CDC Host Client Driver State
 *****************************************/
typedef enum
{
    /* Error state */
    USB_HOST_CDC_STATE_ERROR = -1,

    /* The instance is not ready */
    USB_HOST_CDC_STATE_NOT_READY = 0,

    /* The instance should set the configuration */
    USB_HOST_CDC_STATE_SET_CONFIGURATION,

    /* Wait for configuration set */
    USB_HOST_CDC_STATE_WAIT_FOR_CONFIGURATION_SET,

    /* Wait for interfaces to get ready */
    USB_HOST_CDC_STATE_WAIT_FOR_INTERFACES,

    /* The instance is ready */
    USB_HOST_CDC_STATE_READY,

} USB_HOST_CDC_STATE;

/*******************************************
 * USB Host CDC Control Transfer Object
 *******************************************/
typedef struct
{
    /* True if the object is in use */
    bool inUse;

    /* The CDC Class Specific request type */
    USB_HOST_CDC_EVENT requestType;

} USB_HOST_CDC_CONTROL_TRANSFER_OBJ;

/*************************************************
 * Control transfer event data type. This matches
 * the data types returned to the application 
 * along with the control transfer events.
 *************************************************/
typedef struct
{
    /* Request handle of this request */
    USB_HOST_CDC_REQUEST_HANDLE requestHandle;

    /* Termination status */
    USB_HOST_CDC_RESULT result;

    /* Size of the data transferred in the request */
    size_t length;

} USB_HOST_CDC_CONTROL_REQUEST_EVENT_DATA;

/*******************************************
 * USB Host CDC Attach Listener Objects
 ******************************************/
typedef struct
{
    /* This object is in use */
    bool inUse;

    /* The attach event handler */
    USB_HOST_CDC_ATTACH_EVENT_HANDLER eventHandler;

    /* Client context */
    uintptr_t context;

} USB_HOST_CDC_ATTACH_LISTENER_OBJ;

/*************************************
 * USB Host CDC Client Driver Object
 *************************************/
typedef struct  
{
    /* True if object is in use */
    bool inUse;

    /* Device client handle */
    USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle;

    /* Device object handle */
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle;
    
    /* Data Interface Handle */
    USB_HOST_DEVICE_INTERFACE_HANDLE dataInterfaceHandle;
    
    /* Communication Interface Handle */
    USB_HOST_DEVICE_INTERFACE_HANDLE communicationInterfaceHandle;

    /*Control Pipe Handle */
    USB_HOST_PIPE_HANDLE controlPipeHandle;

    /* Bulk pipe handles */
    USB_HOST_PIPE_HANDLE bulkInPipeHandle;
    USB_HOST_PIPE_HANDLE bulkOutPipeHandle;
    
    /* Interrupt pipe handles */
    USB_HOST_PIPE_HANDLE interruptPipeHandle;

    /* Setup packet information */
    USB_SETUP_PACKET    setupPacket;

    /* Application defined context */
    uintptr_t context;

    /* Application callback */
    USB_HOST_CDC_EVENT_HANDLER eventHandler;

    /* CDC instance state */
    USB_HOST_CDC_STATE state;

    /* True if an ongoing host request is done */
    bool hostRequestDone;

    /* Result of the host request */
    USB_HOST_RESULT hostRequestResult;

    /* Control transfer object */
    USB_HOST_CDC_CONTROL_TRANSFER_OBJ controlTransferObj;

    /* Interface numbers */
    uint8_t commInterfaceNumber;
    uint8_t dataInterfaceNumber;

} USB_HOST_CDC_INSTANCE_OBJ;

extern USB_HOST_CDC_INSTANCE_OBJ gUSBHostCDCObj[USB_HOST_CDC_INSTANCES_NUMBER];

// *****************************************************************************
/* Function:
    void F_USB_HOST_CDC_Initialize(void * msdInitData)

  Summary:
    This function is called when the Host Layer is initializing.

  Description:
    This function is called when the Host Layer is initializing.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void F_USB_HOST_CDC_Initialize(void * data);

// *****************************************************************************
/* Function:
    void F_USB_HOST_CDC_Deinitialize(void)

  Summary:
    This function is called when the Host Layer is deinitializing.

  Description:
    This function is called when the Host Layer is deinitializing.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void F_USB_HOST_CDC_Deinitialize(void);

// *****************************************************************************
/* Function:
    void F_USB_HOST_CDC_Reinitialize(void)

  Summary:
    This function is called when the Host Layer is reinitializing.

  Description:
    This function is called when the Host Layer is reinitializing.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void F_USB_HOST_CDC_Reinitialize(void * msdInitData);

// *****************************************************************************
/* Function:
    void F_USB_HOST_CDC_DeviceAssign 
    (
        USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
        USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
        USB_DEVICE_DESCRIPTOR * deviceDescriptor
    )
 
  Summary: 
    This function is called when the host layer finds device level class
    subclass protocol match.

  Description:
    This function is called when the host layer finds device level class
    subclass protocol match.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/
void F_USB_HOST_CDC_DeviceAssign 
(
    USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
    USB_DEVICE_DESCRIPTOR * deviceDescriptor
);

// *****************************************************************************
/* Function:
    void F_USB_HOST_CDC_DeviceRelease 
    (
        USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle
    )
 
  Summary: 
    This function is called when the device is detached. 

  Description:
    This function is called when the device is detached. 

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void F_USB_HOST_CDC_DeviceRelease 
(
    USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle
);

// *****************************************************************************
/* Function:
    void F_USB_HOST_CDC_DeviceTasks 
    (
        USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle
    )
 
  Summary: 
    This function is called when the host layer want the device to update its
    state. 

  Description:
    This function is called when the host layer want the device to update its
    state. 

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void F_USB_HOST_CDC_DeviceTasks
(
    USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle
);

// *****************************************************************************
/* Function:
    void F_USB_HOST_CDC_InterfaceAssign 
    (
        USB_HOST_DEVICE_INTERFACE_HANDLE * interfaces,
        USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
        size_t nInterfaces,
        uint8_t * descriptor
    )

  Summary:
    This function is called when the Host Layer attaches this driver to an
    interface.

  Description:
    This function is called when the Host Layer attaches this driver to an
    interface.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void F_USB_HOST_CDC_InterfaceAssign 
(
    USB_HOST_DEVICE_INTERFACE_HANDLE * interfaces,
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
    size_t nInterfaces,
    uint8_t * descriptor
);

// *****************************************************************************
/* Function:
    USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE F_USB_HOST_CDC_InterfaceEventHandler
    (
        USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
        USB_HOST_DEVICE_INTERFACE_EVENT event,
        void * eventData,
        uintptr_t context
    )

  Summary:
    This function is called when the Host Layer generates interface level
    events. 

  Description:
    This function is called when the Host Layer generates interface level
    events. 

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE F_USB_HOST_CDC_InterfaceEventHandler
(
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
    USB_HOST_DEVICE_INTERFACE_EVENT event,
    void * eventData,
    uintptr_t context
);

// *****************************************************************************
/* Function:
    void USB_HOST_CDC_InterfaceTasks
    (
        USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle
    )

  Summary:
    This function is called by the Host Layer to update the state of this
    driver.

  Description:
    This function is called by the Host Layer to update the state of this
    driver.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void F_USB_HOST_CDC_InterfaceTasks
(
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle
);

// *****************************************************************************
/* Function:
    void USB_HOST_CDC_InterfaceRelease
    (
        USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle
    )

  Summary:
    This function is called when the Host Layer detaches this driver from an
    interface.

  Description:
    This function is called when the Host Layer detaches this driver from an
    interface.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void F_USB_HOST_CDC_InterfaceRelease
(
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle
);

// *****************************************************************************
/* Function:
    USB_HOST_DEVICE_EVENT_RESPONSE F_USB_HOST_CDC_DeviceEventHandler
    (
        USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
        USB_HOST_DEVICE_EVENT event,
        void * eventData,
        uintptr_t context
    )

  Summary:
    This function is called when the Host Layer generates device level
    events. 

  Description:
    This function is called when the Host Layer generates device level
    events. 

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

USB_HOST_DEVICE_EVENT_RESPONSE F_USB_HOST_CDC_DeviceEventHandler
(
    USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
    USB_HOST_DEVICE_EVENT event,
    void * eventData,
    uintptr_t context
);

// *****************************************************************************
/* Function:
    int F_USB_HOST_CDC_DeviceHandleToInstance
    (
        USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle
    );

  Summary:
    This function will return the index of the CDC object that own this device.

  Description:
    This function will return the index of the CDC object that own this device.
    If an instance is not found, the function will return -1.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

int F_USB_HOST_CDC_DeviceHandleToInstance
(
    USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle
);

// *****************************************************************************
/* Function:
    int F_USB_HOST_CDC_InterfaceHandleToInstance
    (
        USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle
    );

  Summary:
    This function will return the index of the CDC object that own this
    interface.

  Description:
    This function will return the index of the CDC object that owns this
    interface.  If an instance is not found, the function will return -1.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

int F_USB_HOST_CDC_InterfaceHandleToInstance
(
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle
);

// *****************************************************************************
/* Function:
    int F_USB_HOST_CDC_DeviceObjHandleToInstance
    (
        USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle
    );

  Summary:
    This function will return the index of the CDC object that is associated
    with this device.

  Description:
    This function will return the index of the CDC object that is associated
    with this device.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

int F_USB_HOST_CDC_DeviceObjHandleToInstance
(
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle
);

// *****************************************************************************
/* Function:  
    USB_HOST_CDC_RESULT F_USB_HOST_CDC_HostResutlToCDCResultMap
    (
        USB_HOST_RESULT result
    )

  Summary: 
    This function will map the USB Host result to CDC Result.

  Description:
    This function will map the USB Host result to CDC Result.

  Remarks:
    This is a local function and should not be called by directly by the
    application.
*/

USB_HOST_CDC_RESULT F_USB_HOST_CDC_HostResutlToCDCResultMap
(
    USB_HOST_RESULT result
);

/* Function:
   void F_USB_HOST_CDC_ControlTransferCallback
    (
        USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
        USB_HOST_REQUEST_HANDLE requestHandle,
        USB_HOST_RESULT result,
        size_t size,
        uintptr_t context
    );

  Summary:
    This function is called when a control transfer completes.

  Description:
    This function is called when a control transfer completes.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/

void F_USB_HOST_CDC_ControlTransferCallback
(
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
    USB_HOST_REQUEST_HANDLE requestHandle,
    USB_HOST_RESULT result,
    size_t size,
    uintptr_t context
);

USB_HOST_CDC_RESULT USB_HOST_CDC_ACM_LineCodingGet
(
    USB_HOST_CDC_HANDLE handle, 
    USB_HOST_CDC_REQUEST_HANDLE * requestHandle, 
    USB_CDC_LINE_CODING * lineCoding
);

USB_HOST_CDC_RESULT USB_HOST_CDC_ACM_BreakSend
(
    USB_HOST_CDC_HANDLE handle,
    USB_HOST_CDC_REQUEST_HANDLE * requestHandle,
    uint16_t breakDuration
);

#endif

 /************ End of file *************************************/

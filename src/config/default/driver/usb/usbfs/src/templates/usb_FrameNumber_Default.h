/*******************************************************************************
  USB Peripheral Library Template Implementation

  File Name:
    usb_FrameNumber_Default.h

  Summary:
    USB PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : FrameNumber
    and its Variant : Default
    For following APIs :
        PLIB_USB_FrameNumberGet
        PLIB_USB_ExistsFrameNumber

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

//DOM-IGNORE-END

#ifndef USB_FRAMENUMBER_DEFAULT_H
#define USB_FRAMENUMBER_DEFAULT_H

#include "driver/usb/usbfs/src/templates/usbfs_registers.h"

/* MISRA C-2012 Rule 10.6 and Rule 12.2.
   Deviation record ID - H3_USB_MISRAC_2012_R_10_6_DR_1 and H3_USB_MISRAC_2012_R_12_2_DR_1 */
//******************************************************************************
/* Function :  USB_FrameNumberGet_Default

  Summary:
    Implements Default variant of PLIB_USB_FrameNumberGet 

  Description:
    This template implements the Default variant of the PLIB_USB_FrameNumberGet
    function.
*/

PLIB_TEMPLATE uint16_t USB_FrameNumberGet_Default( USB_MODULE_ID index )
{
    uint16_t retval;
    volatile usb_registers_t   * usb = ((usb_registers_t *)(index));
    retval =  (( usb->UxFRML.FRML ) |   ( usb->UxFRMH.FRMH << 8 ) );
    return retval;
}

//******************************************************************************
/* Function :  USB_ExistsFrameNumber_Default

  Summary:
    Implements Default variant of PLIB_USB_ExistsFrameNumber

  Description:
    This template implements the Default variant of the
    PLIB_USB_ExistsFrameNumber function.
*/

#define PLIB_USB_ExistsFrameNumber PLIB_USB_ExistsFrameNumber
PLIB_TEMPLATE bool USB_ExistsFrameNumber_Default( USB_MODULE_ID index )
{
    return true;
}

/* MISRAC 2012 deviation block end */

#endif /*USB_FRAMENUMBER_DEFAULT_H*/

/******************************************************************************
 End of File
*/


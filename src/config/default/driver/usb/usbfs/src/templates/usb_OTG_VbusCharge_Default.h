/*******************************************************************************
  USB Peripheral Library Template Implementation

  File Name:
    usb_OTG_VbusCharge_Default.h

  Summary:
    USB PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : OTG_VbusCharge
    and its Variant : Default
    For following APIs :
        PLIB_USB_OTG_VBusChargeEnable
        PLIB_USB_OTG_VBusChargeDisable
        PLIB_USB_ExistsOTG_VbusCharge

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

#ifndef USB_OTG_VBUSCHARGE_DEFAULT_H
#define USB_OTG_VBUSCHARGE_DEFAULT_H

#include "driver/usb/usbfs/src/templates/usbfs_registers.h"


//******************************************************************************
/* Function :  USB_OTG_VBusChargeEnable_Default

  Summary:
    Implements Default variant of PLIB_USB_OTG_VBusChargeEnable 

  Description:
    This template implements the Default variant of the PLIB_USB_OTG_VBusChargeEnable function.
*/

PLIB_TEMPLATE void USB_OTG_VBusChargeEnable_Default( USB_MODULE_ID index )
{
    volatile usb_registers_t   * usb = ((usb_registers_t *)(index));
    usb->UxOTGCONSET = UxOTGCON_VBUSCHG_MASK ;
   
}

//******************************************************************************
/* Function :  USB_OTG_VBusChargeDisable_Default

  Summary:
    Implements Default variant of PLIB_USB_OTG_VBusChargeDisable 

  Description:
    This template implements the Default variant of the PLIB_USB_OTG_VBusChargeDisable function.
*/

PLIB_TEMPLATE void USB_OTG_VBusChargeDisable_Default( USB_MODULE_ID index )
{
    volatile usb_registers_t   * usb = ((usb_registers_t *)(index));
    usb->UxOTGCONCLR = UxOTGCON_VBUSCHG_MASK ;
}

//******************************************************************************
/* Function :  USB_ExistsOTG_VbusCharge_Default

  Summary:
    Implements Default variant of PLIB_USB_ExistsOTG_VbusCharge

  Description:
    This template implements the Default variant of the PLIB_USB_ExistsOTG_VbusCharge function.
*/

#define PLIB_USB_ExistsOTG_VbusCharge PLIB_USB_ExistsOTG_VbusCharge
PLIB_TEMPLATE bool USB_ExistsOTG_VbusCharge_Default( USB_MODULE_ID index )
{
    return true;
}


#endif /*USB_OTG_VBUSCHARGE_DEFAULT_H*/

/******************************************************************************
 End of File
*/


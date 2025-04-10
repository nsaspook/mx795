/*******************************************************************************
  USB Host Hub Driver interface names mapping

  Company:
    Microchip Technology Inc.

  File Name:
    usb_host_hub_mapping.h

  Summary:
    USB Device Layer Interface names mapping

  Description:
    This file contain mapppings required for the hub driver.
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
//DOM-IGNORE-END

#ifndef USB_HOST_HUB_MAPPING_H
#define USB_HOST_HUB_MAPPING_H

#include "usb/src/usb_external_dependencies.h"

#ifdef USB_HOST_HUB_SUPPORT
#if (USB_HOST_HUB_SUPPORT == true)
    #include "usb/usb_host_hub_interface.h"
    extern USB_HUB_INTERFACE externalHubInterface;
    #define USB_HOST_HUB_INTERFACE_EXTRNL &externalHubInterface;
#else
    #define USB_HOST_HUB_INTERFACE_EXTRNL (NULL)
#endif
#else
    #define USB_HOST_HUB_INTERFACE_EXTRNL (NULL)
#endif



#endif
